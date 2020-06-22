#pragma once
#include <stdint.h>

struct gpio_reg;
struct rcc_reg;
struct usb_reg;
struct exti_reg;
struct adc_reg;

static volatile uint32_t *const flash_acr = (uint32_t *) 0x40022000;
static volatile struct rcc_reg *const rcc = (struct rcc_reg *) 0x40021000;
static volatile struct gpio_reg *const gpio_reg_a = (struct gpio_reg *) 0x40010800;
static volatile struct gpio_reg *const gpio_reg_b = (struct gpio_reg *) 0x40010C00;
static volatile struct gpio_reg *const gpio_reg_c = (struct gpio_reg *) 0x40011000;
static volatile struct usb_reg *const usbfs = (struct usb_reg *) 0x40005C00;
static volatile struct usart_reg *const usart1 = (struct usart_reg *) 0x40013800;
static volatile struct adc_reg *const adc1 = (struct adc_reg *) 0x40012400;
static volatile struct adc_reg *const adc2 = (struct adc_reg *) 0x40012800;

static uint32_t *const nvic_iser = (uint32_t *) 0xe000e100;
static volatile struct exti_reg *const exti = (struct exti_reg *) 0x40010400;
static uint32_t *const usb_packet_memory = (uint32_t *) 0x40006000;

