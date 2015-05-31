#include "f0-gpio.h"
#include "f0-spi.h"
#include "f0-rcc.h"
#include "f0-dma.h"
#include "f0-tim.h"
#include <stdbool.h>

#include "conf.h"

#define VOPSET	(1 << 7)
#define BSSET	(1 << 4) /* H=1 */
#define FNSET	(1 << 5)
#define H	(1 << 0) /* extended instruction set */
#define V	(1 << 1) /* use vertical addressing */
#define PD	(1 << 2)
#define DISPCTL	(1 << 3)
#define D	(1 << 2)
#define E	(1 << 0)
#define SETY	(1 << 6)
#define SETX	(1 << 7)
#define WIDTH	84
#define HEIGHT	48

extern void PCD8544_Init(unsigned char contrast);

extern void assert(bool);

void lcd_send(bool data_mode /* false:command mode */,
		const uint8_t *a, int l);

static void lcd_initsequence();
static void lcd_bglight();

void setup_lcd(void)
{
	rcc->apb2enr.spi1_en = 1;
	rcc->ahbenr.iopa_en = 1;
	rcc->ahbenr.iopc_en = 1;

	rcc->cfgr.p_pre = RCC_P_PRE_NONE;

	/* Setting up pins. */
	/* Data/command pin */
	io_lcd->moder.pin_lcd_dc = GPIO_MODER_OUT;
	io_lcd->ospeedr.pin_lcd_dc = GPIO_OSPEEDR_HIGH;
	io_lcd->otyper.pin_lcd_dc = GPIO_OTYPER_PP;
	io_lcd->pupdr.pin_lcd_dc = GPIO_PUPDR_NONE;

	/* Reset pin (active-low), deactivate by default;
	 * res may be low before vdd */
	io_lcd->moder.pin_lcd_res = GPIO_MODER_OUT;
	io_lcd->ospeedr.pin_lcd_res = GPIO_OSPEEDR_HIGH;
	io_lcd->otyper.pin_lcd_res = GPIO_OTYPER_PP;
	io_lcd->bsrr.reset.pin_lcd_res = 1;

	lcd_bglight();

	/* Chip select */
	io_lcd->moder.pin_lcd_nss = GPIO_MODER_OUT; /* reconf to AF later */
	io_lcd->ospeedr.pin_lcd_nss = GPIO_OSPEEDR_HIGH;
	io_lcd->otyper.pin_lcd_nss = GPIO_OTYPER_PP;
	io_lcd->pupdr.pin_lcd_nss = GPIO_PUPDR_NONE;
	io_lcd->afr.pin_lcd_nss = 0;

	/* Mosi & Miso */
	io_lcd->moder.pin_lcd_mosi = GPIO_MODER_AF;
	io_lcd->ospeedr.pin_lcd_mosi = GPIO_OSPEEDR_HIGH;
	io_lcd->otyper.pin_lcd_mosi = GPIO_OTYPER_PP;
	io_lcd->pupdr.pin_lcd_mosi = GPIO_PUPDR_NONE;
	io_lcd->afr.pin_lcd_mosi = 0;

	/* Sck */
	io_lcd->moder.pin_lcd_sck = GPIO_MODER_AF;
	io_lcd->ospeedr.pin_lcd_sck = GPIO_OSPEEDR_HIGH;
	io_lcd->otyper.pin_lcd_sck = GPIO_OTYPER_PP;
	io_lcd->pupdr.pin_lcd_sck = GPIO_PUPDR_NONE;
	io_lcd->afr.pin_lcd_sck = 0;

	/* Vdd 3v0 */
	io_lcd->moder.pin_lcd_vdd = GPIO_MODER_OUT;
	io_lcd->otyper.pin_lcd_vdd = GPIO_OTYPER_PP;
	io_lcd->bsrr.reset.pin_lcd_vdd = 1; /* set to low */

	/* Setting up SPI */
	spi_lcd->cr1.cpha = 0; /* CPHA, second clock edge for data capture */
	spi_lcd->cr1.cpol = 0; /* CPOL, clock polarity high when idle */
	spi_lcd->cr1.mstr = 1; /* MSTR, master sel */
	spi_lcd->cr1.br = SPI_BR_2; /* BR */
	spi_lcd->cr1.lsbfirst = 0; /* LSBFIRST */
	spi_lcd->cr1.ssi = 0; /* SSI */
	spi_lcd->cr1.ssm = 0; /* SSM, software slave mngment */
	spi_lcd->cr1.rxonly = 0; /* RXONLY */
	//spi_lcd->cr1.crcl = 0;
	//spi_lcd->cr1.crcnext = 0;
	spi_lcd->cr1.crc_en = 0; /* CRC-EN */
	spi_lcd->cr1.bidioe = 1; /* BIDIOE, transmit only */
	spi_lcd->cr1.bidimode = 0; /* BIDIMODE, bidirectional mode */


	spi_lcd->cr2.rxdma_en = 0;
	spi_lcd->cr2.txdma_en = 1;
	spi_lcd->cr2.ssoe = 1; /* SSOE */
	spi_lcd->cr2.nssp = 1; /* NSSP, NSS pulse between data */
	spi_lcd->cr2.frf = 0; /* FRF, frame format motorola */
	spi_lcd->cr2.errie = 1; /* ERRIE, error interrupt */
	spi_lcd->cr2.rxneie = 0;
	spi_lcd->cr2.txeie = 0;
	spi_lcd->cr2.ds = SPI_DS_8;
	spi_lcd->cr2.frxth = 0; /* FRXTH, fifo reception thresh. (fxne ev) */

	/* Setup DMA, */
	rcc->ahbenr.dma_en = 1;
	dma->ch_lcd.ccr.pl = 1; /* priority medium */
	dma->ch_lcd.ccr.msize = DMA_MSIZE_8;
	dma->ch_lcd.ccr.psize = DMA_PSIZE_8;
	dma->ch_lcd.ccr.dir = DMA_DIR_FMEM;
	dma->ch_lcd.ccr.circ = 0; /* circular mode */
	dma->ch_lcd.ccr.minc = 1; /* increment memory addr */
	dma->ch_lcd.ccr.pinc = 0; /* periperal address does not change */
	dma->ch_lcd.ccr.htie = 0; /* half-transfter interrupt */
	dma->ch_lcd.ccr.tcie = 0; /* transfer complete interrupt */
	dma->ch_lcd.ccr.teie = 0; /* transfer error interrupt */
	dma->ch_lcd.pa = (uint32_t) &spi_lcd->dr;
	//dma->ch_lcd.ma = (uint32_t) 0;

	/* Initialisation sequence */
	int i;
	io_lcd->bsrr.set.pin_lcd_vdd = 1;
	io_lcd->bsrr.reset.pin_lcd_nss = 1;
	for (i = 0; i < 150 * 1000; i++);
	io_lcd->bsrr.set.pin_lcd_res = 1;
	io_lcd->bsrr.set.pin_lcd_nss = 1;

	for (i = 0; i < 50 * 1000; i++);

	io_lcd->moder.pin_lcd_nss = GPIO_MODER_AF;
	spi_lcd->cr1.spe = 1; /* SPI1_CR1, SPE, spi enable */
	assert(!spi_lcd->sr.modf); /* MODF, mode fault */
	lcd_initsequence();
}

