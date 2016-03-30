#include "conf.h"
#include "lcd-com.h"
#include "app.h"
#include <stddef.h>
#include "rcc.h"
#include "adc.h"
#include "decimal.h"

static void draw(int all)
{
	// TODO: unfinished app, unfinished function
	lcd_setcaret(2, 0);
	lcd_puts("adc infos:");
	lcd_setcaret(2, 1);
	lcd_puts("cpu t: ");

	lcd_setcaret(LCD_WIDTH - 3 - 4 * 5, 1);

	int t = get_temp();
	print_decimal(t, 2);

	lcd_puts("\tc");

}

static void sel()
{
	lcd_clear();
	draw(1);
}

static void ev(enum ev_type type, enum ev_key key)
{
	if (key == EV_KEY_LEFT && type == EV_TYPE_RELEASE) {
		app_pop();
		return;
	}
}

struct app app_voltm = {
	.select = sel,
	.remove = NULL,
	.event = ev,
	.previous = NULL,
};

