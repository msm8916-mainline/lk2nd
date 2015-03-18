/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
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
#include "scm.h"
#include "mdtp.h"

#define DIP_ENCRYPT 0
#define DIP_DECRYPT 1

static int mdtp_tzbsp_dec_verify_DIP(DIP_t *enc_dip, DIP_t *dec_dip, uint32_t *verified);
static int mdtp_tzbsp_enc_hash_DIP(DIP_t *dec_dip, DIP_t *enc_dip);

static int is_mdtp_activated = -1;
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
		dprintf(CRITICAL, "mdtp: write_DIP: ERROR, cannot read DIP info\n");
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
static int verify_partition_single_hash(char *name, uint32_t size, DIP_hash_table_entry_t *hash_table)
{
	unsigned char digest[32]={0};
	unsigned long long ptn = 0;
	int index = INVALID_PTN;
	unsigned char *buf = (unsigned char *)target_get_scratch_address();
	uint32_t block_size = mmc_get_device_blocksize();
	uint32_t actual_partition_size = ROUNDUP(size, block_size);

	dprintf(SPEW, "mdtp: verify_partition_single_hash: %s, %u\n", name, size);

	ASSERT(name != NULL);
	ASSERT(hash_table != NULL);

	index = partition_get_index(name);
	ptn = partition_get_offset(index);

	if(ptn == 0) {
		dprintf(CRITICAL, "mdtp: verify_partition_single_hash: %s: partition was not found\n", name);
		return -1;
	}

	if (mmc_read(ptn, (void *)buf, actual_partition_size))
	{
		dprintf(CRITICAL, "mdtp: verify_partition__single_hash: %s: mmc_read() fail.\n", name);
		return -1;
	}

	/* calculating the hash value using HW crypto */
	target_crypto_init_params();
	hash_find(buf, size, (unsigned char *)&digest, CRYPTO_AUTH_ALG_SHA256);

	if (memcmp(&digest[0], &(hash_table->hash[0]), HASH_LEN))
	{
		dprintf(CRITICAL, "mdtp: verify_partition_single_hash: %s: Failed partition hash verification\n", name);

		return -1;
	}

	dprintf(SPEW, "verify_partition_single_hash: %s: VERIFIED!\n", name);

	return 0;
}

