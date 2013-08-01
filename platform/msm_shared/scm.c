/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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
#include <arch/ops.h>
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

	cmd = memalign(CACHE_LINE, ROUNDUP(len, CACHE_LINE));
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
	uint8_t *resp_ptr;

	cmd = alloc_scm_command(cmd_len, resp_len);
	if (!cmd)
		return ERR_NO_MEMORY;

	cmd->id = (svc_id << 10) | cmd_id;
	if (cmd_buf)
		memcpy(scm_get_command_buffer(cmd), cmd_buf, cmd_len);

	/* Flush command to main memory for TZ */
	arch_clean_invalidate_cache_range((addr_t) cmd, cmd->len);

	ret = smc((uint32_t) cmd);
	if (ret)
		goto out;

	if (resp_len) {
		rsp = scm_command_to_response(cmd);

		do
		{
			/* Need to invalidate before each check since TZ will update
			 * the response complete flag in main memory.
			 */
			arch_invalidate_cache_range((addr_t) rsp, sizeof(*rsp));
		} while (!rsp->is_complete);


		resp_ptr = scm_get_response_buffer(rsp);

		/* Invalidate any cached response data */
		arch_invalidate_cache_range((addr_t) resp_ptr, resp_len);

		if (resp_buf)
			memcpy(resp_buf, resp_ptr, resp_len);
	}
 out:
	free_scm_command(cmd);
	return ret;
}

int restore_secure_cfg(uint32_t id)
{
	int ret, scm_ret = 0;
	tz_secure_cfg secure_cfg;

	secure_cfg.id    = id;
	secure_cfg.spare = 0;

	ret = scm_call(SVC_MEMORY_PROTECTION, IOMMU_SECURE_CFG, &secure_cfg, sizeof(secure_cfg),
			&scm_ret, sizeof(scm_ret));

	if (ret || scm_ret) {
		dprintf(CRITICAL, "Secure Config failed\n");
		ret = 1;
	} else
		ret = 0;

	return ret;

}

/* SCM Encrypt Command */
int encrypt_scm(uint32_t ** img_ptr, uint32_t * img_len_ptr)
{
	int ret;
	img_req cmd;

	cmd.img_ptr     = (uint32*) img_ptr;
	cmd.img_len_ptr = img_len_ptr;

	/* Image data is operated upon by TZ, which accesses only the main memory.
	 * It must be flushed/invalidated before and after TZ call.
	 */
	arch_clean_invalidate_cache_range((addr_t) *img_ptr, *img_len_ptr);

	ret = scm_call(SCM_SVC_SSD, SSD_ENCRYPT_ID, &cmd, sizeof(cmd), NULL, 0);

	/* Values at img_ptr and img_len_ptr are updated by TZ. Must be invalidated
	 * before we use them.
	 */
	arch_clean_invalidate_cache_range((addr_t) img_ptr, sizeof(img_ptr));
	arch_clean_invalidate_cache_range((addr_t) img_len_ptr, sizeof(img_len_ptr));

	/* Invalidate the updated image data */
	arch_clean_invalidate_cache_range((addr_t) *img_ptr, *img_len_ptr);

	return ret;
}

/* SCM Decrypt Command */
int decrypt_scm(uint32_t ** img_ptr, uint32_t * img_len_ptr)
{
	int ret;
	img_req cmd;

	cmd.img_ptr     = (uint32*) img_ptr;
	cmd.img_len_ptr = img_len_ptr;

	/* Image data is operated upon by TZ, which accesses only the main memory.
	 * It must be flushed/invalidated before and after TZ call.
	 */
	arch_clean_invalidate_cache_range((addr_t) *img_ptr, *img_len_ptr);

	ret = scm_call(SCM_SVC_SSD, SSD_DECRYPT_ID, &cmd, sizeof(cmd), NULL, 0);

	/* Values at img_ptr and img_len_ptr are updated by TZ. Must be invalidated
	 * before we use them.
	 */
	arch_clean_invalidate_cache_range((addr_t) img_ptr, sizeof(img_ptr));
	arch_clean_invalidate_cache_range((addr_t) img_len_ptr, sizeof(img_len_ptr));

	/* Invalidate the updated image data */
	arch_clean_invalidate_cache_range((addr_t) *img_ptr, *img_len_ptr);

	return ret;
}


