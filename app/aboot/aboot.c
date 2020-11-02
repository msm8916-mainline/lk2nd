/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of The Linux Foundation nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <app.h>
#include <debug.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <kernel/thread.h>
#include <arch/ops.h>

#include <dev/flash.h>
#include <dev/flash-ubi.h>
#include <lib/ptable.h>
#include <dev/keys.h>
#include <dev/fbcon.h>
#include <baseband.h>
#include <target.h>
#include <mmc.h>
#include <partition_parser.h>
#include <platform.h>
#include <crypto_hash.h>
#include <malloc.h>
#include <boot_stats.h>
#if VERIFIED_BOOT
#include <sha.h>
#endif
#include <platform/iomap.h>
#include <boot_device.h>
#include <boot_verifier.h>
#include <decompress.h>
#include <platform/timer.h>
#include <sys/types.h>
#if USE_RPMB_FOR_DEVINFO
#include <rpmb.h>
#endif

#if DEVICE_TREE
#include <libfdt.h>
#include <dev_tree.h>
#include <lk2nd-device.h>
#endif

#include <reboot.h>
#include "image_verify.h"
#include "recovery.h"
#include "bootimg.h"
#include "fastboot.h"
#include "sparse_format.h"
#include "meta_format.h"
#include "mmc.h"
#include "devinfo.h"
#include "board.h"
#include "scm.h"
#include "secapp_loader.h"
#include <menu_keys_detect.h>
#include <display_menu.h>

extern  bool target_use_signed_kernel(void);
extern void platform_uninit(void);
extern void target_uninit(void);
extern int get_target_boot_params(const char *cmdline, const char *part,
				  char *buf, int buflen);

void *info_buf;
void write_device_info_mmc(device_info *dev);
void write_device_info_flash(device_info *dev);
static int aboot_save_boot_hash_mmc(uint32_t image_addr, uint32_t image_size);
extern void display_fbcon_message(char *str);
static int aboot_frp_unlock(char *pname, void *data, unsigned sz);
static inline uint64_t validate_partition_size();
bool pwr_key_is_pressed = false;
unsigned bytes_to_round_page = 0;
unsigned rounded_size = 0;

/* fastboot command function pointer */
typedef void (*fastboot_cmd_fn) (const char *, void *, unsigned);

struct fastboot_cmd_desc {
	char * name;
	fastboot_cmd_fn cb;
};

#define EXPAND(NAME) #NAME
#define TARGET(NAME) EXPAND(NAME)

#ifdef MEMBASE
#define EMMC_BOOT_IMG_HEADER_ADDR (0xFF000+(MEMBASE))
#else
#define EMMC_BOOT_IMG_HEADER_ADDR 0xFF000
#endif

#ifndef MEMSIZE
#define MEMSIZE 1024*1024
#endif

#define MAX_TAGS_SIZE   1024

/* make 4096 as default size to ensure EFS,EXT4's erasing */
#define DEFAULT_ERASE_SIZE  4096
#define MAX_PANEL_BUF_SIZE 128

#define DISPLAY_DEFAULT_PREFIX "mdss_mdp"
#define BOOT_DEV_MAX_LEN  64

#define IS_ARM64(ptr) (ptr->magic_64 == KERNEL64_HDR_MAGIC) ? true : false

#define ADD_OF(a, b) (UINT_MAX - b > a) ? (a + b) : UINT_MAX

#if UFS_SUPPORT || USE_BOOTDEV_CMDLINE
static const char *emmc_cmdline = " androidboot.bootdevice=";
#else
static const char *emmc_cmdline = " androidboot.emmc=true";
#endif
static const char *usb_sn_cmdline = " androidboot.serialno=";
static const char *androidboot_mode = " androidboot.mode=";
static const char *alarmboot_cmdline = " androidboot.alarmboot=true";
static const char *loglevel         = " quiet";
static const char *battchg_pause = " androidboot.mode=charger";
static const char *auth_kernel = " androidboot.authorized_kernel=true";
static const char *secondary_gpt_enable = " gpt";

static const char *baseband_apq     = " androidboot.baseband=apq";
static const char *baseband_msm     = " androidboot.baseband=msm";
static const char *baseband_csfb    = " androidboot.baseband=csfb";
static const char *baseband_svlte2a = " androidboot.baseband=svlte2a";
static const char *baseband_mdm     = " androidboot.baseband=mdm";
static const char *baseband_mdm2    = " androidboot.baseband=mdm2";
static const char *baseband_sglte   = " androidboot.baseband=sglte";
static const char *baseband_dsda    = " androidboot.baseband=dsda";
static const char *baseband_dsda2   = " androidboot.baseband=dsda2";
static const char *baseband_sglte2  = " androidboot.baseband=sglte2";
static const char *warmboot_cmdline = " qpnp-power-on.warm_boot=1";

#if VERIFIED_BOOT
#if !VBOOT_MOTA
static const char *verity_mode = " androidboot.veritymode=";
static const char *verified_state= " androidboot.verifiedbootstate=";
static const char *keymaster_v1= " androidboot.keymaster=1";
//indexed based on enum values, green is 0 by default

struct verified_boot_verity_mode vbvm[] =
{
#if ENABLE_VB_ATTEST
	{false, "eio"},
#else
	{false, "logging"},
#endif
	{true, "enforcing"},
};
struct verified_boot_state_name vbsn[] =
{
	{GREEN, "green"},
	{ORANGE, "orange"},
	{YELLOW,"yellow"},
	{RED,"red" },
};
#endif
#endif
/*As per spec delay wait time before shutdown in Red state*/
#define DELAY_WAIT 30000
static unsigned page_size = 0;
static unsigned page_mask = 0;
static unsigned mmc_blocksize = 0;
static unsigned mmc_blocksize_mask = 0;
static char ffbm_mode_string[FFBM_MODE_BUF_SIZE];
static bool boot_into_ffbm;
static char target_boot_params[64];
static bool boot_reason_alarm;
static bool devinfo_present = true;
static uint32_t dt_size = 0;

/* Assuming unauthorized kernel image by default */
static int auth_kernel_img = 0;

#if VBOOT_MOTA
static device_info device = {DEVICE_MAGIC, 0, 0, 0, 0, {0}, {0},{0}};
#else
static device_info device = {DEVICE_MAGIC, 0, 0, 0, 0, {0}, {0},{0}, 1, M_DEVICE_INFO_VER};
#endif

static bool is_allow_unlock = 0;

static char frp_ptns[2][8] = {"config","frp"};

static const char *critical_flash_allowed_ptn[] = {
	"aboot",
	"rpm",
	"tz",
	"sbl",
	"sdi",
	"sbl1",
	"xbl",
	"hyp",
	"pmic",
	"bootloader",
	"devinfo",
	"partition"};

struct atag_ptbl_entry
{
	char name[16];
	unsigned offset;
	unsigned size;
	unsigned flags;
};

/*
 * Partition info, required to be published
 * for fastboot
 */
struct getvar_partition_info {
	char part_name[MAX_GPT_NAME_SIZE]; /* Partition name */
	char getvar_size[MAX_GET_VAR_NAME_SIZE]; /* fastboot get var name for size */
	char getvar_type[MAX_GET_VAR_NAME_SIZE]; /* fastboot get var name for type */
	char size_response[MAX_RSP_SIZE];        /* fastboot response for size */
	char type_response[MAX_RSP_SIZE];        /* fastboot response for type */
};

/*
 * Update the part_type_known for known paritions types.
 */
struct getvar_partition_info part_info[NUM_PARTITIONS];
struct getvar_partition_info part_type_known[] =
{
	{ "system"  , "partition-size:", "partition-type:", "", "ext4" },
	{ "userdata", "partition-size:", "partition-type:", "", "ext4" },
	{ "cache"   , "partition-size:", "partition-type:", "", "ext4" },
};

char max_download_size[MAX_RSP_SIZE];
char charger_screen_enabled[MAX_RSP_SIZE];
char sn_buf[13];
char display_panel_buf[MAX_PANEL_BUF_SIZE];
char panel_display_mode[MAX_RSP_SIZE];
char get_variant[MAX_RSP_SIZE];
char battery_voltage[MAX_RSP_SIZE];

extern int emmc_recovery_init(void);

#if NO_KEYPAD_DRIVER
extern int fastboot_trigger(void);
#endif

static void update_ker_tags_rdisk_addr(struct boot_img_hdr *hdr, struct kernel64_hdr *kptr)
{
	/* overwrite the destination of specified for the project */
#ifdef ABOOT_IGNORE_BOOT_HEADER_ADDRS
	if (kptr && IS_ARM64(kptr))
		hdr->kernel_addr = ABOOT_FORCE_KERNEL64_ADDR;
	else
		hdr->kernel_addr = ABOOT_FORCE_KERNEL_ADDR;
	hdr->ramdisk_addr = ABOOT_FORCE_RAMDISK_ADDR;
	hdr->tags_addr = ABOOT_FORCE_TAGS_ADDR;

	/*
	 * ARM64 kernels specify the expected text offset in kptr->text_offset.
	 * However, this is not reliable until Linux 3.17.
	 * Check if image_size != 0 to detect newer kernels and use their
	 * expected offset in that case to avoid:
	 *
	 * [Firmware Bug]: Kernel image misaligned at boot, please fix your bootloader!
	 *
	 * See Linux commit a2c1d73b94ed49f5fac12e95052d7b140783f800.
	 */
	if (kptr && IS_ARM64(kptr) && kptr->image_size) {
		/* text_offset bytes from a 2MB aligned base address */
		hdr->kernel_addr &= ~0x1fffff;
		hdr->kernel_addr += kptr->text_offset;
	}
#endif
}

static void ptentry_to_tag(unsigned **ptr, struct ptentry *ptn)
{
	struct atag_ptbl_entry atag_ptn;

	memcpy(atag_ptn.name, ptn->name, 16);
	atag_ptn.name[15] = '\0';
	atag_ptn.offset = ptn->start;
	atag_ptn.size = ptn->length;
	atag_ptn.flags = ptn->flags;
	memcpy(*ptr, &atag_ptn, sizeof(struct atag_ptbl_entry));
	*ptr += sizeof(struct atag_ptbl_entry) / sizeof(unsigned);
}

static unsigned char *update_cmdline0(const char * cmdline)
{
	int cmdline_len = 0;
	int have_cmdline = 0;
	unsigned char *cmdline_final = NULL;
	int pause_at_bootup = 0;
	bool warm_boot = false;
	bool gpt_exists = partition_gpt_exists();
	int have_target_boot_params = 0;
	char *boot_dev_buf = NULL;
        bool is_mdtp_activated = 0;
#if VERIFIED_BOOT
#if !VBOOT_MOTA
    uint32_t boot_state = boot_verify_get_state();
#endif
#endif

#ifdef MDTP_SUPPORT
    mdtp_activated(&is_mdtp_activated);
#endif /* MDTP_SUPPORT */

	if (cmdline && cmdline[0]) {
		cmdline_len = strlen(cmdline);
		have_cmdline = 1;
	}
	if (target_is_emmc_boot()) {
		cmdline_len += strlen(emmc_cmdline);
#if UFS_SUPPORT || USE_BOOTDEV_CMDLINE
		boot_dev_buf = (char *) malloc(sizeof(char) * BOOT_DEV_MAX_LEN);
		ASSERT(boot_dev_buf);
		platform_boot_dev_cmdline(boot_dev_buf);
		cmdline_len += strlen(boot_dev_buf);
#endif
	}

	cmdline_len += strlen(usb_sn_cmdline);
	cmdline_len += strlen(sn_buf);

#if VERIFIED_BOOT
#if !VBOOT_MOTA
	cmdline_len += strlen(verified_state) + strlen(vbsn[boot_state].name);
	cmdline_len += strlen(verity_mode) + strlen(vbvm[device.verity_mode].name);
	cmdline_len += strlen(keymaster_v1);
#endif
#endif

	if (boot_into_recovery && gpt_exists)
		cmdline_len += strlen(secondary_gpt_enable);

	if (boot_into_ffbm) {
		cmdline_len += strlen(androidboot_mode);
		cmdline_len += strlen(ffbm_mode_string);
		/* reduce kernel console messages to speed-up boot */
		cmdline_len += strlen(loglevel);
	} else if (boot_reason_alarm) {
		cmdline_len += strlen(alarmboot_cmdline);
	} else if (device.charger_screen_enabled &&
			target_pause_for_battery_charge()) {
		pause_at_bootup = 1;
		cmdline_len += strlen(battchg_pause);
	}

	if(target_use_signed_kernel() && auth_kernel_img) {
		cmdline_len += strlen(auth_kernel);
	}

	if (get_target_boot_params(cmdline, boot_into_recovery ? "recoveryfs" :
								 "system",
				   target_boot_params,
				   sizeof(target_boot_params)) == 0) {
		have_target_boot_params = 1;
		cmdline_len += strlen(target_boot_params);
	}

	/* Determine correct androidboot.baseband to use */
	switch(target_baseband())
	{
		case BASEBAND_APQ:
			cmdline_len += strlen(baseband_apq);
			break;

		case BASEBAND_MSM:
			cmdline_len += strlen(baseband_msm);
			break;

		case BASEBAND_CSFB:
			cmdline_len += strlen(baseband_csfb);
			break;

		case BASEBAND_SVLTE2A:
			cmdline_len += strlen(baseband_svlte2a);
			break;

		case BASEBAND_MDM:
			cmdline_len += strlen(baseband_mdm);
			break;

		case BASEBAND_MDM2:
			cmdline_len += strlen(baseband_mdm2);
			break;

		case BASEBAND_SGLTE:
			cmdline_len += strlen(baseband_sglte);
			break;

		case BASEBAND_SGLTE2:
			cmdline_len += strlen(baseband_sglte2);
			break;

		case BASEBAND_DSDA:
			cmdline_len += strlen(baseband_dsda);
			break;

		case BASEBAND_DSDA2:
			cmdline_len += strlen(baseband_dsda2);
			break;
	}

	if (cmdline) {
		if ((strstr(cmdline, DISPLAY_DEFAULT_PREFIX) == NULL) &&
			target_display_panel_node(device.display_panel,
			display_panel_buf, MAX_PANEL_BUF_SIZE) &&
			strlen(display_panel_buf)) {
			cmdline_len += strlen(display_panel_buf);
		}
	}

	if (target_warm_boot()) {
		warm_boot = true;
		cmdline_len += strlen(warmboot_cmdline);
	}

	if (cmdline_len > 0) {
		const char *src;
		unsigned char *dst;

		cmdline_final = (unsigned char*) malloc((cmdline_len + 4) & (~3));
		ASSERT(cmdline_final != NULL);
		memset((void *)cmdline_final, 0, sizeof(*cmdline_final));
		dst = cmdline_final;

		/* Save start ptr for debug print */
		if (have_cmdline) {
			src = cmdline;
			while ((*dst++ = *src++));
		}
		if (target_is_emmc_boot()) {
			src = emmc_cmdline;
			if (have_cmdline) --dst;
			have_cmdline = 1;
			while ((*dst++ = *src++));
#if UFS_SUPPORT  || USE_BOOTDEV_CMDLINE
			src = boot_dev_buf;
			if (have_cmdline) --dst;
			while ((*dst++ = *src++));
#endif
		}

#if VERIFIED_BOOT
#if !VBOOT_MOTA
		src = verified_state;
		if(have_cmdline) --dst;
		have_cmdline = 1;
		while ((*dst++ = *src++));
		src = vbsn[boot_state].name;
		if(have_cmdline) --dst;
		while ((*dst++ = *src++));

		src = verity_mode;
		if(have_cmdline) --dst;
		while ((*dst++ = *src++));
		src = vbvm[device.verity_mode].name;
		if(have_cmdline) -- dst;
		while ((*dst++ = *src++));
		src = keymaster_v1;
		if(have_cmdline) --dst;
		while ((*dst++ = *src++));
#endif
#endif
		src = usb_sn_cmdline;
		if (have_cmdline) --dst;
		have_cmdline = 1;
		while ((*dst++ = *src++));
		src = sn_buf;
		if (have_cmdline) --dst;
		have_cmdline = 1;
		while ((*dst++ = *src++));
		if (warm_boot) {
			if (have_cmdline) --dst;
			src = warmboot_cmdline;
			while ((*dst++ = *src++));
		}

		if (boot_into_recovery && gpt_exists) {
			src = secondary_gpt_enable;
			if (have_cmdline) --dst;
			while ((*dst++ = *src++));
		}

		if (boot_into_ffbm) {
			src = androidboot_mode;
			if (have_cmdline) --dst;
			while ((*dst++ = *src++));
			src = ffbm_mode_string;
			if (have_cmdline) --dst;
			while ((*dst++ = *src++));
			src = loglevel;
			if (have_cmdline) --dst;
			while ((*dst++ = *src++));
		} else if (boot_reason_alarm) {
			src = alarmboot_cmdline;
			if (have_cmdline) --dst;
			while ((*dst++ = *src++));
		} else if (pause_at_bootup) {
			src = battchg_pause;
			if (have_cmdline) --dst;
			while ((*dst++ = *src++));
		}

		if(target_use_signed_kernel() && auth_kernel_img) {
			src = auth_kernel;
			if (have_cmdline) --dst;
			while ((*dst++ = *src++));
		}

		switch(target_baseband())
		{
			case BASEBAND_APQ:
				src = baseband_apq;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;

			case BASEBAND_MSM:
				src = baseband_msm;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;

			case BASEBAND_CSFB:
				src = baseband_csfb;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;

			case BASEBAND_SVLTE2A:
				src = baseband_svlte2a;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;

			case BASEBAND_MDM:
				src = baseband_mdm;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;

			case BASEBAND_MDM2:
				src = baseband_mdm2;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;

			case BASEBAND_SGLTE:
				src = baseband_sglte;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;

			case BASEBAND_SGLTE2:
				src = baseband_sglte2;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;

			case BASEBAND_DSDA:
				src = baseband_dsda;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;

			case BASEBAND_DSDA2:
				src = baseband_dsda2;
				if (have_cmdline) --dst;
				while ((*dst++ = *src++));
				break;
		}

		if (strlen(display_panel_buf)) {
			src = display_panel_buf;
			if (have_cmdline) --dst;
			while ((*dst++ = *src++));
		}

		if (have_target_boot_params) {
			if (have_cmdline) --dst;
			src = target_boot_params;
			while ((*dst++ = *src++));
		}
	}


	if (boot_dev_buf)
		free(boot_dev_buf);

	dprintf(INFO, "cmdline: %s\n", cmdline_final ? cmdline_final : "");
	return cmdline_final;
}

