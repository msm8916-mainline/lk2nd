/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *	may be used to endorse or promote products derived from this
 *	software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <reg.h>
#include <debug.h>
#include <dev/keys.h>
#include <dev/ssbi.h>
#include <lib/ptable.h>
#include <dev/flash.h>
#include <smem.h>
#include <mmc.h>
#include <platform/iomap.h>
#include <target/board.h>
#include <platform.h>
#include <crypto_hash.h>

#define VARIABLE_LENGTH		0x10101010
#define DIFF_START_ADDR		0xF0F0F0F0
#define NUM_PAGES_PER_BLOCK	0x40
#define RECOVERY_MODE		0x77665502
#define FOTA_COOKIE		0x64645343

unsigned int fota_cookie[1];

static struct ptable flash_ptable;
unsigned hw_platform = 0;
unsigned target_msm_id = 0;

/* Setting this variable to different values defines the
 * behavior of CE engine:
 * platform_ce_type = CRYPTO_ENGINE_TYPE_NONE : No CE engine
 * platform_ce_type = CRYPTO_ENGINE_TYPE_SW : Software CE engine
 * platform_ce_type = CRYPTO_ENGINE_TYPE_HW : Hardware CE engine
 * Behavior is determined in the target code.
 */
static crypto_engine_type platform_ce_type = CRYPTO_ENGINE_TYPE_SW;

int machine_is_evb();

/* for these partitions, start will be offset by either what we get from
 * smem, or from the above offset if smem is not useful. Also, we should
 * probably have smem_ptable code populate our flash_ptable.
 *
 * When smem provides us with a full partition table, we can get rid of
 * this altogether.
 *
 */
static struct ptentry board_part_list[] = {
	{
	 .start = 0,
	 .length = 10 /* In MB */ ,
	 .name = "boot",
	 },
	{
	 .start = DIFF_START_ADDR,
	 .length = 253 /* In MB */ ,
	 .name = "system",
	 },
	{
	 .start = DIFF_START_ADDR,
	 .length = 40 /* In MB */ ,
	 .name = "cache",
	 },
	{
	 .start = DIFF_START_ADDR,
	 .length = 4 /* In MB */ ,
	 .name = "misc",
	 },
	{
	 .start = DIFF_START_ADDR,
	 .length = VARIABLE_LENGTH,
	 .name = "userdata",
	 },
	{
	 .start = DIFF_START_ADDR,
	 .length = 4 /* In MB */ ,
	 .name = "persist",
	 },
	{
	 .start = DIFF_START_ADDR,
	 .length = 10 /* In MB */ ,
	 .name = "recovery",
	 },
};

static int num_parts = sizeof(board_part_list) / sizeof(struct ptentry);

void smem_ptable_init(void);
unsigned smem_get_apps_flash_start(void);

void keypad_init(void);

int target_is_emmc_boot(void);

void target_init(void)
{
	unsigned offset;
	struct flash_info *flash_info;
	unsigned total_num_of_blocks;
	unsigned next_ptr_start_adr = 0;
	unsigned blocks_per_1MB = 8;	/* Default value of 2k page size on 256MB flash drive */
	int i;

	dprintf(INFO, "target_init()\n");

#if (!ENABLE_NANDWRITE)
	keys_init();
	keypad_init();
#endif

	/* Display splash screen if enabled */
#if DISPLAY_SPLASH_SCREEN
	display_init();
	dprintf(SPEW, "Diplay initialized\n");
#endif

	if (target_is_emmc_boot()) {
		/* Must wait for modem-up before we can intialize MMC.
		 */
		while (readl(MSM_SHARED_BASE + 0x14) != 1) ;

		if (mmc_boot_main(MMC_SLOT, MSM_SDC3_BASE)) {
			dprintf(CRITICAL, "mmc init failed!");
			ASSERT(0);
		}
		return;
	}

	ptable_init(&flash_ptable);
	smem_ptable_init();

	flash_init();
	flash_info = flash_get_info();
	ASSERT(flash_info);

	offset = smem_get_apps_flash_start();
	if (offset == 0xffffffff)
		while (1) ;

	total_num_of_blocks = flash_info->num_blocks;
	blocks_per_1MB = (1 << 20) / (flash_info->block_size);

	for (i = 0; i < num_parts; i++) {
		struct ptentry *ptn = &board_part_list[i];
		unsigned len = ((ptn->length) * blocks_per_1MB);

		if (ptn->start != 0)
			ASSERT(ptn->start == DIFF_START_ADDR);

		ptn->start = next_ptr_start_adr;

		if (ptn->length == VARIABLE_LENGTH) {
			unsigned length_for_prt = 0;
			unsigned j;
			for (j = i + 1; j < num_parts; j++) {
				struct ptentry *temp_ptn = &board_part_list[j];
				ASSERT(temp_ptn->length != VARIABLE_LENGTH);
				length_for_prt +=
				    ((temp_ptn->length) * blocks_per_1MB);
			}
			len =
			    (total_num_of_blocks - 1) - (offset + ptn->start +
							 length_for_prt);
			ASSERT(len >= 0);
		}
		next_ptr_start_adr = ptn->start + len;
		ptable_add(&flash_ptable, ptn->name, offset + ptn->start,
			   len, ptn->flags, TYPE_APPS_PARTITION,
			   PERM_WRITEABLE);
	}

	smem_add_modem_partitions(&flash_ptable);

	ptable_dump(&flash_ptable);
	flash_set_ptable(&flash_ptable);
}

