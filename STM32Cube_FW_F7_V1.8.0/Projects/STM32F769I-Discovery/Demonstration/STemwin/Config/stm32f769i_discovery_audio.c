/**
  ******************************************************************************
  * @file    stm32f769i_discovery_audio.c
  * @author  MCD Application Team
  * @brief   This file provides the Audio driver for the STM32F769I-DISCOVERY
  *          board.
  @verbatim
  How To use this driver:
  -----------------------
   + This driver supports STM32F7xx devices on STM32F769I-DISCOVERY (MB1225) Evaluation boards.
   + Call the function BSP_AUDIO_OUT_Init(
                                    OutputDevice: physical output mode (OUTPUT_DEVICE_SPEAKER,
                                                  OUTPUT_DEVICE_HEADPHONE or OUTPUT_DEVICE_BOTH)
                                    Volume      : Initial volume to be set (0 is min (mute), 100 is max (100%)
                                    AudioFreq   : Audio frequency in Hz (8000, 16000, 22500, 32000...)
                                                  this parameter is relative to the audio file/stream type.
                                   )
      This function configures all the hardware required for the audio application (codec, I2C, SAI,
      GPIOs, DMA and interrupt if needed). This function returns AUDIO_OK if configuration is OK.
      If the returned value is different from AUDIO_OK or the function is stuck then the communication with
      the codec or the MFX has failed (try to un-plug the power or reset device in this case).
      - OUTPUT_DEVICE_SPEAKER  : only speaker will be set as output for the audio stream.
      - OUTPUT_DEVICE_HEADPHONE: only headphones will be set as output for the audio stream.
      - OUTPUT_DEVICE_BOTH     : both Speaker and Headphone are used as outputs for the audio stream
                                 at the same time.
      Note. On STM32F769I-DISCOVERY SAI_DMA is configured in CIRCULAR mode. Due to this the application
        does NOT need to call BSP_AUDIO_OUT_ChangeBuffer() to assure streaming.
   + Call the function BSP_DISCOVERY_AUDIO_OUT_Play(
                                  pBuffer: pointer to the audio data file address
                                  Size   : size of the buffer to be sent in Bytes
                                 )
      to start playing (for the first time) from the audio file/stream.
   + Call the function BSP_AUDIO_OUT_Pause() to pause playing
   + Call the function BSP_AUDIO_OUT_Resume() to resume playing.
       Note. After calling BSP_AUDIO_OUT_Pause() function for pause, only BSP_AUDIO_OUT_Resume() should be called
          for resume (it is not allowed to call BSP_AUDIO_OUT_Play() in this case).
       Note. This function should be called only when the audio file is played or paused (not stopped).
   + For each mode, you may need to implement the relative callback functions into your code.
      The Callback functions are named AUDIO_OUT_XXX_CallBack() and only their prototypes are declared in
      the stm32f769i_discovery_audio.h file. (refer to the example for more details on the callbacks implementations)
   + To Stop playing, to modify the volume level, the frequency, the audio frame slot,
      the device output mode the mute or the stop, use the functions: BSP_AUDIO_OUT_SetVolume(),
      AUDIO_OUT_SetFrequency(), BSP_AUDIO_OUT_SetAudioFrameSlot(), BSP_AUDIO_OUT_SetOutputMode(),
      BSP_AUDIO_OUT_SetMute() and BSP_AUDIO_OUT_Stop().
   + The driver API and the callback functions are at the end of the stm32f769i_discovery_audio.h file.
 
  Driver architecture:
  --------------------
   + This driver provides the High Audio Layer: consists of the function API exported in the
 stm32f769i_discovery_audio.h file (BSP_AUDIO_OUT_Init(), BSP_AUDIO_OUT_Play() ...)
   + This driver provide also the Media Access Layer (MAL): which consists of functions allowing to access the media
 containing/ providing the audio file/stream. These functions are also included as local functions into the
 stm32f769i_discovery_audio_codec.c file (DFSDMx_Init(), DFSDMx_DeInit(), SAIx_Init() and SAIx_DeInit())

  Known Limitations:
  ------------------
   1- If the TDM Format used to play in parallel 2 audio Stream (the first Stream is configured in codec SLOT0 and
 second Stream in SLOT1) the Pause/Resume, volume and mute feature will control the both streams. 2- Parsing of audio
 file is not implemented (in order to determine audio file properties: Mono/Stereo, Data size, File size, Audio
 Frequency, Audio Data header size ...). The configuration is fixed for the given audio file. 3- Supports only Stereo
 audio streaming. 4- Supports only 16-bits audio data size.
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
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
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f769i_discovery_audio.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup STM32F769I_DISCOVERY
 * @{
 */

/** @defgroup STM32F769I_DISCOVERY_AUDIO STM32F769I_DISCOVERY_AUDIO
 * @brief This file includes the low layer driver for wm8994 Audio Codec
 *        available on STM32F769I-DISCOVERY discoveryuation board(MB1219).
 * @{
 */

/** @defgroup STM32F769I_DISCOVERY_AUDIO_Private_Types STM32F769I_DISCOVERY_AUDIO Private Types
 * @{
 */
typedef struct
{
   uint16_t* pRecBuf; /* Pointer to record user buffer */
   uint32_t RecSize; /* Size to record in mono, double size to record in stereo */
} AUDIOIN_TypeDef;

/**
 * @}
 */

/** @defgroup STM32F769I_DISCOVERY_AUDIO_Private_Defines STM32F769I_DISCOVERY_AUDIO Private Defines
 * @{
 */
/**
 * @}
 */

/** @defgroup STM32F769I_DISCOVERY_AUDIO_Private_Macros STM32F769I_DISCOVERY_AUDIO Private Macros
 * @{
 */
/*### RECORD ###*/
#define DFSDM_OVER_SAMPLING(__FREQUENCY__) \
   (__FREQUENCY__ == AUDIO_FREQUENCY_8K) \
      ? 256 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) \
           ? 256 \
           : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) \
                ? 128 \
                : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) \
                     ? 128 \
                     : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) \
                          ? 64 \
                          : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 64 \
                                                                   : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 40 : 20

#define DFSDM_CLOCK_DIVIDER(__FREQUENCY__) \
   (__FREQUENCY__ == AUDIO_FREQUENCY_8K) \
      ? 24 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) \
           ? 4 \
           : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) \
                ? 24 \
                : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) \
                     ? 4 \
                     : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) \
                          ? 24 \
                          : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 4 \
                                                                   : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 25 : 25

#define DFSDM_FILTER_ORDER(__FREQUENCY__) \
   (__FREQUENCY__ == AUDIO_FREQUENCY_8K) \
      ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) \
           ? DFSDM_FILTER_SINC3_ORDER \
           : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) \
                ? DFSDM_FILTER_SINC3_ORDER \
                : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) \
                     ? DFSDM_FILTER_SINC3_ORDER \
                     : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) \
                          ? DFSDM_FILTER_SINC4_ORDER \
                          : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) \
                               ? DFSDM_FILTER_SINC3_ORDER \
                               : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? DFSDM_FILTER_SINC3_ORDER \
                                                                        : DFSDM_FILTER_SINC5_ORDER

#define DFSDM_RIGHT_BIT_SHIFT(__FREQUENCY__) \
   (__FREQUENCY__ == AUDIO_FREQUENCY_8K) \
      ? 8 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) \
           ? 8 \
           : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) \
                ? 3 \
                : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) \
                     ? 4 \
                     : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) \
                          ? 7 \
                          : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 0 \
                                                                   : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 0 : 4

/* Saturate the record PCM sample */
#define SaturaLH(N, L, H) (((N) < (L)) ? (L) : (((N) > (H)) ? (H) : (N)))
/**
 * @}
 */

/** @defgroup STM32F769I_DISCOVERY_AUDIO_Private_Variables STM32F769I_DISCOVERY_AUDIO Private Variables
 * @{
 */
/*### PLAY ###*/
AUDIO_DrvTypeDef* audio_drv;
SAI_HandleTypeDef haudio_out_sai;

/*### RECORD ###*/
AUDIOIN_TypeDef hAudioIn;

DFSDM_Channel_HandleTypeDef haudio_in_top_leftchannel;
DFSDM_Channel_HandleTypeDef haudio_in_top_rightchannel;
DFSDM_Filter_HandleTypeDef haudio_in_top_leftfilter;
DFSDM_Filter_HandleTypeDef haudio_in_top_rightfilter;
DMA_HandleTypeDef hdma_top_left;
DMA_HandleTypeDef hdma_top_right;

