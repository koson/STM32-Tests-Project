/**
 ******************************************************************************
 * @file    k_menu.c
 * @author  MCD Application Team
 * @brief   This file provides the kernel menu functions
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "up.c"
#include "cancel.c"
#include "ok.c"

/** @addtogroup CORE
 * @{
 */

/** @defgroup KERNEL_MENU
 * @brief Kernel menu routines
 * @{
 */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define ID_ICONVIEW_MENU (GUI_ID_USER + 0x00)
#define ID_FEATURES_FPU (GUI_ID_USER + 0x44)
#define ID_FEATURES_IC (GUI_ID_USER + 0x45)
#define ID_FEATURES_DC (GUI_ID_USER + 0x46)
#define ID_FEATURES_CPU (GUI_ID_USER + 0x47)
#define ID_FEATURES_FREQ (GUI_ID_USER + 0x48)
#define ID_STM32_F7 (GUI_ID_USER + 0x49)
#define ID_TARA_NAME (GUI_ID_USER + 0x50)

ICONVIEW_Handle hIcon;
WM_HWIN hCPULoad;
extern GUI_CONST_STORAGE GUI_BITMAP bmF7Logo;
static void CleanDisplay(void);
/**
 * @brief  Callback routine of desktop window.
 * @param  pMsg: pointer to data structure of type WM_MESSAGE
 * @retval None
 */
static void _cbBk(WM_MESSAGE* pMsg)
{
   uint32_t NCode, Id;
   static uint8_t sel;

   switch (pMsg->MsgId)
   {
   case WM_PAINT:
      GUI_SetBkColor(GUI_WHITE);
      GUI_Clear();

      /*H/W Information */
      GUI_SetColor(0x007C18CC);
      GUI_AA_FillRoundedRect(650, 60, 840, 420, 20);

      GUI_DrawBitmap(&bmF7Logo, 690, 80);
      break;

   case WM_NOTIFY_PARENT:
      Id = WM_GetId(pMsg->hWinSrc);
      NCode = pMsg->Data.v;

      switch (NCode)
      {
      case WM_NOTIFICATION_RELEASED:
         if (Id == ID_ICONVIEW_MENU)
         {
            sel = ST_AnimatedIconView_GetSel(pMsg->hWinSrc);

            if (sel < k_ModuleGetNumber())
            {
               ST_AnimatedIconView_SetSel(pMsg->hWinSrc, -1);
               if (module_prop[sel].in_use == 0)
               {
                  module_prop[sel].in_use = 1;
                  module_prop[sel].module->startup(pMsg->hWin, 0, 0);
               }
               else if (module_prop[sel].win_state == 1)
               {
                  module_prop[sel].module->startup(pMsg->hWin, 0, 0);
               }
            }
            else
            {
               WM_InvalidateWindow(pMsg->hWinSrc);
            }
         }
         break;

      case WM_NOTIFICATION_CHILD_DELETED:
         module_prop[sel].in_use = 0;
         module_prop[sel].win_state = 0;
         break;

      default:
         break;
      }
      break;
   default:
      WM_DefaultProc(pMsg);
   }
}

/**
 * @brief  set the GUI profile
 * @param  None.
 * @retval None.
 */
void k_PeriodicProcesses(void)
{
   char tmp[16];
   WM_HWIN hItem;
   static uint32_t InitTick = 0;

   hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_CPU);

   if (hItem)
   {
      if ((WM_IsVisible(hItem)) && ((osKernelSysTick() - InitTick) > 500))
      {
         InitTick = osKernelSysTick();

         sprintf((char*)tmp, "MCU Load : %d%%", osGetCPUUsage());
         TEXT_SetText(hItem, tmp);

         WM_InvalidateWindow(hItem);
         WM_Update(hItem);
      }
   }
}

/**
 * @brief  set the GUI profile
 * @param  None.
 * @retval None.
 */
void k_SetGuiProfile(void)
{
   BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
   DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);

   FRAMEWIN_SetDefaultTextColor(0, GUI_WHITE);
   FRAMEWIN_SetDefaultTextColor(1, GUI_WHITE);

   FRAMEWIN_SetDefaultBarColor(0, GUI_STCOLOR_LIGHTBLUE);
   FRAMEWIN_SetDefaultBarColor(1, GUI_STCOLOR_LIGHTBLUE);

   FRAMEWIN_SetDefaultFont(&GUI_FontAvantGarde32);

   FRAMEWIN_SetDefaultClientColor(GUI_WHITE);
   FRAMEWIN_SetDefaultTitleHeight(35);

   WINDOW_SetDefaultBkColor(GUI_WHITE);

   LISTVIEW_SetDefaultGridColor(GUI_WHITE);
   LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SEL, GUI_STCOLOR_LIGHTBLUE);
   DROPDOWN_SetDefaultColor(DROPDOWN_CI_SEL, GUI_STCOLOR_LIGHTBLUE);
   LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SELFOCUS, GUI_STCOLOR_DARKBLUE);
   LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_UNSEL, GUI_STCOLOR_LIGHTBLUE);
   DROPDOWN_SetDefaultColor(DROPDOWN_CI_SELFOCUS, GUI_STCOLOR_LIGHTBLUE);
   SCROLLBAR_SetDefaultWidth(24);
   SCROLLBAR_SetDefaultSTSkin();

   HEADER_SetDefaultBkColor(GUI_STCOLOR_LIGHTBLUE);
   HEADER_SetDefaultTextColor(GUI_WHITE);
   HEADER_SetDefaultFont(&GUI_FontAvantGarde24);
   DROPDOWN_SetDefaultFont(&GUI_FontAvantGarde24);
   LISTVIEW_SetDefaultFont(&GUI_FontAvantGarde20);
   HEADER_SetDefaultSTSkin();
   SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_THUMB);
   SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_SHAFT);
   SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_ARROW);
   ST_CHOOSEFILE_SetDelim('/');
   ST_CHOOSEFILE_SetButtonSize(60, 40);

   ST_CHOOSEFILE_SetButtonBitmaps(2, &bmup, NULL);

   ST_CHOOSEFILE_SetButtonBitmaps(1, &bmok, &bmOK_disabled);

   ST_CHOOSEFILE_SetButtonBitmaps(0, &bm_fccancel, NULL);

   GUI_SetDefaultFont(&GUI_FontLubalGraph20);

   TEXT_SetDefaultTextColor(GUI_STCOLOR_LIGHTBLUE);
   TEXT_SetDefaultFont(&GUI_FontLubalGraph20);
}

