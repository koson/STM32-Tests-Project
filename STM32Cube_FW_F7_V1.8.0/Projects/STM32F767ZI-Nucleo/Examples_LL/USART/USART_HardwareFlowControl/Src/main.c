/**
 ******************************************************************************
 * @file    Examples_LL/USART/USART_HardwareFlowControl/Src/main.c
 * @author  MCD Application Team
 * @brief   This example describes how to transfer bytes from/to an USART peripheral
 *          to/from a PC UART with HW FLow Control enabled,
 *          through the STM32F7xx USART LL API.
 *          Peripheral initialization done using LL unitary services functions.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F7xx_LL_Examples
 * @{
 */

/** @addtogroup USART_HardwareFlowControl
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
__IO uint8_t ubButtonPress = 0;

/**
 * @brief Variables used for character tranmission to PC Com port
 */
__IO uint8_t ubSend = 0;
const uint8_t aStringToSend[] =
   "STM32F7xx USART LL API Example : TX/RX in IT mode\r\nConfiguration UART 115200 bps, 8 data bit/1 stop bit/No "
   "parity/HW flow control enabled.\r\nEnter any character (S or s will end program).\r\n";
uint8_t ubSizeToSend = sizeof(aStringToSend);

/**
 * @brief Variables used for character reception from PC Com port
 */
__IO uint8_t ubFinalCharReceived = 0;
__IO uint32_t ubReceivedChar;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Configure_USART1(void);
void StartTransfers(void);
void LED_Init(void);
void LED_On(void);
void LED_Blinking(uint32_t Period);
void LED_Off(void);
void UserButton_Init(void);
void WaitForUserButtonPress(void);
void WaitAndCheckEndOfTransfer(void);
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
   /* Enable the CPU Cache */
   CPU_CACHE_Enable();

   /* Configure the system clock to 216 MHz */
   SystemClock_Config();

   /* Initialize LED1 */
   LED_Init();

   /* Set LED1 Off */
   LED_Off();

   /* Initialize User push-button in EXTI mode */
   UserButton_Init();

   /* Configure USART1 (USART IP configuration and related GPIO initialization) */
   Configure_USART1();

   /* Wait for User push-button press to start transfer */
   WaitForUserButtonPress();

   /* Initiate DMA transfers */
   StartTransfers();

   /* Wait for the end of the transfer and check received data */
   WaitAndCheckEndOfTransfer();

   /* Infinite loop */
   while (1)
   {
   }
}

/**
 * @brief  This function configures USART1.
 * @note   This function is used to :
 *         -1- Enable GPIO clock and configures the USART1 pins.
 *         -2- NVIC Configuration for USART1 interrupts.
 *         -3- Enable the USART1 peripheral clock and clock source.
 *         -4- Configure USART1 functional parameters.
 *         -5- Enable USART1.
 * @note   Peripheral configuration is minimal configuration from reset values.
 *         Thus, some useless LL unitary functions calls below are provided as
 *         commented examples - setting is default configuration from reset.
 * @param  None
 * @retval None
 */
void Configure_USART1(void)
{
   /* (1) Enable GPIO clock and configures the USART1 pins **********************/
   /*    (TX on PA.9, RX on PA.10)                         **********************/
   /*    (CTS on PA.11, RTS on PA.12)                      **********************/

   /* Enable the peripheral clock of GPIOA */
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

   /* Configure TX Pin as : Alternate function, High Speed, PushPull, Pull up */
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_7);
   LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

   /* Configure RX Pin as : Alternate function, High Speed, PushPull, Pull up */
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_7);
   LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);

   /* Configure CTS Pin as : Alternate function, High Speed, OpenDrain, Pull up */
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_11, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_11, LL_GPIO_AF_7);
   LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_11, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_11, LL_GPIO_OUTPUT_OPENDRAIN);
   LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_11, LL_GPIO_PULL_UP);

   /* Configure RTS Pin as : Alternate function, High Speed, OpenDrain, Pull up */
   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_12, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_12, LL_GPIO_AF_7);
   LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_12, LL_GPIO_OUTPUT_OPENDRAIN);
   LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_12, LL_GPIO_PULL_UP);

   /* (2) NVIC Configuration for USART1 interrupts */
   /*  - Set priority for USART1_IRQn */
   /*  - Enable USART1_IRQn           */
   NVIC_SetPriority(USART1_IRQn, 0);
   NVIC_EnableIRQ(USART1_IRQn);

   /* (3) Enable the USART1 peripheral clock and clock source ****************/
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

   /* Set clock source */
   LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

   /* (4) Configure USART1 functional parameters ********************************/

   /* Disable USART1 prior modifying configuration registers */
   /* Note: Commented as corresponding to Reset value */
   // LL_USART_Disable(USART1);

   /* TX/RX direction */
   LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);

   /* 8 data bit, 1 start bit, 1 stop bit, no parity */
   LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

   /* Enable CTS/RTS Hardware Flow Control */
   LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_RTS_CTS);

   /* Oversampling by 16 */
   /* Reset value is LL_USART_OVERSAMPLING_16 */
   // LL_USART_SetOverSampling(USART1, LL_USART_OVERSAMPLING_16);

   /* Set Baudrate to 115200 using APB frequency set to 108000000 Hz */
   /* Frequency available for USART peripheral can also be calculated through LL RCC macro */
   /* Ex :
       Periphclk = LL_RCC_GetUSARTClockFreq(Instance); or LL_RCC_GetUARTClockFreq(Instance); depending on USART/UART
   instance
   
       In this example, Peripheral Clock is expected to be equal to 108000000 Hz => equal to SystemCoreClock/2
   */
   LL_USART_SetBaudRate(USART1, SystemCoreClock / 2, LL_USART_OVERSAMPLING_16, 115200);

   /* (5) Enable USART *********************************************************/
   LL_USART_Enable(USART1);

   /* Enable RXNE and Error interrupts */
   LL_USART_EnableIT_RXNE(USART1);
   LL_USART_EnableIT_ERROR(USART1);
}