DFSDM_Channel_HandleTypeDef haudio_in_buttom_leftchannel;
DFSDM_Channel_HandleTypeDef haudio_in_buttom_rightchannel;
DFSDM_Filter_HandleTypeDef haudio_in_buttom_leftfilter;
DFSDM_Filter_HandleTypeDef haudio_in_buttom_rightfilter;
DMA_HandleTypeDef hdma_buttom_left;
DMA_HandleTypeDef hdma_buttom_right;

/* Buffers for right and left samples */
int32_t* pScratchBuff[DEFAULT_AUDIO_IN_CHANNEL_NBR];
__IO int32_t ScratchSize;

/* Buffers status flags */
uint32_t DmaTopLeftRecHalfCplt = 0;
uint32_t DmaTopLeftRecCplt = 0;
uint32_t DmaTopRightRecHalfCplt = 0;
uint32_t DmaTopRightRecCplt = 0;

uint32_t DmaButtomLeftRecHalfCplt = 0;
uint32_t DmaButtomLeftRecCplt = 0;
uint32_t DmaButtomRightRecHalfCplt = 0;
uint32_t DmaButtomRightRecCplt = 0;

/* Application Buffer Trigger */
__IO uint32_t AppBuffTrigger = 0;
__IO uint32_t AppBuffHalf = 0;

uint16_t __IO AudioInVolume = DEFAULT_AUDIO_IN_VOLUME;
/**
 * @}
 */

/** @defgroup STM32F769I_DISCOVERY_AUDIO_Private_Function_Prototypes STM32F769I_DISCOVERY_AUDIO Private Function
 * Prototypes
 * @{
 */
static void SAIx_Out_Init(uint32_t AudioFreq);
static void SAIx_Out_DeInit(void);
static void DFSDMx_ChannelMspInit(void);
static void DFSDMx_FilterMspInit(void);
static void DFSDMx_ChannelMspDeInit(void);
static void DFSDMx_FilterMspDeInit(void);
static uint8_t DFSDMx_Init(uint32_t AudioFreq);
static uint8_t DFSDMx_DeInit(void);

/**
 * @}
 */

/** @defgroup STM32F769I_DISCOVERY_AUDIO_out_Private_Functions STM32F769I_DISCOVERY_AUDIO_Out Private Functions
 * @{
 */

/**
 * @brief  Configures the audio peripherals.
 * @param  OutputDevice: OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
 *                       or OUTPUT_DEVICE_BOTH.
 * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
 * @param  AudioFreq: Audio frequency used to play the audio stream.
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_OUT_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq)
{
   uint8_t ret = AUDIO_ERROR;
   uint32_t deviceid = 0x00;

   /* Disable SAI */
   SAIx_Out_DeInit();

   /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */
   BSP_AUDIO_OUT_ClockConfig(&haudio_out_sai, AudioFreq, NULL);

   /* SAI data transfer preparation:
   Prepare the Media to be used for the audio transfer from memory to SAI peripheral */
   haudio_out_sai.Instance = AUDIO_OUT_SAIx;
   if (HAL_SAI_GetState(&haudio_out_sai) == HAL_SAI_STATE_RESET)
   {
      /* Init the SAI MSP: this __weak function can be redefined by the application*/
      BSP_AUDIO_OUT_MspInit(&haudio_out_sai, NULL);
   }
   SAIx_Out_Init(AudioFreq);

   /* wm8994 codec initialization */
   deviceid = wm8994_drv.ReadID(AUDIO_I2C_ADDRESS);

   if ((deviceid) == WM8994_ID)
   {
      /* Reset the Codec Registers */
      wm8994_drv.Reset(AUDIO_I2C_ADDRESS);
      /* Initialize the audio driver structure */
      audio_drv = &wm8994_drv;
      ret = AUDIO_OK;
   }
   else
   {
      ret = AUDIO_ERROR;
   }

   if (ret == AUDIO_OK)
   {
      /* Initialize the codec internal registers */
      audio_drv->Init(AUDIO_I2C_ADDRESS, OutputDevice, Volume, AudioFreq);
   }

   return ret;
}

/**
 * @brief  Starts playing audio stream from a data buffer for a determined size.
 * @param  pBuffer: Pointer to the buffer
 * @param  Size: Number of audio data BYTES.
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_OUT_Play(uint16_t* pBuffer, uint32_t Size)
{
   /* Call the audio Codec Play function */
   if (audio_drv->Play(AUDIO_I2C_ADDRESS, (uint16_t*)pBuffer, Size) != 0)
   {
      return AUDIO_ERROR;
   }
   else
   {
      /* Update the Media layer and enable it for play */
      HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t*)pBuffer, DMA_MAX(Size / AUDIODATA_SIZE));

      return AUDIO_OK;
   }
}

/**
 * @brief  Sends n-Bytes on the SAI interface.
 * @param  pData: pointer on data address
 * @param  Size: number of data to be written
 * @retval None
 */
void BSP_AUDIO_OUT_ChangeBuffer(uint16_t* pData, uint16_t Size)
{
   HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t*)pData, Size);
}

/**
 * @brief  This function Pauses the audio file stream. In case
 *         of using DMA, the DMA Pause feature is used.
 * @WARNING When calling BSP_AUDIO_OUT_Pause() function for pause, only
 *          BSP_AUDIO_OUT_Resume() function should be called for resume (use of BSP_AUDIO_OUT_Play()
 *          function for resume could lead to unexpected behaviour).
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_OUT_Pause(void)
{
   /* Call the Audio Codec Pause/Resume function */
   if (audio_drv->Pause(AUDIO_I2C_ADDRESS) != 0)
   {
      return AUDIO_ERROR;
   }
   else
   {
      /* Call the Media layer pause function */
      HAL_SAI_DMAPause(&haudio_out_sai);

      /* Return AUDIO_OK when all operations are correctly done */
      return AUDIO_OK;
   }
}

/**
 * @brief  This function  Resumes the audio file stream.
 * @WARNING When calling BSP_AUDIO_OUT_Pause() function for pause, only
 *          BSP_AUDIO_OUT_Resume() function should be called for resume (use of BSP_AUDIO_OUT_Play()
 *          function for resume could lead to unexpected behaviour).
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_OUT_Resume(void)
{
   /* Call the Audio Codec Pause/Resume function */
   if (audio_drv->Resume(AUDIO_I2C_ADDRESS) != 0)
   {
      return AUDIO_ERROR;
   }
   else
   {
      /* Call the Media layer pause/resume function */
      HAL_SAI_DMAResume(&haudio_out_sai);

      /* Return AUDIO_OK when all operations are correctly done */
      return AUDIO_OK;
   }
}

/**
 * @brief  Stops audio playing and Power down the Audio Codec.
 * @param  Option: could be one of the following parameters
 *           - CODEC_PDWN_SW: for software power off (by writing registers).
 *                            Then no need to reconfigure the Codec after power on.
 *           - CODEC_PDWN_HW: completely shut down the codec (physically).
 *                            Then need to reconfigure the Codec after power on.
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_OUT_Stop(uint32_t Option)
{
   /* Call the Media layer stop function */
   HAL_SAI_DMAStop(&haudio_out_sai);

   /* Call Audio Codec Stop function */
   if (audio_drv->Stop(AUDIO_I2C_ADDRESS, Option) != 0)
   {
      return AUDIO_ERROR;
   }
   else
   {
      if (Option == CODEC_PDWN_HW)
      {
         /* Wait at least 100us */
         HAL_Delay(1);
      }
      /* Return AUDIO_OK when all operations are correctly done */
      return AUDIO_OK;
   }
}

/**
 * @brief  Controls the current audio volume level.
 * @param  Volume: Volume level to be set in percentage from 0% to 100% (0 for
 *         Mute and 100 for Max volume level).
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume)
{
   /* Call the codec volume control function with converted volume value */
   if (audio_drv->SetVolume(AUDIO_I2C_ADDRESS, Volume) != 0)
   {
      return AUDIO_ERROR;
   }
   else
   {
      /* Return AUDIO_OK when all operations are correctly done */
      return AUDIO_OK;
   }
}

/**
 * @brief  Enables or disables the MUTE mode by software
 * @param  Cmd: Could be AUDIO_MUTE_ON to mute sound or AUDIO_MUTE_OFF to
 *         unmute the codec and restore previous volume level.
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd)
{
   /* Call the Codec Mute function */
   if (audio_drv->SetMute(AUDIO_I2C_ADDRESS, Cmd) != 0)
   {
      return AUDIO_ERROR;
   }
   else
   {
      /* Return AUDIO_OK when all operations are correctly done */
      return AUDIO_OK;
   }
}

