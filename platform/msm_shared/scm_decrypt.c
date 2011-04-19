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

#include "scm_decrypt.h"

#pragma GCC optimize ("O0")

/* From Linux Kernel asm/system.h */
#define __asmeq(x, y)  ".ifnc " x "," y " ; .err ; .endif\n\t"

void setup_decrypt_cmd ( decrypt_img_req* dec_cmd,
                         uint32_t** img_ptr,
                         uint32_t* img_len_ptr)
{
    dec_cmd->common_req.len = sizeof(decrypt_img_req);
    dec_cmd->common_req.buf_offset = sizeof(scm_command);
    dec_cmd->common_req.resp_hdr_offset = 0;
    dec_cmd->common_req.id = SSD_DECRYPT_IMG_ID;

    dec_cmd->img_ptr = img_ptr;
    dec_cmd->img_len_ptr = img_len_ptr;
}

static uint32_t smc(uint32_t cmd_addr)
{
	uint32_t context_id;
	register uint32_t r0 __asm__("r0") = 1;
	register uint32_t r1 __asm__("r1") = (uint32_t)&context_id;
	register uint32_t r2 __asm__("r2") = cmd_addr;
	__asm__(
		"1:smc	#0	@ switch to secure world\n"
		"cmp	r0, #1				\n"
		"beq	1b				\n"
		: "=r" (r0)
		: "r" (r0), "r" (r1), "r" (r2)
		: "r3", "cc");
	return r0;
}


int decrypt_img_scm(uint32_t** img_ptr, uint32_t* img_len_ptr)
{
   int ret = 0;
   decrypt_img_req *decrypt_cmd;

   /* allocate memory for the command structure */
   /* NEEDS TO BE CONTIGUOUS MEMORY */
   decrypt_cmd = malloc(sizeof(decrypt_img_req));

   /* setup the command for decryption */
   setup_decrypt_cmd(decrypt_cmd, img_ptr, img_len_ptr);

   ret = smc(decrypt_cmd);
   free(decrypt_cmd);
   return ret;
}

