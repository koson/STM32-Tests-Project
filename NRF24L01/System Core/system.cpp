/**
 ******************************************************************************
 * @file    system.cpp
 * @author  Ivan Orfanidi
 * @version V1.0.0
 * @date    04-2019
 * @brief   This file system
 ******************************************************************************
 * @attention
 *
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "system.hpp"

time_t System::SysCount = 0;
FunctionalState System::SysTickState = DISABLE;

/*
 * @brief  Set NVIC Priority Group.
 * @retval None.
 */
void System::SetNvicPriorityGroup(uint32_t priority_group)
{
    /* clang-format off */
    /* The table below gives the allowed values of the pre-emption priority and subpriority according
     to the Priority Grouping configuration performed by NVIC_PriorityGroupConfig function
      ============================================================================================================================
        NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  | Description
      ============================================================================================================================
       NVIC_PriorityGroup_0  |                0                  |            0-15             |   0 bits for pre-emption priority
                             |                                   |                             |   4 bits for subpriority
      ----------------------------------------------------------------------------------------------------------------------------
       NVIC_PriorityGroup_1  |                0-1                |            0-7              |   1 bits for pre-emption priority
                             |                                   |                             |   3 bits for subpriority
      ----------------------------------------------------------------------------------------------------------------------------    
       NVIC_PriorityGroup_2  |                0-3                |            0-3              |   2 bits for pre-emption priority
                             |                                   |                             |   2 bits for subpriority
      ----------------------------------------------------------------------------------------------------------------------------    
       NVIC_PriorityGroup_3  |                0-7                |            0-1              |   3 bits for pre-emption priority
                             |                                   |                             |   1 bits for subpriority
      ----------------------------------------------------------------------------------------------------------------------------    
       NVIC_PriorityGroup_4  |                0-15               |            0                |   4 bits for pre-emption priority
                             |                                   |                             |   0 bits for subpriority                       
      ============================================================================================================================
    */
    /* clang-format on */
    NVIC_PriorityGroupConfig(priority_group);
}

/**
 * @brief  Update and geting System Clock Core.
 * @retval System Clock Core.
 */
time_t System::ClockUpdate()
{
    SystemCoreClockUpdate();
    return SystemCoreClock;
}

/**
 * @brief  This method configures System Tick
 * @param [in]  ticks  Number of ticks between two interrupts.
 *                      / 1000     |   1ms  /
 *                      / 100000   |   10us /
 *                      / 1000000  |   1us  /
 * @retval None.
 */
void System::InitSysTick(time_t ticks_us)
{
    while(SysTick_Config(SystemCoreClock / ticks_us)) {
        // Wait running System Tick
    }
    SysTickDisableIRQ();
    __enable_irq();
}

/**
 * @brief  Set SysTick in mode Clock
 * @param [in] state - ENABLE or DISABLE
 */
void System::SysTickClock(FunctionalState state)
{
    if(ENABLE == state) {
        SysTickEnableIRQ();
        __enable_irq();
    }
    else {
        SysTickDisableIRQ();
    }
}

/**
 * @brief  Get conunter clock.
 * @retval counter.
 */
time_t System::GetSysCount()
{
    return SysCount;
}

/**
 * @brief  Delay in MS
 * @retval None.
 */
void System::DelayMS(time_t delay)
{
    bool last = false;
    if(DISABLE == SysTickState) {
        SysTickEnableIRQ();
        last = true;
    }

    delay += SysCount;
    while(delay >= SysCount) {
    }

    if(last) {
        SysTickDisableIRQ();
    }
}

/**
 * @brief  Sleep Device.
 * @retval None
 */
void System::SleepDevice()
{
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
    PWR_EnterSTANDBYMode();    //  -_-zZ
}

/**
 * @brief Enable WKUP pin.
 * @retval None
 */
void System::WakeUpPinEnable()
{
    /* Enable WKUP pin */
    PWR_WakeUpPinCmd(ENABLE);
}

/*
 *       INTERRUPTS
 */
void SysTick_Handler(void)
{
    System::SysCount++;
}