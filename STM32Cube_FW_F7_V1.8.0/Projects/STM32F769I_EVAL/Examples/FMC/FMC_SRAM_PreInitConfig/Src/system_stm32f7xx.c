/**
 ******************************************************************************
 * @file    system_stm32f7xx.c
 * @author  MCD Application Team
 * @brief   CMSIS Cortex-M7 Device Peripheral Access Layer System Source File.
 *
 *   This file provides two functions and one global variable to be called from
 *   user application:
 *      - SystemInit(): This function is called at startup just after reset and
 *                      before branch to main program. This call is made inside
 *                      the "startup_stm32f7xx.s" file.
 *
 *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
 *                                  by the user application to setup the SysTick
 *                                  timer or configure other parameters.
 *
 *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
 *                                 be called whenever the core clock is changed
 *                                 during program execution.
 *
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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

/** @addtogroup CMSIS
 * @{
 */

/** @addtogroup stm32f7xx_system
 * @{
 */

/** @addtogroup STM32F7xx_System_Private_Includes
 * @{
 */

#include "stm32f7xx.h"

#if !defined(HSE_VALUE)
#   define HSE_VALUE ((uint32_t)25000000) /*!< Default value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined(HSI_VALUE)
#   define HSI_VALUE ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

/**
 * @}
 */

/** @addtogroup STM32F7xx_System_Private_TypesDefinitions
 * @{
 */

/**
 * @}
 */

/** @addtogroup STM32F7xx_System_Private_Defines
 * @{
 */

/************************* Miscellaneous Configuration ************************/
#define DATA_IN_ExtSRAM

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET \
   0x00 /*!< Vector Table base offset field. \ \
             This value must be a multiple of 0x200. */
/******************************************************************************/

/**
 * @}
 */

/** @addtogroup STM32F7xx_System_Private_Macros
 * @{
 */

/**
 * @}
 */

/** @addtogroup STM32F7xx_System_Private_Variables
 * @{
 */

/* This variable is updated in three ways:
    1) by calling CMSIS function SystemCoreClockUpdate()
    2) by calling HAL API function HAL_RCC_GetHCLKFreq()
    3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
       Note: If you use this function to configure the system clock; then there
             is no need to call the 2 first functions listed above, since SystemCoreClock
             variable is updated automatically.
*/

#if defined(__CC_ARM)
uint32_t SystemCoreClock __attribute__((section("NoInit"), zero_init)); /* Uninitialized Variable */

#elif defined(__ICCARM__)
__no_init uint32_t SystemCoreClock;

#elif defined(__GNUC__)
uint32_t SystemCoreClock = 16000000;
#endif

const uint8_t AHBPrescTable[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9 };
const uint8_t APBPrescTable[8] = { 0, 0, 0, 0, 1, 2, 3, 4 };

/**
 * @}
 */

/** @addtogroup STM32F7xx_System_Private_FunctionPrototypes
 * @{
 */
#if defined(DATA_IN_ExtSRAM)
static void SetSysClk(void);
static void SystemInit_ExtMemCtl(void);
#endif /* DATA_IN_ExtSRAM */

/**
 * @}
 */

/** @addtogroup STM32F7xx_System_Private_Functions
 * @{
 */

/**
 * @brief  Setup the microcontroller system
 *         Initialize the Embedded Flash Interface, the PLL and update the
 *         SystemFrequency variable.
 * @param  None
 * @retval None
 */