/**
 * @brief  Initializes the menu.
 * @param  None.
 * @retval None.
 */
void k_InitMenu(void)
{
   uint8_t i = 0;
   WM_HWIN hItem;

   WM_SetCallback(WM_GetDesktopWindowEx(0), _cbBk);

   hIcon = ST_AnimatedIconView_CreateEx(0,
                                        0,
                                        LCD_GetXSize() - 155,
                                        LCD_GetYSize() - 50,
                                        WM_GetDesktopWindowEx(0),
                                        WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_LATE_CLIP,
                                        0,
                                        ID_ICONVIEW_MENU,
                                        150,
                                        180,
                                        20,
                                        5);

   ST_AnimatedIconView_SetDualFont(hIcon, &GUI_FontAvantGarde24, &GUI_FontAvantGarde24);

   ST_AnimatedIconView_SetSpace(hIcon, GUI_COORD_Y, 5);
   ST_AnimatedIconView_SetSpace(hIcon, GUI_COORD_X, 5);
   ST_AnimatedIconView_SetFrame(hIcon, GUI_COORD_Y, 30);
   ST_AnimatedIconView_SetFrame(hIcon, GUI_COORD_X, 10);
   ST_AnimatedIconView_SetTextColor(hIcon, ICONVIEW_CI_UNSEL, GUI_STCOLOR_LIGHTBLUE);
   ST_AnimatedIconView_SetBkColor(hIcon, ICONVIEW_CI_UNSEL, GUI_WHITE);
   ST_AnimatedIconView_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_WHITE);
   ST_AnimatedIconView_SetSel(hIcon, -1);
   ST_AnimatedIconView_SetDualTextColor(hIcon, ICONVIEW_CI_SEL, GUI_STCOLOR_LIGHTBLUE, GUI_STCOLOR_DARKBLUE);

   for (i = 0; i < k_ModuleGetNumber(); i++)
   {
      ST_AnimatedIconView_AddIcon(hIcon,
                                  module_prop[i].module->open_icon,
                                  module_prop[i].module->close_icon,
                                  (char*)module_prop[i].module->name);
   }

   hItem = TEXT_CreateEx(360, 395, 100, 30, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_TARA_NAME, "Wizard");
   TEXT_SetFont(hItem, &GUI_FontAvantGarde24);
   TEXT_SetTextColor(hItem, GUI_STCOLOR_DARKBLUE);

   hItem =
      TEXT_CreateEx(665, 165, 250, 30, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_STM32_F7, "STM32 F7");
   TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
   TEXT_SetTextColor(hItem, GUI_WHITE);

   hItem =
      TEXT_CreateEx(660, 240, 250, 30, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_FPU, "FPU : ON");
   TEXT_SetFont(hItem, GUI_FONT_20_1);
   TEXT_SetTextColor(hItem, GUI_WHITE);

   hItem = TEXT_CreateEx(
      660, 270, 250, 30, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_IC, "I-Cache : ON");
   TEXT_SetFont(hItem, GUI_FONT_20_1);
   TEXT_SetTextColor(hItem, GUI_WHITE);

   hItem = TEXT_CreateEx(
      660, 300, 250, 30, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_DC, "D-Cache : ON");
   TEXT_SetFont(hItem, GUI_FONT_20_1);
   TEXT_SetTextColor(hItem, GUI_WHITE);

   hItem = TEXT_CreateEx(
      660, 330, 250, 30, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_CPU, "MCU Load < 1%");
   TEXT_SetFont(hItem, GUI_FONT_20_1);
   TEXT_SetTextColor(hItem, GUI_WHITE);

   hItem = TEXT_CreateEx(
      660, 360, 250, 30, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_FREQ, "Clk : 200 MHz");
   TEXT_SetFont(hItem, GUI_FONT_20_1);
   TEXT_SetTextColor(hItem, GUI_WHITE);

   WM_Exec();
   CleanDisplay();
}

/**
 * @brief  Clean Display
 * @retval None.
 */
static void CleanDisplay(void)
{
   /* Set up mode */
   DMA2D->CR = 0x00030000UL | (1 << 9);
   DMA2D->OCOLR = 0x00;

   /* Set up pointers */
   DMA2D->OMAR = (U32)(0xC0000000);

   /* Set up offsets */
   DMA2D->OOR = 0;

   /* Set up pixel format */
   DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB888;

   /*  Set up size */
   DMA2D->NLR = (U32)(800 << 16) | (U16)480;

   DMA2D->CR |= DMA2D_CR_START;

   /* Wait until transfer is done */
   while (DMA2D->CR & DMA2D_CR_START)
   {
   }
}
/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