static char *concat_args(const char *a, const char *b)
{
	int lenA = strlen(a), lenB = strlen(b);
	char *r = malloc(lenA + lenB + 2);
	memcpy(r, a, lenA);
	r[lenA] = ' ';
	memcpy(r + lenA + 1, b, lenB + 1);
	return r;
}

unsigned char *update_cmdline(const char* cmdline)
{
	/* Only take cmdline from original bootloader if downstream or lk2nd */
	if (cmdline && lk2nd_dev.cmdline &&
	    (strstr(cmdline, "androidboot.hardware=qcom") || strstr(cmdline, "lk2nd")))
		return concat_args(cmdline, lk2nd_dev.cmdline);
	return strdup(cmdline);
}

unsigned *atag_core(unsigned *ptr)
{
	/* CORE */
	*ptr++ = 2;
	*ptr++ = 0x54410001;

	return ptr;

}

unsigned *atag_ramdisk(unsigned *ptr, void *ramdisk,
							   unsigned ramdisk_size)
{
	if (ramdisk_size) {
		*ptr++ = 4;
		*ptr++ = 0x54420005;
		*ptr++ = (unsigned)ramdisk;
		*ptr++ = ramdisk_size;
	}

	return ptr;
}

unsigned *atag_ptable(unsigned **ptr_addr)
{
	int i;
	struct ptable *ptable;

	if ((ptable = flash_get_ptable()) && (ptable->count != 0)) {
		*(*ptr_addr)++ = 2 + (ptable->count * (sizeof(struct atag_ptbl_entry) /
							sizeof(unsigned)));
		*(*ptr_addr)++ = 0x4d534d70;
		for (i = 0; i < ptable->count; ++i)
			ptentry_to_tag(ptr_addr, ptable_get(ptable, i));
	}

	return (*ptr_addr);
}

unsigned *atag_cmdline(unsigned *ptr, const char *cmdline)
{
	int cmdline_length = 0;
	int n;
	char *dest;

	cmdline_length = strlen((const char*)cmdline);
	n = (cmdline_length + 4) & (~3);

	*ptr++ = (n / 4) + 2;
	*ptr++ = 0x54410009;
	dest = (char *) ptr;
	while ((*dest++ = *cmdline++));
	ptr += (n / 4);

	return ptr;
}

unsigned *atag_end(unsigned *ptr)
{
	/* END */
	*ptr++ = 0;
	*ptr++ = 0;

	return ptr;
}

void generate_atags(unsigned *ptr, const char *cmdline,
                    void *ramdisk, unsigned ramdisk_size)
{
	unsigned *orig_ptr = ptr;
	ptr = atag_core(ptr);
	ptr = atag_ramdisk(ptr, ramdisk, ramdisk_size);
	ptr = target_atag_mem(ptr);

	/* Skip NAND partition ATAGS for eMMC boot */
	if (!target_is_emmc_boot()){
		ptr = atag_ptable(&ptr);
	}

	/*
	 * Atags size filled till + cmdline size + 1 unsigned for 4-byte boundary + 4 unsigned
	 * for atag identifier in atag_cmdline and atag_end should be with in MAX_TAGS_SIZE bytes
	 */
	if (((ptr - orig_ptr) + strlen(cmdline) + 5 * sizeof(unsigned)) <  MAX_TAGS_SIZE) {
		ptr = atag_cmdline(ptr, cmdline);
		ptr = atag_end(ptr);
	}
	else {
		dprintf(CRITICAL,"Crossing ATAGs Max size allowed\n");
		ASSERT(0);
	}
}

/* todo: give lk strtoul and nuke this */
static unsigned hex2unsigned(const char *x)
{
    unsigned n = 0;

    while(*x) {
        switch(*x) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            n = (n << 4) | (*x - '0');
            break;
        case 'a': case 'b': case 'c':
        case 'd': case 'e': case 'f':
            n = (n << 4) | (*x - 'a' + 10);
            break;
        case 'A': case 'B': case 'C':
        case 'D': case 'E': case 'F':
            n = (n << 4) | (*x - 'A' + 10);
            break;
        default:
            return n;
        }
        x++;
    }

    return n;
}

/* generate a unique locally administrated MAC */
unsigned char* generate_mac_address()
{
	int len, i;
	char sn[] = "00000000";
	unsigned char * mac;

	/* make sure we have exactly 8 char for serialno */
	len = MIN(strlen(sn_buf), 8);
	memcpy(&sn[8-len], sn_buf, len);

	mac = (unsigned char*) malloc(6*sizeof(unsigned char));
	ASSERT(mac != NULL);

	/* fill in the mac with serialno, use locally adminstrated pool */
	mac[0] = 0x02;
	mac[1] = 00;
	for (i = 3 ; i >= 0; i--)
	{
		mac[i+2] = hex2unsigned(&sn[2*i]);
		sn[2*i]=0;
	}

	return mac;
}

typedef void entry_func_ptr(unsigned, unsigned, unsigned*);
void boot_linux(void *kernel, unsigned *tags,
		const char *cmdline, unsigned machtype,
		void *ramdisk, unsigned ramdisk_size)
{
	unsigned char *final_cmdline;
#if DEVICE_TREE
	int ret = 0;
	unsigned char* mac;
#endif

	void (*entry)(unsigned, unsigned, unsigned*) = (entry_func_ptr*)(PA((addr_t)kernel));
	uint32_t tags_phys = PA((addr_t)tags);
	struct kernel64_hdr *kptr = (struct kernel64_hdr*)kernel;

	ramdisk = PA(ramdisk);

	final_cmdline = update_cmdline((const char*)cmdline);

#if DEVICE_TREE
	dprintf(INFO, "Updating device tree: start\n");

	mac = generate_mac_address();

	/* Update the Device Tree */
	ret = update_device_tree((void *)tags, final_cmdline, ramdisk, ramdisk_size, mac);
	if(ret)
	{
		dprintf(CRITICAL, "ERROR: Updating Device Tree Failed \n");
		ASSERT(0);
	}
	dprintf(INFO, "Updating device tree: done\n");
#else
	/* Generating the Atags */
	generate_atags(tags, final_cmdline, ramdisk, ramdisk_size);
#endif

	free(final_cmdline);

#if VERIFIED_BOOT
#if !VBOOT_MOTA
	if (device.verity_mode == 0) {
#if FBCON_DISPLAY_MSG
#if ENABLE_VB_ATTEST
		display_bootverify_menu(DISPLAY_MENU_EIO);
		wait_for_users_action();
		if(!pwr_key_is_pressed)
			shutdown_device();
#else
		display_bootverify_menu(DISPLAY_MENU_LOGGING);
#endif
		wait_for_users_action();
#else
		dprintf(CRITICAL,
			"The dm-verity is not started in enforcing mode.\nWait for 5 seconds before proceeding\n");
		mdelay(5000);
#endif
	}

#endif
#endif

#if VERIFIED_BOOT
	/* Write protect the device info */
	if (target_build_variant_user() && devinfo_present && mmc_write_protect("devinfo", 1))
	{
		dprintf(INFO, "Failed to write protect dev info\n");
		ASSERT(0);
	}
#endif

	/* Perform target specific cleanup */
	target_uninit();

	/* Turn off splash screen if enabled */
#if DISPLAY_SPLASH_SCREEN
	target_display_shutdown();
#endif


	dprintf(INFO, "booting linux @ %p, ramdisk @ %p (%d), tags/device tree @ %p\n",
		entry, ramdisk, ramdisk_size, tags_phys);

	enter_critical_section();

	/* do any platform specific cleanup before kernel entry */
	platform_uninit();

	arch_disable_cache(UCACHE);

#if ARM_WITH_MMU
	arch_disable_mmu();
#endif
	bs_set_timestamp(BS_KERNEL_ENTRY);

	if (IS_ARM64(kptr))
		/* Jump to a 64bit kernel */
		scm_elexec_call((paddr_t)kernel, tags_phys);
	else
		/* Jump to a 32bit kernel */
		entry(0, machtype, (unsigned*)tags_phys);
}

/* Function to check if the memory address range falls within the aboot
 * boundaries.
 * start: Start of the memory region
 * size: Size of the memory region
 */
int check_aboot_addr_range_overlap(uintptr_t start, uint32_t size)
{
	/* Check for boundary conditions. */
	if ((UINT_MAX - start) < size)
		return -1;

	/* Check for memory overlap. */
	if ((start < MEMBASE) && ((start + size) <= MEMBASE))
		return 0;
	else if (start >= (MEMBASE + MEMSIZE))
		return 0;
	else
		return -1;
}

/* Function to check if the memory address range falls beyond ddr region.
 * start: Start of the memory region
 * size: Size of the memory region
 */
int check_ddr_addr_range_bound(uintptr_t start, uint32_t size)
{
	uintptr_t ddr_pa_start_addr = PA(get_ddr_start());
	uint64_t ddr_size = smem_get_ddr_size();
	uint64_t ddr_pa_end_addr = ddr_pa_start_addr + ddr_size;
	uintptr_t pa_start_addr = PA(start);

	/* Check for boundary conditions. */
	if ((UINT_MAX - start) < size)
		return -1;

	/* Check if memory range is beyond the ddr range. */
	if (pa_start_addr < ddr_pa_start_addr ||
		pa_start_addr >= (ddr_pa_end_addr) ||
		(pa_start_addr + size) > ddr_pa_end_addr)
		return -1;
	else
		return 0;
}

BUF_DMA_ALIGN(buf, BOOT_IMG_MAX_PAGE_SIZE); //Equal to max-supported pagesize

static void verify_signed_bootimg(uint32_t bootimg_addr, uint32_t bootimg_size)
{
	int ret;
#if IMAGE_VERIF_ALGO_SHA1
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA1;
#else
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA256;
#endif

	/* Assume device is rooted at this time. */
	device.is_tampered = 1;

	dprintf(INFO, "Authenticating boot image (%d): start\n", bootimg_size);

#if VERIFIED_BOOT
	if(boot_into_recovery)
	{
		ret = boot_verify_image((unsigned char *)bootimg_addr,
				bootimg_size, "/recovery");
	}
	else
	{
		ret = boot_verify_image((unsigned char *)bootimg_addr,
				bootimg_size, "/boot");
	}
	boot_verify_print_state();
#else
	ret = image_verify((unsigned char *)bootimg_addr,
					   (unsigned char *)(bootimg_addr + bootimg_size),
					   bootimg_size,
					   auth_algo);
#endif
	dprintf(INFO, "Authenticating boot image: done return value = %d\n", ret);

	if (ret)
	{
		/* Authorized kernel */
		device.is_tampered = 0;
		auth_kernel_img = 1;
	}

#if USE_PCOM_SECBOOT
	set_tamper_flag(device.is_tampered);
#endif

#if VERIFIED_BOOT
	switch(boot_verify_get_state())
	{
		case RED:
#if FBCON_DISPLAY_MSG
			display_bootverify_menu(DISPLAY_MENU_RED);
#if ENABLE_VB_ATTEST
			mdelay(DELAY_WAIT);
			shutdown_device();
#else
			wait_for_users_action();
#endif
#else
			dprintf(CRITICAL,
					"Your device has failed verification and may not work properly.\nWait for 5 seconds before proceeding\n");
			mdelay(5000);
#endif

			break;
		case YELLOW:
#if FBCON_DISPLAY_MSG
			display_bootverify_menu(DISPLAY_MENU_YELLOW);
			wait_for_users_action();
#else
			dprintf(CRITICAL,
					"Your device has loaded a different operating system.\nWait for 5 seconds before proceeding\n");
			mdelay(5000);
#endif
			break;
		default:
			break;
	}
#endif
#if !VERIFIED_BOOT
	if(device.is_tampered)
	{
		write_device_info_mmc(&device);
	#ifdef TZ_TAMPER_FUSE
		set_tamper_fuse_cmd();
	#endif
	#ifdef ASSERT_ON_TAMPER
		dprintf(CRITICAL, "Device is tampered. Asserting..\n");
		ASSERT(0);
	#endif
	}
#endif
}

static bool check_format_bit()
{
	bool ret = false;
	int index;
	uint64_t offset;
	struct boot_selection_info *in = NULL;
	char *buf = NULL;

	index = partition_get_index("bootselect");
	if (index == INVALID_PTN)
	{
		dprintf(INFO, "Unable to locate /bootselect partition\n");
		return ret;
	}
	offset = partition_get_offset(index);
	if(!offset)
	{
		dprintf(INFO, "partition /bootselect doesn't exist\n");
		return ret;
	}
	buf = (char *) memalign(CACHE_LINE, ROUNDUP(page_size, CACHE_LINE));
	ASSERT(buf);
	if (mmc_read(offset, (unsigned int *)buf, page_size))
	{
		dprintf(INFO, "mmc read failure /bootselect %d\n", page_size);
		free(buf);
		return ret;
	}
	in = (struct boot_selection_info *) buf;
	if ((in->signature == BOOTSELECT_SIGNATURE) &&
			(in->version == BOOTSELECT_VERSION)) {
		if ((in->state_info & BOOTSELECT_FORMAT) &&
				!(in->state_info & BOOTSELECT_FACTORY))
			ret = true;
	} else {
		dprintf(CRITICAL, "Signature: 0x%08x or version: 0x%08x mismatched of /bootselect\n",
				in->signature, in->version);
		ASSERT(0);
	}
	free(buf);
	return ret;
}