/**
 * @brief  Switch dynamically (while audio file is played) the output target
 *         (speaker or headphone).
 * @param  Output: The audio output target: OUTPUT_DEVICE_SPEAKER,
 *         OUTPUT_DEVICE_HEADPHONE or OUTPUT_DEVICE_BOTH
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_OUT_SetOutputMode(uint8_t Output)
{
   /* Call the Codec output device function */
   if (audio_drv->SetOutputMode(AUDIO_I2C_ADDRESS, Output) != 0)
   {
      return AUDIO_ERROR;
   }
   else
   {
      /* Return AUDIO_OK when all operations are correctly done */
      return AUDIO_OK;
   }
}

/**
 * @brief  Updates the audio frequency.
 * @param  AudioFreq: Audio frequency used to play the audio stream.
 * @note   This API should be called after the BSP_AUDIO_OUT_Init() to adjust the
 *         audio frequency.
 * @retval None
 */
void BSP_AUDIO_OUT_SetFrequency(uint32_t AudioFreq)
{
   /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */
   BSP_AUDIO_OUT_ClockConfig(&haudio_out_sai, AudioFreq, NULL);

   /* Disable SAI peripheral to allow access to SAI internal registers */
   __HAL_SAI_DISABLE(&haudio_out_sai);

   /* Update the SAI audio frequency configuration */
   haudio_out_sai.Init.AudioFrequency = AudioFreq;
   HAL_SAI_Init(&haudio_out_sai);

   /* Enable SAI peripheral to generate MCLK */
   __HAL_SAI_ENABLE(&haudio_out_sai);
}

/**
 * @brief  Updates the Audio frame slot configuration.
 * @param  AudioFrameSlot: specifies the audio Frame slot
 *         This parameter can be any value of @ref CODEC_AudioFrame_SLOT_TDMMode
 * @note   This API should be called after the BSP_AUDIO_OUT_Init() to adjust the
 *         audio frame slot.
 * @retval None
 */
void BSP_AUDIO_OUT_SetAudioFrameSlot(uint32_t AudioFrameSlot)
{
   /* Disable SAI peripheral to allow access to SAI internal registers */
   __HAL_SAI_DISABLE(&haudio_out_sai);

   /* Update the SAI audio frame slot configuration */
   haudio_out_sai.SlotInit.SlotActive = AudioFrameSlot;
   HAL_SAI_Init(&haudio_out_sai);

   /* Enable SAI peripheral to generate MCLK */
   __HAL_SAI_ENABLE(&haudio_out_sai);
}

/**
 * @brief  Deinit the audio peripherals.
 * @retval None
 */
void BSP_AUDIO_OUT_DeInit(void)
{
   SAIx_Out_DeInit();
   /* DeInit the SAI MSP : this __weak function can be rewritten by the application */
   BSP_AUDIO_OUT_MspDeInit(&haudio_out_sai, NULL);
}

/**
 * @brief  Tx Transfer completed callbacks.
 * @param  hsai: SAI handle
 * @retval None
 */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef* hsai)
{
   /* Manage the remaining file size and new address offset: This function
      should be coded by user (its prototype is already declared in stm32f769i_discovery_audio.h) */
   BSP_AUDIO_OUT_TransferComplete_CallBack();
}

/**
 * @brief  Tx Half Transfer completed callbacks.
 * @param  hsai: SAI handle
 * @retval None
 */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef* hsai)
{
   /* Manage the remaining file size and new address offset: This function
      should be coded by user (its prototype is already declared in stm32f769i_discovery_audio.h) */
   BSP_AUDIO_OUT_HalfTransfer_CallBack();
}

/**
 * @brief  SAI error callbacks.
 * @param  hsai: SAI handle
 * @retval None
 */
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef* hsai)
{
   BSP_AUDIO_OUT_Error_CallBack();
}

/**
 * @brief  Manages the DMA full Transfer complete event.
 * @retval None
 */
__weak void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
}

/**
 * @brief  Manages the DMA Half Transfer complete event.
 * @retval None
 */
__weak void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
}

/**
 * @brief  Manages the DMA FIFO error event.
 * @retval None
 */
__weak void BSP_AUDIO_OUT_Error_CallBack(void)
{
}

/**
 * @brief  Initializes BSP_AUDIO_OUT MSP.
 * @param  hsai: SAI handle
 * @retval None
 */
__weak void BSP_AUDIO_OUT_MspInit(SAI_HandleTypeDef* hsai, void* Params)
{
   static DMA_HandleTypeDef hdma_sai_tx;
   GPIO_InitTypeDef gpio_init_structure;

   /* Enable SAI clock */
   AUDIO_OUT_SAIx_CLK_ENABLE();

   /* Enable GPIO clock */
   AUDIO_OUT_SAIx_MCLK_ENABLE();
   AUDIO_OUT_SAIx_SD_FS_CLK_ENABLE();

   /* CODEC_SAI pins configuration: FS, SCK, MCK and SD pins ------------------*/
   gpio_init_structure.Pin = AUDIO_OUT_SAIx_FS_PIN | AUDIO_OUT_SAIx_SCK_PIN | AUDIO_OUT_SAIx_SD_PIN;
   gpio_init_structure.Mode = GPIO_MODE_AF_PP;
   gpio_init_structure.Pull = GPIO_NOPULL;
   gpio_init_structure.Speed = GPIO_SPEED_HIGH;
   gpio_init_structure.Alternate = AUDIO_OUT_SAIx_AF;
   HAL_GPIO_Init(AUDIO_OUT_SAIx_SD_FS_SCK_GPIO_PORT, &gpio_init_structure);

   gpio_init_structure.Pin = AUDIO_OUT_SAIx_MCLK_PIN;
   HAL_GPIO_Init(AUDIO_OUT_SAIx_MCLK_GPIO_PORT, &gpio_init_structure);

   /* Enable the DMA clock */
   AUDIO_OUT_SAIx_DMAx_CLK_ENABLE();

   if (hsai->Instance == AUDIO_OUT_SAIx)
   {
      /* Configure the hdma_saiTx handle parameters */
      hdma_sai_tx.Init.Channel = AUDIO_OUT_SAIx_DMAx_CHANNEL;
      hdma_sai_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
      hdma_sai_tx.Init.PeriphInc = DMA_PINC_DISABLE;
      hdma_sai_tx.Init.MemInc = DMA_MINC_ENABLE;
      hdma_sai_tx.Init.PeriphDataAlignment = AUDIO_OUT_SAIx_DMAx_PERIPH_DATA_SIZE;
      hdma_sai_tx.Init.MemDataAlignment = AUDIO_OUT_SAIx_DMAx_MEM_DATA_SIZE;
      hdma_sai_tx.Init.Mode = DMA_CIRCULAR;
      hdma_sai_tx.Init.Priority = DMA_PRIORITY_HIGH;
      hdma_sai_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
      hdma_sai_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
      hdma_sai_tx.Init.MemBurst = DMA_MBURST_SINGLE;
      hdma_sai_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;

      hdma_sai_tx.Instance = AUDIO_OUT_SAIx_DMAx_STREAM;

      /* Associate the DMA handle */
      __HAL_LINKDMA(hsai, hdmatx, hdma_sai_tx);

      /* Deinitialize the Stream for new transfer */
      HAL_DMA_DeInit(&hdma_sai_tx);

      /* Configure the DMA Stream */
      HAL_DMA_Init(&hdma_sai_tx);
   }

   /* SAI DMA IRQ Channel configuration */
   HAL_NVIC_SetPriority(AUDIO_OUT_SAIx_DMAx_IRQ, AUDIO_OUT_IRQ_PREPRIO, 0);
   HAL_NVIC_EnableIRQ(AUDIO_OUT_SAIx_DMAx_IRQ);
}

/**
 * @brief  Deinitializes SAI MSP.
 * @param  hsai: SAI handle
 * @retval None
 */
