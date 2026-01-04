#include "ti_msp_dl_config.h"

#include "AllHeader.h"

#define ecd_L motorL_encoder.count
#define ecd_R motorR_encoder.count

#define oled_x 0
#define oled_y 0

int8_t i = 0;

unsigned int rgbN=0;

//pid0
int16_t SPDL = 0, SPDR = 0;
uint8_t Kp = 2, Ki = 3, Kd = 1;
int16_t PL = 0, PR = 0, pre_PL = 0, pre_PR = 0, sum_PL = 0, sum_PR = 0;
int8_t GL = 0, GR = 0;

//pid1
static const int8_t jq[8] = {15, 20, 12, 8, -8, -12, -20, -15};
//volatile float LKp = 5, LKi = 0.025, LKd = 18;
// volatile float LKp = 4.5, LKi = 0, LKd = 18;
// volatile float LKp =8, LKi = 0, LKd = 18;
// volatile float LKp = 4.2, LKi = 0, LKd = 14;
// volatile float LKp = 3.7, LKi = 0, LKd = 16.6;
volatile float LKp = 4.7, LKi = 0, LKd = 15.9;

// volatile float LKp = 5.3, LKi = 0, LKd = 24.6;
int8_t Er, pre_Er;
int16_t sum_Er, G_temp;
uint8_t GLR = 40;

//巡线
uint8_t Last_ir_bits;

void pid0(void);
void pid1(void);

void loop0(void);
void loop1(void);

void BEEP(void);

void Stime_loop(void);
void beep_loop(void);
void rgb_loop(void);

void Timer_pid_Init(void) {
	NVIC_ClearPendingIRQ(TIMER_pid_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_pid_INST_INT_IRQN);
	DL_TimerG_startCounter(TIMER_pid_INST);
}

void TIMER_pid_INST_IRQHandler(void) {
	if ( DL_TimerG_getPendingInterrupt(TIMER_pid_INST) == DL_TIMER_IIDX_ZERO )
		if (beep_num < 5)
			loop0();
		else
			loop1();

		if (rgbN++%25==0)
			set_ALL_RGB_COLOR(rgbColors[rand() % 7]);
}

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
	srand(Stime);
	set_ALL_RGB_COLOR(rgbColors[rand() % 7]);
	while (ReadKEY1);
	Timer_pid_Init();
	Stime = 0;

	while(1);

	while (0) {
		if (beep_num < 5)
			loop0();
		else
			loop1();
	}
}

void loop0(void) {

	if (ir_bits) {
		pid1();
		if (((ir_bits&15)==15)||((ir_bits&240)==240))
			Last_ir_bits = ir_bits;
	} else {
		sum_Er = 0;
		if (Last_ir_bits & LEFT) {
			GL = -30;
			GR = 30;
		} else if (Last_ir_bits & RIGHT) {
			GL = 30;
			GR = -30;
		}
	}

	pid0();
}

void loop1(void) {
	Motor_Stop(1);
	Stime_loop();
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

void Stime_loop(void) {

	char time_str[10];
	sprintf(time_str, "%03d.%02d s", Stime / 100, Stime % 100);
	OLED_ClearRF();
	OLED_ShowString(3, 3, time_str, 12, 1);
	OLED_Refresh();

	while (1) {
		set_ALL_RGB_COLOR(rgbColors[rand() % 7]);
		delay_ms(500);
	}

}
