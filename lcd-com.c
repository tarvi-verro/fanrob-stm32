#include "f0-gpio.h"
#include "f0-spi.h"
#include "f0-rcc.h"
#include <stdbool.h>

#include "conf.h"

extern void PCD8544_Init(unsigned char contrast);

extern void assert(bool);

void lcd_send(bool data_mode /* false:command mode */,
		const uint8_t *a, int l);

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

	/* Background lights */
	io_lcd->moder.pin_lcd_bg = GPIO_MODER_OUT;
	io_lcd->ospeedr.pin_lcd_bg = GPIO_OSPEEDR_LOW;
	io_lcd->otyper.pin_lcd_bg = GPIO_OTYPER_PP;
	io_lcd->pupdr.pin_lcd_bg = GPIO_PUPDR_NONE;
	//io_lcd->afr = 1; /* tim3 ch1 */
	io_lcd->bsrr.set.pin_lcd_bg = 1;

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
	spi_lcd->cr1.cpha = 1; /* CPHA, second clock edge for data capture */
	spi_lcd->cr1.cpol = 0; /* CPOL, clock polarity high when idle */
	spi_lcd->cr1.mstr = 1; /* MSTR, master sel */
	spi_lcd->cr1.br = SPI_BR_8; /* BR */
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
	spi_lcd->cr2.txdma_en = 0;
	spi_lcd->cr2.ssoe = 1; /* SSOE */
	spi_lcd->cr2.nssp = 1; /* NSSP, NSS pulse between data */
	spi_lcd->cr2.frf = 0; /* FRF, frame format motorola */
	spi_lcd->cr2.errie = 1; /* ERRIE, error interrupt */
	spi_lcd->cr2.rxneie = 0;
	spi_lcd->cr2.txeie = 0;
	spi_lcd->cr2.ds = SPI_DS_8;
	spi_lcd->cr2.frxth = 0; /* FRXTH, fifo reception thresh. (fxne ev) */


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
	PCD8544_Init(0x36);

		uint8_t cmd[] = {
			32 | 0,
		};
		lcd_send(false, cmd, sizeof(cmd));
}

void lcd_send(bool data_mode /* false:command mode */,
		const uint8_t *a, int l)
{
	assert(!spi_lcd->sr.bsy);
	assert(!spi_lcd->sr.ftlvl);
	assert(spi_lcd->sr.txe);
	assert(!spi_lcd->sr.modf);

	if (data_mode)
		io_lcd->bsrr.set.pin_lcd_dc = 1;
	else
		io_lcd->bsrr.reset.pin_lcd_dc = 1;

	int i;

	//for (i = 0; i < 100; i++);

	//io_lcd->bsrr.reset.pin_lcd_nss = 1;

	for (i = 0; i < l; i++) {
		spi_lcd->dr.dr = a[i];
		while (spi_lcd->sr.bsy);
	}

	while (spi_lcd->sr.bsy);
	//io_lcd->bsrr.set.pin_lcd_nss = 1;
	//if (data_mode)
	//	io_lcd->bsrr.reset.pin_lcd_dc = 1;


}

extern void PCD8544_Putc(char c, int color, int size);
extern void PCD8544_Home(void);
extern void PCD8544_Clear(void);
extern void PCD8544_Refresh(void);
extern void PCD8544_Invert(int invert);
extern void PCD8544_DrawFilledRectangle(unsigned char x0, unsigned char y0,
		unsigned char x1, unsigned char y1, int color);
static unsigned int mytoggle = 1;
void lcd_sendstuff()
{
	mytoggle++;
	if ((mytoggle & 1) == 1) {
#if 1
			uint8_t dat[] = {
				1 | 0 | 4 | 8 | 16 | 32 | 0 | 128,
				1 | 0 | 4 | 8 | 16 | 32 | 0 | 128,
				1 | 0 | 4 | 8 | 16 | 32 | 0 | 128,
				1 | 0 | 4 | 8 | 16 | 32 | 0 | 128,
				1 | 0 | 4 | 8 | 16 | 32 | 0 | 128,
				1 | 0 | 4 | 8 | 16 | 32 | 0 | 128,
				1 | 0 | 4 | 8 | 16 | 32 | 0 | 128,
				1 | 0 | 4 | 8 | 16 | 32 | 0 | 128,
			};
			lcd_send(true, dat, sizeof(dat));
#else
		PCD8544_Clear();
		PCD8544_Home();
		PCD8544_Puts("Hello sweet world!", 1, 0);
		PCD8544_Refresh();
#endif
	} else {
		PCD8544_Invert(!(mytoggle & 1));
	}
}

