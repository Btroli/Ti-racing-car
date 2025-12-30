#ifndef __ALLHEADER_H_
#define __ALLHEADER_H_

//LINE 黑线1，白线0
#define LINE 1
#define Motor_Switch 1

#define LEFT	0b00000111
#define MID 	0b00011000
#define RIGHT	0b11100000

#define u8  uint8_t
#define u16 uint16_t 
#define u32 uint32_t 

#define ReadKEY1 (DL_GPIO_readPins(KEY_button1_PORT, KEY_button1_PIN)&KEY_button1_PIN)
#define ReadKEY2 (DL_GPIO_readPins(KEY_button2_PORT, KEY_button2_PIN)&KEY_button2_PIN)
#define ReadKEY3 (DL_GPIO_readPins(KEY_button3_PORT, KEY_button3_PIN)&KEY_button3_PIN)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "ti_msp_dl_config.h"

#include "delay.h"
#include "usart0.h"

#include "beep_led.h"
#include "RGB.h"
#include "ir.h"
#include "oled.h"

#include "encoder.h"
#include "timer.h"
#include "motor.h"

//#include "app_motor.h"
//#include "bsp_PID_motor.h"
//#include "app_irtracking_eight.h"


#endif

