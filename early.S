

	.syntax unified
	.cpu cortex-m0
	.thumb

.global  Default_Handler
.global vecs

.word _data_lma
.word _data_vma_s
.word _data_vma_e

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
	bcc	CpyDat
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
Default_Handler:
Infinite_Loop:
	b  Infinite_Loop
	.size Default_Handler, .-Default_Handler

// see 91010 memory registers @ 169
.section .vec_table,"a",%progbits
.type vecs, %object
vecs:
	.word _estack		// 0x00, reserved
	.word Reset_Handler	// 0x04, reset handler
	.word undef_interr	// 0x08, NMI
	.word Default_Handler	// 0x0C, hardfault
	.word 0			// 0x10
	.word 0			// 0x14
	.word 0			// 0x18
	.word 0			// 0x1C
	.word 0			// 0x20
	.word 0			// 0x24
	.word 0			// 0x28
	.word undef_interr	// 0x2C, SVCall
	.word 0			// 0x30
	.word 0			// 0x34
	.word undef_interr	// 0x38, PendSV
	.word undef_interr	// 0x3C, systick
	.word undef_interr	// 0x40, WWDG
	.word 0			// 0x44, reserved
	.word undef_interr	// 0x48, RTC interrupts
	.word undef_interr	// 0x4C, flash global interr
	.word undef_interr	// 0x50, RCC global
	.word undef_interr	// 0x54, EXTI
	.word undef_interr	// 0x58, EXTI
	.word undef_interr	// 0x5C, EXTI
	.word 0			// 0x60, reserved
	.word undef_interr	// 0x64, DMA_CH1
	.word undef_interr	// 0x68, DMA_CH2_3
	.word undef_interr	// 0x6C, DMA_CH4_5
	.word undef_interr	// 0x70, ADC
	.word undef_interr	// 0x74, TIM1 break, update, trig, comm
	.word undef_interr	// 0x78, TIM1 capt, compare
	.word 0			// 0x7C, reserved
	.word undef_interr	// 0x80, TIM3 global
	.word undef_interr	// 0x84, TIM6 global, glitch in doc
	.word 0			// 0x88, reserved
	.word undef_interr	// 0x8C, TIM14 global
	.word undef_interr	// 0x90, TIM15 global
	.word undef_interr	// 0x94, TIM16
	.word undef_interr	// 0x98, TIM17
	.word undef_interr	// 0x9C, I2C1
	.word undef_interr	// 0xA0, I2C2
	.word undef_interr	// 0xA4, SPI1
	.word undef_interr	// 0xA8, SPI2
	.word undef_interr	// 0xAC, USART1
	.word undef_interr	// 0xB0, USART2
	.word undef_interr	// 0xB4, USART3456 w/ EXTI line 28
	.word 0			// 0xB8, reserved
	.word undef_interr	// 0xBC, USB
.size vecs, .-vecs

	.weak undef_interr
	.thumb_set undef_interr,Default_Handler


