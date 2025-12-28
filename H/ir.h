#ifndef _IR_H_
#define _IR_H_

#include "AllHeader.h"

/* 串口缓冲区的数据长度 */
#define USART_RECEIVE_LENGTH  200
#define IR_Num 8 //探头数量

extern volatile u8 IR_recv_complete_flag;
//extern u8 IR_Data_number[];
extern uint8_t ir_bits;

void uart0_send_char(char ch);
void uart0_send_string(char* str);
void IR_usart_config(void);
void IRDataAnalysis(void);
#endif
