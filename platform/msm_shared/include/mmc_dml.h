/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
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

#ifndef __MMC_DML_H__
#define __MMC_DML_H__

#include <platform/iomap.h>

#define SDCC_DML_CONFIG(base)                       (0x00 + base)
#define SDCC_CONSUMER_CRCI_SEL_SHIFT                2
#define SDCC_PRODUCER_CRCI_SEL_SHIFT                0
#define SDCC_PRODUCER_TRANS_END_EN_SHIFT            4
#define SDCC_BYPASS_SHIFT                           16
#define SDCC_DIRECT_MODE_SHIFT                      17
#define SDCC_INFINITE_CONS_TRANS_SHIFT              18

#define SDCC_DML_STATUS(base)                       (0x04 + base)
#define SDCC_DML_PRODUCER_IDLE_SHIFT                0
#define SDCC_DML_CONSUMER_IDLE_SHIFT                16

#define SDCC_DML_SW_RESET(base)                     (0x08 + base)
#define SDCC_DML_PRODUCER_START(base)               (0x0C + base)
#define SDCC_DML_CONSUMER_START(base)               (0x10 + base)
#define SDCC_DML_PRODUCER_PIPE_LOGICAL_SIZE(base)   (0x14 + base)
#define SDCC_DML_CONSUMER_PIPE_LOGICAL_SIZE(base)   (0x18 + base)

#define SDCC_DML_PIPE_ID(base)                      (0x1C + base)
#define SDCC_CONSUMER_PIPE_ID_SHIFT                 16
#define SDCC_PRODUCER_PIPE_ID_SHIFT                 0

#define SDCC_DML_PRODUCER_BAM_BLOCK_SIZE(base)      (0x24 + base)
#define SDCC_DML_PRODUCER_BAM_TRANS_SIZE(base)      (0x28 + base)

#endif
