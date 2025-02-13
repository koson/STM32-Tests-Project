/**
 ******************************************************************************
 * @file    main.c
 * @author  MCD Application Team
 * @brief   Main program
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
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

#include <string.h>
#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"

/* FreeRTOS buffer for static allocation */

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
#endif

static void SystemClock_Config(void);
static void RNG_Init(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);
static void MainThread(void const* argument);

RNG_HandleTypeDef RngHandle;

int main()
{
   /* Configure the MPU attributes as Write Through */
   MPU_Config();

   /* Enable the CPU Cache */
   CPU_CACHE_Enable();

   HAL_Init();

   /* Configure the system clock to 200 MHz */
   SystemClock_Config();

   /* Initialize BSP Led for LED1 and LED2 */
   BSP_LED_Init(LED_RED);
   BSP_LED_Init(LED_GREEN);

   RNG_Init();

#ifdef USE_LCD

   /* Initialize the LCD */
   BSP_LCD_Init();

   /* Initialize the LCD Layers */
   BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

   /* Set LCD Foreground Layer  */
   BSP_LCD_SelectLayer(1);
   LCD_LOG_Init();
   LCD_LOG_SetHeader("mbedTLS: SSL Server Application");

#endif
   /* Init thread */
   osThreadDef(Start, MainThread, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 2);
   osThreadCreate(osThread(Start), NULL);

   /* Start scheduler */
   osKernelStart();

   /* We should never get here as control is now taken by the scheduler */
   Error_Handler();
}

/**
 * @brief  Start Thread
 * @param  argument not used
 * @retval None
 */
static void MainThread(void const* argument)
{
   UNUSED(argument);
#ifdef USE_LCD
   LCD_UsrLog("\r\n Starting Main Thread...\n");
#endif

   /* Start SSL Client task : Connect to SSL server and provide the SSL handshake protocol */
   osThreadDef(Server, SSL_Server, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 20);
   osThreadCreate(osThread(Server), NULL);

   for (;;)
   {
      /* Delete the start Thread */
      osThreadTerminate(NULL);
   }
}

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
   RCC_OscInitStruct.PLL.PLLN = 400;
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

/* RNG init function */
static void RNG_Init(void)
{
   RngHandle.Instance = RNG;
   /* DeInitialize the RNG peripheral */
   if (HAL_RNG_DeInit(&RngHandle) != HAL_OK)
   {
      /* DeInitialization Error */
      Error_Handler();
   }

   /* Initialize the RNG peripheral */
   if (HAL_RNG_Init(&RngHandle) != HAL_OK)
   {
      /* Initialization Error */
      Error_Handler();
   }
}

/**
 * @brief  Configure the MPU attributes as Write Through for Internal SRAM1/2.
 * @note   The Base Address is 0x20020000 since this memory interface is the AXI.
 *         The Configured Region Size is 512KB because the internal SRAM1/2
 *         memory size is 384KB.
 * @param  None
 * @retval None
 */

static void MPU_Config(void)
{
   MPU_Region_InitTypeDef MPU_InitStruct;

   /* Disable the MPU */
   HAL_MPU_Disable();

   /* Configure the MPU attributes as WT for SRAM */
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress = 0x20020000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable = 0x00;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   /* Enable the MPU */
   HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
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

void Error_Handler(void)
{
   while (1)
   {
      BSP_LED_Toggle(LED_RED);
      osDelay(100);
   }
}
