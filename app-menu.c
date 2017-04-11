#include "app.h"
#include "lcd-com.h"
#include <stddef.h>

static int focus = 1;
enum {
	ITEM_INFO,
	ITEM_CONF,
	ITEM_EXPO,
	ITEM_HEAT,
	ITEM_VOLTM,
	ITEM_SNAKE,
	ITEM_MAX,
};
static const char items[][16 - 2] = {
	"just info",
	"configure sys",
	"single expo",
	"heater",
	"voltmeter",
	"snake",
};
#define items_length (sizeof(items)/sizeof(items[0]))
_Static_assert(items_length <= 6,
		"too many items in menu, make menu scroll or something");
_Static_assert(items_length == ITEM_MAX,
		"Update the ITEM_X enum aswell as the items list.");

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
		else if (focus == ITEM_CONF)
			app_push(&app_conf);
#if defined (CONF_F0) || defined (CONF_L4)
		else if (focus == ITEM_HEAT)
			app_push(&app_heater);
		else if (focus == ITEM_EXPO)
			app_push(&app_exposure);
#else
#warning app-menu needs a dynamic menu thing
#endif
		else if (focus == ITEM_VOLTM)
			app_push(&app_voltm);
		/* to next app */
	}
}

struct app app_menu = {
	.select = sel,
	.remove = NULL,
	.event = ev,
	.previous = NULL,
};

