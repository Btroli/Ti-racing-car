#include "oledfont.h"
#include "oled.h"

u8 OLED_GRAM[144][4];	//OLED显存缓冲区 OLED display buffer

void IIC_delay(void) {
	delay_us(3);
}

//起始信号
void I2C_Start(void) {
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

//结束信号
void I2C_Stop(void) {
	OLED_SDA_Clr();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Set();
}

//等待信号响应
void I2C_WaitAck(void) { //测数据信号的电平
	OLED_SDA_Set();
	IIC_delay();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

//写入一个字节
void Send_Byte(u8 dat) {
	u8 i;
	for (i = 0; i < 8; i++) {
		if (dat & 0x80) { //将dat的8位从最高位依次写入
			OLED_SDA_Set();
		} else {
			OLED_SDA_Clr();
		}
		IIC_delay();
		OLED_SCL_Set();
		IIC_delay();
		OLED_SCL_Clr();//将时钟信号设置为低电平
		dat <<= 1;
	}
}

//发送一个字节
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat, u8 mode) {
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if (mode) {
		Send_Byte(0x40);
	} else {
		Send_Byte(0x00);
	}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}

//***********************************************************

void OLED_ColorTurn(u8 i) {
	if (i == 0) {
		OLED_WR_Byte(0xA6, OLED_CMD); //正常显示
	}
	if (i == 1) {
		OLED_WR_Byte(0xA7, OLED_CMD); //反色显示
	}
}

void OLED_DisplayTurn(u8 i) {
	if (i == 0) {
		OLED_WR_Byte(0xC8, OLED_CMD); //正常显示
		OLED_WR_Byte(0xA1, OLED_CMD);
	}
	if (i == 1) {
		OLED_WR_Byte(0xC0, OLED_CMD); //反转显示
		OLED_WR_Byte(0xA0, OLED_CMD);
	}
}

//开启OLED显示
void OLED_DisPlay_On(void) {
	OLED_WR_Byte(0x8D, OLED_CMD); //电荷泵使能
	OLED_WR_Byte(0x14, OLED_CMD); //开启电荷泵
	OLED_WR_Byte(0xAF, OLED_CMD); //点亮屏幕
}

//关闭OLED显示
void OLED_DisPlay_Off(void) {
	OLED_WR_Byte(0x8D, OLED_CMD); //电荷泵使能
	OLED_WR_Byte(0x10, OLED_CMD); //关闭电荷泵
	OLED_WR_Byte(0xAE, OLED_CMD); //关闭屏幕
}

//更新显存到OLED
void OLED_Refresh(void) {
	u8 i, n;
	for (i = 0; i < 4; i++) {
		OLED_WR_Byte(0xb0+i, OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x02, OLED_CMD);  //设置低列起始地址
		OLED_WR_Byte(0x10, OLED_CMD);  //设置高列起始地址
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for (n = 0; n < 128; n++) {
			Send_Byte(OLED_GRAM[n][i]);
			I2C_WaitAck();
		}
		I2C_Stop();
	}
}

//清屏函数
void OLED_Clear(void) {
	u8 i, n;
	for (i = 0; i < 4; i++) {
		for (n = 0; n < 128; n++) {
			OLED_GRAM[n][i] = 0; //清除所有数据
		}
	}
	OLED_Refresh();//更新显示
}

//清屏函数
void OLED_ClearRF(void) {
	u8 i, n;
	for (i = 0; i < 4; i++)
		for (n = 0; n < 128; n++)
			OLED_GRAM[n][i] = 0;
}

//***********************************************************

//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_DrawPoint(u8 x, u8 y, u8 t) {
	u8 i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	if (t) {
		OLED_GRAM[x][i] |= n;
	} else {
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
		OLED_GRAM[x][i] |= n;
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
	}
}

//逆天，这都能出错
//画线
//x1,y1:起点坐标
//x2,y2:结束坐标
//void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode) {
//	u16 t;
//	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
//	int incx, incy, uRow, uCol;
//	delta_x = x2 - x1; //计算坐标增量
//	delta_y = y2 - y1;
//	uRow = x1; //画线起点坐标
//	uCol = y1;
//	if (delta_x > 0)
//		incx = 1; //设置单步方向
//	else if (delta_x == 0)
//		incx = 0; //垂直线
//	else {
//		incx = -1;
//		delta_x = -delta_x;
//	}
//	if (delta_y > 0)
//		incy = 1;
//	else if (delta_y == 0)
//		incy = 0; //水平线
//	else {
//		incy = -1;
//		delta_y = -delta_x;
//	}
//	if (delta_x > delta_y)
//		distance = delta_x; //选取基本增量坐标轴
//	else
//		distance = delta_y;
//	for (t = 0; t < distance + 1; t++) {
//		OLED_DrawPoint(uRow, uCol, mode); //画点
//		xerr += delta_x;
//		yerr += delta_y;
//		if (xerr > distance) {
//			xerr -= distance;
//			uRow += incx;
//		}
//		if (yerr > distance) {
//			yerr -= distance;
//			uCol += incy;
//		}
//	}
//}

