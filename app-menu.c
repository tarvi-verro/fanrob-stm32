#include "app.h"
#include "lcd-com.h"
#include <stddef.h>

static int focus = 1;
enum {
	ITEM_INFO,
	ITEM_CONF,
	ITEM_EXPO,
	ITEM_TLAPSE,
	ITEM_SNAKE,
};
static const char items[][16 - 2] = {
	"just info",
	"configure sys",
	"single expo",
	"timelapse",
	"snake",
};
#define items_length (sizeof(items)/sizeof(items[0]))
_Static_assert(items_length <= 6,
		"too many items in menu, make menu scroll or something");

static void refocus(int oldfocus)
{
	lcd_setcaret(2, oldfocus);
	lcd_putc(' ');
	lcd_setcaret(LCD_WIDTH - 3 - 5, oldfocus);
	lcd_putc(' ');
	lcd_setcaret(2, focus);
	lcd_putc('>');
	lcd_setcaret(LCD_WIDTH - 3 - 5, focus);
	lcd_putc('<');
}

static void drw()
{
	int i, j;
	for (i = 0; i < items_length; i++) {
		if (focus == i) {
			lcd_setcaret(2, i);
			lcd_putc('>');
		} else {
			lcd_setcaret(2 + 5, i);
		}
		for (j = 0; j < sizeof(items[0]); j++) {
			if (items[i][j] == '\0')
				break;
			lcd_putc(items[i][j]);
		}
		if (focus == i) {
			lcd_setcaret(LCD_WIDTH - 3 - 5, i);
			lcd_putc('<');
		}
	}
}

static void sel()
{
	lcd_clear();
	drw();
}

static void ev(enum ev_type type, enum ev_key key)
{
	if (type != EV_TYPE_RELEASE)
		return;
	if (key == EV_KEY_LEFT) {
		app_pop();
	} else if (key == EV_KEY_UP) {
		int oldfocus = focus;
		focus--;
		if (focus < 0)
			focus = items_length - 1;
		refocus(oldfocus);
	} else if (key == EV_KEY_DOWN) {
		int oldfocus = focus;
		focus++;
		if (focus >= items_length)
			focus = 0;
		refocus(oldfocus);
	} else if (key == EV_KEY_RIGHT) {
		if (focus == ITEM_INFO)
			app_pop();
		else if (focus == ITEM_EXPO)
			app_push(&app_exposure);
		else if (focus == ITEM_CONF)
			app_push(&app_conf);
		/* to next app */
	}
}

struct app app_menu = {
	.select = sel,
	.remove = NULL,
	.event = ev,
	.previous = NULL,
};