void SystemInit(void)
{
   SystemCoreClock = 16000000;

/* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
   SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
#endif
   /* Reset the RCC clock configuration to the default reset state ------------*/
   /* Set HSION bit */
   RCC->CR |= (uint32_t)0x00000001;

   /* Reset CFGR register */
   RCC->CFGR = 0x00000000;

   /* Reset HSEON, CSSON and PLLON bits */
   RCC->CR &= (uint32_t)0xFEF6FFFF;

   /* Reset PLLCFGR register */
   RCC->PLLCFGR = 0x24003010;

   /* Reset HSEBYP bit */
   RCC->CR &= (uint32_t)0xFFFBFFFF;

   /* Disable all interrupts */
   RCC->CIR = 0x00000000;

#if defined(DATA_IN_ExtSRAM)
   SetSysClk();
   SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM */

   /* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
   SCB->VTOR = SRAM1_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
   SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif
}

/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 *         The SystemCoreClock variable contains the core clock (HCLK), it can
 *         be used by the user application to setup the SysTick timer or configure
 *         other parameters.
 *
 * @note   Each time the core clock (HCLK) changes, this function must be called
 *         to update SystemCoreClock variable value. Otherwise, any configuration
 *         based on this variable will be incorrect.
 *
 * @note   - The system frequency computed by this function is not the real
 *           frequency in the chip. It is calculated based on the predefined
 *           constant and the selected clock source:
 *
 *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
 *
 *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
 *
 *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
 *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
 *
 *         (*) HSI_VALUE is a constant defined in stm32f7xx.h file (default value
 *             16 MHz) but the real value may vary depending on the variations
 *             in voltage and temperature.
 *
 *         (**) HSE_VALUE is a constant defined in stm32f7xx.h file (default value
 *              25 MHz), user has to ensure that HSE_VALUE is same as the real
 *              frequency of the crystal used. Otherwise, this function may
 *              have wrong result.
 *
 *         - The result of this function could be not correct when using fractional
 *           value for HSE crystal.
 *
 * @param  None
 * @retval None
 */
void SystemCoreClockUpdate(void)
{
   uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

   /* Get SYSCLK source -------------------------------------------------------*/
   tmp = RCC->CFGR & RCC_CFGR_SWS;

   switch (tmp)
   {
   case 0x00: /* HSI used as system clock source */
      SystemCoreClock = HSI_VALUE;
      break;
   case 0x04: /* HSE used as system clock source */
      SystemCoreClock = HSE_VALUE;
      break;
   case 0x08: /* PLL used as system clock source */

      /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
         SYSCLK = PLL_VCO / PLL_P
         */
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;

      if (pllsource != 0)
      {
         /* HSE used as PLL clock source */
         pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }
      else
      {
         /* HSI used as PLL clock source */
         pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }

      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> 16) + 1) * 2;
      SystemCoreClock = pllvco / pllp;
      break;
   default:
      SystemCoreClock = HSI_VALUE;
      break;
   }
   /* Compute HCLK frequency --------------------------------------------------*/
   /* Get HCLK prescaler */
   tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
   /* HCLK frequency */
   SystemCoreClock >>= tmp;
}

#if defined(DATA_IN_ExtSRAM)
/**
 * @brief  Configures the clock at 216MHz.
 *         Called in startup_stm32f7xx.s before jump to main.
 *         This function configures the clock for fast access to external memories
 * @param  None
 * @retval None
 */
void SetSysClk(void)
{
   register uint32_t tmpreg = 0, timeout = 0xFFFF;

   /******************************************************************************/
   /*            PLL (clocked by HSE) used as System clock source                */
   /******************************************************************************/

   /************************* PLL Parameters for clock at 216MHz******************/
   uint32_t PLL_M = 25, PLL_Q = 9, PLL_R = 7, PLL_N = 400, PLL_P = 2;

   /* Enable Power Control clock */
   RCC->APB1ENR |= RCC_APB1ENR_PWREN;

   /* Config Voltage Scale 1 */
   PWR->CR1 |= PWR_CR1_VOS;

   /* Enable HSE */
   RCC->CR |= ((uint32_t)RCC_CR_HSEON);

   /* Wait till HSE is ready and if Time out is reached exit */
   do
   {
      tmpreg = RCC->CR & RCC_CR_HSERDY;
   } while ((tmpreg != RCC_CR_HSERDY) && (timeout-- > 0));

   if (timeout != 0)
   {
      /* Select regulator voltage output Scale 1 mode */
      RCC->APB1ENR |= RCC_APB1ENR_PWREN;

      PWR->CR1 |= PWR_CR1_VOS;

      /* Enable Over Drive to reach the 216MHz frequency */
      /* Enable ODEN */
      PWR->CR1 |= 0x00010000;
      timeout = 0xFFFF;
      /* Wait till ODR is ready and if Time out is reached exit */
      do
      {
         tmpreg = PWR->CSR1 & PWR_CSR1_ODRDY;
      } while ((tmpreg != PWR_CSR1_ODRDY) && (timeout-- > 0));

      /* Enable ODSW */
      PWR->CR1 |= 0x00020000;
      timeout = 0xFFFF;
      /* Wait till ODR is ready and if Time out is reached exit */
      do
      {
         tmpreg = PWR->CSR1 & PWR_CSR1_ODSWRDY;
      } while ((tmpreg != PWR_CSR1_ODSWRDY) && (timeout-- > 0));

      /* HCLK = SYSCLK / 1*/
      RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

      /* PCLK2 = HCLK / 2*/
      RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

      /* PCLK1 = HCLK / 4*/
      RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

      /* Configure the main PLL */
      RCC->PLLCFGR =
         PLL_M | (PLL_N << 6) | (((PLL_P >> 1) - 1) << 16) | (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24) | (PLL_R << 28);

      /* Enable the main PLL */
      RCC->CR |= RCC_CR_PLLON;
   }
   /* Wait that PLL is ready */
   timeout = 0xFFFF;
   do
   {
      tmpreg = (RCC->CR & RCC_CR_PLLRDY);
   } while ((tmpreg != RCC_CR_PLLRDY) && (timeout-- > 0));

   if (timeout != 0)
   {
      /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
      FLASH->ACR = FLASH_ACR_LATENCY_7WS;

      /* Select the main PLL as system clock source */
      RCC->CFGR &= (uint32_t)((uint32_t) ~(RCC_CFGR_SW));
      RCC->CFGR |= RCC_CFGR_SW_PLL;

      timeout = 0xFFFF;
      do
      {
         tmpreg = (RCC->CFGR & RCC_CFGR_SWS);
      } while ((tmpreg != RCC_CFGR_SWS) && (timeout-- > 0));
   }

   /* Update the SystemCoreClock */
   SystemCoreClockUpdate();
}

