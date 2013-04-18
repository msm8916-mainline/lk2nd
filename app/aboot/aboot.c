/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2013, The Linux Foundation. All rights reserved.
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
#include <arch/arm.h>
#include <dev/udc.h>
#include <string.h>
#include <kernel/thread.h>
#include <arch/ops.h>

#include <dev/flash.h>
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

#if DEVICE_TREE
#include <libfdt.h>
#include <dev_tree.h>
#endif

#include "image_verify.h"
#include "recovery.h"
#include "bootimg.h"
#include "fastboot.h"
#include "sparse_format.h"
#include "mmc.h"
#include "devinfo.h"
#include "board.h"

#include "scm.h"

extern  bool target_use_signed_kernel(void);
extern void dsb();
extern void isb();
extern void platform_uninit(void);

void write_device_info_mmc(device_info *dev);
void write_device_info_flash(device_info *dev);

#define EXPAND(NAME) #NAME
#define TARGET(NAME) EXPAND(NAME)

#ifdef MEMBASE
#define EMMC_BOOT_IMG_HEADER_ADDR (0xFF000+(MEMBASE))
#else
#define EMMC_BOOT_IMG_HEADER_ADDR 0xFF000
#endif

#define RECOVERY_MODE   0x77665502
#define FASTBOOT_MODE   0x77665500

static const char *emmc_cmdline = " androidboot.emmc=true";
static const char *usb_sn_cmdline = " androidboot.serialno=";
static const char *battchg_pause = " androidboot.mode=charger";
static const char *auth_kernel = " androidboot.authorized_kernel=true";

static const char *baseband_apq     = " androidboot.baseband=apq";
static const char *baseband_msm     = " androidboot.baseband=msm";
static const char *baseband_csfb    = " androidboot.baseband=csfb";
static const char *baseband_svlte2a = " androidboot.baseband=svlte2a";
static const char *baseband_mdm     = " androidboot.baseband=mdm";
static const char *baseband_sglte   = " androidboot.baseband=sglte";
static const char *baseband_dsda    = " androidboot.baseband=dsda";
static const char *baseband_dsda2   = " androidboot.baseband=dsda2";
static const char *baseband_sglte2  = " androidboot.baseband=sglte2";

/* Assuming unauthorized kernel image by default */
static int auth_kernel_img = 0;

static device_info device = {DEVICE_MAGIC, 0, 0};

static struct udc_device surf_udc_device = {
	.vendor_id	= 0x18d1,
	.product_id	= 0xD00D,
	.version_id	= 0x0100,
	.manufacturer	= "Google",
	.product	= "Android",
};

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
	const char part_name[MAX_GPT_NAME_SIZE]; /* Partition name */
	char getvar_size[MAX_GET_VAR_NAME_SIZE]; /* fastboot get var name for size */
	char getvar_type[MAX_GET_VAR_NAME_SIZE]; /* fastboot get var name for type */
	char size_response[MAX_RSP_SIZE];        /* fastboot response for size */
	char type_response[MAX_RSP_SIZE];        /* fastboot response for type */
};

/*
 * Right now, we are publishing the info for only
 * three partitions
 */
struct getvar_partition_info part_info[] =
{
	{ "system"  , "partition-size:", "partition-type:", "", "ext4" },
	{ "userdata", "partition-size:", "partition-type:", "", "ext4" },
	{ "cache"   , "partition-size:", "partition-type:", "", "ext4" },
};

char max_download_size[MAX_RSP_SIZE];
char sn_buf[13];

extern int emmc_recovery_init(void);

#if NO_KEYPAD_DRIVER
extern int fastboot_trigger(void);
#endif

