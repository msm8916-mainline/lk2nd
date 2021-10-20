/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
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

#include "../OEMPublicKey.h"
#include "avb_sysdeps.h"
#include "libavb.h"
#include <platform.h>
#include <err.h>
#include <ab_partition_parser.h>
#include <partition_parser.h>
#define MAX_FOOTER_SIZE 4096

struct partition_entry *PtnEntries;

bool IsCurrentSlotSuccessful()
{
   struct ab_slot_info slot_info[AB_SUPPORTED_SLOTS];
   int slot_idx =INVALID;

   slot_idx = partition_find_active_slot();
   if(slot_idx == INVALID)
   {
	   dprintf(CRITICAL,
	           "IsCurrentSlotSuccessful: no active slots found!\n");
	   return FALSE;
	}
   partition_fill_slot_meta(slot_info);
   if(strncmp(slot_info[slot_idx].slot_is_succesful_rsp,"yes",strlen("yes")))
       return TRUE;

   return FALSE;
}

static struct partition_entry *Getpartition_entry(const char *Partition)
{
	int32_t Index = partition_get_index(Partition);
	struct partition_entry *partition_entries =
				partition_get_partition_entries();
	if (partition_entries == NULL) {
                dprintf(CRITICAL, "Getpartition_entry: No partition entry found\n");
                return NULL;
        }

	PtnEntries = partition_entries;

	if (Index == INVALID_PTN) {
		dprintf(CRITICAL, "Getpartition_entry: No partition entry for "
		           "%s, invalid index\n", Partition);
		return NULL;
	}
	return &PtnEntries[Index];
}

int get_unique_guid(const char *Partition, char *unique_guid)
{
        struct partition_entry *gp = Getpartition_entry(Partition);
	if(gp == NULL)
	{
		dprintf(CRITICAL, "Partition entry not found\n");
                return -1;

	}
        if (!unique_guid)
                return -1;

        memcpy(unique_guid, gp->unique_partition_guid, UNIQUE_PARTITION_GUID_SIZE);

        return 0;
}

static struct partition_entry *GetBootpartition_entry(Slot *BootSlot)
{
	int32_t Index = INVALID_PTN;
	struct partition_entry *partition_entries =
				partition_get_partition_entries();
	if( BootSlot == NULL)
        {
                dprintf(INFO, "No bootable slot found \n");
                return NULL;

        }
	if( partition_entries == NULL)
	{
		dprintf(INFO, "No partition entry found \n");
                return NULL;

	}
	PtnEntries = partition_entries;

	if (strncmp("_a", (const char *)BootSlot->Suffix, strlen((const char *)BootSlot->Suffix)) == 0) {
		Index = partition_get_index("boot_a");
	} else if (strncmp("_b", (const char *)BootSlot->Suffix, strlen((const char *)BootSlot->Suffix)) == 0) {
		Index = partition_get_index("boot_b");
	} else {
		dprintf(CRITICAL, "GetBootpartition_entry: No boot partition "
		                    "entry for slot %s\n", (char *)BootSlot->Suffix);
		return NULL;
	}

	if (Index == INVALID_PTN) {
		dprintf(CRITICAL,
		       "GetBootpartition_entry: No boot partition entry "
		       "for slot %s, invalid index\n", (char *)BootSlot->Suffix);
		return NULL;
	}
	return &PtnEntries[Index];
}

