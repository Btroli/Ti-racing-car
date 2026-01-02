#include "ti_msp_dl_config.h"

#include "AllHeader.h"

#define ecd_L motorL_encoder.count
#define ecd_R motorR_encoder.count

#define oled_x 0
#define oled_y 0

int8_t i = 0;

//pid0
int16_t SPDL = 0, SPDR = 0;
uint8_t Kp = 2, Ki = 3, Kd = 1;
int16_t PL = 0, PR = 0, pre_PL = 0, pre_PR = 0, sum_PL = 0, sum_PR = 0;
int8_t GL = 2, GR = 2;

//pid1
static const int8_t jq[8] = {25, 20, 12, 5, -5, -12, -20, -25};
//volatile float LKp = 5, LKi = 0.025, LKd = 18;
volatile float LKp = 5, LKi = 0.025, LKd = 12;
int8_t Er, pre_Er;
int16_t sum_Er, G_temp;
uint8_t GLR = 40;

//巡线
uint8_t Last_ir_bits;

//竞速赛
int8_t beep_num = 0;

void pid0(void);
void pid1(void);

void loop0(void);

void BEEP(void);

void Stime_loop(void);
void beep_loop(void);
void rgb_loop(void);

int main(void) {
	SYSCFG_DL_init();

	OLED_SET();
	Init_Motor_PWM();
	encoder_init();
	Timer_Init();
	USART_Init();

	uart0_send_string("$0,0,1#");
	Motor_Stop(0);

	while (!ReadKEY1);

	while (1) {
		loop0();
	}
}

void rgb_loop(void) {
	static uint8_t phase = 0;  // 0-255

	uint8_t r = (phase < 128) ? 255 - phase * 2 : 0;
	uint8_t g = (phase >= 64 && phase < 192) ? 255 - abs(phase - 128) * 2 : 0;
	uint8_t b = (phase >= 128) ? (phase - 128) * 2 : 0;

	set_ALL_RGB_COLOR((r << 16) | (g << 8) | b);
	phase++;
	delay_ms(20);
}

void beep_loop(void) {
	if (!ReadKEY2) {
		while (!ReadKEY2);
		beep_num++;
		beep_num = -beep_num;
	}

	if (beep_num < 0) {
		beep_num = -beep_num;
		beep_count = 2 * beep_num;
	}
	delay_cycles(800000);
}

void BEEP(void) {
	beep_num++;
	beep_count = 2 * beep_num;
}

void Stime_loop(void) {
	if (!ReadKEY2)
		Stime = 0;
	if (!ReadKEY3) {
		char time_str[10];
		sprintf(time_str, "%03d.%02d s", Stime / 100, Stime % 100);
		OLED_ClearRF();
		OLED_ShowString(3, 3, time_str, 12, 1);
		OLED_Refresh();

		while (1);
	}
}

void loop0(void) {

	if (ir_bits) {
		pid1();
		Last_ir_bits = ir_bits;
		//if (Last_ir_bits == 0x11111111)
		//	BEEP();
	} else {
		sum_Er = 0;
		if (Last_ir_bits & LEFT) {
			GL = -25;
			GR = 40;
		} else if (Last_ir_bits & RIGHT) {
			GL = 40;
			GR = -25;
		}
	}

	pid0();

	delay_cycles(800000 * 5);
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

	G_temp = LKp * Er + LKi * sum_Er + LKd * (Er - pre_Er);
	G_temp /= 20;

	GL = GLR + G_temp;
	GR = GLR - G_temp;

	GL = (-20 < GL && GL < 70) ? GL : ((GL < 0) ? -20 : 70);
	GR = (-20 < GR && GR < 70) ? GR : ((GR < 0) ? -20 : 70);

}
