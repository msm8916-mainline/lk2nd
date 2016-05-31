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

#include <debug.h>
#include <dev/fbcon.h>
#include <target.h>
#include <mmc.h>
#include <partition_parser.h>
#include <platform.h>
#include <crypto_hash.h>
#include <malloc.h>
#include <sha.h>
#include <string.h>
#include <rand.h>
#include <stdlib.h>
#include <boot_verifier.h>
#include <image_verify.h>
#include <qtimer.h>
#include "scm.h"
#include "mdtp.h"
#include "mdtp_fs.h"


#define DIP_ENCRYPT              (0)
#define DIP_DECRYPT              (1)
#define MAX_CIPHER_DIP_SCM_CALLS (3)

#define MDTP_MAJOR_VERSION (0)
#define MDTP_MINOR_VERSION (2)

#define MDTP_CORRECT_PIN_DELAY_MSEC (1000)

/** Extract major version number from complete version. */
#define MDTP_GET_MAJOR_VERSION(version) ((version) >> 16)


/** UT defines **/
#define BAD_PARAM_SIZE 0
#define BAD_PARAM_VERIF_RATIO 101
#define BAD_HASH_MODE 10

/********************************************************************************/

static int mdtp_tzbsp_dec_verify_DIP(DIP_t *enc_dip, DIP_t *dec_dip, uint32_t *verified);
static int mdtp_tzbsp_enc_hash_DIP(DIP_t *dec_dip, DIP_t *enc_dip);
static void mdtp_tzbsp_disallow_cipher_DIP(void);

uint32_t g_mdtp_version = (((MDTP_MAJOR_VERSION << 16) & 0xFFFF0000) | (MDTP_MINOR_VERSION & 0x0000FFFF));
static int is_mdtp_activated = -1;

extern int check_aboot_addr_range_overlap(uintptr_t start, uint32_t size);
int scm_random(uint32_t * rbuf, uint32_t  r_len);
void free_mdtp_image(void);

/********************************************************************************/

/* Read the DIP from EMMC */
static int read_DIP(DIP_t *dip)
{
	unsigned long long ptn = 0;
	uint32_t actual_partition_size;
	uint32_t block_size = mmc_get_device_blocksize();

	int index = INVALID_PTN;

	ASSERT(dip != NULL);

	index = partition_get_index("dip");
	ptn = partition_get_offset(index);

	if(ptn == 0)
	{
		return -1;
	}

	actual_partition_size = ROUNDUP(sizeof(DIP_t), block_size);

	if(mmc_read(ptn, (void *)dip, actual_partition_size))
	{
		dprintf(CRITICAL, "mdtp: read_DIP: ERROR, cannot read DIP info\n");
		return -1;
	}

	dprintf(SPEW, "mdtp: read_DIP: SUCCESS, read %d bytes\n", actual_partition_size);

	return 0;
}

/* Store the DIP into the EMMC */
static int write_DIP(DIP_t *dip)
{
	unsigned long long ptn = 0;
	uint32_t block_size = mmc_get_device_blocksize();

	int index = INVALID_PTN;

	ASSERT(dip != NULL);

	index = partition_get_index("dip");
	ptn = partition_get_offset(index);

	if(ptn == 0)
	{
		return -1;
	}

	if(mmc_write(ptn, ROUNDUP(sizeof(DIP_t), block_size), (void *)dip))
	{
		dprintf(CRITICAL, "mdtp: write_DIP: ERROR, cannot write DIP info\n");
		return -1;
	}

	dprintf(SPEW, "mdtp: write_DIP: SUCCESS, write %d bytes\n", ROUNDUP(sizeof(DIP_t), block_size));

	return 0;
}

