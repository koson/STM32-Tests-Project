/**
  ******************************************************************************
  * @file    stm324xg_eval_ioe.c
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    19-September-2013
  * @brief   This file provides a set of functions needed to manage the STMPE811
  *          IO Expander devices mounted on STM324xG-EVAL evaluation board.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* File Info : ---------------------------------------------------------------
  
    Note:
    -----
    - This driver uses the DMA method for sending and receiving data on I2C bus
      which allow higher efficiency and reliability of the communication.  
  
    SUPPORTED FEATURES:
      - IO Read/write : Set/Reset and Read (Polling/Interrupt)
      - Joystick: config and Read (Polling/Interrupt)
      - Touch Screen Features: Single point mode (Polling/Interrupt)
      - TempSensor Feature: accuracy not determined (Polling).

    UNSUPPORTED FEATURES:
      - Row ADC Feature is not supported (not implemented on STM324xG_EVAL board)
  ----------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm324xg_eval_ioe.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */

/** @addtogroup STM324xG_EVAL
  * @{
  */

/** @defgroup STM324xG_EVAL_IOE 
  * @brief  This file includes the IO Expander driver for STMPE811 IO Expander 
  *         devices.
  * @{
  */

/** @defgroup STM324xG_EVAL_IOE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup STM324xG_EVAL_IOE_Private_Defines
  * @{
  */
#define TIMEOUT_MAX 0x3000 /*<! The value of the maximal timeout for I2C waiting loops */
/**
  * @}
  */

/** @defgroup STM324xG_EVAL_IOE_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup STM324xG_EVAL_IOE_Private_Variables
  * @{
  */
TS_STATE TS_State; /* The global structure holding the TS state */

uint32_t IOE_TimeOut = TIMEOUT_MAX; /* Value of Timeout when I2C communication fails */
/**
  * @}
  */

/** @defgroup STM324xG_EVAL_IOE_Private_FunctionPrototypes
  * @{
  */
static uint16_t IOE_TS_Read_X(void);
static uint16_t IOE_TS_Read_Y(void);
static uint16_t IOE_TS_Read_Z(void);

static void IOE_GPIO_Config(void);
static void IOE_I2C_Config(void);
static void IOE_DMA_Config(IOE_DMADirection_TypeDef Direction, uint8_t* buffer);
static void IOE_EXTI_Config(void);

#ifndef USE_Delay
static void delay(__IO uint32_t nCount);
#endif /* USE_Delay*/
/**
  * @}
  */

/** @defgroup STM324xG_EVAL_IOE_Private_Functions
  * @{
  */

/**
  * @brief  Initializes and Configures the two IO_Expanders Functionalities 
  *         (IOs, Touch Screen ..) and configures all STM324xG_EVAL necessary
  *         hardware (GPIOs, APB clocks ..).
  * @param  None
  * @retval IOE_OK if all initializations done correctly. Other value if error.
  */
uint8_t IOE_Config(void)
{
    /* Configure the needed pins */
    IOE_GPIO_Config();

    IOE_I2C_Config();

    /* Read IO Expander 1 ID  */
    if(IOE_IsOperational(IOE_1_ADDR)) {
        return IOE1_NOT_OPERATIONAL;
    }
    if(IOE_IsOperational(IOE_2_ADDR)) {
        return IOE2_NOT_OPERATIONAL;
    }

    /* Generate IOExpander Software reset */
    IOE_Reset(IOE_1_ADDR);
    IOE_Reset(IOE_2_ADDR);

    /* ---------------------- IO Expander 1 configuration --------------------- */
    /* Enable the GPIO, Touch Screen, MEMS and ADC functionalities */
    IOE_FnctCmd(IOE_1_ADDR, IOE_IO_FCT | IOE_ADC_FCT, ENABLE);

    /* Configure the VBAT pin in output mode pin*/
    IOE_IOPinConfig(IOE_1_ADDR, VBAT_DIV_PIN, Direction_OUT);
    /* ENABLE the alternate function for IN1 pin */
    IOE_IOAFConfig(IOE_1_ADDR, VBAT_DIV_PIN, ENABLE);
    /* Apply the default state for the out pins */
    IOE_WriteIOPin(VBAT_DIV_PIN, BitReset);

    /* Touch Screen controller configuration */
    IOE_TS_Config();

    /* Configure the MEMS interrupt pins in Input mode */
    IOE_IOPinConfig(IOE_1_ADDR, (uint32_t)(MEMS_INT1_PIN | MEMS_INT2_PIN), Direction_IN);
    /* ENABLE the alternate function for the MEMS interrupt pins */
    IOE_IOAFConfig(IOE_1_ADDR, (uint32_t)IOE_INMEMS_IT, ENABLE);
    /* Configure the IOs to detect Falling and Rising Edges */
    IOE_IOEdgeConfig(IOE_1_ADDR, (uint32_t)IOE_INMEMS_IT, (uint32_t)(EDGE_FALLING | EDGE_RISING));

    /* ---------------------- IO Expander 2 configuration --------------------- */
    /* Enable the GPIO, Temperature Sensor and ADC functionalities */
    IOE_FnctCmd(IOE_2_ADDR, IOE_IO_FCT | IOE_ADC_FCT, ENABLE);

    /* Configure the Audio Codec Reset pin in output mode pin*/
    IOE_IOPinConfig(IOE_2_ADDR, (uint32_t)(AUDIO_RESET_PIN), Direction_OUT);
    IOE_IOPinConfig(IOE_2_ADDR, (uint32_t)(MII_INT_PIN), Direction_IN);

    /* ENABLE the alternate function for IN1 pin */
    IOE_IOAFConfig(IOE_2_ADDR, (uint32_t)(AUDIO_RESET_PIN | MII_INT_PIN), ENABLE);

    /* Apply the default state for the out pins */
    IOE_WriteIOPin(AUDIO_RESET_PIN, BitReset);
    IOE_WriteIOPin(MII_INT_PIN, BitReset);

    /* Configure the Joystick pins in Input mode */
    IOE_IOPinConfig(IOE_2_ADDR, JOY_IO_PINS, Direction_IN);
    /* ENABLE the alternate function for the Joystick pins */
    IOE_IOAFConfig(IOE_2_ADDR, JOY_IO_PINS, ENABLE);
    /* Configure the IOs to detect Falling and Rising Edges */
    IOE_IOEdgeConfig(IOE_2_ADDR, JOY_IO_PINS, (uint8_t)(EDGE_FALLING | EDGE_RISING));

    /* Temperature Sensor module configuration */
    IOE_TempSens_Config();

    /* Configuration is OK */
    return IOE_OK;
}

