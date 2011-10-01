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

char *ext3_partitions[] = {"system", "userdata", "persist", "cache", "tombstones"};
char *vfat_partitions[] = {"modem", "mdm", "NONE"};
unsigned int ext3_count = 0;
unsigned int vfat_count = 0;

struct partition_entry partition_entries[NUM_PARTITIONS];
unsigned gpt_partitions_exist = 0;
unsigned partition_count = 0;

//TODO: Remove the dependency of mmc in these functions
unsigned int partition_read_table( struct mmc_boot_host * mmc_host,
                                   struct mmc_boot_card * mmc_card)
{
    unsigned int ret;

    /* Read MBR of the card */
    ret = mmc_boot_read_mbr( mmc_host, mmc_card );
    if( ret != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL,  "MMC Boot: MBR read failed!\n" );
        return MMC_BOOT_E_FAILURE;
    }

    /* Read GPT of the card if exist */
    if(gpt_partitions_exist){
        ret = mmc_boot_read_gpt(mmc_host, mmc_card);
        if( ret != MMC_BOOT_E_SUCCESS )
        {
            dprintf(CRITICAL,  "MMC Boot: GPT read failed!\n" );
            return MMC_BOOT_E_FAILURE;
        }
    }
    return MMC_BOOT_E_SUCCESS;
}

/*
 * Read MBR from MMC card and fill partition table.
 */
unsigned int mmc_boot_read_mbr( struct mmc_boot_host * mmc_host,
                                struct mmc_boot_card * mmc_card)
{
    unsigned char buffer[MMC_BOOT_RD_BLOCK_LEN];
    unsigned int dtype;
    unsigned int dfirstsec;
    unsigned int EBR_first_sec;
    unsigned int EBR_current_sec;
    int ret = MMC_BOOT_E_SUCCESS;
    int idx, i;

    /* Print out the MBR first */
    ret = mmc_boot_read_from_card( mmc_host, mmc_card, 0, \
                                   MMC_BOOT_RD_BLOCK_LEN,   \
                                   (unsigned int *)buffer);
    if (ret)
    {
        dprintf(CRITICAL, "Could not read partition from mmc");
        return ret;
    }

    /* Check to see if signature exists */
    ret = partition_verify_mbr_signature(MMC_BOOT_RD_BLOCK_LEN, buffer);
    if (ret)
    {
       return ret;
    }

    /*
     * Process each of the four partitions in the MBR by reading the table
     * information into our mbr table.
     */
    partition_count = 0;
    idx = TABLE_ENTRY_0;
    for (i = 0; i < 4; i++)
    {
        /* Type 0xEE indicates end of MBR and GPT partitions exist */
        dtype  = buffer[idx + i * TABLE_ENTRY_SIZE + OFFSET_TYPE];
        if (dtype == MBR_PROTECTED_TYPE){
            gpt_partitions_exist = 1;
            return ret;
        }
        partition_entries[partition_count].dtype = dtype;
        partition_entries[partition_count].attribute_flag = \
                    buffer[idx + i * TABLE_ENTRY_SIZE + OFFSET_STATUS];
        partition_entries[partition_count].first_lba = \
                    GET_LWORD_FROM_BYTE(&buffer[idx + \
                                        i * TABLE_ENTRY_SIZE + \
                                        OFFSET_FIRST_SEC]);
        partition_entries[partition_count].size  = \
                    GET_LWORD_FROM_BYTE(&buffer[idx + \
                                        i * TABLE_ENTRY_SIZE + \
                                        OFFSET_SIZE]);
        dfirstsec = partition_entries[partition_count].first_lba;
        mbr_fill_name(&partition_entries[partition_count],  \
                      partition_entries[partition_count].dtype);
        partition_count++;
        if (partition_count == NUM_PARTITIONS)
            return ret;
    }

    /* See if the last partition is EBR, if not, parsing is done */
    if (dtype != MBR_EBR_TYPE)
    {
        return ret;
    }

    EBR_first_sec = dfirstsec;
    EBR_current_sec = dfirstsec;

