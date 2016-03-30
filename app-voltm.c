#include "conf.h"
#include "lcd-com.h"
#include "app.h"
#include <stddef.h>
#include "rcc.h"

static void draw(int all)
{
	// TODO: unfinished app, unfinished function
	lcd_setcaret(2, 0);
	lcd_puts("adc says:");
	lcd_setcaret(2, 1);
	lcd_puts("internal temp: ");
	int i;
	for (i = 0; i < 30; i++) // TODO: this was just for testing
		sleep_busy(1000*1000*1000*1);
	lcd_puts(" ok");

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

