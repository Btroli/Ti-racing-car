#ifndef __BEEP_LED_H_
#define __BEEP_LED_H_

#include "AllHeader.h"


#define TOG(gpio,pins) DL_GPIO_togglePins(gpio,pins)

#define OPEN_MCULED	DL_GPIO_setPins(LED_PORT, LED_MCU_PIN)	//置位LED引脚
#define CLOSE_MCULED	DL_GPIO_clearPins(LED_PORT, LED_MCU_PIN)	//清除LED引脚
#define TOG_MCULED	TOG(LED_PORT, LED_MCU_PIN)

#define Buzzer_open_state	DL_GPIO_setPins(BEEP_PORT, BEEP_Buzzer_PIN)	//置位蜂鸣器引脚
#define Buzzer_close_state	DL_GPIO_clearPins(BEEP_PORT, BEEP_Buzzer_PIN)	//清除蜂鸣器引脚
#define Buzzer_TOG_state	TOG(BEEP_PORT, BEEP_Buzzer_PIN)


#endif
