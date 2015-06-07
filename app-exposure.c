#include "app.h"
#include "lcd-com.h"
#include <stdbool.h>
#include <stddef.h>

extern void assert(bool);

static int expo_sec = 342;
static int expo_delay = 2;
static int expo_n = 1;
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

static int focus = CONF_TIME;


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
	if (!fastdirection)
		return;
	velocity++;
	if (velocity <= 10)
		return;
	if (fastdirection == EV_KEY_RIGHT)
		conf_offset(focus, (velocity - 10));
	else
		conf_offset(focus, -(velocity - 10));
}

static void upd_conf(enum conf_id cnf)
{
	int xn;
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
	default:
		assert(false);
	};
}

static void drw_conf()
{
	lcd_setcaret(0, CONF_TITLE);
	lcd_puts("take an exposure");

	lcd_setcaret(2, CONF_TIME);
	lcd_puts("t:");
	upd_conf(CONF_TIME);

	lcd_setcaret(2, CONF_DELAY);
	lcd_puts("delay:");
	upd_conf(CONF_DELAY);

	lcd_setcaret(2, CONF_N);
	lcd_puts("n:");
	upd_conf(CONF_N);

	lcd_setcaret(LCD_WIDTH - 3 - 7*5, CONF_START);
	lcd_puts("start >");

	lcd_setcaret(2, CONF_BACK);
	lcd_puts("< back");
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
		upd_conf(focus);
		break;
	case CONF_DELAY:
		if (expo_delay == 1 && off < 0)
			break;
		expo_delay += off;
		if (expo_delay < 1)
			expo_delay = 1;
		upd_conf(focus);
		break;
	case CONF_N:
		if (expo_n == 1 && off < 0)
			break;
		expo_n += off;
		if (expo_n < 1)
			expo_n = 1;
		upd_conf(focus);
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
		if (focus < CONF_TIME)
			focus = CONF_BACK;
		refocus(oldfocus);
	} else if (key == EV_KEY_DOWN) {
		int oldfocus = focus;
		focus++;
		if (focus > CONF_BACK)
			focus = CONF_TIME;
		refocus(oldfocus);
	}

	switch (focus) {
	case CONF_BACK:
		if (key != EV_KEY_LEFT)
			break;
		app_pop();
		break;
	case CONF_START:
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
