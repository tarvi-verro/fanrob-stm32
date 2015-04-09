#include <stdint.h>
#include <stdbool.h>
#include "f0-gpio.h"

/* See DM00091010 - memory registers page 37 */
static uint32_t *const rcc = (uint32_t *) 0x40021000;
static uint32_t *const gpioc = (uint32_t *) 0x48000800;
static uint32_t *const gpioa = (uint32_t *) 0x48000000;
static uint32_t *const tim1 = (uint32_t *) 0x40012C00;
static uint32_t *const exti = (uint32_t *) 0x40010400;
static uint32_t *const syscfg = (uint32_t *) 0x40010000;
static uint32_t *const nvic_iser = (uint32_t *) 0xe000e100;
static uint32_t *const nvic_ispr = (uint32_t *) 0xe000e200;
static uint32_t *const nvic_icpr = (uint32_t *) 0xe000e280;
static uint32_t *const spi1 = (uint32_t *) 0x40013000;

void assert(bool);

void lcd_send(bool data_mode /* false:command mode */,
		const uint8_t *a, int l)
{
	uint32_t t;
	t = spi1[2];
	assert(!(t & (1 << 7))); /* BSY */
	assert(!((t >> 11) & 3)); /* FTLVL */
	assert(!!(t & 2)); /* TXE */
	assert(!(t & (1 << 5))); /* MODF, mode fault */

	const int dc = 3;
	const int nss = 4;

	t = !data_mode ? (1 << dc) : (1 << (dc + 16));
	gpioa[6] |= t; /* set:reset */

	gpioa[6] |= 1 << (nss + 16);
	int i;
	for (i = 0; i < l; i++) {
		while (!(spi1[2] & 2));
		*(uint8_t *)(spi1 + 3) = *(a + i);
		while (!(spi1[2] & 2));
		//while (((spi1[2] >> 11) & 3) == 3);
	}
	while (spi1[2] & (1 << 7)); /* BSY */
	gpioa[6] |= 1 << (nss);
}

void lcd_res()
{
	int i;
	const int vdd = 5; /* PC5 */
	const int res = 2; /* PA2 */
	gpioc[6] |= 1 << (vdd);
	gpioa[6] |= 1 << (res + 16);
	for (i = 0; i < 100*1000; i++);
	gpioa[6] |= 1 << res;
}

void setup_lcd(void)
{
	uint32_t t;
	rcc[6] |= 1 << 12; /* RCC_APB2ENR, SPI1EN */
	rcc[5] |= 1 << 17; /* RCC_AHBENR, IOPA-EN */
	rcc[5] |= 1 << 19; /* RCC_AHBENR, IOPC-en */

	const int mosi = 7;
	const int miso = 6;
	const int sck = 5;
	const int nss = 4;

	const int dc = 3;
	const int res = 2;
	const int vdd = 5; /* PC5 */

	/* Setting up pins. */
	t = gpioa[0];
		t &= ~(3 << (mosi * 2) | 3 << (miso * 2) | 3 << (sck * 2)
				| 3 << (nss * 2));
		t |= 2 << (mosi * 2) | 2 << (miso * 2) | 2 << (sck * 2)
			| 1 << (nss * 2);

		t &= ~(3 << (dc * 2) | 3 << (res * 2));
		t |= 1 << (dc * 2) | 1 << (res * 2);
	gpioa[0] = t;
	//gpioa[6] |= 1 << res; /* active-low, deactivate by default */
	gpioa[6] |= 1 << (res + 16); /* res may be low before vdd */

	t = gpioa[3]; /* Pull-down */
	t &= ~(3 << (sck * 2) | 3 << (nss * 2));
	t |= (2 << (sck * 2)) | 1 << (nss * 2);
	gpioa[3] = t;

	gpioa[6] |= 1 << (nss);

	gpioa[2] |= 3 << (sck * 2); /* high speed */

	t = gpioc[0];
		t &= ~(3 << (vdd * 2));
		t |= 1 << (vdd * 2);
	gpioc[0] = t;
	gpioc[6] |= 1 << (vdd + 16); /* set to low */

	t = gpioa[2]; /* GPIOx_OSPEEDR */
		t &= ~(3 << (dc * 2));
		t |= 3 << (mosi * 2) | 3 << (miso * 2) | 3 << (sck * 2)
			| 3 << (nss * 2);
	gpioa[2] = t;

	/* Setting up SPI */
	t = spi1[0];  /* SPI1_CR1 */

		t &= ~(7 << 3);
		t |= 2 << 3; /* BR, 6 stands for prescaler of 128 */

		t |= 1 << 1; /* CPOL, clock polarity high when idle */

		t |= 1 << 0; /* CPHA, second clock edge for data capture */

		t &= ~(1 << 15); /* BIDIMODE, bidirectional mode */

		t |= 1 << 14; /* BIDIOE, transmit only */

		t &= ~(1 << 13); /* CRC-EN */

		t &= ~(1 << 10); /* RXONLY */

		t &= ~(1 << 7); /* LSBFIRST */

		t |= (1 << 9); /* SSM, software slave mngment */
		t &= ~(1 << 8); /* ISS */

		t |= 1 << 2; /* MSTR, master sel */
	spi1[0] = t;

	t = spi1[1]; /* SPI1_CR2 */
		t &= ~(15 << 8);
		t |= 7 << 8; /* DS, data size, 7 for 8-bit */

		t |= (1 << 2); /* SSOE */

		t &= ~(1 << 4); /* FRF, frame format motorola */

		t &= ~(1 << 3); /* NSSP, NSS pulse between data */

		t &= ~(1 << 12); /* FRXTH, fifo reception thresh. (fxne ev) */
	spi1[1] = t;

	spi1[0] |= 1 << 6; /* SPI1_CR1, SPE */

	lcd_res();
//	t = spi1[4];

}

