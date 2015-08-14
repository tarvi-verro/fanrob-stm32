#include "app.h"
#include "lcd-com.h"
#include <stdbool.h>
#include <stddef.h>
#include "clock.h"
#include "camsig.h"

extern void assert(bool);

static int expo_active = 0;
static int expo_active_t;
static int expo_sec = 3;
static int expo_delay = 2;
static int expo_n = 1;
static int expo_ended = 0; /* written to 1 when expo over, upd screen */
static int expo_back = 0;
//static int expo_fract = 120; /* TODO: subseconds */
/* TODO: start exposure */

enum conf_id {
	CONF_TITLE,
	CONF_TIME,
	CONF_DELAY,
	CONF_N,
	CONF_START,
	CONF_BACK,
	CONF_MAX,
};
_Static_assert(CONF_MAX <= LCD_HEIGHT/8,
		"Too many lines in exposure conf screen.");

static void upd_conf(enum conf_id cnf, bool full);
static int focus = CONF_TIME;

static struct rtc_alrmar alrm_a[2] = { 0 };

static struct rtc_alrmar alarm_smh_from_sec(int sec)
{
	struct rtc_alrmar a = { 0 };
	a.su = (sec % 60) % 10;
	a.st = (sec % 60) / 10;
	a.mnu = ((sec / 60) % 60) % 10;
	a.mnt = ((sec / 60) % 60) / 10;
	a.hu = ((sec / (60*60) % 24) % 10);
	a.ht = ((sec/ (60*60) % 24) / 10);
	a.pm = 0;
	a.msk4 = 1; /* don't care about date */
	return a;
}

static void exposure_callback();

static void exposure_program(int current_sec)
{
	uint32_t *n = (uint32_t *) alrm_a;
	n[0] = 0;
	n[1] = 0;
	int start = current_sec + expo_delay;
	int end = start + expo_sec;

	alrm_a[0] = alarm_smh_from_sec(start);
	alrm_a[1] = alarm_smh_from_sec(end);
	clock_alarm(alrm_a[0], exposure_callback);
	expo_active++;
}

static void exposure_callback()
{
	if (expo_active & 1) {
		camsig_set(1);
		expo_active++;
		clock_alarm(alrm_a[1], exposure_callback);
		upd_conf(CONF_START, false);
		return;
	}
	camsig_set(0);
	if ((expo_active / 2) >= expo_n) {
		expo_active = 0;
		expo_ended = 1;
		clock_alarm_stop(exposure_callback);
		return;
	}
	expo_active_t += expo_delay + expo_sec;
	exposure_program(expo_active_t);
	upd_conf(CONF_START, false);
}

static void exposure_start()
{
	struct rtc_tr time;
	struct rtc_dr date;
	clock_get(&time, &date);
	int sec = time.su + time.st * 10
		+ (time.mnu + time.mnt * 10) * 60
		+ (time.hu + time.ht * 10) * 60 * 60;

	expo_active_t = sec;
	expo_active = 0;
	exposure_program(sec);
}

static int decimal_length(int i)
{
	assert(i >= 0 && i < 100*1000);

	return 1 + (i >= 10) + (i >= 100) + (i >= 1000) + (i >= 10*1000);
}

static void print_decimal(int d, int len)
{
	assert(len <= 5);
	if (len >= 5)
		lcd_putc('0' + (d / 10000) % 10);
	if (len >= 4)
		lcd_putc('0' + (d / 1000) % 10);
	if (len >= 3)
		lcd_putc('0' + (d / 100) % 10);
	if (len >= 2)
		lcd_putc('0' + (d / 10) % 10);
	lcd_putc('0' + (d % 10));
}