AvbIOResult AvbReadFromPartition(AvbOps *Ops, const char *Partition, int64_t ReadOffset,
                     size_t NumBytes, void *Buffer, size_t *OutNumRead)
{
	AvbIOResult Result = AVB_IO_RESULT_OK;
	EFI_STATUS Status = EFI_SUCCESS;
	VOID *Page = NULL;
	UINT64 Offset = 0;
	UINT64 ptn = 0;
	UINT64 part_size = 0;
	UINT32 PageSize = 0;
	UINT64 StartBlock = 0;
	UINT64 LastBlock = 0;
	UINT64 FullBlock = 0;
	UINT64 StartPageReadSize = 0;
	int index = INVALID_PTN;

	if (Partition == NULL || Buffer == NULL || OutNumRead == NULL || NumBytes <= 0) {
		dprintf(CRITICAL, "bad input paramaters\n");
		Result = AVB_IO_RESULT_ERROR_IO;
		goto out;
	}
	*OutNumRead = 0;

	if (NumBytes > MAX_FOOTER_SIZE && !getimage(Buffer, OutNumRead, Partition)) {
		/* API returns previously loaded Images buffer address and size */
                dprintf(SPEW, "DEBUG: %s already loaded \n", Partition);
		return AVB_IO_RESULT_OK;
	}
	dprintf(SPEW, "Loading image %s\n", Partition);
	index = partition_get_index(Partition);
	ptn = partition_get_offset(index);
	part_size = partition_get_size(index);

	if (ReadOffset < 0) {
		if ((-ReadOffset) > (int64_t)part_size) {
			dprintf(CRITICAL,
			       "Negative Offset outside range.\n");
			Result = AVB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION;
			goto out;
		}
		Offset = part_size - (-ReadOffset);
		dprintf(DEBUG,
		       "negative Offset (%lld) converted to (0x%llx) \n", ReadOffset, Offset);
	} else {
		// check int64_t to UINT32 converstion?
		Offset = ReadOffset;
	}

	if (Offset > part_size) {
		dprintf(CRITICAL, "Offset outside range.\n");
		Result = AVB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION;
		goto out;
	}

	if (NumBytes > part_size - Offset) {
		NumBytes = part_size - Offset;
	}

	dprintf(CRITICAL,
	       "read from %s, 0x%x bytes at Offset 0x%llx, partition size %llu\n",
	       Partition, NumBytes, Offset, ptn);

	/* |NumBytes| and or |Offset| can be unaligned to block size.
	 */
	PageSize = mmc_get_device_blocksize();
	Page = avb_malloc(PageSize);
	if (Page == NULL) {
		dprintf(CRITICAL, "Allocate for partial read failed!");
		Result = AVB_IO_RESULT_ERROR_OOM;
		goto out;
	}

	StartBlock = Offset / PageSize;
	LastBlock = (NumBytes + Offset) / PageSize;
	FullBlock = StartBlock;
	StartPageReadSize = 0;

	if (Offset % PageSize != 0) {
		/* Offset not aligned to PageSize*/
		UINT64 StartPageReadOffset = Offset - (StartBlock * PageSize);

		if (StartBlock == LastBlock) {
			/* Offset & Offset + NumBytes are in same block */
			StartPageReadSize = NumBytes;
		} else {
			StartPageReadSize = PageSize - StartPageReadOffset;
			FullBlock++;
		}

		dprintf(DEBUG,
		   "StartBlock 0x%llx, ReadOffset 0x%llx, read_size 0x%llx\n",
		    StartBlock, StartPageReadOffset, StartPageReadSize);
		if (StartPageReadSize <= 0 || StartPageReadOffset >= PageSize ||
		    StartPageReadSize > PageSize - StartPageReadOffset ||
		    StartPageReadSize > NumBytes) {
			dprintf(CRITICAL,
			       "StartBlock 0x%llx, ReadOffset 0x%llx,"
				"read_size 0x%llx outside range.\n",
			       StartBlock, StartPageReadOffset, StartPageReadSize);
			Result = AVB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION;
			goto out;
		}

		Status = mmc_read(ptn + (StartBlock * PageSize), Page, PageSize);
		if (Status == EFI_SUCCESS) {
			avb_memcpy(Buffer, Page + StartPageReadOffset, StartPageReadSize);
			*OutNumRead += StartPageReadSize;
		} else {
			*OutNumRead = 0;
			dprintf(CRITICAL, "ReadBlocks failed %d\n", Status);
			goto out;
		}
	}

	if (*OutNumRead < NumBytes && (NumBytes + Offset) % PageSize != 0) {
		/* NumBytes + Offset not aligned to PageSize*/
		/* Offset for last block is always zero, start at Page boundary
		 */
		UINT64 LastPageReadOffset = 0;
		UINT64 ReadOffset2 = (LastBlock * PageSize);
		UINT64 LastPageReadSize = (Offset + NumBytes) - ReadOffset2;

		dprintf(DEBUG,
		      "LastBlock 0x%llx, ReadOffset 0x%llx, read_size 0x%llx\n",
		       LastBlock, LastPageReadOffset, LastPageReadSize);

		if (LastPageReadSize <= 0 || LastPageReadSize >= PageSize ||
		    LastPageReadSize > (NumBytes - *OutNumRead)) {
			dprintf(CRITICAL,
			       "LastBlock 0x%llx, ReadOffset 0x%llx, read_size "
			       "0x%llx outside range.\n",
			       LastBlock, LastPageReadOffset, LastPageReadSize);
			Result = AVB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION;
			goto out;
		}

		Status = mmc_read(ptn + ReadOffset2, Page, PageSize);
		if (Status == EFI_SUCCESS) {
			avb_memcpy(Buffer + (NumBytes - LastPageReadSize), Page,
			           LastPageReadSize);
			*OutNumRead += LastPageReadSize;
		} else {
			*OutNumRead = 0;
			dprintf(CRITICAL, "ReadBlocks failed %d\n", Status);
			goto out;
		}
	}

	if (*OutNumRead < NumBytes) {
		/* full block reads */
		UINT64 FillPageReadSize = NumBytes - *OutNumRead;

		if ((FillPageReadSize % PageSize) != 0 ||
		    (NumBytes - StartPageReadSize) < FillPageReadSize) {
			dprintf(CRITICAL,
			       "FullBlock 0x%llx, ReadOffset 0x%x, read_size "
			       "0x%llx outside range.\n",
			       FullBlock, 0, FillPageReadSize);
			Result = AVB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION;
			goto out;
		}
			dprintf(SPEW,
			       "FullBlock 0x%llx, ReadOffset 0x%x, read_size "
			       "0x%llx outside range. StartPageReadSize %#llx PageSize %d ptn %#llx Buffer %p\n",
			       FullBlock, 0, FillPageReadSize, StartPageReadSize, PageSize, ptn, Buffer);
		Status = mmc_read(ptn + FullBlock * PageSize, Buffer + StartPageReadSize,
					FillPageReadSize);
		if (Status == EFI_SUCCESS) {
			*OutNumRead += FillPageReadSize;
		} else {
			*OutNumRead = 0;
			dprintf(CRITICAL, "ReadBlocks failed %d\n", Status);
			goto out;
		}
	}
out:
	if (Page != NULL) {
		avb_free(Page);
	}

	return Result;
}

