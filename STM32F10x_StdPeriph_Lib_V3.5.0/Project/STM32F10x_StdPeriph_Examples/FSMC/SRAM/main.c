/**
  ******************************************************************************
  * @file    FSMC/SRAM/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#ifdef STM32F10X_HD_VL /* High-density Value line devices */
#include "stm32100e_eval_fsmc_sram.h"
#else /* High- and XL-denisty devices */
#include "stm3210e_eval_fsmc_sram.h"
#endif

#include "stm32_eval.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup FSMC_SRAM
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE 0x400
#define WRITE_READ_ADDR 0x8000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t TxBuffer[BUFFER_SIZE];
uint16_t RxBuffer[BUFFER_SIZE];
uint32_t WriteReadStatus = 0, Index = 0;

/* Private function prototypes -----------------------------------------------*/
void Fill_Buffer(uint16_t* pBuffer, uint16_t BufferLenght, uint32_t Offset);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */

    /* Initialize Leds mounted on STM3210X-EVAL board */
    STM_EVAL_LEDInit(LED1);
    STM_EVAL_LEDInit(LED2);

    /* Write/read to/from FSMC SRAM memory  *************************************/
    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    /* Configure FSMC Bank1 NOR/SRAM3 */
    SRAM_Init();

    /* Write data to FSMC SRAM memory */
    /* Fill the buffer to send */
    Fill_Buffer(TxBuffer, BUFFER_SIZE, 0x3212);
    SRAM_WriteBuffer(TxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    /* Read data from FSMC SRAM memory */
    SRAM_ReadBuffer(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    /* Read back SRAM memory and check content correctness */
    for(Index = 0x00; (Index < BUFFER_SIZE) && (WriteReadStatus == 0); Index++) {
        if(RxBuffer[Index] != TxBuffer[Index]) {
            WriteReadStatus = Index + 1;
        }
    }

    if(WriteReadStatus == 0) {
        /* OK */
        /* Turn on LED1 */
        STM_EVAL_LEDOn(LED1);
    }
    else {
        /* KO */
        /* Turn on LED2 */
        STM_EVAL_LEDOn(LED2);
    }

    while(1) {
    }
}

/**
  * @brief  Fill the global buffer
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferSize: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  */
void Fill_Buffer(uint16_t* pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
    uint16_t IndexTmp = 0;

    /* Put in global buffer same values */
    for(IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++) {
        pBuffer[IndexTmp] = IndexTmp + Offset;
    }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while(1) {
    }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