/* Validate a hash table calculated per block of a given partition */
static int verify_partition_block_hash(char *name,
								uint32_t size,
								uint32_t total_num_blocks,
								uint32_t verify_num_blocks,
								DIP_hash_table_entry_t *hash_table,
							    uint8_t *force_verify_block)
{
	unsigned char digest[32]={0};
	unsigned long long ptn = 0;
	int index = INVALID_PTN;
	unsigned char *buf = (unsigned char *)target_get_scratch_address();
	uint32_t bytes_to_read;
	uint32_t block_num = 0;

	dprintf(SPEW, "mdtp: verify_partition_block_hash: %s, %u\n", name, size);

	ASSERT(name != NULL);
	ASSERT(hash_table != NULL);

	index = partition_get_index(name);
	ptn = partition_get_offset(index);

	if(ptn == 0) {
		dprintf(CRITICAL, "mdtp: verify_partition_block_hash: %s: partition was not found\n", name);
		return -1;
	}

	/* initiating parameters for hash calculation using HW crypto */
	target_crypto_init_params();

	while (MDTP_FWLOCK_BLOCK_SIZE * block_num < size)
	{
		if (*force_verify_block == 0)
		{
			/* Skip validation of this block with probability of verify_num_blocks / total_num_blocks */
			if ((rand() % total_num_blocks) >= verify_num_blocks)
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

		if (mmc_read(ptn + (MDTP_FWLOCK_BLOCK_SIZE * block_num), (void *)buf, bytes_to_read))
		{
			dprintf(CRITICAL, "mdtp: verify_partition_block_hash: %s: mmc_read() fail.\n", name);
			return -1;
		}

		/* calculating the hash value using HW */
		hash_find(buf, bytes_to_read, (unsigned char *)&digest, CRYPTO_AUTH_ALG_SHA256);

		if (memcmp(&digest[0], &(hash_table->hash[0]), HASH_LEN))
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

/* Verify a given partitinon */
static int verify_partition(char *name,
						uint32_t size,
						mdtp_fwlock_mode_t hash_mode,
						uint32_t total_num_blocks,
						uint32_t verify_num_blocks,
						DIP_hash_table_entry_t *hash_table,
						uint8_t *force_verify_block)
{

	ASSERT(name != NULL);
	ASSERT(hash_table != NULL);

	if (hash_mode == MDTP_FWLOCK_MODE_SINGLE)
	{
		return verify_partition_single_hash(name, size, hash_table);
	} else if (hash_mode == MDTP_FWLOCK_MODE_BLOCK || hash_mode == MDTP_FWLOCK_MODE_FILES)
	{
		return verify_partition_block_hash(name, size, total_num_blocks, verify_num_blocks, hash_table, force_verify_block);
	}
	else
	{
		dprintf(CRITICAL, "mdtp: verify_partition: %s: Wrong DIP partition hash mode\n", name);
		return -1;
	}

	return 0;
}

/* Display the recovery UI to allow the user to enter the PIN and continue boot */
static int display_recovery_ui(DIP_t *dip)
{
	uint32_t pin_length = 0;
	char entered_pin[MDTP_MAX_PIN_LEN+1] = {0};
	uint32_t i;
	uint32_t equal_count = 0, different_count = 0;

	if (dip->mdtp_cfg.enable_local_pin_authentication)
	{
		dprintf(SPEW, "mdtp: display_recovery_ui: Local deactivation enabled\n");

		pin_length = strlen(dip->mdtp_cfg.mdtp_pin.mdtp_pin);

		if (pin_length > MDTP_MAX_PIN_LEN || pin_length < MDTP_MIN_PIN_LEN)
		{
			dprintf(CRITICAL, "mdtp: display_recovery_ui: Error, invalid PIN length\n");
			display_error_msg();
			return -1;
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
		    get_pin_from_user(entered_pin, pin_length);

		    // Go over the entire PIN in any case, to prevent side-channel attacks
		    for (i=0; i<pin_length; i++)
		    {
		        if (dip->mdtp_cfg.mdtp_pin.mdtp_pin[i] == entered_pin[i])
		            equal_count++;
		        else
		            different_count++;
		    }

		    if (equal_count == pin_length)
		    {
		        // Valid PIN - deactivate and continue boot
		        dprintf(SPEW, "mdtp: display_recovery_ui: valid PIN, continue boot\n");
		        write_deactivated_DIP();
		        return 0;
		    }
		    else
		    {
		        // Invalid PIN - display an appropriate message (which also includes a wait
		        // for INVALID_PIN_DELAY_MSECONDS), and allow the user to try again
		        dprintf(CRITICAL, "mdtp: display_recovery_ui: ERROR, invalid PIN\n");
		        display_invalid_pin_msg();

		        equal_count = 0;
		        different_count = 0;
		    }
		}
	}
	else
	{
		dprintf(CRITICAL, "mdtp: display_recovery_ui: Local deactivation disabled, unable to display recovery UI\n");
		display_error_msg();
		return -1;
	}
}

/* Verify all protected partitinons according to the DIP */
static int verify_all_partitions(DIP_t *dip, verify_result_t *verify_result)
{
	int i;
	int verify_failure = 0;
	uint32_t total_num_blocks;

	ASSERT(dip != NULL);
	ASSERT(verify_result != NULL);

	*verify_result = VERIFY_FAILED;

	if (dip->status == DIP_STATUS_DEACTIVATED)
	{
		*verify_result = VERIFY_SKIPPED;
		return 0;
	}
	else if (dip->status == DIP_STATUS_ACTIVATED)
	{
		for(i=0; i<MAX_PARTITIONS; i++)
		{
			if(dip->partition_cfg[i].lock_enabled && dip->partition_cfg[i].size)
			{
				total_num_blocks = ((dip->partition_cfg[i].size - 1) / MDTP_FWLOCK_BLOCK_SIZE);

				verify_failure |= verify_partition(dip->partition_cfg[i].name,
							 dip->partition_cfg[i].size,
							 dip->partition_cfg[i].hash_mode,
							 total_num_blocks,
							 (dip->partition_cfg[i].verify_ratio * total_num_blocks) / 100,
							 dip->partition_cfg[i].hash_table,
							 dip->partition_cfg[i].force_verify_block);
			}
		}

		if (verify_failure)
		{
			dprintf(CRITICAL, "mdtp: verify_all_partitions: Failed partition verification\n");
			return 0;
		}
		is_mdtp_activated = 1;

	}

	*verify_result = VERIFY_OK;
	return 0;
}

/* Verify the DIP and all protected partitions */
static void validate_DIP_and_firmware()
{
	int ret;
	DIP_t *enc_dip;
	DIP_t *dec_dip;
	uint32_t verified = 0;
	verify_result_t verify_result;
	uint32_t block_size = mmc_get_device_blocksize();

	enc_dip = malloc(ROUNDUP(sizeof(DIP_t), block_size));
	if (enc_dip == NULL)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, cannot allocate DIP\n");
		return;
	}

	dec_dip = malloc(ROUNDUP(sizeof(DIP_t), block_size));
	if (dec_dip == NULL)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, cannot allocate DIP\n");
		free(enc_dip);
		return;
	}

	/* Read the DIP holding the MDTP Firmware Lock state from the DIP partition */
	ret = read_DIP(enc_dip);
	if(ret < 0)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, cannot read DIP\n");
		goto out;
	}

	/* Decrypt and verify the integrity of the DIP */
	ret = mdtp_tzbsp_dec_verify_DIP(enc_dip, dec_dip, &verified);
	if(ret < 0)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, cannot verify DIP\n");
		display_error_msg();
		goto out;
	}

	/* In case DIP integrity verification fails, notify the user and halt */
	if(!verified)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, corrupted DIP\n");
		display_error_msg();
		goto out;
	}

	/* Verify the integrity of the partitions which are protectedm, according to the content of the DIP */
	ret = verify_all_partitions(dec_dip, &verify_result);
	if(ret < 0)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, cannot verify firmware\n");
		goto out;
	}

	if (verify_result == VERIFY_OK)
	{
		dprintf(SPEW, "mdtp: validate_DIP_and_firmware: Verify OK\n");
	}
	else if (verify_result  == VERIFY_FAILED)
	{
		dprintf(CRITICAL, "mdtp: validate_DIP_and_firmware: ERROR, corrupted firmware\n");
		display_recovery_ui(dec_dip);
	} else /* VERIFY_SKIPPED */
	{
		dprintf(SPEW, "mdtp: validate_DIP_and_firmware: Verify skipped\n");
	}