/**
 * @brief  This function initiates TX transfer
 * @param  None
 * @retval None
 */
void StartTransfers(void)
{
   /* Start transfer only if not already ongoing */
   if (ubSend == 0)
   {
      /* Start USART transmission : Will initiate TXE interrupt after TDR register is empty */
      LL_USART_TransmitData8(USART1, aStringToSend[ubSend++]);

      /* Enable TXE interrupt */
      LL_USART_EnableIT_TXE(USART1);
   }
}

/**
 * @brief  Initialize LED1.
 * @param  None
 * @retval None
 */
void LED_Init(void)
{
   /* Enable the LED1 Clock */
   LED1_GPIO_CLK_ENABLE();

   /* Configure IO in output push-pull mode to drive external LED1 */
   LL_GPIO_SetPinMode(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
   /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
   // LL_GPIO_SetPinOutputType(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
   /* Reset value is LL_GPIO_SPEED_FREQ_LOW */
   // LL_GPIO_SetPinSpeed(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);
   /* Reset value is LL_GPIO_PULL_NO */
   // LL_GPIO_SetPinPull(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_PULL_NO);
}

/**
 * @brief  Turn-on LED1.
 * @param  None
 * @retval None
 */
void LED_On(void)
{
   /* Turn LED1 on */
   LL_GPIO_SetOutputPin(LED1_GPIO_PORT, LED1_PIN);
}

/**
 * @brief  Turn-off LED1.
 * @param  None
 * @retval None
 */
void LED_Off(void)
{
   /* Turn LED1 off */
   LL_GPIO_ResetOutputPin(LED1_GPIO_PORT, LED1_PIN);
}

/**
 * @brief  Set LED1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
 * @param  Period : Period of time (in ms) between each toggling of LED
 *   This parameter can be user defined values. Pre-defined values used in that example are :
 *     @arg LED_BLINK_FAST : Fast Blinking
 *     @arg LED_BLINK_SLOW : Slow Blinking
 *     @arg LED_BLINK_ERROR : Error specific Blinking
 * @retval None
 */
void LED_Blinking(uint32_t Period)
{
   /* Toggle LED1 in an infinite loop */
   while (1)
   {
      LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
      LL_mDelay(Period);
   }
}

/**
 * @brief  Configures User push-button in GPIO or EXTI Line Mode.
 * @param  None
 * @retval None
 */
void UserButton_Init(void)
{
   /* Enable the BUTTON Clock */
   USER_BUTTON_GPIO_CLK_ENABLE();

   /* Configure GPIO for BUTTON */
   LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT);
   LL_GPIO_SetPinPull(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_PULL_NO);

   /* Connect External Line to the GPIO*/
   USER_BUTTON_SYSCFG_SET_EXTI();

   /* Enable a rising trigger EXTI_Line15_10 Interrupt */
   USER_BUTTON_EXTI_LINE_ENABLE();
   USER_BUTTON_EXTI_FALLING_TRIG_ENABLE();

   /* Configure NVIC for USER_BUTTON_EXTI_IRQn */
   NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 3);
   NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn);
}

/**
 * @brief  Wait for User push-button press to start transfer.
 * @param  None
 * @retval None
 */
/*  */
void WaitForUserButtonPress(void)
{
   while (ubButtonPress == 0)
   {
      LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
      LL_mDelay(LED_BLINK_FAST);
   }
   /* Ensure that LED1 is turned Off */
   LED_Off();
}

/**
 * @brief  Wait end of transfer and check if received Data are well.
 * @param  None
 * @retval None
 */
