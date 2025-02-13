#include "stm32f10x.h"
#include "ili9320.h"
#include "ili9320_font.h"
#include "lcdconf.h"

extern void LCD_WR_REG(unsigned char index);
extern void LCD_WR_CMD(unsigned int index, unsigned int val);

extern void LCD_WR_Data(unsigned int val);
extern void LCD_WR_Data_8(unsigned char val);
//extern void LCD_test(void);
extern void LCD_clear(unsigned int p);

extern void lcd_wr_zf(unsigned int a, unsigned int b, unsigned int a1, unsigned int b1, unsigned int d, unsigned int e, unsigned char g, unsigned char* f);
extern void lcd_wr_pixel(unsigned int a, unsigned int b, unsigned int e);
extern unsigned char* num_pub(unsigned int a);
extern unsigned int LCD_RD_data(void);

/*
for stmfans-ev board
lcd:
PD0 -> nCS  PD1 -> RS  PD2 -> nWR  PD3 -> nRD  PD4 -> nRESET
touch:
PD5 -> TS_CLK  PD6 -> TS_nCS  PD7 -> TS_DIN  PD8 -> TS_BUSY  PD9 -> TS_nPENIRQ  PD10 -> TS_DOUT
*/
#define nCS GPIO_Pin_0
#define RS GPIO_Pin_1
#define nWR GPIO_Pin_2
#define nRD GPIO_Pin_3
#define nReset GPIO_Pin_4
//#define LCD_BK  GPIO_Pin_0

//GPIO_InitTypeDef GPIO_InitStructure;

u16 CheckController(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    u16 tmp = 0;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = nReset | nCS | nWR | RS | nRD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_PinLockConfig(GPIOD, nReset | nCS | nWR | RS | nRD);

    GPIO_SetBits(GPIOD, nReset | nCS | nWR | RS | nRD);

    ili9320_Reset();

    ili9320_WriteRegister(0x0000, 0x0001);
    ili9320_Delay(50000);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOD, nCS);

    GPIO_SetBits(GPIOD, RS);

    GPIO_ResetBits(GPIOD, nRD);
    tmp = GPIO_ReadInputData(GPIOE);

    GPIO_SetBits(GPIOD, nRD);

    GPIO_SetBits(GPIOD, nCS);

    return tmp;
}

/****************************************************************************

****************************************************************************/
void ili9320_SetCursor(u16 x, u16 y)
{
    //LCD_WR_CMD(0,0x02,x);
    //LCD_WR_CMD(1,0x03,y);

    LCD_WR_CMD(32, y);
    LCD_WR_CMD(33, 319 - x);
    //*(__IO uint16_t *) (Bank1_LCD_C)= 34;
    //LCD_WR_REG(34);
}

/****************************************************************************
SetWindows(0,0,100,100)��
****************************************************************************/
void ili9320_SetWindows(u16 StartX, u16 StartY, u16 EndX, u16 EndY)
{
    LCD_WR_CMD(0x0050, StartY);          // Horizontal GRAM Start Address
    LCD_WR_CMD(0x0051, EndX);            // Horizontal GRAM End Address
    LCD_WR_CMD(0x0052, 319 - StartX);    // Vertical GRAM Start Address
    LCD_WR_CMD(0x0053, EndY);            // Vertical GRAM Start Address

    //LCD_WR_REG(34);
}

/****************************************************************************
ili9320_Clear(0xffff);
****************************************************************************/
void ili9320_Clear(u16 dat)
{
    u32 i;
    LCD_WR_CMD(0x0050, 0);      // Horizontal GRAM Start Address
    LCD_WR_CMD(0x0051, 239);    // Horizontal GRAM End Address
    LCD_WR_CMD(0x0052, 0);      // Vertical GRAM Start Address
    LCD_WR_CMD(0x0053, 319);    // Vertical GRAM Start Address
    LCD_WR_CMD(32, 0);
    LCD_WR_CMD(33, 0);
    //*(__IO uint16_t *) (Bank1_LCD_C)= 34;
    LCD_WR_REG(34);
    for(i = 0; i < 76800; i++) LCD_WR_Data(dat);
}

/****************************************************************************
i=ili9320_GetPoint(10,10);
****************************************************************************/
u16 ili9320_GetPoint(u16 x, u16 y)
{
    ili9320_SetCursor(x, y);
    LCD_WR_REG(34);
    //temp = ili9320_ReadData(); //dummy
    //temp = ili9320_ReadData();

    return (ili9320_BGR2RGB(ili9320_ReadData()));
    //  return (ili9320_ReadData());
}

