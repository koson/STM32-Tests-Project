/**
  @page FMC_SDRAM_LowPower SDRAM memory functionalities example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    FMC/FMC_SDRAM_LowPower/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the FMC_SDRAM_LowPower example.
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

This example describes how to configure the FMC controller to access the SDRAM 
memory in low power mode (SDRAM Self Refresh mode).

The SDRAM is IS42S32800G.

It gives a simple application of the FMC SDRAM low power mode use (self refresh mode) 
while the MCU is in a low power mode (STOP mode).

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 200 MHz.
  
The purpose is to show how the SDRAM can retain data written after entering STOP mode. 
STOP mode is a CPU low power mode which stops all peripherals clocks in the 1.2V 
domain. Only internal SRAM and registers content are preserved in this mode.
  
After SDRAM initialization, the data is written to the memory and a "self refresh" 
command is sent to the SDRAM. The program waits for Tamper push-button to be pushed 
to enter the CPU in STOP mode, the LED4 is then turned ON.
The wakeup from STOP mode is done when pushing Tamper push-button and the CPU returns 
to RUN mode. At this time, the system clock is reconfigured using the function 
SystemClock_Config(). The LED4 is then turned OFF.

Finally, a "normal mode" command is send to SDRAM memory to exit self refresh mode. 
The data written to SDRAM is read back and checked.  
   
LEDs are used to indicate the system state as following:
  - LED4 ON: CPU enters STOP mode.
  - LED4 OFF: the CPU is in RUN mode.
  - LED1 ON: correct data transfer (PASS). 
  - LED2 ON: incorrect data transfer (FAIL).
  - LED3 is ON in case of send command error.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Memory, FMC, SDRAM, Read, Write, Initialization, Access, Low power, Self Refresh, Stop mode, 

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents 

  - FMC/FMC_SDRAM_LowPower/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - FMC/FMC_SDRAM_LowPower/Inc/main.h                  Header for main.c module  
  - FMC/FMC_SDRAM_LowPower/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - FMC/FMC_SDRAM_LowPower/Src/main.c                  Main program
  - FMC/FMC_SDRAM_LowPower/Src/stm32f7xx_it.c          Interrupt handlers
  - FMC/FMC_SDRAM_LowPower/Src/system_stm32f7xx.c      STM32F7xx system source file


@par Hardware and Software environment

  - This example runs on STM32F767xx/STM32F769xx/STM32F777xx/STM32F779xx devices.
    
  - This example has been tested with STM32F769I-EVAL board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
