#include "conf.h"
#include "f0-exti.h"
#include "f0-rcc.h"
#include "f0-gpio.h"
#include "f0-syscfg.h"
#include "lcd-com.h" /* bgset */
#include <stdbool.h>
#include "app.h"
extern struct app *app_top;
extern void assert(bool);

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

void kbd_exti_4_15(void)
{
	if (exti->pr.pr10) { /* pin_up */
		exti->pr.pr10 = 1;
		if (io_kbd->idr.pin_up)
			key_states_now |= EV_KEY_UP;
		else
			key_states_now &= ~(EV_KEY_UP);
		key_changes |= EV_KEY_UP;
	}
	if (exti->pr.pr11) { /* pin_down */
		exti->pr.pr11 = 1;
		if (io_kbd->idr.pin_down)
			key_states_now |= EV_KEY_DOWN;
		else
			key_states_now &= ~(EV_KEY_DOWN);
		key_changes |= EV_KEY_DOWN;
	}
	if (exti->pr.pr4) { /* pin_left */
		exti->pr.pr4 = 1;
		if (io_kbd->idr.pin_left)
			key_states_now |= EV_KEY_LEFT;
		else
			key_states_now &= ~(EV_KEY_LEFT);
		key_changes |= EV_KEY_LEFT;
	}
	if (exti->pr.pr5) { /* pin_right */
		exti->pr.pr5 = 1;
		if (io_kbd->idr.pin_right)
			key_states_now |= EV_KEY_RIGHT;
		else
			key_states_now &= ~(EV_KEY_RIGHT);
		key_changes |= EV_KEY_RIGHT;
	}
}

void setup_kbd()
{
	cmb_bgval_upload();
	rcc->ahbenr.iop_kbd_en = 1;
	io_kbd->moder.pin_up = GPIO_MODER_IN;
	io_kbd->moder.pin_down = GPIO_MODER_IN;
	io_kbd->pupdr.pin_up = GPIO_PUPDR_PULLDOWN;
	io_kbd->pupdr.pin_down = GPIO_PUPDR_PULLDOWN;

	io_kbd->moder.pin_left = GPIO_MODER_IN;
	io_kbd->moder.pin_right = GPIO_MODER_IN;
	io_kbd->pupdr.pin_left = GPIO_PUPDR_PULLDOWN;
	io_kbd->pupdr.pin_right = GPIO_PUPDR_PULLDOWN;

	exti->imr.mr10 = exti->imr.mr11 = exti->imr.mr4 = exti->imr.mr5 = 1;

	exti->rtsr.tr10 = exti->rtsr.tr11 = exti->rtsr.tr4 = exti->rtsr.tr5 = 1;

	exti->ftsr.tr10 = exti->ftsr.tr11 = exti->ftsr.tr4 = exti->ftsr.tr5 = 1;

	exti->pr.pr10 = exti->pr.pr11 = exti->pr.pr4 = exti->pr.pr5 = 1;

	syscfg->exticr2.exti4 = SYSCFG_EXTI_PC;
	syscfg->exticr2.exti5 = SYSCFG_EXTI_PC;
	syscfg->exticr3.exti10 = SYSCFG_EXTI_PC;
	syscfg->exticr3.exti11 = SYSCFG_EXTI_PC;

	nvic_iser[0] |= 1 << 7;

}
