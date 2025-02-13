/**
 ******************************************************************************
 * @file    stm32f7xx_hal_i2c_ex.h
 * @author  MCD Application Team
 * @brief   Header file of I2C HAL Extended module.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_HAL_I2C_EX_H
#   define __STM32F7xx_HAL_I2C_EX_H

#   ifdef __cplusplus
extern "C" {
#   endif

/* Includes ------------------------------------------------------------------*/
#   include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
 * @{
 */

/** @addtogroup I2CEx
 * @{
 */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup I2CEx_Exported_Constants I2C Extended Exported Constants
 * @{
 */

/** @defgroup I2CEx_Analog_Filter I2C Extended Analog Filter
 * @{
 */
#   define I2C_ANALOGFILTER_ENABLE 0x00000000U
#   define I2C_ANALOGFILTER_DISABLE I2C_CR1_ANFOFF
/**
 * @}
 */

/** @defgroup I2CEx_FastModePlus I2C Extended Fast Mode Plus
 * @{
 */
#   define I2C_FMP_NOT_SUPPORTED 0xAAAA0000U /*!< Fast Mode Plus not supported       */
#   if defined(SYSCFG_PMC_I2C_PB6_FMP)
#      define I2C_FASTMODEPLUS_PB6 SYSCFG_PMC_I2C_PB6_FMP /*!< Enable Fast Mode Plus on PB6       */
#      define I2C_FASTMODEPLUS_PB7 SYSCFG_PMC_I2C_PB7_FMP /*!< Enable Fast Mode Plus on PB7       */
#   else
#      define I2C_FASTMODEPLUS_PB6 \
         (uint32_t)(0x00000004U | I2C_FMP_NOT_SUPPORTED) /*!< Fast Mode Plus PB6 not supported   */
#      define I2C_FASTMODEPLUS_PB7 \
         (uint32_t)(0x00000008U | I2C_FMP_NOT_SUPPORTED) /*!< Fast Mode Plus PB7 not supported   */
#   endif
#   if defined(SYSCFG_PMC_I2C_PB8_FMP)
#      define I2C_FASTMODEPLUS_PB8 SYSCFG_PMC_I2C_PB8_FMP /*!< Enable Fast Mode Plus on PB8       */
#      define I2C_FASTMODEPLUS_PB9 SYSCFG_PMC_I2C_PB9_FMP /*!< Enable Fast Mode Plus on PB9       */
#   else
#      define I2C_FASTMODEPLUS_PB8 \
         (uint32_t)(0x00000010U | I2C_FMP_NOT_SUPPORTED) /*!< Fast Mode Plus PB8 not supported   */
#      define I2C_FASTMODEPLUS_PB9 \
         (uint32_t)(0x00000012U | I2C_FMP_NOT_SUPPORTED) /*!< Fast Mode Plus PB9 not supported   */
#   endif
#   if defined(SYSCFG_PMC_I2C1_FMP)
#      define I2C_FASTMODEPLUS_I2C1 SYSCFG_PMC_I2C1_FMP /*!< Enable Fast Mode Plus on I2C1 pins */
#   else
#      define I2C_FASTMODEPLUS_I2C1 \
         (uint32_t)(0x00000100U | I2C_FMP_NOT_SUPPORTED) /*!< Fast Mode Plus I2C1 not supported  */
#   endif
#   if defined(SYSCFG_PMC_I2C2_FMP)
#      define I2C_FASTMODEPLUS_I2C2 SYSCFG_PMC_I2C2_FMP /*!< Enable Fast Mode Plus on I2C2 pins */
#   else
#      define I2C_FASTMODEPLUS_I2C2 \
         (uint32_t)(0x00000200U | I2C_FMP_NOT_SUPPORTED) /*!< Fast Mode Plus I2C2 not supported  */
#   endif
#   if defined(SYSCFG_PMC_I2C3_FMP)
#      define I2C_FASTMODEPLUS_I2C3 SYSCFG_PMC_I2C3_FMP /*!< Enable Fast Mode Plus on I2C3 pins */
#   else
#      define I2C_FASTMODEPLUS_I2C3 \
         (uint32_t)(0x00000400U | I2C_FMP_NOT_SUPPORTED) /*!< Fast Mode Plus I2C3 not supported  */
#   endif
#   if defined(SYSCFG_PMC_I2C4_FMP)
#      define I2C_FASTMODEPLUS_I2C4 SYSCFG_PMC_I2C4_FMP /*!< Enable Fast Mode Plus on I2C4 pins */
#   else
#      define I2C_FASTMODEPLUS_I2C4 \
         (uint32_t)(0x00000800U | I2C_FMP_NOT_SUPPORTED) /*!< Fast Mode Plus I2C4 not supported  */
#   endif
/**
 * @}
 */

/**
 * @}
 */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup I2CEx_Exported_Functions I2C Extended Exported Functions
 * @{
 */

/** @addtogroup I2CEx_Exported_Functions_Group1 Extended features functions
 * @brief    Extended features functions
 * @{
 */

/* Peripheral Control functions  ************************************************/
HAL_StatusTypeDef HAL_I2CEx_ConfigAnalogFilter(I2C_HandleTypeDef* hi2c, uint32_t AnalogFilter);
HAL_StatusTypeDef HAL_I2CEx_ConfigDigitalFilter(I2C_HandleTypeDef* hi2c, uint32_t DigitalFilter);
#   if defined(SYSCFG_PMC_I2C1_FMP)
void HAL_I2CEx_EnableFastModePlus(uint32_t ConfigFastModePlus);
void HAL_I2CEx_DisableFastModePlus(uint32_t ConfigFastModePlus);
#   endif /* SYSCFG_PMC_I2C1_FMP */

/* Private constants ---------------------------------------------------------*/
/** @defgroup I2CEx_Private_Constants I2C Extended Private Constants
 * @{
 */

/**
 * @}
 */

/* Private macros ------------------------------------------------------------*/
/** @defgroup I2CEx_Private_Macro I2C Extended Private Macros
 * @{
 */
#   define IS_I2C_ANALOG_FILTER(FILTER) \
      (((FILTER) == I2C_ANALOGFILTER_ENABLE) || ((FILTER) == I2C_ANALOGFILTER_DISABLE))

#   define IS_I2C_DIGITAL_FILTER(FILTER) ((FILTER) <= 0x0000000FU)

#   if defined(SYSCFG_PMC_I2C1_FMP) && defined(SYSCFG_PMC_I2C2_FMP) && defined(SYSCFG_PMC_I2C3_FMP) && \
      defined(SYSCFG_PMC_I2C4_FMP)
#      define IS_I2C_FASTMODEPLUS(__CONFIG__) \
         ((((__CONFIG__)&I2C_FASTMODEPLUS_PB6) == I2C_FASTMODEPLUS_PB6) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB7) == I2C_FASTMODEPLUS_PB7) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB8) == I2C_FASTMODEPLUS_PB8) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB9) == I2C_FASTMODEPLUS_PB9) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C1) == I2C_FASTMODEPLUS_I2C1) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C2) == I2C_FASTMODEPLUS_I2C2) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C3) == I2C_FASTMODEPLUS_I2C3) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C4) == I2C_FASTMODEPLUS_I2C4))