void WaitAndCheckEndOfTransfer(void)
{
   /* Wait end of reception from USART1 */
   while (ubFinalCharReceived != 1)
   {
   }

   /* Turn On Led indicating final character has been received */
   LED_On();
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 216000000
 *            HCLK(Hz)                       = 216000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSI Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 432
 *            PLL_P                          = 2
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 7
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
   /* Enable HSE clock */
   LL_RCC_HSE_EnableBypass();
   LL_RCC_HSE_Enable();
   while (LL_RCC_HSE_IsReady() != 1)
   {
   };

   /* Set FLASH latency */
   LL_FLASH_SetLatency(LL_FLASH_LATENCY_7);

   /* Enable PWR clock */
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

   /* Activation OverDrive Mode */
   LL_PWR_EnableOverDriveMode();
   while (LL_PWR_IsActiveFlag_OD() != 1)
   {
   };

   /* Activation OverDrive Switching */
   LL_PWR_EnableOverDriveSwitching();
   while (LL_PWR_IsActiveFlag_ODSW() != 1)
   {
   };

   /* Main PLL configuration and activation */
   LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 432, LL_RCC_PLLP_DIV_2);
   LL_RCC_PLL_Enable();
   while (LL_RCC_PLL_IsReady() != 1)
   {
   };

   /* Sysclk activation on the main PLL */
   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
   LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
   while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
   {
   };

   /* Set APB1 & APB2 prescaler */
   LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
   LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);

   /* Set systick to 1ms */
   SysTick_Config(216000000 / 1000);

   /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
   SystemCoreClock = 216000000;
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT Functions                                     */
/******************************************************************************/
/**
 * @brief  Function to manage User push-button
 * @param  None
 * @retval None
 */
void UserButton_Callback(void)
{
   /* Update User push-button variable : to be checked in waiting loop in main program */
   ubButtonPress = 1;
}

/**
 * @brief  Function called for achieving next TX Byte sending
 * @param  None
 * @retval None
 */
void USART_TXEmpty_Callback(void)
{
   if (ubSend == (ubSizeToSend - 1))
   {
      /* Disable TXE interrupt */
      LL_USART_DisableIT_TXE(USART1);

      /* Enable TC interrupt */
      LL_USART_EnableIT_TC(USART1);
   }

   /* Fill TDR with a new char */
   LL_USART_TransmitData8(USART1, aStringToSend[ubSend++]);
}

/**
 * @brief  Function called at completion of last byte transmission
 * @param  None
 * @retval None
 */
void USART_CharTransmitComplete_Callback(void)
{
   if (ubSend == sizeof(aStringToSend))
   {
      ubSend = 0;

      /* Disable TC interrupt */
      LL_USART_DisableIT_TC(USART1);
   }
}

/**
 * @brief  Function called from USART IRQ Handler when RXNE flag is set
 *         Function is in charge of reading character received on USART RX line.
 * @param  None
 * @retval None
 */
void USART_CharReception_Callback(void)
{
   __IO uint32_t received_char;

   /* Read Received character. RXNE flag is cleared by reading of RDR register */
   received_char = LL_USART_ReceiveData8(USART1);

   /* Check if received value is corresponding to specific one : S or s */
   if ((received_char == 'S') || (received_char == 's'))
   {
      ubFinalCharReceived = 1;

      /* Disable RXNE interrupt */
      LL_USART_DisableIT_RXNE(USART1);
   }

   /* Echo received character on TX */
   LL_USART_TransmitData8(USART1, received_char);
}

/**
 * @brief  Function called in case of error detected in USART IT Handler
 * @param  None
 * @retval None
 */
void Error_Callback(void)
{
   __IO uint32_t isr_reg;

   /* Disable USART1_IRQn */
   NVIC_DisableIRQ(USART1_IRQn);

   /* Error handling example :
     - Read USART ISR register to identify flag that leads to IT raising
     - Perform corresponding error handling treatment according to flag
   */
   isr_reg = LL_USART_ReadReg(USART1, ISR);
   if (isr_reg & LL_USART_ISR_NE)
   {
      /* case Noise Error flag is raised : Clear NF Flag */
      LL_USART_ClearFlag_NE(USART1);
   }
   else
   {
      /* Unexpected IT source : Set LED to Blinking mode to indicate error occurs */
      LED_Blinking(LED_BLINK_ERROR);
   }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
   /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d", file, line) */

   /* Infinite loop */
   while (1)
   {
   }
}
#endif
/**
 * @brief  CPU L1-Cache enable.
 * @param  None
 * @retval None
 */
static void CPU_CACHE_Enable(void)
{
   /* Enable I-Cache */
   SCB_EnableICache();

   /* Enable D-Cache */
   SCB_EnableDCache();
}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