/* Deactivate MDTP by storing the default DIP into the EMMC */
static void write_deactivated_DIP()
{
	DIP_t *enc_dip;
	DIP_t *dec_dip;
	int ret;

	enc_dip = malloc(sizeof(DIP_t));
	if (enc_dip == NULL)
	{
		dprintf(CRITICAL, "mdtp: write_deactivated_DIP: ERROR, cannot allocate DIP\n");
		return;
	}

	dec_dip = malloc(sizeof(DIP_t));
	if (dec_dip == NULL)
	{
		dprintf(CRITICAL, "mdtp: write_deactivated_DIP: ERROR, cannot allocate DIP\n");
		free(enc_dip);
		return;
	}

	memset(dec_dip, 0, sizeof(DIP_t));

	dec_dip->status = DIP_STATUS_DEACTIVATED;
	dec_dip->version = g_mdtp_version;

	ret = mdtp_tzbsp_enc_hash_DIP(dec_dip, enc_dip);
	if(ret < 0)
	{
		dprintf(CRITICAL, "mdtp: write_deactivated_DIP: ERROR, cannot cipher DIP\n");
		goto out;
	}

	ret = write_DIP(enc_dip);
	if(ret < 0)
	{
		dprintf(CRITICAL, "mdtp: write_deactivated_DIP: ERROR, cannot write DIP\n");
		goto out;
	}

out:
	free(enc_dip);
	free(dec_dip);
}

/* Validate a hash calculated on entire given partition */
static int verify_partition_single_hash(char *name, uint64_t size, DIP_hash_table_entry_t *hash_table)
{
	unsigned char digest[HASH_LEN]={0};
	unsigned long long ptn = 0;
	int index = INVALID_PTN;
	unsigned char *buf = (unsigned char *)target_get_scratch_address() + MDTP_SCRATCH_OFFSET;
	uint32_t block_size = mmc_get_device_blocksize();
	uint64_t actual_partition_size = ROUNDUP(size, block_size);

	dprintf(SPEW, "mdtp: verify_partition_single_hash: %s, %llu\n", name, size);

	ASSERT(name != NULL);
	ASSERT(hash_table != NULL);
	ASSERT(size > 0);

	index = partition_get_index(name);
	ptn = partition_get_offset(index);

	if(ptn == 0) {
		dprintf(CRITICAL, "mdtp: verify_partition_single_hash: %s: partition was not found\n", name);
		return -1;
	}

	if (mmc_read(ptn, (void *)buf, actual_partition_size))
	{
		dprintf(CRITICAL, "mdtp: verify_partition_single_hash: %s: mmc_read() fail.\n", name);
		return -1;
	}

	/* calculating the hash value using HW crypto */
	target_crypto_init_params();

	if(strcmp(name, "mdtp") == 0){
		buf[0] = 0; // removes first byte
		dprintf(INFO, "mdtp: verify_partition_single_hash: removes 1st byte\n");
	}

	hash_find(buf, size, digest, CRYPTO_AUTH_ALG_SHA256);

	if (memcmp(digest, hash_table->hash, HASH_LEN))
	{
		dprintf(CRITICAL, "mdtp: verify_partition_single_hash: %s: Failed partition hash verification\n", name);

		return -1;
	}

	dprintf(SPEW, "verify_partition_single_hash: %s: VERIFIED!\n", name);

	return 0;
}

