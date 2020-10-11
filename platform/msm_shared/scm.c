/* Copyright (c) 2011-2015, The Linux Foundation. All rights reserved.
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
#include <asm.h>
#include <bits.h>
#include <arch/ops.h>
#include <dload_util.h>
#include "scm.h"

#pragma GCC optimize ("O0")

/* From Linux Kernel asm/system.h */
#define __asmeq(x, y)  ".ifnc " x "," y " ; .err ; .endif\n\t"

#ifndef offsetof
#  define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define SCM_CLASS_REGISTER         (0x2 << 8)
#define SCM_MASK_IRQS              BIT(5)
#define SCM_ATOMIC(svc, cmd, n)    ((((((svc) & 0x3f) << 10)|((cmd) & 0x3ff)) << 12) | \
                                   SCM_CLASS_REGISTER | \
                                   SCM_MASK_IRQS | \
                                   ((n) & 0xf))

/* SCM interface as per ARM spec present? */
bool scm_arm_support;
static uint32_t scm_io_write(addr_t address, uint32_t val);

bool is_scm_armv8_support()
{
	return scm_arm_support;
}

int is_scm_call_available(uint32_t svc_id, uint32_t cmd_id)
{
	uint32_t ret;
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_INFO, IS_CALL_AVAIL_CMD);
	scm_arg.x1 = MAKE_SCM_ARGS(0x1);
	scm_arg.x2 = MAKE_SIP_SCM_CMD(svc_id, cmd_id);

	ret = scm_call2(&scm_arg, &scm_ret);

	if (!ret)
		return scm_ret.x1;

	return ret;
}

static int scm_arm_support_available(uint32_t svc_id, uint32_t cmd_id)
{
	int ret;

	ret = is_scm_call_available(SCM_SVC_INFO, IS_CALL_AVAIL_CMD);

	if (ret > 0)
		scm_arm_support = true;

	return ret;
}

void scm_init()
{
	int ret;

	ret = scm_arm_support_available(SCM_SVC_INFO, IS_CALL_AVAIL_CMD);

	if (ret)
		dprintf(CRITICAL, "Failed to initialize SCM\n");
}

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
		memset(cmd, 0, len);
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
* scm_call_automic: Make scm call with one or no argument
* @svc: service id
* @cmd: command id
* @ arg1: argument
*/

static int scm_call_atomic(uint32_t svc, uint32_t cmd, uint32_t arg1)
{
	uint32_t context_id;
	register uint32_t r0 __asm__("r0") = SCM_ATOMIC(svc, cmd, 1);
	register uint32_t r1 __asm__("r1") = &context_id;
	register uint32_t r2 __asm__("r2") = arg1;

	__asm__ volatile(
		__asmeq("%0", "r0")
		__asmeq("%1", "r0")
		__asmeq("%2", "r1")
		__asmeq("%3", "r2")
		"smc    #0  @ switch to secure world\n"
		: "=r" (r0)
		: "r" (r0), "r" (r1), "r" (r2)
		: "r3");
	return r0;
}

/**
 * scm_call_atomic2() - Send an atomic SCM command with two arguments
 * @svc_id: service identifier
 * @cmd_id: command identifier
 * @arg1: first argument
 * @arg2: second argument
 *
 * This shall only be used with commands that are guaranteed to be
 * uninterruptable, atomic and SMP safe.
 */
