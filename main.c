#include "ti_msp_dl_config.h"

#include "AllHeader.h"

#define ecd_L motorL_encoder.count
#define ecd_R motorR_encoder.count

#define oled_x 0
#define oled_y 0

int8_t i = 0;

int8_t RL_flag = 0, turn_flag = 0;

//pid0
int16_t SPDL = 0, SPDR = 0;
uint8_t Kp = 2, Ki = 3, Kd = 1;
int16_t PL = 0, PR = 0, pre_PL = 0, pre_PR = 0, sum_PL = 0, sum_PR = 0;
int8_t GL = 2, GR = 2;

//pid1
static const int8_t jq[8] = {25, 20, 12, 5, -5, -12, -20, -25};
volatile float LKp=6, LKi=0, LKd=10;
int8_t Er, pre_Er;
int16_t sum_Er;
uint8_t GLR=40;

//巡线
uint8_t Last_ir_bits;

void pid0(void);
void pid1(void);
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

	while (1) {

		if (ir_bits) {
			pid1();
			Last_ir_bits = ir_bits;
		} else {
			sum_PL = 0;
			sum_PR = 0;

			if (Last_ir_bits & LEFT) {
				GL = -20;
				GR = 50;
			} else if (Last_ir_bits & RIGHT) {
				GL = 50;
				GR = -20;
			}
		}

		pid0();

		OLED_ShowString(oled_y, oled_x, "ecd_L", 12, 1);
		OLED_ShowNum(oled_y, oled_x + 10, ecd_L, 3, 12, 1);
		OLED_ShowString(oled_y + 40, oled_x, "ecd_R", 12, 1);
		OLED_ShowNum(oled_y + 40, oled_x + 10, ecd_R, 3, 12, 1);
		OLED_ShowNum(oled_y, oled_x + 20, SPDL, 3, 12, 1);
		OLED_ShowNum(oled_y + 40, oled_x + 20, SPDR, 3, 12, 1);
		OLED_Refresh();

		delay_cycles(800000);
	}

	while (0) {

		pid0();

		OLED_ShowString(oled_y, oled_x, "ecd_L", 12, 1);
		OLED_ShowNum(oled_y, oled_x + 10, ecd_L, 3, 12, 1);
		OLED_ShowString(oled_y + 40, oled_x, "ecd_R", 12, 1);
		OLED_ShowNum(oled_y + 40, oled_x + 10, ecd_R, 3, 12, 1);
		OLED_ShowNum(oled_y, oled_x + 20, SPDL, 3, 12, 1);
		OLED_ShowNum(oled_y + 40, oled_x + 20, SPDR, 3, 12, 1);
		OLED_Refresh();
		//delay_cycles(800000);
		//delay_cycles(8000000);	//100ms
	}

	while (0) {
		Motor_Run(-200,-200);
		delay_cycles(800000);
	}
}

void pid0(void) {

	Motor_Run(SPDL, SPDR);

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

void pid1(void) {

	pre_Er = Er;

	Er = 0;

	for (i = 0; i < 8; i++)
		if (ir_bits & (1 << i))
			Er -= jq[i];

	if (sum_Er < GLR * 100 && sum_Er > GLR * -100)
		sum_Er += Er;

	GR = LKp * Er + LKi * sum_Er + LKd * (Er - pre_Er);
	GR /= 20;

	GL = GLR + GR;
	GR = GLR - GR;

	GL = (-20 < GL && GL < 70) ? GL : ((GL < 0) ? -20 : 70);
	GR = (-20 < GR && GR < 70) ? GR : ((GR < 0) ? -20 : 70);

}