static void refocus(int oldfocus)
{
	uint8_t non[2] = { 0 };
	lcd_setcaret(0, oldfocus);
	lcd_send(true, non, 2);
	lcd_setcaret(LCD_WIDTH - 2, oldfocus);
	lcd_send(true, non, 2);

	uint8_t sel_a[] = { 0x3f, 0x01 };
	uint8_t sel_b[] = { 0x80, 0xfc };
	lcd_setcaret(0, focus);
	lcd_send(true, sel_a, 2);
	lcd_setcaret(LCD_WIDTH - 2, focus);
	lcd_send(true, sel_b, 2);

}

static void conf_offset(enum conf_id cfg, int off);

static int velocity = 0;
static enum ev_key fastdirection = 0;
static void app_upd()
{
	if (expo_ended) {
		expo_ended = 0;
		upd_conf(CONF_START, true);
		upd_conf(CONF_TITLE, true);
		upd_conf(CONF_BACK, true);
	}
	if (!fastdirection)
		return;
	velocity++;
	if (velocity <= 7)
		return;
	if (fastdirection == EV_KEY_RIGHT)
		conf_offset(focus, (velocity - 7));
	else
		conf_offset(focus, -(velocity - 7));
}


static void upd_conf(enum conf_id cnf, bool full)
{
	int xn, n;
	switch (cnf) {
	case CONF_TIME: // format: 30'20"
		lcd_setcaret(LCD_WIDTH - 3 - (6 * 5), CONF_TIME);
		lcd_putc('0' + (expo_sec / (60 * 10)));
		lcd_putc('0' + ((expo_sec / (60)) % 10));
		lcd_putc('\'');
		lcd_putc('0' + ((expo_sec % 60) / 10));
		lcd_putc('0' + ((expo_sec % 60) % 10));
		lcd_putc('"');
		break;
	case CONF_DELAY:
		xn = decimal_length(expo_delay); /* prepend ' ' */
		lcd_setcaret(LCD_WIDTH - 3 - ((xn + 1) * 5), CONF_DELAY);
		lcd_putc(' ');
		print_decimal(expo_delay, xn);
		break;
	case CONF_N:
		xn = decimal_length(expo_n);
		lcd_setcaret(LCD_WIDTH - 3 - ((xn + 1) * 5), CONF_N);
		lcd_putc(' ');
		print_decimal(expo_n, xn);
		break;
	case CONF_START:
		if (full) {
			lcd_setcaret(2, CONF_START);
			lcd_repeat(0x0, LCD_WIDTH - 4);
		}
		if (!expo_active) {
			lcd_setcaret(LCD_WIDTH - 3 - 7*5, CONF_START);
			lcd_puts("start >");
		} else {
			lcd_setcaret(20, CONF_START);
			char c = expo_active & 1 ? ' ' : '#';
			lcd_putc(c);
			lcd_putc(c);
			lcd_putc(c);

			n = (expo_active - 1) / 2;
			xn = decimal_length(n);
			lcd_setcaret(LCD_WIDTH - 3 - ((xn + 1) * 5),
					CONF_START);
			lcd_putc(' ');
			print_decimal(n, xn);
		}
		break;
	case CONF_TITLE:
		lcd_setcaret(0, CONF_TITLE);
		if (!expo_active)
			lcd_puts("take an exposure");
		else
			lcd_puts("exposures live..");
		break;
	case CONF_BACK:
		if (full) {
			lcd_setcaret(2, CONF_BACK);
			lcd_repeat(0x0, LCD_WIDTH - 4);
		}
		if (!expo_active) {
			lcd_setcaret(2, CONF_BACK);
			lcd_puts("< back");
		} else {
			lcd_setcaret(LCD_WIDTH - 3 - 7*5, CONF_BACK);
			if (expo_back == 0)
				lcd_puts(" < stop");
			else
				lcd_puts("<< stop");
		}
		break;
	default:
		assert(false);
	};
}

