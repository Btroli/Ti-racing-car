#include "ti_msp_dl_config.h"

#include "AllHeader.h"

#define ecd_L motorL_encoder.count
#define ecd_R motorR_encoder.count

#define oled_x 0
#define oled_y 0

int8_t i = 0;

int8_t RL_flag = 0, turn_flag = 0;

int16_t SPDL = 0, SPDR = 0;
uint8_t Kp = 10, Ki = 6, Kd = 1;
int16_t PL = 0, PR = 0, pre_PL = 0, pre_PR = 0, sum_PL = 0, sum_PR = 0;

int8_t GL = 20, GR = 20;

void pid0(void);

int main(void) {
	SYSCFG_DL_init();

	OLED_SET();
	Init_Motor_PWM();
	encoder_init();
	Timer_20ms_Init();

	USART_Init();

	uart0_send_string("$0,0,1#");
	Motor_Stop(0);

	//while (!ReadKEY1);

	while (0) {
	}

	while (1) {

		pid0();

		Motor_Run(SPDL, SPDR);

		OLED_ShowString(oled_y, oled_x, "ecd_L", 12, 1);
		OLED_ShowNum(oled_y, oled_x + 10, ecd_L, 3, 12, 1);
		OLED_ShowString(oled_y + 40, oled_x, "ecd_R", 12, 1);
		OLED_ShowNum(oled_y + 40, oled_x + 10, ecd_R, 3, 12, 1);
		OLED_ShowNum(oled_y, oled_x + 20, SPDL, 3, 12, 1);
		OLED_ShowNum(oled_y + 40, oled_x + 20, SPDR, 3, 12, 1);
		OLED_Refresh();
		//delay_cycles(800000);
		//delay_cycles(8000000);	//100ms
		delay_cycles(80000);
	}
}

void pid0(void) {
	pre_PL = PL;
	pre_PR = PR;
	PL = GL - ecd_L;
	PR = GR - ecd_R;

	sum_PL += PL;
	sum_PR += PR;

	SPDL = Kp * PL + Ki * sum_PL - Kd * (PL - pre_PL);
	SPDR = Kp * PR + Ki * sum_PR - Kd * (PR - pre_PR);

	if (SPDL > 400)
		SPDL = 400;
	if (SPDL < -400)
		SPDL = -400;
	if (SPDR > 400)
		SPDR = 400;
	if (SPDR < -400)
		SPDR = -400;
}
