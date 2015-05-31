
#include <stdio.h>
#include <assert.h>
#include "font-tiny.h"
#include <stdlib.h>

int main(int argc, const char *argv)
{
	assert(height == 8);
	assert((width % 5) == 0); /* hardcoded at 5 */

	int l = width / 5;

	unsigned char *b = calloc(1, width);

	char *d = header_data;
	unsigned char px[4];
	int i;
	for (i = 0; i < width * height; i++) {
		HEADER_PIXEL(d, px);
		b[i % width] |= ((px[0] < 127)) << (i / width);
	}

	if (argc == 1) {
		for (i = 0; i < width; i++)
			putc(b[i], stdout);

		//putc('\0', stdout);
		return 0;
	}

	int j, k;
	for (i = 0; i < l; i++) {
		for (j = 0; j < 8; j++) {
			for (k = 0; k < 5; k++) {
				putc((b[i * 5 + k] & (1 << j)) ? ' ' : '#',
						stdout);
			}
			putc('\n', stdout);
		}
		printf("------\n");
	}

	return 0;
}