#if VERIFIED_BOOT
void boot_verifier_init()
{

	uint32_t boot_state;
	/* Check if device unlock */
	if(device.is_unlocked)
	{
		boot_verify_send_event(DEV_UNLOCK);
		boot_verify_print_state();
		dprintf(CRITICAL, "Device is unlocked! Skipping verification...\n");
		return;
	}
	else
	{
		boot_verify_send_event(BOOT_INIT);
	}

	/* Initialize keystore */
	boot_state = boot_verify_keystore_init();
	if(boot_state == YELLOW)
	{
		boot_verify_print_state();
		dprintf(CRITICAL, "Keystore verification failed! Continuing anyways...\n");
	}
}
#endif

int boot_linux_from_mmc(void)
{
	struct boot_img_hdr *hdr = (void*) buf;
	struct boot_img_hdr *uhdr;
	unsigned offset = 0;
	int rcode;
	unsigned long long ptn = 0;
	int index = INVALID_PTN;

	unsigned char *image_addr = 0;
	unsigned kernel_actual;
	unsigned ramdisk_actual;
	unsigned imagesize_actual;
	unsigned second_actual = 0;

	unsigned int dtb_size = 0;
	unsigned int out_len = 0;
	unsigned int out_avai_len = 0;
	unsigned char *out_addr = NULL;
	uint32_t dtb_offset = 0;
	unsigned char *kernel_start_addr = NULL;
	unsigned int kernel_size = 0;
	int rc;

#if DEVICE_TREE
	struct dt_table *table;
	struct dt_entry dt_entry;
	unsigned dt_table_offset;
	uint32_t dt_actual;
	uint32_t dt_hdr_size;
	unsigned char *best_match_dt_addr = NULL;
#endif
	struct kernel64_hdr *kptr = NULL;

	if (check_format_bit())
		boot_into_recovery = 1;

	if (!boot_into_recovery) {
		memset(ffbm_mode_string, '\0', sizeof(ffbm_mode_string));
		rcode = get_ffbm(ffbm_mode_string, sizeof(ffbm_mode_string));
		if (rcode <= 0) {
			boot_into_ffbm = false;
			if (rcode < 0)
				dprintf(CRITICAL,"failed to get ffbm cookie");
		} else
			boot_into_ffbm = true;
	} else
		boot_into_ffbm = false;
	uhdr = (struct boot_img_hdr *)EMMC_BOOT_IMG_HEADER_ADDR;
	if (!memcmp(uhdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		dprintf(INFO, "Unified boot method!\n");
		hdr = uhdr;
		goto unified_boot;
	}
	if (!boot_into_recovery) {
		index = partition_get_index("boot");
		ptn = partition_get_offset(index);
		if(ptn == 0) {
			dprintf(CRITICAL, "ERROR: No boot partition found\n");
                    return -1;
		}
	}
	else {
		index = partition_get_index("recovery");
		ptn = partition_get_offset(index);
		if(ptn == 0) {
			dprintf(CRITICAL, "ERROR: No recovery partition found\n");
                    return -1;
		}
	}

	if (mmc_read(ptn + offset, (unsigned int *) buf, page_size)) {
		dprintf(CRITICAL, "ERROR: Cannot read boot image header\n");
                return -1;
	}

	if (memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		dprintf(CRITICAL, "ERROR: Invalid boot image header\n");
                return -1;
	}

	if (hdr->page_size && (hdr->page_size != page_size)) {

		if (hdr->page_size > BOOT_IMG_MAX_PAGE_SIZE) {
			dprintf(CRITICAL, "ERROR: Invalid page size\n");
			return -1;
		}
		page_size = hdr->page_size;
		page_mask = page_size - 1;
	}

	kernel_actual  = ROUND_TO_PAGE(hdr->kernel_size,  page_mask);
	ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
	second_actual  = ROUND_TO_PAGE(hdr->second_size, page_mask);

	image_addr = (unsigned char *)target_get_scratch_address();
	memcpy(image_addr, (void *)buf, page_size);

	/* ensure commandline is terminated */
        hdr->cmdline[BOOT_ARGS_SIZE-1] = 0;

#if DEVICE_TREE
#ifndef OSVERSION_IN_BOOTIMAGE
	dt_size = hdr->dt_size;
#endif
	dt_actual = ROUND_TO_PAGE(dt_size, page_mask);
	if (UINT_MAX < ((uint64_t)kernel_actual + (uint64_t)ramdisk_actual+ (uint64_t)second_actual + (uint64_t)dt_actual + page_size)) {
		dprintf(CRITICAL, "Integer overflow detected in bootimage header fields at %u in %s\n",__LINE__,__FILE__);
		return -1;
	}
	imagesize_actual = (page_size + kernel_actual + ramdisk_actual + second_actual + dt_actual);
#else
	if (UINT_MAX < ((uint64_t)kernel_actual + (uint64_t)ramdisk_actual + (uint64_t)second_actual + page_size)) {
		dprintf(CRITICAL, "Integer overflow detected in bootimage header fields at %u in %s\n",__LINE__,__FILE__);
		return -1;
	}
	imagesize_actual = (page_size + kernel_actual + ramdisk_actual + second_actual);
#endif

#if VERIFIED_BOOT
	boot_verifier_init();
#endif

	if (check_aboot_addr_range_overlap((uintptr_t) image_addr, imagesize_actual))
	{
		dprintf(CRITICAL, "Boot image buffer address overlaps with aboot addresses.\n");
		return -1;
	}

	/*
	 * Update loading flow of bootimage to support compressed/uncompressed
	 * bootimage on both 64bit and 32bit platform.
	 * 1. Load bootimage from emmc partition onto DDR.
	 * 2. Check if bootimage is gzip format. If yes, decompress compressed kernel
	 * 3. Check kernel header and update kernel load addr for 64bit and 32bit
	 *    platform accordingly.
	 * 4. Sanity Check on kernel_addr and ramdisk_addr and copy data.
	 */

	dprintf(INFO, "Loading boot image (%d): start\n", imagesize_actual);
	bs_set_timestamp(BS_KERNEL_LOAD_START);

	offset = page_size;
	/* Read image without signature and header*/
	if (mmc_read(ptn + offset, (void *)(image_addr + offset), imagesize_actual - page_size))
	{
		dprintf(CRITICAL, "ERROR: Cannot read boot image\n");
		return -1;
	}

	dprintf(INFO, "Loading boot image (%d): done\n", imagesize_actual);
	bs_set_timestamp(BS_KERNEL_LOAD_DONE);

	/* Authenticate Kernel */
	dprintf(INFO, "use_signed_kernel=%d, is_unlocked=%d, is_tampered=%d.\n",
		(int) target_use_signed_kernel(),
		device.is_unlocked,
		device.is_tampered);

	if(target_use_signed_kernel() && (!device.is_unlocked))
	{
		offset = imagesize_actual;
		if (check_aboot_addr_range_overlap((uintptr_t)image_addr + offset, page_size))
		{
			dprintf(CRITICAL, "Signature read buffer address overlaps with aboot addresses.\n");
			return -1;
		}

		/* Read signature */
		if(mmc_read(ptn + offset, (void *)(image_addr + offset), page_size))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image signature\n");
			return -1;
		}

		verify_signed_bootimg((uint32_t)image_addr, imagesize_actual);
	} else {
		second_actual  = ROUND_TO_PAGE(hdr->second_size,  page_mask);
		#ifdef TZ_SAVE_KERNEL_HASH
		aboot_save_boot_hash_mmc((uint32_t) image_addr, imagesize_actual);
		#endif /* TZ_SAVE_KERNEL_HASH */

#if VERIFIED_BOOT
#if !VBOOT_MOTA
	if(boot_verify_get_state() == ORANGE)
	{
#if FBCON_DISPLAY_MSG
		display_bootverify_menu(DISPLAY_MENU_ORANGE);
		wait_for_users_action();
#else
		dprintf(CRITICAL,
			"Your device has been unlocked and can't be trusted.\nWait for 5 seconds before proceeding\n");
		mdelay(5000);
#endif
	}
#endif /* !VBOOT_MOTA */
#endif

#ifdef MDTP_SUPPORT
		{
			/* Verify MDTP lock.
			 * For boot & recovery partitions, MDTP will use boot_verifier APIs,
			 * since verification was skipped in aboot. The signature is not part of the loaded image.
			 */
			mdtp_ext_partition_verification_t ext_partition;
			ext_partition.partition = boot_into_recovery ? MDTP_PARTITION_RECOVERY : MDTP_PARTITION_BOOT;
			ext_partition.integrity_state = MDTP_PARTITION_STATE_UNSET;
			ext_partition.page_size = page_size;
			ext_partition.image_addr = (uint32)image_addr;
			ext_partition.image_size = imagesize_actual;
			ext_partition.sig_avail = FALSE;
			mdtp_fwlock_verify_lock(&ext_partition);
		}
#endif /* MDTP_SUPPORT */
	}

#if VERIFIED_BOOT
#if !VBOOT_MOTA
	// send root of trust
	if(!send_rot_command((uint32_t)device.is_unlocked))
		ASSERT(0);
#endif
#endif
	/*
	 * Check if the kernel image is a gzip package. If yes, need to decompress it.
	 * If not, continue booting.
	 */
	if (is_gzip_package((unsigned char *)(image_addr + page_size), hdr->kernel_size))
	{
		out_addr = (unsigned char *)(image_addr + imagesize_actual + page_size);
		out_avai_len = target_get_max_flash_size() - imagesize_actual - page_size;
		dprintf(SPEW, "decompress image start\n");
		rc = decompress((unsigned char *)(image_addr + page_size),
				hdr->kernel_size, out_addr, out_avai_len,
				&dtb_offset, &out_len);
		if (rc)
		{
			dprintf(CRITICAL, "decompress image failed!!!\n");
			ASSERT(0);
		}

		dprintf(SPEW, "decompressed image finished.\n");
		kptr = (struct kernel64_hdr *)out_addr;
		kernel_start_addr = out_addr;
		kernel_size = out_len;
	} else {
		kptr = (struct kernel64_hdr *)(image_addr + page_size);
		kernel_start_addr = (unsigned char *)(image_addr + page_size);
		kernel_size = hdr->kernel_size;
	}

	/*
	 * Update the kernel/ramdisk/tags address if the boot image header
	 * has default values, these default values come from mkbootimg when
	 * the boot image is flashed using fastboot flash:raw
	 */
	update_ker_tags_rdisk_addr(hdr, kptr);

	/* Get virtual addresses since the hdr saves physical addresses. */
	hdr->kernel_addr = VA((addr_t)(hdr->kernel_addr));
	hdr->ramdisk_addr = VA((addr_t)(hdr->ramdisk_addr));
	hdr->tags_addr = VA((addr_t)(hdr->tags_addr));

	kernel_size = ROUND_TO_PAGE(kernel_size,  page_mask);
	/* Check if the addresses in the header are valid. */
	if (check_aboot_addr_range_overlap(hdr->kernel_addr, kernel_size) ||
		check_ddr_addr_range_bound(hdr->kernel_addr, kernel_size) ||
		check_aboot_addr_range_overlap(hdr->ramdisk_addr, ramdisk_actual) ||
		check_ddr_addr_range_bound(hdr->ramdisk_addr, ramdisk_actual))
	{
		dprintf(CRITICAL, "kernel/ramdisk addresses are not valid.\n");
		return -1;
	}

#ifndef DEVICE_TREE
	if (check_aboot_addr_range_overlap(hdr->tags_addr, MAX_TAGS_SIZE) ||
		check_ddr_addr_range_bound(hdr->tags_addr, MAX_TAGS_SIZE))
	{
		dprintf(CRITICAL, "Tags addresses are not valid.\n");
		return -1;
	}
#endif

	/* Move kernel, ramdisk and device tree to correct address */
	memmove((void*) hdr->kernel_addr, kernel_start_addr, kernel_size);
	memmove((void*) hdr->ramdisk_addr, (char *)(image_addr + page_size + kernel_actual), hdr->ramdisk_size);

	#if DEVICE_TREE
	if(dt_size) {
		dt_table_offset = ((uint32_t)image_addr + page_size + kernel_actual + ramdisk_actual + second_actual);
		table = (struct dt_table*) dt_table_offset;

		if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0) {
			dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
			return -1;
		}

		/* Its Error if, dt_hdr_size (table->num_entries * dt_entry size + Dev_Tree Header)
		goes beyound hdr->dt_size*/
		if (dt_hdr_size > ROUND_TO_PAGE(dt_size,hdr->page_size)) {
			dprintf(CRITICAL, "ERROR: Invalid Device Tree size \n");
			return -1;
		}

		/* Find index of device tree within device tree table */
		if(dev_tree_get_entry_info(table, &dt_entry) != 0){
			dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
			return -1;
		}

		if(dt_entry.offset > (UINT_MAX - dt_entry.size)) {
			dprintf(CRITICAL, "ERROR: Device tree contents are Invalid\n");
			return -1;
		}

		/* Ensure we are not overshooting dt_size with the dt_entry selected */
		if ((dt_entry.offset + dt_entry.size) > dt_size) {
			dprintf(CRITICAL, "ERROR: Device tree contents are Invalid\n");
			return -1;
		}

		if (is_gzip_package((unsigned char *)dt_table_offset + dt_entry.offset, dt_entry.size))
		{
			unsigned int compressed_size = 0;
			out_addr += out_len;
			out_avai_len -= out_len;
			dprintf(SPEW, "decompress dtb start\n");
			rc = decompress((unsigned char *)dt_table_offset + dt_entry.offset,
					dt_entry.size, out_addr, out_avai_len,
					&compressed_size, &dtb_size);
			if (rc)
			{
				dprintf(CRITICAL, "decompress dtb failed!!!\n");
				ASSERT(0);
			}

			dprintf(SPEW, "decompressed dtb finished.\n");
			best_match_dt_addr = out_addr;
		} else {
			best_match_dt_addr = (unsigned char *)dt_table_offset + dt_entry.offset;
			dtb_size = dt_entry.size;
		}

		/* Validate and Read device device tree in the tags_addr */
		if (check_aboot_addr_range_overlap(hdr->tags_addr, dtb_size) ||
			check_ddr_addr_range_bound(hdr->tags_addr, dtb_size))
		{
			dprintf(CRITICAL, "Device tree addresses are not valid\n");
			return -1;
		}

		memmove((void *)hdr->tags_addr, (char *)best_match_dt_addr, dtb_size);
	} else {
		/* Validate the tags_addr */
		if (check_aboot_addr_range_overlap(hdr->tags_addr, kernel_actual) ||
			check_ddr_addr_range_bound(hdr->tags_addr, kernel_actual))
		{
			dprintf(CRITICAL, "Device tree addresses are not valid.\n");
			return -1;
		}
		/*
		 * If appended dev tree is found, update the atags with
		 * memory address to the DTB appended location on RAM.
		 * Else update with the atags address in the kernel header
		 */
		void *dtb;
		dtb = dev_tree_appended((void*)(image_addr + page_size),
					hdr->kernel_size, dtb_offset,
					(void *)hdr->tags_addr);
		if (!dtb) {
			dprintf(CRITICAL, "ERROR: Appended Device Tree Blob not found\n");
			return -1;
		}
	}
	#endif

	if (boot_into_recovery && !device.is_unlocked && !device.is_tampered)
		target_load_ssd_keystore();

unified_boot:

	boot_linux((void *)hdr->kernel_addr, (void *)hdr->tags_addr,
		   (const char *)hdr->cmdline, board_machtype(),
		   (void *)hdr->ramdisk_addr, hdr->ramdisk_size);

	return 0;
}

