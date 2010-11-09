/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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

struct smem_proc_comm
{
	unsigned command;
	unsigned status;
	unsigned data1;
	unsigned data2;
};

struct smem_heap_info
{
	unsigned initialized;
	unsigned free_offset;
	unsigned heap_remaining;
	unsigned reserved;
};

struct smem_alloc_info
{
	unsigned allocated;
	unsigned offset;
	unsigned size;
	unsigned reserved;
};

struct smem {
	struct smem_proc_comm		proc_comm[4];
	unsigned			version_info[32];
	struct smem_heap_info		heap_info;
	struct smem_alloc_info		alloc_info[128];
};

struct smem_board_info_v3
{
    unsigned format;
    unsigned msm_id;
    unsigned msm_version;
    char     build_id[32];
    unsigned raw_msm_id;
    unsigned raw_msm_version;
    unsigned hw_platform;
};

struct smem_board_info_v4
{
    struct smem_board_info_v3 board_info_v3;
    unsigned platform_version;
    unsigned buffer_align; //Need for 8 bytes alignment while reading from shared memory.
};

struct smem_board_info_v5
{
    struct smem_board_info_v3 board_info_v3;
    unsigned platform_version;
    unsigned fused_chip;
};

/* chip information */
enum {
    UNKNOWN = 0,
    MDM9200 = 57,
    MDM9600 = 58,
};

enum platform
{
    HW_PLATFORM_UNKNOWN = 0,
    HW_PLATFORM_SURF    = 1,
    HW_PLATFORM_FFA     = 2,
    HW_PLATFORM_FLUID   = 3,
    HW_PLATFORM_SVLTE   = 4,
    HW_PLATFORM_32BITS  = 0x7FFFFFFF
};


typedef enum {
	SMEM_SPINLOCK_ARRAY = 7,

	SMEM_AARM_PARTITION_TABLE = 9,

	SMEM_APPS_BOOT_MODE = 106,

	SMEM_BOARD_INFO_LOCATION = 137,

	SMEM_USABLE_RAM_PARTITION_TABLE = 402,

	SMEM_POWER_ON_STATUS_INFO = 403,

	SMEM_FIRST_VALID_TYPE = SMEM_SPINLOCK_ARRAY,
	SMEM_LAST_VALID_TYPE = SMEM_POWER_ON_STATUS_INFO,
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
};

enum {
    DEFAULT_DOMAIN = 0x0,
    APPS_DOMAIN,
    MODEM_DOMAIN,
    SHARED_DOMAIN,
};

enum {
	SYS_MEMORY = 1,        /* system memory*/
	BOOT_REGION_MEMORY1,   /* boot loader memory 1*/
	BOOT_REGION_MEMORY2,   /* boot loader memory 2,reserved*/
	APPSBL_MEMORY,         /* apps boot loader memory*/
	APPS_MEMORY,           /* apps  usage memory*/
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
#define PWR_ON_EVENT_USB_CHG 0x20

#endif /* __PLATFORM_MSM_SHARED_SMEM_H */