static void lcd_initsequence()
{
	uint8_t cmd[] = {
		FNSET | H,
#if 1
		BSSET | 5, /* 0 -- 7 */
		VOPSET | 0x34, /* 0 -- 0x7f */
#else
		BSSET | 4, /* 0 -- 7 */
		VOPSET | 0x2e, /* 0 -- 0x7f */
#endif

		FNSET,
		DISPCTL, /* display blank */
		DISPCTL | D, /* normal mode */
	};
	lcd_send(false, cmd, sizeof(cmd));
	/* Clear all of lcd with some magic */
	io_lcd->bsrr.set.pin_lcd_dc = 1; /* data */
	dma->ch_lcd.ccr.minc = 0; /* no memory increment */
	uint8_t zero = 0; /* send this over and over again */
	dma->ch_lcd.ma = (uint32_t) &zero;
	dma->ch_lcd.ndt = (WIDTH*HEIGHT/8); /* this many times */

	dma->ch_lcd.ccr.en = 1;
	while (dma->ch_lcd.ndt);
	dma->ch_lcd.ccr.en = 0;
	dma->ch_lcd.ccr.minc = 1; /* reset correct setting */
}

void lcd_send(bool data_mode /* false:command mode */,
		const uint8_t *a, int l)
{
	assert(!spi_lcd->sr.bsy);
	assert(!spi_lcd->sr.ftlvl);
	assert(spi_lcd->sr.txe);
	assert(!spi_lcd->sr.modf);
	assert(l > 0);
	assert(!dma->ch_lcd.ccr.en || dma->isr.ch_lcd.tcif);

	if (data_mode)
		io_lcd->bsrr.set.pin_lcd_dc = 1;
	else
		io_lcd->bsrr.reset.pin_lcd_dc = 1;

	int i;

	if (l == 1) {
		for (i = 0; i < l; i++) {
			spi_lcd->dr.dr = a[i];
			while (spi_lcd->sr.bsy);
		}
	} else {
		dma->ch_lcd.ma = (uint32_t) a;
		dma->ch_lcd.ndt = l;
		dma->ch_lcd.ccr.en = 1;
		while (dma->ch_lcd.ndt);
		dma->ch_lcd.ccr.en = 0;
		dma->ifcr.ch_lcd.ctcif = 1;
	}

	while (spi_lcd->sr.bsy);

}



