/* Copyright (c) 2011-2015, The Linux Foundation. All rights reserved.

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

/* ARM SCM format support related flags */
#define SIP_SVC_CALLS                          0x02000000
#define MAKE_SIP_SCM_CMD(svc_id, cmd_id)       ((((svc_id << 8) | (cmd_id)) & 0xFFFF) | SIP_SVC_CALLS)
#define MAKE_SCM_VAR_ARGS(num_args, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, ...) (\
						  (((t0) & 0xff) << 4) | \
						  (((t1) & 0xff) << 6) | \
						  (((t2) & 0xff) << 8) | \
						  (((t3) & 0xff) << 10) | \
						  (((t4) & 0xff) << 12) | \
						  (((t5) & 0xff) << 14) | \
						  (((t6) & 0xff) << 16) | \
						  (((t7) & 0xff) << 18) | \
						  (((t8) & 0xff) << 20) | \
						  (((t9) & 0xff) << 22) | \
						  (num_args & 0xffff))
#define MAKE_SCM_ARGS(...)                     MAKE_SCM_VAR_ARGS(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define SCM_ATOMIC_BIT                         BIT(31)
#define SCM_MAX_ARG_LEN                        5
#define SCM_INDIR_MAX_LEN                      10

enum
{
	SMC_PARAM_TYPE_VALUE = 0,
	SMC_PARAM_TYPE_BUFFER_READ,
	SMC_PARAM_TYPE_BUFFER_READWRITE,
	SMC_PARAM_TYPE_BUFFER_VALIDATION,
} scm_arg_type;

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
	uint32 *status_ptr;
	uint32 status_len;
} get_secure_state_req;

typedef struct{
	uint32 status_low;
	uint32 status_high;
} get_secure_state_rsp;

typedef struct{
	uint32 row_address;
	uint32 addr_type;
	uint32 *row_data;
	uint32 *qfprom_api_status;
} qfprom_read_row_req;

typedef struct{
  uint32 *keystore_ptr;
  uint32  keystore_len;
} ssd_protect_keystore_req;

typedef struct{
  uint32 status;
} ssd_protect_keystore_rsp;

typedef struct {
	uint32_t config;
	uint32_t spare;
} tz_xpu_prot_cmd;

typedef struct {
	uint64_t el1_x0;
	uint64_t el1_x1;
	uint64_t el1_x2;
	uint64_t el1_x3;
	uint64_t el1_x4;
	uint64_t el1_x5;
	uint64_t el1_x6;
	uint64_t el1_x7;
	uint64_t el1_x8;
	uint64_t el1_elr;
} el1_system_param;

struct tz_prng_data {
	uint8_t *out_buf;
	uint32_t out_buf_size;
}__packed;

typedef struct {
	uint8_t *in_buf;
	uint32_t in_buf_size;
	uint8_t *out_buf;
	uint32_t out_buf_size;
	uint32_t direction;
} mdtp_cipher_dip_req;

/* SCM support as per ARM spec */
/*
 * Structure to define the argument for scm call
 * x0: is the command ID
 * x1: Number of argument & type of arguments
 *   : Type can be any of
 *   : SMC_PARAM_TYPE_VALUE             0
 *   : SMC_PARAM_TYPE_BUFFER_READ       1
 *   : SMC_PARAM_TYPE_BUFFER_READWRITE  2
 *   : SMC_PARAM_TYPE_BUFFER_VALIDATION 3
 *   @Note: Number of argument count starts from X2.
 * x2-x4: Arguments
 * X5[10]: if the number of argument is more, an indirect
 *       : list can be passed here.
 */
typedef struct {
	uint32_t x0;/* command ID details as per ARMv8 spec :
					0:7 command, 8:15 service id
					0x02000000: SIP calls
					30: SMC32 or SMC64
					31: Standard or fast calls*/
	uint32_t x1; /* # of args and attributes for buffers
				  * 0-3: arg #
				  * 4-5: type of arg1
				  * 6-7: type of arg2
				  * :
				  * :
				  * 20-21: type of arg8
				  * 22-23: type of arg9
				  */
	uint32_t x2; /* Param1 */
	uint32_t x3; /* Param2 */
	uint32_t x4; /* Param3 */
	uint32_t x5[10]; /* Indirect parameter list */
	uint32_t atomic; /* To indicate if its standard or fast call */
} scmcall_arg;

/* Return value for the SCM call:
 * SCM call returns values in register if its less than
 * 12 bytes, anything greater need to be input buffer + input len
 * arguments
 */
typedef struct
{
	uint32_t x1;
	uint32_t x2;
	uint32_t x3;
} scmcall_ret;

/* Service IDs */
#define SCM_SVC_BOOT                0x01
#define TZBSP_SVC_INFO              0x06
#define SCM_SVC_SSD                 0x07
#define SVC_MEMORY_PROTECTION       0x0C
#define TZ_SVC_CRYPTO               0x0A
#define SCM_SVC_INFO                0x06

/*Service specific command IDs */
#define ERR_FATAL_ENABLE            0x0
#define SSD_DECRYPT_ID              0x01
#define SSD_ENCRYPT_ID              0x02
#define SSD_PROTECT_KEYSTORE_ID     0x05
#define SSD_PARSE_MD_ID             0x06
#define SSD_DECRYPT_IMG_FRAG_ID     0x07
#define WDOG_DEBUG_DISABLE          0x09
#define SCM_DLOAD_CMD               0x10
#define XPU_ERR_FATAL               0xe

