#include <stdint.h>

/*
 * PA2 (A7) → lpuart1 tx
 * PA3 (A2) → lpuart1 rx
 */
#define io_uart gpio_reg_a
#define iop_uart_en iopaen
#define iop_uart_rcc iopenr
#define pin_uart_tx pin2
#define pin_uart_rx pin3

/* PA12 (D2) → gpio, rpm counter */
#define io_rpm gpio_reg_a
#define iop_rpm_en iopaen
#define iop_rpm_rcc iopenr
#define pin_rpm pin12

/* PB0 (D3) → TIM2_CH3 */
#define io_fanctl gpio_reg_b
#define iop_fanctl_en iopben
#define iop_fanctl_rcc iopenr
#define pin_fan1_ctl pin0

/* PB3 is LD3, green led */
#define io_green gpio_reg_b
#define iop_green_en iopben
#define iop_green_rcc iopenr
#define pin_green pin3

/* Configure uart.c */
#define ic_dma_receiver i_dma1_ch2_3