    ret = mmc_boot_read_from_card( mmc_host, mmc_card,  \
                                   (EBR_first_sec * 512), \
                                   MMC_BOOT_RD_BLOCK_LEN, \
                                   (unsigned int *)buffer);
    if (ret)
    {
        return ret;
    }
    /* Loop to parse the EBR */
    for (i = 0;; i++)
    {
        ret = partition_verify_mbr_signature(MMC_BOOT_RD_BLOCK_LEN, buffer);
        if (ret)
        {
           ret = MMC_BOOT_E_SUCCESS;
           break;
        }
        partition_entries[partition_count].attribute_flag = \
                    buffer[TABLE_ENTRY_0 + OFFSET_STATUS];
        partition_entries[partition_count].dtype   = \
                    buffer[TABLE_ENTRY_0 + OFFSET_TYPE];
        partition_entries[partition_count].first_lba = \
                    GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_0 + \
                                        OFFSET_FIRST_SEC])    + \
                                        EBR_current_sec;
        partition_entries[partition_count].size = \
                    GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_0 + \
                                        OFFSET_SIZE]);
        mbr_fill_name(&(partition_entries[partition_count]), \
                      partition_entries[partition_count].dtype);
        partition_count++;
        if (partition_count == NUM_PARTITIONS)
            return ret;

        dfirstsec =
            GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_1 + OFFSET_FIRST_SEC]);
        if(dfirstsec == 0)
        {
            /* Getting to the end of the EBR tables */
            break;
        }
        /* More EBR to follow - read in the next EBR sector */
        dprintf(SPEW, "Reading EBR block from 0x%X\n", EBR_first_sec
                                                          + dfirstsec);
        ret = mmc_boot_read_from_card( mmc_host, mmc_card, \
                                       ((EBR_first_sec + dfirstsec) * 512), \
                                       MMC_BOOT_RD_BLOCK_LEN, \
                                       (unsigned int *)buffer);
        if (ret)
        {
            return ret;
        }
        EBR_current_sec = EBR_first_sec + dfirstsec;
    }
    return ret;
}

/*
 * Read GPT from MMC and fill partition table
 */
unsigned int mmc_boot_read_gpt( struct mmc_boot_host * mmc_host,
                               struct mmc_boot_card * mmc_card)
{

    int ret = MMC_BOOT_E_SUCCESS;
    unsigned int header_size;
    unsigned long long first_usable_lba;
    unsigned long long backup_header_lba;
    unsigned int max_partition_count = 0;
    unsigned int partition_entry_size;
    unsigned char data[MMC_BOOT_RD_BLOCK_LEN];
    unsigned int i = 0; /* Counter for each 512 block */
    unsigned int j = 0; /* Counter for each 128 entry in the 512 block */
    unsigned int n = 0; /* Counter for UTF-16 -> 8 conversion */
    unsigned char UTF16_name[MAX_GPT_NAME_SIZE];
    /* LBA of first partition -- 1 Block after Protected MBR + 1 for PT */
    unsigned long long partition_0 = 2;

    /* Print out the GPT first */
    ret = mmc_boot_read_from_card( mmc_host, mmc_card, \
                                   PROTECTIVE_MBR_SIZE, \
                                   MMC_BOOT_RD_BLOCK_LEN, \
                                   (unsigned int *)data);

    if (ret)
        dprintf(CRITICAL, "GPT: Could not read primary gpt from mmc\n");

    ret = partition_parse_gpt_header(data, &first_usable_lba,
                                     &partition_entry_size, &header_size,
                                     &max_partition_count);

    if (ret)
    {
        dprintf(INFO, "GPT: (WARNING) Primary signature invalid\n" );

        /* Check the backup gpt */
        backup_header_lba = GET_LLWORD_FROM_BYTE(&data[BACKUP_HEADER_OFFSET]);
        ret = mmc_boot_read_from_card( mmc_host, mmc_card, \
                                       (backup_header_lba * BLOCK_SIZE), \
                                       MMC_BOOT_RD_BLOCK_LEN, \
                                       (unsigned int *)data);

        if (ret)
        {
            dprintf(CRITICAL, "GPT: Could not read backup gpt from mmc\n");
            return ret;
        }

        ret = partition_parse_gpt_header(data, &first_usable_lba,
                                         &partition_entry_size, &header_size,
                                         &max_partition_count);
        if (ret)
        {
            dprintf(CRITICAL, "GPT: Primary and backup signatures invalid\n");
            return ret;
        }
        partition_0 = backup_header_lba - (max_partition_count / 4);
    }

