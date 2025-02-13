/**
 ******************************************************************************
 * @file    SPI/SPI_HalfDuplex_ComPollingIT/Src/main.c
 * @author  MCD Application Team
 * @brief   This sample code shows how to use STM32F7xx SPI HAL API to transmit
 *          and receive a data buffer with a communication process based on
 *          Interrupt transfer.
 *          The communication is done using 2 Boards.
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

/** @addtogroup STM32F7xx_MIX_Examples
 * @{
 */

/** @addtogroup SPI_FullDuplex_ComIT
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
enum
{
   TRANSFER_WAIT,
   TRANSFER_COMPLETE,
   TRANSFER_ERROR
};

/* Private macro -------------------------------------------------------------*/
/* Uncomment this line to use the board as master, if not it is used as slave */
//#define MASTER_BOARD

/* Private variables ---------------------------------------------------------*/
/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = "**** SPI - Two Boards communication based on Polling (LL driver) for Master Board and Interrupt "
                      "(HAL Driver) for Slave Board ****";
#ifdef MASTER_BOARD
__IO uint8_t ubNbDataToTransmit = BUFFERSIZE;
__IO uint8_t ubNbDataTransmitted = 0;
#endif

/* Buffer used for reception */
uint8_t aRxBuffer[BUFFERSIZE];

/* transfer state */
__IO uint32_t wTransferState = TRANSFER_WAIT;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
#ifndef MASTER_BOARD
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
#endif
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
   /* Enable the CPU Cache */
   CPU_CACHE_Enable();

   /* STM32F7xx HAL library initialization:
        - Configure the Flash prefetch
        - Systick timer is configured by default as source of time base, but user
          can eventually implement his proper time base source (a general purpose
          timer for example or other time source), keeping in mind that Time base
          duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
          handled in milliseconds basis.
        - Set NVIC Group Priority to 4
        - Low Level Initialization
      */
   HAL_Init();

   /* Configure the system clock to 216 MHz */
   SystemClock_Config();

   /* Configure LED1, LED2 and LED3 */
   BSP_LED_Init(LED1);
   BSP_LED_Init(LED2);
   BSP_LED_Init(LED3);

   /*##-1- Configure the SPI peripheral #######################################*/
   /* Set the SPI parameters */
   SpiHandle.Instance = SPIx;
   SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
   SpiHandle.Init.Direction = SPI_DIRECTION_1LINE;
   SpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
   SpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
   SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
   SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
   SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
   SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
   SpiHandle.Init.CRCPolynomial = 7;
   SpiHandle.Init.NSS = SPI_NSS_SOFT;

#ifdef MASTER_BOARD
   SpiHandle.Init.Mode = SPI_MODE_MASTER;
#else
   SpiHandle.Init.Mode = SPI_MODE_SLAVE;
#endif /* MASTER_BOARD */

   if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
   {
      /* Initialization Error */
      Error_Handler();
   }

#ifdef MASTER_BOARD
   /* Configure User push-button */
   BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
   /* Wait for User push-button press before starting the Communication */
   while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
   {
      BSP_LED_Toggle(LED1);
      HAL_Delay(100);
   }
   BSP_LED_Off(LED1);

   /*##-2- Start the Half Duplex Communication process ########################*/
   /* Half Duplex Direction (Tx) not Done by HAL_Init. */
   LL_SPI_SetTransferDirection(SpiHandle.Instance, LL_SPI_HALF_DUPLEX_TX);

   /* Enable SPI before start transmission */
   LL_SPI_Enable(SpiHandle.Instance);

   while (ubNbDataToTransmit > 0)
   {
      /* Check TXE flag to transmit data */
      if (LL_SPI_IsActiveFlag_TXE(SpiHandle.Instance))
      {
         /* Transmit 8bit Data */
         LL_SPI_TransmitData8(SpiHandle.Instance, aTxBuffer[ubNbDataTransmitted++]);
         ubNbDataToTransmit--;
      }
   }

   /* Wait End Of Transmission: TXE set and Tx Fifo empty */
   while ((LL_SPI_IsActiveFlag_TXE(SpiHandle.Instance) != 1))
      ;
   while (LL_SPI_GetTxFIFOLevel(SpiHandle.Instance) != LL_SPI_TX_FIFO_EMPTY)
      ;

   /* Disable SPI after End of Transmission */
   LL_SPI_Disable(SpiHandle.Instance);