AvbIOResult AvbWriteToPartition(AvbOps *Ops, const char *Partition, int64_t Offset,
                                size_t NumBytes, const void *Buffer)
{
	/* unsupported api */
	return AVB_IO_RESULT_ERROR_IO;
}

AvbIOResult
AvbValidateVbmetaPublicKey(AvbOps *Ops, const uint8_t *PublicKeyData,
                           size_t PublicKeyLength, const uint8_t *PublicKeyMetadata,
                           size_t PublicKeyMetadataLength, bool *OutIsTrusted)
{
	UINT8 *UserKeyBuffer = NULL;
	UINT32 UserKeyLength = 0;
	EFI_STATUS Status = EFI_SUCCESS;
	AvbOpsUserData *UserData = NULL;

	dprintf(DEBUG, "ValidateVbmetaPublicKey PublicKeyLength %d, "
	                      "PublicKeyMetadataLength %d\n",
	       PublicKeyLength, PublicKeyMetadataLength);

	if (Ops == NULL || OutIsTrusted == NULL || PublicKeyData == NULL) {
		dprintf(CRITICAL, "Invalid parameters\n");
		return AVB_IO_RESULT_ERROR_IO;
	}

	Status = get_userkey(&UserKeyBuffer, &UserKeyLength);
	if (Status != EFI_SUCCESS) {
		dprintf(CRITICAL, "get_userkey failed\n");
		return AVB_IO_RESULT_ERROR_IO;
	}

	UserData = (AvbOpsUserData *)Ops->user_data;
	UserData->IsUserKey = FALSE;

	if (PublicKeyLength == UserKeyLength &&
	    memcmp(PublicKeyData, UserKeyBuffer, PublicKeyLength) == 0) {
		*OutIsTrusted = true;
		UserData->IsUserKey = TRUE;
	} else if (PublicKeyLength == ARRAY_SIZE(OEMPublicKey) &&
	           memcmp(PublicKeyData, OEMPublicKey, PublicKeyLength) == 0) {
		*OutIsTrusted = true;
	} else {
		*OutIsTrusted = false;
		memset(UserData->PublicKey, 0, ARRAY_SIZE(UserData->PublicKey));
		UserData->PublicKeyLen = 0;
	}

	if (*OutIsTrusted == true) {
		if (PublicKeyLength > ARRAY_SIZE(UserData->PublicKey)) {
			dprintf(CRITICAL, "ValidateVbmetaPublicKey: "
			                    "public key length too large %d\n",
			       PublicKeyLength);
			return AVB_IO_RESULT_ERROR_OOM;
		}
		memcpy(UserData->PublicKey, PublicKeyData, PublicKeyLength);
		UserData->PublicKeyLen = PublicKeyLength;
	}
	dprintf(DEBUG,
	       "ValidateVbmetaPublicKey OutIsTrusted %d, UserKey %d\n",
	       *OutIsTrusted, UserData->IsUserKey);
	return AVB_IO_RESULT_OK;
}