    /* Read GPT Entries */
    for(i = 0; i < (max_partition_count/4); i++)
    {
        ret = mmc_boot_read_from_card( mmc_host, mmc_card,
                                       (partition_0 * BLOCK_SIZE) +
                                       (i * MMC_BOOT_RD_BLOCK_LEN),
                                       MMC_BOOT_RD_BLOCK_LEN,
                                       (uint32_t *)data);

        if (ret)
        {
            dprintf(CRITICAL,
                    "GPT: mmc read card failed reading partition entries.\n" );
            return ret;
        }

        for(j=0; j < 4; j++)
        {
            memcpy(&(partition_entries[partition_count].type_guid),
                    &data[(j * partition_entry_size)],
                    PARTITION_TYPE_GUID_SIZE);
            if (partition_entries[partition_count].type_guid[0] == 0x00 &&
                partition_entries[partition_count].type_guid[1] == 0x00)
            {
                i = max_partition_count;
                break;
            }
            memcpy(&(partition_entries[partition_count].unique_partition_guid),
                    &data[(j * partition_entry_size) +
                            UNIQUE_GUID_OFFSET], UNIQUE_PARTITION_GUID_SIZE);
            partition_entries[partition_count].first_lba =
                GET_LLWORD_FROM_BYTE(&data[(j * partition_entry_size) +
                                             FIRST_LBA_OFFSET]);
            partition_entries[partition_count].last_lba =
                GET_LLWORD_FROM_BYTE(&data[(j * partition_entry_size) +
                                             LAST_LBA_OFFSET]);
            partition_entries[partition_count].size =
                partition_entries[partition_count].last_lba -
                partition_entries[partition_count].first_lba;
            partition_entries[partition_count].attribute_flag =
                GET_LLWORD_FROM_BYTE(&data[(j * partition_entry_size) +
                                             ATTRIBUTE_FLAG_OFFSET]);

            memset(&UTF16_name, 0x00, MAX_GPT_NAME_SIZE);
            memcpy(UTF16_name, &data[(j * partition_entry_size) +
                                             PARTITION_NAME_OFFSET],
                                             MAX_GPT_NAME_SIZE);
            /*
             * Currently partition names in *.xml are UTF-8 and lowercase
             * Only supporting english for now so removing 2nd byte of UTF-16
             */
            for(n = 0; n < MAX_GPT_NAME_SIZE/2; n++){
                partition_entries[partition_count].name[n] = UTF16_name[n*2];
            }
            partition_count++;
        }
    }

    return ret;
}

/*
 * Fill name for android partition found.
 */
static void mbr_fill_name (struct partition_entry *partition_ent,
                            unsigned int type)
{
    switch(type)
    {
        memset(partition_ent->name, 0, MAX_GPT_NAME_SIZE);
        case MBR_MODEM_TYPE:
        case MBR_MODEM_TYPE2:
            /* if already assigned last name available then return */
            if(!strcmp((const char *)vfat_partitions[vfat_count], "NONE"))
                return;
            strlcpy((char *)partition_ent->name,
                    (const char *)vfat_partitions[vfat_count],
                    sizeof(partition_ent->name));
            vfat_count++;
            break;
        case MBR_SBL1_TYPE:
            memcpy(partition_ent->name,"sbl1",4);
            break;
        case MBR_SBL2_TYPE:
            memcpy(partition_ent->name,"sbl2",4);
            break;
        case MBR_SBL3_TYPE:
            memcpy(partition_ent->name,"sbl3",4);
            break;
        case MBR_RPM_TYPE:
            memcpy(partition_ent->name,"rpm",3);
            break;
        case MBR_TZ_TYPE:
            memcpy(partition_ent->name,"tz",2);
            break;
        case MBR_ABOOT_TYPE:
            memcpy(partition_ent->name,"aboot",5);
            break;
        case MBR_BOOT_TYPE:
            memcpy(partition_ent->name,"boot",4);
            break;
        case MBR_MODEM_ST1_TYPE:
            memcpy(partition_ent->name,"modem_st1",9);
            break;
        case MBR_MODEM_ST2_TYPE:
            memcpy(partition_ent->name,"modem_st2",9);
            break;
        case MBR_EFS2_TYPE:
            memcpy(partition_ent->name,"efs2",4);
            break;
        case MBR_USERDATA_TYPE:
            if (ext3_count == sizeof(ext3_partitions) / sizeof(char*))
                return;
            strlcpy((char *)partition_ent->name,
                    (const char *)ext3_partitions[ext3_count],
                    sizeof(partition_ent->name));
            ext3_count++;
            break;
        case MBR_RECOVERY_TYPE:
            memcpy(partition_ent->name,"recovery",8);
            break;
        case MBR_MISC_TYPE:
            memcpy(partition_ent->name,"misc",4);
            break;
    };
}

/*
 * Find index of parition in array of partition entries
 */
unsigned partition_get_index (const char * name)
{
    unsigned int input_string_length = strlen(name);
    unsigned n;

    for(n = 0; n < partition_count; n++){
        if(!memcmp(name, &partition_entries[n].name, input_string_length) &&
        input_string_length == strlen((const char *)&partition_entries[n].name))
        {
            return n;
        }
    }
    return INVALID_PTN;
}

