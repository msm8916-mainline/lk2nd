/* Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Fundation, Inc. nor the names of its
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

#include <reg.h>
#include <bits.h>
#include <debug.h>
#include <arch/arm.h>
#include <arch/defines.h>
#include <qtimer.h>
#include <qgic_v3.h>

#define GIC_WAKER_PROCESSORSLEEP                 BIT(1)
#define GIC_WAKER_CHILDRENASLEEP                 BIT(2)

void qgic_dist_init()
{
	uint32_t num_irq;
	uint32_t affinity;
	uint32_t i;

	/* Read the mpidr register to find out the boot up cluster */
	__asm__ volatile("mrc p15, 0, %0, c0, c0, 5" : "=r" (affinity));

	/* For aarch32 mode we have only 3 affinity values: aff0:aff1:aff2*/
	affinity = affinity & 0x00ffffff;

	writel(0, GIC_DIST_CTRL);

	/*
	 * Find out how many interrupts are supported.
	 */
	num_irq = readl(GIC_DIST_CTR) & 0x1f;
	num_irq = (num_irq + 1) * 32;

	/* Do the qgic dist initialization */
	qgic_dist_config(num_irq);

	/* Write the affinity value, for routing all the SPIs */
	for (i = 32; i < num_irq; i++)
		writel(affinity, GICD_IROUTER + i * 8);

	/* Enable affinity routing of grp0/grp1 interrupts */
	writel(ENABLE_GRP0_SEC | ENABLE_GRP1_NS | ENABLE_ARE, GIC_DIST_CTRL);
}

void qgic_cpu_init()
{
	uint32_t retry = 1000;
	uint32_t sre = 0;
	uint32_t pmr = 0xff;
	uint32_t eoimode = 0;
	uint32_t grpen1 = 0x1;

	/* Wait until redistributor is up */
	while(readl(GICR_WAKER_CPU0) & GIC_WAKER_CHILDRENASLEEP)
	{
		retry--;
		if (!retry)
		{
			dprintf(CRITICAL, "Failed to wake redistributor for CPU0\n");
			ASSERT(0);
		}
		mdelay(1);
	}


	/* Make sure the system register access is enabled for us */
	__asm__ volatile("mrc p15, 0, %0, c12, c12, 5" : "=r" (sre));
	sre |= BIT(0);
	__asm__ volatile("mcr p15, 0, %0, c12, c12, 5" :: "r" (sre));

	isb();

	/* If system register access is not set, we fail */
	__asm__ volatile("mrc p15, 0, %0, c12, c12, 5" : "=r" (sre));
	if (!(sre & BIT(0)))
	{
		dprintf(CRITICAL, "Failed to set SRE for NS world\n");
		ASSERT(0);
	}

	/* Set the priortiy mask register, interrupts with priority
	 * higher than this value will be signalled to processor.
	 * Lower value means higher priority.
	 */
	__asm__ volatile("mcr p15, 0, %0, c4, c6, 0" :: "r" (pmr));
	isb();

	/* Make sure EOI is handled in NS EL3 */
	__asm__ volatile("mrc p15, 0, %0, c12, c12, 4" : "=r" (eoimode));
	eoimode &= ~BIT(1);
	__asm__ volatile("mcr p15, 0, %0, c12, c12, 4" :: "r" (eoimode));
	isb();

	/* Enable grp1 interrupts for NS EL3*/
	__asm__ volatile("mcr p15, 0, %0, c12, c12, 7" :: "r" (grpen1));
	isb();
}

uint32_t qgic_read_iar()
{
	uint32_t iar;

	/* Read the interrupt ack register, for the current interrupt number */
	__asm__ volatile("mrc p15, 0, %0, c12, c12, 0" : "=r" (iar));
	isb();

	return iar;
}

void qgic_write_eoi(uint32_t iar)
{
	/* Write end of interrupt to indicate CPU that this interrupt is processed*/
	__asm__ volatile("mcr p15, 0, %0, c12, c12, 1" :: "r" (iar));
	isb();
}
