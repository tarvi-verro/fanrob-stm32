

	.syntax unified
	.cpu cortex-m4
	.thumb

.global  Default_Handler
.global vecs


.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
	ldr	r0, =_estack       /* set stack pointer */
	mov	sp, r0
/* copy data section to ram */
	ldr	r3, =_data_vma_s
	ldr	r1, =_data_lma
	ldr	r2, =_data_vma_e
CpyDat:
	ldr	r4, [r1]
	str	r4, [r3]
	adds	r1, r1, #4
	adds	r3, r3, #4
	cmp	r2, r3
	bhi	CpyDat
/*
	extern uint8_t __bss_start__, __bss_end__; */
/* null the bss */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__
	movs	r3, #0
	cmp	r1, r2
	beq	NoNull
NullBss:
	str	r3, [r1]
	adds	r1, r1, #4
	cmp	r1, r2
	bcc	NullBss
NoNull:
//	bl	early_reset
	bl	main
	bx	lr

.size Reset_Handler, .-Reset_Handler

.section .text.Default_Handler,"ax",%progbits
.weak Default_Handler
.type Default_Handler, %function
Default_Handler:
Infinite_Loop:
	movs	r0, #0
	bl	assert
	b	Infinite_Loop
.weak Hardfault_Handler
.type Hardfault_Handler, %function
Hardfault_Handler:
	movs	r0, #0
	bl	assert
	b Hardfault_Handler
.size Default_Handler, .-Default_Handler

// see 91010 memory registers @ 169
.section .vec_table,"a",%progbits
.type vecs, %object
vecs:
	.word _estack		// 0x00, reserved
	.word Reset_Handler	// 0x04, Reset handler
	.word undef_interr	// 0x08, NMI
	.word Hardfault_Handler	// 0x0C, HardFault

	.word undef_interr	// 0x10, MemManage
	.word undef_interr	// 0x14, BusFault
	.word undef_interr	// 0x18, UsageFault
	.word 0			// 0x1C

	.word 0			// 0x20
	.word 0			// 0x24
	.word 0			// 0x28
	.word undef_interr	// 0x2C, SVCall

	.word undef_interr	// 0x30, Debug
	.word 0			// 0x34
	.word undef_interr	// 0x38, PendSV
	.word undef_interr	// 0x3C, systick

	.word undef_interr	// 0x40,  0, WWDG
	.word undef_interr	// 0x44,  1, PVD_PVM
	.word undef_interr	// 0x48,  2, RTC_TAMP_STAMP/CSS_LSE	1 << 2
	.word undef_interr	// 0x4C,  3, RTC_WKUP

	.word undef_interr	// 0x50,  4, FLASH
	.word undef_interr	// 0x54,  5, RCC 			1 << 5
	.word undef_interr	// 0x58,  6, EXTI0
	.word undef_interr	// 0x5C,  7, EXTI1			1 << 7

	.word undef_interr	// 0x60,  8, EXTI2
	.word undef_interr	// 0x64,  9, EXTI3
	.word undef_interr	// 0x68, 10, EXTI4
	.word undef_interr	// 0x6C, 11, DMA1_CH1

	.word undef_interr	// 0x70, 12, DMA1_CH2
	.word undef_interr	// 0x74, 13, DMA1_CH3
	.word undef_interr	// 0x78, 14, DMA1_CH4
	.word undef_interr	// 0x7C, 15, DMA1_CH5

	.word undef_interr	// 0x80, 16, DMA1_CH6
	.word undef_interr	// 0x84, 17, DMA1_CH7
	.word undef_interr	// 0x88, 18, ADC1
	.word undef_interr	// 0x8C, 19, CAN1_TX

	.word undef_interr	// 0x90, 20, CAN1_RX0
	.word undef_interr	// 0x94, 21, CAN1_RX1
	.word undef_interr	// 0x98, 22, CAN1_SCE
	.word undef_interr	// 0x9C, 23, EXTI9_5

	.word undef_interr	// 0xA0, 24, TIM1_BRK/TIM15
	.word undef_interr	// 0xA4, 25, TIM1_UP/TIM16
	.word undef_interr	// 0xA8, 26, TIM1_TRG_COM
	.word undef_interr	// 0xAC, 27, TIM1_CC

	.word undef_interr	// 0xB0, 28, TIM2
	.word 0			// 0xB4, 29, reserved
	.word 0			// 0xB8, 30, reserved
	.word undef_interr	// 0xBC, 31, I2C1_EV

	.word undef_interr	// 0xC0, 32, I2C1_ER
	.word undef_interr	// 0xC4, 33, I2C2_EV
	.word undef_interr	// 0xC8, 34, I2C2_ER
	.word undef_interr	// 0xCC, 35, SPI1

	.word undef_interr	// 0xD0, 36, SPI2
	.word undef_interr	// 0xD4, 37, USART1
	.word undef_interr	// 0xD8, 38, USART2
	.word undef_interr	// 0xDC, 39, USART3

	.word undef_interr	// 0xE0, 40, EXTI15_10
	.word undef_interr	// 0xE4, 41, RTC_ALARM
	.word 0			// 0xE8, 42, reserved
	.word 0			// 0xEC, 43, reserved

	.word 0			// 0xF0, 44, reserved
	.word 0			// 0xF4, 45, reserved
	.word 0			// 0xF8, 46, reserved
	.word 0			// 0xFC, 47, reserved

	.word 0			// 0x100, 48, reserved
	.word undef_interr	// 0x104, 49, SDMMC1
	.word 0			// 0x108, 50, reserved
	.word undef_interr	// 0x10C, 51, SPI3

	.word 0			// 0x110, 52, reserved
	.word 0			// 0x114, 53, reserved
	.word undef_interr	// 0x118, 54, TIM6_DACUNDER
	.word 0			// 0x11C, 55, TIM7

	.word undef_interr	// 0x120, 56, DMA2_CH1
	.word undef_interr	// 0x124, 57, DMA2_CH2
	.word undef_interr	// 0x128, 58, DMA2_CH3
	.word undef_interr	// 0x12C, 59, DMA2_CH4

	.word undef_interr	// 0x130, 60, DMA2_CH5
	.word 0			// 0x134, 61, reserved
	.word 0			// 0x138, 62, reserved
	.word 0			// 0x13C, 63, reserved

	.word undef_interr	// 0x140, 64, COMP
	.word undef_interr	// 0x144, 65, LPTIM1
	.word undef_interr	// 0x148, 66, LPTIM2
	.word undef_interr	// 0x14C, 67, USB_FS

	.word undef_interr	// 0x150, 68, DMA2_CH6
	.word interr_dma2_ch7	// 0x154, 69, DMA2_CH7
	.word interr_lpuart1	// 0x158, 70, LPUART1
	.word undef_interr	// 0x15C, 71, QUADSPI

	.word undef_interr	// 0x160, 72, I2C3_EV
	.word undef_interr	// 0x164, 73, I2C3_ER
	.word undef_interr	// 0x168, 74, SAI1
	.word 0			// 0x16C, 75, reserved

	.word undef_interr	// 0x170, 76, SWPMI1
	.word undef_interr	// 0x174, 77, TSC
	.word 0			// 0x178, 78, reserved --- missing in doc
	.word undef_interr	// 0x17C, 79, AES

	.word undef_interr	// 0x180, 80, RNG
	.word undef_interr	// 0x184, 81, FPU
	.word undef_interr	// 0x188, 82, CRS
.size vecs, .-vecs

	.weak undef_interr
	.thumb_set undef_interr,Default_Handler


