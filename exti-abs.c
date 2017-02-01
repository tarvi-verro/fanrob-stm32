
#include "exti-abs.h"
#include "assert-c.h"
#include "regs.h"
#include "syscfg.h"
#include "rcc.h"


static uint64_t gpio_callbs = 0x0;
extern void (*_rodata_exti_gpio_callb_s)(enum pin);
extern void (*_rodata_exti_gpio_callb_e)(enum pin);

void exti_configure_pin(enum pin p, struct exti_conf *cfg, void (*const *exti_callb)(enum pin))
{
	if (p == PNONE) return;
	assert(&_rodata_exti_gpio_callb_e - &_rodata_exti_gpio_callb_s < 0xf);
	assert(exti_callb >= &_rodata_exti_gpio_callb_s && exti_callb < &_rodata_exti_gpio_callb_e);

	assert(p <= PNONE);

	int callb_off = exti_callb - &_rodata_exti_gpio_callb_s;

	int z = p % 16;
	gpio_callbs &= ~(0xf << (4 * z));
	uint64_t bb = (callb_off + 1ll) << (4 * z);
	gpio_callbs |= bb;

#if defined(CONF_L0) || defined(CONF_F0)
	switch (z) {
	case 0: case 1:
		nvic_iser[0] |= 1 << 5;
		break;
	case 2: case 3:
		nvic_iser[0] |= 1 << 6;
		break;
	default:
		nvic_iser[0] |= 1 << 7;
	}
#elif defined(CONF_L4)
	switch (z) {
	case 0: nvic_iser[0] |= 1 <<  6; break;
	case 1: nvic_iser[0] |= 1 <<  7; break;
	case 2: nvic_iser[0] |= 1 <<  8; break;
	case 3: nvic_iser[0] |= 1 <<  9; break;
	case 4: nvic_iser[0] |= 1 << 10; break;
	case 5: case 6: case 7: case 8: case 9:
		nvic_iser[0] |= 1 << 23; break;
	case 10: case 11: case 12: case 13: case 14: case 15:
		nvic_iser[1] |= 1 << 8; break;
	default:
		assert(0);
	}
#endif

	uint32_t *exticr;
	rcc->apb2enr.syscfgen = 1;
	switch (z / 4) {
	case 0: exticr = (uint32_t *) &syscfg->exticr1; break;
	case 1: exticr = (uint32_t *) &syscfg->exticr2; break;
	case 2: exticr = (uint32_t *) &syscfg->exticr3; break;
	case 3: exticr = (uint32_t *) &syscfg->exticr4; break;
	};
	int port = p / 16;
	*exticr &= ~(0xf << (z % 4)*4);
	*exticr |= port << (z % 4)*4;
	exti_configure_line(p % 16, cfg);
}

static inline int get_pin(int exti_nr)
{
	int exti;
	switch (exti_nr / 4) {
	case 0: exti = *(uint32_t *) &syscfg->exticr1; break;
	case 1: exti = *(uint32_t *) &syscfg->exticr2; break;
	case 2: exti = *(uint32_t *) &syscfg->exticr3; break;
	case 3: exti = *(uint32_t *) &syscfg->exticr4; break;
	};
	int port = (exti >> 4*(exti_nr % 4)) & 0xf;
	return port * 16 + exti_nr;
}

static void handle_gpio_exti(int from, int to)
{
#if defined(CONF_F0) || defined(CONF_L0)
	uint32_t *_pr = (uint32_t *) &exti->pr;
#elif defined(CONF_L4)
	uint32_t *_pr = (uint32_t *) &exti->pr1;
#endif
	uint32_t pr = *_pr;
	for (int i = from; i <= to; i++) {
		int regp = 1 << i;
		if (!(regp & pr))
			continue;
		int callb_off = ((gpio_callbs >> i*4) & 0xf) - 1;
		if (callb_off == -1)
			assert(0);


		(&_rodata_exti_gpio_callb_s)[callb_off](get_pin(i));
		*_pr |= regp; // pr is cleared when writing 1
	}
}

#if defined(CONF_F0) || defined(CONF_L0)
void i_exti_0_1() { handle_gpio_exti(0, 1); }
void i_exti_2_3() { handle_gpio_exti(2, 3); }
void i_exti_4_15() { handle_gpio_exti(4, 15); }
#elif defined(CONF_L4)
void i_exti_0() { handle_gpio_exti(0, 0); }
void i_exti_1() { handle_gpio_exti(1, 1); }
void i_exti_2() { handle_gpio_exti(2, 2); }
void i_exti_3() { handle_gpio_exti(3, 3); }
void i_exti_4() { handle_gpio_exti(4, 4); }
void i_exti_5_9() { handle_gpio_exti(5, 9); }
void i_exti_10_15() { handle_gpio_exti(10, 15); }
#else
#error uhuh, also check the rest of the macro spaghetti
#endif