__weak void BSP_AUDIO_OUT_MspDeInit(SAI_HandleTypeDef* hsai, void* Params)
{
   GPIO_InitTypeDef gpio_init_structure;

   /* SAI DMA IRQ Channel deactivation */
   HAL_NVIC_DisableIRQ(AUDIO_OUT_SAIx_DMAx_IRQ);

   if (hsai->Instance == AUDIO_OUT_SAIx)
   {
      /* Deinitialize the DMA stream */
      HAL_DMA_DeInit(hsai->hdmatx);
   }

   /* Disable SAI peripheral */
   __HAL_SAI_DISABLE(hsai);

   /* Deactivates CODEC_SAI pins FS, SCK, MCK and SD by putting them in input mode */
   gpio_init_structure.Pin = AUDIO_OUT_SAIx_FS_PIN | AUDIO_OUT_SAIx_SCK_PIN | AUDIO_OUT_SAIx_SCK_PIN;
   HAL_GPIO_DeInit(AUDIO_OUT_SAIx_SD_FS_SCK_GPIO_PORT, gpio_init_structure.Pin);

   gpio_init_structure.Pin = AUDIO_OUT_SAIx_MCLK_PIN;
   HAL_GPIO_DeInit(AUDIO_OUT_SAIx_MCLK_GPIO_PORT, gpio_init_structure.Pin);

   /* Disable SAI clock */
   AUDIO_OUT_SAIx_CLK_DISABLE();

   /* GPIO pins clock and DMA clock can be shut down in the applic
      by surcharging this __weak function */
}

/**
 * @brief  Clock Config.
 * @param  hsai: might be required to set audio peripheral predivider if any.
 * @param  AudioFreq: Audio frequency used to play the audio stream.
 * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
 *         Being __weak it can be overwritten by the application
 * @retval None
 */
__weak void BSP_AUDIO_OUT_ClockConfig(SAI_HandleTypeDef* hsai, uint32_t AudioFreq, void* Params)
{
   RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;

   HAL_RCCEx_GetPeriphCLKConfig(&rcc_ex_clk_init_struct);

   /* Set the PLL configuration according to the audio frequency */
   if ((AudioFreq == AUDIO_FREQUENCY_11K) || (AudioFreq == AUDIO_FREQUENCY_22K) || (AudioFreq == AUDIO_FREQUENCY_44K))
   {
      /* Configure PLLSAI prescalers */
      /* PLLSAI_VCO: VCO_429M
      SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
      SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 214.5/19 = 11.289 Mhz */
      rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
      rcc_ex_clk_init_struct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;
      rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 429;
      rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 2;
      rcc_ex_clk_init_struct.PLLI2SDivQ = 19;

      HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
   }
   else /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_48K, AUDIO_FREQUENCY_96K */
   {
      /* SAI clock config
      PLLSAI_VCO: VCO_344M
      SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 344/7 = 49.142 Mhz
      SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 49.142/1 = 49.142 Mhz */
      rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
      rcc_ex_clk_init_struct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;
      rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 344;
      rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 7;
      rcc_ex_clk_init_struct.PLLI2SDivQ = 1;

      HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
   }
}

/*******************************************************************************
                            Static Functions
*******************************************************************************/

/**
 * @brief  Initializes the Audio Codec audio interface (SAI).
 * @param  AudioFreq: Audio frequency to be configured for the SAI peripheral.
 * @note   The default SlotActive configuration is set to CODEC_AUDIOFRAME_SLOT_0123
 *         and user can update this configuration using
 * @retval None
 */
static void SAIx_Out_Init(uint32_t AudioFreq)
{
   /* Initialize the haudio_out_sai Instance parameter */
   haudio_out_sai.Instance = AUDIO_OUT_SAIx;

   /* Disable SAI peripheral to allow access to SAI internal registers */
   __HAL_SAI_DISABLE(&haudio_out_sai);

   /* Configure SAI_Block_x
   LSBFirst: Disabled
   DataSize: 16 */
   haudio_out_sai.Init.MonoStereoMode = SAI_STEREOMODE;
   haudio_out_sai.Init.AudioFrequency = AudioFreq;
   haudio_out_sai.Init.AudioMode = SAI_MODEMASTER_TX;
   haudio_out_sai.Init.NoDivider = SAI_MASTERDIVIDER_ENABLED;
   haudio_out_sai.Init.Protocol = SAI_FREE_PROTOCOL;
   haudio_out_sai.Init.DataSize = SAI_DATASIZE_16;
   haudio_out_sai.Init.FirstBit = SAI_FIRSTBIT_MSB;
   haudio_out_sai.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
   haudio_out_sai.Init.Synchro = SAI_ASYNCHRONOUS;
   haudio_out_sai.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLED;
   haudio_out_sai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
   haudio_out_sai.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
   haudio_out_sai.Init.CompandingMode = SAI_NOCOMPANDING;
   haudio_out_sai.Init.TriState = SAI_OUTPUT_NOTRELEASED;
   haudio_out_sai.Init.Mckdiv = 0;

   /* Configure SAI_Block_x Frame
   Frame Length: 64
   Frame active Length: 32
   FS Definition: Start frame + Channel Side identification
   FS Polarity: FS active Low
   FS Offset: FS asserted one bit before the first bit of slot 0 */
   haudio_out_sai.FrameInit.FrameLength = 64;
   haudio_out_sai.FrameInit.ActiveFrameLength = 32;
   haudio_out_sai.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
   haudio_out_sai.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
   haudio_out_sai.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

   /* Configure SAI Block_x Slot
   Slot First Bit Offset: 0
   Slot Size  : 16
   Slot Number: 4
   Slot Active: All slot actives */
   haudio_out_sai.SlotInit.FirstBitOffset = 0;
   haudio_out_sai.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
   haudio_out_sai.SlotInit.SlotNumber = 4;
   haudio_out_sai.SlotInit.SlotActive = CODEC_AUDIOFRAME_SLOT_0123;

   HAL_SAI_Init(&haudio_out_sai);

   /* Enable SAI peripheral to generate MCLK */
   __HAL_SAI_ENABLE(&haudio_out_sai);
}

/**
 * @brief  Deinitializes the Audio Codec audio interface (SAI).
 * @retval None
 */
static void SAIx_Out_DeInit(void)
{
   /* Initialize the haudio_out_sai Instance parameter */
   haudio_out_sai.Instance = AUDIO_OUT_SAIx;

   /* Disable SAI peripheral */
   __HAL_SAI_DISABLE(&haudio_out_sai);

   HAL_SAI_DeInit(&haudio_out_sai);
}

/**
 * @}
 */

/** @defgroup STM32F769I_DISCOVERY_AUDIO_out_Private_Functions STM32F769I_DISCOVERY_AUDIO_Out Private Functions
 * @{
 */
/**
 * @brief  Initialize wave recording.
 * @param  AudioFreq: Audio frequency to be configured for the DFSDM peripheral.
 * @param  BitRes: Audio frequency to be configured for the DFSDM peripheral.
 * @param  ChnlNbr: Audio frequency to be configured for the DFSDM peripheral.
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_IN_Init(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{
   /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */
   BSP_AUDIO_IN_ClockConfig(&haudio_in_top_leftfilter, AudioFreq, NULL);

   /* Init the SAI MSP: this __weak function can be redefined by the application*/
   BSP_AUDIO_IN_MspInit();

   /* Initializes DFSDM peripheral */
   DFSDMx_Init(AudioFreq);

   /* Return AUDIO_OK when all operations are correctly done */
   return AUDIO_OK;
}

/**
 * @brief  Allocate channel buffer scratch
 * @param  pScratch : pointer to scratch tables.
 * @param  size of scratch buffer
 */
uint8_t BSP_AUDIO_IN_AllocScratch(int32_t* pScratch, uint32_t size)
{
   uint32_t idx;

   ScratchSize = (size / (DEFAULT_AUDIO_IN_CHANNEL_NBR));

   /* copy scratch pointers */
   for (idx = 0; idx < DEFAULT_AUDIO_IN_CHANNEL_NBR; idx++)
   {
      pScratchBuff[idx] = (int32_t*)(pScratch + (idx * ScratchSize));
   }
   /* Return AUDIO_OK */
   return AUDIO_OK;
}

/**
 * @brief  Start audio recording.
 * @param  pbuf: Main buffer pointer for the recorded data storing
 * @param  size: Current size of the recorded buffer
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_IN_Record(uint16_t* pbuf, uint32_t size)
{
   hAudioIn.pRecBuf = pbuf;
   hAudioIn.RecSize = size;
   /* Reset Application Buffer Trigger */
   AppBuffTrigger = 0;
   AppBuffHalf = 0;

   //  /* Call the Media layer start function for buttom right channel */
   //  if(HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_buttom_rightfilter, pScratchBuff[2], ScratchSize))
   //  {
   //    return AUDIO_ERROR;
   //  }
   //
   //  /* Call the Media layer start function for buttom left channel */
   //  if(HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_buttom_leftfilter, pScratchBuff[3], ScratchSize))
   //  {
   //    return AUDIO_ERROR;
   //  }

   /* Call the Media layer start function for top right channel */
   if (HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_top_rightfilter, pScratchBuff[0], ScratchSize))
   {
      return AUDIO_ERROR;
   }

   /* Call the Media layer start function for top left channel */
   if (HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_top_leftfilter, pScratchBuff[1], ScratchSize))
   {
      return AUDIO_ERROR;
   }

   /* Return AUDIO_OK when all operations are correctly done */
   return AUDIO_OK;
}

