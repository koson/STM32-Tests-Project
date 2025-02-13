/**
  @page PWR_CurrentConsumption PWR Current Consumption example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    PWR/PWR_CurrentConsumption/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR Current Consumption example.
  ******************************************************************************
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
  @endverbatim

@par Example Description 

This example shows how to configure the STM32F7xx system to measure different
Low Power modes current consumption. The Low Power modes are:
  - Sleep Mode
  - STOP mode with RTC
  - STANDBY mode without RTC and BKPSRAM
  - STANDBY mode with RTC
  - STANDBY mode with BKPSRAM
  
To run this example, user has to follow these steps:
 1. Select the Low power modes to be measured by uncommenting the corresponding
    line inside the stm32f7xx_lp_modes.h file.
    @code
       /* #define SLEEP_MODE               */
       /* #define STOP_MODE                */
       /* #define STANDBY_MODE             */
       /* #define STANDBY_RTC_MODE         */
       /* #define STANDBY_BKPSRAM_MODE     */
    @endcode       

 2. Use an external amperemeter to measure the IDD current. 

 3. This example can not be used in DEBUG mode, this is due to the fact that the 
    Cortex-M7 core is no longer clocked during low power mode so debugging 
    features are disabled.

Here below a detailed description of the example code:

  @verbatim

 1. After reset, LED1 is blinking slowly (1s) waiting for user button to be pressed to enter the selected low power mode.

 - When MCU enters low power modes, LED1 turns OFF.

 - When RTC is configured in the low power mode, the wakup is generated automatically (after 20s). OtherWise, user button should be pressed again to wakup from low power mode.

 - In case of STANDBY modes : 
  - If RTC is used: LED1 is blinking fast (100ms) while entering in RTC IT callback
  - If RTC is not used, LED1 is blinking fast (100ms)  while entering IT callback after Pressing user button. 

--> In any case, wrong end of test wil turn red LED (LED3) ON

 2. Low power modes description:

    - Sleep Mode
    ============  
            - System Running at PLL (216MHz)
            - Flash 3 wait state
            - Instruction and Data caches ON
            - Prefetch OFF       
            - Code running from Internal FLASH
            - All peripherals disabled
            - Wakeup using EXTI Line (User Button PC.13)

    - STOP Mode
    ===========
            - RTC Clocked by LSI
            - Regulator in LP mode
            - HSI, HSE OFF and LSI if not used as RTC Clock source
            - No IWDG
            - FLASH in deep power down mode
            - Automatic Wakeup using RTC clocked by LSI (after ~20s)

    - STANDBY Mode
    ==============
            - Backup SRAM and RTC OFF
            - IWDG and LSI OFF
            - Wakeup using EXTI Line (User Button PC.13)
                        
    - STANDBY Mode with RTC clocked by LSI 
    ==========================================
            - RTC Clocked by LSI
            - IWDG OFF and LSI OFF  if not used as RTC Clock source
            - Backup SRAM OFF
            - Automatic Wakeup using RTC clocked by LSI (after ~20s)

    - STANDBY Mode with backup SRAM ON
    ======================================================
            - RTC Clocked by LSI
            - IWDG OFF and LSI OFF  if not used as RTC Clock source
            - Backup SRAM ON
            - Automatic Wakeup using RTC clocked by LSI (after ~20s)

   @endverbatim


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.
  
@note On the NUCLEO-F722ZE board, an extra current consumption(~4µA) is added due to the Ethernet PHY. 
      So, to reach the correct current consumption values, this example configures the Ethernet
      PHYs in Low power mode. 

@par Keywords

Power, STOP, Sleep, Standby, Current Consumption, Low Power, LSI, Backup SRAM, Voltage range

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 “Managing memory protection unit (MPU) in STM32 MCUs”

@par Directory contents 

  - PWR/PWR_CurrentConsumption/Inc/stm32f7xx_hal_conf.h     HAL configuration file
  - PWR/PWR_CurrentConsumption/Inc/stm32f7xx_it.h           Interrupt handlers header file
  - PWR/PWR_CurrentConsumption/Inc/main.h                   Main program header file 
  - PWR/PWR_CurrentConsumption/Inc/stm32f7xx_lp_modes.h     STM32F7xx Low Power Modes header file
  - PWR/PWR_CurrentConsumption/Src/stm32f7xx_it.c           Interrupt handlers
  - PWR/PWR_CurrentConsumption/Src/main.c                   Main program
  - PWR/PWR_CurrentConsumption/Src/stm32f7xx_hal_msp.c      HAL MSP module
  - PWR/PWR_CurrentConsumption/Src/stm32f7xx_lp_modes.c     STM32F7xx Low Power Modes source file
  - PWR/PWR_CurrentConsumption/Src/system_stm32f7xx.c       STM32F7xx system clock configuration file


@par Hardware and Software environment

  - This example runs on STM32F722xx/STM32F723xx/STM32F732xx/STM32F733xx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-F722ZE
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-F722ZE Set-up
    - Make sure that solder bridge SB13 is open to have correct current consumption (pin disconnected to Ethernet PHY).
    - Use LED3 connected to PB14 pin.
      * LED3 (RED) will stay ON if initialization fails.
    - Use LED1 connected to PB00 pin.
      * LED1 (GREEN) will slowly toggle (1sec.) waiting for user to launch test, then will turn OFF
    - Use User Button connected to PC13 pin.
    - Connect an amperemeter to jumper JP5 to measure the IDD current
    
  

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