static void update_ker_tags_rdisk_addr(struct boot_img_hdr *hdr)
{
	/* overwrite the destination of specified for the project */
	/*
	 * Update the value to sane values only when the boot image
	 * header does not have sane values, this is added to make sure
	 * that we always use values from boot.img header and use the
	 * force values when boot image header has default values.
	 */
#ifdef ABOOT_FORCE_KERNEL_ADDR
	if (hdr->kernel_addr == ABOOT_DEFAULT_KERNEL_ADDR)
		hdr->kernel_addr = ABOOT_FORCE_KERNEL_ADDR;
#endif
#ifdef ABOOT_FORCE_RAMDISK_ADDR
	if (hdr->ramdisk_addr == ABOOT_DEFAULT_RAMDISK_ADDR)
		hdr->ramdisk_addr = ABOOT_FORCE_RAMDISK_ADDR;
#endif
#ifdef ABOOT_FORCE_TAGS_ADDR
	if (hdr->tags_addr == ABOOT_DEFAULT_TAGS_ADDR)
		hdr->tags_addr = ABOOT_FORCE_TAGS_ADDR;
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

unsigned char *update_cmdline(const char * cmdline)
{
	int cmdline_len = 0;
	int have_cmdline = 0;
	unsigned char *cmdline_final = NULL;
	int pause_at_bootup = 0;

	if (cmdline && cmdline[0]) {
		cmdline_len = strlen(cmdline);
		have_cmdline = 1;
	}
	if (target_is_emmc_boot()) {
		cmdline_len += strlen(emmc_cmdline);
	}

	cmdline_len += strlen(usb_sn_cmdline);
	cmdline_len += strlen(sn_buf);

	if (target_pause_for_battery_charge()) {
		pause_at_bootup = 1;
		cmdline_len += strlen(battchg_pause);
	}

	if(target_use_signed_kernel() && auth_kernel_img) {
		cmdline_len += strlen(auth_kernel);
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

	if (cmdline_len > 0) {
		const char *src;
		unsigned char *dst = (unsigned char*) malloc((cmdline_len + 4) & (~3));
		ASSERT(dst != NULL);

		/* Save start ptr for debug print */
		cmdline_final = dst;
		if (have_cmdline) {
			src = cmdline;
			while ((*dst++ = *src++));
		}
		if (target_is_emmc_boot()) {
			src = emmc_cmdline;
			if (have_cmdline) --dst;
			have_cmdline = 1;
			while ((*dst++ = *src++));
		}

		src = usb_sn_cmdline;
		if (have_cmdline) --dst;
		have_cmdline = 1;
		while ((*dst++ = *src++));
		src = sn_buf;
		if (have_cmdline) --dst;
		have_cmdline = 1;
		while ((*dst++ = *src++));

		if (pause_at_bootup) {
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
	}

	dprintf(INFO, "cmdline: %s\n", cmdline_final);

	return cmdline_final;
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

	ptr = atag_core(ptr);
	ptr = atag_ramdisk(ptr, ramdisk, ramdisk_size);
	ptr = target_atag_mem(ptr);

	/* Skip NAND partition ATAGS for eMMC boot */
	if (!target_is_emmc_boot()){
		ptr = atag_ptable(&ptr);
	}

	ptr = atag_cmdline(ptr, cmdline);
	ptr = atag_end(ptr);
}

typedef void entry_func_ptr(unsigned, unsigned, unsigned*);
void boot_linux(void *kernel, unsigned *tags,
		const char *cmdline, unsigned machtype,
		void *ramdisk, unsigned ramdisk_size)
{
	unsigned char *final_cmdline;
#if DEVICE_TREE
	int ret = 0;
#endif

	void (*entry)(unsigned, unsigned, unsigned*) = (entry_func_ptr*)(PA((addr_t)kernel));
	uint32_t tags_phys = PA((addr_t)tags);

	ramdisk = PA(ramdisk);

	final_cmdline = update_cmdline((const char*)cmdline);

#if DEVICE_TREE
	dprintf(INFO, "Updating device tree: start\n");

	/* Update the Device Tree */
	ret = update_device_tree((void *)tags, final_cmdline, ramdisk, ramdisk_size);
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
	entry(0, machtype, (unsigned*)tags_phys);
}

unsigned page_size = 0;
unsigned page_mask = 0;

#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

static unsigned char buf[4096]; //Equal to max-supported pagesize
#if DEVICE_TREE
static unsigned char dt_buf[4096];
#endif

int boot_linux_from_mmc(void)
{
	struct boot_img_hdr *hdr = (void*) buf;
	struct boot_img_hdr *uhdr;
	unsigned offset = 0;
	unsigned long long ptn = 0;
	int index = INVALID_PTN;

	unsigned char *image_addr = 0;
	unsigned kernel_actual;
	unsigned ramdisk_actual;
	unsigned imagesize_actual;
	unsigned second_actual = 0;

#if DEVICE_TREE
	struct dt_table *table;
	struct dt_entry *dt_entry_ptr;
	unsigned dt_table_offset;
	uint32_t dt_actual;
#endif

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
		page_size = hdr->page_size;
		page_mask = page_size - 1;
	}

	/*
	 * Update the kernel/ramdisk/tags address if the boot image header
	 * has default values, these default values come from mkbootimg when
	 * the boot image is flashed using fastboot flash:raw
	 */
	update_ker_tags_rdisk_addr(hdr);

	/* Get virtual addresses since the hdr saves physical addresses. */
	hdr->kernel_addr = VA((addr_t)(hdr->kernel_addr));
	hdr->ramdisk_addr = VA((addr_t)(hdr->ramdisk_addr));
	hdr->tags_addr = VA((addr_t)(hdr->tags_addr));

	/* Authenticate Kernel */
	if(target_use_signed_kernel() && (!device.is_unlocked) && (!device.is_tampered))
	{
		image_addr = (unsigned char *)target_get_scratch_address();
		kernel_actual = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
		ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
#if DEVICE_TREE
		dt_actual = ROUND_TO_PAGE(hdr->dt_size, page_mask);
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual + dt_actual);
#else
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual);
#endif
		offset = 0;

		/* Assuming device rooted at this time */
		device.is_tampered = 1;

		dprintf(INFO, "Loading boot image (%d): start\n", imagesize_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_START);

		/* Read image without signature */
		if (mmc_read(ptn + offset, (void *)image_addr, imagesize_actual))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image\n");
				return -1;
		}

		dprintf(INFO, "Loading boot image (%d): done\n", imagesize_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		offset = imagesize_actual;
		/* Read signature */
		if(mmc_read(ptn + offset, (void *)(image_addr + offset), page_size))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image signature\n");
		}
		else
		{
			dprintf(INFO, "Authenticating boot image (%d): start\n", imagesize_actual);

			auth_kernel_img = image_verify((unsigned char *)image_addr,
					(unsigned char *)(image_addr + imagesize_actual),
					imagesize_actual,
					CRYPTO_AUTH_ALG_SHA256);

			dprintf(INFO, "Authenticating boot image (%d): done\n", imagesize_actual);

			if(auth_kernel_img)
			{
				/* Authorized kernel */
				device.is_tampered = 0;
			}
		}

		/* Move kernel, ramdisk and device tree to correct address */
		memmove((void*) hdr->kernel_addr, (char *)(image_addr + page_size), hdr->kernel_size);
		memmove((void*) hdr->ramdisk_addr, (char *)(image_addr + page_size + kernel_actual), hdr->ramdisk_size);
#if DEVICE_TREE
		memmove((void*) hdr->tags_addr, (char *)(image_addr + page_size + kernel_actual + ramdisk_actual), hdr->dt_size);
#endif

		#if DEVICE_TREE
		if(hdr->dt_size) {
			table = (struct dt_table*) dt_buf;
			dt_table_offset = ((uint32_t)image_addr + page_size + kernel_actual + ramdisk_actual + second_actual);

			memmove((void *) dt_buf, (char *)dt_table_offset, page_size);

			/* Restriction that the device tree entry table should be less than a page*/
			ASSERT(((table->num_entries * sizeof(struct dt_entry))+ DEV_TREE_HEADER_SIZE) < hdr->page_size);

			/* Validate the device tree table header */
			if((table->magic != DEV_TREE_MAGIC) && (table->version != DEV_TREE_VERSION)) {
				dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
				return -1;
			}

			/* Find index of device tree within device tree table */
			if((dt_entry_ptr = dev_tree_get_entry_ptr(table)) == NULL){
				dprintf(CRITICAL, "ERROR: Device Tree Blob cannot be found\n");
				return -1;
			}

			/* Read device device tree in the "tags_add */
			memmove((void *)hdr->tags_addr, (char *)dt_table_offset + dt_entry_ptr->offset, dt_entry_ptr->size);
		} else {
			/*
			 * If appended dev tree is found, update the atags with
			 * memory address to the DTB appended location on RAM.
			 * Else update with the atags address in the kernel header
			 */
			void *dtb;
			dtb = dev_tree_appended((void*) hdr->kernel_addr,
						(void *)hdr->tags_addr);
			if (!dtb) {
				dprintf(CRITICAL, "ERROR: Appended Device Tree Blob not found\n");
				return -1;
			}
		}
		#endif
		/* Make sure everything from scratch address is read before next step!*/
		if(device.is_tampered)
		{
			write_device_info_mmc(&device);
		#ifdef TZ_TAMPER_FUSE
			set_tamper_fuse_cmd();
		#endif
		}
	#if USE_PCOM_SECBOOT
		set_tamper_flag(device.is_tampered);
	#endif
	}
	else
	{
		kernel_actual  = ROUND_TO_PAGE(hdr->kernel_size,  page_mask);
		ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
		second_actual  = ROUND_TO_PAGE(hdr->second_size,  page_mask);

		dprintf(INFO, "Loading boot image (%d): start\n",
				kernel_actual + ramdisk_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_START);

		offset = page_size;

		/* Load kernel */
		if (mmc_read(ptn + offset, (void *)hdr->kernel_addr, kernel_actual)) {
			dprintf(CRITICAL, "ERROR: Cannot read kernel image\n");
					return -1;
		}
		offset += kernel_actual;

		/* Load ramdisk */
		if(ramdisk_actual != 0)
		{
			if (mmc_read(ptn + offset, (void *)hdr->ramdisk_addr, ramdisk_actual)) {
				dprintf(CRITICAL, "ERROR: Cannot read ramdisk image\n");
				return -1;
			}
		}
		offset += ramdisk_actual;

		dprintf(INFO, "Loading boot image (%d): done\n",
				kernel_actual + ramdisk_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		if(hdr->second_size != 0) {
			offset += second_actual;
			/* Second image loading not implemented. */
			ASSERT(0);
		}

		#if DEVICE_TREE
		if(hdr->dt_size != 0) {

			/* Read the device tree table into buffer */
			if(mmc_read(ptn + offset,(unsigned int *) dt_buf, page_size)) {
				dprintf(CRITICAL, "ERROR: Cannot read the Device Tree Table\n");
				return -1;
			}
			table = (struct dt_table*) dt_buf;

			/* Restriction that the device tree entry table should be less than a page*/
			ASSERT(((table->num_entries * sizeof(struct dt_entry))+ DEV_TREE_HEADER_SIZE) < hdr->page_size);

			/* Validate the device tree table header */
			if((table->magic != DEV_TREE_MAGIC) && (table->version != DEV_TREE_VERSION)) {
				dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
				return -1;
			}

			/* Calculate the offset of device tree within device tree table */
			if((dt_entry_ptr = dev_tree_get_entry_ptr(table)) == NULL){
				dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
				return -1;
			}

			/* Read device device tree in the "tags_add */
			if(mmc_read(ptn + offset + dt_entry_ptr->offset,
						 (void *)hdr->tags_addr, dt_entry_ptr->size)) {
				dprintf(CRITICAL, "ERROR: Cannot read device tree\n");
				return -1;
			}
		} else {
			/*
			 * If appended dev tree is found, update the atags with
			 * memory address to the DTB appended location on RAM.
			 * Else update with the atags address in the kernel header
			 */
			void *dtb;
			dtb = dev_tree_appended((void*) hdr->kernel_addr,
						(void *)hdr->tags_addr);
			if (!dtb) {
				dprintf(CRITICAL, "ERROR: Appended Device Tree Blob not found\n");
				return -1;
			}
		}
		#endif
	}

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
	unsigned second_actual;

#if DEVICE_TREE
	struct dt_table *table;
	struct dt_entry *dt_entry_ptr;
	uint32_t dt_actual;
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

	/*
	 * Update the kernel/ramdisk/tags address if the boot image header
	 * has default values, these default values come from mkbootimg when
	 * the boot image is flashed using fastboot flash:raw
	 */
	update_ker_tags_rdisk_addr(hdr);

	/* Get virtual addresses since the hdr saves physical addresses. */
	hdr->kernel_addr = VA(hdr->kernel_addr);
	hdr->ramdisk_addr = VA(hdr->ramdisk_addr);
	hdr->tags_addr = VA(hdr->tags_addr);

	/* Authenticate Kernel */
	if(target_use_signed_kernel() && (!device.is_unlocked) && (!device.is_tampered))
	{
		image_addr = (unsigned char *)target_get_scratch_address();
		kernel_actual = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
		ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
#if DEVICE_TREE
		dt_actual = ROUND_TO_PAGE(hdr->dt_size, page_mask);
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual + dt_actual);
#else
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual);
#endif
		offset = 0;

		/* Assuming device rooted at this time */
		device.is_tampered = 1;

		dprintf(INFO, "Loading boot image (%d): start\n", imagesize_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_START);

		/* Read image without signature */
		if (flash_read(ptn, offset, (void *)image_addr, imagesize_actual))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image\n");
				return -1;
		}

		dprintf(INFO, "Loading boot image (%d): done\n", imagesize_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		offset = imagesize_actual;
		/* Read signature */
		if (flash_read(ptn, offset, (void *)(image_addr + offset), page_size))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image signature\n");
		}
		else
		{
			dprintf(INFO, "Authenticating boot image (%d): start\n", imagesize_actual);

			/* Verify signature */
			auth_kernel_img = image_verify((unsigned char *)image_addr,
						(unsigned char *)(image_addr + imagesize_actual),
						imagesize_actual,
						CRYPTO_AUTH_ALG_SHA256);

			dprintf(INFO, "Authenticating boot image (%d): done\n", imagesize_actual);

			if(auth_kernel_img)
			{
				/* Authorized kernel */
				device.is_tampered = 0;
			}
		}

		/* Move kernel and ramdisk to correct address */
		memmove((void*) hdr->kernel_addr, (char *)(image_addr + page_size), hdr->kernel_size);
		memmove((void*) hdr->ramdisk_addr, (char *)(image_addr + page_size + kernel_actual), hdr->ramdisk_size);
