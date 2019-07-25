/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Copyright (c) 2014,2016,2019 The Linux Foundation. All rights reserved.
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

#ifndef _BOOT_IMAGE_H_
#define _BOOT_IMAGE_H_

#define BOOT_MAGIC "ANDROID!"
#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE  16
#define BOOT_ARGS_SIZE  512
#define BOOT_IMG_MAX_PAGE_SIZE 4096
#define BOOT_EXTRA_ARGS_SIZE 1024

#define BOOT_HEADER_VERSION_ZERO 0
/* Struct def for boot image header
  * Bootloader expects the structure of boot_img_hdr with header version
  * BOOT_HEADER_VERSION_ZERO to be as follows:
  */
struct boot_img_hdr_v0 {
    unsigned char magic[BOOT_MAGIC_SIZE];

    unsigned kernel_size;  /* size in bytes */
    unsigned kernel_addr;  /* physical load addr */

    unsigned ramdisk_size; /* size in bytes */
    unsigned ramdisk_addr; /* physical load addr */

    unsigned second_size;  /* size in bytes */
    unsigned second_addr;  /* physical load addr */

    unsigned tags_addr;    /* physical addr for kernel tags */
    unsigned page_size;    /* flash page size we assume */
#if OSVERSION_IN_BOOTIMAGE
    uint32_t header_version; /* version for the boot image header */
    uint32_t os_version; /* version << 11 | patch_level */
#else
    unsigned dt_size;      /* device_tree in bytes */
    unsigned unused;    /* future expansion: should be 0 */
#endif
    unsigned char name[BOOT_NAME_SIZE]; /* asciiz product name */
    
    unsigned char cmdline[BOOT_ARGS_SIZE];

    unsigned id[8]; /* timestamp / checksum / sha1 / etc */

    /* Supplemental command line data; kept here to maintain
        * binary compatibility with older versions of mkbootimg
        */
    uint8_t extra_cmdline[BOOT_EXTRA_ARGS_SIZE];
} __attribute__((packed));

/*
 * It is expected that callers would explicitly specify which version of the
 * boot image header they need to use.
 */
typedef struct boot_img_hdr_v0 boot_img_hdr;

/**
 * Offset of recovery DTBO length in a boot image header of version V1 or
 * above.
  */
#define BOOT_IMAGE_HEADER_V1_RECOVERY_DTBO_SIZE_OFFSET sizeof (boot_img_hdr)

/*
** +-----------------+ 
** | boot header     | 1 page
** +-----------------+
** | kernel          | n pages  
** +-----------------+
** | ramdisk         | m pages  
** +-----------------+
** | second stage    | o pages
** +-----------------+
** n = (kernel_size + page_size - 1) / page_size
** m = (ramdisk_size + page_size - 1) / page_size
** o = (second_size + page_size - 1) / page_size
** 0. all entities are page_size aligned in flash
** 1. kernel and ramdisk are required (size != 0)
** 2. second is optional (second_size == 0 -> no second)
** 3. load each element (kernel, ramdisk, second) at
**    the specified physical address (kernel_addr, etc)
** 4. prepare tags at tag_addr.  kernel_args[] is
**    appended to the kernel commandline in the tags.
** 5. r0 = 0, r1 = MACHINE_TYPE, r2 = tags_addr
** 6. if second_size != 0: jump to second_addr
**    else: jump to kernel_addr
*/

#define BOOT_HEADER_VERSION_ONE 1

struct boot_img_hdr_v1 {
    uint32_t recovery_dtbo_size;   /* size in bytes for recovery DTBO image */
    uint64_t recovery_dtbo_offset; /* physical load addr */
    uint32_t header_size;
} __attribute__((packed));

