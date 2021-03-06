#include "conf.h"
#include "lcd-com.h"
#include "app.h"
#include <stddef.h>
#include "rcc-c.h"
#include "adc-c.h"
#include "decimal.h"

static void draw(int all)
{
	// TODO: unfinished app, unfinished function
	lcd_setcaret(2, 0);
	lcd_puts("adc infos:");

	// CPU temp
	lcd_setcaret(2, 1);
	lcd_puts("cpu t: ");

	lcd_setcaret(LCD_WIDTH - 3 - 4 * 5, 1);

	int t = get_temp();
	print_decimal(lcd_putc, t, 2);
	lcd_puts("\tc");

	// VDDA
	lcd_setcaret(2, 2);
	lcd_puts("vdda:");

	int v = get_vdda();
	int xn = decimal_length(v);
	lcd_setcaret(LCD_WIDTH - 3 - (xn + 2) * 5, 2);
	print_decimal_fixpt(lcd_putc, v, xn, 3, 1000);
	lcd_putc('v');

#ifdef CONF_L4
	// HVPWR display
	lcd_setcaret(2, 3);
	lcd_puts("hvpwr:");

	int hvpwr_vdd = get_hvpwr_vdd();
	xn = decimal_length(hvpwr_vdd);
	lcd_setcaret(LCD_WIDTH - 3 - (xn + 2) * 5, 3);
	print_decimal_fixpt(lcd_putc, hvpwr_vdd, xn, 3, 1000);
	lcd_putc('v');
#endif
}

static void sel()
{
	lcd_clear();
	draw(1);
}

static void ev(enum ev_type type, enum ev_key key)
{
	if (type != EV_TYPE_RELEASE)
		return;
	if (key == EV_KEY_LEFT) {
		app_pop();
		return;
	} else if (key == EV_KEY_RIGHT) {
		draw(1);
	}
}

struct app app_voltm = {
	.select = sel,
	.remove = NULL,
	.event = ev,
	.previous = NULL,
};

