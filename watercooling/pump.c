#define CFG_PUMP
#include "gpio-abs.h"
#include "tim-preset.h"
#include "uart.h"
#include "pump.h"
#include "conf.h"
#include "decimal.h"

void setup_pump()
{
	// PWM timer
	struct gpio_conf ctlc = {
		.otype = GPIO_OTYPER_PP,
		.pupd = GPIO_PUPDR_PULLDOWN,
		.mode = GPIO_MODER_AF,
		.alt = cfg_pump.gaten_af,
	};
	gpio_configure(cfg_pump.gaten, &ctlc);
	setup_tim_fast();
	tim_fast_duty_set(cfg_pump.fast_ch, 0);
}

void pump_cmd(char *cmd, int len)
{
	assert(*cmd == 'p');

	if (len <= 2 || cmd[1] != ' ') {
		uart_puts("Expected a speed [0:255].\r\n");
		return;
	}
	int sum = parseBase10(cmd + 2, len - 2);
	if (sum > 255) {
		uart_puts("Speed must be between [0:255]!\r\n");
		return;
	}
	tim_fast_duty_set(cfg_pump.fast_ch, sum);
	uart_puts("Set speed ");
	uart_putc('0' + sum/100);
	uart_putc('0' + (sum/10) % 10);
	uart_putc('0' + (sum/1) % 10);
	uart_puts("\r\n");
}
