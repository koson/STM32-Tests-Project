/**
 ******************************************************************************
 * @file    boadr.hpp
 * @author  Ivan Orfanidi
 * @version V1.0.0
 * @date    28-March-2018
 * @brief   This file contains all the methods prototypes for the RTC
 *          firmware library.
 ******************************************************************************
 * @attention
 *
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOARD_HPP
#define __BOARD_HPP

#include <stdint.h>
#include "stm32f10x_gpio.h"

#define GpioHigh(a, b) a->BSRR = b
#define GpioLow(a, b) a->BRR = b
#define GpioToggle(a, b) a->ODR ^= b

/// LIGHT LED
#define LED_PIN GPIO_Pin_1
#define PORT_LED GPIOA
#define PORT_LED_CLK RCC_APB2Periph_GPIOA

#ifdef __cplusplus

/*
 * @brief Class Board
 */
class Board {
  public:
    static void SetNvicPriorityGroup(uint32_t);    ///< Set NVIC Priority Group

    static void InitSysTick(uint32_t ticks_us = 1000);    ///< Init System Tick

    static uint32_t ClockUpdate();    ///< Update and geting System Clock Core

    static void InitBKP();    ///< Initialisation Backup

    static void InitIWDG();    ///< Initialisation Watchdog timer

    static void SleepDevice();    ///< Sleep Device

    static void WakeUpPinEnable();    ///< Enable WKUP pin

    static void DelayMS(uint32_t);    ///< Delay msec

    static uint32_t GetSysCount();    ///< Get System Counter

    static void GpioClock(const GPIO_TypeDef*, FunctionalState);    ///< Enable/Disable Clock Port

    static uint32_t SysCount;

  protected:
    Board();    ///< Constructor

  private:
    virtual ~Board();
};

/*
 * @brief Extern interrupt
 */
extern "C" {
void SysTick_Handler(void);
}
#endif

#endif