AvbIOResult
AvbValidatePartitionPublicKey(AvbOps *Ops, const char* Partition,
                           const uint8_t *PublicKeyData, size_t PublicKeyLength,
                           const uint8_t *PublicKeyMetadata, size_t PublicKeyMetadataLength,
                           bool *OutIsTrusted, uint32_t* OutRollbackIndexLocation)
{
	UINT8 *UserKeyBuffer = NULL;
	UINT32 UserKeyLength = 0;
	EFI_STATUS Status = EFI_SUCCESS;
	AvbOpsUserData *UserData = NULL;

	dprintf(DEBUG, "ValidatePartitionPublicKey PublicKeyLength %d, "
	                      "PublicKeyMetadataLength %d\n",
		   PublicKeyLength, PublicKeyMetadataLength);

	if (OutIsTrusted == NULL || PublicKeyData == NULL) {
		dprintf(ERROR, "Invalid parameters\n");
		return AVB_IO_RESULT_ERROR_IO;
	}

	Status = get_userkey(&UserKeyBuffer, &UserKeyLength);
	if (Status != EFI_SUCCESS) {
		dprintf(ERROR, "get_userkey failed!, %d\n", Status);
		return AVB_IO_RESULT_ERROR_IO;
	}

	UserData = (AvbOpsUserData *)Ops->user_data;
	UserData->IsUserKey = FALSE;

	if (PublicKeyLength == UserKeyLength &&
			memcmp (PublicKeyData, UserKeyBuffer, PublicKeyLength) == 0) {
		*OutIsTrusted = true;
		UserData->IsUserKey = TRUE;
	} else if (PublicKeyLength == ARRAY_SIZE(OEMPublicKey) &&
			   memcmp(PublicKeyData, OEMPublicKey, PublicKeyLength) == 0) {
		*OutIsTrusted = true;
	} else {
		*OutIsTrusted = false;
		memset (UserData->PublicKey, ARRAY_SIZE (UserData->PublicKey), 0);
		UserData->PublicKeyLen = 0;
	}

	if (*OutIsTrusted == true) {
		if (PublicKeyLength > ARRAY_SIZE (UserData->PublicKey)) {
			dprintf(ERROR, "ValidatePartitionPublicKey: "
				"public key length too large %d\n",
			PublicKeyLength);
			return AVB_IO_RESULT_ERROR_OOM;
        }
        memcpy (UserData->PublicKey, PublicKeyData, PublicKeyLength);
        UserData->PublicKeyLen = PublicKeyLength;
    }

	*OutRollbackIndexLocation = 1; // Recovery rollback index
	dprintf(DEBUG,
		   "ValidatePartitionPublicKey OutIsTrusted %d, UserKey %d\n",
		   *OutIsTrusted, UserData->IsUserKey);
    return AVB_IO_RESULT_OK;
}

AvbIOResult AvbReadRollbackIndex(AvbOps *Ops, size_t RollbackIndexLocation,
                                 uint64_t *OutRollbackIndex)
{

	EFI_STATUS Status = read_rollback_index(RollbackIndexLocation, OutRollbackIndex);

	if (Status != EFI_SUCCESS) {
		dprintf(CRITICAL, "ReadRollbackIndex failed! %d\n", Status);
		return AVB_IO_RESULT_ERROR_IO;
	}
	dprintf(DEBUG,
	       "ReadRollbackIndex Location %zu, RollbackIndex %llu\n",
	       RollbackIndexLocation, *OutRollbackIndex);
	return AVB_IO_RESULT_OK;
}