int boot_linux_from_flash(void)
{
	struct boot_img_hdr *hdr = (void*) buf;
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;

	unsigned char *image_addr = 0;
	unsigned kernel_actual;
	unsigned ramdisk_actual;
	unsigned imagesize_actual;
	unsigned second_actual = 0;

#if DEVICE_TREE
	struct dt_table *table;
	struct dt_entry dt_entry;
	unsigned dt_table_offset;
	uint32_t dt_actual;
	uint32_t dt_hdr_size;
	unsigned int dtb_size =0;
	unsigned char *best_match_dt_addr = NULL;
#endif

	if (target_is_emmc_boot()) {
		hdr = (struct boot_img_hdr *)EMMC_BOOT_IMG_HEADER_ADDR;
		if (memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
			dprintf(CRITICAL, "ERROR: Invalid boot image header\n");
			return -1;
		}
		goto continue_boot;
	}

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}

	if(!boot_into_recovery)
	{
	        ptn = ptable_find(ptable, "boot");

	        if (ptn == NULL) {
		        dprintf(CRITICAL, "ERROR: No boot partition found\n");
		        return -1;
	        }
	}
	else
	{
	        ptn = ptable_find(ptable, "recovery");
	        if (ptn == NULL) {
		        dprintf(CRITICAL, "ERROR: No recovery partition found\n");
		        return -1;
	        }
	}

	/* Read boot.img header from flash */
	if (flash_read(ptn, offset, buf, page_size)) {
		dprintf(CRITICAL, "ERROR: Cannot read boot image header\n");
		return -1;
	}

	if (memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		dprintf(CRITICAL, "ERROR: Invalid boot image header\n");
		return -1;
	}

	if (hdr->page_size != page_size) {
		dprintf(CRITICAL, "ERROR: Invalid boot image pagesize. Device pagesize: %d, Image pagesize: %d\n",page_size,hdr->page_size);
		return -1;
	}

	image_addr = (unsigned char *)target_get_scratch_address();
	memcpy(image_addr, (void *)buf, page_size);

	/*
	 * Update the kernel/ramdisk/tags address if the boot image header
	 * has default values, these default values come from mkbootimg when
	 * the boot image is flashed using fastboot flash:raw
	 */
	update_ker_tags_rdisk_addr(hdr, NULL);

	/* Get virtual addresses since the hdr saves physical addresses. */
	hdr->kernel_addr = VA((addr_t)(hdr->kernel_addr));
	hdr->ramdisk_addr = VA((addr_t)(hdr->ramdisk_addr));
	hdr->tags_addr = VA((addr_t)(hdr->tags_addr));

	kernel_actual  = ROUND_TO_PAGE(hdr->kernel_size,  page_mask);
	ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
	second_actual = ROUND_TO_PAGE(hdr->second_size, page_mask);

	/* ensure commandline is terminated */
	hdr->cmdline[BOOT_ARGS_SIZE-1] = 0;

	/* Check if the addresses in the header are valid. */
	if (check_aboot_addr_range_overlap(hdr->kernel_addr, kernel_actual) ||
		check_ddr_addr_range_bound(hdr->kernel_addr, kernel_actual) ||
		check_aboot_addr_range_overlap(hdr->ramdisk_addr, ramdisk_actual) ||
		check_ddr_addr_range_bound(hdr->ramdisk_addr, ramdisk_actual))
	{
		dprintf(CRITICAL, "kernel/ramdisk addresses are not valid.\n");
		return -1;
	}

#ifndef DEVICE_TREE
	if (UINT_MAX < ((uint64_t)kernel_actual + (uint64_t)ramdisk_actual+ (uint64_t)second_actual + page_size)) {
		dprintf(CRITICAL, "Integer overflow detected in bootimage header fields\n");
		return -1;
	}
	imagesize_actual = (page_size + kernel_actual + ramdisk_actual + second_actual);

	if (check_aboot_addr_range_overlap(hdr->tags_addr, MAX_TAGS_SIZE) ||
		check_ddr_addr_range_bound(hdr->tags_addr, MAX_TAGS_SIZE))
	{
		dprintf(CRITICAL, "Tags addresses are not valid.\n");
		return -1;
	}
#else

#ifndef OSVERSION_IN_BOOTIMAGE
	dt_size = hdr->dt_size;
#endif
	dt_actual = ROUND_TO_PAGE(dt_size, page_mask);
	if (UINT_MAX < ((uint64_t)kernel_actual + (uint64_t)ramdisk_actual+ (uint64_t)second_actual + (uint64_t)dt_actual + page_size)) {
		dprintf(CRITICAL, "Integer overflow detected in bootimage header fields\n");
		return -1;
	}

	imagesize_actual = (page_size + kernel_actual + ramdisk_actual + second_actual + dt_actual);

	if (check_aboot_addr_range_overlap(hdr->tags_addr, dt_size) ||
		check_ddr_addr_range_bound(hdr->tags_addr, dt_size))
	{
		dprintf(CRITICAL, "Device tree addresses are not valid.\n");
		return -1;
	}
#endif

	/* Read full boot.img from flash */
	dprintf(INFO, "Loading (%s) image (%d): start\n",
		(!boot_into_recovery ? "boot" : "recovery"),imagesize_actual);
	bs_set_timestamp(BS_KERNEL_LOAD_START);

	if (UINT_MAX - page_size < imagesize_actual)
	{
		dprintf(CRITICAL,"Integer overflow detected in bootimage header fields %u %s\n", __LINE__,__func__);
		return -1;
	}

	/*Check the availability of RAM before reading boot image + max signature length from flash*/
	if (target_get_max_flash_size() < (imagesize_actual + page_size))
	{
		dprintf(CRITICAL, "bootimage  size is greater than DDR can hold\n");
		return -1;
	}

	offset = page_size;
	/* Read image without signature and header */
	if (flash_read(ptn, offset, (void *)(image_addr + offset), imagesize_actual - page_size))
	{
		dprintf(CRITICAL, "ERROR: Cannot read boot image\n");
			return -1;
	}

	dprintf(INFO, "Loading (%s) image (%d): done\n",
		(!boot_into_recovery ? "boot" : "recovery"), imagesize_actual);
	bs_set_timestamp(BS_KERNEL_LOAD_DONE);

	/* Authenticate Kernel */
	if(target_use_signed_kernel() && (!device.is_unlocked))
	{
		offset = imagesize_actual;

		/* Read signature */
		if (flash_read(ptn, offset, (void *)(image_addr + offset), page_size))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image signature\n");
			return -1;
		}

		verify_signed_bootimg((uint32_t)image_addr, imagesize_actual);
	}
	offset = page_size;
	if(hdr->second_size != 0) {
		if (UINT_MAX - offset < second_actual)
		{
			dprintf(CRITICAL, "ERROR: Integer overflow in boot image header %s\t%d\n",__func__,__LINE__);
			return -1;
		}
		offset += second_actual;
		/* Second image loading not implemented. */
		ASSERT(0);
	}

	/* Move kernel and ramdisk to correct address */
	memmove((void*) hdr->kernel_addr, (char*) (image_addr + page_size), hdr->kernel_size);
	memmove((void*) hdr->ramdisk_addr, (char*) (image_addr + page_size + kernel_actual), hdr->ramdisk_size);

#if DEVICE_TREE
	if(dt_size != 0) {

		dt_table_offset = ((uint32_t)image_addr + page_size + kernel_actual + ramdisk_actual + second_actual);

		table = (struct dt_table*) dt_table_offset;

		if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0) {
			dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
			return -1;
		}

		/* Its Error if, dt_hdr_size (table->num_entries * dt_entry size + Dev_Tree Header)
		goes beyound hdr->dt_size*/
		if (dt_hdr_size > ROUND_TO_PAGE(dt_size,hdr->page_size)) {
			dprintf(CRITICAL, "ERROR: Invalid Device Tree size \n");
			return -1;
		}

		/* Find index of device tree within device tree table */
		if(dev_tree_get_entry_info(table, &dt_entry) != 0){
			dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
			return -1;
		}

		/* Validate and Read device device tree in the "tags_add */
		if (check_aboot_addr_range_overlap(hdr->tags_addr, dt_entry.size) ||
			check_ddr_addr_range_bound(hdr->tags_addr, dt_entry.size))
		{
			dprintf(CRITICAL, "Device tree addresses are not valid.\n");
			return -1;
		}

		if(dt_entry.offset > (UINT_MAX - dt_entry.size)) {
			dprintf(CRITICAL, "ERROR: Device tree contents are Invalid\n");
			return -1;
		}

		/* Ensure we are not overshooting dt_size with the dt_entry selected */
		if ((dt_entry.offset + dt_entry.size) > dt_size) {
			dprintf(CRITICAL, "ERROR: Device tree contents are Invalid\n");
			return -1;
		}

		best_match_dt_addr = (unsigned char *)table + dt_entry.offset;
		dtb_size = dt_entry.size;
		memmove((void *)hdr->tags_addr, (char *)best_match_dt_addr, dtb_size);
	}
#endif
	if(target_use_signed_kernel() && (!device.is_unlocked))
	{
		/* Make sure everything from scratch address is read before next step!*/
		if(device.is_tampered)
		{
			write_device_info_flash(&device);
		}
#if USE_PCOM_SECBOOT
		set_tamper_flag(device.is_tampered);
#endif
	}

continue_boot:

	/* TODO: create/pass atags to kernel */

	boot_linux((void *)hdr->kernel_addr, (void *)hdr->tags_addr,
		   (const char *)hdr->cmdline, board_machtype(),
		   (void *)hdr->ramdisk_addr, hdr->ramdisk_size);

	return 0;
}

void write_device_info_mmc(device_info *dev)
{
#ifndef SAFE_MODE
	struct device_info *info = (void*) info_buf;
	unsigned long long ptn = 0;
	unsigned long long size;
	int index = INVALID_PTN;
	uint32_t blocksize;
	uint8_t lun = 0;
	uint32_t ret = 0;

	if (devinfo_present)
		index = partition_get_index("devinfo");
	else
		index = partition_get_index("aboot");

	ptn = partition_get_offset(index);
	if(ptn == 0)
	{
		return;
	}

	lun = partition_get_lun(index);
	mmc_set_lun(lun);

	size = partition_get_size(index);

	memcpy(info, dev, sizeof(device_info));

	blocksize = mmc_get_device_blocksize();

	if (devinfo_present)
		ret = mmc_write(ptn, blocksize, (void *)info_buf);
	else
		ret = mmc_write((ptn + size - blocksize), blocksize, (void *)info_buf);
	if (ret)
	{
		dprintf(CRITICAL, "ERROR: Cannot write device info\n");
		return;
	}
#else
	dprintf(CRITICAL, "Ignoring attempt to write device info\n");
#endif
}

void read_device_info_mmc(device_info *dev)
{
	struct device_info *info = (void*) info_buf;
	unsigned long long ptn = 0;
	unsigned long long size;
	int index = INVALID_PTN;
	uint32_t blocksize;
	uint32_t ret  = 0;

	if ((index = partition_get_index("devinfo")) < 0)
	{
		devinfo_present = false;
		index = partition_get_index("aboot");
	}

	ptn = partition_get_offset(index);
	if(ptn == 0)
	{
		return;
	}

	mmc_set_lun(partition_get_lun(index));

	size = partition_get_size(index);

	blocksize = mmc_get_device_blocksize();

	if (devinfo_present)
		ret = mmc_read(ptn, (void *)info_buf, blocksize);
	else
		ret = mmc_read((ptn + size - blocksize), (void *)info_buf, blocksize);
	if (ret)
	{
		dprintf(CRITICAL, "ERROR: Cannot read device info\n");
		return;
	}

	if (memcmp(info->magic, DEVICE_MAGIC, DEVICE_MAGIC_SIZE))
	{
		memcpy(info->magic, DEVICE_MAGIC, DEVICE_MAGIC_SIZE);
#if DEFAULT_UNLOCK
		info->is_unlocked = 1;
#else
		info->is_unlocked = 0;
#endif
		info->is_tampered = 0;

#if USER_BUILD_VARIANT
		info->charger_screen_enabled = 1;
#else
		info->charger_screen_enabled = 0;
#endif

		write_device_info_mmc(info);
	}
	memcpy(dev, info, sizeof(device_info));
}

void write_device_info_flash(device_info *dev)
{
#ifndef SAFE_MODE
	struct device_info *info = memalign(PAGE_SIZE, ROUNDUP(BOOT_IMG_MAX_PAGE_SIZE, PAGE_SIZE));
	struct ptentry *ptn;
	struct ptable *ptable;
	if(info == NULL)
	{
		dprintf(CRITICAL, "Failed to allocate memory for device info struct\n");
		ASSERT(0);
	}
	info_buf = info;
	ptable = flash_get_ptable();
	if (ptable == NULL)
	{
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return;
	}

	ptn = ptable_find(ptable, "devinfo");
	if (ptn == NULL)
	{
		dprintf(CRITICAL, "ERROR: No boot partition found\n");
			return;
	}

	memset(info, 0, BOOT_IMG_MAX_PAGE_SIZE);
	memcpy(info, dev, sizeof(device_info));

	if (flash_write(ptn, 0, (void *)info_buf, page_size))
	{
		dprintf(CRITICAL, "ERROR: Cannot write device info\n");
			return;
	}
	free(info);
#else
	dprintf(CRITICAL, "Ignoring attempt to write device info\n");
#endif
}

static int read_allow_oem_unlock(device_info *dev)
{
	unsigned offset;
	int index;
	unsigned long long ptn;
	unsigned long long ptn_size;
	unsigned blocksize = mmc_get_device_blocksize();
	char buf[blocksize];

	index = partition_get_index(frp_ptns[0]);
	if (index == INVALID_PTN)
	{
		index = partition_get_index(frp_ptns[1]);
		if (index == INVALID_PTN)
		{
			dprintf(CRITICAL, "Neither '%s' nor '%s' partition found\n", frp_ptns[0],frp_ptns[1]);
			return -1;
		}
	}

	ptn = partition_get_offset(index);
	ptn_size = partition_get_size(index);
	offset = ptn_size - blocksize;

	if (mmc_read(ptn + offset, buf, sizeof(buf)))
	{
		dprintf(CRITICAL, "Reading MMC failed\n");
		return -1;
	}

	/*is_allow_unlock is a bool value stored at the LSB of last byte*/
	is_allow_unlock = buf[blocksize-1] & 0x01;
	return 0;
}

static int write_allow_oem_unlock(bool allow_unlock)
{
#ifndef SAFE_MODE
	unsigned offset;
	int index;
	unsigned long long ptn;
	unsigned long long ptn_size;
	unsigned blocksize = mmc_get_device_blocksize();
	char buf[blocksize];

	index = partition_get_index(frp_ptns[0]);
	if (index == INVALID_PTN)
	{
		index = partition_get_index(frp_ptns[1]);
		if (index == INVALID_PTN)
		{
			dprintf(CRITICAL, "Neither '%s' nor '%s' partition found\n", frp_ptns[0],frp_ptns[1]);
			return -1;
		}
	}

	ptn = partition_get_offset(index);
	ptn_size = partition_get_size(index);
	offset = ptn_size - blocksize;

	if (mmc_read(ptn + offset, buf, sizeof(buf)))
	{
		dprintf(CRITICAL, "Reading MMC failed\n");
		return -1;
	}

	/*is_allow_unlock is a bool value stored at the LSB of last byte*/
	buf[blocksize-1] = allow_unlock;
	if (mmc_write(ptn + offset, blocksize, buf))
	{
		dprintf(CRITICAL, "Writing MMC failed\n");
		return -1;
	}

	return 0;
#else
	dprintf(CRITICAL, "Ignoring attempt to allow OEM unlock\n");
	return -1;
#endif
}

void read_device_info_flash(device_info *dev)
{
	struct device_info *info = memalign(PAGE_SIZE, ROUNDUP(BOOT_IMG_MAX_PAGE_SIZE, PAGE_SIZE));
	struct ptentry *ptn;
	struct ptable *ptable;
	if(info == NULL)
	{
		dprintf(CRITICAL, "Failed to allocate memory for device info struct\n");
		ASSERT(0);
	}
	info_buf = info;
	ptable = flash_get_ptable();
	if (ptable == NULL)
	{
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return;
	}

	ptn = ptable_find(ptable, "devinfo");
	if (ptn == NULL)
	{
		dprintf(CRITICAL, "ERROR: No boot partition found\n");
			return;
	}

	if (flash_read(ptn, 0, (void *)info_buf, page_size))
	{
		dprintf(CRITICAL, "ERROR: Cannot write device info\n");
			return;
	}

	if (memcmp(info->magic, DEVICE_MAGIC, DEVICE_MAGIC_SIZE))
	{
		memcpy(info->magic, DEVICE_MAGIC, DEVICE_MAGIC_SIZE);
		info->is_unlocked = 0;
		info->is_tampered = 0;
		write_device_info_flash(info);
	}
	memcpy(dev, info, sizeof(device_info));
	free(info);
}

