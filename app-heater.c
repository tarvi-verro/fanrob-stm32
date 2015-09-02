#include "app.h"
#include <stddef.h>
#include <stdbool.h>
#include "decimal.h"
#include "lcd-com.h"

extern void assert(bool);

static int heat_voltage = 74;
static int heat_watt = 10;
static bool heat_online = false;

static void refocus(int oldfocus);

enum conf_id {
	CONF_TITLE,
	CONF_VOLTAGE,
	CONF_WATT,
	CONF_STATE,
	CONF_USED,
	CONF_BACK,
	CONF_MAX,
};
_Static_assert(CONF_MAX <= LCD_HEIGHT/8,
		"Too many lines in heater conf screen.");

static int focus = CONF_BACK;

static void upd_conf(enum conf_id cnf)
{
	int xn;
	lcd_setcaret(2, cnf);
	switch (cnf) {
	case CONF_VOLTAGE:
		xn = decimal_length(heat_voltage);
		lcd_setcaret(LCD_WIDTH - 3 - ((xn + 2) * 5), CONF_VOLTAGE);
		lcd_putc(' ');
		print_decimal_fixpt(heat_voltage, xn, 1, 10);
		break;
	case CONF_WATT:
		xn = decimal_length(heat_watt);
		lcd_setcaret(LCD_WIDTH - 3 - ((xn + 2) * 5), CONF_WATT);
		lcd_putc(' ');
		print_decimal_fixpt(heat_watt, xn, 1, 10);
		break;
	case CONF_USED:
		break;
	case CONF_STATE:
		if (heat_online) {
			lcd_setcaret(LCD_WIDTH - 3 - 2 * 5, CONF_STATE);
			lcd_puts("on");
		} else {
			lcd_setcaret(LCD_WIDTH - 3 - 3 * 5, CONF_STATE);
			lcd_puts("off");
		}
		break;
	default:
		assert(false);
	}
}

static void app_select()
{
	lcd_clear();
	refocus(CONF_BACK);

	lcd_setcaret(2, CONF_TITLE);
	lcd_puts("heater settings");

	lcd_setcaret(2, CONF_VOLTAGE);
	lcd_puts("voltage");
	upd_conf(CONF_VOLTAGE);

	lcd_setcaret(2, CONF_WATT);
	lcd_puts("wattage");
	upd_conf(CONF_WATT);

	lcd_setcaret(2, CONF_STATE);
	lcd_puts("state");
	upd_conf(CONF_STATE);

	lcd_setcaret(2, CONF_BACK);
	lcd_puts("< back");
}

static void app_remove()
{
}

static void refocus(int oldfocus)
{ // TODO: duplicated code between app-exposure and this
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

static void app_event(enum ev_type typ, enum ev_key key)
{
	if (typ != EV_TYPE_RELEASE)
		return;


	if (key == EV_KEY_UP) {
		int oldfocus = focus;
		focus--;
		if (focus < CONF_VOLTAGE)
			focus = CONF_BACK;
		refocus(oldfocus);
		return;
	} else if (key == EV_KEY_DOWN) {
		int oldfocus = focus;
		focus++;
		if (focus > CONF_BACK)
			focus = CONF_VOLTAGE;
		refocus(oldfocus);
		return;
	}

	switch (focus) {
	case CONF_BACK:
		if (key != EV_KEY_LEFT)
			break;
		app_pop();
		break;
	}

}

struct app app_heater = {
	.select = app_select,
	.remove = app_remove,
	.event = app_event,
	.previous = NULL,
};

