/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#include <err.h>
#include <msm_panel.h>
#include "splash.h"

/* PANEL INFO */
#define QVGA_PANEL_XRES 240
#define QVGA_PANEL_YRES 320
#define BPP_16 16

/* FB Base Address */
#define QPIC_FB_ADDR  0x06D00000

static struct msm_fb_panel_data panel;

void target_display_init(const char *panel_name)
{
	uint32_t ret = 0;
	dprintf(SPEW, "%s: Panel name = %s\n", __func__, panel_name);

	/* Setting panel info */
	panel.panel_info.xres = QVGA_PANEL_XRES;
	panel.panel_info.yres = QVGA_PANEL_YRES;
	panel.panel_info.bpp = BPP_16;
	panel.panel_info.type = QPIC_PANEL;

	/* Setting FB info */
	panel.fb.width =  panel.panel_info.xres;
	panel.fb.height =  panel.panel_info.yres;
	panel.fb.stride =  panel.panel_info.xres;
	panel.fb.bpp =  panel.panel_info.bpp;
	panel.fb.format = FB_FORMAT_RGB565;
	panel.fb.base = QPIC_FB_ADDR;

	ret = msm_display_init(&panel);
	if (ret)
		dprintf(CRITICAL, "%s: ERROR: Display init failed\n", __func__);
}

void target_display_shutdown(void)
{
	msm_display_off();
}