void write_device_info(device_info *dev)
{
	if(target_is_emmc_boot())
	{
		struct device_info *info = memalign(PAGE_SIZE, ROUNDUP(BOOT_IMG_MAX_PAGE_SIZE, PAGE_SIZE));
		if(info == NULL)
		{
			dprintf(CRITICAL, "Failed to allocate memory for device info struct\n");
			ASSERT(0);
		}
		info_buf = info;
		memcpy(info, dev, sizeof(struct device_info));

#if USE_RPMB_FOR_DEVINFO
		if (is_secure_boot_enable())
			write_device_info_rpmb((void*) info, PAGE_SIZE);
		else
			write_device_info_mmc(info);
#else
		write_device_info_mmc(info);
#endif
		free(info);
	}
	else
	{
		write_device_info_flash(dev);
	}
}

void read_device_info(device_info *dev)
{
	if(target_is_emmc_boot())
	{
		struct device_info *info = memalign(PAGE_SIZE, ROUNDUP(BOOT_IMG_MAX_PAGE_SIZE, PAGE_SIZE));
		if(info == NULL)
		{
			dprintf(CRITICAL, "Failed to allocate memory for device info struct\n");
			ASSERT(0);
		}
		info_buf = info;

#if USE_RPMB_FOR_DEVINFO
		if (is_secure_boot_enable())
			read_device_info_rpmb((void*) info, PAGE_SIZE);
		else
			read_device_info_mmc(info);
#else
		read_device_info_mmc(info);
#endif
		if (memcmp(info->magic, DEVICE_MAGIC, DEVICE_MAGIC_SIZE))
		{
			memcpy(info->magic, DEVICE_MAGIC, DEVICE_MAGIC_SIZE);
			if (is_secure_boot_enable()) {
				info->is_unlocked = 0;
#if !VBOOT_MOTA
				info->is_unlock_critical = 0;
#endif
			} else {
				info->is_unlocked = 1;
#if !VBOOT_MOTA
				info->is_unlock_critical = 1;
#endif
			}
			info->is_tampered = 0;
#if USER_BUILD_VARIANT
			info->charger_screen_enabled = 1;
#else
			info->charger_screen_enabled = 0;
#endif

#if !VBOOT_MOTA
			info->verity_mode = 1; //enforcing by default
			info->devinfo_version = M_DEVICE_INFO_VER;
#endif
		}
#if !VBOOT_MOTA
		/* Else condition is for MOTA upgrade for updating M specific fields*/
		else if(info->devinfo_version != M_DEVICE_INFO_VER)
		{
			info->devinfo_version = M_DEVICE_INFO_VER;
			if (is_secure_boot_enable())
				info->is_unlock_critical = 0;
			else
				info->is_unlock_critical = 1;
			info->verity_mode = 1; //enforcing by default
		}
#endif
		write_device_info(info);
		memcpy(dev, info, sizeof(device_info));
		free(info);
	}
	else
	{
		read_device_info_flash(dev);
	}

	if (lk2nd_dev.bootloader) {
		strcpy(dev->bootloader_version, lk2nd_dev.bootloader);
	}
}

void reset_device_info()
{
	dprintf(ALWAYS, "reset_device_info called.");
	device.is_tampered = 0;
	write_device_info(&device);
}

void set_device_root()
{
	dprintf(ALWAYS, "set_device_root called.");
	device.is_tampered = 1;
	write_device_info(&device);
}

/* set device unlock value
 * Must check FRP before call this function
 * Need to wipe data when unlock status changed
 * type 0: oem unlock
 * type 1: unlock critical
 * status 0: unlock as false
 * status 1: lock as true
 */
void set_device_unlock_value(int type, bool status)
{
	if (type == UNLOCK)
		device.is_unlocked = status;
#if !VBOOT_MOTA
	else if (type == UNLOCK_CRITICAL)
		device.is_unlock_critical = status;
#endif
	write_device_info(&device);
}

static void set_device_unlock(int type, bool status)
{
	int is_unlocked = -1;
	char response[MAX_RSP_SIZE];

	/* check device unlock status if it is as expected */
	if (type == UNLOCK)
		is_unlocked = device.is_unlocked;
#if !VBOOT_MOTA
	else if (type == UNLOCK_CRITICAL)
		is_unlocked = device.is_unlock_critical;
#endif
	if (is_unlocked == status) {
		snprintf(response, sizeof(response), "\tDevice already : %s", (status ? "unlocked!" : "locked!"));
		fastboot_info(response);
		fastboot_okay("");
		return;
	}

	/* status is true, it means to unlock device */
	if (status) {
		if(!is_allow_unlock) {
			fastboot_fail("oem unlock is not allowed");
			return;
		}

#if FBCON_DISPLAY_MSG
		display_unlock_menu(type);
		fastboot_okay("");
		return;
#else
		if (type == UNLOCK) {
			fastboot_fail("Need wipe userdata. Do 'fastboot oem unlock-go'");
			return;
		}
#endif
	}

	set_device_unlock_value(type, status);

	/* wipe data */
	struct recovery_message msg;
	memset(&msg, 0, sizeof(msg));
	snprintf(msg.recovery, sizeof(msg.recovery), "recovery\n--wipe_data");
	write_misc(0, &msg, sizeof(msg));

	fastboot_okay("");
	reboot_device(RECOVERY_MODE);
}

static bool critical_flash_allowed(const char * entry)
{
	uint32_t i = 0;
	if (entry == NULL)
		return false;

	for (i = 0; i < ARRAY_SIZE(critical_flash_allowed_ptn); i++) {
		if(!strcmp(entry, critical_flash_allowed_ptn[i]))
			return true;
	}
	return false;
}

#if DEVICE_TREE
int copy_dtb(uint8_t *boot_image_start, unsigned int scratch_offset)
{
	uint32 dt_image_offset = 0;
	uint32_t n;
	struct dt_table *table;
	struct dt_entry dt_entry;
	uint32_t dt_hdr_size;
	unsigned int compressed_size = 0;
	unsigned int dtb_size = 0;
	unsigned int out_avai_len = 0;
	unsigned char *out_addr = NULL;
	unsigned char *best_match_dt_addr = NULL;
	int rc;

	struct boot_img_hdr *hdr = (struct boot_img_hdr *) (boot_image_start);

#ifndef OSVERSION_IN_BOOTIMAGE
	dt_size = hdr->dt_size;
#endif

	if(dt_size != 0) {
		/* add kernel offset */
		dt_image_offset += page_size;
		n = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
		dt_image_offset += n;

		/* add ramdisk offset */
		n = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
		dt_image_offset += n;

		/* add second offset */
		if(hdr->second_size != 0) {
			n = ROUND_TO_PAGE(hdr->second_size, page_mask);
			dt_image_offset += n;
		}

		/* offset now point to start of dt.img */
		table = (struct dt_table*)(boot_image_start + dt_image_offset);

		if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0) {
			dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
			return -1;
		}

		/* Its Error if, dt_hdr_size (table->num_entries * dt_entry size + Dev_Tree Header)
		goes beyound hdr->dt_size*/
		if (dt_hdr_size > ROUND_TO_PAGE(dt_size,hdr->page_size)) {
			dprintf(CRITICAL, "ERROR: Invalid Device Tree size \n");
			return -1;
		}

		/* Find index of device tree within device tree table */
		if(dev_tree_get_entry_info(table, &dt_entry) != 0){
			dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
			return -1;
		}

		best_match_dt_addr = (unsigned char *)boot_image_start + dt_image_offset + dt_entry.offset;
		if (is_gzip_package(best_match_dt_addr, dt_entry.size))
		{
			out_addr = (unsigned char *)target_get_scratch_address() + scratch_offset;
			out_avai_len = target_get_max_flash_size() - scratch_offset;
			dprintf(SPEW, "decompress dtb start\n");
			rc = decompress(best_match_dt_addr,
					dt_entry.size, out_addr, out_avai_len,
					&compressed_size, &dtb_size);
			if (rc)
			{
				dprintf(CRITICAL, "decompress dtb failed!!!\n");
				ASSERT(0);
			}

			dprintf(SPEW, "decompressed dtb finished.\n");
			best_match_dt_addr = out_addr;
		} else {
			dtb_size = dt_entry.size;
		}
		/* Validate and Read device device tree in the "tags_add */
		if (check_aboot_addr_range_overlap(hdr->tags_addr, dtb_size) ||
			check_ddr_addr_range_bound(hdr->tags_addr, dtb_size))
		{
			dprintf(CRITICAL, "Device tree addresses are not valid.\n");
			return -1;
		}

		/* Read device device tree in the "tags_add */
		memmove((void*) hdr->tags_addr, (void *)best_match_dt_addr, dtb_size);
	} else
		return -1;

	/* Everything looks fine. Return success. */
	return 0;
}
#endif

void cmd_boot(const char *arg, void *data, unsigned sz)
{
	unsigned kernel_actual;
	unsigned ramdisk_actual;
	unsigned second_actual;
	uint32_t image_actual;
	uint32_t dt_actual = 0;
	uint32_t sig_actual = 0;
	uint32_t sig_size = 0;
	struct boot_img_hdr *hdr = NULL;
	struct kernel64_hdr *kptr = NULL;
	char *ptr = ((char*) data);
	int ret = 0;
	uint8_t dtb_copied = 0;
	unsigned int out_len = 0;
	unsigned int out_avai_len = 0;
	unsigned char *out_addr = NULL;
	uint32_t dtb_offset = 0;
	unsigned char *kernel_start_addr = NULL;
	unsigned int kernel_size = 0;
	unsigned int scratch_offset = 0;


#if VERIFIED_BOOT
	if(!device.is_unlocked)
	{
		fastboot_fail("unlock device to use this command");
		return;
	}
#endif

	if (sz < sizeof(hdr)) {
		fastboot_fail("invalid bootimage header");
		return;
	}

	hdr = (struct boot_img_hdr *)data;

	/* ensure commandline is terminated */
	hdr->cmdline[BOOT_ARGS_SIZE-1] = 0;

	if(target_is_emmc_boot() && hdr->page_size) {
		page_size = hdr->page_size;
		page_mask = page_size - 1;
	}

	kernel_actual = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
	ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
	second_actual = ROUND_TO_PAGE(hdr->second_size, page_mask);
#if DEVICE_TREE
#ifndef OSVERSION_IN_BOOTIMAGE
	dt_size = hdr->dt_size;
#endif
	dt_actual = ROUND_TO_PAGE(dt_size, page_mask);
#endif

	image_actual = ADD_OF(page_size, kernel_actual);
	image_actual = ADD_OF(image_actual, ramdisk_actual);
	image_actual = ADD_OF(image_actual, second_actual);
	image_actual = ADD_OF(image_actual, dt_actual);

	/* Checking to prevent oob access in read_der_message_length */
	if (image_actual > sz) {
		fastboot_fail("bootimage header fields are invalid");
		return;
	}
	sig_size = sz - image_actual;

	if (target_use_signed_kernel() && (!device.is_unlocked)) {
		/* Calculate the signature length from boot image */
		sig_actual = read_der_message_length(
				(unsigned char*)(data + image_actual), sig_size);
		image_actual = ADD_OF(image_actual, sig_actual);

		if (image_actual > sz) {
			fastboot_fail("bootimage header fields are invalid");
			return;
		}
	}

	// Initialize boot state before trying to verify boot.img
#if VERIFIED_BOOT
		boot_verifier_init();
#endif
	/* Handle overflow if the input image size is greater than
	 * boot image buffer can hold
	 */
	if ((target_get_max_flash_size() - (image_actual - sig_actual)) < page_size)
	{
		fastboot_fail("booimage: size is greater than boot image buffer can hold");
		return;
	}

	/* Verify the boot image
	 * device & page_size are initialized in aboot_init
	 */
	if (target_use_signed_kernel() && (!device.is_unlocked)) {
		/* Pass size excluding signature size, otherwise we would try to
		 * access signature beyond its length
		 */
		verify_signed_bootimg((uint32_t)data, (image_actual - sig_actual));
	}

#if VERIFIED_BOOT
#if !VBOOT_MOTA
	// send root of trust
	if(!send_rot_command((uint32_t)device.is_unlocked))
		ASSERT(0);
#endif
#endif
	/*
	 * Check if the kernel image is a gzip package. If yes, need to decompress it.
	 * If not, continue booting.
	 */
	if (is_gzip_package((unsigned char *)(data + page_size), hdr->kernel_size))
	{
		out_addr = (unsigned char *)target_get_scratch_address();
		out_addr = (unsigned char *)(out_addr + image_actual + page_size);
		out_avai_len = target_get_max_flash_size() - image_actual - page_size;
		dprintf(SPEW, "decompress image start\n");
		ret = decompress((unsigned char *)(ptr + page_size),
				hdr->kernel_size, out_addr, out_avai_len,
				&dtb_offset, &out_len);
		if (ret)
		{
			dprintf(CRITICAL, "decompress image failed!!!\n");
			ASSERT(0);
		}

		dprintf(SPEW, "decompressed image finished.\n");
		kptr = (struct kernel64_hdr *)out_addr;
		kernel_start_addr = out_addr;
		kernel_size = out_len;
	} else {
		kptr = (struct kernel64_hdr*)((char *)data + page_size);
		kernel_start_addr = (unsigned char *)((char *)data + page_size);
		kernel_size = hdr->kernel_size;
	}

	/*
	 * Update the kernel/ramdisk/tags address if the boot image header
	 * has default values, these default values come from mkbootimg when
	 * the boot image is flashed using fastboot flash:raw
	 */
	update_ker_tags_rdisk_addr(hdr, kptr);

	/* Get virtual addresses since the hdr saves physical addresses. */
	hdr->kernel_addr = VA(hdr->kernel_addr);
	hdr->ramdisk_addr = VA(hdr->ramdisk_addr);
	hdr->tags_addr = VA(hdr->tags_addr);

	kernel_size  = ROUND_TO_PAGE(kernel_size,  page_mask);
	/* Check if the addresses in the header are valid. */
	if (check_aboot_addr_range_overlap(hdr->kernel_addr, kernel_size) ||
		check_ddr_addr_range_bound(hdr->kernel_addr, kernel_size) ||
		check_aboot_addr_range_overlap(hdr->ramdisk_addr, ramdisk_actual) ||
		check_ddr_addr_range_bound(hdr->ramdisk_addr, ramdisk_actual))
	{
		dprintf(CRITICAL, "kernel/ramdisk addresses are not valid.\n");
		return;
	}

#if DEVICE_TREE
	scratch_offset = image_actual + page_size + out_len;
	/* find correct dtb and copy it to right location */
	ret = copy_dtb(data, scratch_offset);

	dtb_copied = !ret ? 1 : 0;
#else
	if (check_aboot_addr_range_overlap(hdr->tags_addr, MAX_TAGS_SIZE) ||
		check_ddr_addr_range_bound(hdr->tags_addr, MAX_TAGS_SIZE))
	{
		dprintf(CRITICAL, "Tags addresses are not valid.\n");
		return;
	}
#endif

	/* Load ramdisk & kernel */
	memmove((void*) hdr->ramdisk_addr, ptr + page_size + kernel_actual, hdr->ramdisk_size);
	memmove((void*) hdr->kernel_addr, (char*) (kernel_start_addr), kernel_size);

#if DEVICE_TREE
	if (check_aboot_addr_range_overlap(hdr->tags_addr, kernel_actual) ||
		check_ddr_addr_range_bound(hdr->tags_addr, kernel_actual))
	{
		dprintf(CRITICAL, "Tags addresses are not valid.\n");
		return;
	}

	/*
	 * If dtb is not found look for appended DTB in the kernel.
	 * If appended dev tree is found, update the atags with
	 * memory address to the DTB appended location on RAM.
	 * Else update with the atags address in the kernel header
	 */
	if (!dtb_copied) {
		void *dtb;
		dtb = dev_tree_appended((void*)(ptr + page_size),
					hdr->kernel_size, dtb_offset,
					(void *)hdr->tags_addr);
		if (!dtb) {
			fastboot_fail("dtb not found");
			return;
		}
	}
#endif

	fastboot_okay("");
	fastboot_stop();

	boot_linux((void*) hdr->kernel_addr, (void*) hdr->tags_addr,
		   (const char*) hdr->cmdline, board_machtype(),
		   (void*) hdr->ramdisk_addr, hdr->ramdisk_size);
}

