#include "conf.h"
#include "f0-tim.h"
#include "f0-rcc.h"

void setup_tim1()
{
	if (tim1->cr1.cen)
		return;

	rcc->apb2enr.tim1_en = 1;

	/* Setup tim1 */
	tim1->arr = 2550; /* auto-reload aka period */
	tim1->psc = 32000 - 1; /* prescaler */

//	tim1->cr1.arpe = 1; /* auto-preload enable */
	tim1->cr1.dir = TIM_DIR_UPCNT; /* upcounter */
//	tim1->ccer.cc1p = 0; /* polarity */
//	tim1->ccer.cc1ne = 0;
//	tim1->egr.cc1g = 1; /* capture/compare generation */
	tim1->egr.ug = 1; /* update generation */
	tim1->bdtr.moe = 1; /* main output enable */
	tim1->cr1.cen = 1; /* enable */
}

