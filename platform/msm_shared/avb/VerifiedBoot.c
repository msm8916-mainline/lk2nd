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

#include "libavb/libavb.h"
#include <malloc.h>

#include <boot_verifier.h>
#include <ab_partition_parser.h>
#include <partition_parser.h>
#include <recovery.h>
#include <display_menu.h>
#include <../../../app/aboot/mdtp.h>
#include <platform/timer.h>
#include "verifiedboot.h"
#include <err.h>
#include <target.h>

#ifndef DTB_PAD_SIZE
#define DTB_PAD_SIZE            2048
#endif
#define INTERMEDIATE_DIGEST_LENGTH	64
#define MAX_PART_NAME_SIZE		10
#define MAX_NUM_REQ_PARTITION	8
#define BOOT_HEADER_VERSION_ZERO	0

char *avb_verify_partition_name[] = {
	"boot",
	"dtbo",
	"vbmeta",
	"recovery"
};

#ifndef MDTP_SUPPORT
int mdtp_activated(bool * activated)
{
	return 0;
}
void mdtp_fwlock_verify_lock(mdtp_ext_partition_verification_t *ext_partition)
{
	return;
}
#endif

static const CHAR8 *VerifiedState = " androidboot.verifiedbootstate=";
static const CHAR8 *KeymasterLoadState = " androidboot.keymaster=1";
static const CHAR8 *Space = " ";
#if !VERIFIED_BOOT_2
static const CHAR8 *VerityMode = " androidboot.veritymode=";
static struct verified_boot_verity_mode VbVm[] =
{
	{FALSE, "logging"},
	{TRUE, "enforcing"},
};
#endif

static struct verified_boot_state_name VbSn[] =
{
	{GREEN, "green"},
	{ORANGE, "orange"},
	{YELLOW, "yellow"},
	{RED, "red"},
};

struct boolean_string
{
	BOOLEAN value;
	CHAR8 *name;
};

static struct boolean_string BooleanString[] =
{
	{FALSE, "false"},
	{TRUE, "true"}
};


typedef struct {
	AvbOps *Ops;
	AvbSlotVerifyData *SlotData;
} VB2Data;

UINT32 GetAVBVersion()
{
#if VERIFIED_BOOT_2
	return 2;
#elif VERIFIED_BOOT
	return 1;
#else
	return 0;
#endif
}

BOOLEAN VerifiedBootEnabled()
{
	return (GetAVBVersion() > NO_AVB);
}

static int check_img_header(void *ImageHdrBuffer, uint32_t ImageHdrSize, uint32_t *imgsizeActual)
{
    /* These checks are already done before calling auth remove from here */
#if VERIFIED_BOOT || VERIFIED_BOOT_2
	boot_verifier_init();
#endif
	return 0;
}

static int HandleActiveSlotUnbootable()
{
   int curr_slot;
   curr_slot = partition_find_active_slot();
   partition_deactivate_slot(curr_slot);
   partition_find_boot_slot();

   // should not reach here
   return ERROR;
}

/*
 * Returns length = 0 when there is failure.
 */