static int ssd_image_is_encrypted(uint32_t ** img_ptr, uint32_t * img_len_ptr, uint32 * ctx_id)
{
	int              ret     = 0;
	ssd_parse_md_req parse_req;
	ssd_parse_md_rsp parse_rsp;
	int              prev_len = 0;

	/* Populate meta-data ptr. Here md_len is the meta-data length.
	 * The Code below follows a growing length approach. First send
	 * min(img_len_ptr,SSD_HEADER_MIN_SIZE) say 128 bytes for example.
	 * If parse_rsp.status = PARSING_INCOMPLETE we send md_len = 256.
	 * If subsequent status = PARSING_INCOMPLETE we send md_len = 512,
	 * 1024bytes and so on until we get an valid response(rsp.status) from TZ*/

	parse_req.md     = (uint32*)*img_ptr;
	parse_req.md_len = ((*img_len_ptr) >= SSD_HEADER_MIN_SIZE) ? SSD_HEADER_MIN_SIZE : (*img_len_ptr);

	arch_clean_invalidate_cache_range((addr_t) *img_ptr, parse_req.md_len);

	do
	{
		ret = scm_call(SCM_SVC_SSD,
				SSD_PARSE_MD_ID,
				&parse_req,
				sizeof(parse_req),
				&parse_rsp,
				sizeof(parse_rsp));

		if(!ret && (parse_rsp.status == SSD_PMD_PARSING_INCOMPLETE))
		{
			prev_len          = parse_req.md_len;

			parse_req.md_len *= MULTIPLICATION_FACTOR;

			arch_clean_invalidate_cache_range((addr_t) *(img_ptr + prev_len),
		                                         (parse_req.md_len - prev_len) );

			continue;
		}
		else
			break;

	} while(true);

	if(!ret)
	{
		if(parse_rsp.status == SSD_PMD_ENCRYPTED)
		{
			*ctx_id      = parse_rsp.md_ctx_id;
			*img_len_ptr = *img_len_ptr - ((uint8_t*)parse_rsp.md_end_ptr - (uint8_t*)*img_ptr);
			*img_ptr     = (uint32_t*)parse_rsp.md_end_ptr;
		}

		ret = parse_rsp.status;
	}
	else
	{
		dprintf(CRITICAL,"ssd_image_is_encrypted call failed");

		ASSERT(ret == 0);
	}

	return ret;
}

int decrypt_scm_v2(uint32_t ** img_ptr, uint32_t * img_len_ptr)
{
	int                      ret    = 0;
	uint32                   ctx_id = 0;
	ssd_decrypt_img_frag_req decrypt_req;
	ssd_decrypt_img_frag_rsp decrypt_rsp;

	ret = ssd_image_is_encrypted(img_ptr,img_len_ptr,&ctx_id);
	switch(ret)
	{
		case SSD_PMD_ENCRYPTED:
			/* Image data is operated upon by TZ, which accesses only the main memory.
			* It must be flushed/invalidated before and after TZ call.
			*/

			arch_clean_invalidate_cache_range((addr_t) *img_ptr, *img_len_ptr);

			/*decrypt the image here*/

			decrypt_req.md_ctx_id = ctx_id;
			decrypt_req.last_frag = 1;
			decrypt_req.frag_len  = *img_len_ptr;
			decrypt_req.frag      = *img_ptr;

			ret = scm_call(SCM_SVC_SSD,
					SSD_DECRYPT_IMG_FRAG_ID,
					&decrypt_req,
					sizeof(decrypt_req),
					&decrypt_rsp,
					sizeof(decrypt_rsp));

			if(!ret){
				ret = decrypt_rsp.status;
			}

			/* Values at img_ptr and img_len_ptr are updated by TZ. Must be invalidated
			* before we use them.
			*/
			arch_invalidate_cache_range((addr_t) img_ptr, sizeof(img_ptr));
			arch_invalidate_cache_range((addr_t) img_len_ptr, sizeof(img_len_ptr));

			/* Invalidate the updated image data */
			arch_invalidate_cache_range((addr_t) *img_ptr, *img_len_ptr);

			break;

		case SSD_PMD_NOT_ENCRYPTED:
		case SSD_PMD_NO_MD_FOUND:
			ret = 0;
			break;

		case SSD_PMD_BUSY:
		case SSD_PMD_BAD_MD_PTR_OR_LEN:
		case SSD_PMD_PARSING_INCOMPLETE:
		case SSD_PMD_PARSING_FAILED:
		case SSD_PMD_SETUP_CIPHER_FAILED:
			dprintf(CRITICAL,"decrypt_scm_v2: failed status %d\n",ret);
			break;

		default:
			dprintf(CRITICAL,"decrypt_scm_v2: case default: failed status %d\n",ret);
			break;
	}
	return ret;
}