/**
  * @brief  Configures The selected interrupts on the IO Expanders.
  * @param  IOE_ITSRC_Source: the source of the interrupts. Could be one or a 
  *         combination of the following parameters:
  *   @arg  IOE_ITSRC_JOYSTICK: Joystick IO intputs.
  *   @arg  IOE_ITSRC_TSC: Touch Screen interrupts.
  *   @arg  IOE_ITSRC_INMEMS: MEMS interrupt lines.
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_ITConfig(uint32_t IOE_ITSRC_Source)
{
    /* Configure the Interrupt output pin to generate low level (INT_CTRL) */
    IOE_ITOutConfig(Polarity_Low, Type_Level);

    /* Manage the Joystick Interrupts */
    if(IOE_ITSRC_Source & IOE_ITSRC_JOYSTICK) {
        /* Enable the Global interrupt */
        IOE_GITCmd(IOE_2_ADDR, ENABLE);

        /* Enable the Joystick pins to generate interrupt (GPIO_INT_EN) */
        IOE_IOITConfig(IOE_2_ADDR, IOE_JOY_IT, ENABLE);

        /* Enable the Global GPIO Interrupt */
        IOE_GITConfig(IOE_2_ADDR, IOE_GIT_GPIO, ENABLE);

        /* Read the GPIO_IT_STA to clear all pending bits if any */
        I2C_ReadDeviceRegister(IOE_2_ADDR, IOE_REG_GPIO_INT_STA);

        /* Enable the Joystick pins to generate interrupt */
        IOE_IOITConfig(IOE_2_ADDR, IOE_JOY_IT, ENABLE);

        /* Read the GPIO_IT_STA to clear all pending bits if any */
        I2C_ReadDeviceRegister(IOE_2_ADDR, IOE_REG_GPIO_INT_STA);
    }

    /* Manage the MEMS Interrupts lines  */
    if(IOE_ITSRC_Source & IOE_ITSRC_INMEMS) {
        /* Enable the Global interrupt */
        IOE_GITCmd(IOE_1_ADDR, ENABLE);

        /* Enable the pins to generate interrupt (GPIO_INT_EN) */
        IOE_IOITConfig(IOE_1_ADDR, IOE_INMEMS_IT, ENABLE);

        /* Enable the Global GPIO Interrupt */
        IOE_GITConfig(IOE_1_ADDR, IOE_GIT_GPIO, ENABLE);

        /* Read the GPIO_IT_STA to clear all pending bits if any */
        I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_GPIO_INT_STA);

        /* Enable the pins to generate interrupt */
        IOE_IOITConfig(IOE_1_ADDR, IOE_INMEMS_IT, ENABLE);

        /* Read the GPIO_IT_STA to clear all pending bits if any */
        I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_GPIO_INT_STA);
    }

    /* Manage the Touch Screen Interrupts */
    if(IOE_ITSRC_Source & IOE_ITSRC_TSC) {
        /* Enable the Global interrupt */
        IOE_GITCmd(IOE_1_ADDR, ENABLE);

        /* Enable the Global GPIO Interrupt */
        IOE_GITConfig(IOE_1_ADDR, (uint8_t)(IOE_GIT_TOUCH | IOE_GIT_FTH | IOE_GIT_FOV), ENABLE);

        /* Read the GPIO_IT_STA to clear all pending bits if any */
        I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_GPIO_INT_STA);
    }

    /* Configure the Interrupt line as EXTI source */
    IOE_EXTI_Config();

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Writes a bit value to an output IO pin.
  * @param IO_Pin: The output pin to be set or reset. This parameter can be one 
  *        of the following values:
  *   @arg  AUDIO_RESET_PIN: Audio codec reset pin
  *   @arg  MII_INT_PIN: Ethernet Phy MII interrupt pin
  *   @arg  VBAT_DIV_PIN: Battery divider pin
  * @param BitVal: The value to be set. This parameter can be one of the
  *        following values: BitSet or BitReset. See IOE_BitVal_TypeDef.
  * @retval IOE_OK or PARAM_ERROR
  */
uint8_t IOE_WriteIOPin(uint8_t IO_Pin, IOE_BitValue_TypeDef BitVal)
{
    uint8_t DeviceAddr = 0;

    /* Get the IO expander Address according to which pin is to be controlled */
    if(IO_Pin & IO1_OUT_ALL_PINS) {
        DeviceAddr = IOE_1_ADDR;
    }
    else if(IO_Pin & IO2_OUT_ALL_PINS) {
        DeviceAddr = IOE_2_ADDR;
    }
    else {
        return PARAM_ERROR;
    }

    /* Apply the bit value to the selected pin */
    if(BitVal == BitReset) {
        /* Set the register */
        I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_CLR_PIN, IO_Pin);
    }
    else {
        /* Set the register */
        I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_SET_PIN, IO_Pin);
    }

    return IOE_OK;
}

/**
  * @brief  Returns the status of the selected input IO pin.
  * @param IO_Pin: The input pin to be read. This parameter can be one 
  *  of the following values:
  *   @arg  MEMS_INT1_PIN: MEMS interrupt line 1.
  *   @arg  MEMS_INT2_PIN: MEMS interrupt line 2.
  *   @arg  JOY_IO_PINS: Joystick IO pins (use IOE_JoyStickGetState for these pins)  
  * @retval None
  */
uint8_t IOE_ReadIOPin(uint32_t IO_Pin)
{
    uint8_t DeviceAddr = 0;
    uint8_t tmp = 0;
    if(IO_Pin & IO1_IN_ALL_PINS) {
        DeviceAddr = IOE_1_ADDR;
    }
    else if(IO_Pin & IO2_IN_ALL_PINS) {
        DeviceAddr = IOE_2_ADDR;
    }
    else {
        return PARAM_ERROR;
    }

    /* Get all the Pins status */
    tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_MP_STA);
    if((tmp & (uint8_t)IO_Pin) != 0) {
        return BitSet;
    }
    else {
        return BitReset;
    }
}

