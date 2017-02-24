#include "gpio-abs.h"
#include "exti-abs.h"
#define CFG_ONEWIRE
#include "one-wire.h"
#include "conf.h"
#include "usart.h"
#include "rcc-abs.h"
#include "uart.h"
#include "decimal.h"
#include <limits.h>

static int ow_readbuf_length = 0;
static char ow_readbuf[16] = { 0 };

void ic_usart_ow()
{
	volatile struct usart_reg *usart = cfg_ow.usart;
	assert(usart->isr.rxne);
	exti_pending_clear(cfg_ow.extiln);
	ow_readbuf_length++;
	if (ow_readbuf_length > sizeof(ow_readbuf) - 1)
		return;
	ow_readbuf[ow_readbuf_length - 1] = usart->rdr.rdr;
}

static uint8_t crc(uint8_t reg, unsigned char inpz)
{
	for (int j = 0; j < 8; j++) {
		int inp = !!(inpz & (1 << j));
		inp ^= reg & 1;
		reg = (inp << 7) | ((reg ^ ((inp << 3) | (inp << 4))) >> 1);

	}
	return reg;
}

// returns actual baud
static unsigned baud_set(unsigned baud)
{
	volatile struct usart_reg *usart = cfg_ow.usart;
	assert(!usart->cr1.over8);

	usart->cr1.ue = 0;
	while (usart->cr1.ue);

	unsigned syclk = rcc_get_sysclk();

	unsigned usartdiv = (syclk * 10 / baud + 5) / 10;
	usart->brr.brr = usartdiv;

	usart->cr1.ue = 1;
	while (!usart->cr1.ue);
	return syclk/usartdiv;
}

static char ow_putc(char c)
{
	volatile struct usart_reg *usart = cfg_ow.usart;

	//usart->cr1.te = 1;
	//while (!lpuart->isr.teack);
	while (!usart->isr.txe);
	usart->tdr.tdr = c;
	while (!usart->isr.txe);
	//usart->cr1.te = 0;
	while (!usart->isr.tc);
	return usart->rdr.rdr;
}

static int ow_write_bit(int b);
static int ow_read_bit();
static int ow_write_byte(char by)
{
	for (int i = 0; i <  8; i++) {
		if (!ow_write_bit(by & (1 << i)))
			return 0;
	}
	return 1;
}

static int ow_write_bit(int b)
{
	char wo = b ? 0xff : 0x00;
	char retr = ow_putc(wo);
	return retr == wo;
}

static int ow_read_bit()
{
	return ow_putc(0xff) == 0xff;
}

static int ow_read_byte()
{
	int r = 0;
	for (int i = 0; i < 8; i++) {
		r |= ow_read_bit() << i;
	}
	return r;
}


static char ow_reset_and_presence_detect()
{
	baud_set(9600);
	char z = ow_putc(0xF0);
	baud_set(115200);
	return z;
}

static void ow_cmd_tst();
static uint8_t addr_sen[8] = { 0x4A, 0x03, 0x16, 0x05, 0x89, 0x2C, 0xFF, 0x28 };
//static uint8_t addr_sen[8] = { 0x22, 0x03, 0x16, 0x05, 0x5E, 0xED, 0xFF, 0x28 };

int ow_reset_and_select_device(uint8_t *dev)
{
	int b = ow_reset_and_presence_detect();
	if (b == 0xf0) {
		uart_puts("Nobody present.\r\n");
		return 0;
	}
	if (!ow_write_byte(0x55 )) {
		uart_puts("Somebody is being naughty on the line.\r\n");
		return 0;
	}
	for (int a = 7; a >= 0; a--) {
		for (int i = 0; i < 8; i++) {
			if (!ow_write_bit(dev[a] & (1 << i))) {
				uart_puts("Line not clear.\r\n");
				return 0;
			}

		}
	}

	return 1;
}

int ow_measure_temp(const uint8_t dev[8])
{
	if (!ow_reset_and_select_device(addr_sen))
		return INT_MIN;

	if (!ow_write_byte(0x44))
		return INT_MIN;

	for (int i = 0; i < 10000; i++) asm("nop");

	if (!ow_reset_and_select_device(addr_sen))
		return INT_MIN;
	if (!ow_write_byte(0xbe))
		return INT_MIN;

	unsigned char buf[9];
	for (int i = 0; i < 9; i++) {
		buf[i] = ow_read_byte();
		//uart_puts_hex(buf[i], 2);
		//uart_putc(' ');
	}

	// Calculate CRC
	int reg = 0;
	for (int i = 0; i < 8; i++) {
		reg = crc(reg, buf[i]);
	}
	if (reg != buf[8])
		return INT_MIN;

	//ow_print(* (uint16_t *) buf);

	struct decomp {
		uint16_t m4 : 1, m3 : 1, m2 : 1, m1 : 1, units : 12;
	};
	struct decomp d = *(struct decomp *) buf;
	int nom = 8*d.m1 + 4*d.m2 + 2*d.m3 + 1*d.m4;
	int denom = 16;
	int frac = (nom*10000/denom + 5) /10; // 2344

	return d.units * 1000 + frac;
}

static void ow_print(uint16_t t)
{
	struct decomp {
		uint16_t m4 : 1, m3 : 1, m2 : 1, m1 : 1, units : 12;
	};
	struct decomp d = *(struct decomp *) &t;
	int nom = 8*d.m1 + 4*d.m2 + 2*d.m3 + 1*d.m4;
	int denom = 16;
	int frac = (nom*100000/denom + 5) /10; // 2344

	print_decimal(uart_putc, d.units, 2);
	uart_putc('.');
	print_decimal(uart_putc, frac, 4);
}