int scm_svc_version(uint32 * major, uint32 * minor)
{
	feature_version_req feature_req;
	feature_version_rsp feature_rsp;
	int                 ret = 0;

	feature_req.feature_id = TZBSP_FVER_SSD;

	ret = scm_call(TZBSP_SVC_INFO,
		       TZ_INFO_GET_FEATURE_ID,
		       &feature_req,
		       sizeof(feature_req),
		       &feature_rsp,
		       sizeof(feature_rsp));
	if(!ret)
		*major = TZBSP_GET_FEATURE_VERSION(feature_rsp.version);

	return ret;
}

int scm_protect_keystore(uint32_t * img_ptr, uint32_t  img_len)
{
	int                      ret=0;
	ssd_protect_keystore_req protect_req;
	ssd_protect_keystore_rsp protect_rsp;

	protect_req.keystore_ptr = img_ptr;
	protect_req.keystore_len = img_len;

	arch_clean_invalidate_cache_range((addr_t) img_ptr, img_len);

	ret = scm_call(SCM_SVC_SSD,
		       SSD_PROTECT_KEYSTORE_ID,
		       &protect_req,
		       sizeof(protect_req),
		       &protect_rsp,
		       sizeof(protect_rsp));
	if(!ret)
	{
		if(protect_rsp.status == TZBSP_SSD_PKS_SUCCESS)
			dprintf(INFO,"Successfully loaded the keystore ");
		else
		{
			dprintf(INFO,"Loading keystore failed status %d ",protect_rsp.status);
			ret = protect_rsp.status;
		}
	}
	else
	  dprintf(INFO,"scm_call failed ");

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

#define SHA256_DIGEST_LENGTH	(256/8)
/*
 * struct qseecom_save_partition_hash_req
 * @partition_id - partition id.
 * @digest[SHA256_DIGEST_LENGTH] -  sha256 digest.
 */
struct qseecom_save_partition_hash_req {
	uint32_t partition_id; /* in */
	uint8_t digest[SHA256_DIGEST_LENGTH]; /* in */
};


void save_kernel_hash_cmd(void *digest)
{
	uint32_t svc_id;
	uint32_t cmd_id;
	void *cmd_buf;
	size_t cmd_len;
	void *resp_buf = NULL;
	size_t resp_len = 0;
	struct qseecom_save_partition_hash_req req;

	/*no response */
	resp_buf = NULL;
	resp_len = 0;

	req.partition_id = 0; /* kernel */
	memcpy(req.digest, digest, sizeof(req.digest));

	svc_id = SCM_SVC_ES;
	cmd_id = SCM_SAVE_PARTITION_HASH_ID;
	cmd_buf = (void *)&req;
	cmd_len = sizeof(req);

	scm_call(svc_id, cmd_id, cmd_buf, cmd_len, resp_buf, resp_len);
}

/*
 * Switches the CE1 channel between ADM and register usage.
 * channel : AP_CE_REGISTER_USE, CE1 uses register interface
 *         : AP_CE_ADM_USE, CE1 uses ADM interface
 */
uint8_t switch_ce_chn_cmd(enum ap_ce_channel_type channel)
{
	uint32_t svc_id;
	uint32_t cmd_id;
	void *cmd_buf;
	size_t cmd_len;
	size_t resp_len = 0;
	uint8_t resp_buf;

	struct {
		uint32_t resource;
		uint32_t chn_id;
		}__PACKED switch_ce_chn_buf;

	switch_ce_chn_buf.resource = TZ_RESOURCE_CE_AP;
	switch_ce_chn_buf.chn_id = channel;
	cmd_buf = (void *)&switch_ce_chn_buf;
	cmd_len = sizeof(switch_ce_chn_buf);

	/*response */
	resp_len = sizeof(resp_buf);

	svc_id = SCM_SVC_CE_CHN_SWITCH_ID;
	cmd_id = SCM_CE_CHN_SWITCH_ID;

	scm_call(svc_id, cmd_id, cmd_buf, cmd_len, &resp_buf, resp_len);
	return resp_buf;
}

