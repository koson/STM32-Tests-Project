/**
  ******************************************************************************
  * @file    STM324x7i_eval_i2c_ee.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    19-September-2013
  * @brief   This file contains all the functions prototypes for the 
  *          STM324x7i_eval_i2c_ee.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM324x7I_EVAL_I2C_EE_H
#define __STM324x7I_EVAL_I2C_EE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM324x7i_eval.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */

/** @addtogroup STM324x7I_EVAL
  * @{
  */

/** @addtogroup STM324x7I_EVAL_I2C_EE
  * @{
  */

/** @defgroup STM324x7I_EVAL_I2C_EE_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup STM324x7I_EVAL_I2C_EE_Exported_Constants
  * @{
  */

/* Uncomment the following line to use the default sEE_TIMEOUT_UserCallback() 
   function implemented in stm32_evel_i2c_ee.c file.
   sEE_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...). */
/* #define USE_DEFAULT_TIMEOUT_CALLBACK */

#if !defined(sEE_M24C08) && !defined(sEE_M24C64_32)
/* Use the defines below the choose the EEPROM type */
/* #define sEE_M24C08*/ /* Support the device: M24C08. */
/* note: Could support: M24C01, M24C02, M24C04 and M24C16 if the blocks and 
   HW address are correctly defined*/
#define sEE_M24C64_32 /* Support the devices: M24C32 and M24C64 */
#endif

#ifdef sEE_M24C64_32
/* For M24C32 and M24C64 devices, E0,E1 and E2 pins are all used for device 
  address selection (ne need for additional address lines). According to the 
  Harware connection on the board (on STM324x7I-EVAL board E0 = E1 = E2 = 0) */

#define sEE_HW_ADDRESS 0xA0 /* E0 = E1 = E2 = 0 */

#elif defined(sEE_M24C08)
/* The M24C08W contains 4 blocks (128byte each) with the adresses below: E2 = 0 
   EEPROM Addresses defines */
#define sEE_Block0_ADDRESS 0xA0 /* E2 = 0 */
/*#define sEE_Block1_ADDRESS     0xA2*/ /* E2 = 0 */
/*#define sEE_Block2_ADDRESS     0xA4*/ /* E2 = 0 */
/*#define sEE_Block3_ADDRESS     0xA6*/ /* E2 = 0 */

#endif /* sEE_M24C64_32 */

/* I2C clock speed configuration (in Hz) 
  WARNING: 
   Make sure that this define is not already declared in other files (ie. 
  STM324x7I_eval.h file). It can be used in parallel by other modules. */
#ifndef I2C_SPEED
#define I2C_SPEED 100000
#endif /* I2C_SPEED */

#define I2C_SLAVE_ADDRESS7 0xA0

#if defined(sEE_M24C08)
#define sEE_PAGESIZE 16
#elif defined(sEE_M24C64_32)
#define sEE_PAGESIZE 32
#endif

/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define sEE_FLAG_TIMEOUT ((uint32_t)0x1000)
#define sEE_LONG_TIMEOUT ((uint32_t)(30 * sEE_FLAG_TIMEOUT))

/* Maximum number of trials for sEE_WaitEepromStandbyState() function */
#define sEE_MAX_TRIALS_NUMBER 300

/* Defintions for the state of the DMA transfer */
#define sEE_STATE_READY 0
#define sEE_STATE_BUSY 1
#define sEE_STATE_ERROR 2

#define sEE_OK 0
#define sEE_FAIL 1

/**
  * @}
  */

/** @defgroup STM324x7I_EVAL_I2C_EE_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup STM324x7I_EVAL_I2C_EE_Exported_Functions
  * @{
  */
void sEE_DeInit(void);
void sEE_Init(void);
uint32_t sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t* NumByteToRead);
uint32_t sEE_WritePage(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t* NumByteToWrite);
void sEE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
uint32_t sEE_WaitEepromStandbyState(void);

/* USER Callbacks: These are functions for which prototypes only are declared in
   EEPROM driver and that should be implemented into user applicaiton. */
/* sEE_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...).
   You can use the default timeout callback implementation by uncommenting the 
   define USE_DEFAULT_TIMEOUT_CALLBACK in STM324x7I_eval_i2c_ee.h file.
   Typically the user implementation of this callback should reset I2C peripheral
   and re-initialize communication or in worst case reset all the application. */
uint32_t sEE_TIMEOUT_UserCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM324x7I_EVAL_I2C_EE_H */
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
