/*********************************************************************
*          Portions COPYRIGHT 2013 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
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
File        : ICONVIEW_Private.h
Purpose     : ICONVIEW private header file
--------------------END-OF-HEADER-------------------------------------
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

#ifndef ICONVIEW_PRIVATE_H
#define ICONVIEW_PRIVATE_H

#include "WM.h"
#include "GUI_ARRAY.h"
#include "ICONVIEW.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
    const GUI_FONT GUI_UNI_PTR* pFont;
    GUI_COLOR aBkColor[3];
    GUI_COLOR aTextColor[3];
    int FrameX, FrameY;
    int SpaceX, SpaceY;
    int TextAlign;
    int IconAlign;
    GUI_WRAPMODE WrapMode;
} ICONVIEW_PROPS;

typedef struct {
    WIDGET Widget;
    WM_SCROLL_STATE ScrollStateV;
    WM_SCROLL_STATE ScrollStateH;
    ICONVIEW_PROPS Props;
    GUI_ARRAY ItemArray;
    int xSizeItems;
    int ySizeItems;
    int Sel;
    U16 Flags;
/* Type check in debug version */
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
    U32 DebugId;
#endif
} ICONVIEW_OBJ;

typedef void tDrawImage(const void* pData, int xPos, int yPos);
typedef void tDrawText(ICONVIEW_OBJ* pObj, GUI_RECT* pRect, const char* s);
typedef void tGetImageSizes(const void* pData, int* xSize, int* ySize);

typedef struct {
    tDrawImage* pfDrawImage;
    tDrawText* pfDrawText;
    tGetImageSizes* pfGetImageSizes;
    const void* pData;
    U32 UserData;
    int SizeofData;
    char acText[1];
} ICONVIEW_ITEM;

/*********************************************************************
*
*       Function pointer(s)
*
**********************************************************************
*/
extern void (*ICONVIEW__pfDrawStreamedBitmap)(const void* p, int x, int y);

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
#define ICONVIEW_INIT_ID(p) (p->DebugId = ICONVIEW_ID)
#else
#define ICONVIEW_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
ICONVIEW_OBJ* ICONVIEW_LockH(ICONVIEW_Handle h);
#define ICONVIEW_LOCK_H(h) ICONVIEW_LockH(h)
#else
#define ICONVIEW_LOCK_H(h) (ICONVIEW_OBJ*)GUI_LOCK_H(h)
#endif

#endif /* GUI_WINSUPPORT */
#endif /* ICONVIEW_H */

/*************************** End of file ****************************/
