/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <arch/arm.h>
#include <platform/irqs.h>
#include <platform/interrupts.h>
#include <debug.h>

extern int target_supports_qgic();

enum handler_return platform_irq(struct arm_iframe *frame)
{
#if TARGET_USES_GIC_VIC
	if(target_supports_qgic())
		return gic_platform_irq(frame);
	else
		return vic_platform_irq(frame);
#else
	return gic_platform_irq(frame);
#endif
}

void platform_fiq(struct arm_iframe *frame)
{
#if TARGET_USES_GIC_VIC
	if(target_supports_qgic())
		gic_platform_fiq(frame);
	else
		vic_platform_fiq(frame);
#else
	gic_platform_fiq(frame);
#endif
}

status_t mask_interrupt(unsigned int vector)
{
#if TARGET_USES_GIC_VIC
	if(target_supports_qgic())
		return gic_mask_interrupt(vector);
	else
		return vic_mask_interrupt(vector);
#else
	return gic_mask_interrupt(vector);
#endif
}

status_t unmask_interrupt(unsigned int vector)
{
#if TARGET_USES_GIC_VIC
	if(target_supports_qgic())
		return gic_unmask_interrupt(vector);
	else
		return vic_unmask_interrupt(vector);
#else
	return gic_unmask_interrupt(vector);
#endif
}

void register_int_handler(unsigned int vector, int_handler func, void *arg)
{
#if TARGET_USES_GIC_VIC
	if(target_supports_qgic())
		gic_register_int_handler(vector, func, arg);
	else
		vic_register_int_handler(vector, func, arg);
#else
		gic_register_int_handler(vector, func, arg);
#endif
}