/**
  * @brief  Returns the current Joystick status.
  * @param  None
  * @retval The code of the Joystick key pressed: 
  *   @arg  JOY_NONE
  *   @arg  JOY_SEL
  *   @arg  JOY_DOWN
  *   @arg  JOY_LEFT
  *   @arg  JOY_RIGHT
  *   @arg  JOY_UP
  */
JOYState_TypeDef IOE_JoyStickGetState(void)
{
    uint8_t tmp = 0;
    /* Read the status of all pins */
    tmp = (uint32_t)I2C_ReadDeviceRegister(IOE_2_ADDR, IOE_REG_GPIO_MP_STA);

    /* Check the pressed keys */
    if((tmp & JOY_IO_NONE) == JOY_IO_NONE) {
        return (JOYState_TypeDef)JOY_NONE;
    }
    else if(!(tmp & JOY_IO_SEL)) {
        return (JOYState_TypeDef)JOY_SEL;
    }
    else if(!(tmp & JOY_IO_DOWN)) {
        return (JOYState_TypeDef)JOY_DOWN;
    }
    else if(!(tmp & JOY_IO_LEFT)) {
        return (JOYState_TypeDef)JOY_LEFT;
    }
    else if(!(tmp & JOY_IO_RIGHT)) {
        return (JOYState_TypeDef)JOY_RIGHT;
    }
    else if(!(tmp & JOY_IO_UP)) {
        return (JOYState_TypeDef)JOY_UP;
    }
    else {
        return (JOYState_TypeDef)JOY_NONE;
    }
}

/**
  * @brief  Returns Status and positions of the Touch screen.
  * @param  None
  * @retval Pointer to TS_STATE structure holding Touch Screen information.
  */
TS_STATE* IOE_TS_GetState(void)
{
    uint32_t xDiff, yDiff, x, y;
    static uint32_t _x = 0, _y = 0;

    /* Check if the Touch detect event happened */
    TS_State.TouchDetected = (I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_CTRL) & 0x80);
    if(TS_State.TouchDetected) {
        x = IOE_TS_Read_X();
        y = IOE_TS_Read_Y();
        xDiff = x > _x ? (x - _x) : (_x - x);
        yDiff = y > _y ? (y - _y) : (_y - y);
        if(xDiff + yDiff > 5) {
            _x = x;
            _y = y;
        }
    }
    /* Update the X position */
    TS_State.X = _x;

    /* Update the Y position */
    TS_State.Y = _y;
    /* Update the Z Pression index */
    TS_State.Z = IOE_TS_Read_Z();

    /* Clear the interrupt pending bit and enable the FIFO again */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x01);
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x00);

    /* Return pointer to the updated structure */
    return &TS_State;
}

/**
  * @brief  Returns the temperature row value (in 16 bit format).
  * @param  None
  * @retval The temperature row value.
  */
uint32_t IOE_TempSens_GetData(void)
{
    static __IO uint32_t tmp = 0;

    /* Acquire data enable */
    I2C_WriteDeviceRegister(IOE_2_ADDR, IOE_REG_TEMP_CTRL, 0x03);

    /* Enable the TEMPSENS module */
    tmp = (uint32_t)((I2C_ReadDeviceRegister(IOE_2_ADDR, IOE_REG_TEMP_DATA) & 0x03) << 8);
    tmp |= (uint32_t)I2C_ReadDeviceRegister(IOE_2_ADDR, IOE_REG_TEMP_DATA + 1);

    tmp = (uint32_t)((33 * tmp * 100) / 751);
    tmp = (uint32_t)((tmp + 5) / 10);

    /* return the temperature row value */
    return tmp;
}

/**
  * @brief  Checks the selected Global interrupt source pending bit
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  Global_IT: the Global interrupt source to be checked, could be:
  *   @arg  Global_IT_GPIO : All IOs interrupt
  *   @arg  Global_IT_ADC : ADC interrupt
  *   @arg  Global_IT_TEMP : Temperature Sensor interrupts      
  *   @arg  Global_IT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  Global_IT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  Global_IT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  Global_IT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  Global_IT_TOUCH : Touch Screen Controller Touch Detected interrupt      
  * @retval Status of the checked flag. Could be SET or RESET.
  */
FlagStatus IOE_GetGITStatus(uint8_t DeviceAddr, uint8_t Global_IT)
{
    __IO uint8_t tmp = 0;

    /* get the Interrupt status */
    tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_INT_STA);

    if((tmp & (uint8_t)Global_IT) != 0) {
        return SET;
    }
    else {
        return RESET;
    }
}

/**
  * @brief  Clears the selected Global interrupt pending bit(s)
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  Global_IT: the Global interrupt to be cleared, could be any combination
  *         of the following values:   
  *   @arg  Global_IT_GPIO : All IOs interrupt
  *   @arg  Global_IT_ADC : ADC interrupt
  *   @arg  Global_IT_TEMP : Temperature Sensor interrupts      
  *   @arg  Global_IT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  Global_IT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  Global_IT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  Global_IT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  Global_IT_TOUCH : Touch Screen Controller Touch Detected interrupt 
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_ClearGITPending(uint8_t DeviceAddr, uint8_t Global_IT)
{
    /* Write 1 to the bits that have to be cleared */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_INT_STA, Global_IT);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Checks the status of the selected IO interrupt pending bit
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_IT: the IO interrupt to be checked could be IO_ITx Where x can be 
  *         from 0 to 7.             
  * @retval Status of the checked flag. Could be SET or RESET.
  */
FlagStatus IOE_GetIOITStatus(uint8_t DeviceAddr, uint8_t IO_IT)
{
    uint8_t tmp = 0;

    /* get the Interrupt status */
    tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_INT_STA);

    if((tmp & (uint8_t)IO_IT) != 0) {
        return SET;
    }
    else {
        return RESET;
    }
}

/**
  * @brief  Clears the selected IO interrupt pending bit(s).
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_IT: the IO interrupt to be checked could be IO_ITx Where x can be 
  *         from 0 to 7.              
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_ClearIOITPending(uint8_t DeviceAddr, uint8_t IO_IT)
{
    /* Write 1 to the bits that have to be cleared */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_INT_STA, IO_IT);

    /* Clear the Edge detection pending bit*/
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_ED, IO_IT);

    /* Clear the Rising edge pending bit */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_RE, IO_IT);

    /* Clear the Falling edge pending bit */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_FE, IO_IT);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Checks if the selected device is correctly configured and 
  *         communicates correctly ont the I2C bus.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @retval IOE_OK if IOE is operational. Other value if failure.
  */
