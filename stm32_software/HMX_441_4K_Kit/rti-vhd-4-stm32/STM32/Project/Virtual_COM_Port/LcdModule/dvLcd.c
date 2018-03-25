
#include "si_datatypes.h"
#include "dvLcd.h"
#include "si_logger.h"
#include "util.h"

//static const char *const g_pcHex        = "0123456789abcdef";
//static const char *const g_pcHexCapital = "0123456789ABCDEF";



//595作为1602的8位数据口
static void LCD_Send_595(unsigned char data)
{
    unsigned char i = 0;

    LCDHC595_DATA_LOW;                              //clean all port
    LCDSHCP_LOW;
    LCDSTCP_LOW;

    for (i = 0; i < 8; i++)
    {
        if (data & 0x01)
        {LCDHC595_DATA_HIGH;}                  //1;
        else
        {LCDHC595_DATA_LOW;}                   //0;


        LCDSHCP_HIGH;
        LCDSHCP_LOW;
        data >>= 1;            //send sequence: MSB bit to LSB bit
    }
    LCDSTCP_HIGH;
    LCDHC595_DATA_LOW;                              //clean all port
    LCDSHCP_LOW;
    LCDSTCP_LOW;
}


void dvLCDInit(void)
{

// the gpio is inited in GPIO_Configuration()
    LCD_RS_CMD;
    delay_nus(60);
    // LCD_RW_WRITE;
    LCD_Send_595(0x38);        // display mode setting
    delay_nus(60);
    LCD_Enable;
    LCD_Send_595(0x0C);
    delay_nus(60);
    LCD_Enable;
    LCD_Send_595(0x06);
    delay_nus(60);
    LCD_Enable;
    LCD_Send_595(0x01);       // clearn the display
    delay_nus(60);
    LCD_Enable;
    LCD_Send_595(0x38);
    delay_nus(60);
    LCD_Enable;
    delay_nms(60);      //delay. it will take  a moment time
}

 void dvLCDLineDraw(BYTE  cLine, BYTE cPostion, PBYTE string)
{
    BYTE cAddr = (cLine == 0) ? (0x80+cPostion) : (0xc0+cPostion);
    unsigned char i = 0;
    LCD_RS_CMD;
    LCD_Send_595(cAddr);        // data register address
    delay_nus(100);
    LCD_Enable;
    delay_nus(100);
    LCD_RS_DAT;
    while (*string && (i+cPostion < 16))
    {
        LCD_Send_595(*string);
        delay_nus(100);
        LCD_Enable;
        string++;
        i++;
    }
    // 这里好像不应该擦除
    //*
    while (i < 16)
    {
        LCD_Send_595(0x20);
        delay_nus(100);
        LCD_Enable;
        i++;
    }
    //*/
}



void dvLCD_On(BOOL bEn)
{
    if (bEn)
    {
        BL_ON;
    }
    else
    {
        BL_OFF;
    }

}


