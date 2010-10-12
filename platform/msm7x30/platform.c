/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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
#include <dev/fbcon.h>
#include <kernel/thread.h>
#include <platform/debug.h>
#include <mddi_hw.h>
#include "gpio_hw.h"

void platform_init_interrupts(void);
void platform_init_timer();

void uart3_clock_init(void);
void uart_init(void);

struct fbcon_config *lcdc_init(void);

void platform_early_init(void)
{
        //uart3_clock_init();
	//uart_init();

	platform_init_interrupts();
	platform_init_timer();
}

void platform_init(void)
{
	struct fbcon_config *fb_cfg;

	dprintf(INFO, "platform_init()\n");
	acpu_clock_init();
	adm_enable_clock();
}

void mdp4_display_intf_sel(int output, int intf)
{
    unsigned bits, mask;
    unsigned dma2_cfg_reg;
    bits =  readl(MSM_MDP_BASE1 + 0x0038);
    mask = 0x03;	/* 2 bits */
    intf &= 0x03;	/* 2 bits */

    switch (output) {
        case EXTERNAL_INTF_SEL:
                intf <<= 4;
                mask <<= 4;
                break;
        case SECONDARY_INTF_SEL:
                intf &= 0x02;   /* only MDDI and EBI2 support */
                intf <<= 2;
                mask <<= 2;
                break;
        default:
                break;
        }

    bits &= ~mask;
    bits |= intf;
    writel(bits, MSM_MDP_BASE1 + 0x0038);	/* MDP_DISP_INTF_SEL */
}



void display_init(void)
{
    struct fbcon_config *fb_cfg;

#if DISPLAY_TYPE_MDDI
    mddi_clock_init(0, 480000000);
    mddi_panel_poweron();
    /* We need to config GPIO 38 for Sleep clock with Spl Fun 2 */
    toshiba_pmic_gpio_init(GPIO38_GPIO_CNTRL);
    fb_cfg = mddi_init();
    fbcon_setup(fb_cfg);
#endif

#if DISPLAY_TYPE_LCDC
    mdp_clock_init(122880000);
    fb_cfg = lcdc_init();
    panel_poweron();
    fbcon_setup(fb_cfg);
#endif
}

void display_shutdown(void)
{
#if DISPLAY_TYPE_LCDC
    /* Turning off LCDC */
    lcdc_shutdown();
#endif
}

