#include "timer.h"


//20ms定时器
void Timer_20ms_Init(void) {
	//打开20ms定时器
	NVIC_ClearPendingIRQ(TIMER_20ms_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_20ms_INST_INT_IRQN);
	DL_TimerG_startCounter(TIMER_20ms_INST);
}


volatile int8_t beep_count = 0, beep_num = 0;
int8_t beep_timecount = 0;

void TIMER_20ms_INST_IRQHandler(void) {
	if ( DL_TimerG_getPendingInterrupt(TIMER_20ms_INST) == DL_TIMER_IIDX_ZERO ) {
		IRDataAnalysis();
		encoder_update();

#if BEEP_KG
		if (beep_count) {
			if (beep_timecount++ >= 5) {
				beep_timecount = 0;
				Buzzer_TOG_state;
				beep_count--;
			}
		} else if (ir_bits == 0b11111111) {
			beep_num++;
			beep_count = 2 * beep_num;
		}
#endif

	}
}

void Timer_10ms_Init(void) {
	NVIC_ClearPendingIRQ(TIMER_10ms_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_10ms_INST_INT_IRQN);
	DL_TimerG_startCounter(TIMER_10ms_INST);
}

volatile uint16_t Stime;

void TIMER_10ms_INST_IRQHandler(void) {
	if ( DL_TimerG_getPendingInterrupt(TIMER_10ms_INST) == DL_TIMER_IIDX_ZERO )
		Stime++;
}

void Timer_Init(void) {
	Timer_20ms_Init();
	Timer_10ms_Init();
}