#define SECURE_DEVICE_MDSS          0x01

#define IOMMU_SECURE_CFG            0x02

#define TZ_INFO_GET_FEATURE_ID      0x03
#define TZ_INFO_GET_SECURE_STATE    0x04

#define PRNG_CMD_ID                 0x01
#define IS_CALL_AVAIL_CMD           0x01
#define IS_SECURE_BOOT_ENABLED      0x04

/* Download Mode specific arguments to be passed to TZ */
#define SCM_EDLOAD_MODE 0x01
#define SCM_DLOAD_MODE  0x10

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

/**
 * Check security status on the device. Returns the security check result.
 * Bit value 0 means the check passing, and bit value 1 means the check failing.
 *
 * @state_low[out] : lower 32 bits of the state:
 *                   Bit 0: secboot enabling check failed
 *                   Bit 1: Sec HW key is not programmed
 *                   Bit 2: debug disable check failed
 *                   Bit 3: Anti-rollback check failed
 *                   Bit 4: fuse config check failed
 *                   Bit 5: rpmb fuse check failed
 * @state_high[out] : highr 32 bits of the state.
 *
 * Returns 0 on success, negative on failure.
 */
int scm_svc_get_secure_state(uint32_t *state_low, uint32_t *state_high);

int scm_protect_keystore(uint32_t * img_ptr, uint32_t  img_len);
int
scm_call(uint32_t svc_id, uint32_t cmd_id, const void *cmd_buf,
        size_t cmd_len, void *resp_buf, size_t resp_len);


#define SCM_SVC_FUSE                0x08
#define SCM_BLOW_SW_FUSE_ID         0x01
#define SCM_IS_SW_FUSE_BLOWN_ID     0x02
#define SCM_QFPROM_READ_ROW_ID      0x05

#define HLOS_IMG_TAMPER_FUSE        0


#define SCM_SVC_CE_CHN_SWITCH_ID    0x04
#define SCM_CE_CHN_SWITCH_ID        0x02

#define SCM_SVC_ES                      0x10
#define SCM_SAVE_PARTITION_HASH_ID      0x01

#define SCM_SVC_MDTP                    0x12
#define SCM_MDTP_CIPHER_DIP             0x01

#define SCM_SVC_PWR                     0x9
#define SCM_IO_DISABLE_PMIC_ARBITER     0x1
#define SCM_IO_DISABLE_PMIC_ARBITER1    0x2

#define SCM_SVC_MILESTONE_32_64_ID      0x1
#define SCM_SVC_MILESTONE_CMD_ID        0xf
#define SCM_SVC_TZSCHEDULER             0xFC

enum ap_ce_channel_type {
AP_CE_REGISTER_USE = 0,
AP_CE_ADM_USE = 1
};

/* Apps CE resource. */
#define TZ_RESOURCE_CE_AP  2

/* Secure IO Service IDs */
#define SCM_IO_READ     0x1
#define SCM_IO_WRITE    0x2
#define SCM_SVC_IO      0x5

uint8_t switch_ce_chn_cmd(enum ap_ce_channel_type channel);

/**
 * Encrypt or Decrypt a Data Integrity Partition (DIP) structure using a
 * HW derived key. The DIP is used for storing integrity information for
 * Mobile Device Theft Protection (MDTP) service.
 *
 * @in_buf[in] : Pointer to plain text buffer.
 * @in_buf_size[in] : Plain text buffer size.
 * @out_buf[in] : Pointer to encrypted buffer.
 * @out_buf_size[in] : Encrypted buffer size.
 * @direction[in] : 0 for ENCRYPTION, 1 for DECRYPTION.
 *
 * Returns 0 on success, negative on failure.
 */
int mdtp_cipher_dip_cmd(uint8_t *in_buf, uint32_t in_buf_size, uint8_t *out_buf,
                          uint32_t out_buf_size, uint32_t direction);

void set_tamper_fuse_cmd();

/**
 * Reads the row data of the specified QFPROM row address.
 *
 * @row_address[in] : Row address in the QFPROM region to read.
 * @addr_type[in] : Raw or corrected address.
 * @row_data[in] : Pointer to the data to be read.
 * @qfprom_api_status[out] : Status of the read operation.
 *
 * Returns Any errors while reading data from the specified
 * Returns 0 on success, negative on failure.
 */
int qfprom_read_row_cmd(uint32_t row_address, uint32_t addr_type, uint32_t *row_data, uint32_t *qfprom_api_status);

int scm_halt_pmic_arbiter();
int scm_call_atomic2(uint32_t svc, uint32_t cmd, uint32_t arg1, uint32_t arg2);

void scm_elexec_call(paddr_t kernel_entry, paddr_t dtb_offset);
uintptr_t get_canary();
/* API to configure XPU violations as fatal */
int scm_xpu_err_fatal_init();

/* APIs to support ARM scm standard
 * Takes arguments : x0-x5 and returns result
 * in x0-x3*/
uint32_t scm_call2(scmcall_arg *arg, scmcall_ret *ret);

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
/* Perform any scm init needed before making scm calls
 * Used for checking if armv8 SCM support present
 */
void scm_init();
uint32_t is_secure_boot_enable();
int scm_dload_mode();

/* Is armv8 supported */
bool is_scm_armv8_support();
int is_scm_call_available(uint32_t svc_id, uint32_t cmd_id);
#endif