void cmd_erase_nand(const char *arg, void *data, unsigned sz)
{
	struct ptentry *ptn;
	struct ptable *ptable;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		fastboot_fail("partition table doesn't exist");
		return;
	}

	ptn = ptable_find(ptable, arg);
	if (ptn == NULL) {
		fastboot_fail("unknown partition name");
		return;
	}

	if (flash_erase(ptn)) {
		fastboot_fail("failed to erase partition");
		return;
	}
	fastboot_okay("");
}


void cmd_erase_mmc(const char *arg, void *data, unsigned sz)
{
	BUF_DMA_ALIGN(out, DEFAULT_ERASE_SIZE);
	unsigned long long ptn = 0;
	unsigned long long size = 0;
	int index = INVALID_PTN;
	uint8_t lun = 0;

#if VERIFIED_BOOT
	if(!strcmp(arg, KEYSTORE_PTN_NAME))
	{
		if(!device.is_unlocked)
		{
			fastboot_fail("unlock device to erase keystore");
			return;
		}
	}
#endif

	index = partition_get_index(arg);
	ptn = partition_get_offset(index);
	size = partition_get_size(index);

	if(ptn == 0) {
		fastboot_fail("Partition table doesn't exist\n");
		return;
	}

	lun = partition_get_lun(index);
	mmc_set_lun(lun);

#if MMC_SDHCI_SUPPORT
	if (mmc_erase_card(ptn, size)) {
		fastboot_fail("failed to erase partition\n");
		return;
	}
#else
	size = partition_get_size(index);
	if (size > DEFAULT_ERASE_SIZE)
		size = DEFAULT_ERASE_SIZE;

	/* Simple inefficient version of erase. Just writing
       0 in first several blocks */
	if (mmc_write(ptn , size, (unsigned int *)out)) {
		fastboot_fail("failed to erase partition");
		return;
	}
#if VERIFIED_BOOT
#if !VBOOT_MOTA
	if(!(strncmp(arg, "userdata", 8)))
		if(send_delete_keys_to_tz())
			ASSERT(0);
#endif
#endif
#endif
	fastboot_okay("");
}

void cmd_erase(const char *arg, void *data, unsigned sz)
{
#if CHECK_BAT_VOLTAGE
	if (!target_battery_soc_ok()) {
		fastboot_fail("Warning: battery's capacity is very low\n");
		return;
	}
#endif

#if VERIFIED_BOOT
	if(!device.is_unlocked)
	{
		fastboot_fail("device is locked. Cannot erase");
		return;
	}
#endif

	if(target_is_emmc_boot())
		cmd_erase_mmc(arg, data, sz);
	else
		cmd_erase_nand(arg, data, sz);
}

void cmd_flash_mmc_img(const char *arg, void *data, unsigned sz)
{
	unsigned long long ptn = 0;
	unsigned long long size = 0;
	int index = INVALID_PTN;
	char *token = NULL;
	char *pname = NULL;
	char *sp;
	uint8_t lun = 0;
	bool lun_set = false;

	token = strtok_r((char *)arg, ":", &sp);
	pname = token;
	token = strtok_r(NULL, ":", &sp);
	if(token)
	{
		lun = atoi(token);
		mmc_set_lun(lun);
		lun_set = true;
	}

	if (pname)
	{
		if (!strncmp(pname, "frp-unlock", strlen("frp-unlock")))
		{
			if (!aboot_frp_unlock(pname, data, sz))
			{
				fastboot_info("FRP unlock successful");
				fastboot_okay("");
			}
			else
				fastboot_fail("Secret key is invalid, please update the bootloader with secret key");

			return;
		}

		if (!strcmp(pname, "partition"))
		{
			dprintf(INFO, "Attempt to write partition image.\n");
			if (write_partition(sz, (unsigned char *) data)) {
				fastboot_fail("failed to write partition");
				return;
			}
		}
		else
		{
#if VERIFIED_BOOT
			if(!strcmp(pname, KEYSTORE_PTN_NAME))
			{
				if(!device.is_unlocked)
				{
					fastboot_fail("unlock device to flash keystore");
					return;
				}
				if(!boot_verify_validate_keystore((unsigned char *)data,sz))
				{
					fastboot_fail("image is not a keystore file");
					return;
				}
			}
#endif
			index = partition_get_index(pname);
			ptn = partition_get_offset(index);
			if(ptn == 0) {
				fastboot_fail("partition table doesn't exist");
				return;
			}

			if (!strcmp(pname, "boot") || !strcmp(pname, "recovery")) {
				if (memcmp((void *)data, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
					fastboot_fail("image is not a boot image");
					return;
				}
			}

			if(!lun_set)
			{
				lun = partition_get_lun(index);
				mmc_set_lun(lun);
			}

			size = partition_get_size(index);
			if (ROUND_TO_PAGE(sz, mmc_blocksize_mask) > size) {
				fastboot_fail("size too large");
				return;
			}
			else if (mmc_write(ptn , sz, (unsigned int *)data)) {
				fastboot_fail("flash write failure");
				return;
			}
		}
	}
	fastboot_okay("");
	return;
}

void cmd_flash_meta_img(const char *arg, void *data, unsigned sz)
{
	int i, images;
	meta_header_t *meta_header;
	img_header_entry_t *img_header_entry;
	/*End of the image address*/
	uintptr_t data_end;

	if( (UINT_MAX - sz) > (uintptr_t)data )
		data_end  = (uintptr_t)data + sz;
	else
	{
		fastboot_fail("Cannot  flash: image header corrupt");
		return;
	}

	if( data_end < ((uintptr_t)data + sizeof(meta_header_t)))
	{
		fastboot_fail("Cannot  flash: image header corrupt");
		return;
	}

	meta_header = (meta_header_t*) data;
	if( data_end < ((uintptr_t)data + meta_header->img_hdr_sz))
	{
		fastboot_fail("Cannot  flash: image header corrupt");
		return;
	}
	img_header_entry = (img_header_entry_t*) (data+sizeof(meta_header_t));

	images = meta_header->img_hdr_sz / sizeof(img_header_entry_t);

	for (i=0; i<images; i++) {

		if((img_header_entry[i].ptn_name == NULL) ||
			(img_header_entry[i].start_offset == 0) ||
			(img_header_entry[i].size == 0))
			break;
		if ((UINT_MAX - img_header_entry[i].start_offset) < (uintptr_t)data) {
			fastboot_fail("Integer overflow detected in start_offset of img");
			break;
		}
		else if ((UINT_MAX - (img_header_entry[i].start_offset + (uintptr_t)data)) < img_header_entry[i].size) {
			fastboot_fail("Integer overflow detected in size of img");
			break;
		}
		if( data_end < ((uintptr_t)data + img_header_entry[i].start_offset
						+ img_header_entry[i].size) )
		{
			fastboot_fail("Cannot  flash: image size mismatch");
			break;
		}

		cmd_flash_mmc_img(img_header_entry[i].ptn_name,
					(void *) data + img_header_entry[i].start_offset,
					img_header_entry[i].size);
	}

	if (!strncmp(arg, "bootloader", strlen("bootloader")))
	{
		strlcpy(device.bootloader_version, TARGET(BOARD), MAX_VERSION_LEN);
		strlcat(device.bootloader_version, "-", MAX_VERSION_LEN);
		strlcat(device.bootloader_version, meta_header->img_version, MAX_VERSION_LEN);
	}
	else
	{
		strlcpy(device.radio_version, TARGET(BOARD), MAX_VERSION_LEN);
		strlcat(device.radio_version, "-", MAX_VERSION_LEN);
		strlcat(device.radio_version, meta_header->img_version, MAX_VERSION_LEN);
	}

	write_device_info(&device);
	fastboot_okay("");
	return;
}

void cmd_flash_mmc_sparse_img(const char *arg, void *data, unsigned sz)
{
	unsigned int chunk;
	unsigned int chunk_data_sz;
	uint32_t *fill_buf = NULL;
	uint32_t fill_val;
	uint32_t chunk_blk_cnt = 0;
	uint32_t blk_sz_actual = 0;
	sparse_header_t *sparse_header;
	chunk_header_t *chunk_header;
	uint32_t total_blocks = 0;
	unsigned long long ptn = 0;
	unsigned long long size = 0;
	int index = INVALID_PTN;
	int i;
	uint8_t lun = 0;
	/*End of the sparse image address*/
	uint32_t data_end = (uint32_t)data + sz;

	index = partition_get_index(arg);
	ptn = partition_get_offset(index);
	if(ptn == 0) {
		fastboot_fail("partition table doesn't exist");
		return;
	}

	size = partition_get_size(index);

	lun = partition_get_lun(index);
	mmc_set_lun(lun);

	if (sz < sizeof(sparse_header_t)) {
		fastboot_fail("size too low");
		return;
	}

	/* Read and skip over sparse image header */
	sparse_header = (sparse_header_t *) data;

	if (!sparse_header->blk_sz || (sparse_header->blk_sz % 4)){
		fastboot_fail("Invalid block size\n");
		return;
	}

	if (((uint64_t)sparse_header->total_blks * (uint64_t)sparse_header->blk_sz) > size) {
		fastboot_fail("size too large");
		return;
	}

	data += sizeof(sparse_header_t);

	if (data_end < (uint32_t)data) {
		fastboot_fail("buffer overreads occured due to invalid sparse header");
		return;
	}

	if(sparse_header->file_hdr_sz != sizeof(sparse_header_t))
	{
		fastboot_fail("sparse header size mismatch");
		return;
	}

	dprintf (SPEW, "=== Sparse Image Header ===\n");
	dprintf (SPEW, "magic: 0x%x\n", sparse_header->magic);
	dprintf (SPEW, "major_version: 0x%x\n", sparse_header->major_version);
	dprintf (SPEW, "minor_version: 0x%x\n", sparse_header->minor_version);
	dprintf (SPEW, "file_hdr_sz: %d\n", sparse_header->file_hdr_sz);
	dprintf (SPEW, "chunk_hdr_sz: %d\n", sparse_header->chunk_hdr_sz);
	dprintf (SPEW, "blk_sz: %d\n", sparse_header->blk_sz);
	dprintf (SPEW, "total_blks: %d\n", sparse_header->total_blks);
	dprintf (SPEW, "total_chunks: %d\n", sparse_header->total_chunks);

	/* Start processing chunks */
	for (chunk=0; chunk<sparse_header->total_chunks; chunk++)
	{
		/* Make sure the total image size does not exceed the partition size */
		if(((uint64_t)total_blocks * (uint64_t)sparse_header->blk_sz) >= size) {
			fastboot_fail("size too large");
			return;
		}
		/* Read and skip over chunk header */
		chunk_header = (chunk_header_t *) data;
		data += sizeof(chunk_header_t);

		if (data_end < (uint32_t)data) {
			fastboot_fail("buffer overreads occured due to invalid sparse header");
			return;
		}

		dprintf (SPEW, "=== Chunk Header ===\n");
		dprintf (SPEW, "chunk_type: 0x%x\n", chunk_header->chunk_type);
		dprintf (SPEW, "chunk_data_sz: 0x%x\n", chunk_header->chunk_sz);
		dprintf (SPEW, "total_size: 0x%x\n", chunk_header->total_sz);

		if(sparse_header->chunk_hdr_sz != sizeof(chunk_header_t))
		{
			fastboot_fail("chunk header size mismatch");
			return;
		}

		chunk_data_sz = sparse_header->blk_sz * chunk_header->chunk_sz;

		switch (chunk_header->chunk_type)
		{
			case CHUNK_TYPE_RAW:
			/* Make sure multiplication does not overflow uint32 size */
			if (sparse_header->blk_sz && (chunk_header->chunk_sz != chunk_data_sz / sparse_header->blk_sz))
			{
			  fastboot_fail("Bogus size sparse and chunk header");
			  return;
			}

			/* Make sure that the chunk size calculated from sparse image does not
			 * exceed partition size
			 */
			if ((uint64_t)total_blocks * (uint64_t)sparse_header->blk_sz + chunk_data_sz > size)
			{
			  fastboot_fail("Chunk data size exceeds partition size");
			  return;
			}

			if(chunk_header->total_sz != (sparse_header->chunk_hdr_sz +
											chunk_data_sz))
			{
				fastboot_fail("Bogus chunk size for chunk type Raw");
				return;
			}

			if (data_end < (uint32_t)data + chunk_data_sz) {
				fastboot_fail("buffer overreads occured due to invalid sparse header");
				return;
			}

			if(mmc_write(ptn + ((uint64_t)total_blocks*sparse_header->blk_sz),
						chunk_data_sz,
						(unsigned int*)data))
			{
				fastboot_fail("flash write failure");
				return;
			}
			if(total_blocks > (UINT_MAX - chunk_header->chunk_sz)) {
				fastboot_fail("Bogus size for RAW chunk type");
				return;
			}
			total_blocks += chunk_header->chunk_sz;
			data += chunk_data_sz;
			break;

			case CHUNK_TYPE_FILL:
			if(chunk_header->total_sz != (sparse_header->chunk_hdr_sz +
											sizeof(uint32_t)))
			{
				fastboot_fail("Bogus chunk size for chunk type FILL");
				return;
			}

			blk_sz_actual = ROUNDUP(sparse_header->blk_sz, CACHE_LINE);
			/* Integer overflow detected */
			if (blk_sz_actual < sparse_header->blk_sz)
			{
				fastboot_fail("Invalid block size");
				return;
			}

			fill_buf = (uint32_t *)memalign(CACHE_LINE, blk_sz_actual);
			if (!fill_buf)
			{
				fastboot_fail("Malloc failed for: CHUNK_TYPE_FILL");
				return;
			}

			if (data_end < (uint32_t)data + sizeof(uint32_t)) {
				fastboot_fail("buffer overreads occured due to invalid sparse header");
				free(fill_buf);
				return;
			}
			fill_val = *(uint32_t *)data;
			data = (char *) data + sizeof(uint32_t);
			chunk_blk_cnt = chunk_data_sz / sparse_header->blk_sz;

			for (i = 0; i < (sparse_header->blk_sz / sizeof(fill_val)); i++)
			{
				fill_buf[i] = fill_val;
			}

			if(total_blocks > (UINT_MAX - chunk_header->chunk_sz))
			{
				fastboot_fail("bogus size for chunk FILL type");
				free(fill_buf);
				return;
			}

			for (i = 0; i < chunk_blk_cnt; i++)
			{
				/* Make sure that the data written to partition does not exceed partition size */
				if ((uint64_t)total_blocks * (uint64_t)sparse_header->blk_sz + sparse_header->blk_sz > size)
				{
					fastboot_fail("Chunk data size for fill type exceeds partition size");
					free(fill_buf);
					return;
				}

				if(mmc_write(ptn + ((uint64_t)total_blocks*sparse_header->blk_sz),
							sparse_header->blk_sz,
							fill_buf))
				{
					fastboot_fail("flash write failure");
					free(fill_buf);
					return;
				}

				total_blocks++;
			}

			free(fill_buf);
			break;

			case CHUNK_TYPE_DONT_CARE:
			if(total_blocks > (UINT_MAX - chunk_header->chunk_sz)) {
				fastboot_fail("bogus size for chunk DONT CARE type");
				return;
			}
			total_blocks += chunk_header->chunk_sz;
			break;

			case CHUNK_TYPE_CRC:
			if(chunk_header->total_sz != sparse_header->chunk_hdr_sz)
			{
				fastboot_fail("Bogus chunk size for chunk type Dont Care");
				return;
			}
			if(total_blocks > (UINT_MAX - chunk_header->chunk_sz)) {
				fastboot_fail("bogus size for chunk CRC type");
				return;
			}
			total_blocks += chunk_header->chunk_sz;
			if ((uint32_t)data > UINT_MAX - chunk_data_sz) {
				fastboot_fail("integer overflow occured");
				return;
			}
			data += chunk_data_sz;
			if (data_end < (uint32_t)data) {
				fastboot_fail("buffer overreads occured due to invalid sparse header");
				return;
			}
			break;

			default:
			dprintf(CRITICAL, "Unkown chunk type: %x\n",chunk_header->chunk_type);
			fastboot_fail("Unknown chunk type");
			return;
		}
	}

	dprintf(INFO, "Wrote %d blocks, expected to write %d blocks\n",
					total_blocks, sparse_header->total_blks);

	if(total_blocks != sparse_header->total_blks)
	{
		fastboot_fail("sparse image write failure");
	}

	fastboot_okay("");
	return;
}

void cmd_flash_mmc(const char *arg, void *data, unsigned sz)
{
	sparse_header_t *sparse_header;
	meta_header_t *meta_header;

#ifdef SSD_ENABLE
	/* 8 Byte Magic + 2048 Byte xml + Encrypted Data */
	unsigned int *magic_number = (unsigned int *) data;
	int              ret=0;
	uint32           major_version=0;
	uint32           minor_version=0;

	ret = scm_svc_version(&major_version,&minor_version);
	if(!ret)
	{
		if(major_version >= 2)
		{
			if( !strcmp(arg, "ssd") || !strcmp(arg, "tqs") )
			{
				ret = encrypt_scm((uint32 **) &data, &sz);
				if (ret != 0) {
					dprintf(CRITICAL, "ERROR: Encryption Failure\n");
					return;
				}

				/* Protect only for SSD */
				if (!strcmp(arg, "ssd")) {
					ret = scm_protect_keystore((uint32 *) data, sz);
					if (ret != 0) {
						dprintf(CRITICAL, "ERROR: scm_protect_keystore Failed\n");
						return;
					}
				}
			}
			else
			{
				ret = decrypt_scm_v2((uint32 **) &data, &sz);
				if(ret != 0)
				{
					dprintf(CRITICAL,"ERROR: Decryption Failure\n");
					return;
				}
			}
		}
		else
		{
			if (magic_number[0] == DECRYPT_MAGIC_0 &&
			magic_number[1] == DECRYPT_MAGIC_1)
			{
				ret = decrypt_scm((uint32 **) &data, &sz);
				if (ret != 0) {
					dprintf(CRITICAL, "ERROR: Invalid secure image\n");
					return;
				}
			}
			else if (magic_number[0] == ENCRYPT_MAGIC_0 &&
				magic_number[1] == ENCRYPT_MAGIC_1)
			{
				ret = encrypt_scm((uint32 **) &data, &sz);
				if (ret != 0) {
					dprintf(CRITICAL, "ERROR: Encryption Failure\n");
					return;
				}
			}
		}
	}
	else
	{
		dprintf(CRITICAL,"INVALID SVC Version\n");
		return;
	}
#endif /* SSD_ENABLE */

#if VERIFIED_BOOT
	if(!device.is_unlocked)
	{
		/* if device is locked:
		 * common partition will not allow to be flashed
		 * critical partition will allow to flash image.
		 */
		if(!device.is_unlocked && !critical_flash_allowed(arg)) {
			fastboot_fail("Partition flashing is not allowed");
			return;
		}
#if !VBOOT_MOTA
		/* if device critical is locked:
		 * common partition will allow to be flashed
		 * critical partition will not allow to flash image.
		 */
		if(!device.is_unlock_critical && critical_flash_allowed(arg)) {
			fastboot_fail("Critical partition flashing is not allowed");
			return;
		}
#endif
	}
#endif

	sparse_header = (sparse_header_t *) data;
        meta_header = (meta_header_t *) data;
        if (sparse_header->magic == SPARSE_HEADER_MAGIC)
                cmd_flash_mmc_sparse_img(arg, data, sz);
        else if (meta_header->magic == META_HEADER_MAGIC)
                cmd_flash_meta_img(arg, data, sz);
        else
                cmd_flash_mmc_img(arg, data, sz);

#if VERIFIED_BOOT
#if !VBOOT_MOTA
	if((!strncmp(arg, "system", 6)) && !device.verity_mode)
	{
		// reset dm_verity mode to enforcing
		device.verity_mode = 1;
		write_device_info(&device);
	}
#endif
#endif

	return;
}

void cmd_flash_nand(const char *arg, void *data, unsigned sz)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned extra = 0;
	uint64_t partition_size = 0;

	if((uintptr_t)data > (UINT_MAX - sz)) {
		fastboot_fail("Cannot flash: image header corrupt");
                return;
        }

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		fastboot_fail("partition table doesn't exist");
		return;
	}

	ptn = ptable_find(ptable, arg);
	if (ptn == NULL) {
		fastboot_fail("unknown partition name");
		return;
	}

	if (!strcmp(ptn->name, "boot") || !strcmp(ptn->name, "recovery")) {
		if((sz > BOOT_MAGIC_SIZE) && (!memcmp((void *)data, BOOT_MAGIC, BOOT_MAGIC_SIZE))) {
			dprintf(INFO, "Verified the BOOT_MAGIC in image header  \n");
		} else {
			fastboot_fail("Image is not a boot image");
			return;
		}
	}

	if (!strcmp(ptn->name, "system")
		|| !strcmp(ptn->name, "userdata")
		|| !strcmp(ptn->name, "persist")
		|| !strcmp(ptn->name, "recoveryfs")
		|| !strcmp(ptn->name, "modem"))
		extra = 1;
	else {
		rounded_size = ROUNDUP(sz, page_size);
		bytes_to_round_page = rounded_size - sz;
		if (bytes_to_round_page) {
			if (((uintptr_t)data + sz ) > (UINT_MAX - bytes_to_round_page)) {
				fastboot_fail("Integer overflow detected");
				return;
			}
			if (((uintptr_t)data + sz + bytes_to_round_page) >
			((uintptr_t)target_get_scratch_address() + target_get_max_flash_size())) {
				fastboot_fail("Buffer size is not aligned to page_size");
				return;
			}
			else {
				memset(data + sz, 0, bytes_to_round_page);
				sz = rounded_size;
			}

		}
	}

	/*Checking partition_size for the possible integer overflow */
	partition_size = validate_partition_size(ptn);

	if (sz > partition_size) {
		fastboot_fail("Image size too large");
		return;
	}

	dprintf(INFO, "writing %d bytes to '%s'\n", sz, ptn->name);
	if ((sz > UBI_EC_HDR_SIZE) &&
		(!memcmp((void *)data, UBI_MAGIC, UBI_MAGIC_SIZE))) {
		if (flash_ubi_img(ptn, data, sz)) {
			fastboot_fail("flash write failure");
			return;
		}
	} else {
		if (flash_write(ptn, extra, data, sz)) {
			fastboot_fail("flash write failure");
			return;
		}
	}
	dprintf(INFO, "partition '%s' updated\n", ptn->name);
	fastboot_okay("");
}


