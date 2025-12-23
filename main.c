#include "ti_msp_dl_config.h"

#include "AllHeader.h"


int main(void)
{
    SYSCFG_DL_init();

	OLED_SET();

	while (1) {
		OLED_ShowChar(3,3,'A',16,1);
		OLED_Refresh();

		delay_cycles(800000000);
	}
}

