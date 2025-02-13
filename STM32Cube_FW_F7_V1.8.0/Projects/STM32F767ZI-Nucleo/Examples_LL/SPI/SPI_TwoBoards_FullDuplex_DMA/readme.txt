/**
  @page SPI_TwoBoards_FullDuplex_DMA SPI example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    Examples_LL/SPI/SPI_TwoBoards_FullDuplex_DMA/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the SPI_TwoBoards_FullDuplex_DMA example.
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

This example shows how to ensure SPI data buffer transmission and reception in
DMA mode. The example is based on the STM32F7xx SPI LL API. Peripheral initialization
is done using LL unitary services functions for optimization purpose
(performance and size).

The communication is done with 2 boards through SPI.
   _________________________                        _________________________
  |       ___________ ______|                      |__________________       |
  |      |SPI1              |                      |             SPI1 |      |
  |      |                  |                      |                  |      |
  |      |         CLK(PA5) |______________________|(PA5)CLK          |      |
  |      |                  |                      |                  |      |
  |      |         MISO(PA6)|______________________|(PA6)MISO         |      |
  |      |                  |                      |                  |      |
  |      |         MOSI(PA7)|______________________|(PA7)MOSI         |      |
  |      |                  |                      |                  |      |
  |      |__________________|                      |__________________|      |
  |      __                 |                      |                         |
  |     |__|                |                      |                         |
  |     USER                |                      |                         |
  |                      GND|______________________|GND                      |
  |                         |                      |                         |
  |_STM32F7xx Master _______|                      |_STM32F7xx Slave ________|

This example shows how to configure GPIO and SPI peripherals
to use a Full-Duplex communication using DMA Transfer mode through the STM32F7xx SPI LL API.

The project is splitted in two parts the Master Board and the Slave Board:

- Master Board
  SPI1 Peripheral is configured in Master mode.
  DMA2_Stream2 and DMA2_Stream3 configured to transfer Data via SPI peripheral
  GPIO associated to User push-button is linked with EXTI. 
  
- Slave Board
  SPI1 Peripheral is configured in Slave mode.
  DMA2_Stream2 and DMA2_Stream3 configured to transfer Data via SPI peripheral

The user can choose between Master and Slave through "#define MASTER_BOARD"
in the "main.c" file:
- Uncomment "#define MASTER_BOARD" to select Master board.
- Comment "#define MASTER_BOARD" to select Slave board.

Example execution:
On BOARD MASTER, LED1 is blinking Fast (200ms) and wait User push-button action.
Press User push-button on BOARD MASTER start a Full-Duplex communication through DMA.
On MASTER side, Clock will be generated on SCK line, Transmission(MOSI Line) and reception (MISO Line) 
will be done at the same time. 
SLAVE SPI will received  the Clock (SCK Line), so Transmission(MISO Line) and reception (MOSI Line) will be done also.

LED1 is On if data is well received.

In case of errors, LED1 is blinking Slowly (1s).

@note You need to perform a reset on Master board, then perform it on Slave board
      to have the correct behaviour of this example.


@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents 

  - SPI/SPI_TwoBoards_FullDuplex_DMA/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - SPI/SPI_TwoBoards_FullDuplex_DMA/Inc/main.h                  Header for main.c module
  - SPI/SPI_TwoBoards_FullDuplex_DMA/Inc/stm32_assert.h          Template file to include assert_failed function
  - SPI/SPI_TwoBoards_FullDuplex_DMA/Src/stm32f7xx_it.c          Interrupt handlers
  - SPI/SPI_TwoBoards_FullDuplex_DMA/Src/main.c                  Main program
  - SPI/SPI_TwoBoards_FullDuplex_DMA/Src/system_stm32f7xx.c      STM32F7xx system source file

@par Hardware and Software environment

  - This example runs on STM32F767xx devices.

  - This example has been tested with NUCLEO-F767ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-F767ZI Set-up
    - Connect Master board PA5 to Slave Board PA5 (connected to pin 10 of CN7 connector)
    - Connect Master board PA6 to Slave Board PA6 (connected to pin 12 of CN7 connector)
    - Connect Master board PA7 to Slave Board PA7 (connected to pin 14 of CN7 connector)
    - Connect Master board GND to Slave Board GND

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
    o Uncomment "#define MASTER_BOARD" and load the project in Master Board
    o Comment "#define MASTER_BOARD" and load the project in Slave Board
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