int scm_call_atomic2(uint32_t svc, uint32_t cmd, uint32_t arg1, uint32_t arg2)
{
	int context_id;
	register uint32_t r0 __asm__("r0") = SCM_ATOMIC(svc, cmd, 2);
	register uint32_t r1 __asm__("r1") = &context_id;
	register uint32_t r2 __asm__("r2") = arg1;
	register uint32_t r3 __asm__("r3") = arg2;

	__asm__ volatile(
		__asmeq("%0", "r0")
		__asmeq("%1", "r0")
		__asmeq("%2", "r1")
		__asmeq("%3", "r2")
		__asmeq("%4", "r3")
		"smc	#0	@ switch to secure world\n"
		: "=r" (r0)
		: "r" (r0), "r" (r1), "r" (r2), "r" (r3));
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
	int ret = 0;
	tz_secure_cfg secure_cfg;

	secure_cfg.id    = id;
	secure_cfg.spare = 0;
	scmcall_arg scm_arg = {0};

	if(!scm_arm_support)
	{
		ret = scm_call(SVC_MEMORY_PROTECTION, IOMMU_SECURE_CFG, &secure_cfg, sizeof(secure_cfg),
					   NULL, 0);
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SVC_MEMORY_PROTECTION, IOMMU_SECURE_CFG);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2);
		scm_arg.x2 = id;
		scm_arg.x3 = 0x0; /* Spare unused */

		ret = scm_call2(&scm_arg, NULL);
	}

	if (ret)
	{
		dprintf(CRITICAL, "Secure Config failed\n");
		ret = 1;
	}

	return ret;
}

/* SCM Encrypt Command */
int encrypt_scm(uint32_t ** img_ptr, uint32_t * img_len_ptr)
{
	int ret;
	img_req cmd;
	scmcall_arg scm_arg = {0};


	cmd.img_ptr     = (uint32*) img_ptr;
	cmd.img_len_ptr = img_len_ptr;

	/* Image data is operated upon by TZ, which accesses only the main memory.
	 * It must be flushed/invalidated before and after TZ call.
	 */
	arch_clean_invalidate_cache_range((addr_t) *img_ptr, *img_len_ptr);

	if (!scm_arm_support)
	{
		ret = scm_call(SCM_SVC_SSD, SSD_ENCRYPT_ID, &cmd, sizeof(cmd), NULL, 0);
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_SSD,SSD_ENCRYPT_ID);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2,SMC_PARAM_TYPE_BUFFER_READWRITE,SMC_PARAM_TYPE_BUFFER_READWRITE);
		scm_arg.x2 = (uint32_t) cmd.img_ptr;
		scm_arg.x3 = (uint32_t) cmd.img_len_ptr;

		ret = scm_call2(&scm_arg, NULL);
	}

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

	if (scm_arm_support)
	{
		dprintf(INFO, "%s:SCM call is not supported\n",__func__);
		return -1;
	}

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
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};
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
		if (!scm_arm_support)
		{
			ret = scm_call(SCM_SVC_SSD,
					SSD_PARSE_MD_ID,
					&parse_req,
					sizeof(parse_req),
					&parse_rsp,
					sizeof(parse_rsp));
		}
		else
		{
			scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_SSD, SSD_PARSE_MD_ID);
			scm_arg.x1 = MAKE_SCM_ARGS(0x2,SMC_PARAM_TYPE_VALUE,SMC_PARAM_TYPE_BUFFER_READWRITE);
			scm_arg.x2 = parse_req.md_len;
			scm_arg.x3 = parse_req.md;
			scm_arg.atomic = true;

			ret = scm_call2(&scm_arg, &scm_ret);
			parse_rsp.status = scm_ret.x1;
		}
		if(!ret && (parse_rsp.status == SSD_PMD_PARSING_INCOMPLETE))
		{
			prev_len          = parse_req.md_len;

			parse_req.md_len *= MULTIPLICATION_FACTOR;

			arch_clean_invalidate_cache_range((addr_t) (*img_ptr + prev_len),
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
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};


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

			if (!scm_arm_support)
			{
				ret = scm_call(SCM_SVC_SSD,
						SSD_DECRYPT_IMG_FRAG_ID,
						&decrypt_req,
						sizeof(decrypt_req),
						&decrypt_rsp,
						sizeof(decrypt_rsp));
			}
			else
			{
				scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_SSD, SSD_DECRYPT_IMG_FRAG_ID);
				scm_arg.x1 = MAKE_SCM_ARGS(0x4,SMC_PARAM_TYPE_VALUE,SMC_PARAM_TYPE_VALUE,SMC_PARAM_TYPE_VALUE,SMC_PARAM_TYPE_BUFFER_READWRITE);
				scm_arg.x2 = decrypt_req.md_ctx_id;
				scm_arg.x3 = decrypt_req.last_frag;
				scm_arg.x4 = decrypt_req.frag_len;
				scm_arg.x5[0] = decrypt_req.frag;

				ret = scm_call2(&scm_arg, &scm_ret);
				decrypt_rsp.status = scm_ret.x1;
			}
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
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	feature_req.feature_id = TZBSP_FVER_SSD;

	if (!scm_arm_support)
	{
		ret = scm_call(TZBSP_SVC_INFO,
					   TZ_INFO_GET_FEATURE_ID,
					   &feature_req,
					   sizeof(feature_req),
					   &feature_rsp,
					   sizeof(feature_rsp));
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(TZBSP_SVC_INFO, TZ_INFO_GET_FEATURE_ID);
		scm_arg.x1 = MAKE_SCM_ARGS(0x1,SMC_PARAM_TYPE_VALUE);
		scm_arg.x2 = feature_req.feature_id;

		ret = scm_call2(&scm_arg, &scm_ret);
		feature_rsp.version = scm_ret.x1;
	}

	if(!ret)
		*major = TZBSP_GET_FEATURE_VERSION(feature_rsp.version);

	return ret;
}

