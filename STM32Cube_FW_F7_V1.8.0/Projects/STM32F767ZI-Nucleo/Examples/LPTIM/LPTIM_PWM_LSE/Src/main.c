/**
 ******************************************************************************
 * @file    LPTIM/LPTIM_PWM_LSE/Src/main.c
 * @author  MCD Application Team
 * @brief   This example describes how to configure and use LPTIM to generate
 *          a PWM in stop mode using the LSE as a counter clock, through the
 *          STM32F7xx HAL API.
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

/** @addtogroup LPTIM_PWM_LSE
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Set the Maximum value of the counter (Auto-Reload) that defines the Period */
#define PeriodValue (uint32_t)(100 - 1)

/* Set the Compare value that defines the duty cycle */
#define PulseValue (uint32_t)(50 - 1)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* LPTIM handle declaration */
LPTIM_HandleTypeDef LptimHandle;

/* Clocks structure declaration */
RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static HAL_StatusTypeDef LSE_ClockEnable(void);
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

   /* Configure LED3 */
   BSP_LED_Init(LED3);

   /* User push-button (EXTI_Line15_10) will be used to wakeup the system from Stop mode */
   BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

   /* Enable the LSE Clock */
   if (LSE_ClockEnable() != HAL_OK)
   {
      Error_Handler();
   }

   /* ### - 1 - Re-target the LSE to Clock the LPTIM Counter ################# */
   /* Select the LSE clock as LPTIM peripheral clock */
   RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
   RCC_PeriphCLKInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSE;
   HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

   /* ### - 2 - Initialize the LPTIM peripheral ############################## */
   /*
    *  Instance        = LPTIM1
    *  Clock Source    = APB or LowPowerOSCillator (in this example LSE is
    *                    already selected from the RCC stage)
    *  Counter source  = External event.
    *  Clock prescaler = 1 (No division)
    *  Counter Trigger = Software trigger
    *  Output Polarity = High
    *  Update mode     = Immediate (Registers are immediately updated after any
    *                    write access)
    */

   LptimHandle.Instance = LPTIM1;

   LptimHandle.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
   LptimHandle.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
   LptimHandle.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
   LptimHandle.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
   LptimHandle.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
   LptimHandle.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;

   /* Initialize LPTIM peripheral according to the passed parameters */
   if (HAL_LPTIM_Init(&LptimHandle) != HAL_OK)
   {
      Error_Handler();
   }

   /* ### - 3 - Start counting in interrupt mode ############################# */
   /*
    *  Period = 99
    *  Pulse  = 49
    *  According to this configuration, the duty cycle will be equal to 50%
    */
   if (HAL_LPTIM_PWM_Start(&LptimHandle, PeriodValue, PulseValue) != HAL_OK)
   {
      Error_Handler();
   }

   /* ### - 4 - Enter in Stop mode ########################################### */
   HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

   /* ### - 5 - Stop counting when leaving Stop mode ########################## */
   if (HAL_LPTIM_PWM_Stop(&LptimHandle) != HAL_OK)
   {
      Error_Handler();
   }

   /* Infinite Loop */
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
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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
 * @brief  Enable External Low Speed Clock (LSE)
 * @param  None
 * @retval Status
 */
static HAL_StatusTypeDef LSE_ClockEnable(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct;

   /* Enable LSE clock */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
   RCC_OscInitStruct.LSEState = RCC_LSE_ON;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
   return (HAL_RCC_OscConfig(&RCC_OscInitStruct));
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
