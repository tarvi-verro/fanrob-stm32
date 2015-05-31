#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

extern uint8_t font_bin[];
extern void assert(bool);

const uint8_t *glyph_5x8_lookup(const char c)
{
	if (c >= 'a' && c <= 'z')
		return font_bin + (c - 'a') * 5;
	if (c >= ' ' && c <= '@')
		return font_bin + 130 + (c - ' ') * 5;
	assert(false);
	return NULL;
}
