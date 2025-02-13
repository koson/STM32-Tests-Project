/**
  @page I2S_DataExchangeInterrupt I2S Interrupt example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    I2S/I2S_DataExchangeInterrupt/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the I2S Interrupt example.
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

This example provides a description of how to set a communication between two
SPIs in I2S mode using interrupts and performing a transfer from Master to Slave.

*------------------------------------------------------------------------------*
|                BOARD A                                BOARD B                |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         | 
|        |     __________     |                 |     __________     |         |
|        |    |SPI Master|____|_______SD________|____|SPI Slave |    |         |
|        |    |  Device  |____|_______WS________|____|  Device  |    |         |
|        |    |__________|____|_______CK________|____|__________|    |         |
|        |                    |                 |                    |         |
|        |                    |                 |  O LD1             |         |
|        |            Tamper  |                 |  O LD2             |         |
|        |               _    |                 |  O LD3             |         |
|        |              |_|   |                 |                    |         |
|        |                    |                 |                    |         |
|        |             GND O--|-----------------|--O GND             |         |
|        |____________________|                 |____________________|         |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------*

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 216 MHz.

The I2S peripheral configuration is ensured by the HAL_I2S_Init() function.
This later is calling the HAL_I2S_MspInit()function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK, 
GPIO and NVIC). You may update this function to change I2S configuration.

The I2S communication is then initiated.
The HAL_I2S_Transmit_IT() function allows the transmission of a predefined data buffer 
and HAL_I2S_Receive_IT() function allows the reception.

The user can choose between Master and Slave through "#define I2S_MASTER_TRANSMITTER"
in the "main.c" file.
If the Master board is used, the "#define I2S_MASTER_TRANSMITTER" must be uncommented.
If the Slave board is used the "#define I2S_MASTER_TRANSMITTER" must be commented.

For this example the I2S_Buffer_Tx is predefined and the I2S_Buffer_Rx size is same as I2S_Buffer_Tx.

In master board, I2S peripheral is configured as Master Transmitter with Interrupt,
whereas in Slave board, I2S peripheral is configured as Slave Receiver with Interrupt. 
The both devices are in Philips standard configuration with 16bit and 48KHz audio frequency.

When the user press on the Tamper Button on the Master Board, the master transmitter
start sending data stored in the I2S_Buffer_Tx buffer while the Slave Board receives
data I2S_Buffer_Rx buffer.

Once the transfer is completed a comparison is done and TransferStatus gives the 
data transfer status.

STM32 board's LEDs can be used to monitor the transfer status:
 - LED1 turned OFF waiting User push-button to be pressed on master board.
 - LED1 turns ON if transmission/reception is complete and OK.
 - LED3 turns ON if error is generated.        

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Audio, I2S, Data Exchange, Interrupt, Philips Standard, Master, Slave

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents  

  - I2S/I2S_DataExchangeInterrupt/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - I2S/I2S_DataExchangeInterrupt/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - I2S/I2S_DataExchangeInterrupt/Inc/stm32f7xx.h             Header for main.c module  
  - I2S/I2S_DataExchangeInterrupt/Src/stm32f7xx_it.c          Interrupt handlers
  - I2S/I2S_DataExchangeInterrupt/Src/main.c                  Main program
  - I2S/I2S_DataExchangeInterrupt/Src/system_stm32f7xx.c      STM32F7xx system source file

@par Hardware and Software environment

  - This example runs on STM32F767ZI devices.
    
  - This example has been tested with STMicroelectronics STM32F767ZI-Nucleo board and can be
    easily tailored to any other supported device and development board.  

  - STM32F767ZI-Nucleo Set-up
    - Connect Master board PA5(CN7.Pin10) to Slave Board PA5(CN7.Pin10)
    - Connect Master board PA7(CN7.Pin14) to Slave Board PA7(CN7.Pin14)
    - Connect Master board PA4(CN7.Pin17) to Slave Board PA4(CN7.Pin17) 
    - Connect Master board GND to Slave Board GND 
    - The connection between the pins should use a short wires.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
