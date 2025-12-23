#ifndef __OLED_H
#define __OLED_H

#include "AllHeader.h"

//-----------------OLED端口定义----------------

#define OLED_SCL_Clr() DL_GPIO_clearPins(OLED_PORT,OLED_SCL_PIN)	//SCL
#define OLED_SCL_Set() DL_GPIO_setPins(OLED_PORT,OLED_SCL_PIN)

#define OLED_SDA_Clr() DL_GPIO_clearPins(OLED_PORT,OLED_SDA_PIN)	//DIN
#define OLED_SDA_Set() DL_GPIO_setPins(OLED_PORT,OLED_SDA_PIN)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


void OLED_SET(void);	//初始化


void OLED_DisPlay_On(void);	//启动屏幕
void OLED_DisPlay_Off(void);	//关闭屏幕
void OLED_ColorTurn(u8 i);	//1=反相
void OLED_DisplayTurn(u8 i);	//1=反转180
void OLED_Refresh(void);	//将显存OLED_GRAM数据搬到屏幕上
void OLED_Clear(void);	//清屏
void OLED_ClearRF(void);	//注意：误删。只清除OLED_GRAM，不Refresh。

void OLED_DrawPoint(u8 x, u8 y, u8 t);	//画点(x,y)，t=0是擦掉点
void OLED_DrawLineX(u8 x, u8 mode);
void OLED_DrawLineY(u8 y, u8 mode);
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);	//画线(x1,y1)->(x2,y2)
void OLED_DrawBoxLine(u8 x0, u8 y0, u8 x1, u8 y1, u8 mode);	//画框
void OLED_DrawBoxXuLine(u8 x0, u8 y0, u8 x1, u8 y1, u8 linelong, u8 mode);	//虚线框
void OLED_DrawBox(u8 x0, u8 y0, u8 x1, u8 y1, u8 mode);	//画矩(实心)
void OLED_DrawCircle(u8 x, u8 y, u8 r, u8 mode);	//圆(x,y),r

u8 StringLenX(u8 size, u8 num);	//字符串长度
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode);	//显示普通字符(x,y,编号,大小,反相=0<0就是写上0,1就是写上1>)
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1, u8 mode);	//显示字符串
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1, u8 mode);	//显示数字(x,y,num,位数,大小,反相=0)
void OLED_ShowNumNoLen(u8 x, u8 y, u32 num, u8 size1, u8 mode);
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1, u8 mode);	//中文

//*********************************************************************************************

void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat, u8 mode);

#endif
