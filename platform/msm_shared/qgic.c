/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011,2014, The Linux Foundation. All rights reserved.
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
#include <arch/arm.h>
#include <qgic.h>

static uint8_t qgic_get_cpumask()
{
	uint32_t mask=0, i;

	/* Fetch the CPU MASK from the SGI/PPI reg */
	for (i=0; i < 32; i += 4) {
		mask = readl(GIC_DIST_TARGET + i);
		mask |= mask >> 16;
		mask |= mask >> 8;
		if (mask)
			break;
	}
	return mask;
}

/* Intialize distributor */
void qgic_dist_init(void)
{
	uint32_t i;
	uint32_t num_irq = 0;
	uint32_t cpumask;

	cpumask = qgic_get_cpumask();

	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

	/* Disabling GIC */
	writel(0, GIC_DIST_CTRL);

	/*
	 * Find out how many interrupts are supported.
	 */
	num_irq = readl(GIC_DIST_CTR) & 0x1f;
	num_irq = (num_irq + 1) * 32;

	/* Set up interrupts for this CPU */
	for (i = 32; i < num_irq; i += 4)
		writel(cpumask, GIC_DIST_TARGET + i * 4 / 4);

	qgic_dist_config(num_irq);

	/*Enabling GIC */
	writel(1, GIC_DIST_CTRL);
}

/* Intialize cpu specific controller */
void qgic_cpu_init(void)
{
	writel(0xf0, GIC_CPU_PRIMASK);
	writel(1, GIC_CPU_CTRL);
}

uint32_t qgic_read_iar()
{
	return readl(GIC_CPU_INTACK);
}

void qgic_write_eoi(uint32_t num)
{
	writel(num, GIC_CPU_EOI);
}
