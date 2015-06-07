
#include <stdbool.h>
extern void assert(bool);

#include "conf.h"
#include "f0-rtc.h"
#include "f0-rcc.h"
#include "f0-pwr.h"
#include "clock.h"

void setup_clock(void)
{
	rcc->apb1enr.pwr_en = 1;
	while(!rcc->apb1enr.pwr_en);

	pwr->cr.dbp = 1;

	rcc->bdcr.rtcsel = RCC_RTCSEL_LSI;

	rcc->csr.lsion = 1;
	while(!rcc->csr.lsirdy); /* wait for lsi to get ready */

	rcc->bdcr.rtc_en = 1;

	if (rtc->isr.inits) /* already setup */
		return;

	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->isr.init = 1;
	while (!rtc->isr.initf); /* wait for init mode */

	/* prediv lsi (~40kHz) to gen 1Hz */
	rtc->prer.prediv_s = 320;
	rtc->prer.prediv_a = 125 - 1;

	/* load initial date/time */
	struct rtc_tr a = {
		.su = 1,	.st = 3,
		.mnu = 7,	.mnt = 3,
		.hu = 2,	.ht = 0,
		.pm = RTC_PM_24H,
	};
	rtc->tr = a;
	struct rtc_dr b = {
		.du = 1,	.dt = 3,
		.mu = 5,	.mt = 0,
		.wdu = 7,
		.yu = 5,	.yt = 1,
	};
	rtc->dr = b;

	rtc->cr.fmt = RTC_FMT_12H;

	rtc->isr.init = 0;
	rtc->wpr.key = 0xff; /* random key write enables protection again */
}

void clock_get(struct rtc_tr *time, struct rtc_dr *date)
{
	assert(rtc->isr.rsf);
	*time = rtc->tr;
	*date = rtc->dr;
}