/* Validate a hash table calculated per block of a given partition */
static int verify_partition_block_hash(char *name,
									uint64_t size,
									uint32_t verify_num_blocks,
									DIP_hash_table_entry_t *hash_table,
									uint8_t *force_verify_block)
{
	unsigned char digest[HASH_LEN]={0};
	unsigned long long ptn = 0;
	int index = INVALID_PTN;
	unsigned char *buf = (unsigned char *)target_get_scratch_address() + MDTP_SCRATCH_OFFSET;
	uint32_t bytes_to_read;
	uint32_t block_num = 0;
	uint32_t total_num_blocks = ((size - 1) / MDTP_FWLOCK_BLOCK_SIZE) + 1;
	uint32_t rand_int;
	uint32_t block_size = mmc_get_device_blocksize();

	dprintf(SPEW, "mdtp: verify_partition_block_hash: %s, %llu\n", name, size);

	ASSERT(name != NULL);
	ASSERT(hash_table != NULL);
	ASSERT(size > 0);
	ASSERT(force_verify_block != NULL);

	index = partition_get_index(name);
	ptn = partition_get_offset(index);

	if(ptn == 0) {
		dprintf(CRITICAL, "mdtp: verify_partition_block_hash: %s: partition was not found\n", name);
		return -1;
	}

	/* initiating parameters for hash calculation using HW crypto */
	target_crypto_init_params();
	if (check_aboot_addr_range_overlap((uintptr_t)buf, ROUNDUP(MDTP_FWLOCK_BLOCK_SIZE, block_size)))
	{
		dprintf(CRITICAL, "mdtp: verify_partition_block_hash: %s: image buffer address overlaps with aboot addresses.\n", name);
		return -1;
	}

	while (MDTP_FWLOCK_BLOCK_SIZE * block_num < size)
	{
		if (*force_verify_block == 0)
		{
			if(scm_random((uint32_t *)&rand_int, sizeof(rand_int)))
			{
				dprintf(CRITICAL,"mdtp: scm_call for random failed\n");
				return -1;
			}

			/* Skip validation of this block with probability of verify_num_blocks / total_num_blocks */
			if ((rand_int % total_num_blocks) >= verify_num_blocks)
			{
				block_num++;
				hash_table += 1;
				force_verify_block += 1;
				dprintf(CRITICAL, "mdtp: verify_partition_block_hash: %s: skipped verification of block %d\n", name, block_num);
				continue;
			}
		}

		if ((size - (MDTP_FWLOCK_BLOCK_SIZE * block_num) <  MDTP_FWLOCK_BLOCK_SIZE))
		{
			bytes_to_read = size - (MDTP_FWLOCK_BLOCK_SIZE * block_num);
		} else
		{
			bytes_to_read = MDTP_FWLOCK_BLOCK_SIZE;
		}

		if (mmc_read(ptn + (MDTP_FWLOCK_BLOCK_SIZE * block_num), (void *)buf, ROUNDUP(bytes_to_read, block_size)))
		{
			dprintf(CRITICAL, "mdtp: verify_partition_block_hash: %s: mmc_read() fail.\n", name);
			return -1;
		}

		/* calculating the hash value using HW */
		hash_find(buf, bytes_to_read, digest, CRYPTO_AUTH_ALG_SHA256);

		if (memcmp(digest, hash_table->hash, HASH_LEN))
		{
			dprintf(CRITICAL, "mdtp: verify_partition_block_hash: %s: Failed partition hash[%d] verification\n", name, block_num);
			return -1;
		}

		block_num++;
		hash_table += 1;
		force_verify_block += 1;
	}

	dprintf(SPEW, "verify_partition_block_hash: %s: VERIFIED!\n", name);

	return 0;
}

/* Validate the partition parameters read from DIP */
static int validate_partition_params(uint64_t size,
									mdtp_fwlock_mode_t hash_mode,
									uint32_t verify_ratio)
{
	if (size == 0 || size > (uint64_t)MDTP_FWLOCK_BLOCK_SIZE * (uint64_t)MAX_BLOCKS ||
			hash_mode > MDTP_FWLOCK_MODE_FILES || verify_ratio > 100)
	{
		dprintf(CRITICAL, "mdtp: validate_partition_params: error, size=%llu, hash_mode=%d, verify_ratio=%d\n",
				size, hash_mode, verify_ratio);
		return -1;
	}

	return 0;
}

/* Verify a given partitinon */
static int verify_partition(char *name,
							uint64_t size,
							mdtp_fwlock_mode_t hash_mode,
							uint32_t verify_num_blocks,
							DIP_hash_table_entry_t *hash_table,
							uint8_t *force_verify_block)
{
	if (hash_mode == MDTP_FWLOCK_MODE_SINGLE)
	{
		return verify_partition_single_hash(name, size, hash_table);
	} else if (hash_mode == MDTP_FWLOCK_MODE_BLOCK || hash_mode == MDTP_FWLOCK_MODE_FILES)
	{
		return verify_partition_block_hash(name, size, verify_num_blocks, hash_table, force_verify_block);
	}

	/* Illegal value of hash_mode */
	return -1;
}

static int validate_dip(DIP_t *dip)
{
	uint8_t *dip_p;

	ASSERT(dip != NULL);

	/* Make sure DIP version is supported by current SW */
	if (MDTP_GET_MAJOR_VERSION(dip->version) != MDTP_MAJOR_VERSION)
	{
		dprintf(CRITICAL, "mdtp: validate_dip: Wrong DIP version 0x%x\n", dip->version);
		return -1;
	}

	/* Make sure that deactivated DIP content is as expected */
	if (dip->status == DIP_STATUS_DEACTIVATED)
	{
		dip_p = (uint8_t*)&dip->mdtp_cfg;
		while (dip_p < dip->hash)
		{
			if (*dip_p != 0)
			{
				dprintf(CRITICAL, "mdtp: validate_dip: error in deactivated DIP\n");
				return -1;
			}
			dip_p++;
		}
	}

	return 0;
}

