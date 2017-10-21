#include "app.h"
#include <stddef.h>
#include <stdbool.h>
#include "decimal.h"
#include "lcd-com.h"
#include "heater.h"

extern void assert(bool);

static int heat_resistance = 168; // deci-Ω
static const int heat_resistance_selection[] = { 168, 35  }; // deci-Ω
static const int heat_resistance_selection_length = sizeof(heat_resistance_selection)/sizeof(heat_resistance_selection[0]);

static int heat_voltage = 124; // deci-V
static const int heat_voltage_selection[] = { 74, 124, 124*2 }; // deci-V
static const int heat_voltage_selection_length = sizeof(heat_voltage_selection)/sizeof(heat_voltage_selection[0]);

static int heat_watt = 10;
static bool heat_online = false;

static void refocus(int oldfocus);

enum conf_id {
	CONF_TITLE,
	CONF_VOLTAGE,
	CONF_RESISTANCE,
	CONF_WATT,
	CONF_STATE,
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
		print_decimal_fixpt(lcd_putc, heat_voltage, xn, 1, 10);
		break;
	case CONF_RESISTANCE:
		xn = decimal_length(heat_resistance);
		lcd_setcaret(LCD_WIDTH - 3 - ((xn + 2) * 5), CONF_RESISTANCE);
		lcd_putc(' ');
		print_decimal_fixpt(lcd_putc, heat_resistance, xn, 1, 10);
		break;
	case CONF_WATT:
		xn = decimal_length(heat_watt);
		lcd_setcaret(LCD_WIDTH - 3 - ((xn + 2) * 5), CONF_WATT);
		lcd_putc(' ');
		print_decimal_fixpt(lcd_putc, heat_watt, xn, 1, 10);
		break;
	case CONF_STATE:
		if (heat_online) {
			lcd_setcaret(LCD_WIDTH - 3 - 3 * 5, CONF_STATE);
			lcd_puts(" on");
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

	lcd_setcaret(2, CONF_RESISTANCE);
	lcd_puts("resistance");
	upd_conf(CONF_RESISTANCE);

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

static bool heat_upd()
{
	if (!heat_online) {
		heater_set(0);
		return true;
	}

	int w_full_cycle_centi = heat_voltage * heat_voltage * 10 / heat_resistance;
	int w_target_centi = heat_watt * 10;
	if (w_target_centi > w_full_cycle_centi)
		return false;
	int freq = w_target_centi * 100 / w_full_cycle_centi;
	heater_set((uint8_t) (((float) freq) / 100.f * 255.f));
	return true;
}

static void app_event(enum ev_type typ, enum ev_key key)
{
	if (typ != EV_TYPE_RELEASE)
		return;


	if (key == EV_KEY_UP) {
		int oldfocus = focus;
		focus--;
		if (heat_online && focus < CONF_STATE)
			focus = CONF_BACK;
		else if (focus < CONF_VOLTAGE)
			focus = CONF_BACK;
		refocus(oldfocus);
		return;
	} else if (key == EV_KEY_DOWN) {
		int oldfocus = focus;
		focus++;
		if (focus > CONF_BACK)
			focus = heat_online ? CONF_STATE : CONF_VOLTAGE;
		refocus(oldfocus);
		return;
	}


	int val;
	int i;
	switch (focus) {
	case CONF_RESISTANCE:
		for (i = 0; i < heat_resistance_selection_length; i++)
			if (heat_resistance_selection[i] == heat_resistance) break;
		val = key == EV_KEY_LEFT ? i-1 : i+1;
		if (val >= 0)
			val = val % heat_resistance_selection_length;
		else
			val = heat_resistance_selection_length - 1;
		heat_resistance = heat_resistance_selection[val];
		upd_conf(CONF_RESISTANCE);
		break;
	case CONF_VOLTAGE:
		for (i = 0; i < heat_voltage_selection_length; i++)
			if (heat_voltage_selection[i] == heat_voltage) break;
		val = key == EV_KEY_LEFT ? i-1 : i+1;
		if (val >= 0)
			val = val % heat_voltage_selection_length;
		else
			val = heat_voltage_selection_length - 1;
		heat_voltage = heat_voltage_selection[val];
		upd_conf(CONF_VOLTAGE);
		break;
	case CONF_WATT:
		val = key == EV_KEY_LEFT ? -1 : 1;
		heat_watt += val;

		int w_full_cycle_centi = heat_voltage * heat_voltage * 10 / heat_resistance;
		if (heat_watt * 10 > w_full_cycle_centi)
			heat_watt = 1;
		else if (heat_watt < 1)
			heat_watt = w_full_cycle_centi/10;
		upd_conf(CONF_WATT);
		break;
	case CONF_STATE:
		val = key == EV_KEY_RIGHT;
		if (val == heat_online)
			break;
		heat_online = val;
		if (!heat_upd()) {
			heat_online = !val;
			break;
		}
		upd_conf(CONF_STATE);
		break;
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

