#include "gpio-abs.h"
#define CFG_KBD
#include "kbd.h"
#include "conf.h"

#include "exti-abs.h"
#include "assert.h"
#include "rcc.h"
#include "lcd-com.h" /* bgset */
#include <stdbool.h>
#include "app.h"
extern struct app *app_top;

static uint8_t key_states_now = 0;
static uint8_t key_states_last = 0;
static uint8_t key_changes = 0;
static uint8_t key_spampt = 0;
static void kbd_tick_event(enum ev_key key, int now);

static uint8_t cmb_active = 0;
static int cmb_bgval = 100;
#define BGVAL_MUL 4

#define MSK_UP 0x3
#define MSK_DOWN 0xC
#define MSK_LEFT 0x30
#define MSK_RIGHT 0xC0

static void cmb_bgval_upload()
{
	lcd_bgset((uint8_t) (cmb_bgval*cmb_bgval
				*(1.f/(255.f*BGVAL_MUL*BGVAL_MUL))));
}

void kbd_tick_slow()
{
	if (cmb_active) {
		if (cmb_active == 1) {
			cmb_bgval--;
			if (cmb_bgval < 0)
				cmb_bgval = 0;
		} else {
			cmb_bgval++;
			if (cmb_bgval > 255 * BGVAL_MUL)
				cmb_bgval = 255 * BGVAL_MUL;
		}
		cmb_bgval_upload();
	}
	if (!key_spampt)
		return;
	int i;
	for (i = 0; i < 4; i++) {
		int z;
		if (!(z = key_spampt & (0x3 << (i * 2))))
			continue;
		z = z >> (i * 2);
		z--;
		key_spampt &= ~(0x3 << (i * 2));
		if (z != 0) {
			key_spampt |= z << (i * 2);
			continue;
		}
		int now = key_states_now;
		if ((now & (1 << i)) != (key_states_last & (1 << i)))
			kbd_tick_event(1 << i, now);
	}
}

static void kbd_cmb(enum ev_type type, enum ev_key key)
{
	if (type == EV_TYPE_PRESS)
		cmb_active = (key == EV_KEY_LEFT) ? 1 : 2;
	else
		cmb_active = 0;
}

static void kbd_tick_event(enum ev_key key, int now)
{
	bool edge = !(now & key);
	if ((key_states_last & key) == (now & key)) {
//		app_top->event(!edge, key); /* these generate a lot of noise */
//		app_top->event(edge, key);
	} else {
		if ((key == EV_KEY_LEFT || key == EV_KEY_RIGHT)
				&& (now & EV_KEY_DOWN) == EV_KEY_DOWN) {
			kbd_cmb(edge, key);
		} else {
			app_top->event(edge, key);
		}
		key_states_last ^= key;

	}
}

void kbd_tick()
{
	if (!key_changes)
		return;
	int ch = key_changes;
	key_changes = 0;
	int now = key_states_now;
	if ((ch & EV_KEY_UP) && !(key_spampt & MSK_UP)) {
		kbd_tick_event(EV_KEY_UP, now);
		key_spampt |= MSK_UP;
	}
	if ((ch & EV_KEY_DOWN) && !(key_spampt & MSK_DOWN)) {
		kbd_tick_event(EV_KEY_DOWN, now);
		key_spampt |= MSK_DOWN;
	}
	if ((ch & EV_KEY_LEFT) && !(key_spampt & MSK_LEFT)) {
		kbd_tick_event(EV_KEY_LEFT, now);
		key_spampt |= MSK_LEFT;
	}
	if ((ch & EV_KEY_RIGHT) && !(key_spampt & MSK_RIGHT)) {
		kbd_tick_event(EV_KEY_RIGHT, now);
		key_spampt |= MSK_RIGHT;
	}
}

static void ic_kbd(enum pin p);

__attribute__((section(".rodata.exti.gpio.callb"))) void (*const exti_callb)(enum pin) = ic_kbd;

static void ic_kbd(enum pin p)
{
	int v = gpio_read(p);
	if (p == cfg_kbd.up) { /* pin_up */
		if (v)
			key_states_now |= EV_KEY_UP;
		else
			key_states_now &= ~(EV_KEY_UP);
		key_changes |= EV_KEY_UP;
	} else if (p == cfg_kbd.down) { /* pin_down */
		if (v)
			key_states_now |= EV_KEY_DOWN;
		else
			key_states_now &= ~(EV_KEY_DOWN);
		key_changes |= EV_KEY_DOWN;
	} else if (p == cfg_kbd.left) { /* pin_left */
		if (v)
			key_states_now |= EV_KEY_LEFT;
		else
			key_states_now &= ~(EV_KEY_LEFT);
		key_changes |= EV_KEY_LEFT;
	} else if (p == cfg_kbd.right) { /* pin_right */
		if (v)
			key_states_now |= EV_KEY_RIGHT;
		else
			key_states_now &= ~(EV_KEY_RIGHT);
		key_changes |= EV_KEY_RIGHT;
	}
}

void setup_kbd()
{
	cmb_bgval_upload();
	struct gpio_conf bcfg = {
		.mode = GPIO_MODER_IN,
		.pupd = GPIO_PUPDR_PULLDOWN,
	};
	gpio_configure(cfg_kbd.up, &bcfg);
	gpio_configure(cfg_kbd.down, &bcfg);
	gpio_configure(cfg_kbd.left, &bcfg);
	gpio_configure(cfg_kbd.right, &bcfg);

	struct exti_conf ecfg = {
		.im = 1,
		.ft = 1,
		.rt = 1,
	};

	exti_configure_pin(cfg_kbd.up, &ecfg, &exti_callb);
	exti_configure_pin(cfg_kbd.down, &ecfg, &exti_callb);
	exti_configure_pin(cfg_kbd.left, &ecfg, &exti_callb);
	exti_configure_pin(cfg_kbd.right, &ecfg, &exti_callb);
}