/* Display the recovery UI in case mdtp image is corrupted */
static void display_mdtp_fail_recovery_ui(){
	display_error_msg_mdtp();
}

/* Display the recovery UI to allow the user to enter the PIN and continue boot */
static void display_recovery_ui(mdtp_cfg_t *mdtp_cfg)
{
	uint32_t pin_length = 0;
	char entered_pin[MDTP_PIN_LEN+1] = {0};
	uint32_t i;
	int pin_mismatch = -1;

	if (mdtp_cfg->enable_local_pin_authentication)
	{
		dprintf(SPEW, "mdtp: display_recovery_ui: Local deactivation enabled\n");

		pin_length = strlen(mdtp_cfg->mdtp_pin.mdtp_pin);

		if (pin_length != MDTP_PIN_LEN)
		{
			dprintf(CRITICAL, "mdtp: display_recovery_ui: Error, invalid PIN length\n");
			display_error_msg(); /* This will never return */
		}

		// Set entered_pin to initial '0' string + null terminator
		for (i=0; i<pin_length; i++)
		{
			entered_pin[i] = '0';
		}

		// Allow the user to enter the PIN as many times as he wishes
		// (with INVALID_PIN_DELAY_MSECONDS after each failed attempt)
		while (1)
		{
			pin_mismatch = pin_length;
			get_pin_from_user(entered_pin, pin_length);

			// Go over the entire PIN in any case, to prevent side-channel attacks
			for (i=0; i<pin_length; i++)
			{
				// If current digit match, reduce 1 from pin_mismatch
				pin_mismatch -= (((mdtp_cfg->mdtp_pin.mdtp_pin[i] ^ entered_pin[i]) == 0) ? 1 : 0);
			}

			if (0 == pin_mismatch)
			{
				// Valid PIN - deactivate and continue boot
				dprintf(SPEW, "mdtp: display_recovery_ui: valid PIN, continue boot\n");
				write_deactivated_DIP();
				goto out;
			}
			else
			{
				// Invalid PIN - display an appropriate message (which also includes a wait
				// for INVALID_PIN_DELAY_MSECONDS), and allow the user to try again
				dprintf(CRITICAL, "mdtp: display_recovery_ui: ERROR, invalid PIN\n");
				display_invalid_pin_msg();
			}
		}
	}
	else
	{
		dprintf(CRITICAL, "mdtp: display_recovery_ui: Local deactivation disabled, unable to display recovery UI\n");
		display_error_msg(); /* This will never return */
	}

	out:
	display_image_on_screen();
	free_mdtp_image();
	mdelay(MDTP_CORRECT_PIN_DELAY_MSEC);
}

