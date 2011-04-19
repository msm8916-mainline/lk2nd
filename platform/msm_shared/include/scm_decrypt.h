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

#include <stdlib.h>

/* 8 Byte SSD magic number (LE) */
#define SSD_HEADER_MAGIC_0     0x73737A74
#define SSD_HEADER_MAGIC_1     0x676D6964
#define SSD_HEADER_MAGIC_SIZE  8
#define SSD_HEADER_XML_SIZE    2048

typedef unsigned int uint32;

typedef struct
{
    uint32  len;
    uint32  buf_offset;
    uint32  resp_hdr_offset;
    uint32  id;
} scm_command;

typedef struct
{
    uint32  len;
    uint32  buf_offset;
    uint32  is_complete;
} scm_response;

typedef struct
{
    scm_command     common_req;
    uint32*         img_ptr;
    uint32*         img_len_ptr;
} decrypt_img_req;

#define SYSCALL_CREATE_CMD_ID(s, f) \
  ((uint32)(((s & 0x3ff) << 10) | (f & 0x3ff)))

#define SCM_SVC_SSD                7
#define SSD_DECRYPT_IMG_ID SYSCALL_CREATE_CMD_ID(SCM_SVC_SSD, 0x01)

void setup_decrypt_cmd ( decrypt_img_req* dec_cmd,
                         uint32** img_ptr,
                         uint32* img_len_ptr);
static uint32 smc(uint32 cmd_addr);
int decrypt_img_scm(uint32** img_ptr, uint32* img_len_ptr);