int scm_svc_get_secure_state(uint32_t *state_low, uint32_t *state_high)
{
	get_secure_state_req req;
	get_secure_state_rsp rsp;

	int ret = 0;

	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	if (!scm_arm_support)
	{
		req.status_ptr = (uint32_t*)&rsp;
		req.status_len = sizeof(rsp);

		ret = scm_call(TZBSP_SVC_INFO,
					   TZ_INFO_GET_SECURE_STATE,
					   &req,
					   sizeof(req),
					   NULL,
					   0);
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(TZBSP_SVC_INFO, TZ_INFO_GET_SECURE_STATE);
		scm_arg.x1 = MAKE_SCM_ARGS(0x0);

		ret = scm_call2(&scm_arg, &scm_ret);

		rsp.status_low = scm_ret.x1;
		rsp.status_high = scm_ret.x2;
	}

	if(!ret)
	{
		*state_low = rsp.status_low;
		*state_high = rsp.status_high;
	}

	return ret;
}

int scm_protect_keystore(uint32_t * img_ptr, uint32_t  img_len)
{
	int                      ret=0;
	ssd_protect_keystore_req protect_req;
	ssd_protect_keystore_rsp protect_rsp;
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	protect_req.keystore_ptr = img_ptr;
	protect_req.keystore_len = img_len;

	arch_clean_invalidate_cache_range((addr_t) img_ptr, img_len);

	if (!scm_arm_support)
	{
		ret = scm_call(SCM_SVC_SSD,
				SSD_PROTECT_KEYSTORE_ID,
				&protect_req,
				sizeof(protect_req),
				&protect_rsp,
				sizeof(protect_rsp));
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_SSD, SSD_PROTECT_KEYSTORE_ID);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2,SMC_PARAM_TYPE_BUFFER_READWRITE,SMC_PARAM_TYPE_VALUE);
		scm_arg.x2 = protect_req.keystore_ptr;
		scm_arg.x3 = protect_req.keystore_len;

		ret = scm_call2(&scm_arg, &scm_ret);
		protect_rsp.status = scm_ret.x1;
	}
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
	scmcall_arg scm_arg = {0};

	uint32_t fuse_id = HLOS_IMG_TAMPER_FUSE;
	cmd_buf = (void *)&fuse_id;
	cmd_len = sizeof(fuse_id);

	if (!scm_arm_support)
	{
		/*no response */
		resp_buf = NULL;
		resp_len = 0;

		svc_id = SCM_SVC_FUSE;
		cmd_id = SCM_BLOW_SW_FUSE_ID;

		scm_call(svc_id, cmd_id, cmd_buf, cmd_len, resp_buf, resp_len);
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_FUSE, SCM_BLOW_SW_FUSE_ID);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2,SMC_PARAM_TYPE_BUFFER_READWRITE,SMC_PARAM_TYPE_VALUE);
		scm_arg.x2  = cmd_buf;
		scm_arg.x3 = cmd_len;

		scm_call2(&scm_arg, NULL);

	}

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
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	cmd_buf = (void *)&fuse_id;
	cmd_len = sizeof(fuse_id);

	if (!scm_arm_support)
	{
		/*response */
		resp_len = sizeof(resp_buf);

		svc_id = SCM_SVC_FUSE;
		cmd_id = SCM_IS_SW_FUSE_BLOWN_ID;

		scm_call(svc_id, cmd_id, cmd_buf, cmd_len, &resp_buf, resp_len);
		return resp_buf;
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_FUSE, SCM_IS_SW_FUSE_BLOWN_ID);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2,SMC_PARAM_TYPE_BUFFER_READWRITE,SMC_PARAM_TYPE_VALUE);
		scm_arg.x2  = cmd_buf;
		scm_arg.x3 = cmd_len;

		scm_call2(&scm_arg, &scm_ret);
		return (uint8_t)scm_ret.x1;
	}
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
	scmcall_arg scm_arg = {0};

	/*no response */
	resp_buf = NULL;
	resp_len = 0;

	req.partition_id = 0; /* kernel */
	memcpy(req.digest, digest, sizeof(req.digest));

	if (!scm_arm_support)
	{
		svc_id = SCM_SVC_ES;
		cmd_id = SCM_SAVE_PARTITION_HASH_ID;
		cmd_buf = (void *)&req;
		cmd_len = sizeof(req);

		scm_call(svc_id, cmd_id, cmd_buf, cmd_len, resp_buf, resp_len);
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_ES, SCM_SAVE_PARTITION_HASH_ID);
		scm_arg.x1 = MAKE_SCM_ARGS(0x3, 0, SMC_PARAM_TYPE_BUFFER_READWRITE);
		scm_arg.x2 = req.partition_id;
		scm_arg.x3 = (uint8_t *)&req.digest;
		scm_arg.x4 = sizeof(req.digest);

		if (scm_call2(&scm_arg, NULL))
			dprintf(CRITICAL, "Failed to Save kernel hash\n");
	}
}

