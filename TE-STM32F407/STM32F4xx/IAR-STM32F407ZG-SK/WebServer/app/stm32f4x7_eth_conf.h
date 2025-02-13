/**
 ******************************************************************************
 * @file    stm32f4x7_eth_conf.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    14-October-2011
 * @brief   Configuration file for the STM32F4x7 Ethernet driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4x7_ETH_CONF_H
#define __STM32F4x7_ETH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Uncomment the line below when using time stamping and/or IPv4 checksum offload */
#define USE_ENHANCED_DMA_DESCRIPTORS

/* Uncomment the line below if you want to use user defined Delay function
   (for precise timing), otherwise default _eth_delay_ function defined within
   the Ethernet driver is used (less precise timing) */
//#define USE_Delay

#ifdef USE_Delay
#include "main.h"         /* Header file where the Delay function prototype is exported */
#define _eth_delay_ Delay /* User can provide more timing precise _eth_delay_ function */
#else
#define _eth_delay_ ETH_Delay /* Default _eth_delay_ function with less precise timing */
#endif

/* Uncomment the line below to allow custom configuration of the Ethernet driver buffers */
#define CUSTOM_DRIVER_BUFFERS_CONFIG

#ifdef CUSTOM_DRIVER_BUFFERS_CONFIG
/* Redefinition of the Ethernet driver buffers size and count */
#define ETH_RX_BUF_SIZE ETH_MAX_PACKET_SIZE /* buffer size for receive */
#define ETH_TX_BUF_SIZE ETH_MAX_PACKET_SIZE /* buffer size for transmit */
#define ETH_RXBUFNB 8                       /* 4 Rx buffers of size ETH_RX_BUF_SIZE */
#define ETH_TXBUFNB 2                       /* 2 Tx buffers of size ETH_TX_BUF_SIZE */
#endif

/* PHY configuration section **************************************************/
/* PHY Reset delay */
#define PHY_RESET_DELAY ((uint32_t)0x000FFFFF)
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY ((uint32_t)0x00FFFFFF)

/* The PHY status register value change from a PHY to another, so the user have
   to update this value depending on the used external PHY */
#define PHY_SR ((uint16_t)17) /* Value for ST802RT/STE101P PHY */

/* The Speed and Duplex mask values change from a PHY to another, so the user
   have to update this value depending on the used external PHY */
#define PHY_SPEED_STATUS_POLARITY 0 /* 0 - normal   (10Mbps coded as 0, 100Mbps coded as 1) */
/* 1 - inversed (10Mbps coded as 1, 100Mbps coded as 0) */
#define PHY_SPEED_STATUS ((uint16_t)0x0200)  /* Value for ST802RT/STE101P PHY */
#define PHY_DUPLEX_STATUS ((uint16_t)0x0100) /* Value for ST802RT/STE101P PHY */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4x7_ETH_CONF_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