/* Verify the boot or recovery partitions using boot_verifier. */
static int verify_ext_partition(mdtp_ext_partition_verification_t *ext_partition)
{
	int ret = 0;
	bool restore_to_orange = false;
	unsigned long long ptn = 0;
	int index = INVALID_PTN;

	/* If image was already verified in aboot, return its status */
	if (ext_partition->integrity_state == MDTP_PARTITION_STATE_INVALID)
	{
		dprintf(CRITICAL, "mdtp: verify_ext_partition: image %s verified externally and failed.\n",
				ext_partition->partition == MDTP_PARTITION_BOOT ? "boot" : "recovery");
		return -1;
	}
	else if (ext_partition->integrity_state == MDTP_PARTITION_STATE_VALID)
	{
		dprintf(CRITICAL, "mdtp: verify_ext_partition: image %s verified externally succesfully.\n",
				ext_partition->partition == MDTP_PARTITION_BOOT ? "boot" : "recovery");
		return 0;
	}

	/* If image was not verified in aboot, verify it ourselves using boot_verifier. */

	/* 1) Initialize keystore. We don't care about return value which is Verified Boot's state machine state. */
	boot_verify_keystore_init();

	/* 2) If boot_verifier is ORANGE, it will prevent verifying an image. So
	 *    temporarly change boot_verifier state to BOOT_INIT.
	 */
	if (boot_verify_get_state() == ORANGE)
		restore_to_orange = true;
	boot_verify_send_event(BOOT_INIT);

	switch (ext_partition->partition)
	{
	case MDTP_PARTITION_BOOT:
	case MDTP_PARTITION_RECOVERY:

		/* 3) Signature may or may not be at the end of the image. Read the signature if needed. */
		if (!ext_partition->sig_avail)
		{
			if (check_aboot_addr_range_overlap((uintptr_t)(ext_partition->image_addr + ext_partition->image_size), ext_partition->page_size))
			{
				dprintf(CRITICAL, "ERROR: Signature read buffer address overlaps with aboot addresses.\n");
				return -1;
			}

			index = partition_get_index(ext_partition->partition == MDTP_PARTITION_BOOT ? "boot" : "recovery");
			ptn = partition_get_offset(index);
			if(ptn == 0) {
				dprintf(CRITICAL, "ERROR: partition %s not found\n",
						ext_partition->partition == MDTP_PARTITION_BOOT ? "boot" : "recovery");
				return -1;
			}

			if(mmc_read(ptn + ext_partition->image_size, (void *)(ext_partition->image_addr + ext_partition->image_size), ext_partition->page_size))
			{
				dprintf(CRITICAL, "ERROR: Cannot read %s image signature\n",
						ext_partition->partition == MDTP_PARTITION_BOOT ? "boot" : "recovery");
				return -1;
			}
		}

		/* 4) Verify the image using its signature. */
		ret = boot_verify_image((unsigned char *)ext_partition->image_addr,
								ext_partition->image_size,
								ext_partition->partition == MDTP_PARTITION_BOOT ? "/boot" : "/recovery");
		break;

	default:
		/* Only boot and recovery are legal here */
		dprintf(CRITICAL, "ERROR: wrong partition %d\n", ext_partition->partition);
		return -1;
	}

	if (ret)
	{
		dprintf(INFO, "mdtp: verify_ext_partition: image %s verified succesfully in MDTP.\n",
				ext_partition->partition == MDTP_PARTITION_BOOT ? "boot" : "recovery");
	}
	else
	{
		dprintf(CRITICAL, "mdtp: verify_ext_partition: image %s verification failed in MDTP.\n",
				ext_partition->partition == MDTP_PARTITION_BOOT ? "boot" : "recovery");
	}

	/* 5) Restore the right boot_verifier state upon exit. */
	if (restore_to_orange)
	{
		boot_verify_send_event(DEV_UNLOCK);
	}

	return ret ? 0 : -1;
}

/* Verify all protected partitinons according to the DIP */
static void verify_all_partitions(DIP_t *dip,
								mdtp_ext_partition_verification_t *ext_partition,
								verify_result_t *verify_result)
{
	int i;
	int verify_failure = 0;
	int verify_temp_result = 0;
	int ext_partition_verify_failure = 0;
	uint32_t total_num_blocks;

	ASSERT(dip != NULL);
	ASSERT(verify_result != NULL);

	*verify_result = VERIFY_FAILED;

	if (validate_dip(dip))
	{
		dprintf(CRITICAL, "mdtp: verify_all_partitions: failed DIP validation\n");
		return;
	}

	if (dip->status == DIP_STATUS_DEACTIVATED)
	{
		*verify_result = VERIFY_SKIPPED;
		return;
	}
	else
	{
		if (ext_partition->partition != MDTP_PARTITION_NONE)
		{
			for(i=0; i<MAX_PARTITIONS; i++)
			{
				verify_temp_result = 0;
				if(dip->partition_cfg[i].lock_enabled && dip->partition_cfg[i].size)
				{
					total_num_blocks = ((dip->partition_cfg[i].size - 1) / MDTP_FWLOCK_BLOCK_SIZE);
					if (validate_partition_params(dip->partition_cfg[i].size,
							dip->partition_cfg[i].hash_mode,
							dip->partition_cfg[i].verify_ratio))
					{
						dprintf(CRITICAL, "mdtp: verify_all_partitions: Wrong partition parameters\n");
						verify_failure = TRUE;
						break;
					}

					verify_temp_result |= (verify_partition(dip->partition_cfg[i].name,
							dip->partition_cfg[i].size,
							dip->partition_cfg[i].hash_mode,
							(dip->partition_cfg[i].verify_ratio * total_num_blocks) / 100,
							dip->partition_cfg[i].hash_table,
							dip->partition_cfg[i].force_verify_block) != 0);

					if((verify_temp_result) && (strcmp("mdtp",dip->partition_cfg[i].name) == 0)){
						*verify_result = VERIFY_MDTP_FAILED;
					}

					verify_failure |= verify_temp_result;
				}
			}

			ext_partition_verify_failure = verify_ext_partition(ext_partition);

			if (verify_failure || ext_partition_verify_failure)
			{
				dprintf(CRITICAL, "mdtp: verify_all_partitions: Failed partition verification\n");
				return;
			}
		}
		is_mdtp_activated = 1;
	}

	*verify_result = VERIFY_OK;
	return;
}

