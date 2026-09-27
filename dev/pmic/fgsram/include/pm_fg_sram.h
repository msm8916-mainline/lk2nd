/* Copyright (c) 2015, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation, nor the names of its
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
#ifndef _PM_FG_SRAM_H_
#define _PM_FG_SRAM_H_

#include <sys/types.h>

#define FG_MEMIF_INT_RT_STS             0x4410
#define FG_MEMIF_MEM_INTF_CFG           0x4450
#define FG_MEMIF_MEM_INTF_CTL           0x4451
#define FG_MEMIF_IMA_CFG                0x4452
#define FG_MEMIF_IMA_OPERATION_STS      0x4454
#define FG_MEMIF_IMA_EXCEPTION_STS      0x4455
#define FG_MEMIF_MEM_INTF_ADDR_LSB      0x4461
#define FG_MEMIF_MEM_INTF_ADDR_MSB      0x4462
#define FG_MEMIF_MEM_INTF_WR_DATA0      0x4463
#define FG_MEMIF_MEM_INTF_WR_DATA1      0x4464
#define FG_MEMIF_MEM_INTF_WR_DATA2      0x4465
#define FG_MEMIF_MEM_INTF_WR_DATA3      0x4466
#define FG_MEMIF_MEM_INTF_RD_DATA0      0x4467
#define FG_MEMIF_MEM_INTF_RD_DATA1      0x4468
#define FG_MEMIF_MEM_INTF_RD_DATA2      0x4469
#define FG_MEMIF_MEM_INTF_RD_DATA3      0x446A
#define FG_MEMIF_FG_BEAT_COUNT          0x4457
#define IACS_SLCT                       BIT(5)
#define RIF_MEM_ACCESS_REQ              BIT(7)
#define IACS_RDY                        BIT(1)
#define IACS_CLR                        BIT(2)
#define BEAT_COUNT_MASK                 0x0F
#define FG_MAX_TRY                      10
#define FG_DATA_MASK                    0xffffffff
#define FG_DATA_MAX_LEN                 4

int pmi_fg_sram_read(uint32_t addr, uint32_t *data,int sid, uint8_t offset, uint8_t len);

#endif
