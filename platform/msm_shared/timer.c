/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
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
#include <sys/types.h>

#include <platform/timer.h>
#include <platform/irqs.h>
#include <platform/iomap.h>
#include <platform/interrupts.h>
#include <kernel/thread.h>

#ifdef PLATFORM_MSM7X30

#define MSM_GPT_BASE (MSM_TMR_BASE + 0x4)
#define MSM_DGT_BASE (MSM_TMR_BASE + 0x24)
#define GPT_REG(off) (MSM_GPT_BASE + (off))
#define DGT_REG(off) (MSM_DGT_BASE + (off))
#define SPSS_TIMER_STATUS    (MSM_TMR_BASE + 0x88)

#define GPT_MATCH_VAL        GPT_REG(0x0000)
#define GPT_COUNT_VAL        GPT_REG(0x0004)
#define GPT_ENABLE           GPT_REG(0x0008)
#define GPT_ENABLE_CLR_ON_MATCH_EN        2
#define GPT_ENABLE_EN                     1
#define GPT_CLEAR            GPT_REG(0x000C)

#define DGT_MATCH_VAL        DGT_REG(0x0000)
#define DGT_COUNT_VAL        DGT_REG(0x0004)
#define DGT_ENABLE           DGT_REG(0x0008)
#define DGT_ENABLE_CLR_ON_MATCH_EN        2
#define DGT_ENABLE_EN                     1
#define DGT_CLEAR            DGT_REG(0x000C)
#define DGT_CLK_CTL          DGT_REG(0x0010)

#else
#define GPT_REG(off) (MSM_GPT_BASE + (off))

#define GPT_MATCH_VAL        GPT_REG(0x0000)
#define GPT_COUNT_VAL        GPT_REG(0x0004)
#define GPT_ENABLE           GPT_REG(0x0008)
#define GPT_ENABLE_CLR_ON_MATCH_EN        2
#define GPT_ENABLE_EN                     1
#define GPT_CLEAR            GPT_REG(0x000C)

#define DGT_MATCH_VAL        GPT_REG(0x0010)
#define DGT_COUNT_VAL        GPT_REG(0x0014)
#define DGT_ENABLE           GPT_REG(0x0018)
#define DGT_ENABLE_CLR_ON_MATCH_EN        2
#define DGT_ENABLE_EN                     1
#define DGT_CLEAR            GPT_REG(0x001C)

#define SPSS_TIMER_STATUS    GPT_REG(0x0034)
#endif


static platform_timer_callback timer_callback;
static void *timer_arg;
static time_t timer_interval;

static volatile uint32_t ticks;

static enum handler_return timer_irq(void *arg)
{
	ticks += timer_interval;
	return timer_callback(timer_arg, ticks);
}

status_t platform_set_periodic_timer(
	platform_timer_callback callback,
	void *arg, time_t interval)
{
#ifdef PLATFORM_MSM7X30
        unsigned val = 0;
	unsigned mask = (0x1 << 28);
	//Check for the hardware revision
	val = readl(HW_REVISION_NUMBER);
	if(val & mask)
	    writel(1, DGT_CLK_CTL);
#endif
	enter_critical_section();

	timer_callback = callback;
	timer_arg = arg;
	timer_interval = interval;

	writel(timer_interval * 19200, DGT_MATCH_VAL);
	writel(0, DGT_CLEAR);
	writel(DGT_ENABLE_EN | DGT_ENABLE_CLR_ON_MATCH_EN, DGT_ENABLE);
	
	register_int_handler(INT_DEBUG_TIMER_EXP, timer_irq, 0);
	unmask_interrupt(INT_DEBUG_TIMER_EXP);

	exit_critical_section();
	return 0;
}


time_t current_time(void)
{
	return ticks;
}

void platform_init_timer(void)
{
	writel(0, DGT_ENABLE);
}

static void wait_for_timer_op(void)
{
#if PLATFORM_QSD8K || PLATFORM_MSM7X30
	while(readl(SPSS_TIMER_STATUS)) ;
#endif
}

void platform_uninit_timer(void)
{
	writel(0, DGT_ENABLE);
	wait_for_timer_op();
	writel(0, DGT_CLEAR);
	wait_for_timer_op();
}

void mdelay(unsigned msecs)
{
  msecs *= 33;

  writel(0, GPT_CLEAR);
  writel(0, GPT_ENABLE);
  while(readl(GPT_COUNT_VAL) != 0) ;

  writel(GPT_ENABLE_EN, GPT_ENABLE);
  while(readl(GPT_COUNT_VAL) < msecs) ;

  writel(0, GPT_ENABLE);
  writel(0, GPT_CLEAR);
}
