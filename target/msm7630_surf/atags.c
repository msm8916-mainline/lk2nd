/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <reg.h>
#include <debug.h>
#include <smem.h>

#define EBI1_SIZE_60M         0x03C00000
#define EBI1_ADDR_2M          0x00200000
#define EBI0_SIZE_14M         0x00E00000
#define EBI0_ADDR_114M        0x07200000
#define EBI0_SIZE_8M          0x00800000
#define EBI0_ADDR_120M        0x07800000
#define EBI1_SIZE_128M        0x08000000
#define EBI1_ADDR_128M        0x08000000
#define EBI1_ADDR_1G          0x40000000

static int msm7x30_lpddr1 = -1;
static int target_is_msm7x30_lpddr1(void);
int target_is_emmc_boot(void);

int target_is_msm7x30_lpddr1(void)
{
    struct smem_board_info_v4 board_info_v4;
    unsigned int board_info_len = 0;
    unsigned smem_status;
    char *build_type;
    unsigned format = 0;

    if (msm7x30_lpddr1 != -1)
    {
        return msm7x30_lpddr1;
    }

    smem_status = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
					       &format, sizeof(format), 0);
    if(smem_status)
    {
      dprintf(CRITICAL, "ERROR: unable to read shared memory for offset entry\n");
    }

    if ((format == 3) || (format == 4))
    {
        if (format == 4)
	    board_info_len = sizeof(board_info_v4);
	else
	    board_info_len = sizeof(board_info_v4.board_info_v3);

        smem_status = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
					&board_info_v4, board_info_len);
        if(smem_status)
        {
            dprintf(CRITICAL, "ERROR: unable to read shared memory for Hardware Platform\n");
        }
    }

    msm7x30_lpddr1 = 1;

    build_type  = (char *)(board_info_v4.board_info_v3.build_id) + 8;
    if (*build_type == 'A')
    {
        msm7x30_lpddr1 = 0;
    }

    return msm7x30_lpddr1;
}

unsigned* target_atag_mem(unsigned* ptr)
{
    /* ATAG_MEM for 51MB + [6MB if nand boot] + 128MB setup */
    *ptr++ = 4;
    *ptr++ = 0x54410002;
    *ptr++ = EBI1_SIZE_60M;
    *ptr++ = EBI1_ADDR_2M;

    /* Reclaim EFS partition used in EMMC for NAND boot */
    if (target_is_emmc_boot())
    {
        *ptr++ = 4;
        *ptr++ = 0x54410002;
        *ptr++ = EBI0_SIZE_8M;
        *ptr++ = EBI0_ADDR_120M;
    }
    else
    {
        *ptr++ = 4;
        *ptr++ = 0x54410002;
        *ptr++ = EBI0_SIZE_14M;
        *ptr++ = EBI0_ADDR_114M;
    }

    /* ATAG_MEM */
    *ptr++ = 4;
    *ptr++ = 0x54410002;
    *ptr++ = EBI1_SIZE_128M;
    *ptr++ = (target_is_msm7x30_lpddr1()) ? EBI1_ADDR_128M : EBI1_ADDR_1G;

    return ptr;
}

void *target_get_scratch_address(void)
{
    return (void *)((target_is_msm7x30_lpddr1()) ? EBI1_ADDR_128M : EBI1_ADDR_1G);
}