/**
 * @brief  Stop audio recording.
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_IN_Stop(void)
{
   AppBuffTrigger = 0;
   AppBuffHalf = 0;

   /* Call the Media layer stop function for top right channel */
   if (HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_top_rightfilter))
   {
      return AUDIO_ERROR;
   }

   /* Call the Media layer stop function for top left channel */
   if (HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_top_leftfilter))
   {
      return AUDIO_ERROR;
   }

   //  /* Call the Media layer stop function for buttom right channel */
   //  if(HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_buttom_rightfilter))
   //  {
   //    return AUDIO_ERROR;
   //  }
   //
   //  /* Call the Media layer stop function for buttom left channel */
   //  if(HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_buttom_leftfilter))
   //  {
   //    return AUDIO_ERROR;
   //  }

   /* Return AUDIO_OK when all operations are correctly done */
   return AUDIO_OK;
}

/**
 * @brief  Pause the audio file stream.
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_IN_Pause(void)
{
   /* Call the Media layer stop function */
   if (HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_top_rightfilter))
   {
      return AUDIO_ERROR;
   }

   /* Call the Media layer stop function */
   if (HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_top_leftfilter))
   {
      return AUDIO_ERROR;
   }

   //  /* Call the Media layer stop function */
   //  if(HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_buttom_rightfilter))
   //  {
   //    return AUDIO_ERROR;
   //  }
   //
   //  /* Call the Media layer stop function */
   //  if(HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&haudio_in_buttom_leftfilter))
   //  {
   //    return AUDIO_ERROR;
   //  }

   /* Return AUDIO_OK when all operations are correctly done */
   return AUDIO_OK;
}

/**
 * @brief  Resume the audio file stream.
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
uint8_t BSP_AUDIO_IN_Resume(void)
{
   /* Call the Media layer start function for top right channel */
   if (HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_top_rightfilter, pScratchBuff[0], ScratchSize))
   {
      return AUDIO_ERROR;
   }

   /* Call the Media layer start function for top left channel */
   if (HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_top_leftfilter, pScratchBuff[1], ScratchSize))
   {
      return AUDIO_ERROR;
   }
   //
   //  /* Call the Media layer start function for buttom right channel */
   //  if(HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_buttom_rightfilter, pScratchBuff[2], ScratchSize))
   //  {
   //    return AUDIO_ERROR;
   //  }
   //
   //  /* Call the Media layer start function for buttom left channel */
   //  if(HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&haudio_in_buttom_leftfilter, pScratchBuff[3], ScratchSize))
   //  {
   //    return AUDIO_ERROR;
   //  }

   /* Return AUDIO_OK when all operations are correctly done */
   return AUDIO_OK;
}

/**
 * @brief  Deinit the audio IN peripherals.
 * @retval None
 */
void BSP_AUDIO_IN_DeInit(void)
{
   BSP_AUDIO_IN_MspDeInit();
   DFSDMx_DeInit();
}

/**
  * @brief  Regular conversion complete callback.
  * @note   In interrupt mode, user has to read conversion value in this function
            using HAL_DFSDM_FilterGetRegularValue.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef* hdfsdm_filter)
{
   uint32_t index = 0;

   if (hdfsdm_filter == &haudio_in_top_leftfilter)
   {
      DmaTopLeftRecCplt = 1;
   }
   else if (hdfsdm_filter == &haudio_in_top_rightfilter)
   {
      DmaTopRightRecCplt = 1;
   }
   //  else if(hdfsdm_filter == &haudio_in_buttom_leftfilter)
   //  {
   //    DmaButtomLeftRecCplt = 1;
   //  }
   //  else
   //  {
   //    DmaButtomRightRecCplt = 1;
   //  }
   if ((DmaTopLeftRecCplt == 1) &&
       (DmaTopRightRecCplt == 1))   // && (DmaButtomLeftRecCplt == 1) && (DmaButtomRightRecCplt == 1))
   {
      for (index = (ScratchSize / 2); index < ScratchSize; index++)
      {
         hAudioIn.pRecBuf[AppBuffTrigger] = (uint16_t)(SaturaLH((pScratchBuff[0][index] >> 8), -32760, 32760));
         hAudioIn.pRecBuf[AppBuffTrigger + 1] = (uint16_t)(SaturaLH((pScratchBuff[1][index] >> 8), -32760, 32760));
         //      hAudioIn.pRecBuf[AppBuffTrigger + 2] = (uint16_t)(SaturaLH((pScratchBuff[2][index] >> 8), -32760,
         //      32760)); hAudioIn.pRecBuf[AppBuffTrigger + 3] = (uint16_t)(SaturaLH((pScratchBuff[3][index] >> 8),
         //      -32760, 32760));
         AppBuffTrigger += 2;   // 4
      }
      DmaTopLeftRecCplt = 0;
      DmaTopRightRecCplt = 0;
      DmaButtomLeftRecCplt = 0;
      DmaButtomRightRecCplt = 0;
   }

   /* Call Half Transfer Complete callback */
   if ((AppBuffTrigger == hAudioIn.RecSize / 2) && (AppBuffHalf == 0))
   {
      AppBuffHalf = 1;
      BSP_AUDIO_IN_HalfTransfer_CallBack();
   }
   /* Call Transfer Complete callback */
   if (AppBuffTrigger == hAudioIn.RecSize)
   {
      /* Reset Application Buffer Trigger */
      AppBuffTrigger = 0;
      AppBuffHalf = 0;
      /* Call the record update function to get the next buffer to fill and its size (size is ignored) */
      BSP_AUDIO_IN_TransferComplete_CallBack();
   }
}

/**
 * @brief  Half regular conversion complete callback.
 * @param  hdfsdm_filter : DFSDM filter handle.
 * @retval None
 */
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef* hdfsdm_filter)
{
   uint32_t index = 0;

   if (hdfsdm_filter == &haudio_in_top_leftfilter)
   {
      DmaTopLeftRecHalfCplt = 1;
   }
   else if (hdfsdm_filter == &haudio_in_top_rightfilter)
   {
      DmaTopRightRecHalfCplt = 1;
   }
   //  else if(hdfsdm_filter == &haudio_in_buttom_leftfilter)
   //  {
   //    DmaButtomLeftRecHalfCplt = 1;
   //  }
   //  else
   //  {
   //    DmaButtomRightRecHalfCplt = 1;
   //  }
   if ((DmaTopLeftRecHalfCplt == 1) &&
       (DmaTopRightRecHalfCplt == 1))   // && (DmaButtomLeftRecHalfCplt == 1) && (DmaButtomRightRecHalfCplt == 1))
   {
      for (index = 0; index < ScratchSize / 2; index++)
      {
         hAudioIn.pRecBuf[AppBuffTrigger] = (uint16_t)(SaturaLH((pScratchBuff[0][index] >> 8), -32760, 32760));
         hAudioIn.pRecBuf[AppBuffTrigger + 1] = (uint16_t)(SaturaLH((pScratchBuff[1][index] >> 8), -32760, 32760));
         //      hAudioIn.pRecBuf[AppBuffTrigger + 2] = (uint16_t)(SaturaLH((pScratchBuff[2][index] >> 8), -32760,
         //      32760)); hAudioIn.pRecBuf[AppBuffTrigger + 3] = (uint16_t)(SaturaLH((pScratchBuff[3][index] >> 8),
         //      -32760, 32760));
         AppBuffTrigger += 2;   // 4
      }
      DmaTopLeftRecHalfCplt = 0;
      DmaTopRightRecHalfCplt = 0;
      DmaButtomLeftRecHalfCplt = 0;
      DmaButtomRightRecHalfCplt = 0;
   }

   /* Call Half Transfer Complete callback */
   if ((AppBuffTrigger == hAudioIn.RecSize / 2) && (AppBuffHalf == 0))
   {
      AppBuffHalf = 1;
      BSP_AUDIO_IN_HalfTransfer_CallBack();
   }
   /* Call Transfer Complete callback */
   if (AppBuffTrigger == hAudioIn.RecSize)
   {
      /* Reset Application Buffer Trigger */
      AppBuffTrigger = 0;
      AppBuffHalf = 0;
      /* Call the record update function to get the next buffer to fill and its size (size is ignored) */
      BSP_AUDIO_IN_TransferComplete_CallBack();
   }
}