/* Verify the DIP and all protected partitions */
static void validate_DIP_and_firmware(mdtp_ext_partition_verification_t *ext_partition)
{
	int ret;
	DIP_t *enc_dip;
	DIP_t *dec_dip;
	uint32_t verified = 0;
	verify_result_t verify_result;
	uint32_t block_size = mmc_get_device_blocksize();
	mdtp_cfg_t mdtp_cfg;

	enc_dip = malloc(ROUNDUP(sizeof(DIP_t), block_size));
	if (enc_dip == NULL)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, cannot allocate DIP\n");
		display_error_msg(); /* This will never return */
	}

	dec_dip = malloc(ROUNDUP(sizeof(DIP_t), block_size));
	if (dec_dip == NULL)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, cannot allocate DIP\n");
		free(enc_dip);
		display_error_msg(); /* This will never return */
	}

	/* Read the DIP holding the MDTP Firmware Lock state from the DIP partition */
	ret = read_DIP(enc_dip);
	if(ret < 0)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, cannot read DIP\n");
		display_error_msg(); /* This will never return */
	}

	/* Decrypt and verify the integrity of the DIP */
	ret = mdtp_tzbsp_dec_verify_DIP(enc_dip, dec_dip, &verified);
	if(ret < 0)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, cannot verify DIP\n");
		display_error_msg(); /* This will never return */
	}

	/* In case DIP integrity verification fails, notify the user and halt */
	if(!verified)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, corrupted DIP\n");
		display_error_msg(); /* This will never return */
	}

	/* Verify the integrity of the partitions which are protected, according to the content of the DIP */
	verify_all_partitions(dec_dip, ext_partition, &verify_result);

	mdtp_cfg = dec_dip->mdtp_cfg;

	/* Clear decrypted DIP since we don't need it anymore */
	memset(dec_dip, 0, sizeof(DIP_t));


	if (verify_result == VERIFY_OK)
	{
		dprintf(SPEW, "mdtp: validate_DIP_and_firmware: Verify OK\n");
	}
	else if (verify_result  == VERIFY_SKIPPED)
	{
		dprintf(SPEW, "mdtp: validate_DIP_and_firmware: Verify skipped\n");
	}
	else if(verify_result  == VERIFY_MDTP_FAILED)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, corrupted mdtp image\n");
		display_mdtp_fail_recovery_ui();
	}
	else /* VERIFY_FAILED */
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, corrupted firmware\n");
		display_recovery_ui(&mdtp_cfg);
	}

	memset(&mdtp_cfg, 0, sizeof(mdtp_cfg));

	free(enc_dip);
	free(dec_dip);

	return;
}


/********************************************************************************/

/** Entry point of the MDTP Firmware Lock.
 *  If needed, verify the DIP and all protected partitions.
 *  Allow passing information about partition verified using an external method
 *  (either boot or recovery). For boot and recovery, either use aboot's
 *  verification result, or use boot_verifier APIs to verify internally.
 **/
