
#include <stdbool.h>
extern void assert(bool);

#include "conf.h"
#include "rtc.h"
#include "rcc.h"
#include "pwr.h"
#include "exti.h"
#include "clock.h"
#include "uart.h"


void (*alarm_callb)() = NULL;

#if 0
void clock_exti_rtc()
{
#ifdef CONF_F0
	exti->pr.pr17 = 1; /* writing 1 clears bit */
#elif defined(CONF_L0)
	exti->pr.pif17 = 1; /* writing 1 clears bit */
#endif
	assert(rtc->isr.alraf);
	rtc->isr.alraf = 0;
	if (alarm_callb == NULL)
		return;
	alarm_callb();
}

void clock_alarm(struct rtc_alrmar alrm, struct rtc_alrmassr ss, void (*cb)())
{
	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->cr.alrae = 0;
	while (!rtc->isr.alrawf);

	ss.maskss = 0xf;
	rtc->alrmar = alrm;
	rtc->alrmassr = ss;
	rtc->cr.osel = RTC_OSEL_ALARM_A;
	rtc->cr.alraie = 1;
	rtc->cr.alrae = 1;

	rtc->wpr.key = 0x11; /* relock write protection */
	alarm_callb = cb;
}

void clock_alarm_stop(void (*cb)())
{
	assert(alarm_callb != NULL);
	assert(alarm_callb == cb);
	alarm_callb = NULL;

	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->cr.osel = RTC_OSEL_DISABLED;
	rtc->cr.alrae = 0;
	rtc->cr.alraie = 0;

	rtc->wpr.key = 0x11; /* relock write protection */
}
#endif

void clock_set(struct rtc_dr date, struct rtc_tr time)
{
	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->isr.init = 1;
	while (!rtc->isr.initf); /* wait for init mode */

	rtc->tr = time;
	rtc->dr = date;

	rtc->isr.init = 0;
	rtc->wpr.key = 0x11; /* relock write protection */
	while (!rtc->isr.rsf);
}

void clock_init_wuk()
{
	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

#ifdef CONF_L0
	rtc->cr.wute = 0;
	while (!rtc->isr.wutwf);

	rtc->cr.wucksel = 0;
	rtc->wutr.wut = 2048;
	rtc->isr.wutf = 0;

	exti->rtsr.rt20 = 1;
	exti->imr.im20 = 1;
	//exti->pr.pif20 = 1;
	//exti->emr.em20 = 1;
	//exti->rtsr.rt20 = 1;
	nvic_iser[0] |= 1 << 2;

	rtc->cr.wutie = 1;
	rtc->cr.wute = 1;
#endif
	rtc->wpr.key = 0xff;
}

void clock_init_lse()
{
#ifdef CONF_L4
	rcc->bdcr.rtcsel = RCC_RTCSEL_LSE;

	rcc->bdcr.lseon = 1;
	rcc->bdcr.lsebyp = 0;
	while (!rcc->bdcr.lserdy); /* wait for lse to get ready */
#endif
#ifdef CONF_L0
	rcc->csr.rtcsel = RCC_RTCSEL_LSE;

	rcc->csr.lseon = 1;
	rcc->csr.lsebyp = 0;
	while (!rcc->csr.lserdy); /* wait for lse to get ready */
#endif
}

void setup_clock(void)
{
#ifdef CONF_L4
	rcc->apb1enr1.pwren = 1;
	while (!rcc->apb1enr1.pwren);

	pwr->cr1.dbp = 1;
#elif defined(CONF_L0)
	rcc->apb1enr.pwren = 1;
	while (!rcc->apb1enr.pwren);

	pwr->cr.dbp = 1;
#endif

#ifdef LSI
	_Static_assert(1==2, "Not supported?");
	rcc->bdcr.rtcsel = RCC_RTCSEL_LSI;

	rcc->csr.lsion = 1;
	while (!rcc->csr.lsirdy); /* wait for lsi to get ready */
#else
	clock_init_lse();
#endif

	clock_init_wuk();

#ifdef CONF_L4
	rcc->bdcr.rtcen = 1;
#elif defined(CONF_L0)
	rcc->csr.rtcen = 1;
#endif

	/* Enable RTC interrupts
	 * "RTC Interrupts (combined EXTI lines 17, 19, and 20)" */
	//exti->imr.mr19 = exti->imr.mr20 = exti->imr.mr17 = 1;
	//exti->rtsr.tr19 = exti->rtsr.tr20 = exti->rtsr.tr17 = 1;
	//exti->pr.pr19 = exti->pr.pr20 = exti->pr.pr17 = 1;
	//nvic_iser[0] |= 1 << 2;

	if (rtc->isr.inits) /* already setup */
		return;
	/* ... RTC hasn't been run before */

	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->isr.init = 1;
	while (!rtc->isr.initf); /* wait for init mode */

#ifdef LSI
	/* prediv lsi (~40kHz) to gen 1Hz */
	rtc->prer.prediv_s = 320 - 1;
	rtc->prer.prediv_a = 125 - 1;
#else
	/* prediv lse (32kHz) to gen 1Hz */
	rtc->prer.prediv_a = 128 - 1;
	rtc->prer.prediv_s = 256 - 1;
#endif

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

	rtc->cr.fmt = RTC_FMT_24H;

	rtc->isr.init = 0;
	while (!rtc->isr.rsf);
	rtc->wpr.key = 0xff; /* random key write enables protection again */

}

void clock_get(struct rtc_dr *date, struct rtc_tr *time,
		struct rtc_ssr *subsec)
{
	assert(rtc->isr.rsf);
	if (time != NULL)
		*time = rtc->tr;

	if (date != NULL)
		*date = rtc->dr;
//	*subsec = rtc->ssr;
}

static unsigned seconds = 0;

void clock_cmd(char *cmd, int len)
{
	if (len < 2) {
		uart_print("Say cg(et) or cs(et) instead.\r\n");
		return;
	}

	if (cmd[1] == 'g') {
		struct rtc_tr time;
		clock_get(NULL, &time, NULL);

		const char out[] = {
			'0' + time.ht, '0' + time.hu,
			':',
			'0' + time.mnt, '0' + time.mnu,
			':',
			'0' + time.st, '0' + time.su,
			'\r', '\n'
		};
		uart_print(out);
	} else if (cmd[1] == 's') {
		uart_print("Functionality coming reel suun.\r\n");
	} else if (cmd[1] == 'n') {
		uart_print("Seconds since startup: ");
		uart_print_int(seconds);
		uart_print("\r\n");
	}
}

void rpm_collect_1hz();

void clock_interr()
{
	rtc->isr.wutf = 0;
	exti->pr.pif20 = 1;
	seconds++;
	rpm_collect_1hz();
//	rtc->isr.alraf = 0;
}

