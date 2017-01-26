#include "conf.h"

#ifndef CONF_F0
#warning camsig not supported on target
void setup_camsig() {}
void camsig_set(int state) {}
#else

#include "camsig.h"
#include "f0-gpio.h"
#include "f0-rcc.h"

void setup_camsig()
{
	rcc->ahbenr.iop_camsig_en = 1;
	io_camsig->moder.pin_camsig = GPIO_MODER_OUT;
	io_camsig->ospeedr.pin_camsig = GPIO_OSPEEDR_LOW;
	io_camsig->otyper.pin_camsig = GPIO_OTYPER_PP;
	io_camsig->bsrr.reset.pin_camsig = 1;
}

void camsig_set(int state)
{
	if (state)
		io_camsig->bsrr.set.pin_camsig = 1;
	else
		io_camsig->bsrr.reset.pin_camsig = 1;
}

#endif
