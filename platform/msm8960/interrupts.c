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

#include <debug.h>
#include <reg.h>
#include <arch/arm.h>
#include <kernel/thread.h>
#include <platform/interrupts.h>

#include <platform/irqs.h>
#include <platform/iomap.h>

#define GIC_CPU_REG(off)            (MSM_GIC_CPU_BASE  + (off))
#define GIC_DIST_REG(off)           (MSM_GIC_DIST_BASE + (off))

#define GIC_CPU_CTRL                GIC_CPU_REG(0x00)
#define GIC_CPU_PRIMASK             GIC_CPU_REG(0x04)
#define GIC_CPU_BINPOINT            GIC_CPU_REG(0x08)
#define GIC_CPU_INTACK              GIC_CPU_REG(0x0c)
#define GIC_CPU_EOI                 GIC_CPU_REG(0x10)
#define GIC_CPU_RUNNINGPRI          GIC_CPU_REG(0x14)
#define GIC_CPU_HIGHPRI             GIC_CPU_REG(0x18)

#define GIC_DIST_CTRL               GIC_DIST_REG(0x000)
#define GIC_DIST_CTR                GIC_DIST_REG(0x004)
#define GIC_DIST_ENABLE_SET         GIC_DIST_REG(0x100)
#define GIC_DIST_ENABLE_CLEAR       GIC_DIST_REG(0x180)
#define GIC_DIST_PENDING_SET        GIC_DIST_REG(0x200)
#define GIC_DIST_PENDING_CLEAR      GIC_DIST_REG(0x280)
#define GIC_DIST_ACTIVE_BIT         GIC_DIST_REG(0x300)
#define GIC_DIST_PRI                GIC_DIST_REG(0x400)
#define GIC_DIST_TARGET             GIC_DIST_REG(0x800)
#define GIC_DIST_CONFIG             GIC_DIST_REG(0xc00)
#define GIC_DIST_SOFTINT            GIC_DIST_REG(0xf00)

struct ihandler {
    int_handler func;
    void *arg;
};

static struct ihandler handler[NR_IRQS];
static void platform_gic_dist_init(void);
static void platform_gic_cpu_init(void);

void platform_init_interrupts(void)
{
    platform_gic_dist_init();
    platform_gic_cpu_init();
}

void platform_gic_dist_init(void)
{
    unsigned int i;
    unsigned num_irq = 0;
    unsigned cpumask = 1;

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
       and edge sensitive, active high */
    for (i=32; i < num_irq; i += 16)
        writel(0xffffffff, GIC_DIST_CONFIG + i * 4/16 );

    writel(0xffffffff, GIC_DIST_CONFIG + 4);

    /* Set up interrupts for this CPU */
    for (i = 32; i < num_irq; i += 4)
        writel(cpumask, GIC_DIST_TARGET + i * 4 / 4);

    /* Set priority of all interrupts*/

    /*
     * In bootloader we dont care about priority so
     * setting up equal priorities for all
     */
    for (i=0; i < num_irq; i += 4)
        writel(0xa0a0a0a0, GIC_DIST_PRI + i * 4/4);

    /*Disabling interrupts*/
    for (i=0; i < num_irq; i += 32)
        writel(0xffffffff, GIC_DIST_ENABLE_CLEAR + i * 4/32);

    writel(0x0000ffff, GIC_DIST_ENABLE_SET);

    /*Enabling GIC*/
    writel(1, GIC_DIST_CTRL);
}

void platform_gic_cpu_init(void)
{
    writel(0xf0, GIC_CPU_PRIMASK);
    writel(1, GIC_CPU_CTRL);
}

enum handler_return platform_irq(struct arm_iframe *frame)
{
    unsigned num;
    enum handler_return ret;
    num = readl(GIC_CPU_INTACK);
    if (num > NR_IRQS)
        return 0;
    ret = handler[num].func(handler[num].arg);
    writel(num, GIC_CPU_EOI);
    return ret;
}

void platform_fiq(struct arm_iframe *frame)
{
    PANIC_UNIMPLEMENTED;
}

status_t mask_interrupt(unsigned int vector)
{
    unsigned reg = GIC_DIST_ENABLE_CLEAR + (vector/32)*4;
    unsigned bit = 1 << (vector & 31);
    writel(bit, reg);
    return 0;
}

status_t unmask_interrupt(unsigned int vector)
{

    unsigned reg = GIC_DIST_ENABLE_SET + (vector/32)*4;
    unsigned bit = 1 << (vector & 31);
    writel(bit, reg);
    return 0;
}

void register_int_handler(unsigned int vector, int_handler func, void *arg)
{
    if (vector >= NR_IRQS)
        return;

    enter_critical_section();
    handler[vector].func = func;
    handler[vector].arg = arg;
    exit_critical_section();
}

void clear_pending_int(void)
{
    unsigned num_irq = 0;

    num_irq = readl(GIC_DIST_CTR) & 0x1f;
    num_irq = (num_irq + 1) * 32;
    unsigned i;
    for (i = 0; i < num_irq; i += 32)
        writel(0xffffffff, GIC_DIST_PENDING_CLEAR + i * 4 / 32);
}
