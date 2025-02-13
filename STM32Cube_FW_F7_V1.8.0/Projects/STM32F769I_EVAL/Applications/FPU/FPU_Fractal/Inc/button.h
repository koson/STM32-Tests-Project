/**
 ******************************************************************************
 * @file    FPU/FPU_Fractal/Inc/button.h
 * @author  MCD Application Team
 * @brief   Header for button.c module.
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
#ifndef __BUTTON_H
#   define __BUTTON_H

#   ifdef __cplusplus
extern "C" {
#   endif

/* Includes ------------------------------------------------------------------*/
#   include "main.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#   define PLAY_BUTTON 0x01
#   define PAUSE_BUTTON 0x02
#   define ZOOM_IN_BUTTON 0x03
#   define ZOOM_OUT_BUTTON 0x04
#   define ZOOM_IN_OFF_BUTTON 0x05
#   define ZOOM_OUT_OFF_BUTTON 0x06
/*****************Play/Pause Params******************/

#   define PLAY_PAUSE_BUTTON_HEIGHT 64
#   define PLAY_PAUSE_BUTTON_WIDTH 64

#   define PLAY_PAUSE_BUTTON_IMAGE_SIZE \
      ((PLAY_PAUSE_BUTTON_HEIGHT * PLAY_PAUSE_BUTTON_WIDTH * ARGB8888_BYTES_PER_PIXEL) / 4)

#   define PLAY_PAUSE_BUTTON_XPOS ((LCD_X_SIZE / 2) - (PLAY_PAUSE_BUTTON_WIDTH / 2))
#   define PLAY_PAUSE_BUTTON_YPOS (LCD_Y_SIZE - PLAY_PAUSE_BUTTON_HEIGHT - (PLAY_PAUSE_BUTTON_HEIGHT / 2))

/*****************Zoom In/Out Params******************/

#   define ZOOM_BUTTON_HEIGHT 64
#   define ZOOM_BUTTON_WIDTH 64

#   define ZOOM_IMAGE_SIZE ((ZOOM_BUTTON_HEIGHT * ZOOM_BUTTON_WIDTH * ARGB8888_BYTES_PER_PIXEL) / 4)

#   define ZOOM_IN_BUTTON_XPOS ((LCD_X_SIZE / 2) + (3 * (ZOOM_BUTTON_WIDTH / 2)))
#   define ZOOM_IN_BUTTON_YPOS (LCD_Y_SIZE - ZOOM_BUTTON_HEIGHT - (ZOOM_BUTTON_HEIGHT / 2))

#   define ZOOM_OUT_BUTTON_XPOS ((LCD_X_SIZE / 2) - (5 * (ZOOM_BUTTON_WIDTH / 2)))
#   define ZOOM_OUT_BUTTON_YPOS (LCD_Y_SIZE - ZOOM_BUTTON_HEIGHT - (ZOOM_BUTTON_HEIGHT / 2))

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Exported Variables --------------------------------------------------------*/
extern const uint32_t Play_Button[PLAY_PAUSE_BUTTON_IMAGE_SIZE];
extern const uint32_t Pause_Button[PLAY_PAUSE_BUTTON_IMAGE_SIZE];
extern const uint32_t ZOOM_IN_Button[ZOOM_IMAGE_SIZE];
extern const uint32_t ZOOM_Out_Button[ZOOM_IMAGE_SIZE];
extern const uint32_t ZOOM_IN_Off_Button[ZOOM_IMAGE_SIZE];
extern const uint32_t ZOOM_Out_Off_Button[ZOOM_IMAGE_SIZE];

#   ifdef __cplusplus
}
#   endif

#endif /* __BUTTON_H */
/*************************** End of file ****************************/
