/**
 ******************************************************************************
 * @file    PWR/PWR_STOP_RTC/Src/main.c
 * @author  MCD Application Team
 * @brief   This sample code shows how to use STM32F7xx PWR HAL API to enter
 *          and exit the stop mode with RTC.
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

/** @addtogroup PWR_STOP_RTC
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* RTC handler declaration */
RTC_HandleTypeDef RTCHandle;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void SYSCLKConfig_STOP(void);
static void RTC_Config(void);
static void Error_Handler(void);
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

   /* Configure LED1 */
   BSP_LED_Init(LED1);

   /* Configure the RTC */
   RTC_Config();

   while (1)
   {
      /* Turn LED1 on */
      BSP_LED_On(LED1);

      /* Insert 5 second delay */
      HAL_Delay(5000);

      /* Disable Wakeup Counter */
      HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);

      /* ## Setting the Wake up time ############################################*/
      /*  RTC Wakeup Interrupt Generation:
        Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI))
        Wakeup Time = Wakeup Time Base * WakeUpCounter
        = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI)) * WakeUpCounter
          ==> WakeUpCounter = Wakeup Time / Wakeup Time Base
      
        To configure the wake up timer to 20s the WakeUpCounter is set to 0x1FFF:
        RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
        Wakeup Time Base = 16 /(~39.000KHz) = ~0,410 ms
        Wakeup Time = ~20s = 0,410ms  * WakeUpCounter
          ==> WakeUpCounter = ~20s/0,410ms = 48780 = 0xBE8C */
      HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0xBE8C, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

      /* Turn LED1 OFF  */
      BSP_LED_Off(LED1);

      /* Enter Stop Mode */
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
      /* Configures system clock after wake-up from STOP: enable HSE, PLL and select
      PLL as system clock source (HSE and PLL are disabled in STOP mode) */
      SYSCLKConfig_STOP();
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
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
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
   HAL_StatusTypeDef ret = HAL_OK;

   /* Enable Power Control clock */
   __HAL_RCC_PWR_CLK_ENABLE();

   /* The voltage scaling allows optimizing the power consumption when the device is
      clocked below the maximum system frequency, to update the voltage scaling value
      regarding system frequency refer to product datasheet.  */
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

   /* Enable HSE Oscillator and activate PLL with HSE as source */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 25;
   RCC_OscInitStruct.PLL.PLLN = 432;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 9;
   RCC_OscInitStruct.PLL.PLLR = 7;

   ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
   if (ret != HAL_OK)
   {
      while (1)
      {
         ;
      }
   }

   /* Activate the OverDrive to reach the 216 MHz Frequency */
   ret = HAL_PWREx_EnableOverDrive();
   if (ret != HAL_OK)
   {
      while (1)
      {
         ;
      }
   }

   /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
   RCC_ClkInitStruct.ClockType =
      (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

   ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
   if (ret != HAL_OK)
   {
      while (1)
      {
         ;
      }
   }
}
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
 * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
 *         and select PLL as system clock source.
 * @param  None
 * @retval None
 */
static void SYSCLKConfig_STOP(void)
{
   RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
   RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
   uint32_t pFLatency = 0;

   /* Enable Power Control clock */
   __HAL_RCC_PWR_CLK_ENABLE();

   /* Get the Oscillators configuration according to the internal RCC registers */
   HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

   /* After wake-up from STOP reconfigure the system clock: Enable HSE and PLL */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLM = 25;
   RCC_OscInitStruct.PLL.PLLN = 432;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 9;
   RCC_OscInitStruct.PLL.PLLR = 7;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      Error_Handler();
   }

   /* Get the Clocks configuration according to the internal RCC registers */
   HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);

   /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
      clocks dividers */
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
   {
      Error_Handler();
   }
}

/**
 * @brief  RTC Configuration
 *         RTC Clocked by LSI (see HAL_RTC_MspInit)
 * @param  None
 * @retval None
 */
static void RTC_Config(void)
{
   /* Configure RTC */
   RTCHandle.Instance = RTC;
   /* Configure RTC prescaler and RTC data registers as follow:
     - Hour Format = Format 24
     - Asynch Prediv = Value according to source clock
     - Synch Prediv = Value according to source clock
     - OutPut = Output Disable
     - OutPutPolarity = High Polarity
     - OutPutType = Open Drain */
   RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
   RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
   RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
   RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
   RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
   RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
   if (HAL_RTC_Init(&RTCHandle) != HAL_OK)
   {
      /* Initialization Error */
      Error_Handler();
   }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void)
{
   while (1)
   {
   }
}

/**
 * @brief  RTC Wake Up callback
 * @param  None
 * @retval None
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef* hrtc)
{
   /* Clear Wake Up Flag */
   __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
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
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

   /* Infinite loop */
   while (1)
   {
   }
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