/**
 * @brief  Setup the external memory controller.
 *         Called in startup_stm32f7xx.s before jump to main.
 *         This function configures the external memories (SRAM)
 *         This SRAM will be used as program data memory (including heap and stack).
 * @param  None
 * @retval None
 */
void SystemInit_ExtMemCtl(void)
{
   __IO uint32_t tmp = 0;

   /*-- GPIOs Configuration -----------------------------------------------------*/
   /* Enable GPIOD, GPIOE, GPIOF and GPIOG interface clock */
   RCC->AHB1ENR |= 0x00000078;

   /* Delay after an RCC peripheral clock enabling */
   tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);

   /* Connect PDx pins to FMC Alternate function */
   GPIOD->AFR[0] = 0x00CC00CC;
   GPIOD->AFR[1] = 0xCCCCCCCC;
   /* Configure PDx pins in Alternate function mode */
   GPIOD->MODER = 0xAAAA0A0A;
   /* Configure PDx pins speed to 100 MHz */
   GPIOD->OSPEEDR = 0xFFFF0F0F;
   /* Configure PDx pins Output type to push-pull */
   GPIOD->OTYPER = 0x00000000;
   /* No pull-up, pull-down for PDx pins */
   GPIOD->PUPDR = 0x55550505;

   /* Connect PEx pins to FMC Alternate function */
   GPIOE->AFR[0] = 0xC00CC0CC;
   GPIOE->AFR[1] = 0xCCCCCCCC;
   /* Configure PEx pins in Alternate function mode */
   GPIOE->MODER = 0xAAAA828A;
   /* Configure PEx pins speed to 100 MHz */
   GPIOE->OSPEEDR = 0xFFFFC3CF;
   /* Configure PEx pins Output type to push-pull */
   GPIOE->OTYPER = 0x00000000;
   /* No pull-up, pull-down for PEx pins */
   GPIOE->PUPDR = 0x55554145;

   /* Connect PFx pins to FMC Alternate function */
   GPIOF->AFR[0] = 0x00CCCCCC;
   GPIOF->AFR[1] = 0xCCCC0000;
   /* Configure PFx pins in Alternate function mode */
   GPIOF->MODER = 0xAA000AAA;
   /* Configure PFx pins speed to 100 MHz */
   GPIOF->OSPEEDR = 0xFF000FFF;
   /* Configure PFx pins Output type to push-pull */
   GPIOF->OTYPER = 0x00000000;
   /* No pull-up, pull-down for PFx pins */
   GPIOF->PUPDR = 0x55000555;

   /* Connect PGx pins to FMC Alternate function */
   GPIOG->AFR[0] = 0x0CCCCCCC;
   GPIOG->AFR[1] = 0x00000000;
   /* Configure PGx pins in Alternate function mode */
   GPIOG->MODER = 0x00002AAA;
   /* Configure PGx pins speed to 100 MHz */
   GPIOG->OSPEEDR = 0x00003FFF;
   /* Configure PGx pins Output type to push-pull */
   GPIOG->OTYPER = 0x00000000;
   /* No pull-up, pull-down for PGx pins */
   GPIOG->PUPDR = 0x00001555;

   /*-- FMC/FSMC Configuration --------------------------------------------------*/
   /* Enable the FMC/FSMC interface clock */
   RCC->AHB3ENR |= 0x00000001;

   /* Delay after an RCC peripheral clock enabling */
   tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

   /* Configure and enable Bank1_SRAM3 */
   FMC_Bank1->BTCR[4] = 0x00001091;
   FMC_Bank1->BTCR[5] = 0x00110212;
   FMC_Bank1E->BWTR[4] = 0x0FFFFFFF;

   /*
    * Disable the FMC bank1 (enabled after reset).
    * This, prevents CPU speculation access on this bank which blocks the use of FMC during
    * 24us. During this time the others FMC master (such as LTDC) cannot use it!
    */
   FMC_Bank1->BTCR[0] = 0x000030d2;

   (void)(tmp);
}
#endif /* DATA_IN_ExtSRAM */

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
