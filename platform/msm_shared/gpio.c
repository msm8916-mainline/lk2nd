/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
 */

#include <debug.h>
#include <reg.h>
#include <bits.h>
#include <platform/iomap.h>
#include <gpio.h>

static void tlmm_set_pins(struct tlmm_cfgs *cfg)
{
	uint32_t reg_val;

	/* To support backward compatibility for this API
	 * If the reg is not passed then use SDC1 TLMM as
	 * the default value.
	 */
	if (!cfg->reg)
		cfg->reg = SDC1_HDRV_PULL_CTL;

	reg_val = readl(cfg->reg);

	reg_val &= ~(cfg->mask << cfg->off);

	reg_val |= (cfg->val << cfg->off);

	writel(reg_val, cfg->reg);
}

void tlmm_set_hdrive_ctrl(struct tlmm_cfgs *hdrv_cfgs, uint8_t sz)
{
	uint8_t i;

	for (i = 0; i < sz; i++)
		tlmm_set_pins(&hdrv_cfgs[i]);
}

void tlmm_set_pull_ctrl(struct tlmm_cfgs *pull_cfgs, uint8_t sz)
{
	uint8_t i;

	for (i = 0; i < sz; i++)
		tlmm_set_pins(&pull_cfgs[i]);
}
