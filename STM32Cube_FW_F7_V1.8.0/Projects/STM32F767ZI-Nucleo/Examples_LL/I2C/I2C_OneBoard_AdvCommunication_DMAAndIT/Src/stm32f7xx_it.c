/**
 ******************************************************************************
 * @file    Examples_LL/I2C/I2C_OneBoard_AdvCommunication_DMAAndIT/Src/stm32f7xx_it.c
 * @author  MCD Application Team
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_it.h"

/** @addtogroup STM32F7xx_LL_Examples
 * @{
 */

/** @addtogroup I2C_OneBoard_AdvCommunication_DMAAndIT
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
   /* Go to infinite loop when Hard Fault exception occurs */
   while (1)
   {
   }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
   /* Go to infinite loop when Memory Manage exception occurs */
   while (1)
   {
   }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
   /* Go to infinite loop when Bus Fault exception occurs */
   while (1)
   {
   }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
   /* Go to infinite loop when Usage Fault exception occurs */
   while (1)
   {
   }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f7xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles external line 15_10 interrupt request.
 * @param  None
 * @retval None
 */
void USER_BUTTON_IRQHANDLER(void)
{
   /* Manage Flags */
   if (LL_EXTI_IsActiveFlag_0_31(USER_BUTTON_EXTI_LINE) != RESET)
   {
      LL_EXTI_ClearFlag_0_31(USER_BUTTON_EXTI_LINE);

      /* Manage code in main.c.*/
      UserButton_Callback();
   }
}

/**
 * Brief   This function handles I2C2 (Slave) event interrupt request.
 * Param   None
 * Retval  None
 */
void I2C2_EV_IRQHandler(void)
{
   /* Check ADDR flag value in ISR register */
   if (LL_I2C_IsActiveFlag_ADDR(I2C2))
   {
      /* Verify the Address Match with the OWN Slave address */
      if (LL_I2C_GetAddressMatchCode(I2C2) == SLAVE_OWN_ADDRESS)
      {
         /* Verify the transfer direction, a write direction, Slave enters receiver mode */
         if (LL_I2C_GetTransferDirection(I2C2) == LL_I2C_DIRECTION_WRITE)
         {
            /* Clear ADDR flag value in ISR register */
            LL_I2C_ClearFlag_ADDR(I2C2);

            /* Enable Receive Interrupt */
            LL_I2C_EnableIT_RX(I2C2);
         }
         else if (LL_I2C_GetTransferDirection(I2C2) == LL_I2C_DIRECTION_READ)
         {
            /* Clear ADDR flag value in ISR register */
            LL_I2C_ClearFlag_ADDR(I2C2);

            /* Enable Transmit Interrupt */
            LL_I2C_EnableIT_TX(I2C2);
         }
      }
      else
      {
         /* Clear ADDR flag value in ISR register */
         LL_I2C_ClearFlag_ADDR(I2C2);

         /* Call Error function */
         Error_Callback();
      }
   }
   /* Check NACK flag value in ISR register */
   else if (LL_I2C_IsActiveFlag_NACK(I2C2))
   {
      /* End of Transfer */
      LL_I2C_ClearFlag_NACK(I2C2);
   }
   /* Check RXNE flag value in ISR register */
   else if (LL_I2C_IsActiveFlag_RXNE(I2C2))
   {
      /* Call function Slave Reception Callback */
      Slave_Reception_Callback();
   }
   /* Check TXIS flag value in ISR register */
   else if (LL_I2C_IsActiveFlag_TXIS(I2C2))
   {
      /* Call function Slave Ready to Transmit Callback */
      Slave_Ready_To_Transmit_Callback();
   }
   /* Check STOP flag value in ISR register */
   else if (LL_I2C_IsActiveFlag_STOP(I2C2))
   {
      /* End of Transfer */
      LL_I2C_ClearFlag_STOP(I2C2);

      /* Check TXE flag value in ISR register */
      if (!LL_I2C_IsActiveFlag_TXE(I2C2))
      {
         /* Flush TX buffer */
         LL_I2C_ClearFlag_TXE(I2C2);
      }

      /* Call function Slave Complete Callback */
      Slave_Complete_Callback();
   }
   /* Check TXE flag value in ISR register */
   else if (!LL_I2C_IsActiveFlag_TXE(I2C2))
   {
      /* Do nothing */
      /* This Flag will be set by hardware when the TXDR register is empty */
      /* If needed, use LL_I2C_ClearFlag_TXE() interface to flush the TXDR register  */
   }
   else
   {
      /* Call Error function */
      Error_Callback();
   }
}

/**
 * Brief   This function handles I2C2 (Slave) error interrupt request.
 * Param   None
 * Retval  None
 */
void I2C2_ER_IRQHandler(void)
{
   /* Call Error function */
   Error_Callback();
}

/**
 * @brief  This function handles DMA1_Stream6 interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Stream6_IRQHandler(void)
{
   if (LL_DMA_IsActiveFlag_TC6(DMA1))
   {
      LL_DMA_ClearFlag_TC6(DMA1);

      Transfer_Complete_Callback();
   }
   else if (LL_DMA_IsActiveFlag_TE6(DMA1))
   {
      Transfer_Error_Callback();
   }
}

/**
 * @brief  This function handles DMA1_Stream0 interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Stream0_IRQHandler(void)
{
   if (LL_DMA_IsActiveFlag_TC0(DMA1))
   {
      LL_DMA_ClearFlag_TC0(DMA1);
      Transfer_Complete_Callback();
   }
   else if (LL_DMA_IsActiveFlag_TE0(DMA1))
   {
      Transfer_Error_Callback();
   }
}
/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
