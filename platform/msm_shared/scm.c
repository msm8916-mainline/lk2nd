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
#include <string.h>
#include <err.h>
#include "scm.h"

#pragma GCC optimize ("O0")

/* From Linux Kernel asm/system.h */
#define __asmeq(x, y)  ".ifnc " x "," y " ; .err ; .endif\n\t"

#ifndef offsetof
#  define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * alloc_scm_command() - Allocate an SCM command
 * @cmd_size: size of the command buffer
 * @resp_size: size of the response buffer
 *
 * Allocate an SCM command, including enough room for the command
 * and response headers as well as the command and response buffers.
 *
 * Returns a valid &scm_command on success or %NULL if the allocation fails.
 */
static struct scm_command *alloc_scm_command(size_t cmd_size, size_t resp_size)
{
	struct scm_command *cmd;
	size_t len = sizeof(*cmd) + sizeof(struct scm_response) + cmd_size +
	    resp_size;

	cmd = malloc(len);
	if (cmd) {
		cmd->len = len;
		cmd->buf_offset = offsetof(struct scm_command, buf);
		cmd->resp_hdr_offset = cmd->buf_offset + cmd_size;
	}
	return cmd;
}

/**
 * free_scm_command() - Free an SCM command
 * @cmd: command to free
 *
 * Free an SCM command.
 */
static inline void free_scm_command(struct scm_command *cmd)
{
	free(cmd);
}

/**
 * scm_command_to_response() - Get a pointer to a scm_response
 * @cmd: command
 *
 * Returns a pointer to a response for a command.
 */
static inline struct scm_response *scm_command_to_response(const struct
							   scm_command *cmd)
{
	return (void *)cmd + cmd->resp_hdr_offset;
}

/**
 * scm_get_command_buffer() - Get a pointer to a command buffer
 * @cmd: command
 *
 * Returns a pointer to the command buffer of a command.
 */
static inline void *scm_get_command_buffer(const struct scm_command *cmd)
{
	return (void *)cmd->buf;
}

/**
 * scm_get_response_buffer() - Get a pointer to a response buffer
 * @rsp: response
 *
 * Returns a pointer to a response buffer of a response.
 */
static inline void *scm_get_response_buffer(const struct scm_response *rsp)
{
	return (void *)rsp + rsp->buf_offset;
}

static uint32_t smc(uint32_t cmd_addr)
{
	uint32_t context_id;
	register uint32_t r0 __asm__("r0") = 1;
	register uint32_t r1 __asm__("r1") = (uint32_t) & context_id;
	register uint32_t r2 __asm__("r2") = cmd_addr;
 __asm__("1:smc	#0	@ switch to secure world\n" "cmp	r0, #1				\n" "beq	1b				\n": "=r"(r0): "r"(r0), "r"(r1), "r"(r2):"r3", "cc");
	return r0;
}

/**
 * scm_call() - Send an SCM command
 * @svc_id: service identifier
 * @cmd_id: command identifier
 * @cmd_buf: command buffer
 * @cmd_len: length of the command buffer
 * @resp_buf: response buffer
 * @resp_len: length of the response buffer
 *
 * Sends a command to the SCM and waits for the command to finish processing.
 */
int
scm_call(uint32_t svc_id, uint32_t cmd_id, const void *cmd_buf,
	 size_t cmd_len, void *resp_buf, size_t resp_len)
{
	int ret;
	struct scm_command *cmd;
	struct scm_response *rsp;

	cmd = alloc_scm_command(cmd_len, resp_len);
	if (!cmd)
		return ERR_NO_MEMORY;

	cmd->id = (svc_id << 10) | cmd_id;
	if (cmd_buf)
		memcpy(scm_get_command_buffer(cmd), cmd_buf, cmd_len);

	ret = smc((uint32_t) cmd);
	if (ret)
		goto out;

	if (resp_len) {
		rsp = scm_command_to_response(cmd);

		while (!rsp->is_complete) ;

		if (resp_buf)
			memcpy(resp_buf, scm_get_response_buffer(rsp),
			       resp_len);
	}
 out:
	free_scm_command(cmd);
	return ret;
}

/* SCM Decrypt Command */
void
setup_decrypt_cmd(decrypt_img_req * dec_cmd,
		  uint32_t ** img_ptr, uint32_t * img_len_ptr)
{
	dec_cmd->common_req.len = sizeof(decrypt_img_req);
	dec_cmd->common_req.buf_offset = sizeof(scm_command);
	dec_cmd->common_req.resp_hdr_offset = 0;
	dec_cmd->common_req.id = SSD_DECRYPT_IMG_ID;

	dec_cmd->img_ptr = img_ptr;
	dec_cmd->img_len_ptr = img_len_ptr;
}

int decrypt_img_scm(uint32_t ** img_ptr, uint32_t * img_len_ptr)
{
	int ret = 0;
	decrypt_img_req decrypt_cmd;

	/* setup the command for decryption */
	setup_decrypt_cmd(&decrypt_cmd, img_ptr, img_len_ptr);

	/* Since TZ cannot access cached data, cmd must be flushed to main memory */
	arch_clean_invalidate_cache_range((addr_t) & decrypt_cmd,
					  sizeof(decrypt_cmd));

	/* Invalidate img ptr and len from cache so that we read the updated data
	 * from the main memory.
	 */
	arch_clean_invalidate_cache_range((addr_t) img_ptr, sizeof(img_ptr));
	arch_clean_invalidate_cache_range((addr_t) img_len_ptr,
					  sizeof(img_len_ptr));

	ret = smc(&decrypt_cmd);

	return ret;
}

void set_tamper_fuse_cmd()
{
	uint32_t svc_id;
	uint32_t cmd_id;
	void *cmd_buf;
	size_t cmd_len;
	void *resp_buf = NULL;
	size_t resp_len = 0;

	uint32_t fuse_id = HLOS_IMG_TAMPER_FUSE;
	cmd_buf = (void *)&fuse_id;
	cmd_len = sizeof(fuse_id);

	/*no response */
	resp_buf = NULL;
	resp_len = 0;

	svc_id = SCM_SVC_FUSE;
	cmd_id = SCM_BLOW_SW_FUSE_ID;

	scm_call(svc_id, cmd_id, cmd_buf, cmd_len, resp_buf, resp_len);
	return;
}

uint8_t get_tamper_fuse_cmd()
{
	uint32_t svc_id;
	uint32_t cmd_id;
	void *cmd_buf;
	size_t cmd_len;
	size_t resp_len = 0;
	uint8_t resp_buf;

	uint32_t fuse_id = HLOS_IMG_TAMPER_FUSE;
	cmd_buf = (void *)&fuse_id;
	cmd_len = sizeof(fuse_id);

	/*response */
	resp_len = sizeof(resp_buf);

	svc_id = SCM_SVC_FUSE;
	cmd_id = SCM_IS_SW_FUSE_BLOWN_ID;

	scm_call(svc_id, cmd_id, cmd_buf, cmd_len, &resp_buf, resp_len);
	return resp_buf;
}
