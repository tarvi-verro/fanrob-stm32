#pragma once

extern void setup_kbd();
extern void kbd_tick();
extern void kbd_tick_slow();


#ifdef CFG_KBD
struct kbd_configuration {
	enum pin left, right, up, down, press;
};
static const struct kbd_configuration cfg_kbd;
#endif

