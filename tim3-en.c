#include "conf.h"
#include "f0-tim.h"
#include "f0-rcc.h"


void setup_tim3()
{
	if (tim3->cr1.cen)
		return;

	rcc->apb1enr.tim3_en = 1;
	/* Setup tim3 */
	tim3->arr = 2550; /* auto-reload aka period */
	//tim3->cnt = 500;
	tim3->psc = 8 - 1; /* prescaler */
	tim3->ccmr.ch_lcdbg.out.ccs = TIM_CCS_OUT; /* output */
	tim3->ccmr.ch_lcdbg.out.ocm = 6; /* 110 -- pwm mode 1 */
	tim3->ccmr.ch_lcdbg.out.ocpe = 1; /* preload enable, enables changing ccr1
					on the fly */
//	tim3->cr1.arpe = 1; /* auto-preload enable */
	tim3->cr1.dir = TIM_DIR_UPCNT; /* upcounter */
//	tim3->ccer.cc1p = 0; /* polarity */
//	tim3->ccer.cc1ne = 0;
//	tim3->egr.cc1g = 1; /* capture/compare generation */
	tim3->egr.ug = 1; /* update generation */
	tim3->bdtr.moe = 1; /* main output enable */
	tim3->cr1.cen = 1; /* enable */
}

