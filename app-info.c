
#include "lcd-com.h"
#include "app.h"
#include "clock.h"
#include <stddef.h> /* NULL */

#define row (2)

static void upd()
{
	lcd_setcaret(0, row);
	struct rtc_tr time;
	struct rtc_dr date;
	clock_get(&time, &date);
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

static void sel_conf()
{
	sel();
	lcd_setcaret(2, 4);
	lcd_puts("back");
	refocus(FOCUS_SECOND);
}

static void ev_conf(enum ev_type type, enum ev_key key)
{
	if (type != EV_TYPE_RELEASE)
		return;
	if (focus == FOCUS_BACK && (key == EV_KEY_UP || key == EV_KEY_DOWN))
		app_pop();

	if (key == EV_KEY_RIGHT) {
		int oldf = focus;
		focus++;
		if (focus == FOCUS_MAX)
			focus = 0;
		refocus(oldf);
	} else if (key == EV_KEY_LEFT) {
		int oldf = focus;
		focus--;
		if (focus < 0)
			focus = FOCUS_MAX - 1;
		refocus(oldf);
	}
}

struct app app_conf = {
	.select = sel_conf,
	.remove = rm,
	.event = ev_conf,
	.previous = NULL,
};

