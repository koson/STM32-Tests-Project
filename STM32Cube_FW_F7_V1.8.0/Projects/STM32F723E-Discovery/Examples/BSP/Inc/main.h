/**
 ******************************************************************************
 * @file    BSP/Inc/main.h
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
#   include "stdio.h"
#   include "stm32f723e_discovery.h"
#   include "stm32f723e_discovery_lcd.h"
#   include "stm32f723e_discovery_psram.h"
#   include "stm32f723e_discovery_audio.h"
#   include "stm32f723e_discovery_qspi.h"
#   include "stm32f723e_discovery_ts.h"
/* Macros --------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
   void (*DemoFunc)(void);
   uint8_t DemoName[50];
   uint32_t DemoIndex;
} BSP_DemoTypedef;

typedef enum
{
   AUDIO_ERROR_NONE = 0,
   AUDIO_ERROR_NOTREADY,
   AUDIO_ERROR_IO,
   AUDIO_ERROR_EOF,
} AUDIO_ErrorTypeDef;

extern const unsigned char stlogo[];
/* Exported global variables ---------------------------------------------------*/
#   ifndef USE_FULL_ASSERT
extern uint16_t ErrorCounter;
#   endif
extern uint32_t PsramTest;
/* Exported constants --------------------------------------------------------*/

/* Memories addresses */
#   define FLASH_CODE_ADDRESS 0x08000000
#   define FLASH_DATA_ADDRESS 0x08040000
#   define SRAM_WRITE_READ_ADDR 0x2000b000

#   define AUDIO_REC_START_ADDR 0x60000000

/* The Audio file is flashed with ST-Link Utility @ flash address =  AUDIO_SRC_FILE_ADDRESS */
#   define AUDIO_SRC_FILE_ADDRESS 0x08040000 /* Audio file address in flash */
#   define AUDIO_FILE_SIZE (180 * 1024) /* 180 KBytes */

/* Exported macro ------------------------------------------------------------*/

#   define COUNT_OF_EXAMPLE(x) (sizeof(x) / sizeof(BSP_DemoTypedef))

#   ifdef USE_FULL_ASSERT
/* Assert activated */
#      define ASSERT(__condition__) \
         do \
         { \
            if (__condition__) \
            { \
               assert_failed(__FILE__, __LINE__); \
               while (1) \
                  ; \
            } \
         } while (0)
#   else
/* Assert not activated : macro has no effect */
#      define ASSERT(__condition__) \
         do \
         { \
            if (__condition__) \
            { \
               ErrorCounter++; \
            } \
         } while (0)
#   endif /* USE_FULL_ASSERT */

/* Exported functions ------------------------------------------------------- */
uint8_t Touchscreen_Calibration(void);
uint16_t TouchScreen_Get_Calibrated_X(uint16_t x);
uint16_t TouchScreen_Get_Calibrated_Y(uint16_t y);
uint8_t TouchScreen_IsCalibrationDone(void);
uint8_t TouchScreen_GetTouchPosition(void);
void Touchscreen_DrawBackground_Circles(uint8_t state);
void Touchscreen_demo1(void);
void Touchscreen_demo2(void);
void Touchscreen_demo3(void);
void AudioPlay_demo(void);
void AudioRecAnalog_demo(void);
void AudioLoopback_demo(void);
void LCD_demo(void);
void Log_demo(void);
uint8_t AUDIO_Process(void);
void QSPI_demo(void);
uint8_t CheckForUserInput(void);
void Toggle_Leds(void);
void Error_Handler(void);
void PSRAM_demo(void);
void PSRAM_DMA_demo(void);
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
