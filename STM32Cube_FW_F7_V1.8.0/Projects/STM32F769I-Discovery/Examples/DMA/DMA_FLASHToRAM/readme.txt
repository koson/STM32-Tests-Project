/**
  @page DMA_FLASHToRAM DMA FLASH To RAM Example
  
  @verbatim
  ******************************************************************************
  * @file    DMA/DMA_FLASHToRAM/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the DMA FLASH to RAM example.
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

This example provides a description of how to use a DMA to transfer 
a word data buffer from FLASH memory to embedded SRAM memory through 
the HAL API.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 216 MHz.

DMA2_Stream0 is configured to transfer the contents of a 32-word data 
buffer stored in Flash memory to the reception buffer declared in RAM.

The start of transfer is triggered by software. DMA2_Stream0 memory-to-memory
transfer is enabled. Source and destination addresses incrementing is also enabled.
The transfer is started by setting the stream enable bit for DMA2_Stream0.
At the end of the transfer a Transfer Complete interrupt is generated since it
is enabled and the callback function (customized by user) is called.
 

Board's LEDs can be used to monitor the transfer status:
 - LED2 (Green Led) is ON when the transfer is complete (into the Transfer Complete interrupt
   routine).
 - LED1 (Red Led) is Toggled with a period of 200 ms when there is a transfer error 
 - LED1 (Red Led) is Toggled with a period of 1000 ms when Error_Handler is called
 
It is possible to select a different stream for the DMA transfer
example by modifying defines values in the file main.h.
Note that only DMA2 stream are able to perform Memory-to-Memory transfers.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, DMA, Data Transfer, Memory to memory, Stream, Flash, RAM

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents 

  - DMA/DMA_FLASHToRAM/Src/system_stm32f7xx.c    STM32F7xx system clock configuration file
  - DMA/DMA_FLASHToRAM/Src/stm32f7xx_it.c        Interrupt handlers
  - DMA/DMA_FLASHToRAM/Src/main.c                Main program
  - DMA/DMA_FLASHToRAM/Inc/stm32f7xx_hal_conf.h  HAL Configuration file
  - DMA/DMA_FLASHToRAM/Inc/stm32f7xx_it.h        Interrupt handlers header file
  - DMA/DMA_FLASHToRAM/Inc/main.h                Main program header file

     
@par Hardware and Software environment

  - This example runs on STM32F767xx/STM32F769xx/STM32F777xx/STM32F779xx Devices.

  - This example has been tested with STMicroelectronics STM32F769I-DISCOVERY 
    boards and can be easily tailored to any other supported device 
    and development board.    

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
