/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
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

#ifndef __SCM_H__
#define __SCM_H__

/* 8 Byte SSD magic number (LE) */
#define DECRYPT_MAGIC_0 0x73737A74
#define DECRYPT_MAGIC_1 0x676D6964
#define ENCRYPT_MAGIC_0 0x6B647373
#define ENCRYPT_MAGIC_1 0x676D6973
#define SSD_HEADER_MAGIC_SIZE  8
#define SSD_HEADER_XML_SIZE    2048
#define SSD_HEADER_MIN_SIZE    128
#define MULTIPLICATION_FACTOR  2

typedef unsigned int uint32;

typedef struct {
	uint32 len;
	uint32 buf_offset;
	uint32 resp_hdr_offset;
	uint32 id;
} scm_command;

typedef struct {
	uint32 len;
	uint32 buf_offset;
	uint32 is_complete;
} scm_response;

typedef struct {
	uint32 *img_ptr;
	uint32 *img_len_ptr;
} img_req;

typedef struct {
	uint32 id;
	uint32 spare;
} tz_secure_cfg;

typedef struct {
  uint32  md_len;
  uint32* md;
} ssd_parse_md_req;

typedef struct {
  uint32  status;
  uint32  md_ctx_id;
  uint32* md_end_ptr;
} ssd_parse_md_rsp;

typedef struct {
  uint32  md_ctx_id;
  uint32  last_frag;
  uint32  frag_len;
  uint32 *frag;
} ssd_decrypt_img_frag_req;

typedef struct {
  uint32 status;
} ssd_decrypt_img_frag_rsp;

typedef struct{
  uint32 feature_id;
} feature_version_req;

typedef struct{
  uint32 version;
} feature_version_rsp;

typedef struct{
  uint32 *keystore_ptr;
  uint32  keystore_len;
} ssd_protect_keystore_req;

typedef struct{
  uint32 status;
} ssd_protect_keystore_rsp;

/* Service IDs */
#define TZBSP_SVC_INFO              0x06
#define SCM_SVC_SSD                 0x07
#define SVC_MEMORY_PROTECTION       0x0C

/*Service specific command IDs */
#define SSD_DECRYPT_ID              0x01
#define SSD_ENCRYPT_ID              0x02
#define SSD_PROTECT_KEYSTORE_ID     0x05
#define SSD_PARSE_MD_ID             0x06
#define SSD_DECRYPT_IMG_FRAG_ID     0x07


#define SECURE_DEVICE_MDSS          0x01

#define IOMMU_SECURE_CFG            0x02

#define TZ_INFO_GET_FEATURE_ID      0x03

/* SSD parsing status messages from TZ */
#define SSD_PMD_ENCRYPTED           0
#define SSD_PMD_NOT_ENCRYPTED       1
#define SSD_PMD_NO_MD_FOUND         3
#define SSD_PMD_BUSY                4
#define SSD_PMD_BAD_MD_PTR_OR_LEN   5
#define SSD_PMD_PARSING_INCOMPLETE  6
#define SSD_PMD_PARSING_FAILED      7
#define SSD_PMD_SETUP_CIPHER_FAILED 8

/* Keystore status messages */
#define TZBSP_SSD_PKS_SUCCESS            0 /**< Successful return. */
#define TZBSP_SSD_PKS_INVALID_PTR        1 /**< Keystore pointer invalid. */
#define TZBSP_SSD_PKS_INVALID_LEN        2 /**< Keystore length incorrect. */
#define TZBSP_SSD_PKS_UNALIGNED_PTR      3 /**< Keystore pointer not word
                                             aligned. */
#define TZBSP_SSD_PKS_PROTECT_MEM_FAILED 4 /**< Failure when protecting
                                             the keystore memory.*/
#define TZBSP_SSD_PKS_INVALID_NUM_KEYS   5 /**< Unsupported number of
                                             keys passed.  If a valid
                                             pointer to non-secure
                                             memory is passed that
                                             isn't a keystore, this is
                                             a likely return code. */
#define TZBSP_SSD_PKS_DECRYPT_FAILED     6  /**< The keystore could not be
                                             decrypted. */

/* Features in TZ */
#define TZBSP_FVER_SSD              5

#define TZBSP_GET_FEATURE_VERSION(major) ((major >> 22)& 0x3FF)

static uint32 smc(uint32 cmd_addr);
int decrypt_scm(uint32_t ** img_ptr, uint32_t * img_len_ptr);
int decrypt_scm_v2(uint32_t ** img_ptr, uint32_t * img_len_ptr);
int encrypt_scm(uint32_t ** img_ptr, uint32_t * img_len_ptr);
int scm_svc_version(uint32 * major, uint32 * minor);
int scm_protect_keystore(uint32_t * img_ptr, uint32_t  img_len);

#define SCM_SVC_FUSE                0x08
#define SCM_BLOW_SW_FUSE_ID         0x01
#define SCM_IS_SW_FUSE_BLOWN_ID     0x02

#define HLOS_IMG_TAMPER_FUSE        0


#define SCM_SVC_CE_CHN_SWITCH_ID    0x04
#define SCM_CE_CHN_SWITCH_ID        0x02

#define SCM_SVC_ES                      0x10
#define SCM_SAVE_PARTITION_HASH_ID      0x01

enum ap_ce_channel_type {
AP_CE_REGISTER_USE = 0,
AP_CE_ADM_USE = 1
};

/* Apps CE resource. */
#define TZ_RESOURCE_CE_AP  2

uint8_t switch_ce_chn_cmd(enum ap_ce_channel_type channel);


void set_tamper_fuse_cmd();

/**
 * struct scm_command - one SCM command buffer
 * @len: total available memory for command and response
 * @buf_offset: start of command buffer
 * @resp_hdr_offset: start of response buffer
 * @id: command to be executed
 * @buf: buffer returned from scm_get_command_buffer()
 *
 * An SCM command is layed out in memory as follows:
 *
 *	------------------- <--- struct scm_command
 *	| command header  |
 *	------------------- <--- scm_get_command_buffer()
 *	| command buffer  |
 *	------------------- <--- struct scm_response and
 *	| response header |      scm_command_to_response()
 *	------------------- <--- scm_get_response_buffer()
 *	| response buffer |
 *	-------------------
 *
 * There can be arbitrary padding between the headers and buffers so
 * you should always use the appropriate scm_get_*_buffer() routines
 * to access the buffers in a safe manner.
 */
struct scm_command {
	uint32_t len;
	uint32_t buf_offset;
	uint32_t resp_hdr_offset;
	uint32_t id;
	uint32_t buf[0];
};

/**
 * struct scm_response - one SCM response buffer
 * @len: total available memory for response
 * @buf_offset: start of response data relative to start of scm_response
 * @is_complete: indicates if the command has finished processing
 */
struct scm_response {
	uint32_t len;
	uint32_t buf_offset;
	uint32_t is_complete;
};



#endif