#else /* Slave Board */

   /*##-2- Start the Half Duplex Communication process ########################*/
   /* While the SPI in Receive process, user can receive data through "aRxBuffer" */
   if (HAL_SPI_Receive_IT(&SpiHandle, (uint8_t*)aRxBuffer, BUFFERSIZE) != HAL_OK)
   {
      /* Transfer error in transmission process */
      Error_Handler();
   }

   /*##-3- Wait for the end of the transfer ###################################*/
   /*  Before starting a new communication transfer, you must wait the callback call
       to get the transfer complete confirmation or an error detection.
       For simplicity reasons, this example is just waiting till the end of the
       transfer, but application may perform other tasks while transfer operation
       is ongoing. */
   while (wTransferState == TRANSFER_WAIT)
   {
   }

   switch (wTransferState)
   {
   case TRANSFER_COMPLETE:
      /*##-4- Compare the sent and received buffers ##############################*/
      if (Buffercmp((uint8_t*)aTxBuffer, (uint8_t*)aRxBuffer, BUFFERSIZE))
      {
         /* Processing Error */
         Error_Handler();
      }
      else
      {
         BSP_LED_On(LED2);
      }
      break;
   default:
      Error_Handler();
      break;
   }
#endif /* MASTER_BOARD */

   /* Infinite loop */
   while (1)
   {
   }
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
 *            HSE Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 432
 *            PLL_P                          = 2
 *            PLL_Q                          = 9
 *            PLL_R                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 7
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void)
{
   RCC_ClkInitTypeDef RCC_ClkInitStruct;
   RCC_OscInitTypeDef RCC_OscInitStruct;

   /* Enable HSE Oscillator and activate PLL with HSE as source */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
   RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 8;
   RCC_OscInitStruct.PLL.PLLN = 432;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 9;
   RCC_OscInitStruct.PLL.PLLR = 7;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      while (1)
      {
      };
   }

   /* Activate the OverDrive to reach the 216 Mhz Frequency */
   if (HAL_PWREx_EnableOverDrive() != HAL_OK)
   {
      while (1)
      {
      };
   }

   /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
      clocks dividers */
   RCC_ClkInitStruct.ClockType =
      (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
   {
      while (1)
      {
      };
   }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void)
{
   /* Turn LED3 on */
   BSP_LED_On(LED3);
   while (1)
   {
   }
}

#ifndef MASTER_BOARD
/**
 * @brief  Compares two buffers.
 * @param  pBuffer1, pBuffer2: buffers to be compared.
 * @param  BufferLength: buffer's length
 * @retval 0  : pBuffer1 identical to pBuffer2
 *         >0 : pBuffer1 differs from pBuffer2
 */
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
   while (BufferLength--)
   {
      if ((*pBuffer1) != *pBuffer2)
      {
         return BufferLength;
      }
      pBuffer1++;
      pBuffer2++;
   }

   return 0;
}
#endif

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
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

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

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/

#ifndef MASTER_BOARD
/**
 * @brief  Tx Transfer completed callback.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report end of Interrupt Tx transfer, and
 *         you can add your own implementation.
 * @retval None
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi)
{
   /* Transfer in reception process is complete */
   wTransferState = TRANSFER_COMPLETE;
}

/**
 * @brief  SPI error callbacks.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report transfer error, and you can
 *         add your own implementation.
 * @retval None
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi)
{
   wTransferState = TRANSFER_ERROR;
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
