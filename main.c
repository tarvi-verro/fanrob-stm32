#include <stdint.h>

static uint32_t * const rcc = (uint32_t *) 0x40021000;
static uint32_t * const gpioc = (uint32_t *) 0x48000800;
static uint32_t * const gpioa = (uint32_t *) 0x48000000;

void setup_usrbtn(void)
{
	uint32_t t;
	rcc[5] |= 1 << 17; /* RCC_AHBENR, IOPA-EN */

	const int n = 0;

	gpioa[0] &= ~(3 << (n * 2)); /* GPIOx_MODER */

	t = gpioa[3]; /* GPIOx_PUPDR */
	t &= ~(3 << (n * 2));
	t |= 2 << (n * 2);
	gpioa[3] = t;

}

void setup_leds(void)
{
	uint32_t t;
	rcc[5] |= 1 << 19; /* RCC_AHBENR, IOPC-en */

	/* PC8 is the blue led */
	const int n = 8; /* blue */
	const int m = 9; /* green */

	t = gpioc[0]; /* GPIOx_MODER */
	t &= ~(3 << (n * 2) | 3 << (m * 2));
	t |= 1 << (n * 2) | 1 << (m * 2);
	gpioc[0] = t;

	//t = gpioc[1]; /* GPIOx_OTYPER, default push-pullc */
	//t &= ~(1 << n);
	//t |= 1 << n;
	//gpioc[1] = t;

	t = gpioc[2]; /* GPIOx_OSPEEDR */
	t &= ~(3 << (n * 2) | 3 << (m * 2));
	t |= 0 << (n * 2);
	gpioc[2] = t;

	//gpioc[3] = 2 << (n * 2); /* GPIOx_PUPDR */

	t = gpioc[5]; /* GPIOx_ODR */
	t |= 1 << n | 1 << m;
	gpioc[5] = t;

	//gpioc[6] = 1 << n; /* GPIOx_BSRR */

}

int main(void)
{
	setup_leds();
	setup_usrbtn();

	unsigned int z = 1;
	unsigned int a;
	while (1) {
		z++;
		a = z & 0xffff;
		if (a != 0x0 && a != 0x8000)
			continue;
		if ((gpioc[5] & (1 << 9)))
			gpioc[5] ^= 1 << 8;
		if (a != 0x0)
			continue;
		if (!!(gpioa[4] & 1))
			gpioc[5] ^= 1 << 9;
	}
}