#if DEVICE_TREE
		memmove((void*) hdr->tags_addr, (char *)(image_addr + page_size + kernel_actual + ramdisk_actual), hdr->dt_size);
#endif

		/* Make sure everything from scratch address is read before next step!*/
		if(device.is_tampered)
		{
			write_device_info_flash(&device);
		}
#if USE_PCOM_SECBOOT
		set_tamper_flag(device.is_tampered);
#endif
	}
	else
	{
		offset = page_size;

		kernel_actual = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
		ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
		second_actual = ROUND_TO_PAGE(hdr->second_size, page_mask);

		dprintf(INFO, "Loading boot image (%d): start\n",
				kernel_actual + ramdisk_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_START);

		if (flash_read(ptn, offset, (void *)hdr->kernel_addr, kernel_actual)) {
			dprintf(CRITICAL, "ERROR: Cannot read kernel image\n");
			return -1;
		}
		offset += kernel_actual;

		if (flash_read(ptn, offset, (void *)hdr->ramdisk_addr, ramdisk_actual)) {
			dprintf(CRITICAL, "ERROR: Cannot read ramdisk image\n");
			return -1;
		}
		offset += ramdisk_actual;

		dprintf(INFO, "Loading boot image (%d): done\n",
				kernel_actual + ramdisk_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		if(hdr->second_size != 0) {
			offset += second_actual;
			/* Second image loading not implemented. */
			ASSERT(0);
		}

#if DEVICE_TREE
		if(hdr->dt_size != 0) {

			/* Read the device tree table into buffer */
			if(flash_read(ptn, offset, (void *) dt_buf, page_size)) {
				dprintf(CRITICAL, "ERROR: Cannot read the Device Tree Table\n");
				return -1;
			}

			table = (struct dt_table*) dt_buf;

			/* Restriction that the device tree entry table should be less than a page*/
			ASSERT(((table->num_entries * sizeof(struct dt_entry))+ DEV_TREE_HEADER_SIZE) < hdr->page_size);

			/* Validate the device tree table header */
			if((table->magic != DEV_TREE_MAGIC) && (table->version != DEV_TREE_VERSION)) {
				dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
				return -1;
			}

			/* Calculate the offset of device tree within device tree table */
			if((dt_entry_ptr = dev_tree_get_entry_ptr(table)) == NULL){
				dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
				return -1;
			}

			/* Read device device tree in the "tags_add */
			if(flash_read(ptn, offset + dt_entry_ptr->offset,
						 (void *)hdr->tags_addr, dt_entry_ptr->size)) {
				dprintf(CRITICAL, "ERROR: Cannot read device tree\n");
				return -1;
			}
		}
#endif

	}
continue_boot:

	/* TODO: create/pass atags to kernel */

	boot_linux((void *)hdr->kernel_addr, (void *)hdr->tags_addr,
		   (const char *)hdr->cmdline, board_machtype(),
		   (void *)hdr->ramdisk_addr, hdr->ramdisk_size);

	return 0;
}

