/**
  @page RNG_GenerateRandomNumbers_IT RNG : Random Number Generation using IT
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    Examples_LL/RNG/RNG_GenerateRandomNumbers_IT/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RNG_GenerateRandomNumbers_IT example.
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

This example shows how to configure RNG peripheral to allow generation of 
32-bit long Random Numbers, using interrupts. Peripheral initialization done
using LL unitary services functions for optimization purpose (performance and size).

Example execution:
After startup from reset and system configuration, RNG configuration is performed.
(Configure PLLSA11 to enable 48M domain, then enable PLLSAI1 output mapped on 48MHz domain clock
and configure PLLSAI as RNG clock source).

User is then asked to press key button (LED1 blinking fast).
On user button press, several (8) Random 32bit numbers are generated
(On each raised RNG interrupt, a random number is generated and retrieved from DR register).
Corresponding generated values are available and stored in a u32 array (aRandom32bit), 
whose content could be displayed using debugger (Watch or LiveWatch features).
After successful Random numbers generation, LED1 is turned On. 
In case of errors, LED1 is slowly blinking (1sec period).

@par Keywords

Analog, RNG, Random, FIPS PUB 140-2, Analog Random number generator, Entropy, Period, Interrupt

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents 

  - RNG/RNG_GenerateRandomNumbers_IT/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - RNG/RNG_GenerateRandomNumbers_IT/Inc/main.h                  Header for main.c module
  - RNG/RNG_GenerateRandomNumbers_IT/Inc/stm32_assert.h          Template file to include assert_failed function
  - RNG/RNG_GenerateRandomNumbers_IT/Src/stm32f7xx_it.c          Interrupt handlers
  - RNG/RNG_GenerateRandomNumbers_IT/Src/main.c                  Main program
  - RNG/RNG_GenerateRandomNumbers_IT/Src/system_stm32f7xx.c      STM32F7xx system source file


@par Hardware and Software environment

  - This example runs on STM32F767xx devices.
    
  - This example has been tested with NUCLEO-F767ZI board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Push User button and use Variable watch window from debugger to access to values of generated numbers.
   (A break point could be set on LED_On() call, at end of RandomNumbersGeneration_IT() function).

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
