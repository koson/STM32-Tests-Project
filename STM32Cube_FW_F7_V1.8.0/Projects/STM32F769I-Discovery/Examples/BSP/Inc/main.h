/**
 ******************************************************************************
 * @file    main.h
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
#   include <stdio.h>
#   include <string.h>
#   include "stm32f7xx_hal.h"
#   include "stm32f769i_discovery.h"
#   include "stm32f769i_discovery_lcd.h"
#   include "stm32f769i_discovery_qspi.h"
#   include "stm32f769i_discovery_ts.h"
#   include "stm32f769i_discovery_audio.h"
#   include "stm32f769i_discovery_sd.h"

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

/* Exported variables --------------------------------------------------------*/
extern const unsigned char stlogo[];
extern __IO uint32_t SdmmcTest;
extern __IO uint32_t SdramTest;
/* Exported constants --------------------------------------------------------*/

/* LCD Frame buffer start address : starts at beginning of SDRAM */
#   define ARBG8888_BYTE_PER_PIXEL 4
#   define LCD_FB_LENGTH ((uint32_t)(OTM8009A_480X800_HEIGHT * OTM8009A_480X800_WIDTH * ARBG8888_BYTE_PER_PIXEL))
#   define SDRAM_WRITE_READ_ADDR ((uint32_t)0xC0177000)

/* The Audio file is flashed with ST-Link Utility @ flash address =  AUDIO_SRC_FILE_ADDRESS */
#   define AUDIO_SRC_FILE_ADDRESS 0x08080000 /* Audio file address in flash */
#   define AUDIO_FILE_SIZE 0x80000

/* Exported macro ------------------------------------------------------------*/
#   define COUNT_OF_EXAMPLE(x) (sizeof(x) / sizeof(BSP_DemoTypedef))

#   ifdef USE_FULL_ASSERT
/* Assert activated */
#      define BSP_TEST_APPLI_ASSERT(__error_condition__) \
         do \
         { \
            if (__error_condition__) \
            { \
               while (1) \
                  ; \
            } \
         } while (0)
#   else
/* Assert not activated : macro has no effect */
#      define BSP_TEST_APPLI_ASSERT(__error_condition__) \
         do \
         { \
            if (__error_condition__) \
            { \
               ; \
            } \
         } while (0)
#   endif /* USE_FULL_ASSERT */

/* Exported functions ------------------------------------------------------- */
uint8_t TouchScreen_GetTouchPosition(void);
void Touchscreen_DrawBackground_Circles(uint8_t state);
void Touchscreen_demo1(void);
void Touchscreen_demo2(void);
void AudioPlay_demo(void);
void AudioLoopback_demo(void);
uint8_t AUDIO_Process(void);
void LCD_demo(void);
void SD_demo(void);
void QSPI_demo(void);
void EEPROM_demo(void);
uint8_t CheckForUserInput(void);
void Toggle_Leds(void);
void Error_Handler(void);
void SDRAM_demo(void);
void SDRAM_DMA_demo(void);
void Log_demo(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
