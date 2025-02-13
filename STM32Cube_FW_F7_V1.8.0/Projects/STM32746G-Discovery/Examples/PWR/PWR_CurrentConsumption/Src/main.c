/**
 ******************************************************************************
 * @file    PWR/PWR_CurrentConsumption/Src/main.c
 * @author  MCD Application Team
 * @brief   This sample code shows how to use STM32F7xx PWR HAL API to enter
 * and exit the stop mode.
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

/** @addtogroup PWR_CurrentConsumption
 * @{
 */
/* Private function prototypes -----------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t UserButtonStatus = 0;
/* UART handler declaration */
UART_HandleTypeDef UartHandle;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
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

   /* Configure LED1 */
   BSP_LED_Init(LED1);

   /* Turn on LED1 */
   BSP_LED_On(LED1);

   /* STM32F7xx HAL library initialization:
        - Configure the Flash prefetch, instruction and Data caches
        - Configure the Systick to generate an interrupt each 1 msec
        - Set NVIC Group Priority to 4
        - Global MSP (MCU Support Package) initialization
      */
   HAL_Init();

   /* Configure the system clock to 216 MHz */
   SystemClock_Config();

   /* Enable Power Clock */
   __HAL_RCC_PWR_CLK_ENABLE();

   /* Check and handle if the system was resumed from StandBy mode */
   if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
   {
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

      /* Exit Ethernet Phy from low power mode */
      ETH_PhyExitFromPowerDownMode();

      /* Exit USB Phy from low power mode */
      USB_PhyExitFromLowPowerMode();
   }

   /* Infinite loop */
   while (1)
   {
      /* Configure USER Button */
      BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

      UserButtonStatus = 0;

      /* Wait until USER button is pressed to enter the Low Power mode */
      while (UserButtonStatus == 0x00)
      {
         /* Toggle LED1 */
         BSP_LED_Toggle(LED1);

         /* Inserted Delay */
         HAL_Delay(200);
      }

      /* Loop while User button is maintained pressed */
      while (BSP_PB_GetState(BUTTON_KEY) != RESET)
      {
      }

      /* Ethernet PHY and USB PHY must be in low power mode in order to have the lowest current consumption */
      /* Enter USB PHY to Lowpower Mode */
      USB_PhyEnterLowPowerMode();

      /* Enter Ethernet PHY to Power Down Mode */
      ETH_PhyEnterPowerDownMode();

#if defined(SLEEP_MODE)
      /* Sleep Mode Entry
      - System Running at PLL (216MHz)
      - Flash 5 wait state
      - Instruction and Data caches ON
      - Prefetch ON
      - Code running from Internal FLASH
      - All peripherals disabled.
      - Wake-up using EXTI Line (USER Button)
      */
      SleepMode_Measure();
#elif defined(STOP_MODE)
      /* STOP Mode Entry
      - RTC Clocked by LSI
      - Regulator in LP mode
      - HSI, HSE OFF and LSI OFF if not used as RTC Clock source
      - No IWDG
      - FLASH in deep power down mode
      - Automatic Wake-up using RTC clocked by LSI (after ~20s)
      */
      StopMode_Measure();

#elif defined(STANDBY_MODE)
      /* STANDBY Mode Entry
      - Backup SRAM and RTC OFF
      - IWDG and LSI OFF
      - Wake-up using WakeUp Pin (PI.11)
      */
      StandbyMode_Measure();

#elif defined(STANDBY_RTC_MODE)
      /* STANDBY Mode with RTC on LSI Entry
      - RTC Clocked by LSI
      - IWDG OFF and LSI OFF if not used as RTC Clock source
      - Backup SRAM OFF
      - Automatic Wake-up using RTC clocked by LSI (after ~20s)
      */
      StandbyRTCMode_Measure();

#elif defined(STANDBY_BKPSRAM_MODE)
      /* STANDBY Mode Entry
      - Backup SRAM ON
      - IWDG OFF
      - Wake-up using WakeUp Pin (PI.11)
      */
      StandbyBKPSRAMMode_Measure();
#endif

      /* Configure LED1 */
      BSP_LED_Init(LED1);

      /* Turn LED1 On */
      BSP_LED_On(LED1);

      /* Let LED1 On for 2 seconds */
      HAL_Delay(2000);
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

   /* Enable HSE Oscillator and activate PLL with HSE as source */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 25;
   RCC_OscInitStruct.PLL.PLLN = 432;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 9;

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
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
   /* LED1 blinks rapidly */
   while (1)
   {
      BSP_LED_Toggle(LED1);
      HAL_Delay(50);
   }
}

/**
 * @brief SYSTICK callback
 * @param None
 * @retval None
 */
void HAL_SYSTICK_Callback(void)
{
   HAL_IncTick();
}

/**
 * @brief  Wake Up Timer callback
 * @param  hrtc: RTC handle
 * @retval None
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef* hrtc)
{
   /* NOTE : add the specific code to handle the RTC wake up interrupt */
   /* Configure LED1 */
   BSP_LED_Init(LED1);

   /* Turn LED1 On */
   BSP_LED_On(LED1);
}

/**
 * @brief  EXTI line detection callbacks
 * @param  GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
   if (GPIO_Pin == KEY_BUTTON_PIN)
   {
      /* Configure LED1 */
      BSP_LED_Init(LED1);

      /* Turn LED1 On */
      BSP_LED_On(LED1);
   }
   UserButtonStatus = 1;
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