#   elif defined(SYSCFG_PMC_I2C1_FMP) && defined(SYSCFG_PMC_I2C2_FMP) && defined(SYSCFG_PMC_I2C3_FMP)
#      define IS_I2C_FASTMODEPLUS(__CONFIG__) \
         ((((__CONFIG__)&I2C_FASTMODEPLUS_PB6) == I2C_FASTMODEPLUS_PB6) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB7) == I2C_FASTMODEPLUS_PB7) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB8) == I2C_FASTMODEPLUS_PB8) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB9) == I2C_FASTMODEPLUS_PB9) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C1) == I2C_FASTMODEPLUS_I2C1) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C2) == I2C_FASTMODEPLUS_I2C2) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C3) == I2C_FASTMODEPLUS_I2C3))
#   elif defined(SYSCFG_PMC_I2C1_FMP) && defined(SYSCFG_PMC_I2C2_FMP)
#      define IS_I2C_FASTMODEPLUS(__CONFIG__) \
         ((((__CONFIG__)&I2C_FASTMODEPLUS_PB6) == I2C_FASTMODEPLUS_PB6) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB7) == I2C_FASTMODEPLUS_PB7) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB8) == I2C_FASTMODEPLUS_PB8) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB9) == I2C_FASTMODEPLUS_PB9) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C1) == I2C_FASTMODEPLUS_I2C1) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C2) == I2C_FASTMODEPLUS_I2C2))
#   elif defined(SYSCFG_PMC_I2C1_FMP)
#      define IS_I2C_FASTMODEPLUS(__CONFIG__) \
         ((((__CONFIG__)&I2C_FASTMODEPLUS_PB6) == I2C_FASTMODEPLUS_PB6) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB7) == I2C_FASTMODEPLUS_PB7) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB8) == I2C_FASTMODEPLUS_PB8) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_PB9) == I2C_FASTMODEPLUS_PB9) || \
          (((__CONFIG__)&I2C_FASTMODEPLUS_I2C1) == I2C_FASTMODEPLUS_I2C1))
#   endif /* SYSCFG_PMC_I2C1_FMP && SYSCFG_PMC_I2C2_FMP && SYSCFG_PMC_I2C3_FMP && SYSCFG_PMC_I2C4_FMP */
/**
 * @}
 */

/* Private Functions ---------------------------------------------------------*/
/** @defgroup I2CEx_Private_Functions I2C Extended Private Functions
 * @{
 */
/* Private functions are defined in stm32f7xx_hal_i2c_ex.c file */
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

#   ifdef __cplusplus
}
#   endif

#endif /* __STM32F7xx_HAL_I2C_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