out:
	free(enc_dip);
	free(dec_dip);

	return;
}

/********************************************************************************/

/** Entry point of the MDTP Firmware Lock: If needed, verify the DIP
 *  and all protected partitions **/

int mdtp_fwlock_verify_lock()
{
	int ret;
	bool enabled;

	/* sets the default value of this global to be MDTP not activated */
	is_mdtp_activated = 0;

	ret = mdtp_fuse_get_enabled(&enabled);
	if(ret)
	{
		dprintf(CRITICAL, "mdtp: mdtp_fwlock_verify_lock: ERROR, cannot get enabled fuse\n");
		return -1;
	}

	/* Continue with Firmware Lock verification only if enabled by eFuse */
	if (enabled)
	{
		validate_DIP_and_firmware();
	}

	return 0;
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
	unsigned char *hash_p;
	unsigned char hash[HASH_LEN];
	SHA256_CTX sha256_ctx;
	int ret;

	ASSERT(enc_dip != NULL);
	ASSERT(dec_dip != NULL);
	ASSERT(verified != NULL);

	ret = mdtp_cipher_dip_cmd((uint8_t*)enc_dip, sizeof(DIP_t),
								(uint8_t*)dec_dip, sizeof(DIP_t),
								DIP_DECRYPT);
	if (ret)
	{
		dprintf(CRITICAL, "mdtp: mdtp_tzbsp_dec_verify_DIP: ERROR, cannot cipher DIP\n");
		*verified = 0;
		return -1;
	}

	SHA256_Init(&sha256_ctx);
	SHA256_Update(&sha256_ctx, dec_dip, sizeof(DIP_t) - HASH_LEN);
	SHA256_Final(hash, &sha256_ctx);

	hash_p = (unsigned char*)dec_dip + sizeof(DIP_t) - HASH_LEN;

	if (memcmp(hash, hash_p, HASH_LEN))
	{
		*verified = 0;
	}
	else
	{
		*verified = 1;
	}

	return 0;
}

static int mdtp_tzbsp_enc_hash_DIP(DIP_t *dec_dip, DIP_t *enc_dip)
{
	unsigned char *hash_p;
	SHA256_CTX sha256_ctx;
	int ret;

	ASSERT(dec_dip != NULL);
	ASSERT(enc_dip != NULL);

	hash_p = (unsigned char*)dec_dip + sizeof(DIP_t) - HASH_LEN;

	SHA256_Init(&sha256_ctx);
	SHA256_Update(&sha256_ctx, dec_dip, sizeof(DIP_t) - HASH_LEN);
	SHA256_Final(hash_p, &sha256_ctx);

	ret = mdtp_cipher_dip_cmd((uint8_t*)dec_dip, sizeof(DIP_t),
								(uint8_t*)enc_dip, sizeof(DIP_t),
								DIP_ENCRYPT);
	if (ret)
	{
		dprintf(CRITICAL, "mdtp: mdtp_tzbsp_enc_hash_DIP: ERROR, cannot cipher DIP\n");
		return -1;
	}

	return 0;
}