void OLED_DrawLine(u8 x0, u8 y0, u8 x1, u8 y1, u8 mode) {
	int dx, dy, sx, sy, err, e2;

	dx = abs(x1 - x0);
	dy = abs(y1 - y0);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = dx - dy;

	while (1) {
		OLED_DrawPoint(x0, y0, mode);

		if (x0 == x1 && y0 == y1)
			break;

		e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
}

void OLED_DrawBoxLine(u8 x0, u8 y0, u8 x1, u8 y1, u8 mode) {
	u8 n = x0;
	if (x0 < x1) {
		n--;
		while (n != x1) {
			n++;
			OLED_DrawPoint(n, y0, mode);
			OLED_DrawPoint(n, y1, mode);
		}
	} else {
		n++;
		while (n != x1) {
			n--;
			OLED_DrawPoint(n, y0, mode);
			OLED_DrawPoint(n, y1, mode);
		}
	}
	n = y0;
	if (y0 < y1) {
		n--;
		while (n != y1) {
			n++;
			OLED_DrawPoint(x0, n, mode);
			OLED_DrawPoint(x1, n, mode);
		}
	} else {
		n++;
		while (n != y1) {
			n--;
			OLED_DrawPoint(x0, n, mode);
			OLED_DrawPoint(x1, n, mode);
		}
	}
}
//void OLED_DrawBoxXuLine(u8 x0, u8 y0, u8 x1, u8 y1, u8 linelong, u8 mode) {
//	u8 n = x0, m = 0;	//计划使用两参：实长、虚长。目前用不到。
//	if (x0 < x1) {
//		n--;
//		while (n != x1) {
//			n++;
//			m++;
//			if (m < linelong) {
//				OLED_DrawPoint(n, y0, mode);
//				OLED_DrawPoint(n, y1, mode);
//			} else if (m + 1 == 2 * linelong)
//				m = 0;
//		}
//	} else {
//		n++;
//		while (n != x1) {
//			n--;
//			m++;
//			if (m < linelong) {
//				OLED_DrawPoint(n, y0, mode);
//				OLED_DrawPoint(n, y1, mode);
//			} else if (m + 1 == 2 * linelong)
//				m = 0;
//		}
//	}
//	n = y0;
//	m = 0;
//	if (y0 < y1) {
//		n--;
//		while (n != y1) {
//			n++;
//			m++;
//			if (m < linelong) {
//				OLED_DrawPoint(x0, n, mode);
//				OLED_DrawPoint(x1, n, mode);
//			} else if (m + 1 == 2 * linelong)
//				m = 0;
//		}
//	} else {
//		n++;
//		while (n != y1) {
//			n--;
//			m++;
//			if (m < linelong) {
//				OLED_DrawPoint(x0, n, mode);
//				OLED_DrawPoint(x1, n, mode);
//			} else if (m + 1 == 2 * linelong)
//				m = 0;
//		}
//	}
//}
void OLED_DrawBoxXuLine(u8 x0, u8 y0, u8 x1, u8 y1, u8 linelong, u8 mode) {
	u8 n = x0, m = 0;
	if (x0 < x1) {
		n--;
		while (n != x1) {
			n++;
			m++;
			if (m <= linelong) {
				OLED_DrawPoint(n, y0, mode);
				OLED_DrawPoint(n, y1, mode);
			} else if (m == 2 * linelong)
				m = 0;
		}
	} else {
		n++;
		while (n != x1) {
			n--;
			m++;
			if (m <= linelong) {
				OLED_DrawPoint(n, y0, mode);
				OLED_DrawPoint(n, y1, mode);
			} else if (m == 2 * linelong)
				m = 0;
		}
	}
	n = y0;
	m = 0;
	if (y0 < y1) {
		n--;
		while (n != y1) {
			n++;
			m++;
			if (m <= linelong) {
				OLED_DrawPoint(x0, n, mode);
				OLED_DrawPoint(x1, n, mode);
			} else if (m == 2 * linelong)
				m = 0;
		}
	} else {
		n++;
		while (n != y1) {
			n--;
			m++;
			if (m <= linelong) {
				OLED_DrawPoint(x0, n, mode);
				OLED_DrawPoint(x1, n, mode);
			} else if (m == 2 * linelong)
				m = 0;
		}
	}
}

void OLED_DrawBox(u8 x0, u8 y0, u8 x1, u8 y1, u8 mode) {
	u8 x, y;
	if (x0 < x1)
		if (y0 < y1)
			for (x = x0; x != x1; x++)
				for (y = y0; y != y1; y++)
					OLED_DrawPoint(x, y, mode);
		else
			for (x = x0; x != x1; x++)
				for (y = y0; y != y1; y--)
					OLED_DrawPoint(x, y, mode);
	else {
		if (y0 < y1)
			for (x = x0; x != x1; x--)
				for (y = y0; y != y1; y++)
					OLED_DrawPoint(x, y, mode);
		else
			for (x = x0; x != x1; x--)
				for (y = y0; y != y1; y--)
					OLED_DrawPoint(x, y, mode);
	}
}

void OLED_DrawLineY(u8 x, u8 mode) {
	u8 y;
	for (y = 0; y < 32; y++)
		OLED_DrawPoint(x, y, mode);
}

void OLED_DrawLineX(u8 y, u8 mode) {
	u8 x;
	for (x = 0; x < 128; x++)
		OLED_DrawPoint(x, y, mode);
}

//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(u8 x, u8 y, u8 r, u8 mode) {
	int a, b, num;
	a = 0;
	b = r;
	if (r < 9) {
		while (2 * b > r) {
			OLED_DrawPoint(x + a, y - b, mode);
			OLED_DrawPoint(x - a, y - b, mode);
			OLED_DrawPoint(x - a, y + b, mode);
			OLED_DrawPoint(x + a, y + b, mode);

			OLED_DrawPoint(x + b, y + a, mode);
			OLED_DrawPoint(x + b, y - a, mode);
			OLED_DrawPoint(x - b, y - a, mode);
			OLED_DrawPoint(x - b, y + a, mode);

			a++;
			num = (a * a + b * b) - r * r;
			if (num > 0) {
				b--;
				a--;
			}
		}
	} else {
		while (7 * b * b > 3 * r * r) {
			OLED_DrawPoint(x + a, y - b, mode);
			OLED_DrawPoint(x - a, y - b, mode);
			OLED_DrawPoint(x - a, y + b, mode);
			OLED_DrawPoint(x + a, y + b, mode);

			OLED_DrawPoint(x + b, y + a, mode);
			OLED_DrawPoint(x + b, y - a, mode);
			OLED_DrawPoint(x - b, y - a, mode);
			OLED_DrawPoint(x - b, y + a, mode);

			a++;
			num = (a * a + b * b) - r * r;
			if (num > 0) {
				b--;
				a--;
			}
		}
	}
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode) {
	u8 i, m, temp, size2, chr1;
	u8 x0 = x, y0 = y;
	if (size1 == 8)
		size2 = 6;
	else
		size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2); //得到字体一个字符对应点阵集所占的字节数
	chr1 = chr - ' '; //计算偏移后的值
	for (i = 0; i < size2; i++) {
		if (size1 == 8) {
			temp = asc2_0806[chr1][i];   //调用0806字体
		} else if (size1 == 12) {
			temp = asc2_1206[chr1][i];   //调用1206字体
		} else if (size1 == 16) {
			temp = asc2_1608[chr1][i];   //调用1608字体
		} else if (size1 == 24) {
			temp = asc2_2412[chr1][i];   //调用2412字体
		} else
			return;
		for (m = 0; m < 8; m++) {
			if (temp & 0x01)
				OLED_DrawPoint(x, y, mode);
			else
				OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if ((size1 != 8) && ((x - x0) == size1 / 2)) {
			x = x0;
			y0 = y0 + 8;
		}
		y = y0;
	}
}