AvbIOResult
AvbWriteRollbackIndex(AvbOps *Ops, size_t RollbackIndexLocation, uint64_t RollbackIndex)
{
	EFI_STATUS Status = EFI_SUCCESS;
	BOOLEAN UpdateRollbackIndex = FALSE;
	AvbOpsUserData *UserData = NULL;

	dprintf(DEBUG,
	       "WriteRollbackIndex Location %zu, RollbackIndex %llu\n",
	       RollbackIndexLocation, RollbackIndex);

	UserData = (AvbOpsUserData *)Ops->user_data;
	if(UserData->IsMultiSlot) {
		/* Update rollback if the current slot is successful */
		if (IsCurrentSlotSuccessful()) {
			UpdateRollbackIndex = TRUE;
		} else {
			UpdateRollbackIndex = FALSE;
			dprintf(DEBUG, "Not updating rollback index as current "
					"slot is unbootable\n");
		}
	} else {
		/* When Multislot is disabled, always update*/
		UpdateRollbackIndex = TRUE;
	}

	if(UpdateRollbackIndex == TRUE) {
		dprintf(INFO,
		       "Updating rollback index %llu, for location %zu\n",
		       RollbackIndex, RollbackIndexLocation);
		Status = write_rollback_index(RollbackIndexLocation, RollbackIndex);
		if (Status != EFI_SUCCESS) {
			dprintf(CRITICAL, "ReadRollbackIndex failed! %d\n", Status);
			return AVB_IO_RESULT_ERROR_IO;
		}
	}
	return AVB_IO_RESULT_OK;
}

AvbIOResult AvbReadIsDeviceUnlocked(AvbOps *Ops, bool *OutIsUnlocked)
{
	if (OutIsUnlocked == NULL) {
		dprintf(CRITICAL, "bad input paramaters\n");
		return AVB_IO_RESULT_ERROR_IO;
	}
	*OutIsUnlocked = !is_device_locked();
	return AVB_IO_RESULT_OK;
}

static VOID GuidToHex(CHAR8 *Buf, EFI_GUID *Guid)
{
	CHAR8 HexDigits[17] = "0123456789abcdef";

	Buf[0] = HexDigits[(Guid->Data1 >> 28) & 0x0f];
	Buf[1] = HexDigits[(Guid->Data1 >> 24) & 0x0f];
	Buf[2] = HexDigits[(Guid->Data1 >> 20) & 0x0f];
	Buf[3] = HexDigits[(Guid->Data1 >> 16) & 0x0f];
	Buf[4] = HexDigits[(Guid->Data1 >> 12) & 0x0f];
	Buf[5] = HexDigits[(Guid->Data1 >> 8) & 0x0f];
	Buf[6] = HexDigits[(Guid->Data1 >> 4) & 0x0f];
	Buf[7] = HexDigits[(Guid->Data1 >> 0) & 0x0f];
	Buf[8] = '-';
	Buf[9] = HexDigits[(Guid->Data2 >> 12) & 0x0f];
	Buf[10] = HexDigits[(Guid->Data2 >> 8) & 0x0f];
	Buf[11] = HexDigits[(Guid->Data2 >> 4) & 0x0f];
	Buf[12] = HexDigits[(Guid->Data2 >> 0) & 0x0f];
	Buf[13] = '-';
	Buf[14] = HexDigits[(Guid->Data3 >> 12) & 0x0f];
	Buf[15] = HexDigits[(Guid->Data3 >> 8) & 0x0f];
	Buf[16] = HexDigits[(Guid->Data3 >> 4) & 0x0f];
	Buf[17] = HexDigits[(Guid->Data3 >> 0) & 0x0f];
	Buf[18] = '-';
	Buf[19] = HexDigits[(Guid->Data4[0] >> 4) & 0x0f];
	Buf[20] = HexDigits[(Guid->Data4[0] >> 0) & 0x0f];
	Buf[21] = HexDigits[(Guid->Data4[1] >> 4) & 0x0f];
	Buf[22] = HexDigits[(Guid->Data4[1] >> 0) & 0x0f];
	Buf[23] = '-';
	Buf[24] = HexDigits[(Guid->Data4[2] >> 4) & 0x0f];
	Buf[25] = HexDigits[(Guid->Data4[2] >> 0) & 0x0f];
	Buf[26] = HexDigits[(Guid->Data4[3] >> 4) & 0x0f];
	Buf[27] = HexDigits[(Guid->Data4[3] >> 0) & 0x0f];
	Buf[28] = HexDigits[(Guid->Data4[4] >> 4) & 0x0f];
	Buf[29] = HexDigits[(Guid->Data4[4] >> 0) & 0x0f];
	Buf[30] = HexDigits[(Guid->Data4[5] >> 4) & 0x0f];
	Buf[31] = HexDigits[(Guid->Data4[5] >> 0) & 0x0f];
	Buf[32] = HexDigits[(Guid->Data4[6] >> 4) & 0x0f];
	Buf[33] = HexDigits[(Guid->Data4[6] >> 0) & 0x0f];
	Buf[34] = HexDigits[(Guid->Data4[7] >> 4) & 0x0f];
	Buf[35] = HexDigits[(Guid->Data4[7] >> 0) & 0x0f];
	Buf[36] = '\0';
}

