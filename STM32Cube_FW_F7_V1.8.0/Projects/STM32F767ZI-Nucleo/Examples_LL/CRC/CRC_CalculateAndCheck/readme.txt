/**
  @page CRC_CalculateAndCheck CRC : CRC calculation and computed CRC value checking
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    Examples_LL/CRC/CRC_CalculateAndCheck/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the CRC_CalculateAndCheck example.
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

This example shows how to configure CRC calculation unit to get a CRC code of a given data buffer,
based on a fixed generator polynomial (default value 0x4C11DB7). Peripheral initialization done 
using LL unitary services functions for optimization purpose (performance and size).

CRC Peripheral is configured to work with default polynomial value (32-bit long).
Normal representation of this polynomial value is :
 X^32 + X^26 + X^23 + X^22 + X^16 + X^12 + X^11 + X^10 +X^8 + X^7 + X^5 + X^4 + X^2 + X + 1.
Generated CRC value is then 32 bits long.

Example execution:
After startup from reset and system configuration, CRC configuration is performed (use of default Polynomial and initialisation values).
CRC code of a given data buffer is computed. 
Data buffer length has been chosen as not an exact nb of u32 (32-bit words), in order to illustrate 
use of offered API for feeding the calculator (u32, u16 or u8 inputs).
The calculated CRC code is stored in uwCRCValue variable.
Once calculated, CRC value (uwCRCValue) is compared to the CRC expected value (uwExpectedCRCValue),
and if both are equal, LED1 is turned On. 
In case of errors, LED1 is blinking (1sec period).

@par Keywords

Security, CRC, CRC Polynomial, IEC 60870-5, hardware CRC, user-defined, generating polynomial, CRC Calculate

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents 

  - CRC/CRC_CalculateAndCheck/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - CRC/CRC_CalculateAndCheck/Inc/main.h                  Header for main.c module
  - CRC/CRC_CalculateAndCheck/Inc/stm32_assert.h          Template file to include assert_failed function
  - CRC/CRC_CalculateAndCheck/Src/stm32f7xx_it.c          Interrupt handlers
  - CRC/CRC_CalculateAndCheck/Src/main.c                  Main program
  - CRC/CRC_CalculateAndCheck/Src/system_stm32f7xx.c      STM32F7xx system source file


@par Hardware and Software environment

  - This example runs on STM32F767xx devices.
    
  - This example has been tested with NUCLEO-F767ZI board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 