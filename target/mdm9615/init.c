/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
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
#include <lib/ptable.h>
#include <dev/flash.h>
#include <dev/pm8921.h>
#include <dev/ssbi.h>
#include <string.h>
#include <smem.h>
#include <gsbi.h>
#include <platform/iomap.h>
#include <platform.h>

#define MDM9X15_CDP	3675
#define MDM9X15_MTP	3681
#define LINUX_MACHTYPE	MDM9X15_CDP

#define RECOVERY_MODE	0x77665502
#define FOTA_COOKIE	0x64645343
#define FOTA_PARTITION    5
#define DSP3_PARTITION    5

static struct ptable flash_ptable;
unsigned hw_platform = 0;
unsigned target_msm_id = 0;

/* Partition names for fastboot flash */
static char *apps_ptn_names[] = { "aboot", "boot", "cache", "misc", "recovery", "fota",
	"recoveryfs", "system", "userdata" };

/* Partitions should be in this order */
static char *ptable_ptn_names[] = { "APPSBL", "APPS", "CACHE", "MISC", "RECOVERY",
	"FOTA", "RECOVERYFS", "SYSTEM", "USERDATA" };

static unsigned ptn_name_count = 9;

/* Apps partition flags to detect the presence of FOTA partitions.
 * Initially, assume that the FOTA partitions are absent.
 */
static unsigned int apps_ptn_flag[] = {1, 1, 0, 0, 0, 0, 0, 1, 1};
/* Modem partition flags to detect the presence of DSP3 partitions.
 * Initially, assume that the DSP3 partition is absent.
 * The order of modem partiotions -
 * "mibib", "sbl2", "rpm", "efs2", "dsp1", "dsp3", "dsp2"
 */
static unsigned int modem_ptn_flag[] = {1, 1, 1, 1, 1, 0, 1};

static unsigned modem_ptn_count = 7;

static const uint8_t uart_gsbi_id = GSBI_ID_4;

static pm8921_dev_t pmic;

void smem_ptable_init(void);
extern void reboot(unsigned reboot_reason);
void update_ptable_apps_partitions(void);
void update_ptable_modem_partitions(void);
extern int fake_key_get_state(void);

void target_init(void)
{
	struct flash_info *flash_info;

	dprintf(INFO, "target_init()\n");

	/* Initialize PMIC driver */
	pmic.read = (pm8921_read_func) & pa1_ssbi2_read_bytes;
	pmic.write = (pm8921_write_func) & pa1_ssbi2_write_bytes;

	pm8921_init(&pmic);

	ptable_init(&flash_ptable);
	smem_ptable_init();

	flash_init();
	flash_info = flash_get_info();
	ASSERT(flash_info);

	smem_add_modem_partitions(&flash_ptable);

	/* Update the naming for apps partitions and type */
	update_ptable_apps_partitions();

	/* Update modem partitions to lower case for fastboot */
	update_ptable_modem_partitions();

	ptable_dump(&flash_ptable);
	flash_set_ptable(&flash_ptable);
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


static unsigned int get_fota_cookie(void)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned int cookie = 0;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return 0;
	}

	ptn = ptable_find(ptable, "fota");
	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No fota partition found\n");
		return 0;
	}
	if(read_from_flash(ptn, 0, sizeof(unsigned int), &cookie) == -1){
		dprintf(CRITICAL,"ERROR: failed to read fota cookie from flash");
		return 0;
	}
	return cookie;
}

unsigned check_reboot_mode(void)
{
	unsigned restart_reason = 0;
	unsigned int cookie = 0;

	/* Read reboot reason and scrub it */
	restart_reason = readl(RESTART_REASON_ADDR);
	writel(0x00, RESTART_REASON_ADDR);

	/* SMEM value is not relied upon on power shutdown.
	 * Check either of SMEM or FOTA update cookie is set.
	 * Check fota cookie only when fota partition is available on target
	 */
	if (apps_ptn_flag[FOTA_PARTITION] == 1) {
		cookie = get_fota_cookie();
		if (cookie == FOTA_COOKIE)
			return RECOVERY_MODE;
	}
	return restart_reason;
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

	hw_platform = MDM9X15_CDP;
	target_msm_id = MDM9600;

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
	}
	return;
}

