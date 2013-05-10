/* Copyright (c) 2013, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include <boot_stats.h>
#include <debug.h>
#include <reg.h>
#include <platform/iomap.h>

static uint32_t kernel_load_start;
void bs_set_timestamp(enum bs_entry bs_id)
{
	addr_t bs_imem = get_bs_info_addr();
	uint32_t clk_count = 0;

	if(bs_imem) {
		if (bs_id >= BS_MAX) {
			dprintf(CRITICAL, "bad bs id: %u, max: %u\n", bs_id, BS_MAX);
			ASSERT(0);
		}

		if (bs_id == BS_KERNEL_LOAD_START) {
			kernel_load_start = platform_get_sclk_count();
			return;
		}

		if(bs_id == BS_KERNEL_LOAD_DONE){
			clk_count = platform_get_sclk_count();
			if(clk_count){
				writel(clk_count - kernel_load_start,
					bs_imem + (sizeof(uint32_t) * BS_KERNEL_LOAD_TIME));
			}
		}
		else{
			clk_count = platform_get_sclk_count();
			if(clk_count){
				writel(clk_count,
					bs_imem + (sizeof(uint32_t) * bs_id));
			}
		}
	}
}