void ow_cmd(char *cmd, int len)
{
	if (len == 2 && cmd[1] == 't') {
		ow_cmd_tst();
		return;
	}
	if (len != 2 || cmd[1] != 'r') {
		uart_puts("How about 'ot' or 'or' instead?\r\n");
		return;
	}

	if (!ow_reset_and_select_device(addr_sen))
		return;
	if (!ow_write_byte(0x44)) {
		uart_puts("Something pulled the line in the wrong direction.\r\n");
		return;
	}

	for (int i = 0; i < 100000; i++) asm("nop");

	if (!ow_reset_and_select_device(addr_sen))
		return;
	if (!ow_write_byte(0xbe)) {
		uart_puts("Some device is acting up.\r\n");
		return;
	}

	unsigned char buf[9];
	for (int i = 0; i < 9; i++) {
		buf[i] = ow_read_byte();
		uart_puts_hex(buf[i], 2);
		uart_putc(' ');
	}
	uart_puts(" crc: ");
	int reg = 0;
	for (int i = 0; i < 8; i++) {
		reg = crc(reg, buf[i]);
	}
	uart_puts_hex_0x(reg);
	if (reg == buf[8])
		uart_puts(" ✓");
	else
		uart_puts(" ✕");
	uart_puts("\r\n");

	uart_puts("Temperature is possibly ");
	ow_print(* (uint16_t *) buf);
	uart_puts("\r\n");
}

static void ow_cmd_tst()
{
	uart_puts("Attempting discovery, sending reset and presence detection… ");
	uart_puts("rx: ");
	uart_puts_hex_0x(ow_reset_and_presence_detect());
	uart_puts("\r\n");

	if (!ow_write_byte(0xf0)) {
		uart_puts("Couldn't initiate search.\r\n");
		return;
	}

	uint8_t addr[8];
	int suc = 1;
	for (int a = 7; a >= 0; a--) {
		for (int i = 0; i < 8 && suc; i++) {
			for (int b=0; b < 2000; b++)
				asm("nop");
			int b = ow_read_bit() << 1;
			b |= ow_read_bit();
			if (b == 0b01) {
				suc = ow_write_bit(0);
			} else if (b == 0b10) {
				suc = ow_write_bit(1);
				addr[a] |= ((uint64_t) 1l) << i;
			} else {
				suc = 0;
			}
		}
	}
	if (!suc)
		uart_puts("Some rubbish came along.\r\n");

	void printresult()
	{
		uart_puts("Got device: ");
		for (int a = 0; a < 8; a++) {
			uart_puts_hex(addr[a], 2);
			uart_putc(' ');
		}

		int reg = 0;
		for (int i = 7; i >= 1; i--) {
			reg = crc(reg, addr[i]);
		}
		uart_puts(" crc: ");
		uart_puts_hex_0x(reg);
		if (reg == addr[0])
			uart_puts(" ✓");
		else
			uart_puts(" ✕");

		uart_puts("\r\n");

		uart_puts("Received bits: ");
		for (int a = 7; a >= 0; a--) {
			for (int i = 0; i < 8; i++) {
				uart_putc('0' + !!(addr[a] & (1l << i)));
			}
		}
		uart_puts("\r\n");
	}
	printresult();

	uart_puts("Attempting single device.");
	ow_reset_and_presence_detect();

	if (!ow_write_byte(0x33)) {
		uart_puts("Couldn't initiate read rom.\r\n");
		return;
	}

	for (int i = 0; i < sizeof(addr); i++)
		addr[i] = 0x0;

	for (int a = 7; a >= 0; a--) {
		for (int i = 0; i < 8; i++) {
			int b = ow_read_bit();
			addr[a] |= b * (1 << i);
			//uart_putc('0' + b);
		}
	}
	uart_puts("\r\n");
	printresult();
}

void setup_onewire()
{
	volatile struct usart_reg *usart = cfg_ow.usart;

	struct gpio_conf gcfg = {
		.mode = GPIO_MODER_AF,
		.otype = GPIO_OTYPER_OD,
		.pupd = GPIO_PUPDR_PULLUP,
		.ospeed = GPIO_OSPEEDR_HIGH,
		.alt = cfg_ow.tx_af,
	};
	gpio_configure(cfg_ow.tx, &gcfg);

	struct gpio_conf gcfg_pwr = {
		.mode = GPIO_MODER_OUT,
		.otype = GPIO_OTYPER_PP,
		.ospeed = GPIO_OSPEEDR_MEDIUM,
	};
	gpio_configure(cfg_ow.power, &gcfg_pwr);

	if (usart == usart2)
		rcc->apb1enr.usart2en = 1;
	else
		assert(0);

	struct usart_cr1 cr1 = {
		.te = 1,
		.re = 1,
		.m0 = 0, // 8bit data
		.m1 = 0, // ⋮
		.rxneie = 1,
	};
	usart->cr1 = cr1;

	struct usart_cr2 cr2 = {
		.stop = 0, // 1 stop bit
	};
	usart->cr2 = cr2;

	struct usart_cr3 cr3 = {
		.hdsel = 1, // half-duplex single-wire
	};
	usart->cr3 = cr3;

	// Set up interrupt over exti
	/*struct exti_conf ecfg = {
		.im = 1,
		.ft = 1,
	};
	exti_configure_line(cfg_ow.extiln, &ecfg); */

	usart->cr1.ue = 1;
	while (!usart->cr1.ue) asm("nop");
}