/**
 * @brief  User callback when record buffer is filled.
 * @retval None
 */
__weak void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
   /* This function should be implemented by the user application.
      It is called into this driver when the current buffer is filled
      to prepare the next buffer pointer and its size. */
}

/**
 * @brief  Manages the DMA Half Transfer complete event.
 * @retval None
 */
__weak void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
   /* This function should be implemented by the user application.
      It is called into this driver when the current buffer is filled
      to prepare the next buffer pointer and its size. */
}

/**
 * @brief  Audio IN Error callback function.
 * @retval None
 */
__weak void BSP_AUDIO_IN_Error_Callback(void)
{
   /* This function is called when an Interrupt due to transfer error on or peripheral
      error occurs. */
}

/**
 * @brief  Initialize BSP_AUDIO_IN MSP.
 * @param  None
 * @retval None
 */
__weak void BSP_AUDIO_IN_MspInit(void)
{
   /* MSP channels initialization */
   DFSDMx_ChannelMspInit();
   /* MSP filters initialization */
   DFSDMx_FilterMspInit();
}

/**
 * @brief  DeInitialize BSP_AUDIO_IN MSP.
 * @param  None
 * @retval None
 */
__weak void BSP_AUDIO_IN_MspDeInit(void)
{
   /* MSP channels initialization */
   DFSDMx_ChannelMspDeInit();
   /* MSP filters initialization */
   DFSDMx_FilterMspDeInit();
}

/**
 * @brief  Clock Config.
 * @param  hdfsdm_filter: might be required to set audio peripheral predivider if any.
 * @param  AudioFreq: Audio frequency used to play the audio stream.
 * @note   This API is called by BSP_AUDIO_IN_Init()
 *         Being __weak it can be overwritten by the application
 * @retval None
 */
__weak void BSP_AUDIO_IN_ClockConfig(DFSDM_Filter_HandleTypeDef* hdfsdm_filter, uint32_t AudioFreq, void* Params)
{
   RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;

   HAL_RCCEx_GetPeriphCLKConfig(&rcc_ex_clk_init_struct);

   /* Set the PLL configuration according to the audio frequency */
   if ((AudioFreq == AUDIO_FREQUENCY_11K) || (AudioFreq == AUDIO_FREQUENCY_22K) || (AudioFreq == AUDIO_FREQUENCY_44K))
   {
      /* Configure PLLSAI prescalers */
      /* PLLI2S_VCO: VCO_429M
      SAI_CLK(first level) = PLLI2S_VCO/PLLI2SQ = 429/2 = 214.5 Mhz
      SAI_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ = 214.5/19 = 11.289 Mhz */
      rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
      rcc_ex_clk_init_struct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
      rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 429;
      rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 2;
      rcc_ex_clk_init_struct.PLLI2SDivQ = 19;

      HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
   }
   else /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_32K, AUDIO_FREQUENCY_48K, AUDIO_FREQUENCY_96K */
   {
      /* SAI clock config
      PLLI2S_VCO: VCO_344M
      SAI_CLK(first level) = PLLI2S_VCO/PLLI2SQ = 344/7 = 49.142 Mhz
      SAI_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ = 49.142/1 = 49.142 Mhz */
      rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
      rcc_ex_clk_init_struct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
      rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 344;
      rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 7;
      rcc_ex_clk_init_struct.PLLI2SDivQ = 1;
      HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
   }

   rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_DFSDM1_AUDIO;
   rcc_ex_clk_init_struct.Dfsdm1AudioClockSelection = RCC_DFSDM1AUDIOCLKSOURCE_SAI2;
   HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
}

