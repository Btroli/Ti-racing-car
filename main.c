#include "ti_msp_dl_config.h"

#define TOG(gpio,pins) DL_GPIO_togglePins(gpio,pins)

int main(void) {
	SYSCFG_DL_init();

	while (1) {
		delay_cycles(32000000);
		TOG(GPIOA, GPIO_GRP_0_PIN_0_PIN);
	}
}
