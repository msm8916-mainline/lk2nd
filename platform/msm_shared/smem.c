/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
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

#include <debug.h>
#include <reg.h>
#include <string.h>
#include <sys/types.h>
#include <platform.h>
#include <platform/iomap.h>
#include <board.h>
#include <arch/defines.h>

#include "smem.h"

static struct smem *smem;

const char *hw_platform[] = {
	[HW_PLATFORM_UNKNOWN] = "Unknown",
	[HW_PLATFORM_SURF] = "Surf",
	[HW_PLATFORM_FFA] = "FFA",
	[HW_PLATFORM_FLUID] = "Fluid",
	[HW_PLATFORM_SVLTE] = "SVLTE",
	[HW_PLATFORM_MTP_MDM] = "MDM_MTP_NO_DISPLAY",
	[HW_PLATFORM_MTP] = "MTP",
	[HW_PLATFORM_RCM] = "RCM",
	[HW_PLATFORM_LIQUID] = "Liquid",
	[HW_PLATFORM_DRAGON] = "Dragon",
	[HW_PLATFORM_QRD] = "QRD",
	[HW_PLATFORM_HRD] = "HRD",
	[HW_PLATFORM_DTV] = "DTV",
	[HW_PLATFORM_STP] = "STP",
	[HW_PLATFORM_SBC] = "SBC",
};

/* DYNAMIC SMEM REGION feature enables LK to dynamically
 * read the SMEM addr info from TCSR register or IMEM location.
 * The first word read, if indicates a MAGIC number, then
 * Dynamic SMEM is assumed to be enabled. Read the remaining
 * SMEM info for SMEM Size and Phy_addr from the other bytes.
 */

#if DYNAMIC_SMEM
uint32_t smem_get_base_addr()
{
	struct smem_addr_info *smem_info = NULL;

	smem_info = (struct smem_addr_info *) SMEM_TARG_INFO_ADDR;
	if(smem_info && (smem_info->identifier == SMEM_TARGET_INFO_IDENTIFIER))
		return smem_info->phy_addr;
	else
		return MSM_SHARED_BASE;
}
#endif

/* buf MUST be 4byte aligned, and len MUST be a multiple of 8. */
unsigned smem_read_alloc_entry(smem_mem_type_t type, void *buf, int len)
{
	struct smem_alloc_info *ainfo;
	unsigned *dest = buf;
	unsigned src;
	unsigned size;
	uint32_t smem_addr = 0;

#if DYNAMIC_SMEM
	smem_addr = smem_get_base_addr();
#else
	smem_addr = platform_get_smem_base_addr();
#endif

	smem = (struct smem *)smem_addr;

	if (((len & 0x3) != 0) || (((unsigned)buf & 0x3) != 0))
		return 1;

	if (type < SMEM_FIRST_VALID_TYPE || type > SMEM_LAST_VALID_TYPE)
		return 1;

	/* TODO: Use smem spinlocks */
	ainfo = &smem->alloc_info[type];
	if (readl(&ainfo->allocated) == 0)
		return 1;

	size = readl(&ainfo->size);

	if (size < (unsigned)((len + 7) & ~0x00000007))
		return 1;

	src = smem_addr + readl(&ainfo->offset);
	for (; len > 0; src += 4, len -= 4)
		*(dest++) = readl(src);

	return 0;
}

/* Return a pointer to smem_item with size */
void* smem_get_alloc_entry(smem_mem_type_t type, uint32_t* size)
{
	struct smem_alloc_info *ainfo = NULL;
	uint32_t smem_addr = 0;
	uint32_t base_ext = 0;
	uint32_t offset = 0;
	void *ret = 0;

#if DYNAMIC_SMEM
	smem_addr = smem_get_base_addr();
#else
	smem_addr = platform_get_smem_base_addr();
#endif
	smem = (struct smem *)smem_addr;

	if (type < SMEM_FIRST_VALID_TYPE || type > SMEM_LAST_VALID_TYPE)
		return 1;

	ainfo = &smem->alloc_info[type];
	if (readl(&ainfo->allocated) == 0)
		return 1;

	*size = readl(&ainfo->size);
	base_ext = readl(&ainfo->base_ext);
	offset = readl(&ainfo->offset);

	if(base_ext)
	{
		ret = base_ext + offset;
	}
	else
	{
		ret = (void*) smem_addr + offset;
	}

	return ret;
}

