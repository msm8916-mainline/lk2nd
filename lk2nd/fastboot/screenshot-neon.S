/* SPDX-License-Identifier: BSD-3-Clause */
#include <asm.h>

.text
.fpu neon

/* void *rgb565_to_rgb888(void *out, const void *in, uint32_t npixels) */
/* https://community.arm.com/arm-community-blogs/b/architectures-and-processors-blog/posts/coding-for-neon---part-4-shifting-left-and-right */
FUNCTION(rgb565_to_rgb888)
0:	vld1.16		{q0}, [r1]!
	vshr.u8		q3, q0, #3
	vshrn.i16	d3, q0, #5
	vshl.i16	q0, q0, #3
	vshrn.i16	d2, q3, #5
	vshl.i8		d3, d3, #2
	vmovn.i16	d4, q0
	vst3.8		{d2-d4}, [r0]!
	subs		r2, r2, #8
	bne		0b
	bx		lr

/* void *rgb888_swap(void *out, const void *in, uint32_t npixels) */
/* https://developer.arm.com/documentation/den0018/a/NEON-Code-Examples-with-Intrinsics/Swapping-color-channels */
FUNCTION(rgb888_swap)
0:	vld3.8	{d0-d2}, [r1]!
	vswp	d0, d2
	vst3.8	{d0-d2}, [r0]!
	subs	r2, r2, #8
	bne	0b
	bx	lr

/* void *rgb8888_swap_to_rgb888(void *out, const void *in, uint32_t npixels) */
FUNCTION(rgb8888_swap_to_rgb888)
0:	vld4.8	{d0-d3}, [r1]!
	vswp	d0, d2
	vst3.8	{d0-d2}, [r0]!
	subs	r2, r2, #8
	bne	0b
	bx	lr
