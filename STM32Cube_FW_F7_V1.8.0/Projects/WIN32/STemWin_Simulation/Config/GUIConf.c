/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.40 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              STMicroelectronics International NV, 39, Chemin du Champ-des Filles, 1228 Plan Les Ouates,
Geneva, SWITZERLAND Licensed SEGGER software: emWin License number:           GUI-00429 License model:            Buyout
SRC [Buyout Source Code License, signed November 29th 2012] Licensed product:         - Licensed platform:        STMs
ARM Cortex-M based 32 BIT CPUs Licensed number of seats: -
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2012-12-07 - 2017-12-31
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"

/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
//
// Define the available number of bytes available for the GUI
//
#define GUI_NUMBYTES 0x200000

/*********************************************************************
 *
 *       Public code
 *
 **********************************************************************
 */
/*********************************************************************
 *
 *       GUI_X_Config
 *
 * Purpose:
 *   Called during the initialization process in order to set up the
 *   available memory for the GUI.
 */
void GUI_X_Config(void)
{
   //
   // 32 bit aligned memory area
   //
   static U32 aMemory[GUI_NUMBYTES / 4];
   //
   // Assign memory to emWin
   //
   GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
   //
   // Set default font
   //
   GUI_SetDefaultFont(GUI_FONT_6X8);
}

/*************************** End of file ****************************/
