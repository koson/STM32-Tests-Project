/**
  @page LCD_PicturesFromSDCard LCD pictures from SD card application
  
  @verbatim
  ******************************************************************************
  * @file    Display/LCD_PicturesFromSDCard/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the LCD pictures from SD card application.
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. All rights reserved.
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

This application describes how to display pictures on LCD saved under SD card.
 
Prior to starting the application, the user has to create a "/Media" directory under the micro SD card root and
copy the pictures stored in PC directory "/Utilities/Media/Pictures".

 
@note : The user can add pictures of his choice but the file type, resolution
        and pixel format described below must be respected.
  
  Once the LCD, SD card and file systems initialized and configured, 
  a content check of the "Media" directory is done and the number of ".BMP" files 
  is retained.
  
  LED1(Red Led) is ON when the uSD disk I/O driver is not Linked correctly.
  
@note : An associate LCD driver is used in this application 
  
@note : The maximum number of BMP files is fixed to 25. It can be raised 
        until reaching the maximum of SD card memory space.

  The following steps are performed to display all the images stored in the 
  SD Card :
 
  Step1 :
  ------
  The foreground layer is set, the image copied from SD card to intermediate
  SDRAM memory and then copied to LCD frame buffer. The image is fully visible when 
  the level of transparency is increased until it becomes totally opaque (reaches 255).
 
  Step2 :
  ------
  The transparency of the foreground layer
  begins to decrease until becoming totally transparent and the background layer 
  is set, the image copied from SD card to intermediate SDRAM memory and then 
  copied to LCD frame buffer and finally the level of transparency of the 
  background layer increases until it reaches 255 (totally opaque).
 
  Step3 :
  ------
  The transparency of the background layer
  begins to decrease until becoming totally transparent and return to step1.
  => A delay of 10 ms is inserted between the three steps.
  
@note the system clock (SYSCLK) is configured to run at 200 MHz and 50 MHz is provided 
      at the output PLL divided by PLL_Q. This frequency permits to reach 25 MHz clock 
      needed for SD operation and in line with microSD specification. 
      
@note 
  => If the "Media" directory is empty, a warning message is displayed on 
     the LCD : "  No Bitmap files...  "
    
  => If the file type stored in the "Media" directory is not supported,
     a warning message is displayed on the LCD : "  No Bitmap files...  "
    
  => If the SD card is removed while running the application, no warning message
     is displayed on the LCD. A reset is needed.
   
  => If no SD card is dtected, a warning message is displayed on the 
     LCD : "  No Bitmap files...  ".
   
@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note The STM32F7xx devices can reach a maximum clock frequency of 216MHz but as this application uses SDRAM,
      the system clock is limited to 200MHz. Indeed proper functioning of the SDRAM is only guaranteed 
      at a maximum system clock frequency of 200MHz.

@par Keywords

Graphic, Display, LCD, Pictures, SD Card, BMP, RGB565, RGB888, ARGB8888

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents

    - Display/LCD_PicturesFromSDCard/Inc/main.h                             Main configuration file
    - Display/LCD_PicturesFromSDCard/Inc/stm32f7xx_it.h                     Interrupt handlers header file
    - Display/LCD_PicturesFromSDCard/Inc/stm32f7xx_hal_conf.h               HAL Configuration file 
    - Display/LCD_PicturesFromSDCard/Inc/ffconf.h                           FAT file system module configuration file
    - Display/LCD_PicturesFromSDCard/Inc/fatfs_storage.h                    Header for fatfs_storage.c
    - Display/LCD_PicturesFromSDCard/Src/sd_diskio.h                        SD interface with Fatfs header file
    - Display/LCD_PicturesFromSDCard/Src/main.c                             Main program 
    - Display/LCD_PicturesFromSDCard/Src/fatfs_storage.c                    Storage (FatFs) driver
    - Display/LCD_PicturesFromSDCard/Src/stm32f7xx_it.c                     Interrupt handlers
    - Display/LCD_PicturesFromSDCard/Src/system_stm32f7xx.c                 STM32F7xx system clock configuration file
    - Display/LCD_PicturesFromSDCard/Src/sd_diskio.c                        SD interface with Fatfs source file
    

@par Hardware and Software environment  

  - This application runs on STM32F769xx/STM32F779xx devices.
    
  - This application has been tested with STMicroelectronics STM32F769I-Discovery
    boards and can be easily tailored to any other supported device 
    and development board.
    
  - STM32F769I-Discovery Set-up
    - Plug a micro SD card into CN5 connector


@par How to use it ?

In order to make the program work, you must do the following :
 - The bitmap images, available under ".Media/" 
   should be copied inside a  dedicated directory named "Media" at the micro SD card root.
   The images should have the following properties:
     file type    : *.bmp
     resolution   : up to 800x480
     pixel format : RGB565, RGB888 or ARGB8888     
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the application 
                
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
                                   