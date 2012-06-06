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

#include <stdint.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <sys/types.h>
#include <err.h>
#include <reg.h>
#include <debug.h>
#include <target/display.h>
#include <dev/gpio.h>

static char cmd_rotate[4]= {0x36, 0xc0, 0x15, 0x80};

static struct mipi_dsi_cmd nt35510_panel_rotate_cmds[] = {
	{sizeof(cmd_rotate), cmd_rotate},
};


int mipi_nt35510_panel_dsi_config(int on)
{
	if (on) {
		gpio_config(96, GPIO_OUTPUT);
		gpio_set(96, 0x1);

		gpio_config(35, GPIO_OUTPUT);
		gpio_set(35, 0x1);

		gpio_config(40, GPIO_OUTPUT);
		gpio_set(40, 0x1);

		gpio_config(85, GPIO_OUTPUT);
		gpio_set(85, 0x1);
		mdelay(20);
		gpio_set(85, 0x0);
		mdelay(20);
		gpio_set(85, 0x1);
		mdelay(20);
	} else {
		gpio_set(96, 0x0);
		gpio_set(35, 0x0);
		gpio_set(40, 0x0);
	}
	return 0;
}

int mipi_nt35510_panel_wvga_rotate()
{
        int ret = NO_ERROR;

        ret = mipi_dsi_cmds_tx(nt35510_panel_rotate_cmds, ARRAY_SIZE(nt35510_panel_rotate_cmds));

        return ret;
}
