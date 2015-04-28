/* Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
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
#include <target.h>
#include <mmc.h>
#include <partition_parser.h>
#include <string.h>
#include <stdlib.h>

#include "mdtp.h"
#include "scm.h"

#define MAX_METADATA_SIZE       (0x1000)
#define QFPROM_ADDR_SPACE_RAW   (0)

char metadata_block[MAX_METADATA_SIZE] = {0};

/********************************************************************************/

typedef union
{
    struct {
		uint8_t enable1       : 1;
		uint8_t disable1      : 1;
		uint8_t enable2       : 1;
		uint8_t disable2      : 1;
		uint8_t enable3       : 1;
		uint8_t disable3      : 1;
		uint8_t reserved1     : 1;
		uint8_t reserved2     : 1;
	} bitwise;
	uint8_t mask;
} mdtp_eFuses_t;

typedef struct metadata {
	mdtp_eFuses_t eFuses;
} metadata_t;

/********************************************************************************/

/**
 * Checks if we are in test mode according to relevant eFuses
 *
 * @return - negative value for an error, 0 for success.
 */
static int is_test_mode(void)
{
	static int test_mode_set = 0;
	static int test_mode = 0;
	int ret = 0;
	uint32_t status_low = 0;
	uint32_t status_high = 0;

#define SECBOOT_FUSE       0x01
#define SHK_FUSE           0x02
#define DEBUG_FUSE         0x04

    /* Make sure we only read the test mode once */
	if (test_mode_set)
		return test_mode;

	ret = scm_svc_get_secure_state(&status_low, &status_high);

	if(ret == 0)
	{
		/* (SECBOOT_FUSE | SHK_FUSE | DEBUG_FUSE) implies that none of the fuses are blown */
		if((status_low & (SECBOOT_FUSE | SHK_FUSE | DEBUG_FUSE)) == (SECBOOT_FUSE | SHK_FUSE | DEBUG_FUSE))
			test_mode = 1;
	}
	else
	{
		dprintf(CRITICAL, "mdtp: is_test_mode: qsee_get_secure_state returned error: %d, status.value[0]: %d", ret, status_low);
		test_mode = 0;
    }

	test_mode_set = 1;
	dprintf(INFO, "mdtp: is_test_mode: test mode is set to %d", test_mode);

	return test_mode;
}

/**
 * Read the Firmware Lock Metadata from EMMC
 *
 * @param metadata - Read a metadata block holding eFuse emulation from MDTP partition.
 *
 * @return - negative value for an error, 0 for success.
 */
static int read_metadata(metadata_t *metadata)
{
	unsigned long long ptn = 0;
	uint32_t actual_size;
	int index = INVALID_PTN;
	uint32_t block_size = mmc_get_device_blocksize();

	index = partition_get_index("mdtp");
	ptn = partition_get_offset(index);

	if(ptn == 0)
	{
		return -1;
	}

	actual_size = ROUNDUP(sizeof(metadata_t), block_size);

	if (actual_size > MAX_METADATA_SIZE)
	{
		dprintf(CRITICAL, "mdtp: read_metadata: ERROR, meta data size %d too big\n", actual_size);
		return -1;
	}

	if(mmc_read(ptn, (void *)metadata_block, actual_size))
	{
		dprintf(CRITICAL, "mdtp: read_metadata: ERROR, cannot read mdtp info\n");
		return -1;
	}

	memscpy((uint8_t*)metadata, sizeof(metadata_t), (uint8_t*)(&(metadata_block[0])), MAX_METADATA_SIZE);

	dprintf(INFO, "mdtp: read_metadata: SUCCESS, read %d bytes\n", actual_size);

	return 0;
}

/**
 * read_QFPROM_fuse
 *
 * @param mask[out] - MDTP efuse value represented by a bitfield.
 *
 * @return - negative value for an error, 0 for success.
 */
static int read_QFPROM_fuse(uint8_t *mask)
{
	static const uint32_t row_address = MDTP_EFUSE_ADDRESS;
	uint32_t addr_type = QFPROM_ADDR_SPACE_RAW;
	uint32_t row_data[2] = {0};
	uint32_t qfprom_api_status = 0;

	/* Read the current row where the eFuse is located */
	(void) qfprom_read_row_cmd(row_address, addr_type, row_data, &qfprom_api_status);
	if (qfprom_api_status)
	{
			dprintf(CRITICAL, "mdtp: write_QFPROM_fuse: qsee_fuse_read failed. qfprom_api_status=%d", qfprom_api_status);
			return -1;
	}

	/* Shift the read data to be reflected in mask */
	*mask = (uint8_t)(row_data[0] >> MDTP_EFUSE_START);

	return 0;
}

/*-------------------------------------------------------------------------*/

/**
 * read_test_fuse
 *
 * @param mask[out] - MDTP efuse value represented by a bitfield.
 *
 * @return - negative value for an error, 0 for success.
 */
static int read_test_fuse(uint8_t *mask)
{
	int status = 0;
	metadata_t metadata;

	status = read_metadata(&metadata);
	if (status) {
		dprintf(CRITICAL, "mdtp: read_test_fuse: Failure getting metadata");
		return -1;
	}

	*mask = metadata.eFuses.mask;

	return 0;
}

/*-------------------------------------------------------------------------*/

/**
 * read_fuse
 *
 * @param mask[out] - MDTP efuse value represented by a bitfield.
 *
 * @return - negative value for an error, 0 for success.
 */
static int read_fuse(uint8_t *mask)
{
	if (is_test_mode())
		return read_test_fuse(mask);
	else
		return read_QFPROM_fuse(mask);
}

/*-------------------------------------------------------------------------*/

/**
 * mdtp_fuse_get_enabled
 *
 * Read the Firmware Lock eFuses and return whether the Firmware
 * Lock is currently enabled or disabled in HW.
 *
 * @param[out] enabled: 0 - disabled, 1 - enabled.
 *
 * @return - negative value for an error, 0 for success.
 */
int mdtp_fuse_get_enabled(bool *enabled)
{
	int status;
	mdtp_eFuses_t eFuses;

	*enabled = 1;

	status = read_fuse(&eFuses.mask);
	if (status)
	{
		dprintf(CRITICAL, "mdtp: mdtp_fuse_get_enabled: Failure in reading fuse");
		return -1;
	}

	if (!(eFuses.bitwise.enable1 && !eFuses.bitwise.disable1) &&
		!(eFuses.bitwise.enable2 && !eFuses.bitwise.disable2) &&
		!(eFuses.bitwise.enable3 && !eFuses.bitwise.disable3))
	{
		*enabled = 0;
	}

	return 0;
}