unsigned
smem_read_alloc_entry_offset(smem_mem_type_t type, void *buf, int len,
			     int offset)
{
	struct smem_alloc_info *ainfo;
	unsigned *dest = buf;
	unsigned src;
	unsigned size = len;
	uint32_t smem_addr = 0;

#if DYNAMIC_SMEM
	smem_addr = smem_get_base_addr();
#else
	smem_addr = platform_get_smem_base_addr();
#endif

	smem = (struct smem *)smem_addr;

	if (((len & 0x3) != 0) || (((unsigned)buf & 0x3) != 0))
		return 1;

	if (type < SMEM_FIRST_VALID_TYPE || type > SMEM_LAST_VALID_TYPE)
		return 1;

	ainfo = &smem->alloc_info[type];
	if (readl(&ainfo->allocated) == 0)
		return 1;

	src = smem_addr + readl(&ainfo->offset) + offset;
	for (; size > 0; src += 4, size -= 4)
		*(dest++) = readl(src);

	return 0;
}

/* buf MUST be 4byte aligned, and len MUST be a multiple of 8. */
unsigned smem_alloc_write_entry(smem_mem_type_t type, void *buf, unsigned size)
{
	struct smem_alloc_info *ainfo;
	uint32_t *src = buf, *src_end = (uint32_t*)((uint32_t)buf + size);
	uint32_t *dest;
	unsigned remaining, offset;
	uint32_t smem_addr = 0;

#if DYNAMIC_SMEM
	smem_addr = smem_get_base_addr();
#else
	smem_addr = platform_get_smem_base_addr();
#endif

	smem = (struct smem *)smem_addr;

	if (((size % 8) != 0) || (((unsigned)buf & 0x3) != 0))
		return 22;

	if (type < SMEM_FIRST_VALID_TYPE || type > SMEM_LAST_VALID_TYPE)
		return 22;

	/*
	 * FIXME: We should really use the hwspinlock here, but:
	 *   - I'm lazy.
	 *   - LK is annoying to work with, U-Boot would be nicer.
	 *   - The only remote processor that is already running is the RPM.
	 *     We don't communicate with it from LK so maybe we can get away
	 *     without any locking.
	 *   - YOLO.
	 */
	ainfo = &smem->alloc_info[type];
	if (readl(&ainfo->allocated)) {
		dprintf(CRITICAL, "SMEM entry %d is already allocated\n", type);
		return 1;
	}

	remaining = readl(&smem->heap_info.heap_remaining);
	if (size > remaining) {
		dprintf(CRITICAL, "Not enough space in SMEM for entry %d. size: %u, remaining: %u\n",
			type, size, remaining);
		return 12;
	}

	/* Allocate entry in SMEM */
	offset = readl(&smem->heap_info.free_offset);
	writel(offset, &ainfo->offset);
	writel(size, &ainfo->size);

	dsb();
	writel(1, &ainfo->allocated);

	writel(offset + size, &smem->heap_info.free_offset);
	writel(remaining - size, &smem->heap_info.heap_remaining);
	dsb();

	/* Write data to SMEM */
	for (dest = (uint32_t*)(smem_addr + offset); src < src_end; ++src, ++dest)
		writel(*src, dest);

	return 0;
}

size_t smem_get_hw_platform_name(void *buf, uint32_t buf_size)
{
	uint32_t hw_id;

	if (buf == NULL) {
		dprintf(CRITICAL, "ERROR: buf is NULL\n");
		return 1;
	}

	hw_id = board_hardware_id();
	if (hw_id >= ARRAY_SIZE(hw_platform))
		hw_id = HW_PLATFORM_UNKNOWN;

	if (buf_size < strlen(hw_platform[hw_id]) + 1)
		return 1;

	return snprintf(buf, strlen(hw_platform[hw_id]) + 1,
		"%s\n", hw_platform[hw_id]);
}