int mdtp_cipher_dip_cmd(uint8_t *in_buf, uint32_t in_buf_size, uint8_t *out_buf,
                          uint32_t out_buf_size, uint32_t direction)
{
	uint32_t svc_id;
	uint32_t cmd_id;
	void *cmd_buf;
	void *rsp_buf;
	size_t cmd_len;
	size_t rsp_len;
	mdtp_cipher_dip_req req;
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	ASSERT(in_buf != NULL);
	ASSERT(out_buf != NULL);

	req.in_buf = in_buf;
	req.in_buf_size = in_buf_size;
	req.out_buf = out_buf;
	req.out_buf_size = out_buf_size;
	req.direction = direction;

	if (!scm_arm_support)
	{
		svc_id = SCM_SVC_MDTP;
		cmd_id = SCM_MDTP_CIPHER_DIP;
		cmd_buf = (void *)&req;
		cmd_len = sizeof(req);
		rsp_buf = NULL;
		rsp_len = 0;

		if (scm_call(svc_id, cmd_id, cmd_buf, cmd_len, rsp_buf, rsp_len))
		{
			dprintf(CRITICAL, "Failed to call Cipher DIP SCM\n");
			return -1;
		}
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_MDTP, SCM_MDTP_CIPHER_DIP);
		scm_arg.x1 = MAKE_SCM_ARGS(0x5, SMC_PARAM_TYPE_BUFFER_READ, SMC_PARAM_TYPE_VALUE,
										SMC_PARAM_TYPE_BUFFER_READWRITE, SMC_PARAM_TYPE_VALUE, SMC_PARAM_TYPE_VALUE);
		scm_arg.x2 = (uint32_t)req.in_buf;
		scm_arg.x3 = req.in_buf_size;
		scm_arg.x4 = (uint32_t)req.out_buf;
		scm_arg.x5[0] = req.out_buf_size;
		scm_arg.x5[1] = req.direction;

		if (scm_call2(&scm_arg, &scm_ret))
		{
			dprintf(CRITICAL, "Failed in Cipher DIP SCM call\n");
			return -1;
		}
	}

	return 0;
}

