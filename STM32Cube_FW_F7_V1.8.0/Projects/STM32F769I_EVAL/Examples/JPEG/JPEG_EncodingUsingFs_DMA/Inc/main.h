/**
 ******************************************************************************
 * @file    JPEG/JPEG_EncodingUsingFs_DMA/Inc/main.h
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
#   include "stm32f7xx_hal.h"

#   include "stm32f769i_eval.h"
#   include "stm32f769i_eval_io.h"
#   include "stm32f769i_eval_lcd.h"
#   include "stm32f769i_eval_sdram.h"

/* FatFs includes component */
#   include "ff_gen_drv.h"
#   include "sd_diskio.h"

#   include "jpeg_utils.h"
#   include "encode_dma.h"

/* Exported variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#   define JPEG_CHROMA_SAMPLING \
      JPEG_420_SUBSAMPLING /* Select Chroma Sampling: JPEG_420_SUBSAMPLING, JPEG_422_SUBSAMPLING, JPEG_444_SUBSAMPLING \
                            * \ \
                            */
#   define JPEG_COLOR_SPACE \
      JPEG_YCBCR_COLORSPACE /* Select Color Space: JPEG_YCBCR_COLORSPACE, JPEG_GRAYSCALE_COLORSPACE, \ \
                               JPEG_CMYK_COLORSPACE */
#   define JPEG_IMAGE_QUALITY 75 /* Set Image Quality for Jpeg Encoding */
#   define MAX_INPUT_WIDTH 800 /* Set the Maximum of RGB input images Width to be encoded */
#   define MAX_INPUT_LINES \
      16 /* Set Input buffer lines to 16 for YCbCr420, and 8 for YCbCr422 and YCbCr444 (to save RAM space) */

#   define LCD_FRAME_BUFFER 0xC0000000

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void OnError_Handler(void);
void BMP_GetInfo(FIL* Filename, JPEG_ConfTypeDef* pInfo);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
