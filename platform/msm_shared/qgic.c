/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <reg.h>
#include <debug.h>
#include <arch/arm.h>
#include <kernel/thread.h>
#include <platform/irqs.h>
#include <qgic.h>

static struct ihandler handler[NR_IRQS];

/* Intialize distributor */
static void qgic_dist_init(void)
{
	uint32_t i;
	uint32_t num_irq = 0;
	uint32_t cpumask = 1;

	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

	/* Disabling GIC */
	writel(0, GIC_DIST_CTRL);

	/*
	 * Find out how many interrupts are supported.
	 */
	num_irq = readl(GIC_DIST_CTR) & 0x1f;
	num_irq = (num_irq + 1) * 32;

	/* Set each interrupt line to use N-N software model
	 * and edge sensitive, active high
	 */
	for (i = 32; i < num_irq; i += 16)
		writel(0xffffffff, GIC_DIST_CONFIG + i * 4 / 16);

	writel(0xffffffff, GIC_DIST_CONFIG + 4);

	/* Set up interrupts for this CPU */
	for (i = 32; i < num_irq; i += 4)
		writel(cpumask, GIC_DIST_TARGET + i * 4 / 4);

	/* Set priority of all interrupts */

	/*
	 * In bootloader we dont care about priority so
	 * setting up equal priorities for all
	 */
	for (i = 0; i < num_irq; i += 4)
		writel(0xa0a0a0a0, GIC_DIST_PRI + i * 4 / 4);

	/* Disabling interrupts */
	for (i = 0; i < num_irq; i += 32)
		writel(0xffffffff, GIC_DIST_ENABLE_CLEAR + i * 4 / 32);

	writel(0x0000ffff, GIC_DIST_ENABLE_SET);

	/*Enabling GIC */
	writel(1, GIC_DIST_CTRL);
}

/* Intialize cpu specific controller */
static void qgic_cpu_init(void)
{
	writel(0xf0, GIC_CPU_PRIMASK);
	writel(1, GIC_CPU_CTRL);
}

/* Initialize QGIC. Called from platform specific init code */
void qgic_init(void)
{
	qgic_dist_init();
	qgic_cpu_init();
}

/* IRQ handler */
enum handler_return gic_platform_irq(struct arm_iframe *frame)
{
	uint32_t num;
	enum handler_return ret;

	num = readl(GIC_CPU_INTACK);
	if (num > NR_IRQS)
		return 0;

	ret = handler[num].func(handler[num].arg);
	writel(num, GIC_CPU_EOI);

	return ret;
}

/* FIQ handler */
void gic_platform_fiq(struct arm_iframe *frame)
{
	PANIC_UNIMPLEMENTED;
}

/* Mask interrupt */
status_t gic_mask_interrupt(unsigned int vector)
{
	uint32_t reg = GIC_DIST_ENABLE_CLEAR + (vector / 32) * 4;
	uint32_t bit = 1 << (vector & 31);

	writel(bit, reg);

	return 0;
}

/* Un-mask interrupt */
status_t gic_unmask_interrupt(unsigned int vector)
{
	uint32_t reg = GIC_DIST_ENABLE_SET + (vector / 32) * 4;
	uint32_t bit = 1 << (vector & 31);

	writel(bit, reg);

	return 0;
}

/* Register interrupt handler */
void gic_register_int_handler(unsigned int vector, int_handler func, void *arg)
{
	ASSERT(vector < NR_IRQS);

	enter_critical_section();
	handler[vector].func = func;
	handler[vector].arg = arg;
	exit_critical_section();
}
