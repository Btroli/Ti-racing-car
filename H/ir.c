#include "ir.h"
#include "stdio.h"
#include "string.h"

//定义串口接收缓冲区长度
#define USART_RECEIVE_LENGTH 200

uint8_t ir_bits;

//u8 IR_Data_number[IR_Num];	//存储解析出的数字值
char IR_recv_buff[USART_RECEIVE_LENGTH];	//串口接收缓冲区
volatile uint_fast8_t IR_recv_length = 0;	//接收数据长度
volatile u8 IR_recv_complete_flag = 0;	//接收完成标志位

//红外接收串口配置函数
void IR_usart_config(void) {
	//清除串口中断标志
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	//使能串口中断
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

}

//红外数据解析函数

void IRDataAnalysis(void) {
	char temp[60] = {0};	//临时数据缓冲区
	char *buff = NULL;	//指向有效数据起始地址的指针
	uint8_t head = 0;	//头指针，用于查找数据头
	uint8_t end = 0;	//尾指针，用于查找数据尾

	//没有接收到数据 或者 数据没有接收完成 则不进行处理
	if (IR_recv_complete_flag == 0)
		return;

	//关中断，防止在解析过程中接收到新数据
	__disable_irq();

	//找到格式的头的第一个 '$'
	while ((IR_recv_buff[head] != '$') && (head < IR_recv_length)) {
		head++;
	}
	if (head == IR_recv_length) {
		//未找到数据头，清除接收标志，等待下一次接收
		IR_recv_complete_flag = 0;
		IR_recv_length = 0;
		__enable_irq();
		return;
	}

	buff = &IR_recv_buff[head];	//设置有效数据起始地址

	//找到结尾 '#'
	while ((buff[end] != '#') && (end < IR_recv_length)) {
		end++;
	}
	if ((head + end) == IR_recv_length) {
		//未找到数据尾，清除接收标志，等待下一次接收
		IR_recv_complete_flag = 0;
		IR_recv_length = 0;
		__enable_irq();
		return;
	}

	//复制数据到 temp 缓冲区
	//if (end + 1 < sizeof(temp)) {
	if (end + 1 < 60) {
		strncpy(temp, buff, end + 1);
		temp[end + 1] = '\0';	//确保字符串终止

		//检查数据格式是否正确（$D,...）
		//if (temp[0] == '$' && temp[1] == 'D') {
		//	uint8_t idx = 0;
		//	for (char *p = &temp[2]; idx < IR_Num && *p != '#'; ++p)
		//		if (*p == 'x' && *(p + 1) >= '1' && *(p + 1) <= '8') {
		//			p += 2;
		//			if (*p == ':' && (*(p + 1) == '0' || *(p + 1) == '1'))
		//				IR_Data_number[idx++] = *++p - '0';
		//		}
		//}

		if (temp[0] == '$' && temp[1] == 'D') {
			uint8_t bits = 0;

			//原始方案
			//for (char *p = &temp[2]; *p != '#'; ++p)
			//	if (*p == 'x') {
			//		uint8_t n = p[1] - '0';
			//		if (n >= 1 && n <= 8) {
			//			char *q = p + 2;
			//			while (*q == ' ')
			//				++q;
			//			if (*q == ':' && (q[1] == '0' || q[1] == '1'))
			//				bits |= (uint8_t)(q[1] - '0') << (n - 1);
			//		}
			//	}

			//优化1
			for (char *p = &temp[2]; *p != '#'; ++p)
				if (*p == 'x' && p[1] >= '1' && p[1] <= '8') {
					uint8_t pos = p[1] - '1';
					if (p[2] == ':' && (p[3] == '0' || p[3] == '1'))
						bits |= (uint8_t)(p[3] - '0') << pos;
				}

#if LINE
			ir_bits = ~bits;
#else
			ir_bits = bits;
#endif
		}

	}

	//清除接收完成标志位，重置缓冲区，等待下一次接收
	IR_recv_complete_flag = 0;
	IR_recv_length = 0;
	//memset(IR_recv_buff, 0, USART_RECEIVE_LENGTH);
	__enable_irq();	//重新使能中断
}

//UART0中断处理函数

void UART_0_INST_IRQHandler(void) {
	uint8_t RecvDATA = 0;	//接收的字节数据

	//检查中断来源
	switch (DL_UART_getPendingInterrupt(UART_0_INST)) {
		case DL_UART_IIDX_RX:	//接收中断
			RecvDATA = DL_UART_Main_receiveData(UART_0_INST);	//读取接收的数据

			//检查缓冲区是否已满
			if (IR_recv_length >= USART_RECEIVE_LENGTH - 1) {
				//缓冲区满，重置接收状态（丢弃数据）
				IR_recv_complete_flag = 0;
				IR_recv_length = 0;
				break;
			}

			//存储接收的数据到缓冲区
			IR_recv_buff[IR_recv_length++] = RecvDATA;
			IR_recv_buff[IR_recv_length] = '\0';	//确保字符串终止

			//收到 '#' 时标记接收完成
			if (RecvDATA == '#') {
				IR_recv_complete_flag = 1;
			}
			break;

		default:	//其他中断（如发送中断）
			break;
	}
}