static inline uint64_t validate_partition_size(struct ptentry *ptn)
{
	if (ptn->length && flash_num_pages_per_blk() && page_size) {
		if ((ptn->length < ( UINT_MAX / flash_num_pages_per_blk())) && ((ptn->length * flash_num_pages_per_blk()) < ( UINT_MAX / page_size))) {
			return ptn->length * flash_num_pages_per_blk() * page_size;
		}
        }
	return 0;
}


void cmd_flash(const char *arg, void *data, unsigned sz)
{
#if CHECK_BAT_VOLTAGE
	if (!target_battery_soc_ok()) {
		fastboot_fail("Warning: battery's capacity is very low\n");
		return;
	}
#endif
	if(target_is_emmc_boot())
		cmd_flash_mmc(arg, data, sz);
	else
		cmd_flash_nand(arg, data, sz);
}

void cmd_continue(const char *arg, void *data, unsigned sz)
{
	fastboot_okay("");
	fastboot_stop();

	if (target_is_emmc_boot())
	{
#if FBCON_DISPLAY_MSG
		/* Exit keys' detection thread firstly */
		exit_menu_keys_detection();
#endif
		boot_linux_from_mmc();
	}
	else
	{
		boot_linux_from_flash();
	}
}

void cmd_reboot(const char *arg, void *data, unsigned sz)
{
	dprintf(INFO, "rebooting the device\n");
	fastboot_okay("");
	reboot_device(0);
}

void cmd_reboot_bootloader(const char *arg, void *data, unsigned sz)
{
	dprintf(INFO, "rebooting the device\n");
	fastboot_okay("");
	reboot_device(FASTBOOT_MODE);
}

void cmd_oem_enable_charger_screen(const char *arg, void *data, unsigned size)
{
	dprintf(INFO, "Enabling charger screen check\n");
	device.charger_screen_enabled = 1;
	write_device_info(&device);
	fastboot_okay("");
}

void cmd_oem_disable_charger_screen(const char *arg, void *data, unsigned size)
{
	dprintf(INFO, "Disabling charger screen check\n");
	device.charger_screen_enabled = 0;
	write_device_info(&device);
	fastboot_okay("");
}

void cmd_oem_off_mode_charger(const char *arg, void *data, unsigned size)
{
	char *p = NULL;
	const char *delim = " \t\n\r";
	char *sp;

	if (arg) {
		p = strtok_r((char *)arg, delim, &sp);
		if (p) {
			if (!strncmp(p, "0", 1)) {
				device.charger_screen_enabled = 0;
			} else if (!strncmp(p, "1", 1)) {
				device.charger_screen_enabled = 1;
			}
		}
	}

	/* update charger_screen_enabled value for getvar
	 * command
	 */
	snprintf(charger_screen_enabled, MAX_RSP_SIZE, "%d",
		device.charger_screen_enabled);

	write_device_info(&device);
	fastboot_okay("");
}

void cmd_oem_select_display_panel(const char *arg, void *data, unsigned size)
{
	dprintf(INFO, "Selecting display panel %s\n", arg);
	if (arg)
		strlcpy(device.display_panel, arg,
			sizeof(device.display_panel));
	write_device_info(&device);
	fastboot_okay("");
}

void cmd_oem_unlock(const char *arg, void *data, unsigned sz)
{
	set_device_unlock(UNLOCK, TRUE);
}

void cmd_oem_unlock_go(const char *arg, void *data, unsigned sz)
{
	if(!device.is_unlocked) {
		if(!is_allow_unlock) {
			fastboot_fail("oem unlock is not allowed");
			return;
		}

		set_device_unlock_value(UNLOCK, TRUE);

		/* wipe data */
		struct recovery_message msg;
		memset(&msg, 0, sizeof(msg));
		snprintf(msg.recovery, sizeof(msg.recovery), "recovery\n--wipe_data");
		write_misc(0, &msg, sizeof(msg));

		fastboot_okay("");
		reboot_device(RECOVERY_MODE);
	}
	fastboot_okay("");
}

static int aboot_frp_unlock(char *pname, void *data, unsigned sz)
{
	int ret=1;
	bool authentication_success=false;

	/*
		Authentication method not  implemented.

		OEM to implement, authentication system which on successful validataion,
		calls write_allow_oem_unlock() with is_allow_unlock.
	*/
#if 0
	authentication_success = oem_specific_auth_mthd();
#endif

	if (authentication_success)
	{
		is_allow_unlock = true;
		write_allow_oem_unlock(is_allow_unlock);
		ret = 0;
	}
	return ret;
}

void cmd_oem_lock(const char *arg, void *data, unsigned sz)
{
	set_device_unlock(UNLOCK, FALSE);
}

void cmd_oem_devinfo(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	snprintf(response, sizeof(response), "\tDevice tampered: %s", (device.is_tampered ? "true" : "false"));
	fastboot_info(response);
	snprintf(response, sizeof(response), "\tDevice unlocked: %s", (device.is_unlocked ? "true" : "false"));
	fastboot_info(response);
#if !VBOOT_MOTA
	snprintf(response, sizeof(response), "\tDevice critical unlocked: %s", (device.is_unlock_critical ? "true" : "false"));
	fastboot_info(response);
#endif
	snprintf(response, sizeof(response), "\tCharger screen enabled: %s", (device.charger_screen_enabled ? "true" : "false"));
	fastboot_info(response);
	snprintf(response, sizeof(response), "\tDisplay panel: %s", (device.display_panel));
	fastboot_info(response);
	fastboot_okay("");
}

void cmd_flashing_get_unlock_ability(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	snprintf(response, sizeof(response), "\tget_unlock_ability: %d", is_allow_unlock);
	fastboot_info(response);
	fastboot_okay("");
}

void cmd_flashing_lock_critical(const char *arg, void *data, unsigned sz)
{
	set_device_unlock(UNLOCK_CRITICAL, FALSE);
}

void cmd_flashing_unlock_critical(const char *arg, void *data, unsigned sz)
{
	set_device_unlock(UNLOCK_CRITICAL, TRUE);
}

void cmd_preflash(const char *arg, void *data, unsigned sz)
{
	fastboot_okay("");
}

static uint8_t logo_header[LOGO_IMG_HEADER_SIZE];

int splash_screen_check_header(logo_img_header *header)
{
	if (memcmp(header->magic, LOGO_IMG_MAGIC, 8))
		return -1;
	if (header->width == 0 || header->height == 0)
		return -1;
	return 0;
}

int splash_screen_flash()
{
	struct ptentry *ptn;
	struct ptable *ptable;
	struct logo_img_header *header;
	struct fbcon_config *fb_display = NULL;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}

	ptn = ptable_find(ptable, "splash");
	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: splash Partition not found\n");
		return -1;
	}
	if (flash_read(ptn, 0, (void *)logo_header, LOGO_IMG_HEADER_SIZE)) {
		dprintf(CRITICAL, "ERROR: Cannot read boot image header\n");
		return -1;
	}

	header = (struct logo_img_header *)logo_header;
	if (splash_screen_check_header(header)) {
		dprintf(CRITICAL, "ERROR: Boot image header invalid\n");
		return -1;
	}

	fb_display = fbcon_display();
	if (fb_display) {
		if (header->type && (header->blocks != 0) &&
				(UINT_MAX >= header->blocks * 512) &&
				((header->blocks * 512) <=  (fb_display->width *
				fb_display->height * (fb_display->bpp / 8)))) {
					/* RLE24 compressed data */
			uint8_t *base = (uint8_t *) fb_display->base + LOGO_IMG_OFFSET;

			/* if the logo is full-screen size, remove "fbcon_clear()" */
			if ((header->width != fb_display->width)
						|| (header->height != fb_display->height))
					fbcon_clear();

			if (flash_read(ptn + LOGO_IMG_HEADER_SIZE, 0,
				(uint32_t *)base,
				(header->blocks * 512))) {
				dprintf(CRITICAL, "ERROR: Cannot read splash image from partition\n");
				return -1;
			}
			fbcon_extract_to_screen(header, base);
			return 0;
		}

		if ((header->width != fb_display->width) || (header->height != fb_display->height)) {
			dprintf(CRITICAL, "Logo config doesn't match with fb config. Fall back default logo\n");
			return -1;
		}

		uint8_t *base = (uint8_t *) fb_display->base;
		uint32_t fb_size = ROUNDUP(fb_display->width *
					fb_display->height *
					(fb_display->bpp / 8), 4096);
		uint32_t splash_size = ((((header->width * header->height *
					fb_display->bpp/8) + 511) >> 9) << 9);

		if (splash_size > fb_size) {
			dprintf(CRITICAL, "ERROR: Splash image size invalid\n");
			return -1;
		}

		if (flash_read(ptn + LOGO_IMG_HEADER_SIZE, 0,
			(uint32_t *)base,
			((((header->width * header->height * fb_display->bpp/8) + 511) >> 9) << 9))) {
			fbcon_clear();
			dprintf(CRITICAL, "ERROR: Cannot read splash image from partition\n");
			return -1;
		}
	}

	return 0;
}