unsigned board_machtype(void)
{
	board_info();
	return hw_platform;
}

void reboot_device(unsigned reboot_reason)
{
	/* Write reboot reason */
	writel(reboot_reason, RESTART_REASON_ADDR);

	/* Actually reset the chip */
	pm8921_config_reset_pwr_off(1);
	writel(0, MSM_PSHOLD_CTL_SU);
	mdelay(10000);

	dprintf(CRITICAL, "Rebooting failed\n");
	return;
}

uint8_t target_uart_gsbi(void)
{
	return uart_gsbi_id;
}

/*
 * Return 1 to trigger to fastboot
 */
int fastboot_trigger(void)
{
	int ret;
	ret = fake_key_get_state();
	/* Want to trigger when dip switch is on */
	return (ret);
}

void update_ptable_modem_partitions(void)
{
	uint32_t ptn_index, i = 0;
	uint32_t name_size;
	struct ptentry *ptentry_ptr = flash_ptable.parts;
	struct ptentry *dsp3_ptn;

	dsp3_ptn = ptable_find(&flash_ptable, "DSP3");

	/* Check for DSP3 partition and its size */
	if (dsp3_ptn != NULL)
		if (dsp3_ptn->length > 0)
			modem_ptn_flag[DSP3_PARTITION] = 1;

	for (ptn_index = 0; ptn_index < modem_ptn_count; ptn_index++)
	{
		if (!modem_ptn_flag[ptn_index])
			continue;

		name_size = strlen(ptentry_ptr[ptn_index].name);
		for (i = 0; i < name_size; i++)
		{
			ptentry_ptr[ptn_index].name[i] =
		    tolower(ptentry_ptr[ptn_index].name[i]);
		}
	}
}

void update_ptable_apps_partitions(void)
{
	uint32_t ptn_index, name_index = 0;
	uint32_t end = 0xffffffff;
	uint32_t name_size = strlen(ptable_ptn_names[name_index]);
	struct ptentry *ptentry_ptr = flash_ptable.parts;
	struct ptentry *fota_ptn;
	unsigned int size;

	fota_ptn = ptable_find(&flash_ptable, "FOTA");

	/* Check for FOTA partitions and their size */
	if (fota_ptn != NULL) {
		if (fota_ptn->length > 0) {
			/* FOTA partitions are present */
			apps_ptn_flag[2] = 1;
			apps_ptn_flag[3] = 1;
			apps_ptn_flag[4] = 1;
			apps_ptn_flag[5] = 1;
			apps_ptn_flag[6] = 1;

		}
	}

	for (ptn_index = 0; ptentry_ptr[ptn_index].start != end; ptn_index++) {
		if (!(strncmp(ptentry_ptr[ptn_index].name,
			      ptable_ptn_names[name_index], name_size))) {
			name_size = strlen(apps_ptn_names[name_index]);
			name_size++;	/* For null termination */

			/* Update the partition names to something familiar */
			if (name_size <= MAX_PTENTRY_NAME)
				strncpy(ptentry_ptr[ptn_index].name,
					apps_ptn_names[name_index], name_size);

			/* Aboot uses modem page layout, leave aboot ptn */
			if (name_index != 0)
				ptentry_ptr[ptn_index].type =
				    TYPE_APPS_PARTITION;

			/* Check for valid partitions
			 * according to the apps_ptn_flag
			 */
			do {
				/* Don't go out of bounds */
				name_index++;
				if (name_index >= ptn_name_count)
					goto ptn_name_update_done;
				name_size =
					strlen(ptable_ptn_names[name_index]);
			} while (!apps_ptn_flag[name_index]);
		}
	}

ptn_name_update_done:
	/* Update the end to be actual end for grow partition */
	ptn_index--;
	for (; ptentry_ptr[ptn_index].length != end; ptn_index++) {
	};

	/* If SMEM ptable is updated already then don't manually update this */
	if (ptentry_ptr[ptn_index].start != end)
		ptentry_ptr[ptn_index].length =
		    ((struct flash_info *)flash_get_info())->num_blocks -
		    ptentry_ptr[ptn_index].start;
}