static void drw_conf()
{
	upd_conf(CONF_TITLE, false);

	lcd_setcaret(2, CONF_TIME);
	lcd_puts("t:");
	upd_conf(CONF_TIME, false);

	lcd_setcaret(2, CONF_DELAY);
	lcd_puts("delay:");
	upd_conf(CONF_DELAY, false);

	lcd_setcaret(2, CONF_N);
	lcd_puts("n:");
	upd_conf(CONF_N, false);

	upd_conf(CONF_START, false);

	upd_conf(CONF_BACK, false);
}

static void sel()
{
	lcd_clear();
	drw_conf();
	refocus(CONF_TIME); /* random conf */
	app_update = app_upd;
}

static void rm()
{
	if (expo_active) {
		expo_active = 0;
		clock_alarm_stop(exposure_callback);
		camsig_set(0);
	}
	app_update = NULL;
}

static void conf_offset(enum conf_id cfg, int off)
{
	switch (cfg) {
	case CONF_TIME:
		if (expo_sec == 1 && off < 0)
			break;
		expo_sec += off;
		if (expo_sec < 0)
			expo_sec = 1;
		if (expo_sec >= (99 * 60 + 59))
			expo_sec = (99 * 60 + 59);
		upd_conf(focus, false);
		break;
	case CONF_DELAY:
		if (expo_delay == 1 && off < 0)
			break;
		expo_delay += off;
		if (expo_delay < 1)
			expo_delay = 1;
		upd_conf(focus, false);
		break;
	case CONF_N:
		if (expo_n == 1 && off < 0)
			break;
		expo_n += off;
		if (!expo_active && expo_n < 1)
			expo_n = 1;
		else if (expo_active && expo_n < (expo_active - 1) / 2 + 1)
			expo_n = (expo_active - 1) / 2 + 1;
		upd_conf(focus, false);
		break;
	default:
		assert(false);
	}
}

static void ev(enum ev_type type, enum ev_key key)
{
	if (type == EV_TYPE_PRESS) {
		if (key == EV_KEY_LEFT || key == EV_KEY_RIGHT) {
			fastdirection = key;
			velocity = 0;
		}
		return;
	}
	fastdirection = 0;

	if (key == EV_KEY_UP) {
		int oldfocus = focus;
		focus--;
		if (!expo_active && focus < CONF_TIME)
			focus = CONF_BACK;
		else if (expo_active && focus < CONF_N)
			focus = CONF_BACK;
		refocus(oldfocus);
		if (expo_back) {
			expo_back = 0;
		       upd_conf(CONF_BACK, false);
		}
	} else if (key == EV_KEY_DOWN) {
		int oldfocus = focus;
		focus++;
		if (focus > CONF_BACK)
			focus = expo_active ? CONF_N : CONF_TIME;
		refocus(oldfocus);
		if (expo_back) {
		       expo_back = 0;
		       upd_conf(CONF_BACK, false);
		}
	}

	switch (focus) {
	case CONF_BACK:
		if (key != EV_KEY_LEFT)
			break;
		if (!expo_active) {
			app_pop();
			break;
		} else if (expo_back == 0) {
			expo_back++;
			upd_conf(CONF_BACK, false);
			break;
		}
		/* end active expo */
		expo_active = 0;
		clock_alarm_stop(exposure_callback);
		camsig_set(0);
		upd_conf(CONF_START, true);
		upd_conf(CONF_TITLE, true);
		upd_conf(CONF_BACK, true);
		break;
	case CONF_START:
		if (expo_active || key != EV_KEY_RIGHT)
			break;
		exposure_start();
		upd_conf(CONF_START, true);
		upd_conf(CONF_TITLE, true);
		upd_conf(CONF_BACK, true);
		break;
	default:;
		if (key != EV_KEY_LEFT && key != EV_KEY_RIGHT)
			break;
		int off = (key == EV_KEY_LEFT ? (-1) : (1));
		conf_offset(focus, off);
		break;
	}
}

struct app app_exposure = {
	.select = sel,
	.remove = rm,
	.event = ev,
	.previous = NULL,
};

