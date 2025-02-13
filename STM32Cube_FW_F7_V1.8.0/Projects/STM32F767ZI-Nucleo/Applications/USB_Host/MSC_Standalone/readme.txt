/**
  @page MSC_Standalone USB Host Mass Storage (MSC) application
  
  @verbatim
  ******************************************************************************
  * @file    USB_Host/MSC_Standalone/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USB Host MSC application.
  ******************************************************************************
  *
  * Copyright (c) 2016 STMicroelectronics International N.V. All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

This application is a part of the USB Host Library package using STM32Cube firmware. It describes how to use
USB host application based on the Mass Storage Class (MSC) on the STM32F767ZI device.

This is a typical application on how to use the STM32F767ZI USB OTG Host peripheral to operate with an USB 
flash disk using the Bulk Only Transfer (BOT) and Small Computer System Interface (SCSI) transparent
commands combined with a file system FatFs (Middleware component).

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals,
initialize the Flash interface and the systick. The user is provided with the SystemClock_Config()
function to configure the system clock (SYSCLK) to run at 216 MHz. The Full Speed (FS) USB module uses
internally a 48-MHz clock which is coming from the main PLL.

When the application is started, the connected USB flash disk device is detected in MSC mode and gets 
initialized. The STM32 MCU behaves as a MSC Host, it enumerates the device and extracts VID, PID, 
manufacturer name, Serial no and product name information and displays it on the LCD screen. 
This application is based on read/write file and explore the USB flash disk content trough a MSC routine.
LCD and joystick are provided by Adafruit 1.8" TFT shield to be plugged on Arduino connectors.
  
A menu is displayed and the user can select any operation from the menu using the Joystick buttons:
 - "File Operations" operation writes a small text file (less to 1 KB) on the USB flash disk.
 - "Explorer Disk" operation explores the USB flash disk content and displays it on the LCD screen.
    User has to press the Key button to display the whole USB flash disk content (recursion level 2).
 - "Re-Enumerate" operation performs a new Enumeration of the device.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

For more details about the STM32Cube USB Host library, please refer to UM1720  
"STM32Cube USB Host library".

@par USB Library Configuration

It is possible to fine tune needed USB Host features by modifying defines values in USBH configuration
file �usbh_conf.h� available under the project includes directory, in a way to fit the application
requirements, such as:
  - Level of debug: USBH_DEBUG_LEVEL
                  0: No debug messages
                  1: Only User messages are shown
                  2: User and Error messages are shown
                  3: All messages and internal debug messages are shown
   By default debug messages are displayed on the debugger IO terminal; to redirect the Library
   messages on the LCD screen, lcd_log.c driver need to be added to the application sources.


@par Keywords

Connectivity, USB Host, MSC, Mass Storage, Full Speed, High Speed, BOT, SCSI, Removable drive, FatFs,
File system, Write, Read, Format

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents

  - USB_Host/MSC_Standalone/Src/main.c                  Main program
  - USB_Host/MSC_Standalone/Src/usbh_diskio_dma.c       FatFS usbh diskio driver implementation
  - USB_Host/MSC_Standalone/Src/system_stm32f7xx.c      STM32F7xx system clock configuration file
  - USB_Host/MSC_Standalone/Src/stm32f7xx_it.c          Interrupt handlers
  - USB_Host/MSC_Standalone/Src/menu.c                  MSC State Machine
  - USB_Host/MSC_Standalone/Src/usbh_conf.c             General low level driver configuration
  - USB_Host/MSC_Standalone/Src/explorer.c              Explore the USB flash disk content
  - USB_Host/MSC_Standalone/Src/file_operations.c       Write/read file on the disk
  - USB_Host/MSC_Standalone/Inc/main.h                  Main program header file
  - USB_Host/MSC_Standalone/Inc/usbh_diskio_dma.h       FatFS usbh diskio driver header file
  - USB_Host/MSC_Standalone/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - USB_Host/MSC_Standalone/Inc/lcd_log_conf.h          LCD log configuration file
  - USB_Host/MSC_Standalone/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - USB_Host/MSC_Standalone/Inc/usbh_conf.h             USB Host driver Configuration file
  - USB_Host/MSC_Standalone/Inc/ffconf.h                FAT file system module configuration file
 

@par Hardware and Software environment

  - This application runs on STM32F767ZI devices.
    
  - This application has been tested with STMicroelectronics NUCLEO-F767ZI board 
    and can be easily tailored to any other supported device and development board.

  - NUCLEO-F767ZI Set-up
    - Use CN13 connector to Plug a USB key into the NUCLEO-F767ZI board 
      through 'USB micro A-Male to A-Female' cable

  - Make sure that the jumper JP4 is closed

  - Adafruit 1.8" TFT shield must be connected on CN7, CN10, CN8 and CN9 Arduino connectors, 
    for more details please refer to UM1726.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the application
 
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
