/**
  @page DMA2D_BlendingWithAlphaInversion DMA2D blending with alpha inversion example
  
  @verbatim
  ******************************************************************************
  * @file    DMA2D/DMA2D_BlendingWithAlphaInversion/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the STM32F7xx DMA2D blending with alpha inversion example.
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
  Memory_to_Memory with blending transfer and alpha inversion mode.

  At the beginning of the main program the HAL_Init() function is called to reset 
  all the peripherals, initialize the Flash interface and the systick.
  Then the SystemClock_Config() function is used to configure the system
  clock (SYSCLK) to run at 200 MHz.
 
  In this transfer mode two input sources are fetched : foreground and background.
  In this example, the foreground and background are configured as following :
   - Foreground object 
   - input memory address at FLASH memory (static image arrays coded in FLASH).
   - Format : RGB565
   - Size   : 240x130
   - Background object 
   - input memory address at FLASH memory (static image arrays coded in FLASH).
   - Format : RGB565
   - Size   : 240x130
   - The constant alpha for foreground is decreased to see the background.
   - The alpha mode for foreground and background is configured to see two 
     superposed images in the resulting image out of the ChromArt DMA2D operation.
   - The Foreground alpha inversion is set sequentially to regular then inverted to demonstrate
     this new DMA2D feature capability.
        
  The DMA2D blends the two sources pixels buffers stored into the FLASH to compute 
  the resulting pixel in dedicated output buffer. 
  The transferred data to the output memory address is the result of the blending operation 
  between background and foreground (with Foreground alpha inversion set to regular and inverted).
 
  Next step in the pixel pipeline is the use of the LTDC to display the result blending image.
  
  The example acts as follow :
  In an infinite while loop 
  - Use the DMA2D to blend the two sources images.
  
  - If the foreground alpha inversion is disabled (set to DMA2D_REGULAR_ALPHA) :    
    - Copy the blended image to the LTDC display frame buffer with the following message :
     �Display Blended Image: Foreground Alpha Inversion OFF�
  
  - If the foreground alpha inversion is enabled (set to DMA2D_INVERTED_ALPHA) :    
    - Copy the blended image to the LTDC display frame buffer with the following message :
     �Display Blended Image: Foreground Alpha Inversion ON�
  
  - Change the alpha inversion setting, wait for 5 sec and go to the next loop.
  
  As consequence  user can see the result blending image with foreground alpha inversion enabled and disabled.


STM32769I-DISCOVERY board's LEDs can be used to monitor the transfer status:
 - LED2 is ON when the DMA2D transfer is complete.
 - LED1 toggles every  when there is a DMA2D transfer error.
 - LED1 is ON when there is an error in LTDC transfer/Init process.
 
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

@note The example shows how to use the DMA2D blending with alpha inversion set on the foreground layer.
      Nevertheless the same parameter can be applied on the background DMA2D layer depending of the user 
      application requirements.

@par Keywords

Display, Graphic, DMA2D, LCD, SRAM, ARGB4444, Blending, Alpha Inversion, Memory to memory, Foreground,
Background, RGB565, LTDC, Pixel

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents

    - DMA2D/DMA2D_BlendingWithAlphaInversion/Inc/main.h                Main configuration file
    - DMA2D/DMA2D_BlendingWithAlphaInversion/Inc/stm32f7xx_it.h        Interrupt handlers header file
    - DMA2D/DMA2D_BlendingWithAlphaInversion/Inc/stm32f7xx_hal_conf.h  HAL configuration file 
    - DMA2D/DMA2D_BlendingWithAlphaInversion/Inc/RGB565_230x170_1.h    Image used for DMAD2D validation
    - DMA2D/DMA2D_BlendingWithAlphaInversion/Inc/RGB565_230x170_2.h    Image used for DMAD2D validation
    - DMA2D/DMA2D_BlendingWithAlphaInversion/Src/main.c                Main program  
    - DMA2D/DMA2D_BlendingWithAlphaInversion/Src/stm32f7xx_it.c        Interrupt handlers
    - DMA2D/DMA2D_BlendingWithAlphaInversion/Src/stm32f7xx_hal_msp.c   HAL MSP module
    - DMA2D/DMA2D_BlendingWithAlphaInversion/Src/system_stm32f7xx.c    STM32F7xx system clock configuration file


@par Hardware and Software environment

  - This example runs on STM32F767xx/STM32F769xx/STM32F777xx/STM32F779xx devices.
    
  - This example has been tested with STM32F769I-DISCOVERY board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Download and Debug
 - Run program: Debug->Go(F5) 


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */ 
