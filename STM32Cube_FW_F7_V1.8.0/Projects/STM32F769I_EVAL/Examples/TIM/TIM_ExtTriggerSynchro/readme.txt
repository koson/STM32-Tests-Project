/**
  @page TIM_ExtTriggerSynchro TIM External Trigger Synchro example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_ExtTriggerSynchro/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the TIM External Trigger Synchro example.
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

 This example shows how to synchronize TIM peripherals in cascade mode with an
  external trigger.
  In this example three timers are used:

  1/TIM1 is configured as Master Timer:
      - Toggle Mode is used
      - The TIM1 Enable event is used as Trigger Output 

  2/TIM1 is configured as Slave Timer for an external Trigger connected to TIM1 
    TI2 pin (TIM1 CH2 configured as input pin):
      - The TIM1 TI2FP2 is used as Trigger Input
      - Rising edge is used to start and stop the TIM1: Gated Mode.

  3/TIM4 is slave for TIM1 and Master for TIM5,
      - Toggle Mode is used
      - The ITR0 (TIM1) is used as input trigger 
      - Gated mode is used, so start and stop of slave counter
        are controlled by the Master trigger output signal(TIM1 enable event).
      - The TIM4 enable event is used as Trigger Output. 

  4/TIM5 is slave for TIM4,
      - Toggle Mode is used
      - The ITR2 (TIM4) is used as input trigger
      - Gated mode is used, so start and stop of slave counter
        are controlled by the Master trigger output signal(TIM4 enable event).

   The TIM1CLK is set to SystemCoreClock, 
   TIM4CLK is set to (SystemCoreClock/2) and 
   TIM5CLK frequency is set to (SystemCoreClock/2),
   to get TIMx counter clock at 20 MHz the Prescaler is computed as following: 
   - Prescaler = (TIMx CLK / TIMx counter clock) - 1

   SystemCoreClock is set to 216 MHz.

   TIMx frequency = TIMx  counter clock/ 2*(TIMx_Period + 1) = 100 KHz.
   
  The starts and stops of the TIM1 counters are controlled by the external trigger.
  The TIM4 starts and stops are controlled by the TIM1, and the TIM5 starts and 
  stops are controlled by the TIM4.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Timers, PWM, External Trigger, Synchronization, Cascade mode, Master, Slave, Duty Cycle, Waveform,
Oscilloscope, Output, Signal,

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents  

  - TIM/TIM_ExtTriggerSynchro/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - TIM/TIM_ExtTriggerSynchro/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - TIM/TIM_ExtTriggerSynchro/Inc/main.h                  Header for main.c module  
  - TIM/TIM_ExtTriggerSynchro/Src/stm32f7xx_it.c          Interrupt handlers
  - TIM/TIM_ExtTriggerSynchro/Src/main.c                  Main program
  - TIM/TIM_ExtTriggerSynchro/Src/stm32f7xx_hal_msp.c     HAL MSP file
  - TIM/TIM_ExtTriggerSynchro/Src/system_stm32f7xx.c      STM32F7xx system source file

@par Hardware and Software environment

  - This example runs on STM32F767xx/STM32F769xx/STM32F777xx/STM32F779xx devices.
    
  - This example has been tested with STMicroelectronics STM32F769I-EVAL 
    board and can be easily tailored to any other supported device 
    and development board.

  - STM32F769I-EVAL Set-up
    - Connect an external trigger to the TIM1 CH2 PA.09 (pin 43 in CN6 connector).
    - Connect the following pins to an oscilloscope to monitor the different waveforms:
      - TIM1 CH1 PA.08 (pin 52 in CN6 connector)
      - TIM4 CH1 PB.06 pin 18 in CN6 connector
      - TIM5 CH1 PA.00 (pin 15 in CN5 connector)
	  
@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