AvbIOResult AvbGetUniqueGuidForPartition(AvbOps *Ops, const char *PartitionName,
                                         char *GuidBuf, size_t GuidBufSize)
{
	EFI_STATUS Status = EFI_SUCCESS;
	char unique_partition_guid[UNIQUE_PARTITION_GUID_SIZE];
	CHAR16 UnicodePartition[MAX_GPT_NAME_SIZE] = {0};

	Status = get_unique_guid(PartitionName, unique_partition_guid);
	if (Status) {
		dprintf(CRITICAL,
		       "get_unique_guid: No partition entry for %s\n",
		       PartitionName);
		return AVB_IO_RESULT_ERROR_IO;
	}

	if ((strlen(PartitionName) + 1) > ARRAY_SIZE(UnicodePartition)) {
		dprintf(CRITICAL, "AvbGetUniqueGuidForPartition: Partition "
		                    "%s, name too large\n",
		       PartitionName);
		return AVB_IO_RESULT_ERROR_IO;
	}

	GuidToHex(GuidBuf, (EFI_GUID *)unique_partition_guid);
	dprintf(DEBUG, "%s uuid: %s\n", PartitionName, GuidBuf);

	return AVB_IO_RESULT_OK;
}

AvbIOResult AvbGetSizeOfPartition(AvbOps *Ops, const char *Partition, uint64_t *OutSizeNumBytes)
{
	AvbIOResult Result = AVB_IO_RESULT_OK;
	int index;

	if (Ops == NULL || Partition == NULL || OutSizeNumBytes == NULL) {
		dprintf(CRITICAL,
		       "AvbGetSizeOfPartition invalid parameter pointers\n");
		return AVB_IO_RESULT_ERROR_IO;
	}

	index = partition_get_index(Partition);
	*OutSizeNumBytes = (uint64_t)partition_get_size(index);
	if (*OutSizeNumBytes == 0)
		return AVB_IO_RESULT_ERROR_IO;

	return Result;
}

AvbOps *AvbOpsNew(VOID *UserData)
{
	AvbOps *Ops = avb_calloc(sizeof(AvbOps));
	if (Ops == NULL) {
		dprintf(CRITICAL, "Error allocating memory for AvbOps.\n");
		goto out;
	}

	Ops->user_data = UserData;
	Ops->read_from_partition = AvbReadFromPartition;
	Ops->write_to_partition = AvbWriteToPartition;
	Ops->validate_vbmeta_public_key = AvbValidateVbmetaPublicKey;
	Ops->validate_public_key_for_partition = AvbValidatePartitionPublicKey;
	Ops->read_rollback_index = AvbReadRollbackIndex;
	Ops->write_rollback_index = AvbWriteRollbackIndex;
	Ops->read_is_device_unlocked = AvbReadIsDeviceUnlocked;
	Ops->get_unique_guid_for_partition = AvbGetUniqueGuidForPartition;
	Ops->get_size_of_partition = AvbGetSizeOfPartition;

out:
	return Ops;
}

VOID AvbOpsFree(AvbOps *Ops)
{
	if (Ops != NULL) {
		avb_free(Ops);
	}
}
