/**
  @page LCD_DSI_ULPM_DataClock : LCD DSI example : enter and exit DSI ULPM Mode on data lanes 
  and clock lane while displaying a picture 800x480 (WVGA) 
  in landscape mode in DSI mode Video Burst on LCD screen.

  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    LCD_DSI/LCD_DSI_ULPM_DataClock/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the LCD DSI enter and exit DSI ULPM Mode on data lane 
  *          and clock lane example.
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

This example provides a description of how to use the embedded LCD DSI controller 
(using IPs LTDC and DSI Host) to drive the KoD LCD mounted on board and manage entry and exit in 
DSI ULPM mode on data lane and clock lane. 
In this mode, the DSI PHY state machine is entering a low power state
on data lane and clock lane.
The DSI Dphy_Clk should be derived from the clock tree PLL.PLLR (ck_plldsi) with a 60 MHz clock for instance
to allow a switch off of the PLL DPHY to save power during the ULPM phase.
When exiting from ULPM, the PLL DPHY is first switched back on and locked, then the DSI DPHY will exit
power down and display again as before.

In this example a WVGA landscape picture (800x480) is displayed on LCD in DSI Mode Video Burst.
On a Tamper button press from the user, the LCD display will be switched off. One second later, 
the DSI PHY Data and clock lane will enter ULPM mode. DphyClk is derived from PLL.PLLR, then
DPHY PLL is switched Off.

After 6 seconds in Off mode, The PLL DPHY is switched back on and locked,
then the ULPM on data and clock lanes will be exited in DPHY DSI state machine.
The LCD will then be switched back on and display the same image as before 
with a text notifying that the exit from ULPM was successful. 

The image candies_800x480_argb8888 is of format ARGB8888 and is initially copied from Flash to SDRAM Frame Buffer.
The LTDC is reading continuously the LCD Frame buffer from SDRAM, sent it to DSI Host which sends it in burst mode (DCS commands)
via the DSI DPHY to the KoD display that decodes DSI packets and refresh its internal Graphic RAM to display on glass.

The Frame number display is managed by Line Event callback in which the Frame number is incremented
each time a line event occurs. When entering to ULPM, The Frame number is unchanged untill the
exit from this mode.

LED1 ON: DSI PHY Data and clock lane in ULPM mode
LED1 OFF: DSI PHY Data and clock lane in run mode
LED3 ON: an error occurred.
@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note The STM32F7xx devices can reach a maximum clock frequency of 216MHz but as this example uses SDRAM,
      the system clock is limited to 200MHz. Indeed proper functioning of the SDRAM is only guaranteed 
      at a maximum system clock frequency of 200MHz.
      
@par Keywords

Graphic, Display, LCD, DSI, MIPI Alliance, Ultra low power mode, Tearing effect, Partial refresh, Single buffer,
LTDC, WVGA, ARGB8888, SDRAM, Burst mode, Data lane, clock lane

@Note  If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
       it is recommended to configure the latters as Write Through.
       This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
       Please, refer to Template project for a typical MPU configuration.

@Note  If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
       The memory base address and size must be properly updated.
       The user needs to manage the cache coherence at application level.

For more details about the MPU configuration and use, please refer to AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�

@par Directory contents

  - LCD_DSI/LCD_DSI_ULPM_DataClock/Inc/stm32f7xx_hal_conf.h          HAL configuration file
  - LCD_DSI/LCD_DSI_ULPM_DataClock/Inc/stm32f7xx_it.h                Interrupt handlers header file
  - LCD_DSI/LCD_DSI_ULPM_DataClock/Inc/main.h                        Header for main.c module
  - LCD_DSI/LCD_DSI_ULPM_DataClock/Inc/candies_800x480_argb8888.h    Image 800x480 in ARGB8888 to display on LCD
  - LCD_DSI/LCD_DSI_ULPM_DataClock/Src/stm32f7xx_it.c                Interrupt handlers
  - LCD_DSI/LCD_DSI_ULPM_DataClock/Src/main.c                        Main program
  - LCD_DSI/LCD_DSI_ULPM_DataClock/Src/stm32f7xx_hal_msp.c           HAL MSP file
  - LCD_DSI/LCD_DSI_ULPM_DataClock/Src/system_stm32f7xx.c            STM32F7xx system source file

@par Hardware and Software environment

  - This example runs on STM32F767xx/STM32F769xx/STM32F777xx/STM32F779xx devices.

  - This example has been tested with STMicroelectronics STM32F769I-EVAL
    board and can be easily tailored to any other supported device
    and development board.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
