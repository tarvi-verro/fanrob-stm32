#include "lcd-com.h"
#include <stdbool.h>

extern void assert(bool);

int decimal_length(int i)
{
	assert(i >= 0 && i < 100*1000);

	return 1 + (i >= 10) + (i >= 100) + (i >= 1000) + (i >= 10*1000);
}

static void prntdec5(unsigned int d, int l)
{
	switch (l) {
	default:
	case 5:
		lcd_putc('0' + (d / 10000) % 10);
	case 4:
		lcd_putc('0' + (d / 1000) % 10);
	case 3:
		lcd_putc('0' + (d / 100) % 10);
	case 2:
		lcd_putc('0' + (d / 10) % 10);
	case 1:
		lcd_putc('0' + (d % 10));
	}
}

void print_decimal(unsigned int d, int len)
{
	assert(len < sizeof("4294967295") && len >= 1);
	if (len > 5)
		prntdec5(d / 100000, len - 5);
	prntdec5(d, len);
}

void print_decimal_fixpt(unsigned int d, int len, int exp, int base_to_exp)
{
	int i;
	if (len < exp) {
		lcd_putc('.');
		for (i = exp - len; i > 0; i--)
			lcd_putc('0');
		print_decimal(d, len);
		return;
	}
	if (len > exp)
		print_decimal(d / base_to_exp, len - exp);
	lcd_putc('.');
	print_decimal(d, exp);
}