int qfprom_read_row_cmd(uint32_t row_address,
                        uint32_t addr_type,
                        uint32_t *row_data,
                        uint32_t *qfprom_api_status)
{
	uint32_t svc_id;
	uint32_t cmd_id;
	void *cmd_buf;
	void *rsp_buf;
	size_t cmd_len;
	size_t rsp_len;
	qfprom_read_row_req req;
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	req.row_address = row_address;
	req.addr_type = addr_type;
	req.row_data = row_data;
	req.qfprom_api_status = qfprom_api_status;

	if (!scm_arm_support)
	{
		svc_id = SCM_SVC_FUSE;
		cmd_id = SCM_QFPROM_READ_ROW_ID;
		cmd_buf = (void *)&req;
		cmd_len = sizeof(req);
		rsp_buf = NULL;
		rsp_len = 0;

		if (scm_call(svc_id, cmd_id, cmd_buf, cmd_len, rsp_buf, rsp_len))
		{
			dprintf(CRITICAL, "Failed to call SCM_SVC_FUSE.SCM_QFPROM_READ_ROW_ID SCM\n");
			return -1;
		}
	}

	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_FUSE, SCM_QFPROM_READ_ROW_ID);
		scm_arg.x1 = MAKE_SCM_ARGS(0x4, SMC_PARAM_TYPE_VALUE, SMC_PARAM_TYPE_VALUE,
										SMC_PARAM_TYPE_BUFFER_READWRITE, SMC_PARAM_TYPE_BUFFER_READWRITE);
		scm_arg.x2 = req.row_address;
		scm_arg.x3 = req.addr_type;
		scm_arg.x4 = (uint32_t)req.row_data;
		scm_arg.x5[0] = (uint32_t)req.qfprom_api_status;

		if (scm_call2(&scm_arg, &scm_ret))
		{
			dprintf(CRITICAL, "Failed to call SCM_SVC_FUSE.SCM_QFPROM_READ_ROW_ID SCM\n");
			return -1;
		}
	}

	return 0;
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

	if (scm_arm_support)
	{
		dprintf(INFO, "%s:SCM call is not supported\n",__func__);
		return 0;
	}

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

int scm_halt_pmic_arbiter()
{
	int ret = 0;
	scmcall_arg scm_arg = {0};

	if (scm_arm_support) {
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_PWR, SCM_IO_DISABLE_PMIC_ARBITER);
		scm_arg.x1 = MAKE_SCM_ARGS(0x1);
		scm_arg.x2 = 0;
		scm_arg.atomic = true;
		ret = scm_call2(&scm_arg, NULL);
	} else {
		ret = scm_call_atomic(SCM_SVC_PWR, SCM_IO_DISABLE_PMIC_ARBITER, 0);
	}

	/* Retry with the SCM_IO_DISABLE_PMIC_ARBITER1 func ID if the above Func ID fails*/
	if(ret) {
		if (scm_arm_support) {
			scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_PWR, SCM_IO_DISABLE_PMIC_ARBITER1);
			scm_arg.x1 = MAKE_SCM_ARGS(0x1);
			scm_arg.x2 = 0;
			scm_arg.atomic = true;
			ret = scm_call2(&scm_arg, NULL);
		} else
			ret = scm_call_atomic(SCM_SVC_PWR, SCM_IO_DISABLE_PMIC_ARBITER1, 0);
	}
	return ret;
}

