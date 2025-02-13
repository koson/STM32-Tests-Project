/**
 ******************************************************************************
 * @file    main.h
 * @author  MCD Application Team
 * @brief   main application header file.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
#ifndef __MAIN_H
#define __MAIN_H

#define USE_DHCP

#include "stm32f7xx_hal.h"
#ifdef USE_LCD
#   include "stm32f769i_discovery_lcd.h"
#   include "lcd_log.h"
#endif /* USE_LCD */
#include "stm32f769i_discovery.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rng.h"

#define HTTP_RESPONSE \
   "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" \
   "<h2>mbed TLS Test Server</h2>\r\n" \
   "<p>Successful connection using: %s</p>\r\n"

#ifdef USE_DHCP

#   define IP_ADDR0 0
#   define IP_ADDR1 0
#   define IP_ADDR2 0
#   define IP_ADDR3 0

#   define GW_ADDR0 0
#   define GW_ADDR1 0
#   define GW_ADDR2 0
#   define GW_ADDR3 0

#   define MASK_ADDR0 0
#   define MASK_ADDR1 0
#   define MASK_ADDR2 0
#   define MASK_ADDR3 0

#else

#   define IP_ADDR0 192
#   define IP_ADDR1 168
#   define IP_ADDR2 1
#   define IP_ADDR3 1

#   define GW_ADDR0 192
#   define GW_ADDR1 168
#   define GW_ADDR2 1
#   define GW_ADDR3 1

#   define MASK_ADDR0 255
#   define MASK_ADDR1 255
#   define MASK_ADDR2 254
#   define MASK_ADDR3 0

#endif /* USE_DHCP */
#ifdef MBEDTLS_MEMORY_BUFFER_ALLOC_C
#   define MAX_MEM_SIZE 80 * 1024
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#ifndef USE_LCD

#   define STLINK_USARTx USART1
#   define STLINK_USARTx_CLK_ENABLE() __USART1_CLK_ENABLE()
#   define STLINK_USARTx_RX_GPIO_CLK_ENABLE() __GPIOA_CLK_ENABLE()
#   define STLINK_USARTx_TX_GPIO_CLK_ENABLE() __GPIOA_CLK_ENABLE()

#   define STLINK_USARTx_FORCE_RESET() __USART1_FORCE_RESET()
#   define STLINK_USARTx_RELEASE_RESET() __USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#   define STLINK_USARTx_TX_PIN GPIO_PIN_9
#   define STLINK_USARTx_TX_GPIO_PORT GPIOA
#   define STLINK_USARTx_TX_AF GPIO_AF7_USART1
#   define STLINK_USARTx_RX_PIN GPIO_PIN_10
#   define STLINK_USARTx_RX_GPIO_PORT GPIOA
#   define STLINK_USARTx_RX_AF GPIO_AF7_USART1

#endif

void SSL_Server(void const* argument);
void LED_Thread(void const* argument);
void Error_Handler(void);

#endif /*__MAIN_H */
