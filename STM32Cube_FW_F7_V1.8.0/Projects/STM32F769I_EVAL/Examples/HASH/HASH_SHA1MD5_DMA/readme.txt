/**
  @page HASH_SHA1MD5_DMA  HASH digest calculation using SHA1 and MD5 example
  with DMA transfer.
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    HASH/HASH_SHA1MD5_DMA/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the HASH digest calculation using SHA1 and MD5 example.
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

This example provides a short description of how to use the HASH peripheral to 
hash data using SHA-1 and MD5 Algorithms.

For this example, DMA is used to transfer data from memory to the HASH processor.
The message to hash is a 2048 bit data.
The SHA-1 message digest result is a 160 bit data, and the MD5 message digest
result is a 128 bit data.
The expected HASH digests (for SHA1 and MD5) are already computed using an online
HASH tool. Those values are compared to those computed by the HASH peripheral.
In case there is a missmatch the red LED is turned ON.
In case the SHA1 digest is computed correctly the green LED (LED1) is turned ON.
In case the MD5 digest is computed correctly the blue LED (LED4) is turned ON.

@par Keywords

System, Security, HASH, SHA1, MD5, digest, DMA

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents

  - HASH/HASH_SHA1MD5_DMA/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - HASH/HASH_SHA1MD5_DMA/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - HASH/HASH_SHA1MD5_DMA/Inc/main.h                  Header for main.c module  
  - HASH/HASH_SHA1MD5_DMA/Src/stm32f7xx_it.c          Interrupt handlers
  - HASH/HASH_SHA1MD5_DMA/Src/main.c                  Main program
  - HASH/HASH_SHA1MD5_DMA/Src/stm32f7xx_hal_msp.c     HAL MSP module
  - HASH/HASH_SHA1MD5_DMA/Src/system_stm32f7xx.c      STM32F7xx system source file


@par Hardware and Software environment

  - This example runs on STM32F777xx/STM32F779xx devices.
  
  - This example has been tested with STM32F779I_EVAL board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 