void board_info(void)
{
	struct smem_board_info_v4 board_info_v4;
	unsigned int board_info_len = 0;
	unsigned smem_status;
	unsigned format = 0;
	unsigned id = 0;

	if (hw_platform && target_msm_id)
		return;

	hw_platform = MSM7X27A_SURF;
	target_msm_id = MSM7225A;

	smem_status = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
						   &format, sizeof(format), 0);
	if (!smem_status) {
		if (format == 4) {
			board_info_len = sizeof(board_info_v4);
			smem_status =
			    smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
						  &board_info_v4,
						  board_info_len);
			if (!smem_status) {
				id = board_info_v4.board_info_v3.hw_platform;
				target_msm_id =
				    board_info_v4.board_info_v3.msm_id;
			}
		}

		/* Detect SURF v/s FFA v/s QRD */
		if (target_msm_id >= MSM8225 && target_msm_id <= MSM8625) {
			switch (id) {
			case 0x1:
				hw_platform = MSM8X25_SURF;
				break;
			case 0x2:
				hw_platform = MSM8X25_FFA;
				break;
			case 0x10:
				hw_platform = MSM8X25_EVT;
				break;
			case 0xC:
				hw_platform = MSM8X25_EVB;
				break;
			case 0xF:
				hw_platform = MSM8X25_QRD7;
				break;
			default:
				hw_platform = MSM8X25_SURF;
			}
		} else {
			switch (id) {
			case 0x1:
				/* Set the machine type based on msm ID */
				if (msm_is_7x25a(target_msm_id))
					hw_platform = MSM7X25A_SURF;
				else
					hw_platform = MSM7X27A_SURF;
				break;
			case 0x2:
				if (msm_is_7x25a(target_msm_id))
					hw_platform = MSM7X25A_FFA;
				else
					hw_platform = MSM7X27A_FFA;
				break;
			case 0xB:
				if(target_is_emmc_boot())
					hw_platform = MSM7X27A_QRD1;
				else
					hw_platform = MSM7X27A_QRD3;
				break;
			case 0xC:
				hw_platform = MSM7X27A_EVB;
				break;
			case 0xF:
				hw_platform = MSM7X27A_QRD3;
				break;
			default:
				if (msm_is_7x25a(target_msm_id))
					hw_platform = MSM7X25A_SURF;
				else
					hw_platform = MSM7X27A_SURF;
			};
		}
		/* Set msm ID for target variants based on values read from smem */
		switch (target_msm_id) {
		case MSM7225A:
		case MSM7625A:
		case ESM7225A:
		case MSM7225AA:
		case MSM7625AA:
		case ESM7225AA:
		case MSM7225AB:
		case MSM7625AB:
		case ESM7225AB:
			target_msm_id = MSM7625A;
			break;
		case MSM8225:
		case MSM8625:
			target_msm_id = MSM8625;
			break;
		default:
			target_msm_id = MSM7627A;
		}
	}
	return;
}

unsigned board_machtype(void)
{
	board_info();
	return hw_platform;
}

unsigned board_msm_id(void)
{
	board_info();
	return target_msm_id;
}

crypto_engine_type board_ce_type(void)
{
	return platform_ce_type;
}

void reboot_device(unsigned reboot_reason)
{
	reboot(reboot_reason);
}

static int read_from_flash(struct ptentry* ptn, int offset, int size, void *dest)
{
	void *buffer = NULL;
	unsigned page_size = flash_page_size();
	unsigned page_mask = page_size - 1;
	int read_size = (size + page_mask) & (~page_mask);

	buffer = malloc(read_size);
	if(!buffer){
		dprintf(CRITICAL, "ERROR : Malloc failed for read_from_flash \n");
		return -1;
	}

	if(flash_read(ptn, offset, buffer, read_size)){
		dprintf(CRITICAL, "ERROR : Flash read failed \n");
		return -1;
	}
	memcpy(dest, buffer, size);
	free(buffer);
	return 0;
}

static unsigned int get_fota_cookie_mtd(void)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned int cookie = 0;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return 0;
	}

	ptn = ptable_find(ptable, "FOTA");
	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No FOTA partition found\n");
		return 0;
	}

	if (read_from_flash(ptn, 0, sizeof(unsigned int), &cookie) == -1) {
		dprintf(CRITICAL, "ERROR: failed to read fota cookie from flash\n");
		return 0;
	}
	return cookie;
}