/*******************************************************************************
                            Static Functions
*******************************************************************************/
/**
 * @brief  Initialize the Digital Filter for Sigma-Delta Modulators interface (DFSDM).
 * @param  AudioFreq: Audio frequency to be used to set correctly the DFSDM peripheral.
 * @note   Channel output Clock Divider and Filter Oversampling are calculated as follow:
 *         - Clock_Divider = CLK(input DFSDM)/CLK(micro) with
 *           1MHZ < CLK(micro) < 3.2MHZ (TYP 2.4MHZ for MP34DT01TR)
 *         - Oversampling = CLK(input DFSDM)/(Clock_Divider * AudioFreq)
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
static uint8_t DFSDMx_Init(uint32_t AudioFreq)
{
   /****************************************************************************/
   /********************** Channels configuration  *****************************/
   /****************************************************************************/
   /* CHANNEL 1 configuration */
   __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&haudio_in_top_leftchannel);
   haudio_in_top_leftchannel.Instance = DFSDM1_Channel1;
   haudio_in_top_leftchannel.Init.OutputClock.Activation = ENABLE;
   haudio_in_top_leftchannel.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
   /* Set the DFSDM clock OUT audio frequency configuration */
   haudio_in_top_leftchannel.Init.OutputClock.Divider = DFSDM_CLOCK_DIVIDER(AudioFreq);
   haudio_in_top_leftchannel.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
   haudio_in_top_leftchannel.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
   haudio_in_top_leftchannel.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
   /* Request to sample stable data for LEFT micro on Rising edge */
   haudio_in_top_leftchannel.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;
   haudio_in_top_leftchannel.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
   haudio_in_top_leftchannel.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
   haudio_in_top_leftchannel.Init.Awd.Oversampling = 10;
   haudio_in_top_leftchannel.Init.Offset = 0;
   haudio_in_top_leftchannel.Init.RightBitShift = DFSDM_RIGHT_BIT_SHIFT(AudioFreq);
   if (HAL_OK != HAL_DFSDM_ChannelInit(&haudio_in_top_leftchannel))
   {
      return AUDIO_ERROR;
   }

   /* CHANNEL 0 configuration */
   __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&haudio_in_top_rightchannel);
   haudio_in_top_rightchannel.Instance = DFSDM1_Channel0;
   haudio_in_top_rightchannel.Init.OutputClock.Activation = ENABLE;
   haudio_in_top_rightchannel.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
   /* Set the DFSDM clock OUT audio frequency configuration */
   haudio_in_top_rightchannel.Init.OutputClock.Divider = DFSDM_CLOCK_DIVIDER(AudioFreq);
   haudio_in_top_rightchannel.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
   haudio_in_top_rightchannel.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
   haudio_in_top_rightchannel.Init.Input.Pins = DFSDM_CHANNEL_FOLLOWING_CHANNEL_PINS;
   /* Request to sample stable data for RIGHT micro on Falling edge */
   haudio_in_top_rightchannel.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;
   haudio_in_top_rightchannel.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
   haudio_in_top_rightchannel.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
   haudio_in_top_rightchannel.Init.Awd.Oversampling = 10;
   haudio_in_top_rightchannel.Init.Offset = 0;
   haudio_in_top_rightchannel.Init.RightBitShift = DFSDM_RIGHT_BIT_SHIFT(AudioFreq);
   if (HAL_OK != HAL_DFSDM_ChannelInit(&haudio_in_top_rightchannel))
   {
      return AUDIO_ERROR;
   }

   /* CHANNEL 5 configuration */
   __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&haudio_in_buttom_leftchannel);
   haudio_in_buttom_leftchannel.Instance = DFSDM1_Channel5;
   haudio_in_buttom_leftchannel.Init.OutputClock.Activation = ENABLE;
   haudio_in_buttom_leftchannel.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
   /* Set the DFSDM clock OUT audio frequency configuration */
   haudio_in_buttom_leftchannel.Init.OutputClock.Divider = DFSDM_CLOCK_DIVIDER(AudioFreq);
   haudio_in_buttom_leftchannel.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
   haudio_in_buttom_leftchannel.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
   haudio_in_buttom_leftchannel.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
   /* Request to sample stable data for LEFT micro on Rising edge */
   haudio_in_buttom_leftchannel.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;
   haudio_in_buttom_leftchannel.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
   haudio_in_buttom_leftchannel.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
   haudio_in_buttom_leftchannel.Init.Awd.Oversampling = 10;
   haudio_in_buttom_leftchannel.Init.Offset = 0;
   haudio_in_buttom_leftchannel.Init.RightBitShift = DFSDM_RIGHT_BIT_SHIFT(AudioFreq);
   if (HAL_OK != HAL_DFSDM_ChannelInit(&haudio_in_buttom_leftchannel))
   {
      return AUDIO_ERROR;
   }

   /* CHANNEL 4 configuration */
   __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&haudio_in_buttom_rightchannel);
   haudio_in_buttom_rightchannel.Instance = DFSDM1_Channel4;
   haudio_in_buttom_rightchannel.Init.OutputClock.Activation = ENABLE;
   haudio_in_buttom_rightchannel.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
   /* Set the DFSDM clock OUT audio frequency configuration */
   haudio_in_buttom_rightchannel.Init.OutputClock.Divider = DFSDM_CLOCK_DIVIDER(AudioFreq);
   haudio_in_buttom_rightchannel.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
   haudio_in_buttom_rightchannel.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
   haudio_in_buttom_rightchannel.Init.Input.Pins = DFSDM_CHANNEL_FOLLOWING_CHANNEL_PINS;
   /* Request to sample stable data for RIGHT micro on Falling edge */
   haudio_in_buttom_rightchannel.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;
   haudio_in_buttom_rightchannel.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
   haudio_in_buttom_rightchannel.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
   haudio_in_buttom_rightchannel.Init.Awd.Oversampling = 10;
   haudio_in_buttom_rightchannel.Init.Offset = 0;
   haudio_in_buttom_rightchannel.Init.RightBitShift = DFSDM_RIGHT_BIT_SHIFT(AudioFreq);
   if (HAL_OK != HAL_DFSDM_ChannelInit(&haudio_in_buttom_rightchannel))
   {
      return AUDIO_ERROR;
   }

   /****************************************************************************/
   /********************** Filters configuration  ******************************/
   /****************************************************************************/

   /* FILTER 0 configuration */
   __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&haudio_in_top_leftfilter);
   haudio_in_top_leftfilter.Instance = AUDIO_DFSDMx_TOP_LEFT_FILTER;
   haudio_in_top_leftfilter.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
   haudio_in_top_leftfilter.Init.RegularParam.FastMode = ENABLE;
   haudio_in_top_leftfilter.Init.RegularParam.DmaMode = ENABLE;
   haudio_in_top_leftfilter.Init.InjectedParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
   haudio_in_top_leftfilter.Init.InjectedParam.ScanMode = ENABLE;
   haudio_in_top_leftfilter.Init.InjectedParam.DmaMode = DISABLE;
   haudio_in_top_leftfilter.Init.InjectedParam.ExtTrigger = DFSDM_FILTER_EXT_TRIG_TIM1_TRGO;
   haudio_in_top_leftfilter.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_RISING_EDGE;
   haudio_in_top_leftfilter.Init.FilterParam.SincOrder = DFSDM_FILTER_ORDER(AudioFreq);
   /* Set the DFSDM Filters Oversampling to have correct sample rate */
   haudio_in_top_leftfilter.Init.FilterParam.Oversampling = DFSDM_OVER_SAMPLING(AudioFreq);
   haudio_in_top_leftfilter.Init.FilterParam.IntOversampling = 1;
   if (HAL_OK != HAL_DFSDM_FilterInit(&haudio_in_top_leftfilter))
   {
      return AUDIO_ERROR;
   }

   /* Configure injected channel */
   if (HAL_OK != HAL_DFSDM_FilterConfigRegChannel(
                    &haudio_in_top_leftfilter, AUDIO_DFSDMx_TOP_LEFT_CHANNEL, DFSDM_CONTINUOUS_CONV_ON))
   {
      return AUDIO_ERROR;
   }

   /* FILTER 1 configuration */
   __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&haudio_in_top_rightfilter);
   haudio_in_top_rightfilter.Instance = AUDIO_DFSDMx_TOP_RIGHT_FILTER;
   haudio_in_top_rightfilter.Init.RegularParam.Trigger = DFSDM_FILTER_SYNC_TRIGGER;
   haudio_in_top_rightfilter.Init.RegularParam.FastMode = ENABLE;
   haudio_in_top_rightfilter.Init.RegularParam.DmaMode = ENABLE;
   haudio_in_top_rightfilter.Init.InjectedParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
   haudio_in_top_rightfilter.Init.InjectedParam.ScanMode = DISABLE;
   haudio_in_top_rightfilter.Init.InjectedParam.DmaMode = DISABLE;
   haudio_in_top_rightfilter.Init.InjectedParam.ExtTrigger = DFSDM_FILTER_EXT_TRIG_TIM1_TRGO;
   haudio_in_top_rightfilter.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_RISING_EDGE;
   haudio_in_top_rightfilter.Init.FilterParam.SincOrder = DFSDM_FILTER_ORDER(AudioFreq);
   /* Set the DFSDM Filters Oversampling to have correct sample rate */
   haudio_in_top_rightfilter.Init.FilterParam.Oversampling = DFSDM_OVER_SAMPLING(AudioFreq);
   haudio_in_top_rightfilter.Init.FilterParam.IntOversampling = 1;
   if (HAL_OK != HAL_DFSDM_FilterInit(&haudio_in_top_rightfilter))
   {
      return AUDIO_ERROR;
   }
   /* Configure injected channel */
   if (HAL_OK != HAL_DFSDM_FilterConfigRegChannel(
                    &haudio_in_top_rightfilter, AUDIO_DFSDMx_TOP_RIGHT_CHANNEL, DFSDM_CONTINUOUS_CONV_ON))
   {
      return AUDIO_ERROR;
   }

   /* FILTER 2 configuration */
   __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&haudio_in_buttom_leftfilter);
   haudio_in_buttom_leftfilter.Instance = AUDIO_DFSDMx_BUTTOM_LEFT_FILTER;
   haudio_in_buttom_leftfilter.Init.RegularParam.Trigger = DFSDM_FILTER_SYNC_TRIGGER;
   haudio_in_buttom_leftfilter.Init.RegularParam.FastMode = ENABLE;
   haudio_in_buttom_leftfilter.Init.RegularParam.DmaMode = ENABLE;
   haudio_in_buttom_leftfilter.Init.InjectedParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
   haudio_in_buttom_leftfilter.Init.InjectedParam.ScanMode = ENABLE;
   haudio_in_buttom_leftfilter.Init.InjectedParam.DmaMode = DISABLE;
   haudio_in_buttom_leftfilter.Init.InjectedParam.ExtTrigger = DFSDM_FILTER_EXT_TRIG_TIM1_TRGO;
   haudio_in_buttom_leftfilter.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_RISING_EDGE;
   haudio_in_buttom_leftfilter.Init.FilterParam.SincOrder = DFSDM_FILTER_ORDER(AudioFreq);
   /* Set the DFSDM Filters Oversampling to have correct sample rate */
   haudio_in_buttom_leftfilter.Init.FilterParam.Oversampling = DFSDM_OVER_SAMPLING(AudioFreq);
   haudio_in_buttom_leftfilter.Init.FilterParam.IntOversampling = 1;
   if (HAL_OK != HAL_DFSDM_FilterInit(&haudio_in_buttom_leftfilter))
   {
      return AUDIO_ERROR;
   }

   /* Configure injected channel */
   if (HAL_OK != HAL_DFSDM_FilterConfigRegChannel(
                    &haudio_in_buttom_leftfilter, AUDIO_DFSDMx_BUTTOM_LEFT_CHANNEL, DFSDM_CONTINUOUS_CONV_ON))
   {
      return AUDIO_ERROR;
   }

   /* FILTER 3 configuration */
   __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&haudio_in_buttom_rightfilter);
   haudio_in_buttom_rightfilter.Instance = AUDIO_DFSDMx_BUTTOM_RIGHT_FILTER;
   haudio_in_buttom_rightfilter.Init.RegularParam.Trigger = DFSDM_FILTER_SYNC_TRIGGER;
   haudio_in_buttom_rightfilter.Init.RegularParam.FastMode = ENABLE;
   haudio_in_buttom_rightfilter.Init.RegularParam.DmaMode = ENABLE;
   haudio_in_buttom_rightfilter.Init.InjectedParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
   haudio_in_buttom_rightfilter.Init.InjectedParam.ScanMode = DISABLE;
   haudio_in_buttom_rightfilter.Init.InjectedParam.DmaMode = DISABLE;
   haudio_in_buttom_rightfilter.Init.InjectedParam.ExtTrigger = DFSDM_FILTER_EXT_TRIG_TIM1_TRGO;
   haudio_in_buttom_rightfilter.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_RISING_EDGE;
   haudio_in_buttom_rightfilter.Init.FilterParam.SincOrder = DFSDM_FILTER_ORDER(AudioFreq);
   /* Set the DFSDM Filters Oversampling to have correct sample rate */
   haudio_in_buttom_rightfilter.Init.FilterParam.Oversampling = DFSDM_OVER_SAMPLING(AudioFreq);
   haudio_in_buttom_rightfilter.Init.FilterParam.IntOversampling = 1;
   if (HAL_OK != HAL_DFSDM_FilterInit(&haudio_in_buttom_rightfilter))
   {
      return AUDIO_ERROR;
   }
   /* Configure injected channel */
   if (HAL_OK != HAL_DFSDM_FilterConfigRegChannel(
                    &haudio_in_buttom_rightfilter, AUDIO_DFSDMx_BUTTOM_RIGHT_CHANNEL, DFSDM_CONTINUOUS_CONV_ON))
   {
      return AUDIO_ERROR;
   }

   return AUDIO_OK;
}

