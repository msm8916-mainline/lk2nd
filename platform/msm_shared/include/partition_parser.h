/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

/* Lookup Type */
//TODO: Remove when merging MBR code to mmc_parser
#define PTN_OFFSET 0x01
#define PTN_SIZE   0x02

/* GPT Signature should be 0x5452415020494645 */
#define GPT_SIGNATURE_1 0x54524150
#define GPT_SIGNATURE_2 0x20494645

/* GPT Offsets */
#define PROTECTIVE_MBR_SIZE       512
#define PARTITION_TABLE_SIZE      512
#define PARTITION_ENTRY_SIZE      512
#define HEADER_SIZE_OFFSET        12
#define HEADER_CRC_OFFSET         16
#define FIRST_USABLE_LBA_OFFSET   40
#define LAST_USABLE_LBA_OFFSET    48
#define PARTITION_ENTRIES_OFFSET  72
#define PARTITION_COUNT_OFFSET    80
#define PENTRY_SIZE_OFFSET        84
#define PARTITION_CRC_OFFSET      88

#define UNIQUE_GUID_OFFSET        16
#define FIRST_LBA_OFFSET          32
#define LAST_LBA_OFFSET           40
#define ATTRIBUTE_FLAG_OFFSET     48
#define PARTITION_NAME_OFFSET     56

#define MAX_GPT_NAME_SIZE          72
#define PARTITION_TYPE_GUID_SIZE   16
#define UNIQUE_PARTITION_GUID_SIZE 16
#define NUM_GPT_PARTITIONS         32

#define GET_LLWORD_FROM_BYTE(x)    ((unsigned long long)*(x) | \
        ((unsigned long long)*(x+1) << 8) | \
        ((unsigned long long)*(x+2) << 16) | \
        ((unsigned long long)*(x+3) << 24) | \
        ((unsigned long long)*(x+4) << 32) | \
        ((unsigned long long)*(x+5) << 40) | \
        ((unsigned long long)*(x+6) << 48) | \
        ((unsigned long long)*(x+7) << 56))

struct gpt_entry
{
  unsigned char partition_type_guid[PARTITION_TYPE_GUID_SIZE];
  unsigned char unique_partition_guid[UNIQUE_PARTITION_GUID_SIZE];
  unsigned long long first_lba;
  unsigned long long last_lba;
  unsigned long long attribute_flag;
  unsigned char partition_name[MAX_GPT_NAME_SIZE];
};

unsigned int mmc_boot_read_gpt(struct mmc_boot_host * mmc_host,
                               struct mmc_boot_card * mmc_card);
unsigned long long gpt_lookup(unsigned char * name, unsigned type);