/* Execption Level exec secure-os call
 * Jumps to kernel via secure-os and does not return
 * on successful jump. System parameters are setup &
 * passed on to secure-os and are utilized to boot the
 * kernel.
 *
 @ kernel_entry	: kernel entry point passed in as link register.
 @ dtb_offset	: dt blob address passed in as w0.
 @ svc_id	: indicates direction of switch 32->64 or 64->32
 *
 * Assumes all sanity checks have been performed on arguments.
 */

void scm_elexec_call(paddr_t kernel_entry, paddr_t dtb_offset)
{
	uint32_t svc_id = SCM_SVC_MILESTONE_32_64_ID;
	uint32_t cmd_id = SCM_SVC_MILESTONE_CMD_ID;
	void *cmd_buf;
	size_t cmd_len;
	static el1_system_param param;
	scmcall_arg scm_arg = {0};

	param.el1_x0 = dtb_offset;
	param.el1_elr = kernel_entry;

	/* Response Buffer = Null as no response expected */
	dprintf(INFO, "Jumping to kernel via monitor\n");

	if (!scm_arm_support)
	{
		/* Command Buffer */
		cmd_buf = (void *)&param;
		cmd_len = sizeof(el1_system_param);

		scm_call(svc_id, cmd_id, cmd_buf, cmd_len, NULL, 0);
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_MILESTONE_32_64_ID, SCM_SVC_MILESTONE_CMD_ID);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2, SMC_PARAM_TYPE_BUFFER_READ);
		scm_arg.x2 = (void *)&param;
		scm_arg.x3 = sizeof(el1_system_param);

		scm_call2(&scm_arg, NULL);
	}

	/* Assert if execution ever reaches here */
	dprintf(CRITICAL, "Failed to jump to kernel\n");
	ASSERT(0);
}

/* SCM Random Command */
int scm_random(uintptr_t * rbuf, uint32_t  r_len)
{
	int ret;
	struct tz_prng_data data;
	scmcall_arg scm_arg = {0};
	// Memory passed to TZ should be algined to cache line
	BUF_DMA_ALIGN(rand_buf, sizeof(uintptr_t));

	if (!scm_arm_support)
	{
		data.out_buf     = (uint8_t*) rand_buf;
		data.out_buf_size = r_len;

		/*
		 * random buffer must be flushed/invalidated before and after TZ call.
		 */
		arch_clean_invalidate_cache_range((addr_t) rand_buf, r_len);

		ret = scm_call(TZ_SVC_CRYPTO, PRNG_CMD_ID, &data, sizeof(data), NULL, 0);

		/* Invalidate the updated random buffer */
		arch_clean_invalidate_cache_range((addr_t) rand_buf, r_len);
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(TZ_SVC_CRYPTO, PRNG_CMD_ID);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2,SMC_PARAM_TYPE_BUFFER_READWRITE);
		scm_arg.x2 = (uint32_t) rand_buf;
		scm_arg.x3 = r_len;

		arch_clean_invalidate_cache_range((addr_t) rand_buf, r_len);

		ret = scm_call2(&scm_arg, NULL);
		if (!ret)
			arch_clean_invalidate_cache_range((addr_t) rand_buf, r_len);
		else
			dprintf(CRITICAL, "Secure canary SCM failed: %x\n", ret);
	}

	//Copy back into the return buffer
	memscpy(rbuf, r_len, rand_buf, sizeof(rand_buf));
	return ret;
}

uintptr_t get_canary()
{
	uintptr_t canary;
	if(scm_random(&canary, sizeof(canary))) {
		dprintf(CRITICAL,"scm_call for random failed !!!");
		/*
		* fall back to use lib rand API if scm call failed.
		*/
		canary =  rand();
	}

	return canary;
}