uint8_t IOE_IsOperational(uint8_t DeviceAddr)
{
    /* Return Error if the ID is not correct */
    if(IOE_ReadID(DeviceAddr) != (uint16_t)STMPE811_ID) {
        /* Check if a Timeout occurred */
        if(IOE_TimeOut == 0) {
            return (IOE_TimeoutUserCallback());
        }
        else {
            return IOE_FAILURE; /* ID is not Correct */
        }
    }
    else {
        return IOE_OK; /* ID is correct */
    }
}

/**
  * @brief  Resets the IO Expander by Software (SYS_CTRL1, RESET bit).
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_Reset(uint8_t DeviceAddr)
{
    /* Power Down the IO_Expander */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_SYS_CTRL1, 0x02);

    /* wait for a delay to insure registers erasing */
    _delay_(2);

    /* Power On the Codec after the power off => all registers are reinitialized*/
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_SYS_CTRL1, 0x00);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Reads the selected device's ID.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @retval The Device ID (two bytes).
  */
uint16_t IOE_ReadID(uint8_t DeviceAddr)
{
    uint16_t tmp = 0;

    /* Read device ID  */
    tmp = I2C_ReadDeviceRegister(DeviceAddr, 0);
    tmp = (uint32_t)(tmp << 8);
    tmp |= (uint32_t)I2C_ReadDeviceRegister(DeviceAddr, 1);

    /* Return the ID */
    return (uint16_t)tmp;
}

/**
  * @brief  Configures the selected IO Expander functionalities.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IOE_TEMPSENS_FCT: the functions to be configured. could be any 
  *         combination of the following values:
  *   @arg  IOE_IO_FCT : IO function
  *   @arg  IOE_TS_FCT : Touch Screen function
  *   @arg  IOE_ADC_FCT : ADC function
  *   @arg  IOE_TEMPSENS_FCT : Temperature Sensor function
  * @param  NewState: can be ENABLE pr DISABLE   
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_FnctCmd(uint8_t DeviceAddr, uint8_t Fct, FunctionalState NewState)
{
    uint8_t tmp = 0;

    /* Get the register value */
    tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_SYS_CTRL2);

    if(NewState != DISABLE) {
        /* Set the Functionalities to be Enabled */
        tmp &= ~(uint8_t)Fct;
    }
    else {
        /* Set the Functionalities to be Disabled */
        tmp |= (uint8_t)Fct;
    }

    /* Set the register value */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_SYS_CTRL2, tmp);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Configures the selected pin direction (to be an input or an output)
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_Pin: IO_Pin_x: Where x can be from 0 to 7.   
  * @param  Direction: could be Direction_IN or Direction_OUT.      
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_IOPinConfig(uint8_t DeviceAddr, uint8_t IO_Pin, uint8_t Direction)
{
    uint8_t tmp = 0;

    /* Get all the Pins direction */
    tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_DIR);

    if(Direction != Direction_IN) {
        tmp |= (uint8_t)IO_Pin;
    }
    else {
        tmp &= ~(uint8_t)IO_Pin;
    }

    /* Write the register new value */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_DIR, tmp);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Enables or disables the Global interrupt.
  * @param  DeviceAddr: The address of the IOExpander, could be :I OE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  NewState: could be ENABLE or DISABLE.        
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_GITCmd(uint8_t DeviceAddr, FunctionalState NewState)
{
    uint8_t tmp = 0;

    /* Read the Interrupt Control register  */
    tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_INT_CTRL);

    if(NewState != DISABLE) {
        /* Set the global interrupts to be Enabled */
        tmp |= (uint8_t)IOE_GIT_EN;
    }
    else {
        /* Set the global interrupts to be Disabled */
        tmp &= ~(uint8_t)IOE_GIT_EN;
    }

    /* Write Back the Interrupt Control register */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_INT_CTRL, tmp);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Configures the selected source to generate or not a global interrupt
  * @param DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *        or IOE_2_ADDR.
  * @param Global_IT: the interrupt source to be configured, could be:
  *   @arg  Global_IT_GPIO : All IOs interrupt
  *   @arg  Global_IT_ADC : ADC interrupt
  *   @arg  Global_IT_TEMP : Temperature Sensor interrupts      
  *   @arg  Global_IT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  Global_IT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  Global_IT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  Global_IT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  Global_IT_TOUCH : Touch Screen Controller Touch Detected interrupt 
  * @param  NewState: can be ENABLE pr DISABLE   
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_GITConfig(uint8_t DeviceAddr, uint8_t Global_IT, FunctionalState NewState)
{
    uint8_t tmp = 0;

    /* Get the current value of the INT_EN register */
    tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_INT_EN);

    if(NewState != DISABLE) {
        /* Set the interrupts to be Enabled */
        tmp |= (uint8_t)Global_IT;
    }
    else {
        /* Set the interrupts to be Disabled */
        tmp &= ~(uint8_t)Global_IT;
    }
    /* Set the register */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_INT_EN, tmp);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Configures the selected pins to generate an interrupt or not.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_IT: The IO interrupt to be configured. This parameter could be any
  *         combination of the following values:
  *   @arg  IO_IT_x: where x can be from 0 to 7.
  * @param  NewState: could be ENABLE or DISABLE.  
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_IOITConfig(uint8_t DeviceAddr, uint8_t IO_IT, FunctionalState NewState)
{
    uint8_t tmp = 0;

    tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_INT_EN);

    if(NewState != DISABLE) {
        /* Set the interrupts to be Enabled */
        tmp |= (uint8_t)IO_IT;
    }
    else {
        /* Set the interrupts to be Disabled */
        tmp &= ~(uint8_t)IO_IT;
    }

    /* Set the register */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_INT_EN, tmp);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Configures the touch Screen Controller (Single point detection)
  * @param  None
  * @retval IOE_OK if all initializations are OK. Other value if error.
  */
