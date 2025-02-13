/**
 ******************************************************************************
 * @file    usbd_template_core.h
 * @author  MCD Application Team
 * @brief   Header file for the usbd_template_core.c file.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_TEMPLATE_CORE_H
#   define __USB_TEMPLATE_CORE_H

#   ifdef __cplusplus
extern "C" {
#   endif

/* Includes ------------------------------------------------------------------*/
#   include "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
 * @{
 */

/** @defgroup USBD_TEMPLATE
 * @brief This file is the header file for usbd_template_core.c
 * @{
 */

/** @defgroup USBD_TEMPLATE_Exported_Defines
 * @{
 */
#   define TEMPLATE_EPIN_ADDR 0x81
#   define TEMPLATE_EPIN_SIZE 0x10

#   define USB_TEMPLATE_CONFIG_DESC_SIZ 64

/**
 * @}
 */

/** @defgroup USBD_CORE_Exported_TypesDefinitions
 * @{
 */

/**
 * @}
 */

/** @defgroup USBD_CORE_Exported_Macros
 * @{
 */

/**
 * @}
 */

/** @defgroup USBD_CORE_Exported_Variables
 * @{
 */

extern USBD_ClassTypeDef USBD_TEMPLATE_ClassDriver;
/**
 * @}
 */

/** @defgroup USB_CORE_Exported_Functions
 * @{
 */
/**
 * @}
 */

#   ifdef __cplusplus
}
#   endif

#endif /* __USB_TEMPLATE_CORE_H */
/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