/* Get size of the partition */
unsigned long long partition_get_size (int index)
{
    if (index == INVALID_PTN)
        return 0;
    else{
        return partition_entries[index].size * MMC_BOOT_RD_BLOCK_LEN;
    }
}

/* Get offset of the partition */
unsigned long long partition_get_offset (int index)
{
    if (index == INVALID_PTN)
        return 0;
    else{
        return partition_entries[index].first_lba * MMC_BOOT_RD_BLOCK_LEN;
    }
}

/* Debug: Print all parsed partitions */
void partition_dump()
{
    unsigned i = 0;
    for (i=0; i< partition_count; i++){
        dprintf(SPEW,
                "ptn[%d]:Name[%s] Size[%llu] Type[%u] First[%llu] Last[%llu]\n",
                i, partition_entries[i].name, partition_entries[i].size,
                partition_entries[i].dtype, partition_entries[i].first_lba,
                partition_entries[i].last_lba);
    }
}

unsigned int partition_verify_mbr_signature(unsigned size,
                                            unsigned char* buffer)
{
    /* Avoid checking past end of buffer */
    if ((TABLE_SIGNATURE + 1) > size)
    {
        return MMC_BOOT_E_FAILURE;
    }
    /* Check to see if signature exists */
    if ((buffer[TABLE_SIGNATURE] != MMC_MBR_SIGNATURE_BYTE_0) || \
        (buffer[TABLE_SIGNATURE + 1] != MMC_MBR_SIGNATURE_BYTE_1))
    {
        dprintf(CRITICAL,  "MBR signature does not match. \n" );
        return MMC_BOOT_E_FAILURE;
    }
    return MMC_BOOT_E_SUCCESS;
}

unsigned int mbr_partition_get_type(unsigned size, unsigned char* partition,
                                    unsigned int *partition_type)
{
    unsigned int type_offset = TABLE_ENTRY_0 + OFFSET_TYPE;

    if (size < type_offset)
    {
        goto end;
    }

    *partition_type = partition[type_offset];
end:
    return MMC_BOOT_E_SUCCESS;
}

unsigned int partition_get_type(unsigned size, unsigned char* partition,
                                unsigned int *partition_type)
{
    unsigned int ret = MMC_BOOT_E_SUCCESS;

    /*
     * If the block contains the MBR signature, then it's likely either
     * MBR or MBR with protective type (GPT).  If the MBR signature is
     * not there, then it could be the GPT backup.
     */

    /* First check the MBR signature */
    ret = partition_verify_mbr_signature(size, partition);
    if (ret == MMC_BOOT_E_SUCCESS)
    {
        unsigned int mbr_partition_type = PARTITION_TYPE_MBR;

        /* MBR signature verified.  This could be MBR, MBR + EBR, or GPT */
        ret = mbr_partition_get_type(size, partition, &mbr_partition_type);
        if (ret != MMC_BOOT_E_SUCCESS)
        {
            dprintf(CRITICAL, "Cannot get TYPE of partition");
        }
        else if (MBR_PROTECTED_TYPE == mbr_partition_type)
        {
            *partition_type = PARTITION_TYPE_GPT;
        }
        else
        {
            *partition_type = PARTITION_TYPE_MBR;
        }
    }
    else
    {
        /*
         * This could be the GPT backup.  Make that assumption for now.
         * Anybody who treats the block as GPT backup should check the
         * signature.
         */
        *partition_type = PARTITION_TYPE_GPT_BACKUP;
    }
    return ret;
}

/*
 * Parse the gpt header and get the required header fields
 * Return 0 on valid signature
 */
unsigned int partition_parse_gpt_header(unsigned char * buffer,
                                        unsigned long long * first_usable_lba,
                                        unsigned long * partition_entry_size,
                                        unsigned long * header_size,
                                        unsigned int * max_partition_count)
{
    /* Check GPT Signature */
    if( ((uint32_t *)buffer)[0] != GPT_SIGNATURE_2 ||
        ((uint32_t *)buffer)[1] != GPT_SIGNATURE_1 )
        return 1;

    *header_size = GET_LWORD_FROM_BYTE(&buffer[HEADER_SIZE_OFFSET]);
    *first_usable_lba = GET_LLWORD_FROM_BYTE(&buffer[FIRST_USABLE_LBA_OFFSET]);
    *max_partition_count = GET_LWORD_FROM_BYTE(&buffer[PARTITION_COUNT_OFFSET]);
    *partition_entry_size = GET_LWORD_FROM_BYTE(&buffer[PENTRY_SIZE_OFFSET]);

    return 0;
}
