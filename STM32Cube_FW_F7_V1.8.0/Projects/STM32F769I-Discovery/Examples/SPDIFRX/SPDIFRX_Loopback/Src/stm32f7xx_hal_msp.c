/**
 ******************************************************************************
 * @file    SPDIFRX/SPDIFRX_Loopback/Src/main.c
 * @author  MCD Application Team
 * @brief   HAL MSP module.
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

/** @addtogroup STM32F7xx_HAL_Examples
 * @{
 */

/** @defgroup SPDIFRX_Loopback
 * @brief HAL MSP module.
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
extern DMA_HandleTypeDef hSaiDma;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
 * @{
 */

/**
 * @brief  SAI MSP Init.
 * @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
 *                the configuration information for SAI module.
 * @retval None
 */
void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai)
{
   GPIO_InitTypeDef GPIO_Init;

   /* Enable SAI2 clock */
   __HAL_RCC_SAI2_CLK_ENABLE();

   /* Configure GPIOs used for SAI2 */
   __HAL_RCC_GPIOD_CLK_ENABLE();

   GPIO_Init.Mode = GPIO_MODE_AF_PP;
   GPIO_Init.Pull = GPIO_NOPULL;
   GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_Init.Alternate = GPIO_AF10_SAI2;
   GPIO_Init.Pin = GPIO_PIN_11;
   HAL_GPIO_Init(GPIOD, &GPIO_Init);

   /* Configure DMA used for SAI2 */
   __HAL_RCC_DMA2_CLK_ENABLE();

   if (hsai->Instance == SAI2_Block_A)
   {
      hSaiDma.Init.Channel = DMA_CHANNEL_3;
      hSaiDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
      hSaiDma.Init.PeriphInc = DMA_PINC_DISABLE;
      hSaiDma.Init.MemInc = DMA_MINC_ENABLE;
      hSaiDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      hSaiDma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
      hSaiDma.Init.Mode = DMA_CIRCULAR;
      hSaiDma.Init.Priority = DMA_PRIORITY_HIGH;
      hSaiDma.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
      hSaiDma.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
      hSaiDma.Init.MemBurst = DMA_MBURST_SINGLE;
      hSaiDma.Init.PeriphBurst = DMA_PBURST_SINGLE;

      /* Select the DMA instance to be used for the transfer : DMA2_Stream4 */
      hSaiDma.Instance = DMA2_Stream4;

      /* Associate the DMA handle */
      __HAL_LINKDMA(hsai, hdmatx, hSaiDma);

      /* Deinitialize the Stream for new transfer */
      HAL_DMA_DeInit(&hSaiDma);

      /* Configure the DMA Stream */
      if (HAL_OK != HAL_DMA_Init(&hSaiDma))
      {
         Error_Handler();
      }
   }

   HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0x01, 0);
   HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);

   PeriphClkInitStruct.PLLSAI.PLLSAIN = 344;
   PeriphClkInitStruct.PLLSAI.PLLSAIQ = 7;
   PeriphClkInitStruct.PLLSAIDivQ = 1;
   PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
   PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;
   HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
   __HAL_RCC_SAI2_CLK_ENABLE();
   HAL_NVIC_SetPriority(SAI2_IRQn, 0x01, 0);
   HAL_NVIC_EnableIRQ(SAI2_IRQn);
}

/**
 * @brief SPDIFRX MSP Init
 * @param hspdif: SPDIFRX handle
 * @retval None
 */

void HAL_SPDIFRX_MspInit(SPDIFRX_HandleTypeDef* hspdif)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   RCC_PeriphCLKInitTypeDef rcc_clkex;

   __HAL_RCC_SPDIFRX_CLK_ENABLE();

   rcc_clkex.PeriphClockSelection = RCC_PERIPHCLK_SPDIFRX;
   rcc_clkex.I2sClockSelection = RCC_I2SCLKSOURCE_PLLI2S;
   rcc_clkex.PLLI2S.PLLI2SN = 400;
   rcc_clkex.PLLI2S.PLLI2SP = RCC_PLLI2SP_DIV4;
   HAL_RCCEx_PeriphCLKConfig(&rcc_clkex);

   /* Enable and set SPDIF Interrupt */
   HAL_NVIC_SetPriority(SPDIF_RX_IRQn, 0, 1);
   HAL_NVIC_EnableIRQ(SPDIF_RX_IRQn);

   /* GPIOs Configuration */
   /* RX1   <->   PG12 */

   /*configure SPDIFRX_IN1 PG12 pin */

   /* Enable SPDIF GPIO IN */
   __HAL_RCC_GPIOG_CLK_ENABLE();

   GPIO_InitStructure.Pin = GPIO_PIN_12;
   GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStructure.Pull = GPIO_NOPULL;
   GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStructure.Alternate = GPIO_AF7_SPDIFRX;
   HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
}

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
