/**
 ******************************************************************************
 * @file    USB_Host/MTP_Standalone/Src/main.c
 * @author  MCD Application Team
 * @brief   USB host MTP demo main file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBH_HandleTypeDef hUSBHost;
MTP_ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void CPU_CACHE_Enable(void);
static void MTP_InitApplication(void);
static void USBH_UserProcess(USBH_HandleTypeDef* phost, uint8_t id);

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
        - Configure the Flash ART accelerator on ITCM interface
        - Configure the Systick to generate an interrupt each 1 msec
        - Set NVIC Group Priority to 4
        - Low Level Initialization
      */
   HAL_Init();

   /* Configure the System clock to have a frequency of 200 Mhz */
   SystemClock_Config();

   /* Initialize IO expander */
   BSP_IO_Init();

   /* Init MTP Application */
   MTP_InitApplication();

   /* Init Host Library */
   USBH_Init(&hUSBHost, USBH_UserProcess, 0);

   /* Add Supported Class */
   USBH_RegisterClass(&hUSBHost, USBH_MTP_CLASS);

   /* Start Host Process */
   USBH_Start(&hUSBHost);

   /* Run Application (Blocking mode) */
   while (1)
   {
      /* USB Host Background task */
      USBH_Process(&hUSBHost);

      /* MTP Menu Process */
      MTP_MenuProcess();
   }
}

/**
 * @brief  MTP application Init.
 * @param  None
 * @retval None
 */
static void MTP_InitApplication(void)
{
   /* Configure Key Button */
   BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

   /* Configure Joystick in EXTI mode */
   BSP_JOY_Init(JOY_MODE_EXTI);

   /* Configure LED1 and LED3 */
   BSP_LED_Init(LED1);
   BSP_LED_Init(LED3);

   /* Initialize the LCD */
   BSP_LCD_Init();

   /* LCD Layer Initialization */
   BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

   /* Selects the LCD Layer */
   BSP_LCD_SelectLayer(1);

   /* Enables the display */
   BSP_LCD_DisplayOn();

   /* Init the LCD Log module */
   LCD_LOG_Init();

#ifdef USE_USB_HS
   LCD_LOG_SetHeader((uint8_t*)" USB OTG HS MTP Host");
#else
   LCD_LOG_SetHeader((uint8_t*)" USB OTG FS MTP Host");
#endif

   LCD_UsrLog("USB Host library started.\n");

   /* Start MTP Interface */
   USBH_UsrLog("Starting MTP Demo");

   /* Init Audio interface */
   AUDIO_Init();

   /* Start MTP Interface */
   MTP_MenuInit();
}

/**
 * @brief  User Process
 * @param  phost: Host Handle
 * @param  id: Host Library user message ID
 * @retval None
 */
static void USBH_UserProcess(USBH_HandleTypeDef* phost, uint8_t id)
{
   switch (id)
   {
   case HOST_USER_SELECT_CONFIGURATION:
      break;

   case HOST_USER_DISCONNECTION:
      Appli_state = APPLICATION_DISCONNECT;
      break;

   case HOST_USER_CLASS_ACTIVE:
      Appli_state = APPLICATION_READY;
      break;

   default:
      break;
   }
}

/**
 * @brief This function provides accurate delay (in milliseconds) based
 *        on SysTick counter flag.
 * @note This function is declared as __weak to be overwritten in case of other
 *       implementations in user file.
 * @param Delay: specifies the delay time length, in milliseconds.
 * @retval None
 */

void HAL_Delay(__IO uint32_t Delay)
{
   while (Delay)
   {
      if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
      {
         Delay--;
      }
   }
}

/**
 * @brief  Clock Config.
 * @param  hsai: might be required to set audio peripheral predivider if any.
 * @param  AudioFreq: Audio frequency used to play the audio stream.
 * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
 *         Being __weak it can be overwritten by the application
 * @retval None
 */
void BSP_AUDIO_OUT_ClockConfig(SAI_HandleTypeDef* hsai, uint32_t AudioFreq, void* Params)
{
   RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;

   HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

   /* Set the PLL configuration according to the audio frequency */
   if ((AudioFreq == AUDIO_FREQUENCY_11K) || (AudioFreq == AUDIO_FREQUENCY_22K) || (AudioFreq == AUDIO_FREQUENCY_44K))
   {
      /*
       * Configure PLLSAI prescalers
       * PLLI2S_VCO: VCO_429M
       * SAI_CLK(first level) = PLLI2S_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
       * SAI_CLK_x = SAI_CLK(first level)/PLLI2SDivQ = 214.5/19 = 11.289 Mhz
       */
      RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
      RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
      RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
      RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 429;
      RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 2;
      RCC_ExCLKInitStruct.PLLI2SDivQ = 19;
      HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
   }
   else /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_48K), AUDIO_FREQUENCY_96K */
   {
      /*
       * SAI clock config
       * PLLI2S_VCO: VCO_344M
       * SAI_CLK(first level) = PLLI2S_VCO/PLLSAIQ = 344/7 = 49.142 Mhz
       * SAI_CLK_x = SAI_CLK(first level)/PLLI2SDivQ = 49.142/1 = 49.142 Mhz
       */
      RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
      RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
      RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
      RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 344;
      RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 7;
      RCC_ExCLKInitStruct.PLLI2SDivQ = 1;
      HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
   }
}

/**
 * @brief  Toggles LEDs to show user input state.
 * @param  None
 * @retval None
 */
void Toggle_Leds(void)
{
   static uint32_t ticks;

   if (ticks++ == 100)
   {
      BSP_LED_Toggle(LED1);
      BSP_LED_Toggle(LED3);
      ticks = 0;
   }
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 200000000
 *            HCLK(Hz)                       = 200000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
 *            PLL_N                          = 400
 *            PLL_P                          = 2
 *            PLLSAI_N                       = 384
 *            PLLSAI_P                       = 8
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 7
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
   RCC_ClkInitTypeDef RCC_ClkInitStruct;
   RCC_OscInitTypeDef RCC_OscInitStruct;
   RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

   /* Enable HSE Oscillator and activate PLL with HSE as source */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 25;
   RCC_OscInitStruct.PLL.PLLN = 432;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 9;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      Error_Handler();
   }

   /* Activate the OverDrive to reach the 200 Mhz Frequency */
   if (HAL_PWREx_EnableOverDrive() != HAL_OK)
   {
      Error_Handler();
   }

   /* Select PLLSAI output as USB clock source */
   PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
   PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
   PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
   PeriphClkInitStruct.PLLSAI.PLLSAIQ = 7;
   PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
   if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
   {
      Error_Handler();
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
   /* User may add here some code to deal with this error */
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
