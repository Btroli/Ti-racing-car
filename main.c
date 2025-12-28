#include "ti_msp_dl_config.h"

#include "AllHeader.h"

//#define LEFT	0b11100000
//#define MID 	0b00011000
//#define RIGHT	0b00000111
#define LEFT	0b00011111
#define MID 	0b11100111
#define RIGHT	0b11111000

int8_t i = 0, n = -8;

int main(void) {
	SYSCFG_DL_init();

	OLED_SET();
	Init_Motor_PWM();
	encoder_init();
	Timer_20ms_Init();

	USART_Init();

	uart0_send_string("$0,0,1#");
	Motor_Stop(0);

	while (1) {
		//if (ir_bits & MID)
		//	Motor_Run(200, 200);
		//else if (ir_bits & LEFT)
		//	Motor_Run(200, 300);
		//else if (ir_bits & RIGHT)
		//	Motor_Run(300, 200);

		if (n < 0)
			set_ALL_RGB_COLOR(rgbColors[-n]);
		else
			set_ALL_RGB_COLOR(rgbColors[n]);

		if (n++ == 8)
			n = -8;

		for (i = 0; i < 8; i++) {
			if ((ir_bits >> i) & 1)
				OLED_DrawBox(15 * i, 18, 10 + 15 * i, 28, 1);
			else
				OLED_DrawBoxLine(15 * i, 18, 11 + 15 * i, 29, 1);
		}
		OLED_Refresh();
		OLED_ClearRF();
		delay_cycles(800000);
	}
}