uint8_t IOE_TS_Config(void)
{
    /* Enable touch screen functionality */
    IOE_FnctCmd(IOE_1_ADDR, IOE_TS_FCT, ENABLE);

    /* Select Sample Time, bit number and ADC Reference */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_ADC_CTRL1, 0x49);

    /* Wait for ~20 ms */
    _delay_(2);

    /* Select the ADC clock speed: 3.25 MHz */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_ADC_CTRL2, 0x01);

    /* Select TSC pins in non default mode */
    IOE_IOAFConfig(IOE_1_ADDR, (uint8_t)TOUCH_IO_ALL, DISABLE);

    /* Select 2 nF filter capacitor */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_CFG, 0x9A);

    /* Select single point reading  */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_TH, 0x01);

    /* Write 0x01 to clear the FIFO memory content. */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x01);

    /* Write 0x00 to put the FIFO back into operation mode  */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x00);

    /* set the data format for Z value: 7 fractional part and 1 whole part */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_FRACT_XYZ, 0x01);

    /* set the driving capability of the device for TSC pins: 50mA */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_I_DRIVE, 0x01);

    /* Use no tracking index, touch-screen controller operation mode (XYZ) and 
     enable the TSC */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_CTRL, 0x03);

    /*  Clear all the status pending bits */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_INT_STA, 0xFF);

    /* Initialize the TS structure to their default values */
    TS_State.TouchDetected = TS_State.X = TS_State.Y = TS_State.Z = 0;

    /* All configuration done */
    return IOE_OK;
}

/**
  * @brief  Configures and enables the Temperature sensor module.
  * @param  None
  * @retval IOE_OK if all initializations are OK. Other value if error.
  */
uint8_t IOE_TempSens_Config(void)
{
    __IO uint8_t tmp = 0;

    /* Enable Temperature Sensor Fct */
    IOE_FnctCmd(IOE_2_ADDR, (IOE_TEMPSENS_FCT | IOE_ADC_FCT), ENABLE);

    /* Enable the TEMPSENS module */
    I2C_WriteDeviceRegister(IOE_2_ADDR, IOE_REG_TEMP_CTRL, 0x01);

    /* Aquire data enable */
    I2C_WriteDeviceRegister(IOE_2_ADDR, IOE_REG_TEMP_CTRL, 0x02);

    /* All configuration done */
    return IOE_OK;
}

/**
  * @brief  Configures the selected pin to be in Alternate function or not
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_Pin: IO_Pin_x, Where x can be from 0 to 7.   
  * @param  NewState: State of the AF for the selected pin, could be 
  *         ENABLE or DISABLE.       
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_IOAFConfig(uint8_t DeviceAddr, uint8_t IO_Pin, FunctionalState NewState)
{
    uint8_t tmp = 0;

    /* Get the current state of the GPIO_AF register */
    tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_AF);

    if(NewState != DISABLE) {
        /* Enable the selected pins alternate function */
        tmp |= (uint8_t)IO_Pin;
    }
    else {
        /* Disable the selected pins alternate function */
        tmp &= ~(uint8_t)IO_Pin;
    }

    /* Write back the new value in GPIO_AF register */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_AF, tmp);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Configures the Edge for which a transition is detectable for the
  *         the selected pin.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_Pin: IO_Pin_x, Where x can be from 0 to 7.   
  * @param  Edge: The edge which will be detected. This parameter can be one or a
  *         a combination of following values: EDGE_FALLING and EDGE_RISING .
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_IOEdgeConfig(uint8_t DeviceAddr, uint8_t IO_Pin, uint8_t Edge)
{
    uint8_t tmp1 = 0, tmp2 = 0;

    /* Get the registers values */
    tmp1 = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_FE);
    tmp2 = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_RE);

    /* Disable the Falling Edge */
    tmp1 &= ~(uint8_t)IO_Pin;
    /* Disable the Falling Edge */
    tmp2 &= ~(uint8_t)IO_Pin;

    /* Enable the Falling edge if selected */
    if(Edge & EDGE_FALLING) {
        tmp1 |= (uint8_t)IO_Pin;
    }

    /* Enable the Rising edge if selected */
    if(Edge & EDGE_RISING) {
        tmp2 |= (uint8_t)IO_Pin;
    }

    /* Write back the registers values */
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_FE, tmp1);
    I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_RE, tmp2);

    /* if OK return 0 */
    return IOE_OK;
}

/**
  * @brief  Configures the Interrupt line active state and format (level/edge)
  * @param  Polarity: could be
  *   @arg  Polarity_Low: Interrupt line is active Low/Falling edge      
  *   @arg  Polarity_High: Interrupt line is active High/Rising edge      
  * @param  Type: Interrupt line activity type, could be one of the following values
  *   @arg  Type_Level: Interrupt line is active in level model         
  *   @arg  Type_Edge: Interrupt line is active in edge model           
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_ITOutConfig(uint8_t Polarity, uint8_t Type)
{
    uint8_t tmp = 0;

    /*  Get the register IOE_REG_INT_CTRL value */
    tmp = I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_INT_CTRL);

    /* Mask the polarity and type bits */
    tmp &= ~(uint8_t)0x06;

    /* Modify the Interrupt Output line configuration */
    tmp |= (uint8_t)(Polarity | Type);

    /* Set the register */
    I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_INT_CTRL, tmp);

    /*  Get the register IOE_REG_INT_CTRL value */
    tmp = I2C_ReadDeviceRegister(IOE_2_ADDR, IOE_REG_INT_CTRL);
    /* Mask the polarity and type bits */
    tmp &= ~(uint8_t)0x06;

    /* Modify the Interrupt Output line configuration */
    tmp |= (uint8_t)(Polarity | Type);

    /* Set the register */
    I2C_WriteDeviceRegister(IOE_2_ADDR, IOE_REG_INT_CTRL, tmp);

    /* If all OK return IOE_OK */
    return IOE_OK;
}

/**
  * @brief  Writes a value in a register of the device through I2C.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR. 
  * @param  RegisterAddr: The target register address
  * @param  RegisterValue: The target register value to be written 
  * @retval IOE_OK: if all operations are OK. Other value if error.
  */