static int read_from_mmc(struct ptentry* ptn, int size, void *dest)
{
	void *buffer = NULL;
	unsigned sector_mask = 511;
	int read_size =  (size + sector_mask) & (~sector_mask);

	buffer = malloc(read_size);
	if(!buffer){
		dprintf(CRITICAL, "ERROR : Malloc failed for read_from_flash \n");
		return -1;
	}

	if(mmc_read(ptn, buffer, read_size)) {
		dprintf(CRITICAL, "ERROR : Flash read failed \n");
		return -1;
	}
	memcpy(dest, buffer, size);
	free(buffer);
	return 0;
}

static int get_fota_cookie_mmc(void)
{
	unsigned long long ptn = 0;
	int index = -1;
	unsigned int cookie = 0;

	index = partition_get_index("FOTA");
	ptn = partition_get_offset(index);

	if(ptn == 0) {
		dprintf(CRITICAL,"ERROR: FOTA partition not found\n");
		return 0;
	}
	if(read_from_mmc(ptn, sizeof(unsigned int), &cookie)) {
		dprintf(CRITICAL, "ERROR: Cannot read cookie info\n");
		return 0;
	}
	return cookie;
}

unsigned check_reboot_mode(void)
{
	unsigned mode[2] = { 0, 0 };
	unsigned int mode_len = sizeof(mode);
	unsigned smem_status;
	unsigned int cookie = 0;

	smem_status = smem_read_alloc_entry(SMEM_APPS_BOOT_MODE,
					    &mode, mode_len);

	/*
	 * SMEM value is relied upon on power shutdown. Check either of SMEM
	 * or FOTA update cookie is set
	 */
	if (target_is_emmc_boot())
		cookie = get_fota_cookie_mmc();
	else
		cookie = get_fota_cookie_mtd();

	if ((mode[0] == RECOVERY_MODE) || (cookie == FOTA_COOKIE))
		return RECOVERY_MODE;

	if (smem_status) {
		dprintf(CRITICAL,
			"ERROR: unable to read shared memory for reboot mode\n");
		return 0;
	}
	return mode[0];
}

static unsigned target_check_power_on_reason(void)
{
	unsigned power_on_status = 0;
	unsigned int status_len = sizeof(power_on_status);
	unsigned smem_status;

	smem_status = smem_read_alloc_entry(SMEM_POWER_ON_STATUS_INFO,
					    &power_on_status, status_len);
	if (smem_status) {
		dprintf(CRITICAL,
			"ERROR: unable to read shared memory for power on reason\n");
	}

	return power_on_status;
}

unsigned target_pause_for_battery_charge(void)
{
	if (target_check_power_on_reason() == PWR_ON_EVENT_WALL_CHG)
		return 1;
	return 0;
}

void target_battery_charging_enable(unsigned enable, unsigned disconnect)
{
}

#if _EMMC_BOOT
void target_serialno(unsigned char *buf)
{
	unsigned int serialno;
	serialno = mmc_get_psn();
	sprintf(buf, "%x", serialno);
}

int emmc_recovery_init(void)
{
	int rc;
	rc = _emmc_recovery_init();
	return rc;
}
#endif

int machine_is_evb()
{
	int ret = 0;
	unsigned mach_type = board_machtype();

	switch(mach_type) {
		case MSM7X27A_EVB:
		case MSM8X25_EVB:
		case MSM8X25_EVT:
			ret = 1;
			break;
		default:
			ret = 0;
	}
	return ret;
}
int machine_is_qrd()
{
	int ret = 0;
	unsigned mach_type = board_machtype();

	switch(mach_type) {
		case MSM7X27A_QRD1:
		case MSM7X27A_QRD3:
		case MSM8X25_QRD7:
			ret = 1;
			break;
		default:
			ret = 0;
	}
	return ret;
}
int machine_is_8x25()
{
	int ret = 0;
	unsigned mach_type = board_machtype();

	switch(mach_type) {
		case MSM8X25_SURF:
		case MSM8X25_FFA:
		case MSM8X25_EVB:
		case MSM8X25_EVT:
		case MSM8X25_QRD7:
			ret = 1;
			break;
		default:
			ret = 0;
	}
	return ret;
}

int msm_is_7x25a(int msm_id)
{
	int ret = 0;

	switch (msm_id) {
		case MSM7225A:
		case MSM7625A:
		case ESM7225A:
		case MSM7225AA:
		case MSM7625AA:
		case ESM7225AA:
		case MSM7225AB:
		case MSM7625AB:
		case ESM7225AB:
			ret = 1;
			break;
		default:
			ret = 0;
	};
	return ret;
}
