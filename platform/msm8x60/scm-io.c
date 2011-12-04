/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
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

#include <reg.h>
#include <platform/clock.h>
#include <platform/scm-io.h>
#include <platform/iomap.h>

#pragma GCC optimize ("O0")

#define SCM_IO_READ	((((0x5 << 10) | 0x1) << 12) | (0x2 << 8) | 0x1)
#define SCM_IO_WRITE	((((0x5 << 10) | 0x2) << 12) | (0x2 << 8) | 0x2)

#define BETWEEN(p, st, sz) ((p) >= (void *)(st) && \
				(p) < ((void *)(st) + (sz)))

extern void dmb(void);

uint32_t secure_readl(uint32_t c)
{
	if ((BETWEEN((void *)c, MSM_MMSS_CLK_CTL_BASE, MSM_MMSS_CLK_CTL_SIZE))
	    || (BETWEEN((void *)c, MSM_TCSR_BASE, MSM_TCSR_SIZE))) {
		uint32_t context_id;
		register uint32_t r0 __asm__("r0") = SCM_IO_READ;
		register uint32_t r1 __asm__("r1") = (uint32_t) & context_id;
		register uint32_t r2 __asm__("r2") = c;

 __asm__("smc    #0      @ switch to secure world\n":"=r"(r0)
 :			"r"(r0), "r"(r1), "r"(r2)
		    );
		dmb();
		return r0;
	}
	return readl(c);
}

void secure_writel(uint32_t v, uint32_t c)
{
	if ((BETWEEN((void *)c, MSM_MMSS_CLK_CTL_BASE, MSM_MMSS_CLK_CTL_SIZE))
	    || (BETWEEN((void *)c, MSM_TCSR_BASE, MSM_TCSR_SIZE))) {
		uint32_t context_id;
		register uint32_t r0 __asm__("r0") = SCM_IO_WRITE;
		register uint32_t r1 __asm__("r1") = (uint32_t) & context_id;
		register uint32_t r2 __asm__("r2") = c;
		register uint32_t r3 __asm__("r3") = v;
		dmb();
 __asm__("smc    #0      @ switch to secure world\n":	/* No return value */
 :"r"(r0), "r"(r1), "r"(r2), "r"(r3)
		    );
	} else
		writel(v, c);
}
