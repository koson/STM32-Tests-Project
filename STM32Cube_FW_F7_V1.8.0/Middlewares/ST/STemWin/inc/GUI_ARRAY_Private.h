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
File        : GUI_ARRAY_Private.h
Purpose     : Private array handling routines, should be used only
              from within GUI_ARRAY... routines!
---------------------------END-OF-HEADER------------------------------
*/

/**
 ******************************************************************************
 * @attention
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

#ifndef GUI_ARRAY_PRIVATE_H
#   define GUI_ARRAY_PRIVATE_H

#   include "GUI_ARRAY.h"

#   if GUI_WINSUPPORT

/*********************************************************************
 *
 *       Private types
 *
 **********************************************************************
 */
typedef struct
{
   U16 NumItems;
   WM_HMEM haHandle; /* Handle to buffer holding handles */
} GUI_ARRAY_OBJ;

/*********************************************************************
 *
 *       Private functions
 *
 **********************************************************************
 */
WM_HMEM GUI_ARRAY__GethItem(const GUI_ARRAY_OBJ* pThis, unsigned int Index);
void* GUI_ARRAY__GetpItem(const GUI_ARRAY_OBJ* pThis, unsigned int Index);
void* GUI_ARRAY__GetpItemLocked(const GUI_ARRAY_OBJ* pThis, unsigned int Index);
int GUI_ARRAY__SethItem(GUI_ARRAY_OBJ* pThis, unsigned int Index, WM_HMEM hItem);

#   endif /* GUI_WINSUPPORT */

#endif /* GUI_ARRAY_PRIVATE_H */

/*************************** End of file ****************************/