uint8_t I2C_DMA_WriteDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t RegisterValue)
{
    uint32_t read_verif = 0;
    uint8_t IOE_BufferTX = 0;

    /* Get Value to be written */
    IOE_BufferTX = RegisterValue;

    /* Configure DMA Peripheral */
    IOE_DMA_Config(IOE_DMA_TX, (uint8_t*)(&IOE_BufferTX));

    /* Enable the I2C peripheral */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on SB Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_SB) == RESET) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Transmit the slave address and enable writing operation */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Transmitter);

    /* Test on ADDR Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Transmit the first address for r/w operations */
    I2C_SendData(IOE_I2C, RegisterAddr);

    /* Test on TXE FLag (data dent) */
    IOE_TimeOut = TIMEOUT_MAX;
    while((!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_BTF))) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Enable I2C DMA request */
    I2C_DMACmd(IOE_I2C, ENABLE);

    /* Enable DMA TX Channel */
    DMA_Cmd(IOE_DMA_TX_STREAM, ENABLE);

    /* Wait until DMA Transfer Complete */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!DMA_GetFlagStatus(IOE_DMA_TX_STREAM, IOE_DMA_TX_TCFLAG)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Wait until BTF Flag is set before generating STOP */
    IOE_TimeOut = 0xFF * TIMEOUT_MAX;
    while((!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_BTF))) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send STOP Condition */
    I2C_GenerateSTOP(IOE_I2C, ENABLE);

    /* Disable DMA TX Channel */
    DMA_Cmd(IOE_DMA_TX_STREAM, DISABLE);

    /* Disable I2C DMA request */
    I2C_DMACmd(IOE_I2C, DISABLE);

    /* Clear DMA TX Transfer Complete Flag */
    DMA_ClearFlag(IOE_DMA_TX_STREAM, IOE_DMA_TX_TCFLAG);

#ifdef VERIFY_WRITTENDATA
    /* Verify (if needed) that the loaded data is correct  */

    /* Read the just written register*/
    read_verif = I2C_ReadDeviceRegister(DeviceAddr, RegisterAddr);
    /* Load the register and verify its value  */
    if(read_verif != RegisterValue) {
        /* Control data wrongly transferred */
        read_verif = IOE_FAILURE;
    }
    else {
        /* Control data correctly transferred */
        read_verif = 0;
    }
#endif

    /* Return the verifying value: 0 (Passed) or 1 (Failed) */
    return read_verif;
}

/**
  * @brief  Reads a register of the device through I2C.
  * @param  DeviceAddr: The address of the device, could be : IOE_1_ADDR
  *         or IOE_2_ADDR. 
  * @param  RegisterAddr: The target register address (between 00x and 0x24)
  * @retval The value of the read register (0xAA if Timeout occurred)   
  */
uint8_t I2C_DMA_ReadDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr)
{
    uint8_t IOE_BufferRX[2] = { 0x00, 0x00 };

    /* Configure DMA Peripheral */
    IOE_DMA_Config(IOE_DMA_RX, (uint8_t*)IOE_BufferRX);

    /* Enable DMA NACK automatic generation */
    I2C_DMALastTransferCmd(IOE_I2C, ENABLE);

    /* Enable the I2C peripheral */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on SB Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_SB)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send device address for write */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Transmitter);

    /* Test on ADDR Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send the device's internal address to write to */
    I2C_SendData(IOE_I2C, RegisterAddr);

    /* Test on TXE FLag (data dent) */
    IOE_TimeOut = TIMEOUT_MAX;
    while((!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_BTF))) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send START condition a second time */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on SB Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_SB)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send IOExpander address for read */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Receiver);

    /* Test on ADDR Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Enable I2C DMA request */
    I2C_DMACmd(IOE_I2C, ENABLE);

    /* Enable DMA RX Channel */
    DMA_Cmd(IOE_DMA_RX_STREAM, ENABLE);

    /* Wait until DMA Transfer Complete */
    IOE_TimeOut = 2 * TIMEOUT_MAX;
    while(!DMA_GetFlagStatus(IOE_DMA_RX_STREAM, IOE_DMA_RX_TCFLAG)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send STOP Condition */
    I2C_GenerateSTOP(IOE_I2C, ENABLE);

    /* Disable DMA RX Channel */
    DMA_Cmd(IOE_DMA_RX_STREAM, DISABLE);

    /* Disable I2C DMA request */
    I2C_DMACmd(IOE_I2C, DISABLE);

    /* Clear DMA RX Transfer Complete Flag */
    DMA_ClearFlag(IOE_DMA_RX_STREAM, IOE_DMA_RX_TCFLAG);

    /* return a pointer to the IOE_Buffer */
    return (uint8_t)IOE_BufferRX[0];
}

/**
  * @brief  Reads a buffer of 2 bytes from the device registers.
  * @param  DeviceAddr: The address of the device, could be : IOE_1_ADDR
  *         or IOE_2_ADDR. 
  * @param  RegisterAddr: The target register address (between 00x and 0x24)
  * @retval The data in the buffer containing the two returned bytes (in halfword).  
  */
uint16_t I2C_DMA_ReadDataBuffer(uint8_t DeviceAddr, uint32_t RegisterAddr)
{
    uint8_t tmp = 0;
    uint8_t IOE_BufferRX[2] = { 0x00, 0x00 };

    /* Configure DMA Peripheral */
    IOE_DMA_Config(IOE_DMA_RX, (uint8_t*)IOE_BufferRX);

    /* Enable DMA NACK automatic generation */
    I2C_DMALastTransferCmd(IOE_I2C, ENABLE);

    /* Enable the I2C peripheral */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on SB Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_SB)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send device address for write */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Transmitter);

    /* Test on ADDR Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send the device's internal address to write to */
    I2C_SendData(IOE_I2C, RegisterAddr);

    /* Test on TXE FLag (data dent) */
    IOE_TimeOut = TIMEOUT_MAX;
    while((!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_BTF))) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send START condition a second time */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on SB Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_SB)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send IOExpander address for read */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Receiver);

    /* Test on ADDR Flag */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Enable I2C DMA request */
    I2C_DMACmd(IOE_I2C, ENABLE);

    /* Enable DMA RX Channel */
    DMA_Cmd(IOE_DMA_RX_STREAM, ENABLE);

    /* Wait until DMA Transfer Complete */
    IOE_TimeOut = 2 * TIMEOUT_MAX;
    while(!DMA_GetFlagStatus(IOE_DMA_RX_STREAM, IOE_DMA_RX_TCFLAG)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send STOP Condition */
    I2C_GenerateSTOP(IOE_I2C, ENABLE);

    /* Disable DMA RX Channel */
    DMA_Cmd(IOE_DMA_RX_STREAM, DISABLE);

    /* Disable I2C DMA request */
    I2C_DMACmd(IOE_I2C, DISABLE);

    /* Clear DMA RX Transfer Complete Flag */
    DMA_ClearFlag(IOE_DMA_RX_STREAM, IOE_DMA_RX_TCFLAG);

    /* Reorganize received data */
    tmp = IOE_BufferRX[0];
    IOE_BufferRX[0] = IOE_BufferRX[1];
    IOE_BufferRX[1] = tmp;

    /* return the data */
    return ((uint16_t)IOE_BufferRX[0] | ((uint16_t)IOE_BufferRX[1] << 8));
}

