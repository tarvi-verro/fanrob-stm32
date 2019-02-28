#pragma once
#include <stdint.h>

struct gpio_reg;

static volatile struct rcc_reg *const rcc = (struct rcc_reg *) 0x40021000;
static volatile struct gpio_reg *const gpio_reg_a = (struct gpio_reg *) 0x40010800;
static volatile struct gpio_reg *const gpio_reg_b = (struct gpio_reg *) 0x40010C00;
static volatile struct gpio_reg *const gpio_reg_c = (struct gpio_reg *) 0x40011000;

