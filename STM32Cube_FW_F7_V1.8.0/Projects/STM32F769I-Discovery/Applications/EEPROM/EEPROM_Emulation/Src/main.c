/**
 ******************************************************************************
 * @file    EEPROM_Emulation/src/main.c
 * @author  MCD Application Team
 * @brief   Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/** @addtogroup EEPROM_Emulation
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FLASH_OBProgramInitTypeDef OBInit;

/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NB_OF_VAR] = { 0x5555, 0x6666, 0x7777 };
uint16_t VarDataTab[NB_OF_VAR] = { 0, 0, 0 };
uint16_t VarValue, VarDataTmp = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void ConfigError_Handler(void);
static void Error_Handler(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
   /* Configure the MPU attributes as Write Through */
   MPU_Config();

   /* Enable the CPU Cache */
   CPU_CACHE_Enable();

   /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   HAL_Init();

   /* Configure the system clock to have a system clock = 216 Mhz */
   SystemClock_Config();

   /* Unlock the Flash Program Erase controller */
   HAL_FLASH_Unlock();

   /* Configure LED1 and LED2 */
   BSP_LED_Init(LED1);
   BSP_LED_Init(LED2);

   /* Get FLASH bank confiuration (single or dual) */
   HAL_FLASHEx_OBGetConfig(&OBInit);
#ifdef USE_DUAL_BANK_FLASH
   if ((OBInit.USERConfig & OB_NDBANK_SINGLE_BANK) != OB_NDBANK_DUAL_BANK)
   {
      ConfigError_Handler();
   }
#else /* Use a Single Bank Flash */
   if ((OBInit.USERConfig & OB_NDBANK_SINGLE_BANK) != OB_NDBANK_SINGLE_BANK)
   {
      ConfigError_Handler();
   }
#endif /* USE_DUAL_BANK_FLASH */

   /* EEPROM Init */
   if (EE_Init() != EE_OK)
   {
      Error_Handler();
   }

   /* --- Store successively many values of the three variables in the EEPROM ---*/
   /* Store 0x1000 values of Variable1 in EEPROM */
   for (VarValue = 1; VarValue <= 0x1000; VarValue++)
   {
      /* Sequence 1 */
      if ((EE_WriteVariable(VirtAddVarTab[0], VarValue)) != HAL_OK)
      {
         Error_Handler();
      }
      if ((EE_ReadVariable(VirtAddVarTab[0], &VarDataTab[0])) != HAL_OK)
      {
         Error_Handler();
      }
      if (VarValue != VarDataTab[0])
      {
         Error_Handler();
      }

      /* Sequence 2 */
      if (EE_WriteVariable(VirtAddVarTab[1], ~VarValue) != HAL_OK)
      {
         Error_Handler();
      }
      if (EE_ReadVariable(VirtAddVarTab[1], &VarDataTab[1]) != HAL_OK)
      {
         Error_Handler();
      }
      if (((uint16_t)~VarValue) != VarDataTab[1])
      {
         Error_Handler();
      }

      /* Sequence 3 */
      if (EE_WriteVariable(VirtAddVarTab[2], VarValue << 1) != HAL_OK)
      {
         Error_Handler();
      }
      if (EE_ReadVariable(VirtAddVarTab[2], &VarDataTab[2]) != HAL_OK)
      {
         Error_Handler();
      }
      if ((VarValue << 1) != VarDataTab[2])
      {
         Error_Handler();
      }
   }

   /* Store 0x2000 values of Variable2 in EEPROM */
   for (VarValue = 1; VarValue <= 0x2000; VarValue++)
   {
      if (EE_WriteVariable(VirtAddVarTab[1], VarValue) != HAL_OK)
      {
         Error_Handler();
      }
      if (EE_ReadVariable(VirtAddVarTab[1], &VarDataTab[1]) != HAL_OK)
      {
         Error_Handler();
      }
      if (VarValue != VarDataTab[1])
      {
         Error_Handler();
      }
   }

   /* read the last stored variables data*/
   if (EE_ReadVariable(VirtAddVarTab[0], &VarDataTmp) != HAL_OK)
   {
      Error_Handler();
   }
   if (VarDataTmp != VarDataTab[0])
   {
      Error_Handler();
   }

   if (EE_ReadVariable(VirtAddVarTab[1], &VarDataTmp) != HAL_OK)
   {
      Error_Handler();
   }
   if (VarDataTmp != VarDataTab[1])
   {
      Error_Handler();
   }

   if (EE_ReadVariable(VirtAddVarTab[2], &VarDataTmp) != HAL_OK)
   {
      Error_Handler();
   }
   if (VarDataTmp != VarDataTab[2])
   {
      Error_Handler();
   }

   /* Store 0x3000 values of Variable3 in EEPROM */
   for (VarValue = 1; VarValue <= 0x3000; VarValue++)
   {
      if (EE_WriteVariable(VirtAddVarTab[2], VarValue) != HAL_OK)
      {
         Error_Handler();
      }
      if (EE_ReadVariable(VirtAddVarTab[2], &VarDataTab[2]) != HAL_OK)
      {
         Error_Handler();
      }
      if (VarValue != VarDataTab[2])
      {
         Error_Handler();
      }
   }

   /* read the last stored variables data*/
   if (EE_ReadVariable(VirtAddVarTab[0], &VarDataTmp) != HAL_OK)
   {
      Error_Handler();
   }
   if (VarDataTmp != VarDataTab[0])
   {
      Error_Handler();
   }

   if (EE_ReadVariable(VirtAddVarTab[1], &VarDataTmp) != HAL_OK)
   {
      Error_Handler();
   }
   if (VarDataTmp != VarDataTab[1])
   {
      Error_Handler();
   }

   if (EE_ReadVariable(VirtAddVarTab[2], &VarDataTmp) != HAL_OK)
   {
      Error_Handler();
   }
   if (VarDataTmp != VarDataTab[2])
   {
      Error_Handler();
   }

   while (1)
   {
      /* Turn ON LED2 */
      BSP_LED_On(LED2);
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
   HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
}

/**
 * @brief  This function is executed in case of Flash configuration error.
 * @param  None
 * @retval None
 */
static void ConfigError_Handler(void)
{
   while (1)
   {
      /* Toggle LED1 every 1 second */
      BSP_LED_Toggle(LED1);
      HAL_Delay(1000);
   }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void)
{
   /* Turn ON LED1 */
   BSP_LED_On(LED1);
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
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
 * @brief  Configure the MPU attributes as Write Through for SRAM1/2.
 * @note   The Base Address is 0x20010000 since this memory interface is the AXI.
 *         The Region Size is 256KB, it is related to SRAM1 and SRAM2  memory size.
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

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