int scm_xpu_err_fatal_init()
{
	uint32_t ret = 0;
	uint32_t response = 0;
	tz_xpu_prot_cmd cmd;
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	if (!scm_arm_support)
	{
		cmd.config = ERR_FATAL_ENABLE;
		cmd.spare = 0;

		ret = scm_call(SVC_MEMORY_PROTECTION, XPU_ERR_FATAL, &cmd, sizeof(cmd), &response,
					   sizeof(response));
	}
	else
	{
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SVC_MEMORY_PROTECTION, XPU_ERR_FATAL);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2);
		scm_arg.x2 = ERR_FATAL_ENABLE;
		scm_arg.x3 = 0x0;
		ret =  scm_call2(&scm_arg, &scm_ret);
		response = scm_ret.x1;
	}

	if (ret)
		dprintf(CRITICAL, "Failed to set XPU violations as fatal errors: %u\n", response);
	else
		dprintf(INFO, "Configured XPU violations to be fatal errors\n");

	return ret;
}

static uint32_t scm_call_a32(uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3, uint32_t x4, uint32_t x5, scmcall_ret *ret)
{
	register uint32_t r0 __asm__("r0") = x0;
	register uint32_t r1 __asm__("r1") = x1;
	register uint32_t r2 __asm__("r2") = x2;
	register uint32_t r3 __asm__("r3") = x3;
	register uint32_t r4 __asm__("r4") = x4;
	register uint32_t r5 __asm__("r5") = x5;

	do {
		__asm__ volatile(
			__asmeq("%0", "r0")
			__asmeq("%1", "r1")
			__asmeq("%2", "r2")
			__asmeq("%3", "r3")
			__asmeq("%4", "r0")
			__asmeq("%5", "r1")
			__asmeq("%6", "r2")
			__asmeq("%7", "r3")
			__asmeq("%8", "r4")
			__asmeq("%9", "r5")
			"smc    #0  @ switch to secure world\n"
			: "=r" (r0), "=r" (r1), "=r" (r2), "=r" (r3)
			: "r" (r0), "r" (r1), "r" (r2), "r" (r3), "r" (r4), "r" (r5));
	} while(r0 == 1);

	if (ret)
	{
		ret->x1 = r1;
		ret->x2 = r2;
		ret->x3 = r3;
	}

	return r0;
}

uint32_t scm_call2(scmcall_arg *arg, scmcall_ret *ret)
{
	uint32_t *indir_arg = NULL;
	uint32_t x5;
	int i;
	uint32_t rc;

	arg->x0 = arg->atomic ? (arg->x0 | SCM_ATOMIC_BIT) : arg->x0;
	x5 = arg->x5[0];

	if ((arg->x1 & 0xF) > SCM_MAX_ARG_LEN - 1)
	{
		indir_arg = memalign(CACHE_LINE, ROUNDUP((SCM_INDIR_MAX_LEN * sizeof(uint32_t)), CACHE_LINE));
		ASSERT(indir_arg);

		for (i = 0 ; i < SCM_INDIR_MAX_LEN; i++)
		{
			indir_arg[i] = arg->x5[i];
		}
		arch_clean_invalidate_cache_range((addr_t) indir_arg, ROUNDUP((SCM_INDIR_MAX_LEN * sizeof(uint32_t)), CACHE_LINE));
		x5 = (addr_t) indir_arg;
	}

	rc = scm_call_a32(arg->x0, arg->x1, arg->x2, arg->x3, arg->x4, x5, ret);

	if (rc)
	{
		dprintf(CRITICAL, "SCM call: 0x%x failed with :%x\n", arg->x0, rc);
		return rc;
	}

	if (indir_arg)
		free(indir_arg);

	return 0;
}

