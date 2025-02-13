/**
  @page EEPROM_Emulation application to show an eeprom emulation

  @verbatim
  ******************************************************************************
  * @file    EEPROM/EEPROM_Emulation/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the EEPROM_Emulation application.
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

This application shows how to emulate EEPROM on internal flash.

Please refer to AN3969 for further details regarding this application.

The application requires two Flash memory sectors of identical size allocated to non-volatile data:
one that is initially erased, and offers byte-by-byte programmability; the other that is ready
to take over when the former sector needs to be garbage-collected. A header field that occupies
the first half word (16-bit) of each sector indicates the sector status. Each of these sectors is
considered as a page.
The sectors used on Flash Single and dual Bank are as follows:
  -Dual bank flash: 16KB (Sector12, Sector13)
  -Single bank flash: 32KB (Sector2, Sector3)

If the flash configuration is Dual bank mode uncomment the #define USE_DUAL_BANK_FLASH  in main.h to emulate
an EPROOM with on a dual bank flash, otherwise the default flash configuration is a Single bank flash.

@note You have to initially configure the FLASH memory at Single or Dual Bank mode using STM32 ST-LINK
      Utilities or any similar tool (check "nDBANK" box) according to the flash mode selected on main.h.

@note Using the Dual bank mode, the firmware code is executed from Sector0 on Bank1 and the Read/Write
      operations on EEPROM are done on Bank2.

NUCLEO-F767ZI board Rev.B LEDs can be used to monitor the application status:
  - LED2 is On when the application runs successfully.
  - LED2 is toggle in case of error.

@note It's recommended to deactivate CPU Data Cache. The EEPROM algorithm includes
      many flash erase operations which may make data in cache and physical memory not aligned.
      In case of enabling CPU Data Cache, user should invalidate data cache every read operation
      from Flash memory or configuring MPU as device. Then, it will impact seriously the application performance.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note Before using the application, ensure that dedicated sectors for EEPROM are erased
      to avoid a match with a valid page OPCODE.

@par Keywords

EEPROM, Emulation, Flash, Program, Erase, Sectors, OPCODE

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents

 - EEPROM/EEPROM_Emulation/Inc/stm32f7xx_hal_conf.h          HAL Configuration file
 - EEPROM/EEPROM_Emulation/Inc/main.h                        Header for main.c module
 - EEPROM/EEPROM_Emulation/Inc/eeprom.h                      Header for eeprom.c module
 - EEPROM/EEPROM_Emulation/Inc/stm32f7xx_it.h                Header for stm32f7xx_it.c
 - EEPROM/EEPROM_Emulation/Src/main.c                        Main program
 - EEPROM/EEPROM_Emulation/Src/eeprom.c                      EEPROM program
 - EEPROM/EEPROM_Emulation/Src/stm32f7xx_it.c                Interrupt handlers
 - EEPROM/EEPROM_Emulation/Src/system_stm32f7xx.c            STM32F7xx system clock configuration file


@par Hardware and Software environment

  - This application runs on STM32F767ZI devices.

  - This application has been tested with NUCLEO-F767ZI board and can be
    easily tailored to any other supported device and development board.

Table 1. EEPROM application implementation on STM32F769I-Discovery
+------------+--------------------------------------------------------------+-----------------------+
|  Platform  |                  Implementation                              |  Configuration        |
+------------+--------------------------------------------------------------+-----------------------+
|  Firmware  | The EEPROM program is located at 0x08000000. The Flash       |                       |
|            | routines (program/erase) are executed from the Flash memory. |                       |
|            | The size of this program is about 6 Kbytes and programmed on:|  Sector0              |
|            | -------------------------------------------------------------|-----------------------|
|            | The flash zone used for the EEPROM emulation is located at:  |                       |
|            |   -Dual bank flash:   0x08100000                             | 32 MB (Sector12 + 13) |
|            |   -Single bank flash: 0x08010000                             | 64 KB (Sector2 + 3)   |
+------------+--------------------------------------------------------------+-----------------------+

@note Before using the application, ensure that dedicated sectors for EEPROM (Sector2 and Sector3) on Single Bank
      or sectors (Sector12 and Sector13) on Dual Bank mode are erased to avoid a match with a valid page OPCODE.

Following picture illustrates the situation in program memory:

                                Single bank flash            Dual bank flash
  Flash End       0x08200000  /----------------------\    /----------------------\  0x08200000 Flash End
                              |                      |    |                      |
                              |  Sector4 -> Sector11 |    | Sector14 -> Sector23 |
                              |                      |    |                      |
  Sector4 (128KB) 0x08018000  |----------------------|    |----------------------|  0x08108000 Sector14 (64KB)
                              |   Flash used for     |    |  Flash used for      |
                              |   EEPROM emulation   |    |  EEPROM emulation    |
  Sector3 (32KB)  0x08018000  |   mechanism          |    |  mechanism           |  0x08104000 Sector13 (16KB)
                              |                      |    |                      |
                              |                      |    |                      |
  Sector2 (32KB)  0x08010000  |----------------------|    |----------------------|  0x08100000 Sector12 (16KB)
              :               |                      |    |                      |           :
              :               |                      |    |                      |           :
  Sector1 (32KB)  0x08008000  |----------------------|    |----------------------|  0x08004000 Sector1 (16KB)
                              | flash used for       |    | flash used for       |
                              | implement the EEPROM |    | implement the EEPROM |
                              | emulation mechanism  |    | emulation mechanism  |
                              | (Firmware size: 6KB) |    | (Firmware size: 6KB) |
  Sector0 (32KB)  0x08000000  \----------------------/    \---------------------/   0x08000000 Sector0 (16KB)

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