static void lcd_bglight()
{
	/* Background lights */
	io_lcd->moder.pin_lcd_bg = GPIO_MODER_AF;
	io_lcd->ospeedr.pin_lcd_bg = GPIO_OSPEEDR_MEDIUM;
	io_lcd->otyper.pin_lcd_bg = GPIO_OTYPER_PP;
	io_lcd->pupdr.pin_lcd_bg = GPIO_PUPDR_NONE;
	io_lcd->afr.pin_lcd_bg = 1; /* tim3 ch1 */
	//io_lcd->bsrr.set.pin_lcd_bg = 1;

	rcc->apb1enr.tim3_en = 1;
	/* Setup tim3 */
	tim3->arr = 2550; /* auto-reload aka period */
	tim3->ccr1 = 0; /* duty cycle */
	//tim3->cnt = 500;
	tim3->psc = 8 - 1; /* prescaler */
	tim3->ccmr.ch1.out.ccs = TIM_CCS_OUT; /* output */
	tim3->ccmr.ch1.out.ocm = 6; /* 110 -- pwm mode 1 */
	tim3->ccmr.ch1.out.ocpe = 1; /* preload enable, enables changing ccr1
					on the fly */
//	tim3->cr1.arpe = 1; /* auto-preload enable */
	tim3->cr1.dir = TIM_DIR_UPCNT; /* upcounter */
//	tim3->ccer.cc1p = 0; /* polarity */
//	tim3->ccer.cc1ne = 0;
//	tim3->egr.cc1g = 1; /* capture/compare generation */
	tim3->egr.ug = 1; /* update generation */
	tim3->ccer.cc1e = 1;
	tim3->bdtr.moe = 1; /* main output enable */
	tim3->cr1.cen = 1; /* enable */
}

#include "f0-rtc.h"
extern void clock_get(struct rtc_tr *time, struct rtc_dr *date);
extern const uint8_t *glyph_5x8_lookup(const char c);

void lcd_putc(char c)
{
	lcd_send(true, glyph_5x8_lookup(c), 5);
}

void lcd_bgset(uint8_t b)
{
	tim3->ccr1 = b * 10;
}

void lcd_drawdate()
{
	uint8_t pset[] = { SETY | 0, SETX | 0 };
	lcd_send(false, pset, sizeof(pset));
	struct rtc_tr time;
	struct rtc_dr date;
	clock_get(&time, &date);
	lcd_putc('0' + date.yt);
	lcd_putc('0' + date.yu);
	lcd_putc('-');
	lcd_putc('0' + date.mt);
	lcd_putc('0' + date.mu);
	lcd_putc('-');
	lcd_putc('0' + date.dt);
	lcd_putc('0' + date.du);

	uint8_t u[4] = { 0 };
	lcd_send(true, u, sizeof(u)); /* shorter space */

	lcd_putc('0' + time.ht + (time.pm == RTC_PM_PM));
	lcd_putc('0' + time.hu);
	lcd_putc(':');
	lcd_putc('0' + time.mnt);
	lcd_putc('0' + time.mnu);
	lcd_putc(':');
	lcd_putc('0' + time.st);
	lcd_putc('0' + time.su);
}

