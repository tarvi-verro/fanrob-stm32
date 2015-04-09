#include <stdint.h>

void early_reset(void) __attribute__((section("early")));
void early_reset(void)
{
	extern uint8_t _data_vma_s, _data_vma_e, _data_lma;
	extern uint8_t __bss_start__, __bss_end__;

	static uint8_t * const dlma = &_data_lma;
	static uint8_t * const dvma_s = &_data_vma_s;
	static uint8_t * const dvma_e = &_data_vma_e;

	uint8_t *src = dlma;
	uint8_t *dst = dvma_s;

	while (src < dvma_e)
		*dst++ = *src++;

	for (dst = &__bss_start__; dst < &__bss_end__; dst++)
		*dst = 0;
}
