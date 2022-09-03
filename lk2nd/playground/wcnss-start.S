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
	ldr	r0, =#GPIO_CONFIG_ADDR(120)
	mov	r1, #0x3	/* pull up */
	str	r1, [r0]
	b	.
