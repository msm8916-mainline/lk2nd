/* SPDX-License-Identifier: BSD-3-Clause */
#include <platform/iomap.h>

.cpu	arm926ej-s

.text
.align	16
.global	wcnss_entry
wcnss_entry:
	b	wcnss_reset
	b	wcnss_reset
	b	wcnss_reset
	b	wcnss_reset
	b	wcnss_reset
	b	wcnss_reset
	b	wcnss_reset
	b	wcnss_reset

wcnss_reset:
	/* Turn on LED to show a sign of life */
	ldr	r0, =#GPIO_CONFIG_ADDR(120)
	mov	r1, #0x3	/* pull up */
	str	r1, [r0]

	/* Spin until start address is set */
	adr	r1, wcnss_spin_addr
1:
	ldr	lr, [r1]
	cmp	lr, #0
	beq	1b

	/* Turn off LED again */
	mov	r1, #0x1	/* pull down */
	str	r1, [r0]

	/* Load registers and jump there */
	adr	r1, wcnss_args
	ldm	r1, {r0-r2}
	bx	lr

.global	wcnss_spin_addr
wcnss_spin_addr:
	.word	0
.global	wcnss_args
wcnss_args:
	.word	0, 0, 0	/* r0-r2 */