/**
  * @brief  Return Touch Screen X position value
  * @param  None
  * @retval X position.
  */
static uint16_t IOE_TS_Read_X(void)
{
    int32_t x, xr;

    x = I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_Y);

    /* first correction */
    xr = (x * 320) >> 12;
    /* second correction */
    xr = ((xr * 32) / 29) - 17;

    if(xr <= 0)
        xr = 0;

    return (uint16_t)(xr);
}

/**
  * @brief  Return Touch Screen Y position value
  * @param  None
  * @retval Y position.
  */
static uint16_t IOE_TS_Read_Y(void)
{
    int32_t y, yr;
    y = I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_X);

    yr = (y * 240) >> 12;
    yr = ((yr * 240) / 217) - 12;

    if(yr <= 0)
        yr = 0;

    return (uint16_t)(yr);
}

/**
  * @brief  Return Touch Screen Z position value
  * @param  None
  * @retval Z position.
  */
static uint16_t IOE_TS_Read_Z(void)
{
    uint32_t z;
    z = I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_Z);

    if(z <= 0)
        z = 0;

    return (uint16_t)(z);
}

/**
  * @brief  Initializes the GPIO pins used by the IO expander.
  * @param  None
  * @retval None
  */
static void IOE_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable IOE_I2C and IOE_I2C_GPIO_PORT & Alternate Function clocks */
    RCC_APB1PeriphClockCmd(IOE_I2C_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(IOE_I2C_SCL_GPIO_CLK | IOE_I2C_SDA_GPIO_CLK |
                               IOE_IT_GPIO_CLK,
        ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Reset IOE_I2C IP */
    RCC_APB1PeriphResetCmd(IOE_I2C_CLK, ENABLE);
    /* Release reset signal of IOE_I2C IP */
    RCC_APB1PeriphResetCmd(IOE_I2C_CLK, DISABLE);

    /* Connect PXx to I2C_SCL*/
    GPIO_PinAFConfig(IOE_I2C_SCL_GPIO_PORT, IOE_I2C_SCL_SOURCE, IOE_I2C_SCL_AF);
    /* Connect PXx to I2C_SDA*/
    GPIO_PinAFConfig(IOE_I2C_SDA_GPIO_PORT, IOE_I2C_SDA_SOURCE, IOE_I2C_SDA_AF);

    /* IOE_I2C SCL and SDA pins configuration */
    GPIO_InitStructure.GPIO_Pin = IOE_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(IOE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = IOE_I2C_SDA_PIN;
    GPIO_Init(IOE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

    /* Set EXTI pin as Input PullUp - IO_Expander_INT */
    GPIO_InitStructure.GPIO_Pin = IOE_IT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(IOE_IT_GPIO_PORT, &GPIO_InitStructure);

    /* Connect Button EXTI Line to Button GPIO Pin */
    SYSCFG_EXTILineConfig(IOE_IT_EXTI_PORT_SOURCE, IOE_IT_EXTI_PIN_SOURCE);
}

/**
  * @brief  Configure the I2C Peripheral used to communicate with IO_Expanders.
  * @param  None
  * @retval None
  */
static void IOE_I2C_Config(void)
{
    I2C_InitTypeDef I2C_InitStructure;

    /* IOE_I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;

    /* Initialize the I2C peripheral */
    I2C_Init(IOE_I2C, &I2C_InitStructure);

    /* Enable the I2C peripheral */
    I2C_Cmd(IOE_I2C, ENABLE);
}

/**
  * @brief  Configure the DMA Peripheral used to handle communication via I2C.
  * @param  None
  * @retval None
  */

static void IOE_DMA_Config(IOE_DMADirection_TypeDef Direction, uint8_t* buffer)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(IOE_DMA_CLK, ENABLE);

    /* Initialize the DMA_Channel member */
    DMA_InitStructure.DMA_Channel = IOE_DMA_CHANNEL;

    /* Initialize the DMA_PeripheralBaseAddr member */
    DMA_InitStructure.DMA_PeripheralBaseAddr = IOE_I2C_DR;

    /* Initialize the DMA_Memory0BaseAddr member */
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buffer;

    /* Initialize the DMA_PeripheralInc member */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

    /* Initialize the DMA_MemoryInc member */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    /* Initialize the DMA_PeripheralDataSize member */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

    /* Initialize the DMA_MemoryDataSize member */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

    /* Initialize the DMA_Mode member */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

    /* Initialize the DMA_Priority member */
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;

    /* Initialize the DMA_FIFOMode member */
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;

    /* Initialize the DMA_FIFOThreshold member */
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;

    /* Initialize the DMA_MemoryBurst member */
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;

    /* Initialize the DMA_PeripheralBurst member */
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    /* If using DMA for Reception */
    if(Direction == IOE_DMA_RX) {
        /* Initialize the DMA_DIR member */
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;

        /* Initialize the DMA_BufferSize member */
        DMA_InitStructure.DMA_BufferSize = 2;

        DMA_DeInit(IOE_DMA_RX_STREAM);

        DMA_Init(IOE_DMA_RX_STREAM, &DMA_InitStructure);
    }
    /* If using DMA for Transmission */
    else if(Direction == IOE_DMA_TX) {
        /* Initialize the DMA_DIR member */
        DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;

        /* Initialize the DMA_BufferSize member */
        DMA_InitStructure.DMA_BufferSize = 1;

        DMA_DeInit(IOE_DMA_TX_STREAM);

        DMA_Init(IOE_DMA_TX_STREAM, &DMA_InitStructure);
    }
}

/**
  * @brief  Writes a value in a register of the device through I2C.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR. 
  * @param  RegisterAddr: The target register address
  * @param  RegisterValue: The target register value to be written 
  * @retval IOE_OK: if all operations are OK. Other value if error.
  */
uint8_t I2C_WriteDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t RegisterValue)
{
    uint32_t read_verif = 0;

    /* Begin the configuration sequence */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on EV5 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Transmit the slave address and enable writing operation */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Read status register 2 to clear ADDR flag */
    I2C1->SR2;

    /* Test on EV8_1 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Transmit the first address for r/w operations */
    I2C_SendData(IOE_I2C, RegisterAddr);

    /* Test on EV8 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Prepare the register value to be sent */
    I2C_SendData(IOE_I2C, RegisterValue);

    /* Test on EV8_2 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while((!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_BTF))) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* End the configuration sequence */
    I2C_GenerateSTOP(IOE_I2C, ENABLE);

#ifdef VERIFY_WRITTENDATA
    /* Verify (if needed) that the loaded data is correct  */

    /* Read the just written register*/
    read_verif = IOE_I2C_ReadDeviceRegister(DeviceAddr, RegisterAddr);

    /* Load the register and verify its value  */
    if(read_verif != RegisterValue) {
        /* Control data wrongly transferred */
        read_verif = IOE_FAILURE;
    }
    else {
        /* Control data correctly transferred */
        read_verif = 0;
    }
#endif

    /* Return the verifying value: 0 (Passed) or 1 (Failed) */
    return read_verif;
}

/**
  * @brief  Reads a register of the device through I2C without DMA.
  * @param  DeviceAddr: The address of the device, could be : IOE_1_ADDR
  *         or IOE_2_ADDR. 
  * @param  RegisterAddr: The target register address (between 00x and 0x24)
  * @retval The value of the read register (0xAA if Timeout occurred)   
  */
uint8_t I2C_ReadDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr)
{
    uint8_t tmp = 0;

    /* Enable the I2C peripheral */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on EV5 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_SB)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }
    /* Disable Acknowledgement */
    I2C_AcknowledgeConfig(IOE_I2C, DISABLE);

    /* Transmit the slave address and enable writing operation */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_ADDR)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Read status register 2 to clear ADDR flag */
    IOE_I2C->SR2;

    /* Test on EV8 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Transmit the first address for r/w operations */
    I2C_SendData(IOE_I2C, RegisterAddr);

    /* Test on EV8 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while((!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_BTF))) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Regenerate a start condition */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on EV5 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Transmit the slave address and enable writing operation */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_ADDR)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Read status register 2 to clear ADDR flag */
    IOE_I2C->SR2;

    /* Test on EV7 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_RXNE)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* End the configuration sequence */
    I2C_GenerateSTOP(IOE_I2C, ENABLE);

    /* Load the register value */
    tmp = I2C_ReceiveData(IOE_I2C);

    /* Enable Acknowledgement */
    I2C_AcknowledgeConfig(IOE_I2C, ENABLE);

    /* Return the read value */
    return tmp;
}