/**
 * @brief  De-initialize the Digital Filter for Sigma-Delta Modulators interface (DFSDM).
 * @retval AUDIO_OK if correct communication, else wrong communication
 */
static uint8_t DFSDMx_DeInit(void)
{
   /* De-initializes the DFSDM filters to allow access to DFSDM internal registers */
   if (HAL_OK != HAL_DFSDM_FilterDeInit(&haudio_in_top_leftfilter))
   {
      return AUDIO_ERROR;
   }

   if (HAL_OK != HAL_DFSDM_FilterDeInit(&haudio_in_top_rightfilter))
   {
      return AUDIO_ERROR;
   }

   /* De-initializes the DFSDM channels to allow access to DFSDM internal registers */
   if (HAL_OK != HAL_DFSDM_ChannelDeInit(&haudio_in_buttom_leftchannel))
   {
      return AUDIO_ERROR;
   }

   if (HAL_OK != HAL_DFSDM_ChannelDeInit(&haudio_in_buttom_rightchannel))
   {
      return AUDIO_ERROR;
   }

   return AUDIO_OK;
}

/**
 * @brief  Initialize the DFSDM channel MSP.
 * @param  hdfsdm_channel : DFSDM channel handle.
 * @retval None
 */
static void DFSDMx_ChannelMspInit(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   /* Enable DFSDM clock */
   AUDIO_DFSDMx_CLK_ENABLE();

   /* Enable GPIO clock */
   AUDIO_DFSDMx_DMIC_DATIN_GPIO_CLK_ENABLE();
   AUDIO_DFSDMx_CKOUT_DMIC_GPIO_CLK_ENABLE();

   /* DFSDM pins configuration: DFSDM_CKOUT, DMIC_DATIN pins ------------------*/
   GPIO_InitStruct.Pin = AUDIO_DFSDMx_CKOUT_PIN;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStruct.Alternate = AUDIO_DFSDMx_CKOUT_AF;
   HAL_GPIO_Init(AUDIO_DFSDMx_CKOUT_DMIC_GPIO_PORT, &GPIO_InitStruct);

   GPIO_InitStruct.Pin = AUDIO_DFSDMx_DMIC_DATIN1_PIN | AUDIO_DFSDMx_DMIC_DATIN5_PIN;
   GPIO_InitStruct.Alternate = AUDIO_DFSDMx_DMIC_DATIN_AF;
   HAL_GPIO_Init(AUDIO_DFSDMx_DMIC_DATIN_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief  DeInitialize the DFSDM channel MSP.
 * @param  hdfsdm_channel : DFSDM channel handle.
 * @retval None
 */
static void DFSDMx_ChannelMspDeInit(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   /* DFSDM pins configuration: DFSDM_CKOUT, DMIC_DATIN pins ------------------*/
   GPIO_InitStruct.Pin = AUDIO_DFSDMx_CKOUT_PIN;
   HAL_GPIO_DeInit(AUDIO_DFSDMx_CKOUT_DMIC_GPIO_PORT, GPIO_InitStruct.Pin);
   GPIO_InitStruct.Pin = AUDIO_DFSDMx_DMIC_DATIN1_PIN | AUDIO_DFSDMx_DMIC_DATIN5_PIN;
   HAL_GPIO_DeInit(AUDIO_DFSDMx_DMIC_DATIN_GPIO_PORT, GPIO_InitStruct.Pin);
}

/**
 * @brief  Initialize the DFSDM filter MSP.
 * @param  hdfsdm_filter : DFSDM filter handle.
 * @retval None
 */
static void DFSDMx_FilterMspInit(void)
{
   /* Enable DFSDM clock */
   AUDIO_DFSDMx_CLK_ENABLE();

   /* Enable the DMA clock */
   AUDIO_DFSDMx_DMAx_CLK_ENABLE();

   /*********** Configure DMA stream for TOP LEFT microphone *******************/
   hdma_top_left.Init.Direction = DMA_PERIPH_TO_MEMORY;
   hdma_top_left.Init.PeriphInc = DMA_PINC_DISABLE;
   hdma_top_left.Init.MemInc = DMA_MINC_ENABLE;
   hdma_top_left.Init.PeriphDataAlignment = AUDIO_DFSDMx_DMAx_PERIPH_DATA_SIZE;
   hdma_top_left.Init.MemDataAlignment = AUDIO_DFSDMx_DMAx_MEM_DATA_SIZE;
   hdma_top_left.Init.Mode = DMA_CIRCULAR;
   hdma_top_left.Init.Priority = DMA_PRIORITY_HIGH;
   hdma_top_left.Instance = AUDIO_DFSDMx_DMAx_TOP_LEFT_STREAM;
   hdma_top_left.Init.Channel = AUDIO_DFSDMx_DMAx_CHANNEL;

   /* Associate the DMA handle */
   __HAL_LINKDMA(&haudio_in_top_leftfilter, hdmaReg, hdma_top_left);

   /* Reset DMA handle state */
   __HAL_DMA_RESET_HANDLE_STATE(&hdma_top_left);

   /* Configure the DMA Channel */
   HAL_DMA_Init(&hdma_top_left);

   /* DMA IRQ Channel configuration */
   HAL_NVIC_SetPriority(AUDIO_DFSDMx_DMAx_TOP_LEFT_IRQ, AUDIO_OUT_IRQ_PREPRIO, 0);
   HAL_NVIC_EnableIRQ(AUDIO_DFSDMx_DMAx_TOP_LEFT_IRQ);

   /*********** Configure DMA stream for TOP RIGHT microphone ******************/
   hdma_top_right.Init.Direction = DMA_PERIPH_TO_MEMORY;
   hdma_top_right.Init.PeriphInc = DMA_PINC_DISABLE;
   hdma_top_right.Init.MemInc = DMA_MINC_ENABLE;
   hdma_top_right.Init.PeriphDataAlignment = AUDIO_DFSDMx_DMAx_PERIPH_DATA_SIZE;
   hdma_top_right.Init.MemDataAlignment = AUDIO_DFSDMx_DMAx_MEM_DATA_SIZE;
   hdma_top_right.Init.Mode = DMA_CIRCULAR;
   hdma_top_right.Init.Priority = DMA_PRIORITY_HIGH;
   hdma_top_right.Instance = AUDIO_DFSDMx_DMAx_TOP_RIGHT_STREAM;
   hdma_top_right.Init.Channel = AUDIO_DFSDMx_DMAx_CHANNEL;

   /* Associate the DMA handle */
   __HAL_LINKDMA(&haudio_in_top_rightfilter, hdmaReg, hdma_top_right);

   /* Reset DMA handle state */
   __HAL_DMA_RESET_HANDLE_STATE(&hdma_top_right);

   /* Configure the DMA Channel */
   HAL_DMA_Init(&hdma_top_right);

   /* DMA IRQ Channel configuration */
   HAL_NVIC_SetPriority(AUDIO_DFSDMx_DMAx_TOP_RIGHT_IRQ, AUDIO_OUT_IRQ_PREPRIO, 0);
   HAL_NVIC_EnableIRQ(AUDIO_DFSDMx_DMAx_TOP_RIGHT_IRQ);
}

/**
 * @brief  DeInitialize the DFSDM filter MSP.
 * @param  hdfsdm_filter : DFSDM filter handle.
 * @retval None
 */
static void DFSDMx_FilterMspDeInit(void)
{
   /* Configure the DMA Channel */
   HAL_DMA_DeInit(&hdma_top_left);
   HAL_DMA_DeInit(&hdma_top_right);
   HAL_DMA_DeInit(&hdma_buttom_left);
   HAL_DMA_DeInit(&hdma_buttom_right);
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