uint32_t is_secure_boot_enable()
{
	uint32_t ret = 0;
	uint32_t *resp = NULL;
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	resp =  memalign(CACHE_LINE,(2 *sizeof(uint32_t)));
	ASSERT(resp);
	if (!scm_arm_support) {
		ret = scm_call_atomic2(TZBSP_SVC_INFO, IS_SECURE_BOOT_ENABLED, resp, (2 * sizeof(uint32_t)));
	} else {
		scm_arg.x0 = MAKE_SIP_SCM_CMD(TZBSP_SVC_INFO, IS_SECURE_BOOT_ENABLED);
		ret = scm_call2(&scm_arg, &scm_ret);
		resp[0] = scm_ret.x1;
	}

	/* Invalidate the resp buffer */
	arch_clean_invalidate_cache_range((addr_t) resp, ROUNDUP((2 * sizeof(uint32_t)), CACHE_LINE));

	/* Parse Bit 0 and Bit 2 of the response
	* Bit 0 - SECBOOT_ENABLE_CHECK
	* Bit 2 - DEBUG_DISABLE_CHECK
	*/
	if(!ret) {
		if(!((resp[0] & 0x1) && (resp[0] & 0x4)))
			ret = 1;
	} else
		dprintf(CRITICAL, "scm call is_secure_boot_enable failed\n");
	free(resp);
	return ret;
}

static uint32_t scm_io_read(addr_t address)
{
	uint32_t ret;
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	if (!scm_arm_support) {
		ret = scm_call_atomic(SCM_SVC_IO, SCM_IO_READ, address);
	} else {
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_IO, SCM_IO_READ);
		scm_arg.x1 = MAKE_SCM_ARGS(0x1);
		scm_arg.x2 = address;
		scm_arg.atomic = true;
		ret = scm_call2(&scm_arg, &scm_ret);
	}
	return ret;
}

static uint32_t scm_io_write(addr_t address, uint32_t val)
{
	uint32_t ret;
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	if (!scm_arm_support) {
		ret = scm_call_atomic2(SCM_SVC_IO, SCM_IO_WRITE, address, val);
	} else {
		scm_arg.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_IO, SCM_IO_WRITE);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2);
		scm_arg.x2 = address;
		scm_arg.x3 = val;
		scm_arg.atomic = true;
		ret = scm_call2(&scm_arg, &scm_ret);
	}
	return ret;
}

static int scm_call2_atomic(uint32_t svc, uint32_t cmd, uint32_t arg1, uint32_t arg2)
{
	uint32_t ret = 0;
	scmcall_arg scm_arg = {0};
	scmcall_ret scm_ret = {0};

	if (!scm_arm_support)
	{
		ret = scm_call_atomic2(svc, cmd, arg1, arg2);
	} else {
		scm_arg.x0 = MAKE_SIP_SCM_CMD(svc, cmd);
		scm_arg.x1 = MAKE_SCM_ARGS(0x2);
		scm_arg.x2 = arg1;
		scm_arg.x3 = arg2;
		ret =  scm_call2(&scm_arg, &scm_ret);
	}
	return ret;
}

int scm_dload_mode(int mode)
{
	int ret = 0;
	uint32_t dload_type;

	dprintf(SPEW, "DLOAD mode: %d\n", mode);
	if (mode == NORMAL_DLOAD)
		dload_type = SCM_DLOAD_MODE;
	else if(mode == EMERGENCY_DLOAD)
		dload_type = SCM_EDLOAD_MODE;
	else
		dload_type = 0;

	/* Write to the Boot MISC register */
	ret = is_scm_call_available(SCM_SVC_BOOT, SCM_DLOAD_CMD);

	if (ret > 0)
		ret = scm_call2_atomic(SCM_SVC_BOOT, SCM_DLOAD_CMD, dload_type, 0);
	else
#ifdef TCSR_BOOT_MISC_DETECT
		ret = scm_io_write(TCSR_BOOT_MISC_DETECT,dload_type);
#else
		ret = 1;
#endif

	if(ret) {
		dprintf(CRITICAL, "Failed to write to boot misc: %d\n", ret);
		return ret;
	}

	/* Make WDOG_DEBUG DISABLE scm call only in non-secure boot */
	if(!is_secure_boot_enable()) {
		ret = scm_call2_atomic(SCM_SVC_BOOT, WDOG_DEBUG_DISABLE, 1, 0);
		if(ret)
			dprintf(CRITICAL, "Failed to disable the wdog debug \n");
	}

	return ret;
}
