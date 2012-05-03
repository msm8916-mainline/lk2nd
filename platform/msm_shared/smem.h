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
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
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

#ifndef __PLATFORM_MSM_SHARED_SMEM_H
#define __PLATFORM_MSM_SHARED_SMEM_H

#include <sys/types.h>

struct smem_proc_comm {
	unsigned command;
	unsigned status;
	unsigned data1;
	unsigned data2;
};

struct smem_heap_info {
	unsigned initialized;
	unsigned free_offset;
	unsigned heap_remaining;
	unsigned reserved;
};

struct smem_alloc_info {
	unsigned allocated;
	unsigned offset;
	unsigned size;
	unsigned reserved;
};

struct smem_board_info_v2 {
	unsigned format;
	unsigned msm_id;
	unsigned msm_version;
	char build_id[32];
	unsigned raw_msm_id;
	unsigned raw_msm_version;
};

struct smem_board_info_v3 {
	unsigned format;
	unsigned msm_id;
	unsigned msm_version;
	char build_id[32];
	unsigned raw_msm_id;
	unsigned raw_msm_version;
	unsigned hw_platform;
};

struct smem_board_info_v4 {
	struct smem_board_info_v3 board_info_v3;
	unsigned platform_version;
	unsigned buffer_align;	//Need for 8 bytes alignment while reading from shared memory.
};

struct smem_board_info_v5 {
	struct smem_board_info_v3 board_info_v3;
	unsigned platform_version;
	unsigned fused_chip;
};

struct smem_board_info_v6 {
	struct smem_board_info_v3 board_info_v3;
	unsigned platform_version;
	unsigned fused_chip;
	unsigned platform_subtype;
	unsigned buffer_align;	//Need for 8 bytes alignment while reading from shared memory.
};

typedef struct {
	unsigned key_len;
	unsigned iv_len;
	unsigned char key[32];
	unsigned char iv[32];
} boot_symmetric_key_info;

typedef struct {
	unsigned int update_status;
	unsigned int bl_error_code;
} boot_ssd_status;

#if PLATFORM_MSM7X30

typedef struct {
	boot_symmetric_key_info key_info;
	uint32_t boot_flags;
	uint32_t boot_key_press[5];
	uint32_t time_tick;
	boot_ssd_status status;
	uint8_t buff_align[4];
} boot_info_for_apps;

#elif PLATFORM_MSM7K

typedef struct {
	uint32_t apps_img_start_addr;
	uint32_t boot_flags;
	boot_ssd_status status;
} boot_info_for_apps;

#elif PLATFORM_MSM7X27A

typedef struct {
	uint32_t apps_img_start_addr;
	uint32_t boot_flags;
	boot_ssd_status status;
	boot_symmetric_key_info key_info;
	uint16_t boot_key_press[10];
	uint32_t timetick;
	uint8_t PAD[28];
} boot_info_for_apps;

#else

/* Dummy structure to keep it for other targets */
typedef struct {
	uint32_t boot_flags;
	boot_ssd_status status;
} boot_info_for_apps;

#endif

/* chip information */
enum {
	UNKNOWN = 0,
	MDM9200 = 57,
	MDM9600 = 58,
	MSM8260 = 70,
	MSM8660 = 71,
	APQ8060 = 86,
	MSM8960 = 87,
	MSM7225A = 88,
	MSM7625A = 89,
	MSM7227A = 90,
	MSM7627A = 91,
	ESM7227A = 92,
	ESM7225A = 96,
	ESM7627A = 97,
	MSM7225AA = 98,
	MSM7625AA = 99,
	ESM7225AA = 100,
	MSM7227AA = 101,
	MSM7627AA = 102,
	ESM7227AA = 103,
	APQ8064 = 109,
	MSM8930 = 116,
	MSM8630 = 117,
	MSM8230 = 118,
	APQ8030 = 119,
	MSM8627 = 120,
	MSM8227 = 121,
	MSM8660A = 122,
	MSM8260A = 123,
	APQ8060A = 124,
	MSM8225 = 127,
	MSM8625 = 129,
	MPQ8064 = 130,
	MSM7225AB = 131,
	MSM7625AB = 132,
	ESM7225AB = 133,
};

enum platform {
	HW_PLATFORM_UNKNOWN = 0,
	HW_PLATFORM_SURF = 1,
	HW_PLATFORM_FFA = 2,
	HW_PLATFORM_FLUID = 3,
	HW_PLATFORM_SVLTE = 4,
	HW_PLATFORM_QT = 6,
	HW_PLATFORM_MTP = 8,
	HW_PLATFORM_LIQUID = 9,
	HW_PLATFORM_DRAGON = 10,
	HW_PLATFORM_HRD = 13,
	HW_PLATFORM_DTV = 14,
	HW_PLATFORM_32BITS = 0x7FFFFFFF
};

