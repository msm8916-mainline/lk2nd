/* Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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

#ifndef _PLATFORM_MSM8X60_IOMAP_H_
#define _PLATFORM_MSM8X60_IOMAP_H_

#define MSM_UART3_BASE	0xA9C00000

#define MSM_VIC_BASE	0x02080000
#define MSM_TMR_BASE	0x02000000
#define MSM_GPT_BASE    (MSM_TMR_BASE + 0x04)
#define MSM_CSR_BASE    0x02081000
#define MSM_GCC_BASE	0x02082000
#define MSM_ACC0_BASE	0x02041000
#define MSM_ACC1_BASE	0x02051000


#define MSM_GIC_CPU_BASE    0x02081000
#define MSM_GIC_DIST_BASE   0x02080000

#define MSM_SDC1_BASE       0x12400000
#define MMC_BOOT_MCI_BASE   MSM_SDC1_BASE

#if defined(PLATFORM_MSM8X60)
#define MSM_SHARED_BASE      0x00000000
#else
#define MSM_SHARED_BASE      0x01F00000
#endif
#endif