uint32_t GetSystemPath(char **SysPath)
{
	INT32 Index;
	UINT32 Lun;
	CHAR8 PartitionName[MAX_GPT_NAME_SIZE];
	CHAR8 LunCharMapping[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
	const char *current_slot_suffix;
	int current_active_slot;

	*SysPath = malloc(sizeof(char) * MAX_PATH_SIZE);
	if (!*SysPath) {
		dprintf(CRITICAL, "Failed to allocated memory for System path query\n");
		return 0;
	}

	strlcpy(PartitionName, "system", MAX_GPT_NAME_SIZE);
	current_active_slot = partition_find_active_slot();
	if (partition_multislot_is_supported()) {
		if (current_active_slot == INVALID)
			return 0;
		current_slot_suffix = SUFFIX_SLOT(current_active_slot);
		strlcat(PartitionName, current_slot_suffix, MAX_GPT_NAME_SIZE - 1);
	}

	Index = partition_get_index(PartitionName);
	if (Index == INVALID_PTN || Index >= NUM_PARTITIONS) {
		dprintf(CRITICAL, "System partition does not exit\n");
		free(*SysPath);
		return 0;
	}

	Lun = partition_get_lun(Index);
	if (platform_boot_dev_isemmc()) {
		snprintf(*SysPath, MAX_PATH_SIZE, " root=/dev/mmcblk0p%d",
				Index + 1);
	} else {
		snprintf(*SysPath, MAX_PATH_SIZE, " root=/dev/sd%c%d",
				LunCharMapping[Lun],
				partition_get_index_in_lun(PartitionName, Lun));
	}

	dprintf(DEBUG, "System Path - %s \n", *SysPath);

	return strlen(*SysPath);
}

static EFI_STATUS Appendvbcmdline(bootinfo *Info, const CHAR8 *Src)
{
	INT32 SrcLen = strlen(Src);
	CHAR8 *Dst = (CHAR8 *)Info->vbcmdline + Info->vbcmdline_filled_len;

	strlcat(Dst, Src, SrcLen);
	Info->vbcmdline_filled_len += SrcLen;

	return EFI_SUCCESS;
}

static EFI_STATUS AppendVBCommonCmdLine(bootinfo *Info)
{
	EFI_STATUS Status = EFI_SUCCESS;

	if (GetAVBVersion() >= AVB_1) {
		GUARD(Appendvbcmdline(Info, VerifiedState));
		GUARD(Appendvbcmdline(Info, VbSn[Info->boot_state].name));
	}
	GUARD(Appendvbcmdline(Info, KeymasterLoadState));
	GUARD(Appendvbcmdline(Info, Space));
	return EFI_SUCCESS;
}

static EFI_STATUS VBCommonInit(bootinfo *Info)
{
	EFI_STATUS Status = EFI_SUCCESS;

	Info->boot_state = RED;

	// FIXME: Add boot call
	/* allocate VB command line*/
	Info->vbcmdline = malloc(2*DTB_PAD_SIZE);
	if (Info->vbcmdline == NULL) {
		dprintf(CRITICAL, "VB CmdLine allocation failed!\n");
		Status = EFI_OUT_OF_RESOURCES;
		return Status;
	}
	Info->vbcmdline_len = 2*DTB_PAD_SIZE;
	Info->vbcmdline_filled_len = 0;
	Info->vbcmdline[Info->vbcmdline_filled_len] = '\0';

	return Status;
}

#if VERIFIED_BOOT_2
/* Disable for VB 2.0 as this path is never taken */
static EFI_STATUS LoadImageNoAuth(bootinfo *Info)
{
	return ERROR;
}
static EFI_STATUS load_image_and_authVB1(bootinfo *Info)
{
	return ERROR;
}
#else
static EFI_STATUS LoadImageNoAuth(bootinfo *Info)
{
	EFI_STATUS Status = EFI_SUCCESS;

	if (Info->images[0].image_buffer != NULL && Info->images[0].imgsize > 0) {
		/* fastboot boot option image already loaded */
		return Status;
	}

	Status = LoadImage(Info->pname, (VOID **)&(Info->images[0].image_buffer),
	                   (UINT32 *)&(Info->images[0].imgsize));
	if (Status != EFI_SUCCESS) {
		dprintf(CRITICAL,
		       "ERROR: Failed to load image from partition: %d\n", Status);
		return EFI_LOAD_ERROR;
	}
	Info->num_loaded_images = 1;
	Info->images[0].name = malloc(strlen(Info->pname) + 1);
	strlcpy(Info->images[0].name, Info->pname, strlen(Info->pname)); //FIXME
	return Status;
}

static EFI_STATUS load_image_and_authVB1(bootinfo *Info)
{
	EFI_STATUS Status = EFI_SUCCESS;
	CHAR8 StrPname[MAX_GPT_NAME_SIZE];
	CHAR8 Pname[MAX_GPT_NAME_SIZE];
	CHAR8 *SystemPath = NULL;
	UINT32 SystemPathLen = 0;
	device_info DevInfo_vb;

	GUARD(VBCommonInit(Info));
	GUARD(LoadImageNoAuth(Info));
	boot_verifier_init();

	// FIXME: INIT devinfo()
	DevInfo_vb.is_unlocked = !is_device_locked();
	DevInfo_vb.is_unlock_critical = !is_device_locked_critical();

	strlcpy(StrPname, "/", strlen("/"));
	strlcpy(Pname, Info->pname, MAX_GPT_NAME_SIZE);
	if (Info->multi_slot_boot) {
		strlcat(StrPname, Pname, MAX_GPT_NAME_SIZE);
	} else {
		strlcat(StrPname, Pname, MAX_GPT_NAME_SIZE);
	}

	Status = boot_verify_image((UINT8 *)Info->images[0].image_buffer,
	                                Info->images[0].imgsize,
					StrPname,
	                                &Info->boot_state);
	if (Status != EFI_SUCCESS || Info->boot_state == BOOT_STATE_MAX) {
		dprintf(CRITICAL, "VBVerifyImage failed with: %d\n", Status);
		return Status;
	}

	set_os_version((unsigned char *)Info->images[0].image_buffer);
	if(!send_rot_command((uint32_t)DevInfo_vb.is_unlocked))
		return EFI_LOAD_ERROR;

	SystemPathLen = GetSystemPath(&SystemPath);
	if (SystemPathLen == 0 || SystemPath == NULL) {
		dprintf(CRITICAL, "GetSystemPath failed!\n");
		return EFI_LOAD_ERROR;
	}
	GUARD(AppendVBCommonCmdLine(Info));
	GUARD(Appendvbcmdline(Info, VerityMode));
	GUARD(Appendvbcmdline(Info, VbVm[is_verity_enforcing()].name));
	GUARD(Appendvbcmdline(Info, SystemPath));

	Info->vb_data = NULL;
	return Status;
}
#endif

static BOOLEAN ResultShouldContinue(AvbSlotVerifyResult Result)
{
	switch (Result) {
	case AVB_SLOT_VERIFY_RESULT_ERROR_OOM:
	case AVB_SLOT_VERIFY_RESULT_ERROR_IO:
	case AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA:
	case AVB_SLOT_VERIFY_RESULT_ERROR_UNSUPPORTED_VERSION:
        case AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT:
		return FALSE;

	case AVB_SLOT_VERIFY_RESULT_OK:
	case AVB_SLOT_VERIFY_RESULT_ERROR_VERIFICATION:
	case AVB_SLOT_VERIFY_RESULT_ERROR_ROLLBACK_INDEX:
	case AVB_SLOT_VERIFY_RESULT_ERROR_PUBLIC_KEY_REJECTED:
		return TRUE;
	}

	return FALSE;
}

char *pname[] = {
	"sbl1",
	"rpm",
	"tz",
	"aboot",
};

VOID AddRequestedPartition(CHAR8 **requestedpartititon, UINT32 index)
{
	UINTN i;
	for (i = 0; i < MAX_NUM_REQ_PARTITION; i++) {
		if (requestedpartititon[i] == NULL){
			requestedpartititon[i] = avb_verify_partition_name[index];
			break;
		}
	}
}

static EFI_STATUS load_image_and_authVB2(bootinfo *Info)
{
	EFI_STATUS Status = EFI_SUCCESS;
	AvbSlotVerifyResult Result;
	AvbSlotVerifyData *SlotData = NULL;
	VB2Data *VBData = NULL;
	AvbOpsUserData *UserData = NULL;
	AvbOps *Ops = NULL;
	CHAR8 Pname[MAX_GPT_NAME_SIZE] = {0};
	CHAR8 *SlotSuffix = NULL;
	BOOLEAN AllowVerificationError = !is_device_locked();
	BOOLEAN VerityEnforcing = is_verity_enforcing();
	CHAR8 *RequestedPartitionAll[MAX_NUM_REQ_PARTITION] = {NULL};
	const CHAR8 **RequestedPartition = NULL;
	UINTN NumRequestedPartition = 0;
	UINT32 HeaderVersion = 0;
	UINT32 ImageHdrSize = 0;
	UINT32 imgsizeActual = 0;
	VOID *image_buffer = NULL;
	UINT32 imgsize = 0;
        AvbSlotVerifyFlags VerifyFlags = AllowVerificationError ?
           AVB_SLOT_VERIFY_FLAGS_ALLOW_VERIFICATION_ERROR :
           AVB_SLOT_VERIFY_FLAGS_NONE;
	AvbHashtreeErrorMode VerityFlags = AVB_HASHTREE_ERROR_MODE_RESTART_AND_INVALIDATE;
	device_info DevInfo_vb;

	HeaderVersion = Info->header_version;
	Info->boot_state = RED;
	GUARD(VBCommonInit(Info));

	UserData = avb_calloc(sizeof(AvbOpsUserData));
	if (UserData == NULL) {
		dprintf(CRITICAL,
		       "ERROR: Failed to allocate AvbOpsUserData\n");
		Status = EFI_OUT_OF_RESOURCES;
		goto out;
	}

	Ops = AvbOpsNew(UserData);
	if (Ops == NULL) {
		dprintf(CRITICAL, "ERROR: Failed to allocate AvbOps\n");
		Status = EFI_OUT_OF_RESOURCES;
		goto out;
	}
	UserData->IsMultiSlot = Info->multi_slot_boot;

	if(Info->multi_slot_boot) {
		strlcpy(Pname, Info->pname, MAX_GPT_NAME_SIZE);
	if ((MAX_SLOT_SUFFIX_SZ + 1) > strlen(Pname)) {
		dprintf(CRITICAL, "ERROR: Can not determine slot suffix\n");
		Status = EFI_INVALID_PARAMETER;
		goto out;
	}
	SlotSuffix = &Pname[strlen(Pname) - MAX_SLOT_SUFFIX_SZ + 1];
	} else {
		 SlotSuffix = "\0";
	}

	if(!Info->multi_slot_boot && Info->bootinto_recovery) {
		AddRequestedPartition(RequestedPartitionAll, IMG_RECOVERY);
		NumRequestedPartition += 1;
		/* Add dtbo validation if target supports dtbo image generation and
		dtbo is not included in recovery i.e. HEADER VERSION is 0 */
		if (is_target_support_dtbo() && HeaderVersion == BOOT_HEADER_VERSION_ZERO) {
			AddRequestedPartition(RequestedPartitionAll, IMG_DTBO);
			NumRequestedPartition += 1;
		}
	} else {
		AddRequestedPartition(RequestedPartitionAll, IMG_BOOT);
		NumRequestedPartition += 1;
		if (is_target_support_dtbo()) {
			AddRequestedPartition(RequestedPartitionAll, IMG_DTBO);
			NumRequestedPartition += 1;
		}
	}

	RequestedPartition = (const CHAR8 **)RequestedPartitionAll;
	if (Info->num_loaded_images) {
		/* fastboot boot option, skip Index 0, boot image already loaded */
		RequestedPartition = (const CHAR8 **)&RequestedPartitionAll[1];
		NumRequestedPartition--;
	}

	VerityFlags = VerityEnforcing ?
				AVB_HASHTREE_ERROR_MODE_RESTART :
				AVB_HASHTREE_ERROR_MODE_EIO;

	Result = avb_slot_verify(Ops, RequestedPartition, SlotSuffix,
				VerifyFlags, VerityFlags,
				&SlotData);

	if (AllowVerificationError && ResultShouldContinue(Result)) {
		dprintf(CRITICAL, "State: Unlocked, AvbSlotVerify returned "
		                    "%s, continue boot\n",
		       avb_slot_verify_result_to_string(Result));
	} else if (Result != AVB_SLOT_VERIFY_RESULT_OK) {
		dprintf(CRITICAL,
		       "ERROR: Device State %s, AvbSlotVerify returned %s\n",
		       AllowVerificationError ? "Unlocked" : "Locked",
		       avb_slot_verify_result_to_string(Result));
		Status = EFI_LOAD_ERROR;
		Info->boot_state = RED;
		goto out;
	}
	if (SlotData == NULL) {
		Status = EFI_LOAD_ERROR;
		Info->boot_state = RED;
		goto out;
	}

	for (UINTN ReqIndex = 0; ReqIndex < NumRequestedPartition; ReqIndex++) {
		dprintf(DEBUG, "Requested Partition: %s\n",
		       RequestedPartition[ReqIndex]);
		for (UINTN loadedindex = 0;
		     loadedindex < SlotData->num_loaded_partitions; loadedindex++) {
			dprintf(DEBUG, "Loaded Partition: %s\n",
			       SlotData->loaded_partitions[loadedindex].partition_name);
			if (!strncmp(((const char *)RequestedPartition[ReqIndex]),
			            SlotData->loaded_partitions[loadedindex].partition_name,MAX_GPT_NAME_SIZE))
			  {
				if (Info->num_loaded_images >= ARRAY_SIZE(Info->images)) {
					dprintf(CRITICAL, "NumLoadedPartition"
					                    "(%d) too large "
					                    "max images(%d)\n",
					       Info->num_loaded_images,
					       ARRAY_SIZE(Info->images));
					Status = EFI_LOAD_ERROR;
					Info->boot_state = RED;
					goto out;
				}
				Info->images[Info->num_loaded_images].name =
				        SlotData->loaded_partitions[loadedindex].partition_name;
				Info->images[Info->num_loaded_images].image_buffer =
				        SlotData->loaded_partitions[loadedindex].data;
				Info->images[Info->num_loaded_images].imgsize =
				        SlotData->loaded_partitions[loadedindex].data_size;
				Info->num_loaded_images++;
				break;
			}
		}
	}

	if (Info->num_loaded_images < NumRequestedPartition) {
		dprintf(CRITICAL, "ERROR: AvbSlotVerify slot data: num of loaded partitions %d, requested %llu\n",Info->num_loaded_images, NumRequestedPartition);
		Status = EFI_LOAD_ERROR;
		goto out;
	}

	dprintf(DEBUG, "Total loaded partition %d\n", Info->num_loaded_images);

	VBData = (VB2Data *)avb_calloc(sizeof(VB2Data));
	if (VBData == NULL) {
		dprintf(CRITICAL, "ERROR: Failed to allocate VB2Data\n");
		Status = EFI_OUT_OF_RESOURCES;
		goto out;
	}
	VBData->Ops = Ops;
	VBData->SlotData = SlotData;
	Info->vb_data = (VOID *)VBData;

	ImageHdrSize = get_page_size();
	GUARD_OUT(getimage(&image_buffer, &imgsize,(!Info->multi_slot_boot && Info->bootinto_recovery) ? "recovery" : "boot") );

	Status = check_img_header(image_buffer, ImageHdrSize, &imgsizeActual);
	if (Status != EFI_SUCCESS) {
		dprintf(CRITICAL, "Invalid boot image header:%d\n", Status);
		goto out;
	}

	if (imgsizeActual > imgsize) {
		Status = EFI_BUFFER_TOO_SMALL;
		dprintf(CRITICAL,
		       "Boot size in vbmeta less than actual boot image size "
		       "flash corresponding vbmeta.img\n");
		goto out;
	}
	if (AllowVerificationError) {
		Info->boot_state = ORANGE;
	} else {
		if (UserData->IsUserKey) {
			Info->boot_state = YELLOW;
		} else {
			Info->boot_state = GREEN;
		}
	}

	/* command line */
	GUARD_OUT(AppendVBCommonCmdLine(Info));
	GUARD_OUT(Appendvbcmdline(Info, SlotData->cmdline));
	DevInfo_vb.is_unlocked = !is_device_locked();
	set_os_version((unsigned char *)Info->images[0].image_buffer);
	if(!send_rot_command((uint32_t)DevInfo_vb.is_unlocked))
		return EFI_LOAD_ERROR;
	dprintf(INFO, "VB2: Authenticate complete! boot state is: %s\n",
	       VbSn[Info->boot_state].name);

out:
	if (Status != EFI_SUCCESS) {
		if (SlotData != NULL) {
			avb_slot_verify_data_free(SlotData);
		}
		if (Ops != NULL) {
			AvbOpsFree(Ops);
		}
		if (UserData != NULL) {
			avb_free(UserData);
		}
		if (VBData != NULL) {
			avb_free(VBData);
		}
		Info->boot_state = RED;
		if(Info->multi_slot_boot) {
		HandleActiveSlotUnbootable();
		/* HandleActiveSlotUnbootable should have swapped slots and
		* reboot the device. If no bootable slot found, enter fastboot */
			dprintf(CRITICAL, "No bootable slots found enter fastboot mode\n");
		 } else {
			dprintf(CRITICAL, "Non Multi-slot: Unbootable entering fastboot mode\n");
		}

		}

	dprintf(CRITICAL, "VB2: boot state: %s(%d)\n",
	       VbSn[Info->boot_state].name, Info->boot_state);
	return Status;
}

static EFI_STATUS DisplayVerifiedBootScreen(bootinfo *Info)
{
	EFI_STATUS Status = EFI_SUCCESS;
	CHAR8 ffbm_mode_string[FFBM_MODE_BUF_SIZE] = {'\0'};

	if (GetAVBVersion() < AVB_1) {
		return EFI_SUCCESS;
	}

	if (!strncmp(Info->pname, "boot", MAX_GPT_NAME_SIZE)) {
		Status = get_ffbm(ffbm_mode_string, FFBM_MODE_BUF_SIZE);
		if (Status != EFI_SUCCESS) {
			dprintf(DEBUG,
			       "No Ffbm cookie found, ignore: %d\n", Status);
			ffbm_mode_string[0] = '\0';
		}
	}

	dprintf(DEBUG, "Boot State is : %d\n", Info->boot_state);
	switch (Info->boot_state)
        {
		case RED:
			display_bootverify_menu(DISPLAY_MENU_RED);
			//if (Status != EFI_SUCCESS) {
				dprintf(INFO, "Your device is corrupt. It can't be trusted and will not boot." \
					"\nYour device will shutdown in 30s\n");
			//}
			udelay(30000000);
			shutdown_device();
			break;
		case YELLOW:
			display_bootverify_menu(DISPLAY_MENU_YELLOW);
			//if (Status == EFI_SUCCESS) {
				wait_for_users_action();
			//} else {
				dprintf(INFO, "Your device has loaded a different operating system." \
					"\nWait for 5 seconds before proceeding\n");
				udelay(5000000);
			//}
			break;
		case ORANGE:
			if (ffbm_mode_string[0] != '\0' && !target_build_variant_user()) {
				dprintf(DEBUG, "Device will boot into FFBM mode\n");
			} else {
				display_bootverify_menu(DISPLAY_MENU_ORANGE);
				if (Status == EFI_SUCCESS) {
					wait_for_users_action();
				} else {
					dprintf(INFO, "Device is unlocked, Skipping boot verification\n");
					udelay(5000000);
				}
			}
			break;
		default:
			break;
	}
	return EFI_SUCCESS;
}

EFI_STATUS load_image_and_auth(bootinfo *Info)
{
	EFI_STATUS Status = EFI_SUCCESS;
	BOOLEAN MdtpActive = FALSE;
	UINT32 AVBVersion = NO_AVB;
	mdtp_ext_partition_verification_t ext_partition;
	const char *current_slot_suffix;
	int current_active_slot;

	if (Info == NULL) {
		dprintf(CRITICAL, "Invalid parameter Info\n");
		return EFI_INVALID_PARAMETER;
	}

	if (!Info->multi_slot_boot) {
		if (Info->bootinto_recovery) {
			dprintf(INFO, "Booting Into Recovery Mode\n");
			strlcpy(Info->pname, "recovery", MAX_GPT_NAME_SIZE);
		} else {
			dprintf(INFO, "Booting Into Mission Mode\n");
			strlcpy(Info->pname, "boot", MAX_GPT_NAME_SIZE);
		}
	} else {
		strlcpy(Info->pname, "boot", MAX_GPT_NAME_SIZE);
		current_active_slot = partition_find_active_slot();
		if (current_active_slot != INVALID ) {
			current_slot_suffix = SUFFIX_SLOT(current_active_slot);
			if (strlen(current_slot_suffix) == 0) {
				dprintf(CRITICAL, "No bootable slot\n");
				return EFI_LOAD_ERROR;
			}
			strlcat(Info->pname, current_slot_suffix, MAX_GPT_NAME_SIZE);
		}
	}

	dprintf(DEBUG, "MultiSlot %s, partition name %s\n",
	       BooleanString[Info->multi_slot_boot].name, Info->pname);

	Status = mdtp_activated(&MdtpActive);
	if (Status) {
		dprintf(CRITICAL,
			       "Failed to get activation state for MDTP, "
			       "Status=%d."
			       " Considering MDTP as active and continuing \n",
			       Status);
		if (Status != -1)
			MdtpActive = TRUE;
	}

	AVBVersion = GetAVBVersion();
	dprintf(DEBUG, "AVB version %d\n", AVBVersion);

	/* Load and Authenticate */
	switch (AVBVersion) {
	case NO_AVB:
		return LoadImageNoAuth(Info);
		break;
	case AVB_1:
		Status = load_image_and_authVB1(Info);
		break;
	case AVB_2:
		Status = load_image_and_authVB2(Info);
		break;
	default:
		dprintf(CRITICAL, "Unsupported AVB version %d\n", AVBVersion);
		Status = EFI_UNSUPPORTED;
	}

	// if MDTP is active Display Recovery UI
	if (Status != EFI_SUCCESS && MdtpActive && !target_use_signed_kernel()) {
		//FIXME: Hard coded to BOOT
		ext_partition.partition = Info->bootinto_recovery ? MDTP_PARTITION_RECOVERY : MDTP_PARTITION_BOOT;
		ext_partition.integrity_state = MDTP_PARTITION_STATE_UNSET;
		ext_partition.page_size = get_page_size();
		ext_partition.image_addr = (uint32)Info->images[0].image_buffer;
		ext_partition.image_size = Info->images[0].imgsize;
		ext_partition.sig_avail = FALSE;
		mdtp_fwlock_verify_lock(&ext_partition);
	}

	if (!is_device_locked() && Status != EFI_SUCCESS) {
		dprintf(CRITICAL, "load_image_and_auth failed %d\n", Status);
		return Status;
	}

	DisplayVerifiedBootScreen(Info);

	return Status;
}

#if VERIFIED_BOOT_2
VOID free_verified_boot_resource(bootinfo *Info)
{
	dprintf(DEBUG, "free_verified_boot_resource\n");

	if (Info == NULL) {
		return;
	}

	VB2Data *VBData = Info->vb_data;
	if (VBData != NULL) {
		AvbOps *Ops = VBData->Ops;
		if (Ops != NULL) {
			if (Ops->user_data != NULL) {
				avb_free(Ops->user_data);
			}
			AvbOpsFree(Ops);
		}

		AvbSlotVerifyData *SlotData = VBData->SlotData;
		if (SlotData != NULL) {
			avb_slot_verify_data_free(SlotData);
		}
		avb_free(VBData);
	}

	if (Info->vbcmdline != NULL) {
		free(Info->vbcmdline);
	}
	return;
}
#else
VOID free_verified_boot_resource(bootinfo *Info)
{
	return;
}
#endif