//显示字符串
//x,y:起点坐标
//size1:字体大小
//*chr:字符串起始地址
//mode:0,反色显示;1,正常显示
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1, u8 mode) {
	while ((*chr >= ' ') && (*chr <= '~')) { //判断是不是非法字符!
		OLED_ShowChar(x, y, *chr, size1, mode);
		if (size1 == 8)
			x += 6;
		else
			x += size1 / 2;
		chr++;
	}
}

u8 StringLenX(u8 size, u8 num) {
	if (size == 8)
		size = 6;
	else
		size /= 2;
	return size * num;
}

//m^n
u32 OLED_Pow(u8 m, u8 n) {
	u32 result = 1;
	while (n--) {
		result *= m;
	}
	return result;
}

//显示数字
//x,y :起点坐标
//num :要显示的数字
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1, u8 mode) {
	u8 t, temp, m = 0;
	if (size1 == 8)
		m = 2;
	for (t = 0; t < len; t++) {
		temp = (num / OLED_Pow(10, len - t - 1)) % 10;
		if (temp == 0) {
			OLED_ShowChar(x + (size1 / 2 + m)*t, y, '0', size1, mode);
		} else {
			OLED_ShowChar(x + (size1 / 2 + m)*t, y, temp + '0', size1, mode);
		}
	}
}
void OLED_ShowNumNoLen(u8 x, u8 y, u32 num, u8 size1, u8 mode) {
	u8 t, temp, len = 1, m = 0;
	u32 nnum = num;
	while (nnum / 10) {
		nnum /= 10;
		len++;
	}
	if (size1 == 8)
		m = 2;
	for (t = 0; t < len; t++) {
		temp = (num / OLED_Pow(10, len - t - 1)) % 10;
		if (temp == 0) {
			OLED_ShowChar(x + (size1 / 2 + m)*t, y, '0', size1, mode);
		} else {
			OLED_ShowChar(x + (size1 / 2 + m)*t, y, temp + '0', size1, mode);
		}
	}
}

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//mode:0,反色显示;1,正常显示
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1, u8 mode) {
	u8 m, temp;
	u8 x0 = x, y0 = y;
	u16 i, size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1; //得到字体一个字符对应点阵集所占的字节数
	for (i = 0; i < size3; i++) {
		if (size1 == 16) {
			temp = Hzk1[num][i];   //调用16*16字体
		} else if (size1 == 24) {
			temp = Hzk2[num][i];   //调用24*24字体
		} else if (size1 == 32) {
			temp = Hzk3[num][i];   //调用32*32字体
		} else if (size1 == 64) {
			temp = Hzk4[num][i];   //调用64*64字体
		} else
			return;
		for (m = 0; m < 8; m++) {
			if (temp & 0x01)
				OLED_DrawPoint(x, y, mode);
			else
				OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if ((x - x0) == size1) {
			x = x0;
			y0 = y0 + 8;
		}
		y = y0;
	}
}

void OLED_SET(void) {
	//delay_ms(200);	//延时等待OLED上电稳定 Delay for OLED power stabilization

	//OLED控制器初始化命令序列 OLED controller initialization sequence
	OLED_WR_Byte(0xAE, OLED_CMD);	//关闭显示避免闪烁 Display off to avoid flicker
	OLED_WR_Byte(0x00, OLED_CMD);	//设置列地址低4位 Set lower column address
	OLED_WR_Byte(0x10, OLED_CMD);	//设置列地址高4位 Set higher column address
	OLED_WR_Byte(0x00, OLED_CMD);	//设置显示起始行 Set display start line
	OLED_WR_Byte(0xB0, OLED_CMD);	//设置页地址 Set page address

	OLED_WR_Byte(0x81, OLED_CMD);	//对比度控制 Contrast control
	OLED_WR_Byte(0xFF, OLED_CMD);	//对比度值128 Contrast value 128

	OLED_WR_Byte(0xA1, OLED_CMD);	//段重映射水平翻转 Segment remap horizontal flip
	OLED_WR_Byte(0xA6, OLED_CMD);	//正常显示非反色 Normal display not inverse

	OLED_WR_Byte(0xA8, OLED_CMD);	//设置多路复用比率 Set multiplex ratio
	OLED_WR_Byte(0x1F, OLED_CMD);	//设置为1/32占空比 Set duty 1/32

	OLED_WR_Byte(0xC8, OLED_CMD);	//COM扫描方向垂直翻转 COM scan direction vertical flip

	OLED_WR_Byte(0xD3, OLED_CMD);	//设置显示偏移 Set display offset
	OLED_WR_Byte(0x00, OLED_CMD);	//无偏移 No offset

	OLED_WR_Byte(0xD5, OLED_CMD);	//设置显示时钟分频 Set oscillator division
	OLED_WR_Byte(0x80, OLED_CMD);	//默认值 Default value

	OLED_WR_Byte(0xD9, OLED_CMD);	//设置预充电周期 Set pre-charge period
	OLED_WR_Byte(0x1F, OLED_CMD);	//推荐值 Recommended value

	OLED_WR_Byte(0xDA, OLED_CMD);	//设置COM引脚配置 Set COM pins configuration
	OLED_WR_Byte(0x00, OLED_CMD);	//默认配置 Default configuration

	OLED_WR_Byte(0xDB, OLED_CMD);	//设置VCOMH电压 Set VCOMH deselect level
	OLED_WR_Byte(0x40, OLED_CMD);	//约0.77xVCC Approx 0.77xVCC

	OLED_WR_Byte(0x8D, OLED_CMD);	//电荷泵设置 Charge pump setting
	OLED_WR_Byte(0x14, OLED_CMD);	//启用电荷泵 Enable charge pump


	OLED_WR_Byte(0xAF, OLED_CMD);	//开启显示 Display ON

	OLED_Clear();
}
