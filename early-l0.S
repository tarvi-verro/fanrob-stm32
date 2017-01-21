

	.syntax unified
	.cpu cortex-m0
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
	.word Reset_Handler	// 0x04, reset handler
	.word undef_interr	// 0x08, NMI
	.word Hardfault_Handler	// 0x0C, hardfault
	.word 0			// 0x10
	.word 0			// 0x14
	.word 0			// 0x18
	.word 0			// 0x1C
	.word 0			// 0x20
	.word 0			// 0x24
	.word 0			// 0x28
	.word undef_interr	// 0x2C, SVC Handler
	.word 0			// 0x30
	.word 0			// 0x34
	.word undef_interr	// 0x38, PendSV Handler
	.word undef_interr	// 0x3C, SysTick Handler
	.word undef_interr	// 0x40, WWDG
	.word undef_interr	// 0x44, PVD through EXTI Line detection
	.word undef_interr	// 0x48, RTC global (EXTI 17/19/20 lines) and LSE CSS through EXTI 19		1 << 2
	.word undef_interr	// 0x4C, flash memory and data EEPROM global int.
	.word undef_interr	// 0x50, RCC global
	.word undef_interr	// 0x54, EXTI_0_1		1 << 5
	.word undef_interr	// 0x58, EXTI_2_3
	.word undef_interr	// 0x5C, EXTI_4_15		1 << 7
	.word 0			// 0x60, reserved
	.word undef_interr	// 0x64, DMA1_CH1
	.word undef_interr	// 0x68, DMA1_CH2_3
	.word undef_interr	// 0x6C, DMA1_CH4_7
	.word undef_interr	// 0x70, ADC and comparator int. through EXTI 21 & 22
	.word undef_interr	// 0x74, LPTIMER1 int. through EXTI 29
	.word undef_interr	// 0x78, USART4/5 global int.
	.word 0			// 0x7C, TIM2 global int.
	.word undef_interr	// 0x80, TIM3 global
	.word undef_interr	// 0x84, TIM6 global
	.word undef_interr	// 0x88, TIM7 global
	.word 0			// 0x8C, reserved
	.word undef_interr	// 0x90, TIM21 global
	.word undef_interr	// 0x94, I2C3 global int.
	.word undef_interr	// 0x98, TIM22 global
	.word undef_interr	// 0x9C, I2C1 global int. through EXTI 23
	.word undef_interr	// 0xA0, I2C2 global int.
	.word undef_interr	// 0xA4, SPI1 global int.
	.word undef_interr	// 0xA8, SPI2 global int.
	.word undef_interr	// 0xAC, USART1 global int. through EXTI 25
	.word undef_interr	// 0xB0, USART2 global int. through EXTI 26
	.word undef_interr	// 0xB4, LPUART1 global int. through EXTI 28 + AES global int.
.size vecs, .-vecs

	.weak undef_interr
	.thumb_set undef_interr,Default_Handler


