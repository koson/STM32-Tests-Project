/**
 ******************************************************************************
 * @file    LPTIM/LPTIM_PWMExternalClock/Inc/main.h
 * @author  MCD Application Team
 * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#   define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#   include "stm32f7xx_hal.h"
#   include "stm32f769i_eval.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Definition for LPTIMx clock resources */
#   define LPTIMx LPTIM1
#   define LPTIMx_CLK_ENABLE __HAL_RCC_LPTIM1_CLK_ENABLE

/* Definition for LPTIMx Pins */
#   define LPTIMx_GPIO_PORT_ENABLE __HAL_RCC_GPIOD_CLK_ENABLE();
#   define LPTIMx_OUT_GPIO_PORT GPIOD
#   define LPTIMx_IN1_GPIO_PORT GPIOD
#   define GPIO_PIN_OUT GPIO_PIN_13
#   define GPIO_PIN_IN1 GPIO_PIN_12
#   define GPIO_PUPD_OUT GPIO_PULLUP
#   define GPIO_PUPD_IN1 GPIO_PULLUP
#   define GPIO_AF_LPTIMx_OUT GPIO_AF3_LPTIM1
#   define GPIO_AF_LPTIMx_IN1 GPIO_AF3_LPTIM1
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
