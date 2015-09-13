
#include "lcd-com.h"
#include "app.h"
#include "clock.h"
#include <stddef.h> /* NULL */
#include <stdbool.h>
extern void assert(bool);

#define row (2)


static void display_clock(struct rtc_dr date, struct rtc_tr time)
{
	lcd_setcaret(0, row);
	lcd_putc('0' + date.yt);
	lcd_putc('0' + date.yu);
	lcd_putc('-');
	lcd_putc('0' + date.mt);
	lcd_putc('0' + date.mu);
	lcd_putc('-');
	lcd_putc('0' + date.dt);
	lcd_putc('0' + date.du);

	lcd_repeat(0, 4); /* shorter space */

	lcd_putc('0' + time.ht + (time.pm == RTC_PM_PM));
	lcd_putc('0' + time.hu);
	lcd_putc(':');
	lcd_putc('0' + time.mnt);
	lcd_putc('0' + time.mnu);
	lcd_putc(':');
	lcd_putc('0' + time.st);
	lcd_putc('0' + time.su);
}

static void upd()
{
	struct rtc_tr time;
	struct rtc_dr date;
	struct rtc_ssr subsec;
	clock_get(&date, &time, &subsec);
	display_clock(date, time);
}

static void sel()
{
	app_update = upd;
	lcd_clear();
	upd();
}

extern struct app app_menu;
static void ev(enum ev_type type, enum ev_key key)
{
	if (type != EV_TYPE_RELEASE)
		return;
	app_push(&app_menu);
}

static void rm()
{
	app_update = NULL;
}

struct app app_info = {
	.select = sel,
	.remove = rm,
	.event = ev,
	.previous = 0,
};

/* app-conf */
static int focus = 0;
enum {
	FOCUS_YEAR,
	FOCUS_MONTH,
	FOCUS_DAY,
	FOCUS_HOUR,
	FOCUS_MINUTE,
	FOCUS_SECOND,
	FOCUS_BACK,
	FOCUS_MAX,
};

static struct rtc_tr clock_time;
static struct rtc_dr clock_date;

static void refocus(int oldfocus)
{
	if (oldfocus <= FOCUS_SECOND) {
		int x = oldfocus * 15;
		if (oldfocus >= FOCUS_HOUR)
			x--; /* 1 shorter space */
		lcd_setcaret(x, row + 1);
		lcd_repeat(0, 5*2);
		lcd_setcaret(x, row - 1);
		lcd_repeat(0, 5*2);
	} else if (oldfocus == FOCUS_BACK) {
		lcd_setcaret(2, 3);
		lcd_repeat(0x0, 4 * 5);
		lcd_setcaret(2, 5);
		lcd_repeat(0x0, 4 * 5);
	}

	if (focus <= FOCUS_SECOND) {
		int x = focus * 15;
		if (focus >= FOCUS_HOUR)
			x--; /* 1 shorter space */
		lcd_setcaret(x, row + 1);
		lcd_repeat(0x1, 5*2);
		lcd_setcaret(x, row - 1);
		lcd_repeat(0x40, 5*2);
	} else if (focus == FOCUS_BACK) {
		lcd_setcaret(2, 3);
		lcd_repeat(0x40, 4 * 5);
		lcd_setcaret(2, 5);
		lcd_repeat(0x1, 4 * 5);
	}
}

static void upd_conf()
{
	struct rtc_ssr subsec;
	clock_get(&clock_date, &clock_time, &subsec);
	display_clock(clock_date, clock_time);
}

static void sel_conf()
{
	sel();
	app_update = upd_conf;
	lcd_setcaret(2, 4);
	lcd_puts("back");
	refocus(FOCUS_SECOND);
}

static void ev_conf(enum ev_type type, enum ev_key key)
{
	if (type != EV_TYPE_RELEASE)
		return;
	if (focus == FOCUS_BACK && (key == EV_KEY_UP || key == EV_KEY_DOWN)) {
		app_pop();
		return;
	}

	if (key == EV_KEY_RIGHT) {
		int oldf = focus;
		focus++;
		if (focus == FOCUS_MAX)
			focus = 0;
		refocus(oldf);
		return;
	} else if (key == EV_KEY_LEFT) {
		int oldf = focus;
		focus--;
		if (focus < 0)
			focus = FOCUS_MAX - 1;
		refocus(oldf);
		return;
	}

	int it = key == EV_KEY_UP ? 1 : -1;
	int z;
	switch (focus) {
	case FOCUS_YEAR:
		z = clock_date.yu + clock_date.yt * 10;
		z += it;
		if (z > 99)
			z = 0;
		else if (z < 0)
			z = 99;
		clock_date.yu = z % 10;
		clock_date.yt = z / 10;
		break;
	case FOCUS_MONTH:
		z = clock_date.mu + clock_date.mt * 10;
		z += it;
		if (z > 12)
			z = 1;
		else if (z < 1)
			z = 12;
		clock_date.mu = z % 10;
		clock_date.mt = z / 10;
		break;
	case FOCUS_DAY:
		z = clock_date.du + clock_date.dt * 10;
		z += it;
		int ubnd = 31;
		int m = clock_date.mu + clock_date.mt * 10;
		if (m == 2) { /* feb */
			/* leap year: 12, 16, 20.. */
			int y = clock_date.yu + clock_date.yt * 10;
			ubnd = y % 4 == 0 ? 29 : 28;
		} else if (m == 4 || m == 6 || m == 9 || m == 11) {
			ubnd = 30;
		}
		if (z > ubnd)
			z = 1;
		else if (z < 1)
			z = ubnd;
		clock_date.du = z % 10;
		clock_date.dt = z / 10;
		break;
	case FOCUS_HOUR: /* there's also rtc->cr.sub1h and rtc->cr.add1h */
		assert(clock_time.pm == 0);
		z = clock_time.hu + clock_time.ht * 10;
		z += it;
		if (z > 23)
			z = 0;
		else if (z < 0)
			z = 23;
		clock_time.hu = z % 10;
		clock_time.ht = z / 10;
		break;
	case FOCUS_MINUTE:
		z = clock_time.mnu + clock_time.mnt * 10;
		z += it;
		if (z > 59)
			z = 0;
		else if (z < 0)
			z = 59;
		clock_time.mnu = z % 10;
		clock_time.mnt = z / 10;
		break;
	case FOCUS_SECOND: /* there's also rtc->shiftr.add1s */
		z = clock_time.su + clock_time.st * 10;
		z += it;
		if (z > 59)
			z = 0;
		if (z < 0)
			z = 59;
		clock_time.su = z % 10;
		clock_time.st = z / 10;
		break;
	}
	clock_set(clock_date, clock_time);
}

struct app app_conf = {
	.select = sel_conf,
	.remove = rm,
	.event = ev_conf,
	.previous = NULL,
};

