#include "gpio-abs.h"
#define CFG_LCD
#include "tim-preset.h"
#include "lcd-com.h"
#include "conf.h"


#include "spi.h"
#include "rcc.h"
#include "dma.h"

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


static void lcd_initsequence();
static void lcd_bglight();

void setup_lcd(void)
{
	volatile struct spi_reg *const spi = cfg_lcd.spi;
	if (spi == spi1)
		rcc->apb2enr.spi1en = 1;
	else
		assert(1==3);

#ifdef CONF_F0
	rcc->cfgr.p_pre = RCC_P_PRE_NONE;
#else
	// TODO
#endif

	/* Setting up pins. */

	/* Data/command pin */
	struct gpio_conf dc = {
		.mode = GPIO_MODER_OUT,
		.ospeed = GPIO_OSPEEDR_HIGH,
		.otype = GPIO_OTYPER_PP,
		.pupd = GPIO_PUPDR_NONE,
	};
	gpio_configure(cfg_lcd.dc, &dc);

	/* Reset pin (active-low), deactivate by default;
	 * res may be low before vdd */
	struct gpio_conf res = {
		.mode = GPIO_MODER_OUT,
		.ospeed = GPIO_OSPEEDR_HIGH,
		.otype = GPIO_OTYPER_PP,
		.pupd = GPIO_PUPDR_NONE,
	};
	gpio_configure(cfg_lcd.res, &res);
	gpio_write(cfg_lcd.res, 0);

	lcd_bglight();

	/* Chip select */
	struct gpio_conf nss0 = {
		.mode = GPIO_MODER_OUT, /* reconf to AF later */
		.ospeed = GPIO_OSPEEDR_HIGH,
		.otype = GPIO_OTYPER_PP,
		.pupd = GPIO_PUPDR_NONE,
		.alt = cfg_lcd.gpio_af_spi,
	};
	gpio_configure(cfg_lcd.nss, &nss0);
	gpio_write(cfg_lcd.nss, 0);

	struct gpio_conf spi_io = {
		.mode = GPIO_MODER_AF,
		.ospeed = GPIO_OSPEEDR_HIGH,
		.otype = GPIO_OTYPER_PP,
		.pupd = GPIO_PUPDR_NONE,
		.alt = cfg_lcd.gpio_af_spi,
	};
	gpio_configure(cfg_lcd.mosi, &spi_io);
	gpio_configure(cfg_lcd.sck, &spi_io);

	/* Vdd 3v0 */
	struct gpio_conf vdd = {
		.mode = GPIO_MODER_OUT,
		.otype = GPIO_OTYPER_PP,
	};
	gpio_configure(cfg_lcd.vdd, &vdd);
	gpio_write(cfg_lcd.vdd, 0); /* set to low */

	/* Setting up SPI */
	struct spi_cr1 cr1 = {
		.cpha = 0, /* CPHA, second clock edge for data capture */
		.cpol = 0, /* CPOL, clock polarity high when idle */
		.mstr = 1, /* MSTR, master sel */
		.br = SPI_BR_8, /* BR */
		.lsbfirst = 0, /* LSBFIRST */
		.ssi = 0, /* SSI */
		.ssm = 0, /* SSM, software slave mngment */
		.rxonly = 0, /* RXONLY */
		//spi_->cr1.crcl = 0,
		//spi_->cr1.crcnext = 0,
		.crcen = 0, /* CRC-EN */
		.bidioe = 1, /* BIDIOE, output enable in bidirectional mode */
		.bidimode = 0, /* BIDIMODE, bidirectional mode */
	};
	spi->cr1 = cr1;


	struct spi_cr2 cr2 = {
		.rxdmaen = 0,
		.txdmaen = 0, // Enable it _after_ configuring DMA channel
		.ssoe = 1, /* SSOE */
		.nssp = 1, /* NSSP, NSS pulse between data */
		.frf = 0, /* FRF, frame format motorola */
		.errie = 1, /* ERRIE, error interrupt */
		.rxneie = 0,
		.txeie = 0,
		.ds = SPI_DS_8,
		.frxth = 0, /* FRXTH, fifo reception thresh. (fxne ev) */
	};
	spi->cr2 = cr2;

	/* Setup DMA, */
	volatile struct dma_reg *const dma = cfg_lcd.dma;
#if defined (CONF_F0)
	rcc->ahbenr.dmaen = 1;
#elif defined (CONF_L4)
	assert(dma == dma1);
	rcc->ahb1enr.dma1en = 1;
#endif
	struct dma_ccr dccr = {
		.pl = DMA_PL_MEDIUM, /* priority medium */
		.msize = DMA_MSIZE_8,
		.psize = DMA_PSIZE_8,
		.dir = DMA_DIR_FROM_MEM,
		.circ = 0, /* circular mode */
		.minc = 1, /* increment memory addr */
		.pinc = 0, /* periperal address does not change */
		.htie = 0, /* half-transfter interrupt */
		.tcie = 0, /* transfer complete interrupt */
		.teie = 0, /* transfer error interrupt */
	};
	dma->ch_lcd.ccr = dccr;
	dma->ch_lcd.cpar = (uint32_t) &spi->dr;
	//dma->ch_lcd.cmar = (uint32_t) 0;
	dma->cselr.ch_lcd.cs = 1; // request number 1

	cr2.txdmaen = 1;
	spi->cr2 = cr2;

	/* Initialisation sequence */
	int i;
	gpio_write(cfg_lcd.vdd, 1);
	gpio_write(cfg_lcd.nss, 0);
	for (i = 0; i < 150 * 1000; i++);
	gpio_write(cfg_lcd.res, 1);
	gpio_write(cfg_lcd.nss, 1);

	for (i = 0; i < 50 * 1000; i++);

	gpio_configure(cfg_lcd.nss, &spi_io);
	cr1.spe = 1;
	spi->cr1 = cr1; /* SPI1_CR1, SPE, spi enable */
	assert(!spi->sr.modf); /* MODF, mode fault */
	lcd_initsequence();
}

