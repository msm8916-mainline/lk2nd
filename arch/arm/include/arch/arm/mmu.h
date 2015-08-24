/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __ARCH_ARM_MMU_H
#define __ARCH_ARM_MMU_H

#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

void arm_mmu_init(void);

#if defined(ARM_ISA_ARMV6) | defined(ARM_ISA_ARMV7)

#ifndef LPAE
/* C, B and TEX[2:0] encodings without TEX remap */
                                                       /* TEX      |    CB    */
#define MMU_MEMORY_TYPE_STRONGLY_ORDERED              ((0x0 << 12) | (0x0 << 2))
#define MMU_MEMORY_TYPE_DEVICE_SHARED                 ((0x0 << 12) | (0x1 << 2))
#define MMU_MEMORY_TYPE_DEVICE_NON_SHARED             ((0x2 << 12) | (0x0 << 2))
#define MMU_MEMORY_TYPE_NORMAL                        ((0x1 << 12) | (0x0 << 2))
#define MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH          ((0x0 << 12) | (0x2 << 2))
#define MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_NO_ALLOCATE ((0x0 << 12) | (0x3 << 2))
#define MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_ALLOCATE    ((0x1 << 12) | (0x3 << 2))

#define MMU_MEMORY_AP_NO_ACCESS     (0x0 << 10)
#define MMU_MEMORY_AP_READ_ONLY     (0x7 << 10)
#define MMU_MEMORY_AP_READ_WRITE    (0x3 << 10)

#define MMU_MEMORY_XN               (0x1 << 4)
#else /* LPAE */

typedef enum
{
 /* Secure L1 section */
  MMU_L1_SECTION_MAPPING = 0,

  /* Non-secure L1 section */
  MMU_L1_NS_SECTION_MAPPING,

  /* Secure L2 section */
  MMU_L2_SECTION_MAPPING,

  /* Non-secure L2 section */
  MMU_L2_NS_SECTION_MAPPING,

  /* Secure L3 section */
  MMU_L3_SECTION_MAPPING,

  /* Non-secure L3 section */
  MMU_L3_NS_SECTION_MAPPING,

}mapping_type;


#define ATTR_INDEX(x)         (x << 2) /* [4:2] - AttrIndx[2:0] of block descriptor */

/* define the memory attributes for the block descriptors */

#define MMU_MEMORY_TYPE_STRONGLY_ORDERED               ATTR_INDEX(0)
#define MMU_MEMORY_TYPE_DEVICE_SHARED                  ATTR_INDEX(1) | (3 << 8) /*[9:8] - SH[1:0] - Shareability */
#define MMU_MEMORY_TYPE_NORMAL                         ATTR_INDEX(2)
#define MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH           ATTR_INDEX(7)
#define MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_ALLOCATE     ATTR_INDEX(4)
#define MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_NO_ALLOCATE  ATTR_INDEX(5)

#define MMU_MEMORY_AP_READ_WRITE                       (1 << 6) /* Read/Write at any priveledge */
#define MMU_MEMORY_AP_READ_ONLY                        (0x3 << 6) /* Read only priveledge */
#define MMU_MEMORY_XN                                  (1ULL << 54)
#define MMU_MEMORY_PXN                                 (1ULL << 53)

/* define the memory attributes:
 * For LPAE, the block descriptor contains index into the MAIR registers.
 * MAIR registers define the memory attributes. Below configuration is arrived based on
 * the arm v7 manual section B4.1.104
 */
/* MAIR is 64 bit, with attrm[0..7], where m is 0..7
 * Fill attrm[0..7] as below
 * STRONG ORDERED                                        0000.0000
 * DEVICE MEMORY                                         0000.0100
 * normal memory, non cacheable                          0100.0100
 * normal memory, WB, RA, nWA                            1110.1110
 * normal memory, WB, RA, WA                             1111.1111
 * normal memory, WB, nRA, nWA                           1100.1100
 * normal memory, WT, RA, nWA                            1010.1010
 * normal memory , WT, RA, WA                            1011.1011
 */

#define MAIR0                  0xee440400
#define MAIR1                  0xbbaaccff
#include <mmu.h>
void arm_mmu_map_entry(mmu_section_t *entry);
#endif /* LPAE */

#else

#error "MMU implementation needs to be updated for this ARM architecture"

#endif

void arm_mmu_map_section(addr_t paddr, addr_t vaddr, uint flags);
uint64_t virtual_to_physical_mapping(uint32_t vaddr);
uint32_t physical_to_virtual_mapping(uint64_t paddr);

#if defined(__cplusplus)
}
#endif

#endif