int splash_screen_mmc()
{
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	struct fbcon_config *fb_display = NULL;
	struct logo_img_header *header;
	uint32_t blocksize, realsize, readsize;
	uint8_t *base;

	index = partition_get_index("splash");
	if (index == 0) {
		dprintf(CRITICAL, "ERROR: splash Partition table not found\n");
		return -1;
	}

	ptn = partition_get_offset(index);
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: splash Partition invalid\n");
		return -1;
	}

	mmc_set_lun(partition_get_lun(index));

	blocksize = mmc_get_device_blocksize();
	if (blocksize == 0) {
		dprintf(CRITICAL, "ERROR:splash Partition invalid blocksize\n");
		return -1;
	}

	fb_display = fbcon_display();
	if (fb_display) {
		base = (uint8_t *) fb_display->base;

		if (mmc_read(ptn, (uint32_t *)(base + LOGO_IMG_OFFSET), blocksize)) {
			dprintf(CRITICAL, "ERROR: Cannot read splash image header\n");
			return -1;
		}

		header = (struct logo_img_header *)(base + LOGO_IMG_OFFSET);
		if (splash_screen_check_header(header)) {
			dprintf(CRITICAL, "ERROR: Splash image header invalid\n");
			return -1;
		}
	}

	if (fb_display) {
		if (header->type && (header->blocks != 0) &&
			(UINT_MAX >= header->blocks * 512 + LOGO_IMG_HEADER_SIZE) &&
			((header->blocks * 512) <=  (fb_display->width *
			fb_display->height * (fb_display->bpp / 8)))) {
			/* 1 RLE24 compressed data */
			base += LOGO_IMG_OFFSET;

			realsize =  header->blocks * 512;
			readsize =  ROUNDUP((realsize + LOGO_IMG_HEADER_SIZE), blocksize) - blocksize;

			/* if the logo is not full-screen size, clean screen */
			if ((header->width != fb_display->width)
						|| (header->height != fb_display->height))
				fbcon_clear();

			uint32_t fb_size = ROUNDUP(fb_display->width *
					fb_display->height *
					(fb_display->bpp / 8), 4096);

			if (readsize > fb_size) {
				dprintf(CRITICAL, "ERROR: Splash image size invalid\n");
				return -1;
			}

			if (mmc_read(ptn + blocksize, (uint32_t *)(base + blocksize), readsize)) {
				dprintf(CRITICAL, "ERROR: Cannot read splash image from partition\n");
				return -1;
			}

			fbcon_extract_to_screen(header, (base + LOGO_IMG_HEADER_SIZE));
		} else { /* 2 Raw BGR data */

			if ((header->width != fb_display->width) || (header->height != fb_display->height)) {
				dprintf(CRITICAL, "Logo config doesn't match with fb config. Fall back default logo\n");
				return -1;
			}

			realsize =  header->width * header->height * fb_display->bpp / 8;
			readsize =  ROUNDUP((realsize + LOGO_IMG_HEADER_SIZE), blocksize) - blocksize;

			if (blocksize == LOGO_IMG_HEADER_SIZE) { /* read the content directly */
				if (mmc_read((ptn + LOGO_IMG_HEADER_SIZE), (uint32_t *)base, readsize)) {
					fbcon_clear();
					dprintf(CRITICAL, "ERROR: Cannot read splash image from partition\n");
					return -1;
				}
			} else {
				if (mmc_read(ptn + blocksize ,
						(uint32_t *)(base + LOGO_IMG_OFFSET + blocksize), readsize)) {
					dprintf(CRITICAL, "ERROR: Cannot read splash image from partition\n");
					return -1;
				}
				memmove(base, (base + LOGO_IMG_OFFSET + LOGO_IMG_HEADER_SIZE), realsize);
			}
		}
	}

	return 0;
}

int fetch_image_from_partition()
{
	if (target_is_emmc_boot()) {
		return splash_screen_mmc();
	} else {
		return splash_screen_flash();
	}
}

/* Get the size from partiton name */
static void get_partition_size(const char *arg, char *response)
{
	uint64_t ptn = 0;
	uint64_t size;
	int index = INVALID_PTN;

	index = partition_get_index(arg);

	if (index == INVALID_PTN)
	{
		dprintf(CRITICAL, "Invalid partition index\n");
		return;
	}

	ptn = partition_get_offset(index);

	if(!ptn)
	{
		dprintf(CRITICAL, "Invalid partition name %s\n", arg);
		return;
	}

	size = partition_get_size(index);

	snprintf(response, MAX_RSP_SIZE, "\t 0x%llx", size);
	return;
}

/*
 * Publish the partition type & size info
 * fastboot getvar will publish the required information.
 * fastboot getvar partition_size:<partition_name>: partition size in hex
 * fastboot getvar partition_type:<partition_name>: partition type (ext/fat)
 */
static void publish_getvar_partition_info(struct getvar_partition_info *info, uint8_t num_parts)
{
	uint8_t i,n;
	struct partition_entry *ptn_entry =
				partition_get_partition_entries();

	for (i = 0; i < num_parts; i++) {
		strlcat(info[i].part_name, (char const *)ptn_entry[i].name, MAX_RSP_SIZE);
		strlcat(info[i].getvar_size, "partition-size:", MAX_GET_VAR_NAME_SIZE);
		strlcat(info[i].getvar_type, "partition-type:", MAX_GET_VAR_NAME_SIZE);

		/* Mark partiton type for known paritions only */
		for (n=0; n < ARRAY_SIZE(part_type_known); n++)
		{
			if (!strncmp(part_type_known[n].part_name, info[i].part_name,
					strlen(part_type_known[n].part_name)))
			{
				strlcat(info[i].type_response,
						part_type_known[n].type_response,
						MAX_RSP_SIZE);
				break;
			}
		}

		get_partition_size(info[i].part_name, info[i].size_response);

		if (strlcat(info[i].getvar_size, info[i].part_name, MAX_GET_VAR_NAME_SIZE) >= MAX_GET_VAR_NAME_SIZE)
		{
			dprintf(CRITICAL, "partition size name truncated\n");
			return;
		}
		if (strlcat(info[i].getvar_type, info[i].part_name, MAX_GET_VAR_NAME_SIZE) >= MAX_GET_VAR_NAME_SIZE)
		{
			dprintf(CRITICAL, "partition type name truncated\n");
			return;
		}

		/* publish partition size & type info */
		fastboot_publish((const char *) info[i].getvar_size, (const char *) info[i].size_response);
		fastboot_publish((const char *) info[i].getvar_type, (const char *) info[i].type_response);
	}
}

void get_product_name(unsigned char *buf)
{
	snprintf((char*)buf, MAX_RSP_SIZE, "%s",  TARGET(BOARD));
	return;
}

void get_bootloader_version(unsigned char *buf)
{
	snprintf((char*)buf, MAX_RSP_SIZE, "%s",  device.bootloader_version);
	return;
}

void get_baseband_version(unsigned char *buf)
{
	snprintf((char*)buf, MAX_RSP_SIZE, "%s", device.radio_version);
	return;
}

bool is_device_locked()
{
	return device.is_unlocked ? false:true;
}

/* register commands and variables for fastboot */
void aboot_fastboot_register_commands(void)
{
	int i;
	char hw_platform_buf[MAX_RSP_SIZE];

	struct fastboot_cmd_desc cmd_list[] = {
						/* By default the enabled list is empty. */
						{"", NULL},
						/* move commands enclosed within the below ifndef to here
						 * if they need to be enabled in user build.
						 */
#ifndef DISABLE_FASTBOOT_CMDS
						/* Register the following commands only for non-user builds */
						{"flash:", cmd_flash},
						{"erase:", cmd_erase},
						{"boot", cmd_boot},
						{"continue", cmd_continue},
						{"reboot", cmd_reboot},
						{"reboot-bootloader", cmd_reboot_bootloader},
						{"oem unlock", cmd_oem_unlock},
						{"oem unlock-go", cmd_oem_unlock_go},
						{"oem lock", cmd_oem_lock},
						{"flashing unlock", cmd_oem_unlock},
						{"flashing lock", cmd_oem_lock},
						{"flashing lock_critical", cmd_flashing_lock_critical},
						{"flashing unlock_critical", cmd_flashing_unlock_critical},
						{"flashing get_unlock_ability", cmd_flashing_get_unlock_ability},
						{"oem device-info", cmd_oem_devinfo},
						{"preflash", cmd_preflash},
						{"oem enable-charger-screen", cmd_oem_enable_charger_screen},
						{"oem disable-charger-screen", cmd_oem_disable_charger_screen},
						{"oem off-mode-charge", cmd_oem_off_mode_charger},
						{"oem select-display-panel", cmd_oem_select_display_panel},
#endif
						};

	int fastboot_cmds_count = sizeof(cmd_list)/sizeof(cmd_list[0]);
	for (i = 1; i < fastboot_cmds_count; i++)
		fastboot_register(cmd_list[i].name,cmd_list[i].cb);

	/* publish variables and their values */
	fastboot_publish("product",  TARGET(BOARD));
	fastboot_publish("kernel",   "lk");
	fastboot_publish("serialno", sn_buf);

	/*
	 * partition info is supported only for emmc partitions
	 * Calling this for NAND prints some error messages which
	 * is harmless but misleading. Avoid calling this for NAND
	 * devices.
	 */
	if (target_is_emmc_boot())
		publish_getvar_partition_info(part_info, partition_get_partition_count());

	/* Max download size supported */
	snprintf(max_download_size, MAX_RSP_SIZE, "\t0x%x",
			target_get_max_flash_size());
	fastboot_publish("max-download-size", (const char *) max_download_size);
	/* Is the charger screen check enabled */
	snprintf(charger_screen_enabled, MAX_RSP_SIZE, "%d",
			device.charger_screen_enabled);
	fastboot_publish("charger-screen-enabled",
			(const char *) charger_screen_enabled);
	fastboot_publish("off-mode-charge", (const char *) charger_screen_enabled);
	snprintf(panel_display_mode, MAX_RSP_SIZE, "%s",
			device.display_panel);
	fastboot_publish("display-panel",
			(const char *) panel_display_mode);
	fastboot_publish("version-bootloader", (const char *) device.bootloader_version);
	fastboot_publish("version-baseband", (const char *) device.radio_version);
	fastboot_publish("secure", is_secure_boot_enable()? "yes":"no");
	fastboot_publish("unlocked", device.is_unlocked ? "yes":"no");
	smem_get_hw_platform_name((unsigned char *) hw_platform_buf, sizeof(hw_platform_buf));
	snprintf(get_variant, MAX_RSP_SIZE, "%s %s", hw_platform_buf,
		target_is_emmc_boot()? "eMMC":"UFS");
	fastboot_publish("variant", (const char *) get_variant);
#if CHECK_BAT_VOLTAGE
	snprintf(battery_voltage, MAX_RSP_SIZE, "%d",
		target_get_battery_voltage());
	fastboot_publish("battery-voltage", (const char *) battery_voltage);
	fastboot_publish("battery-soc-ok", target_battery_soc_ok()? "yes":"no");
#endif
}

void aboot_init(const struct app_descriptor *app)
{
	unsigned reboot_mode = 0;
	bool boot_into_fastboot = false;

	/* Setup page size information for nv storage */
	if (target_is_emmc_boot())
	{
		page_size = mmc_page_size();
		page_mask = page_size - 1;
		mmc_blocksize = mmc_get_device_blocksize();
		mmc_blocksize_mask = mmc_blocksize - 1;
	}
	else
	{
		page_size = flash_page_size();
		page_mask = page_size - 1;
	}

	ASSERT((MEMBASE + MEMSIZE) > MEMBASE);

	lk2nd_init();
	read_device_info(&device);
	read_allow_oem_unlock(&device);

	/* Display splash screen if enabled */
#if DISPLAY_SPLASH_SCREEN
	dprintf(SPEW, "Display Init: Start\n");
	target_display_init(device.display_panel);
	dprintf(SPEW, "Display Init: Done\n");
#endif


	target_serialno((unsigned char *) sn_buf);
	dprintf(SPEW,"serial number: %s\n",sn_buf);

	memset(display_panel_buf, '\0', MAX_PANEL_BUF_SIZE);

	/* Check if we should do something other than booting up */
	if (keys_get_state(KEY_VOLUMEUP) && keys_get_state(KEY_VOLUMEDOWN))
	{
		dprintf(ALWAYS,"dload mode key sequence detected\n");
		if (set_download_mode(EMERGENCY_DLOAD))
		{
			dprintf(CRITICAL,"dload mode not supported by target\n");
		}
		else
		{
			reboot_device(DLOAD);
			dprintf(CRITICAL,"Failed to reboot into dload mode\n");
		}
		boot_into_fastboot = true;
	}
	if (!boot_into_fastboot)
	{
		if (keys_get_state(KEY_HOME) || keys_get_state(KEY_VOLUMEUP))
			boot_into_recovery = 1;
		if (!boot_into_recovery &&
			(keys_get_state(KEY_BACK) || keys_get_state(KEY_VOLUMEDOWN)))
			boot_into_fastboot = true;
	}
	#if NO_KEYPAD_DRIVER
	if (fastboot_trigger())
		boot_into_fastboot = true;
	#endif

#if USE_PON_REBOOT_REG
	reboot_mode = check_hard_reboot_mode();
#else
	reboot_mode = check_reboot_mode();
#endif
	if (reboot_mode == RECOVERY_MODE)
	{
		boot_into_recovery = 1;
	}
	else if(reboot_mode == FASTBOOT_MODE)
	{
		boot_into_fastboot = true;
	}
	else if(reboot_mode == ALARM_BOOT)
	{
		boot_reason_alarm = true;
        }
#if VERIFIED_BOOT
#if !VBOOT_MOTA
        else if(reboot_mode == DM_VERITY_ENFORCING) {
		device.verity_mode = 1;
		write_device_info(&device);
	}
#if ENABLE_VB_ATTEST
	else if (reboot_mode == DM_VERITY_EIO)
#else
	else if (reboot_mode == DM_VERITY_LOGGING)
#endif
	{
		device.verity_mode = 0;
		write_device_info(&device);
	} else if(reboot_mode == DM_VERITY_KEYSCLEAR) {
		if(send_delete_keys_to_tz())
			ASSERT(0);
	}
#endif
#endif

normal_boot:
	if (!boot_into_fastboot)
	{
		if (target_is_emmc_boot())
		{
			if(emmc_recovery_init())
				dprintf(ALWAYS,"error in emmc_recovery_init\n");
			if(target_use_signed_kernel())
			{
				if((device.is_unlocked) || (device.is_tampered))
				{
				#ifdef TZ_TAMPER_FUSE
					set_tamper_fuse_cmd();
				#endif
				#if USE_PCOM_SECBOOT
					set_tamper_flag(device.is_tampered);
				#endif
				}
			}
			boot_linux_from_mmc();
		}
		else
		{
			recovery_init();
	#if USE_PCOM_SECBOOT
		if((device.is_unlocked) || (device.is_tampered))
			set_tamper_flag(device.is_tampered);
	#endif
			boot_linux_from_flash();
		}
		dprintf(CRITICAL, "ERROR: Could not do normal boot. Reverting "
			"to fastboot mode.\n");
	}

	/* We are here means regular boot did not happen. Start fastboot. */

	/* register aboot specific fastboot commands */
	aboot_fastboot_register_commands();
	fastboot_lk2nd_register_commands();

	/* dump partition table for debug info */
	partition_dump();

	/* initialize and start fastboot */
	fastboot_init(target_get_scratch_address(), target_get_max_flash_size());
#if FBCON_DISPLAY_MSG
	display_fastboot_menu();
#endif
}

uint32_t get_page_size()
{
	return page_size;
}

/*
 * Calculated and save hash (SHA256) for non-signed boot image.
 *
 * @param image_addr - Boot image address
 * @param image_size - Size of the boot image
 *
 * @return int - 0 on success, negative value on failure.
 */
static int aboot_save_boot_hash_mmc(uint32_t image_addr, uint32_t image_size)
{
	unsigned int digest[8];
#if IMAGE_VERIF_ALGO_SHA1
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA1;
#else
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA256;
#endif

	target_crypto_init_params();
	hash_find(image_addr, image_size, (unsigned char *)&digest, auth_algo);

	save_kernel_hash_cmd(digest);
	dprintf(INFO, "aboot_save_boot_hash_mmc: imagesize_actual size %d bytes.\n", (int) image_size);

	return 0;
}

APP_START(aboot)
	.init = aboot_init,
APP_END
