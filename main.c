#include "ti_msp_dl_config.h"

#include "AllHeader.h"

int8_t i = 0;

int main(void) {
	SYSCFG_DL_init();

	OLED_SET();
	Init_Motor_PWM();
	encoder_init();
	Timer_20ms_Init();

	USART_Init();

	uart0_send_string("$0,0,1#");
	Motor_Stop(0);

	while (!ReadKEY1);

	for (i = 0; i < 8; i++)
		OLED_ShowChar(3 + 15 * i, 2, '1' + i, 12, 1);

	while (1) {
		if (ir_bits & MID)
			Motor_Run(200, 200);
		else if (ir_bits & LEFT)
			Motor_Run(200, 270);
		else if (ir_bits & RIGHT)
			Motor_Run(270, 200);



		for (i = 0; i < 8; i++) {
			OLED_DrawBox(15 * i, 18, 12 + 15 * i, 30, 1);
			if (1 ^ ir_bits >> i & 1)
				OLED_DrawBox(15 * i + 1, 19, 11 + 15 * i, 29, 0);
		}
		OLED_Refresh();
		delay_cycles(800000);
	}
}
