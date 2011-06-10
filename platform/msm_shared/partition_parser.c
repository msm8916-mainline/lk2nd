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

#include <stdlib.h>
#include <string.h>
#include "mmc.h"
#include "partition_parser.h"

static struct gpt_entry gpt[NUM_GPT_PARTITIONS];
static uint32_t gpt_partition_count = 0;

/*
 * Read GPT from MMC and fill partition table
 */
uint32_t mmc_boot_read_gpt(struct mmc_boot_host * mmc_host,
                               struct mmc_boot_card * mmc_card){

    int32_t ret = MMC_BOOT_E_SUCCESS;
    uint32_t header_size = 0;
    //uint32_t header_crc = 0;
    uint64_t first_usable_lba = 0;
    uint64_t last_usable_lba = 0;
    uint32_t partition_count = 0;
    uint32_t partition_entry_size = 0;
    //uint32_t partition_array_crc;
    uint8_t data[MMC_BOOT_RD_BLOCK_LEN];
    uint32_t i = 0; /* Counter for each 512 block */
    uint32_t j = 0; /* Counter for each 128 entry in the 512 block */

    /* Print out the GPT first */
    ret = mmc_boot_read_from_card( mmc_host, mmc_card,
                                   PROTECTIVE_MBR_SIZE,
                                   MMC_BOOT_RD_BLOCK_LEN,
                                   (uint32_t *)data);

    /* Check GPT Signature */
    if( ((uint32_t *)data)[0] != GPT_SIGNATURE_2 ||
        ((uint32_t *)data)[1] != GPT_SIGNATURE_1 )
    {
        dprintf(CRITICAL,  "GPT: signature does not match.\n" );
        return MMC_BOOT_E_FAILURE;
    }

    header_size = GET_LWORD_FROM_BYTE(&data[HEADER_SIZE_OFFSET]);
    //header_crc = GET_LWORD_FROM_BYTE(&data[HEADER_CRC_OFFSET]);
    first_usable_lba = GET_LLWORD_FROM_BYTE(&data[FIRST_USABLE_LBA_OFFSET]);
    last_usable_lba = GET_LLWORD_FROM_BYTE(&data[LAST_USABLE_LBA_OFFSET]);
    partition_count = GET_LWORD_FROM_BYTE(&data[PARTITION_COUNT_OFFSET]);
    partition_entry_size = GET_LWORD_FROM_BYTE(&data[PENTRY_SIZE_OFFSET]);

    /* Read GPT Entries */
    for(i = 0; i < (partition_count/4); i++)
    {
        ret = mmc_boot_read_from_card( mmc_host, mmc_card,
                                       PROTECTIVE_MBR_SIZE + PARTITION_TABLE_SIZE +
                                       (i * MMC_BOOT_RD_BLOCK_LEN),
                                       MMC_BOOT_RD_BLOCK_LEN,
                                       (uint32_t *)data);

        if (ret)
        {
            dprintf(CRITICAL, "GPT: mmc read card failed reading partition entries.\n" );
            return ret;
        }

        for(j=0; j < 4; j++)
        {
            memcpy(&(gpt[j+(i*4)].partition_type_guid),
                    &data[(j * partition_entry_size)], PARTITION_TYPE_GUID_SIZE);

            if (gpt[j+(i*4)].partition_type_guid[0] == 0x00 &&
                gpt[j+(i*4)].partition_type_guid[1] == 0x00)
            {
                i = partition_count;
                break;
            }
            gpt_partition_count++;

            memcpy(&(gpt[j+(i*4)].unique_partition_guid),
                    &data[(j * partition_entry_size) + UNIQUE_GUID_OFFSET], UNIQUE_PARTITION_GUID_SIZE);
            gpt[j+(i*4)].first_lba = GET_LLWORD_FROM_BYTE(&data[(j * partition_entry_size) +
                                                                     FIRST_LBA_OFFSET]);
            gpt[j+(i*4)].last_lba = GET_LLWORD_FROM_BYTE(&data[(j * partition_entry_size) +
                                                                     LAST_LBA_OFFSET]);
            gpt[j+(i*4)].attribute_flag = GET_LLWORD_FROM_BYTE(&data[(j * partition_entry_size) +
                                                                     ATTRIBUTE_FLAG_OFFSET]);
            memcpy(&(gpt[j+(i*4)].partition_name),
                    &data[(j * partition_entry_size) + PARTITION_NAME_OFFSET], MAX_GPT_NAME_SIZE);
        }
    }
    return ret;
}

uint64_t gpt_lookup(uint8_t * name, unsigned type){
    uint32_t input_string_length = strlen(name);
    uint8_t UTF16_name[MAX_GPT_NAME_SIZE];
    unsigned n;

    memset(&UTF16_name, 0x00, MAX_GPT_NAME_SIZE);
    /* Currently partition names in partition.xml are UTF-8 and lowercase */
    for(n = 0; n < input_string_length && n < MAX_GPT_NAME_SIZE/2; n++){
        UTF16_name[n*2] = name[n];
        UTF16_name[n*2+1] = 0x00;
    }
    for(n = 0; n < gpt_partition_count; n++){
        if(!memcmp(&UTF16_name, &gpt[n].partition_name, MAX_GPT_NAME_SIZE)){
            if(type == PTN_SIZE)
                return ((uint64_t)(gpt[n].last_lba - gpt[n].first_lba) * MMC_BOOT_RD_BLOCK_LEN);
            else if(type == PTN_OFFSET)
                return ((uint64_t)gpt[n].first_lba * MMC_BOOT_RD_BLOCK_LEN);
            else
                return 0;
        }
    }
    return 0;
}