unsigned char info_buf[4096];
void write_device_info_mmc(device_info *dev)
{
	struct device_info *info = (void*) info_buf;
	unsigned long long ptn = 0;
	unsigned long long size;
	int index = INVALID_PTN;

	index = partition_get_index("aboot");
	ptn = partition_get_offset(index);
	if(ptn == 0)
	{
		return;
	}

	size = partition_get_size(index);

	memcpy(info, dev, sizeof(device_info));

	if(mmc_write((ptn + size - 512), 512, (void *)info_buf))
	{
		dprintf(CRITICAL, "ERROR: Cannot write device info\n");
		return;
	}
}

void read_device_info_mmc(device_info *dev)
{
	struct device_info *info = (void*) info_buf;
	unsigned long long ptn = 0;
	unsigned long long size;
	int index = INVALID_PTN;

	index = partition_get_index("aboot");
	ptn = partition_get_offset(index);
	if(ptn == 0)
	{
		return;
	}

	size = partition_get_size(index);

	if(mmc_read((ptn + size - 512), (void *)info_buf, 512))
	{
		dprintf(CRITICAL, "ERROR: Cannot read device info\n");
		return;
	}

	if (memcmp(info->magic, DEVICE_MAGIC, DEVICE_MAGIC_SIZE))
	{
		memcpy(info->magic, DEVICE_MAGIC, DEVICE_MAGIC_SIZE);
		info->is_unlocked = 0;
		info->is_tampered = 0;

		write_device_info_mmc(info);
	}
	memcpy(dev, info, sizeof(device_info));
}