/****************************************************************************
ili9320_SetPoint(10,10,0x0fe0);
****************************************************************************/
void ili9320_SetPoint(u16 x, u16 y, u16 point)
{
    //if ( (x>240)||(y>320) ) return;
    //LCD_WR_CMD(0,0x02,x);
    //LCD_WR_CMD(1,0x03,y);

    LCD_WR_CMD(32, y);
    LCD_WR_CMD(33, 319 - x);
    //*(__IO uint16_t *) (Bank1_LCD_C)= 34;
    LCD_WR_REG(34);

    //LCD_WR_CMD(0,0x02,y);
    //LCD_WR_CMD(1,0x03,319-x);
    //LCD_WR_REG(0x0E);
    LCD_WR_Data(point);
}

/****************************************************************************
ili9320_DrawPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void ili9320_DrawPicture(u16 StartX, u16 StartY, u16 EndX, u16 EndY, u16* pic)
{
    u16 i;
    ili9320_SetWindows(StartX, StartY, EndX, EndY);
    ili9320_SetCursor(StartX, StartY);
    for(i = 0; i < (EndX * EndY); i++) LCD_WR_Data(*pic++);
}

/****************************************************************************
ili9320_PutChar(10,10,'a',0x0000,0xffff);
****************************************************************************/
void ili9320_PutChar(u16 x, u16 y, u8 c, u16 charColor, u16 bkColor)
{
    u16 i = 0;
    u16 j = 0;

    u8 tmp_char = 0;

    for(i = 0; i < 16; i++) {
        tmp_char = ascii_8x16[((c - 0x20) * 16) + i];
        for(j = 0; j < 8; j++) {
            if((tmp_char >> 7 - j) & 0x01 == 0x01) {
                ili9320_SetPoint(x + j, y + i, charColor);
            }
            else {
                ili9320_SetPoint(x + j, y + i, bkColor);
            }
        }
    }
}

/****************************************************************************
ili9320_Test();
****************************************************************************/
void ili9320_Test()
{
    u16 i, j;
    ili9320_SetCursor(0, 0);

    for(i = 0; i < 320; i++)
        for(j = 0; j < 240; j++) {
            if(i > 279)
                LCD_WR_Data(0x0000);
            else if(i > 239)
                LCD_WR_Data(0x001f);
            else if(i > 199)
                LCD_WR_Data(0x07e0);
            else if(i > 159)
                LCD_WR_Data(0x07ff);
            else if(i > 119)
                LCD_WR_Data(0xf800);
            else if(i > 79)
                LCD_WR_Data(0xf81f);
            else if(i > 39)
                LCD_WR_Data(0xffe0);
            else
                LCD_WR_Data(0xffff);
        }
}

/****************************************************************************

****************************************************************************/
u16 ili9320_BGR2RGB(u16 c)
{
    u16 r, g, b;

    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;

    return ((b << 11) + (g << 5) + (r << 0));
}

/****************************************************************************
ili9320_WriteIndex(0x0000);
****************************************************************************/
void ili9320_WriteIndex(u16 idx)
{
    LCD_WR_REG(idx);
}

/****************************************************************************
ili9320_WriteData(0x1030)
****************************************************************************/
void ili9320_WriteData(u16 dat)
{
    LCD_WR_Data(dat);
}

/****************************************************************************
i=ili9320_ReadData();
****************************************************************************/
u16 ili9320_ReadData(void)
{
    u16 val = 0;
    val = LCD_RD_data();
    return val;
}

/****************************************************************************
i=ili9320_ReadRegister(0x0022);
****************************************************************************/
u16 ili9320_ReadRegister(u16 index)
{
    u16 tmp;
    tmp = *(volatile unsigned int*)(0x60000000);

    return tmp;
}

/****************************************************************************
ili9320_WriteRegister(0x0000,0x0001);
****************************************************************************/
void ili9320_WriteRegister(u16 index, u16 dat)
{
    /************************************************************************
  **                                                                    **
  ** nCS       ----\__________________________________________/-------  **
  ** RS        ------\____________/-----------------------------------  **
  ** nRD       -------------------------------------------------------  **
  ** nWR       --------\_______/--------\_____/-----------------------  **
  ** DB[0:15]  ---------[index]----------[data]-----------------------  **
  **                                                                    **
  ************************************************************************/

    LCD_WR_CMD(index, dat);
}

/****************************************************************************
ili9320_Reset()
****************************************************************************/
void ili9320_Reset()
{
    /***************************************
   **                                   **
   **  -------\______________/-------   **
   **         | <---Tres---> |          **
   **                                   **
   **  Tres: Min.1ms                    **
   ***************************************/
}

/****************************************************************************
ili9320_BackLight(1);
****************************************************************************/
void ili9320_BackLight(u8 status)
{
    if(status >= 1) {
        //GPIO_SetBits(GPIOC,LCD_BK);
    }
    else {
        //GPIO_ResetBits(GPIOC,LCD_BK);
    }
}

/****************************************************************************
ili9320_Delay(10000);
****************************************************************************/
void ili9320_Delay(vu32 nCount)
{
    for(; nCount != 0; nCount--)
        ;
}