void mdtp_fwlock_verify_lock(mdtp_ext_partition_verification_t *ext_partition)
{
	int ret;
	bool enabled;

	if(mdtp_fs_init() != 0){
		dprintf(CRITICAL, "mdtp: mdtp_img: ERROR, image file could not be loaded\n");
		display_error_msg_mdtp(); /* This will never return */
	}
	/* sets the default value of this global to be MDTP not activated */
	is_mdtp_activated = 0;

	do {
		if (ext_partition == NULL)
		{
			dprintf(CRITICAL, "mdtp: mdtp_fwlock_verify_lock: ERROR, external partition is NULL\n");
			display_error_msg(); /* This will never return */
			break;
		}

		ret = mdtp_fuse_get_enabled(&enabled);
		if(ret)
		{
			dprintf(CRITICAL, "mdtp: mdtp_fwlock_verify_lock: ERROR, cannot get enabled fuse\n");
			display_error_msg(); /* This will never return */
		}

		/* Continue with Firmware Lock verification only if enabled by eFuse */
		if (enabled)
		{
			/* This function will handle firmware verification failure via UI */
			validate_DIP_and_firmware(ext_partition);
		}
	} while (0);

	/* Disallow CIPHER_DIP SCM call from this point, unless we are in recovery */
	/* The recovery image will disallow CIPHER_DIP SCM call by itself. */
	if (ext_partition->partition == MDTP_PARTITION_BOOT)
	{
		mdtp_tzbsp_disallow_cipher_DIP();
	}
}
/********************************************************************************/

/** Indicates whether the MDTP is currently in ACTIVATED state **/
int mdtp_activated(bool * activated){
	if(is_mdtp_activated < 0){
		/* mdtp_fwlock_verify_lock was not called before, the value is not valid */
		return is_mdtp_activated;
	}

	*activated = is_mdtp_activated;
	return 0;
}

/********************************************************************************/

/* Decrypt a given DIP and verify its integrity */
static int mdtp_tzbsp_dec_verify_DIP(DIP_t *enc_dip, DIP_t *dec_dip, uint32_t *verified)
{
	unsigned char hash[HASH_LEN];
	SHA256_CTX sha256_ctx;
	int ret;

	ASSERT(enc_dip != NULL);
	ASSERT(dec_dip != NULL);
	ASSERT(verified != NULL);

	arch_clean_invalidate_cache_range((addr_t)enc_dip, sizeof(DIP_t));
	arch_invalidate_cache_range((addr_t)dec_dip, sizeof(DIP_t));

	ret = mdtp_cipher_dip_cmd((uint8_t*)enc_dip, sizeof(DIP_t),
							(uint8_t*)dec_dip, sizeof(DIP_t),
							DIP_DECRYPT);
	if (ret)
	{
		dprintf(CRITICAL, "mdtp: mdtp_tzbsp_dec_verify_DIP: ERROR, cannot cipher DIP\n");
		*verified = 0;
		memset(dec_dip, 0, sizeof(DIP_t));
		return -1;
	}

	arch_invalidate_cache_range((addr_t)dec_dip, sizeof(DIP_t));

	SHA256_Init(&sha256_ctx);
	SHA256_Update(&sha256_ctx, dec_dip, sizeof(DIP_t) - HASH_LEN);
	SHA256_Final(hash, &sha256_ctx);

	if (memcmp(hash, dec_dip->hash, HASH_LEN))
	{
		*verified = 0;
		memset(dec_dip, 0, sizeof(DIP_t));
	}
	else
	{
		*verified = 1;
	}

	return 0;
}

/* Encrypt a given DIP and calculate its integrity information */
static int mdtp_tzbsp_enc_hash_DIP(DIP_t *dec_dip, DIP_t *enc_dip)
{
	SHA256_CTX sha256_ctx;
	int ret;

	ASSERT(dec_dip != NULL);
	ASSERT(enc_dip != NULL);

	SHA256_Init(&sha256_ctx);
	SHA256_Update(&sha256_ctx, dec_dip, sizeof(DIP_t) - HASH_LEN);
	SHA256_Final(dec_dip->hash, &sha256_ctx);

	arch_clean_invalidate_cache_range((addr_t)dec_dip, sizeof(DIP_t));
	arch_invalidate_cache_range((addr_t)enc_dip, sizeof(DIP_t));

	ret = mdtp_cipher_dip_cmd((uint8_t*)dec_dip, sizeof(DIP_t),
							(uint8_t*)enc_dip, sizeof(DIP_t),
							DIP_ENCRYPT);
	if (ret)
	{
		dprintf(CRITICAL, "mdtp: mdtp_tzbsp_enc_hash_DIP: ERROR, cannot cipher DIP\n");
		return -1;
	}

	arch_invalidate_cache_range((addr_t)enc_dip, sizeof(DIP_t));

	return 0;
}

