#pragma once
#include <stdint.h>

struct gpio_reg;
struct rcc_reg;
struct dma_reg;
struct rtc_reg;
struct pwr_reg;
struct adc_reg;

static volatile struct rcc_reg *const rcc = (struct rcc_reg *) 0x40023800; // ✓
static volatile struct rtc_reg *const rtc = (struct rtc_reg *) 0x40002800; // ✓
static volatile struct dma_reg *const dma1 = (volatile struct dma_reg *) 0x40026000; // ✓
static volatile struct dma_reg *const dma2 = (volatile struct dma_reg *) 0x40026400; // ✓
static volatile struct pwr_reg *const pwr = (struct pwr_reg *) 0x40007000; // ✓
static volatile struct adc_reg *const adc = (struct adc_reg *) 0x40012000; // ✓ "ADC1 - ADC2 - ADC3"
static volatile struct gpio_reg *const gpio_reg_a = (struct gpio_reg *) 0x40020000; // ✓
static volatile struct gpio_reg *const gpio_reg_b = (struct gpio_reg *) 0x40020400; // ✓
static volatile struct gpio_reg *const gpio_reg_c = (struct gpio_reg *) 0x40020800; // ✓
static volatile struct gpio_reg *const gpio_reg_d = (struct gpio_reg *) 0x40020C00; // ✓
static volatile struct gpio_reg *const gpio_reg_e = (struct gpio_reg *) 0x40021000; // ✓
static volatile struct gpio_reg *const gpio_reg_f = (struct gpio_reg *) 0x40021400; // ✓
static volatile struct gpio_reg *const gpio_reg_g = (struct gpio_reg *) 0x40021800; // ✓
static volatile struct gpio_reg *const gpio_reg_h = (struct gpio_reg *) 0x40021C00; // ✓
static volatile struct gpio_reg *const gpio_reg_i = (struct gpio_reg *) 0x40022000; // ✓
static volatile struct gpio_reg *const gpio_reg_j = (struct gpio_reg *) 0x40022400; // ✓
static volatile struct gpio_reg *const gpio_reg_k = (struct gpio_reg *) 0x40022800; // ✓

#define dma1_macro ((volatile struct dma_reg *) 0x40026000)
#define dma2_macro ((volatile struct dma_reg *) 0x40026400)

