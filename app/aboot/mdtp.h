/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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

#ifndef __APP_MDTP_H
#define __APP_MDTP_H

#define TOKEN_LEN              (16)
#define MAX_BLOCKS             (512)
#define MAX_PARTITIONS         (3)
#define MAX_PARTITION_NAME_LEN (100)
#define HASH_LEN               (32)
#define MDTP_PIN_LEN           (8)
#define DIP_PADDING            (15)

#define INITIAL_DELAY_MSECONDS      5000
#define INVALID_PIN_DELAY_MSECONDS  5000

#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))
#define MDTP_FWLOCK_BLOCK_SIZE          (1024*1024*16)
#define MDTP_FWLOCK_MAX_FILES           (100)
#define MDTP_FWLOCK_MAX_FILE_NAME_LEN   (100)
#define MDTP_SCRATCH_OFFSET 0x8000000

#ifdef MDTP_SUPPORT
#ifndef VERIFIED_BOOT
#error MDTP feature requires VERIFIED_BOOT feature
#endif
#endif

#pragma pack(push, mdtp, 1)

typedef enum {
	DIP_STATUS_DEACTIVATED = 0,
	DIP_STATUS_ACTIVATED,
	DIP_STATUS_SIZE = 0x7FFFFFFF
} dip_status_t;

typedef enum {
	MDTP_FWLOCK_MODE_SINGLE = 0,
	MDTP_FWLOCK_MODE_BLOCK,
	MDTP_FWLOCK_MODE_FILES,
	MDTP_FWLOCK_MODE_SIZE = 0x7FFFFFFF
} mdtp_fwlock_mode_t;

typedef struct DIP_hash_table_entry {
	unsigned char hash[HASH_LEN];       /* Hash on block */
} DIP_hash_table_entry_t;

typedef struct DIP_partition_cfg {
	uint64_t size;                                  /* Partition size in bytes */
	char name[MAX_PARTITION_NAME_LEN];              /* Partition name */
	uint8_t lock_enabled;                           /* Image locked? */
	mdtp_fwlock_mode_t hash_mode;                   /* Hash per IMAGE or BLOCK */
	uint8_t force_verify_block[MAX_BLOCKS];   		/* Verify only given block numbers. */
	char files_to_protect[MDTP_FWLOCK_MAX_FILES][MDTP_FWLOCK_MAX_FILE_NAME_LEN]; /* Verify given files */
	uint32_t verify_ratio;                          /* Statistically verify this ratio of blocks */
	DIP_hash_table_entry_t hash_table[MAX_BLOCKS];  /* Hash table */
} DIP_partition_cfg_t;

typedef struct mdtp_pin {
	char mdtp_pin[MDTP_PIN_LEN+1];              /* A null terminated PIN. */
} mdtp_pin_t;

/** MDTP configuration. */
typedef struct mdtp_cfg {
	uint8_t enable_local_pin_authentication;/* Allow local authentication using a PIN. */
	mdtp_pin_t mdtp_pin;                    /* Null terminated PIN provided by the user for local deactivation.
                                               PIN length should be MDTP_PIN_LEN digits. */
} mdtp_cfg_t;

typedef struct DIP {
	/* Management area of the DIP */
	uint32_t version;                                           /* DIP version */
	dip_status_t status;                                        /* DIP activated/deactivated */
	mdtp_cfg_t mdtp_cfg;                                        /* MDTP configuration, such as PIN */

	/* Firmware Lock area of the DIP */
	DIP_partition_cfg_t partition_cfg[MAX_PARTITIONS];          /* Config for each partition */

	/* Footer area of the DIP */
	uint8_t padding[DIP_PADDING];                                   /* Pad to multiple of 16 bytes */
	unsigned char hash[HASH_LEN];                                   /* DIP integrity */
} DIP_t;

#pragma pack(pop, mdtp)

typedef enum {
	MDTP_PARTITION_BOOT = 0,
	MDTP_PARTITION_RECOVERY,
	MDTP_PARTITION_NONE,
	MDTP_PARTITION_NUM,
} mdtp_ext_partition_t;

typedef enum {
	MDTP_PARTITION_STATE_UNSET = 0,
	MDTP_PARTITION_STATE_VALID,
	MDTP_PARTITION_STATE_INVALID,
	MDTP_PARTITION_STATE_SIZE,
} mdtp_ext_partition_state_t;

typedef struct mdtp_ext_partition {
	mdtp_ext_partition_t partition;
	mdtp_ext_partition_state_t integrity_state;
	uint32_t page_size;
	uint32_t image_addr;
	uint32_t image_size;
	bool sig_avail;
} mdtp_ext_partition_verification_t;

typedef enum {
	VERIFY_SKIPPED = 0,
	VERIFY_OK,
	VERIFY_FAILED,
	VERIFY_MDTP_FAILED,
} verify_result_t;

/**
 * mdtp_fuse_get_enabled
 *
 * Return whether the MDTP is currently enabled or
 * disabled in HW.
 *
 * @param[out] enabled: set to true if MDTP enabled,
 * false otherwise.
 *
 * @return - negative value for an error, 0 for success.
 */
int mdtp_fuse_get_enabled(bool *enabled);

/**
 * get_pin_from_user
 *
 * Display the recovery PIN screen and set received buffer
 * with the PIN the user has entered.
 *
 * @param[out] entered_pin: buffer holding the received PIN.
 * @param[in]  pin_length:  PIN length (and also entered_pin buffer length).
 *
 * @return - None.
 */
void get_pin_from_user(char *entered_pin, uint32_t pin_length);

/**
 * display_invalid_pin_msg
 *
 * User has entered invalid PIN, display error message and
 * allow the user to try again.
 *
 * @return - None.
 */
void display_invalid_pin_msg();

/**
 * display_error_msg
 *
 * Display error message and stop boot process.
 *
 * @return - None.
 */
void display_error_msg();

/**
 * display_error_msg_mdtp
 *
 * Display error message in case mdtp image corrupted and stop boot process.
 *
 * @return - None.
 */
void display_error_msg_mdtp();

/**
 * mdtp_activated
 *
 * Indicates whether the MDTP is currently in ACTIVATED state.
 * You must call this function only after calling to mdtp_fwlock_verify_lock();
 *
 * @param[out] activated: MDTP is in ACTIVATED state (TRUE/FALSE).
 *
 * @return - negative value for an error, 0 for success.
 */
int mdtp_activated(bool * activated);


// External functions

/** Entry point of the MDTP Firmware Lock.
 *  If needed, verify the DIP and all protected partitions.
 *  Allow passing information about partition verified using an external method
 *  (either boot or recovery). For boot and recovery, either use aboot's
 *  verification result, or use boot_verifier APIs to verify internally.
 **/
void mdtp_fwlock_verify_lock(mdtp_ext_partition_verification_t *ext_partition);


//UT functions

/** Entry point of the MDTP LK UT.
 * Start UT on LK mdtp components during fastboot.
 **/
void cmd_mdtp_runtests();

int mdtp_verify_hash_ut();
int mdtp_validate_partition_params_ut();
int mdtp_verify_partition_ut();
int mdtp_verify_external_partition_ut(mdtp_ext_partition_verification_t* ext_partition);

#endif
