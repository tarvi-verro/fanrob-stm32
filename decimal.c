#include "lcd-com.h"
#include <stdbool.h>
#include <limits.h>

extern void assert(bool);

int decimal_length(unsigned i)
{
	if (i >= 1000000000)
		return 10;
	if (i >=  100000000)
		return 9;
	if (i >=   10000000)
		return 8;
	if (i >=    1000000)
		return 7;
	if (i >=     100000)
		return 6;
	if (i >=      10000)
		return 5;
	if (i >=       1000)
		return 4;
	if (i >=        100)
		return 3;
	if (i >=         10)
		return 2;
	return 1;
	// uint32_t max 4294967296
}

static void prntdec5(void (*putc)(char), unsigned d, int l)
{
	const unsigned exp10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
	for (int i = l; i >= 1; i--) {
		putc('0' + (d / exp10[i-1]) % 10);
	}
}

void print_decimal(void (*putc)(char), unsigned d, int len)
{
	assert(len < sizeof("4294967295") && len >= 1);
	if (len > 5)
		prntdec5(putc, d / 100000, len - 5);
	prntdec5(putc, d, len);
}

void print_decimal_fixpt(void (*putc)(char), unsigned int d, int len, int exp,
		int base_to_exp)
{
	int i;
	if (len < exp) {
		putc('.');
		for (i = exp - len; i > 0; i--)
			putc('0');
		print_decimal(putc, d, len);
		return;
	}
	if (len > exp)
		print_decimal(putc, d / base_to_exp, len - exp);
	putc('.');
	print_decimal(putc, d, exp);
}

unsigned int parseBase10(const char *a, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		if (a[i] > '9' || a[i] < '0') {
			return UINT_MAX;
		}
	}
	unsigned int sum = 0; int ex = 1;
	for (int b = i-1; b >= 0; b--) {
		sum += (a[b] - '0') * ex;
		ex *= 10;
	}
	return sum;

}