void write_device_info_flash(device_info *dev)
{
	struct device_info *info = (void *) info_buf;
	struct ptentry *ptn;
	struct ptable *ptable;

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

	memcpy(info, dev, sizeof(device_info));

	if (flash_write(ptn, 0, (void *)info_buf, page_size))
	{
		dprintf(CRITICAL, "ERROR: Cannot write device info\n");
			return;
	}
}

void read_device_info_flash(device_info *dev)
{
	struct device_info *info = (void*) info_buf;
	struct ptentry *ptn;
	struct ptable *ptable;

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
}

void write_device_info(device_info *dev)
{
	if(target_is_emmc_boot())
	{
		write_device_info_mmc(dev);
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
		read_device_info_mmc(dev);
	}
	else
	{
		read_device_info_flash(dev);
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

#if DEVICE_TREE
int copy_dtb(uint8_t *boot_image_start)
{
	uint32 dt_image_offset = 0;
	uint32_t n;
	struct dt_table *table;
	struct dt_entry *dt_entry_ptr;

	struct boot_img_hdr *hdr = (struct boot_img_hdr *) (boot_image_start);

	if(hdr->dt_size != 0) {

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

		/* Restriction that the device tree entry table should be less than a page*/
		ASSERT(((table->num_entries * sizeof(struct dt_entry))+ DEV_TREE_HEADER_SIZE) < hdr->page_size);

		/* Validate the device tree table header */
		if((table->magic != DEV_TREE_MAGIC) && (table->version != DEV_TREE_VERSION)) {
			dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
			return -1;
		}

		/* Calculate the offset of device tree within device tree table */
		if((dt_entry_ptr = dev_tree_get_entry_ptr(table)) == NULL){
			dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
			return -1;
		}

		/* Read device device tree in the "tags_add */
		memmove((void*) hdr->tags_addr,
				boot_image_start + dt_image_offset +  dt_entry_ptr->offset,
				dt_entry_ptr->size);
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
	struct boot_img_hdr *hdr;
	char *ptr = ((char*) data);
	int ret = 0;
	uint8_t dtb_copied = 0;

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

	/*
	 * Update the kernel/ramdisk/tags address if the boot image header
	 * has default values, these default values come from mkbootimg when
	 * the boot image is flashed using fastboot flash:raw
	 */
	update_ker_tags_rdisk_addr(hdr);

	/* Get virtual addresses since the hdr saves physical addresses. */
	hdr->kernel_addr = VA(hdr->kernel_addr);
	hdr->ramdisk_addr = VA(hdr->ramdisk_addr);
	hdr->tags_addr = VA(hdr->tags_addr);

	/* sz should have atleast raw boot image */
	if (page_size + kernel_actual + ramdisk_actual > sz) {
		fastboot_fail("incomplete bootimage");
		return;
	}

#if DEVICE_TREE
	/* find correct dtb and copy it to right location */
	ret = copy_dtb(data);

	dtb_copied = !ret ? 1 : 0;
#endif

	/* Load ramdisk & kernel */
	memmove((void*) hdr->ramdisk_addr, ptr + page_size + kernel_actual, hdr->ramdisk_size);
	memmove((void*) hdr->kernel_addr, ptr + page_size, hdr->kernel_size);

#if DEVICE_TREE
	/*
	 * If dtb is not found look for appended DTB in the kernel.
	 * If appended dev tree is found, update the atags with
	 * memory address to the DTB appended location on RAM.
	 * Else update with the atags address in the kernel header
	 */
	if (!dtb_copied) {
		void *dtb;
		dtb = dev_tree_appended((void *)hdr->kernel_addr, (void *)hdr->tags_addr);
		if (!dtb) {
			fastboot_fail("dtb not found");
			return;
		}
	}
#endif

	fastboot_okay("");
	udc_stop();

	boot_linux((void*) hdr->kernel_addr, (void*) hdr->tags_addr,
		   (const char*) hdr->cmdline, board_machtype(),
		   (void*) hdr->ramdisk_addr, hdr->ramdisk_size);
}

void cmd_erase(const char *arg, void *data, unsigned sz)
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
	unsigned long long ptn = 0;
	unsigned int out[512] = {0};
	int index = INVALID_PTN;

	index = partition_get_index(arg);
	ptn = partition_get_offset(index);

	if(ptn == 0) {
		fastboot_fail("Partition table doesn't exist\n");
		return;
	}
	/* Simple inefficient version of erase. Just writing
       0 in first block */
	if (mmc_write(ptn , 512, (unsigned int *)out)) {
		fastboot_fail("failed to erase partition");
		return;
	}
	fastboot_okay("");
}


void cmd_flash_mmc_img(const char *arg, void *data, unsigned sz)
{
	unsigned long long ptn = 0;
	unsigned long long size = 0;
	int index = INVALID_PTN;

	if (!strcmp(arg, "partition"))
	{
		dprintf(INFO, "Attempt to write partition image.\n");
		if (write_partition(sz, (unsigned char *) data)) {
			fastboot_fail("failed to write partition");
			return;
		}
	}
	else
	{
		index = partition_get_index(arg);
		ptn = partition_get_offset(index);
		if(ptn == 0) {
			fastboot_fail("partition table doesn't exist");
			return;
		}

		if (!strcmp(arg, "boot") || !strcmp(arg, "recovery")) {
			if (memcmp((void *)data, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
				fastboot_fail("image is not a boot image");
				return;
			}
		}

		size = partition_get_size(index);
		if (ROUND_TO_PAGE(sz,511) > size) {
			fastboot_fail("size too large");
			return;
		}
		else if (mmc_write(ptn , sz, (unsigned int *)data)) {
			fastboot_fail("flash write failure");
			return;
		}
	}
	fastboot_okay("");
	return;
}

void cmd_flash_mmc_sparse_img(const char *arg, void *data, unsigned sz)
{
	unsigned int chunk;
	unsigned int chunk_data_sz;
	sparse_header_t *sparse_header;
	chunk_header_t *chunk_header;
	uint32_t total_blocks = 0;
	unsigned long long ptn = 0;
	unsigned long long size = 0;
	int index = INVALID_PTN;

	index = partition_get_index(arg);
	ptn = partition_get_offset(index);
	if(ptn == 0) {
		fastboot_fail("partition table doesn't exist");
		return;
	}

	size = partition_get_size(index);
	if (ROUND_TO_PAGE(sz,511) > size) {
		fastboot_fail("size too large");
		return;
	}

	/* Read and skip over sparse image header */
	sparse_header = (sparse_header_t *) data;
	data += sparse_header->file_hdr_sz;
	if(sparse_header->file_hdr_sz > sizeof(sparse_header_t))
	{
		/* Skip the remaining bytes in a header that is longer than
		 * we expected.
		 */
		data += (sparse_header->file_hdr_sz - sizeof(sparse_header_t));
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
		/* Read and skip over chunk header */
		chunk_header = (chunk_header_t *) data;
		data += sizeof(chunk_header_t);

		dprintf (SPEW, "=== Chunk Header ===\n");
		dprintf (SPEW, "chunk_type: 0x%x\n", chunk_header->chunk_type);
		dprintf (SPEW, "chunk_data_sz: 0x%x\n", chunk_header->chunk_sz);
		dprintf (SPEW, "total_size: 0x%x\n", chunk_header->total_sz);

		if(sparse_header->chunk_hdr_sz > sizeof(chunk_header_t))
		{
			/* Skip the remaining bytes in a header that is longer than
			 * we expected.
			 */
			data += (sparse_header->chunk_hdr_sz - sizeof(chunk_header_t));
		}

		chunk_data_sz = sparse_header->blk_sz * chunk_header->chunk_sz;
		switch (chunk_header->chunk_type)
		{
			case CHUNK_TYPE_RAW:
			if(chunk_header->total_sz != (sparse_header->chunk_hdr_sz +
											chunk_data_sz))
			{
				fastboot_fail("Bogus chunk size for chunk type Raw");
				return;
			}

			if(mmc_write(ptn + ((uint64_t)total_blocks*sparse_header->blk_sz),
						chunk_data_sz,
						(unsigned int*)data))
			{
				fastboot_fail("flash write failure");
				return;
			}
			total_blocks += chunk_header->chunk_sz;
			data += chunk_data_sz;
			break;

			case CHUNK_TYPE_DONT_CARE:
			total_blocks += chunk_header->chunk_sz;
			break;

			case CHUNK_TYPE_CRC:
			if(chunk_header->total_sz != sparse_header->chunk_hdr_sz)
			{
				fastboot_fail("Bogus chunk size for chunk type Dont Care");
				return;
			}
			total_blocks += chunk_header->chunk_sz;
			data += chunk_data_sz;
			break;

			default:
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
	/* 8 Byte Magic + 2048 Byte xml + Encrypted Data */
	unsigned int *magic_number = (unsigned int *) data;

#ifdef SSD_ENABLE
	int              ret=0;
	uint32           major_version=0;
	uint32           minor_version=0;

	ret = scm_svc_version(&major_version,&minor_version);
	if(!ret)
	{
		if(major_version >= 2)
		{
			if( !strcmp(arg,"ssd") || !strcmp(arg,"tqs") )
			{
				ret = encrypt_scm((uint32 **) &data, &sz);
				if (ret != 0) {
					dprintf(CRITICAL, "ERROR: Encryption Failure\n");
					return;
				}

				ret = scm_protect_keystore((uint32 *) data, sz);
				if (ret != 0) {
					dprintf(CRITICAL, "ERROR: scm_protect_keystore Failed\n");
					return;
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

	sparse_header = (sparse_header_t *) data;
	if (sparse_header->magic != SPARSE_HEADER_MAGIC)
		cmd_flash_mmc_img(arg, data, sz);
	else
		cmd_flash_mmc_sparse_img(arg, data, sz);
	return;
}

void cmd_flash(const char *arg, void *data, unsigned sz)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned extra = 0;

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
		if (memcmp((void *)data, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
			fastboot_fail("image is not a boot image");
			return;
		}
	}

	if (!strcmp(ptn->name, "system")
		|| !strcmp(ptn->name, "userdata")
		|| !strcmp(ptn->name, "persist")
		|| !strcmp(ptn->name, "recoveryfs")) {
		if (flash_ecc_bch_enabled())
			/* Spare data bytes for 8 bit ECC increased by 4 */
			extra = ((page_size >> 9) * 20);
		else
			extra = ((page_size >> 9) * 16);
	} else
		sz = ROUND_TO_PAGE(sz, page_mask);

	dprintf(INFO, "writing %d bytes to '%s'\n", sz, ptn->name);
	if (flash_write(ptn, extra, data, sz)) {
		fastboot_fail("flash write failure");
		return;
	}
	dprintf(INFO, "partition '%s' updated\n", ptn->name);
	fastboot_okay("");
}

void cmd_continue(const char *arg, void *data, unsigned sz)
{
	fastboot_okay("");
	udc_stop();
	if (target_is_emmc_boot())
	{
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

void cmd_oem_unlock(const char *arg, void *data, unsigned sz)
{
	if(!device.is_unlocked)
	{
		device.is_unlocked = 1;
		write_device_info(&device);
	}
	fastboot_okay("");
}

void cmd_oem_devinfo(const char *arg, void *data, unsigned sz)
{
	char response[64];
	snprintf(response, 64, "\tDevice tampered: %s", (device.is_tampered ? "true" : "false"));
	fastboot_info(response);
	snprintf(response, 64, "\tDevice unlocked: %s", (device.is_unlocked ? "true" : "false"));
	fastboot_info(response);
	fastboot_okay("");
}

void cmd_preflash(const char *arg, void *data, unsigned sz)
{
	fastboot_okay("");
}

void splash_screen ()
{
	struct ptentry *ptn;
	struct ptable *ptable;
	struct fbcon_config *fb_display = NULL;

	if (!target_is_emmc_boot())
	{
		ptable = flash_get_ptable();
		if (ptable == NULL) {
			dprintf(CRITICAL, "ERROR: Partition table not found\n");
			return;
		}

		ptn = ptable_find(ptable, "splash");
		if (ptn == NULL) {
			dprintf(CRITICAL, "ERROR: No splash partition found\n");
		} else {
			fb_display = fbcon_display();
			if (fb_display) {
				if (flash_read(ptn, 0, fb_display->base,
					(fb_display->width * fb_display->height * fb_display->bpp/8))) {
					fbcon_clear();
					dprintf(CRITICAL, "ERROR: Cannot read splash image\n");
				}
			}
		}
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
	uint8_t i;

	for (i = 0; i < num_parts; i++) {
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

void aboot_init(const struct app_descriptor *app)
{
	unsigned reboot_mode = 0;
	unsigned usb_init = 0;
	unsigned sz = 0;

	/* Setup page size information for nand/emmc reads */
	if (target_is_emmc_boot())
	{
		page_size = 2048;
		page_mask = page_size - 1;
	}
	else
	{
		page_size = flash_page_size();
		page_mask = page_size - 1;
	}

	if(target_use_signed_kernel())
	{
		read_device_info(&device);

	}

	target_serialno((unsigned char *) sn_buf);
	dprintf(SPEW,"serial number: %s\n",sn_buf);
	surf_udc_device.serialno = sn_buf;

	/* Check if we should do something other than booting up */
	if (keys_get_state(KEY_HOME) != 0)
		boot_into_recovery = 1;
	if (keys_get_state(KEY_VOLUMEUP) != 0)
		boot_into_recovery = 1;
	if(!boot_into_recovery)
	{
		if (keys_get_state(KEY_BACK) != 0)
			goto fastboot;
		if (keys_get_state(KEY_VOLUMEDOWN) != 0)
			goto fastboot;
	}

	#if NO_KEYPAD_DRIVER
	if (fastboot_trigger())
		goto fastboot;
	#endif

	reboot_mode = check_reboot_mode();
	if (reboot_mode == RECOVERY_MODE) {
		boot_into_recovery = 1;
	} else if(reboot_mode == FASTBOOT_MODE) {
		goto fastboot;
	}

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

fastboot:

	sz = target_get_max_flash_size();

	target_fastboot_init();

	if(!usb_init)
		udc_init(&surf_udc_device);

	fastboot_register("boot", cmd_boot);

	if (target_is_emmc_boot())
	{
		fastboot_register("flash:", cmd_flash_mmc);
		fastboot_register("erase:", cmd_erase_mmc);
	}
	else
	{
		fastboot_register("flash:", cmd_flash);
		fastboot_register("erase:", cmd_erase);
	}

	fastboot_register("continue", cmd_continue);
	fastboot_register("reboot", cmd_reboot);
	fastboot_register("reboot-bootloader", cmd_reboot_bootloader);
	fastboot_register("oem unlock", cmd_oem_unlock);
	fastboot_register("oem device-info", cmd_oem_devinfo);
	fastboot_register("preflash", cmd_preflash);
	fastboot_publish("product", TARGET(BOARD));
	fastboot_publish("kernel", "lk");
	fastboot_publish("serialno", sn_buf);
	publish_getvar_partition_info(part_info, ARRAY_SIZE(part_info));
	/* Max download size supported */
	snprintf(max_download_size, MAX_RSP_SIZE, "\t0x%x", sz);
	fastboot_publish("max-download-size", (const char *) max_download_size);
	partition_dump();
	fastboot_init(target_get_scratch_address(), sz);
	udc_start();
}

APP_START(aboot)
	.init = aboot_init,
APP_END
