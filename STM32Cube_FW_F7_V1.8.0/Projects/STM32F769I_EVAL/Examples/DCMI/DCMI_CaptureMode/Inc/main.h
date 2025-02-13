/**
 ******************************************************************************
 * @file    DCMI/DCMI_CaptureMode/Inc/main.h
 * @author  MCD Application Team
 * @brief   Header for main.c module
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
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#   define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#   include "stm32f769i_eval.h"
#   include "stm32f769i_eval_lcd.h"
#   include "stm32f769i_eval_sdram.h"
#   include "stm32f769i_eval_camera.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* LCD Frame Buffer address */

#   define LCD_FRAME_BUFFER 0xC0000000 /* LCD Frame buffer of size 800x480 in ARGB8888 */

#   define CAMERA_FRAME_BUFFER 0xC0177000
#   define LCD_FRAME_BUFFER 0xC0000000

/* #define TEST_RESOLUTION_R160x120         */
/*          */ #define TEST_RESOLUTION_R320x240
/* #define TEST_RESOLUTION_R480x272         */
/* #define TEST_RESOLUTION_R640x480         */

#   if defined(TEST_RESOLUTION_R160x120)
#      define RESOLUTION RESOLUTION_R160x120
#      define XSIZE 160
#      define YSIZE 120
#   endif

#   if defined(TEST_RESOLUTION_R320x240)
#      define RESOLUTION RESOLUTION_R320x240
#      define XSIZE 320
#      define YSIZE 240
#   endif

#   if defined(TEST_RESOLUTION_R480x272)
#      define RESOLUTION RESOLUTION_R480x272
#      define XSIZE 480
#      define YSIZE 272
#   endif

#   if defined(TEST_RESOLUTION_R640x480)
#      define RESOLUTION RESOLUTION_R640x480
#      define XSIZE 640
#      define YSIZE 480
#   endif
#   define XOFFSET ((BSP_LCD_GetXSize() - XSIZE) / 2)
#   define YOFFSET ((BSP_LCD_GetYSize() - YSIZE) / 2)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

   /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