enum platform_subtype {
	HW_PLATFORM_SUBTYPE_UNKNOWN = 0,
	HW_PLATFORM_SUBTYPE_MDM = 1,
	HW_PLATFORM_SUBTYPE_CSFB = 1,
	HW_PLATFORM_SUBTYPE_SVLTE1 = 2,
	HW_PLATFORM_SUBTYPE_SVLTE2A = 3,
	HW_PLATFORM_SUBTYPE_SGLTE = 6,
	HW_PLATFORM_SUBTYPE_32BITS = 0x7FFFFFFF
};

typedef enum {
	SMEM_SPINLOCK_ARRAY = 7,

	SMEM_AARM_PARTITION_TABLE = 9,

	SMEM_APPS_BOOT_MODE = 106,

	SMEM_BOARD_INFO_LOCATION = 137,

	SMEM_USABLE_RAM_PARTITION_TABLE = 402,

	SMEM_POWER_ON_STATUS_INFO = 403,

	SMEM_RLOCK_AREA = 404,

	SMEM_BOOT_INFO_FOR_APPS = 418,

	SMEM_FIRST_VALID_TYPE = SMEM_SPINLOCK_ARRAY,
	SMEM_LAST_VALID_TYPE = SMEM_BOOT_INFO_FOR_APPS,

	SMEM_MAX_SIZE = SMEM_BOOT_INFO_FOR_APPS + 1,
} smem_mem_type_t;

/* Note: buf MUST be 4byte aligned, and max_len MUST be a multiple of 4. */
unsigned smem_read_alloc_entry(smem_mem_type_t type, void *buf, int max_len);

/* SMEM RAM Partition */
enum {
	DEFAULT_ATTRB = ~0x0,
	READ_ONLY = 0x0,
	READWRITE,
};

enum {
	DEFAULT_CATEGORY = ~0x0,
	SMI = 0x0,
	EBI1,
	EBI2,
	QDSP6,
	IRAM,
	IMEM,
	EBI0_CS0,
	EBI0_CS1,
	EBI1_CS0,
	EBI1_CS1,
	SDRAM = 0xE,
};

enum {
	DEFAULT_DOMAIN = 0x0,
	APPS_DOMAIN,
	MODEM_DOMAIN,
	SHARED_DOMAIN,
};

enum {
	SYS_MEMORY = 1,		/* system memory */
	BOOT_REGION_MEMORY1,	/* boot loader memory 1 */
	BOOT_REGION_MEMORY2,	/* boot loader memory 2,reserved */
	APPSBL_MEMORY,		/* apps boot loader memory */
	APPS_MEMORY,		/* apps  usage memory */
};

struct smem {
	struct smem_proc_comm proc_comm[4];
	unsigned version_info[32];
	struct smem_heap_info heap_info;
	struct smem_alloc_info alloc_info[SMEM_MAX_SIZE];
};

struct smem_ram_ptn {
	char name[16];
	unsigned start;
	unsigned size;

	/* RAM Partition attribute: READ_ONLY, READWRITE etc.  */
	unsigned attr;

	/* RAM Partition category: EBI0, EBI1, IRAM, IMEM */
	unsigned category;

	/* RAM Partition domain: APPS, MODEM, APPS & MODEM (SHARED) etc. */
	unsigned domain;

	/* RAM Partition type: system, bootloader, appsboot, apps etc. */
	unsigned type;

	/* reserved for future expansion without changing version number */
	unsigned reserved2, reserved3, reserved4, reserved5;
} __attribute__ ((__packed__));

struct smem_ram_ptable {
#define _SMEM_RAM_PTABLE_MAGIC_1 0x9DA5E0A8
#define _SMEM_RAM_PTABLE_MAGIC_2 0xAF9EC4E2
	unsigned magic[2];
	unsigned version;
	unsigned reserved1;
	unsigned len;
	struct smem_ram_ptn parts[32];
	unsigned buf;
} __attribute__ ((__packed__));

/* Power on reason/status info */
#define PWR_ON_EVENT_RTC_ALARM 0x2
#define PWR_ON_EVENT_USB_CHG   0x20
#define PWR_ON_EVENT_WALL_CHG  0x40

unsigned smem_read_alloc_entry_offset(smem_mem_type_t type, void *buf, int len,
				      int offset);
int smem_ram_ptable_init(struct smem_ram_ptable *smem_ram_ptable);

#endif				/* __PLATFORM_MSM_SHARED_SMEM_H */