static void lcd_initsequence()
{
	uint8_t cmd[] = {
		FNSET | H,
#if 0
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
	lcd_clear();
}

void lcd_repeat(uint8_t x, int n)
{
	assert(n > 0 && n <= (LCD_WIDTH*LCD_HEIGHT/8)); /* sanity check */
	volatile struct dma_reg *const dma = cfg_lcd.dma;
	/* Clear all of lcd with some magic */
	gpio_write(cfg_lcd.dc, 1); /* data */
	dma->ch_lcd.ccr.minc = 0; /* no memory increment */
	dma->ch_lcd.cmar = (uint32_t) &x; /* send this over and over again */
	dma->ch_lcd.cndtr.ndt = n; /* this many times */

	dma->ch_lcd.ccr.en = 1;
	while (dma->ch_lcd.cndtr.ndt);
	dma->ch_lcd.ccr.en = 0;
	dma->ch_lcd.ccr.minc = 1; /* reset correct setting */

	volatile struct spi_reg *const spi = cfg_lcd.spi;
	while (spi->sr.bsy);
}

void lcd_clear()
{
	lcd_repeat(0, (LCD_WIDTH*LCD_HEIGHT/8));
}

void lcd_send(bool data_mode /* false:command mode */,
		const uint8_t *a, int l)
{
	volatile struct spi_reg *spi = cfg_lcd.spi;
	volatile struct dma_reg *const dma = cfg_lcd.dma;
	assert(!spi->sr.bsy);
	assert(!spi->sr.ftlvl);
	assert(spi->sr.txe);
	assert(!spi->sr.modf);
	assert(l > 0);
	assert(!dma->ch_lcd.ccr.en || dma->isr.ch_lcd.tcif);

	gpio_write(cfg_lcd.dc, data_mode);

	int i;

	if (l == 1) {
		for (i = 0; i < l; i++) {
			spi->dr.dr = a[i];
			while (spi->sr.bsy);
		}
	} else {
		dma->ch_lcd.cmar = (uint32_t) a;
		dma->ch_lcd.cndtr.ndt = l;

		dma->ch_lcd.ccr32 |= 1;
		while (dma->ch_lcd.cndtr.ndt);
		dma->ch_lcd.ccr32 &= ~1;

		dma->ifcr.ch_lcd.ctcif = 1;
	}

	while (spi->sr.bsy);
}


static void lcd_bglight()
{
	/* Background lights */
	struct gpio_conf gcfg = {
		.mode = GPIO_MODER_AF,
		.ospeed = GPIO_OSPEEDR_MEDIUM,
		.otype = GPIO_OTYPER_OD,
		.pupd = GPIO_PUPDR_PULLDOWN,
		.alt = cfg_lcd.gpio_af_bg,
	};
	gpio_configure(cfg_lcd.bg, &gcfg);
	//gpio_write(cfg_lcd.bg, 0);

	setup_tim_fast();
	tim_fast_duty_set(cfg_lcd.bg_tim_fast_ch, 250);
}

extern const uint8_t *glyph_5x8_lookup(const char c);

void lcd_putc(char c)
{
	lcd_send(true, glyph_5x8_lookup(c), 5);
}

void lcd_bgset(uint8_t b)
{
	tim_fast_duty_set(cfg_lcd.bg_tim_fast_ch, 255-b);
}

void lcd_setcaret(int x, int y)
{
	assert(x >= 0 && x < LCD_WIDTH);
	assert(y >= 0 && y < (LCD_HEIGHT/8));
	uint8_t pset[] = { SETY | y, SETX | x };
	lcd_send(false, pset, sizeof(pset));
}

void lcd_puts(const char *msg)
{
	for (; *msg != '\0'; msg++) {
		lcd_putc(*msg);
	}
}