/* Disallow the CIPHER_DIP SCM call */
static void mdtp_tzbsp_disallow_cipher_DIP(void)
{
	DIP_t *dip;
	int i;

	dip = malloc(sizeof(DIP_t));
	if (dip == NULL)
	{
		dprintf(CRITICAL, "mdtp: mdtp_tzbsp_disallow_cipher_DIP: ERROR, cannot allocate DIP\n");
		/* Could not allocate DIP - stop device from booting */
		display_error_msg(); /* This will never return */
	}

	/* Disallow the CIPHER_DIP SCM by calling it MAX_CIPHER_DIP_SCM_CALLS times */
	for (i=0; i<MAX_CIPHER_DIP_SCM_CALLS; i++)
	{
		mdtp_tzbsp_enc_hash_DIP(dip, dip);
	}

	free(dip);
}

/********************************************************************************/

/** UT functions **/

/** Hashing fuctions UT **/
int mdtp_verify_hash_ut(){
	unsigned char digest[HASH_LEN]={0};
	unsigned int hash_expected_result = 0xD42B0A29;
	char *buf = "MTDP LK UT hashing functions sanity check";
	int size = 0;
	DIP_hash_table_entry_t partition_hash_table;
	uint8_t partition_force_verify_block = 0;

	char ptr = buf[0];
	while(ptr){
		ptr = buf[++size];
	}
	//Bad partition name - single mode
	if(verify_partition_single_hash("BAD_PARTITION", 1, &partition_hash_table) != -1){
		dprintf(INFO, "verify_hash_ut: [FAIL (1)].\n");
		return -1;
	}

	//Bad partition name - block mode
	if(verify_partition_block_hash("BAD_PARTITION", 1, 1, &partition_hash_table, &partition_force_verify_block) != -1){
		dprintf(INFO, "verify_hash_ut: [FAIL (2)].\n");
		return -1;
	}

	//Hashing sanity check
	hash_find((unsigned char*)buf, size, digest, CRYPTO_AUTH_ALG_SHA256);
	unsigned int *hash_res = (unsigned int *)digest;
	if (*hash_res != hash_expected_result){
		dprintf(INFO, "verify_hash_ut: [FAIL (3)].\n");
		return -1;
	}
	dprintf(INFO, "verify_hash_ut: [PASS].\n");
	return 0;
}

/** Validate partitions params UT **/
int mdtp_validate_partition_params_ut(){
	int partition_size = 10;
	//Bad size
	if(validate_partition_params(BAD_PARAM_SIZE, MDTP_FWLOCK_MODE_SINGLE, 1) != -1){
		dprintf(INFO, "validate_partition_params_ut: [FAIL (1)].\n");
		return -1;
	}

	//Bad size
	if(validate_partition_params((uint64_t)MDTP_FWLOCK_BLOCK_SIZE * (uint64_t)MAX_BLOCKS + 1,
			MDTP_FWLOCK_MODE_SINGLE, 1) != -1){
		dprintf(INFO, "validate_partition_params_ut: [FAIL (2)].\n");
		return -1;
	}

	//Bad verification ratio
	if(validate_partition_params(partition_size, MDTP_FWLOCK_MODE_SIZE, BAD_PARAM_VERIF_RATIO) != -1){
		dprintf(INFO, "validate_partition_params_ut: [FAIL (3)].\n");
		return -1;
	}
	dprintf(INFO, "MDTP LK UT: validate_partition_params_ut [ PASS ]\n");
	return 0;
}

/** Verify partition UT **/
int mdtp_verify_partition_ut(){
	uint8_t partition_force_verify_block = 0;
	DIP_hash_table_entry_t partition_hash_table;
	int verify_num_blocks = 10,partition_size = 1;

	//Unkown hashing mode
	if(verify_partition("system", partition_size, BAD_HASH_MODE, verify_num_blocks,
			&partition_hash_table, &partition_force_verify_block) != -1){
		dprintf(INFO, "verify_partition_ut: Failed Test 1.\n");
		dprintf(INFO, "MDTP LK UT: verify_partition_ut [ FAIL ]\n");
		return -1;
	}
	dprintf(INFO, "MDTP LK UT: verify_partition_ut [ PASS ]\n");
	return 0;
}

