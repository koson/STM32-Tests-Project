/**
  @page DMA2D_MemToMemWithLCD DMA2D Memory to Memory with LCD example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    DMA2D/DMA2D_MemToMemWithLCD/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the DMA2D Memory to Memory with LCD example.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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

  This example provides a description of how to configure DMA2D peripheral in 
  Memory_to_Memory transfer mode and display the result on LCD.
  The source buffer in format ARGB4444 and size 150x150 located in Flash is copied in Memory to Memory mode 
  by the DMA2D in SRAM Destination buffer of same size and format.
  This SRAM buffer is then displayed by the LTDC centered on LCD screen.

  At the beginning of the main program the HAL_Init() function is called to reset 
  all the peripherals, initialize the Flash interface and the systick.
  Then the SystemClock_Config() function is used to configure the system
  clock (SYSCLK) to run at 200 MHz.
 
  In this basic example the goal is to explain the different fields of the DMA2D 
  structure in the case of Memory_to_Memory transfer mode.

  After DMA2D configuration, the data transfer Memory to Memory is performed from FLASH memory to
  SRAM. Then the LTDC is configured to display obtained DMA2D output buffer in SRAM on LCD. 
  (more details about LCD configuration in LCD examples)
  
 => Output offset is the number of pixel to be skipped after each transferred line
                      
 how to calculate the size of the transferred data ? 
 => selected color mode gives the number of bits per pixel and we have 
    (ARGB4444 => 16bits/pixel) the number of pixel per line and the number of line, 
    therefore :
    
    data_size = (bits per pixel) X (pixel per line + output offset) X (number of line) 
       
STM32769I-EVAL board's LEDs can be used to monitor the transfer status:
 - LED1 is ON when the DMA2D transfer is complete.
 - LED2 is ON when there is a DMA2D transfer error.
 - LED3 is ON when there is an error in LTDC transfer/Init process.
 
@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note The STM32F7xx devices can reach a maximum clock frequency of 216MHz but as this example uses SDRAM,
      the system clock is limited to 200MHz. Indeed proper functioning of the SDRAM is only guaranteed 
      at a maximum system clock frequency of 200MHz.

@par Keywords

Display, Graphic, DMA2D, LCD, SRAM, ARGB4444, Blending, Memory to memory, LTDC, Pixel

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents

    - DMA2D/DMA2D_MemToMemWithLCD/Inc/main.h                Main configuration file
    - DMA2D/DMA2D_MemToMemWithLCD/Inc/stm32f7xx_it.h        Interrupt handlers header file
    - DMA2D/DMA2D_MemToMemWithLCD/Inc/stm32f7xx_hal_conf.h  HAL configuration file
    - DMA2D/DMA2D_MemToMemWithLCD/Inc/ARGB4444_150x150.h    Texture in Flash to be transferred by DMA2D in SRAM
    - DMA2D/DMA2D_MemToMemWithLCD/Src/main.c                Main program  
    - DMA2D/DMA2D_MemToMemWithLCD/Src/stm32f7xx_it.c        Interrupt handlers
    - DMA2D/DMA2D_MemToMemWithLCD/Src/stm32f7xx_hal_msp.c   HAL MSP module    
    - DMA2D/DMA2D_MemToMemWithLCD/Src/system_stm32f7xx.c    STM32F7xx system clock configuration file

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
                                   