/* SPDX-License-Identifier: BSD-3-Clause */

#include <asm.h>

.text
.fpu neon

/* void *rgb888_to_xrgb8888(void *out, const void *in, uint32_t npixels) */
FUNCTION(rgb888_to_xrgb8888)
0:	vld3.8	{d0-d2}, [r1]!
	vst4.8	{d0-d3}, [r0]!
	subs	r2, r2, #8
	bne	0b
	bx	lr