void setup_usrbtn(void)
{
	uint32_t t;
	rcc[5] |= 1 << 17; /* RCC_AHBENR, IOPA-EN */
	rcc[6] |= 1 << 0; /* RCC_APB2ENR, SYSCFGEN */

	const int n = 0; /* PA0 */

	gpioa[0] &= ~(3 << (n * 2)); /* GPIOx_MODER */

	t = gpioa[3]; /* GPIOx_PUPDR */
	t &= ~(3 << (n * 2));
	t |= 2 << (n * 2);
	gpioa[3] = t;

	const int trig = 0; /* page 174 : PA0 -> EXTI0 */
	exti[0] |= 1 << trig; /* EXTI_IMR */
	//exti[1] |= 1 << trig; /* EXTI_EMR */
	/* Clear rising trigger */
	exti[2] &= ~(1 << trig); /*EXTI_RTSR  */
	/* Set falling trigger */
	exti[3] |= 1 << trig; /* EXTI_FTSR */
	/* Software trigger enable */
	//exti[4] |= 1 << trig; /* EXTI_SWIER */

	exti[5] &= ~(1); /* EXTI_PR */

	t = syscfg[2]; /* SYSCFG_EXTICR1 */
	t &= ~(15 << 0); /* page 146  */
	t |= 0;
	syscfg[2] = t;

	nvic_iser[0] |= 1 << 5;

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

static int somec = 0;

void nvic_exti_0_1(void)
{
	exti[5] |= (1); /* EXTI_PR */
	if (somec)
		return;
	gpioc[5] ^= 1 << 9;
	somec = 2;
	//*nvic_icpr |= 1; /* NVIC_ISPR */

//	uint8_t cmd[] = { 32, 8 | 1, 0 };
	uint8_t cmd[] = {
		1 | 32,
		16 | 128,
       		32,
		4 | 8,

	};
	lcd_send(false, cmd, sizeof(cmd));
	uint8_t dat[] = {
		1 | 2 | 4 | 8 | 16,
		1 | 4,
		1 | 2 | 4,

	};
	lcd_send(true, dat, sizeof(dat));
}

void assert(bool cnd)
{
	if (cnd)
		return;
	unsigned int z = 0;
	unsigned int a;
	while (1) {
		z++;
		a = z & 0xffff;
		if (a != 0x0 && a != 0x8000)
			continue;
		gpioc[5] ^= 1 << 9;
	}
}

int main(void)
{
	setup_leds();
	setup_usrbtn();
	setup_lcd();

	unsigned int z = 1;
	unsigned int a;
	while (1) {
		z++;
		a = z & 0xffff;
		if (a != 0x0 && a != 0x8000)
			continue;
		if (somec)
			somec--;
		if ((gpioc[5] & (1 << 9)))
			gpioc[5] ^= 1 << 8;
	}
}

