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

#define EBI1_SIZE_190M  0x0BE00000
#define EBI1_SIZE_123M  0x07B00000
#define EBI1_SIZE_67M   0x04300000
#define EBI1_ADDR1    	0x00200000
#define EBI1_ADDR2      0x40000000

unsigned* target_atag_mem(unsigned* ptr)
{
    struct smem_board_info board_info;
    unsigned int board_info_struct_len = sizeof(board_info);
    unsigned smem_status;
    char *build_type;
    int enable_lpddr2 = 0;

    smem_status = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
					&board_info, board_info_struct_len );
    if(smem_status)
    {
      dprintf(CRITICAL, "ERROR: unable to read shared memory for build id\n");
    }

    build_type  = (char *)(board_info.build_id) + 8;
    if (*build_type == 'A')
    {
        enable_lpddr2 = 1;
    }

    if(enable_lpddr2)
    {
	/* ATAG_MEM for 123MB + 67MB setup */
	*ptr++ = 4;
	*ptr++ = 0x54410002;
	*ptr++ = EBI1_SIZE_123M;
	*ptr++ = EBI1_ADDR1;

	/* ATAG_MEM */
	*ptr++ = 4;
	*ptr++ = 0x54410002;
	*ptr++ = EBI1_SIZE_67M;
	*ptr++ = EBI1_ADDR2;
    }
    else
    {
	/* ATAG_MEM for 190MB setup*/
	*ptr++ = 4;
	*ptr++ = 0x54410002;
	*ptr++ = EBI1_SIZE_190M;
	*ptr++ = EBI1_ADDR1;
    }
    return ptr;
}