/* When the boot image header has a version of BOOT_HEADER_VERSION_ONE,
 * the structure of the boot image is as follows:
 *
 * +-----------------+
 * | boot header     | 1 page
 * +-----------------+
 * | kernel          | n pages
 * +-----------------+
 * | ramdisk         | m pages
 * +-----------------+
 * | second stage    | o pages
 * +-----------------+
 * | recovery dtbo   | p pages
 * +-----------------+
 * n = (kernel_size + page_size - 1) / page_size
 * m = (ramdisk_size + page_size - 1) / page_size
 * o = (second_size + page_size - 1) / page_size
 * p = (recovery_dtbo_size + page_size - 1) / page_size
 *
 * 0. all entities are page_size aligned in flash
 * 1. kernel and ramdisk are required (size != 0)
 * 2. recovery_dtbo is required for recovery.img
 *    in non-A/B devices(recovery_dtbo_size != 0)
 * 3. second is optional (second_size == 0 -> no second)
 * 4. load each element (kernel, ramdisk, second, recovery_dtbo) at
 *    the specified physical address (kernel_addr, etc)
 * 5. prepare tags at tag_addr.  kernel_args[] is
 *    appended to the kernel commandline in the tags.
 * 6. r0 = 0, r1 = MACHINE_TYPE, r2 = tags_addr
 * 7. if second_size != 0: jump to second_addr
 *    else: jump to kernel_addr
 */

#define BOOT_IMAGE_HEADER_V2_OFFSET sizeof (struct boot_img_hdr_v1)
#define BOOT_HEADER_VERSION_TWO 2

struct boot_img_hdr_v2 {
    uint32_t dtb_size; /* size in bytes for DTB image */
    uint64_t dtb_addr; /* physical load address for DTB image */
} __attribute__((packed));

/* When the boot image header has a version of BOOT_HEADER_VERSION_TWO,
 * the structure of the boot image is as follows:
 *
 * +-----------------+
 * | boot header     | 1 page
 * +-----------------+
 * | kernel          | n pages
 * +-----------------+
 * | ramdisk         | m pages
 * +-----------------+
 * | second stage    | o pages
 * +-----------------+
 * | recovery dtbo   | p pages
 * +-----------------+
 * | dtb.img         | q pages
 * +-----------------+
 * n = (kernel_size + page_size - 1) / page_size
 * m = (ramdisk_size + page_size - 1) / page_size
 * o = (second_size + page_size - 1) / page_size
 * p = (recovery_dtbo_size + page_size - 1) / page_size
 * q = (dtb_size + page_size - 1) / page_size
 *
 * 0. all entities are page_size aligned in flash
 * 1. kernel and ramdisk are required (size != 0)
 * 2. recovery_dtbo is required for recovery.img
 *    in non-A/B devices(recovery_dtbo_size != 0)
 * 3. second is optional (second_size == 0 -> no second)
 * 4. load each element (kernel, ramdisk, second, recovery_dtbo) at
 *    the specified physical address (kernel_addr, etc)
 * 5. prepare tags at tag_addr.  kernel_args[] is
 *    appended to the kernel commandline in the tags.
 * 6. r0 = 0, r1 = MACHINE_TYPE, r2 = tags_addr
 * 7. if second_size != 0: jump to second_addr
 *    else: jump to kernel_addr
 */

boot_img_hdr *mkbootimg(void *kernel, unsigned kernel_size,
                        void *ramdisk, unsigned ramdisk_size,
                        void *second, unsigned second_size,
                        unsigned page_size,
                        unsigned *bootimg_size);

void bootimg_set_cmdline(boot_img_hdr *hdr, const char *cmdline);                

#define KERNEL64_HDR_MAGIC 0x644D5241 /* ARM64 */

struct kernel64_hdr
{
    uint32_t insn;
    uint32_t res1;
    uint64_t text_offset;
    uint64_t res2;
    uint64_t res3;
    uint64_t res4;
    uint64_t res5;
    uint64_t res6;
    uint32_t magic_64;
    uint32_t res7;
};

enum
{
	ERR_NO_BOOT_PTN =1,
	ERR_NO_RECOVERY_PTN,
	ERR_MMC_READ_FAIL,
	ERR_INVALID_BOOT_MAGIC,
	ERR_INVALID_PAGE_SIZE,
	ERR_ABOOT_ADDR_OVERLAP,
	ERR_SEND_ROT_FAIL,
	ERR_DECOMPRESSION,
	ERR_DT_PARSE,
	ERR_BOOT_SIGNATURE_MISMATCH,
};
#endif
