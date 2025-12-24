#include "ti_msp_dl_config.h"

#include "AllHeader.h"

//int8_t i=0;


int main(void)
{
    SYSCFG_DL_init();

	OLED_SET();
	Init_Motor_PWM();
	encoder_init();
	Timer_20ms_Init();

	USART_Init();

	uart0_send_string("$0,0,1#");

	while (1) {
		for (uint8_t j=0;j<8;j++) {
			if (IR_Data_number[j])
				OLED_DrawBoxLine(15*j,18,10+15*j,28,1);
			else
				OLED_DrawBox(15*j,18,11+15*j,29,1);
		}

		OLED_Refresh();
		OLED_ClearRF();

		// Motor_Run(100,100);

		delay_cycles(800000);

	}
}