/**
  * @brief  Reads a buffer of 2 bytes from the device registers.
  * @param  DeviceAddr: The address of the device, could be : IOE_1_ADDR
  *         or IOE_2_ADDR. 
  * @param  RegisterAddr: The target register adress (between 00x and 0x24)
  * @retval The data in the buffer containing the two returned bytes (in halfword).   
  */
uint16_t I2C_ReadDataBuffer(uint8_t DeviceAddr, uint32_t RegisterAddr)
{
    uint8_t IOE_BufferRX[2] = { 0x00, 0x00 };

    /* Enable the I2C peripheral */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on EV5 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_SB)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send device address for write */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_ADDR)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Read status register 2 to clear ADDR flag */
    IOE_I2C->SR2;

    /* Test on EV8 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send the device's internal address to write to */
    I2C_SendData(IOE_I2C, RegisterAddr);

    /* Send START condition a second time */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on EV5 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_SB)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send IO Expander address for read */
    I2C_Send7bitAddress(IOE_I2C, DeviceAddr, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_ADDR)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Disable Acknowledgement and set Pos bit */
    I2C_AcknowledgeConfig(IOE_I2C, DISABLE);
    I2C_NACKPositionConfig(IOE_I2C, I2C_NACKPosition_Next);

    /* Read status register 2 to clear ADDR flag */
    IOE_I2C->SR2;

    /* Test on EV7 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while(!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_BTF)) {
        if(IOE_TimeOut-- == 0) return (IOE_TimeoutUserCallback());
    }

    /* Send STOP Condition */
    I2C_GenerateSTOP(IOE_I2C, ENABLE);

    /* Read the first byte from the IO Expander */
    IOE_BufferRX[1] = I2C_ReceiveData(IOE_I2C);

    /* Read the second byte from the IO Expander */
    IOE_BufferRX[0] = I2C_ReceiveData(IOE_I2C);

    /* Enable Acknowledgement and reset POS bit to be ready for another reception */
    I2C_AcknowledgeConfig(IOE_I2C, ENABLE);
    I2C_NACKPositionConfig(IOE_I2C, I2C_NACKPosition_Current);

    /* return the data */
    return ((uint16_t)IOE_BufferRX[0] | ((uint16_t)IOE_BufferRX[1] << 8));
}

/**
  * @brief  Configures the IO expander Interrupt line and GPIO in EXTI mode.
  * @param  None        
  * @retval None
  */
static void IOE_EXTI_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd(IOE_IT_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure Button pin as input floating */
    GPIO_InitStructure.GPIO_Pin = IOE_IT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(IOE_IT_GPIO_PORT, &GPIO_InitStructure);

    /* Connect Button EXTI Line to Button GPIO Pin */
    SYSCFG_EXTILineConfig(IOE_IT_EXTI_PORT_SOURCE, IOE_IT_EXTI_PIN_SOURCE);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = IOE_IT_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = IOE_IT_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

#ifndef USE_TIMEOUT_USER_CALLBACK
/**
  * @brief  IOE_TIMEOUT_UserCallback
  * @param  None
  * @retval 0
  */
uint8_t IOE_TimeoutUserCallback(void)
{
    I2C_InitTypeDef I2C_InitStructure;

    I2C_GenerateSTOP(IOE_I2C, ENABLE);
    I2C_SoftwareResetCmd(IOE_I2C, ENABLE);
    I2C_SoftwareResetCmd(IOE_I2C, DISABLE);

    IOE_GPIO_Config();

    /* CODEC_I2C peripheral configuration */
    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x33;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;

    /* Enable the I2C peripheral */
    I2C_Cmd(IOE_I2C, ENABLE);
    I2C_Init(IOE_I2C, &I2C_InitStructure);

    return 0;
}
#endif /* !USE_TIMEOUT_USER_CALLBACK */

#ifndef USE_Delay
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void delay(__IO uint32_t nCount)
{
    __IO uint32_t index = 0;
    for(index = (100000 * nCount); index != 0; index--) {
    }
}
#endif /* USE_Delay*/
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
