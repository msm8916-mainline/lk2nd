/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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
 *
 */

#include <debug.h>
#include <board.h>
#include <smem.h>
#include <baseband.h>

static struct board_data board = {UNKNOWN,
	HW_PLATFORM_UNKNOWN,
	HW_PLATFORM_SUBTYPE_UNKNOWN,
	LINUX_MACHTYPE_UNKNOWN,};

static void platform_detect()
{
	struct smem_board_info_v6 board_info_v6;
	unsigned int board_info_len = 0;
	unsigned ret = 0;
	unsigned format = 0;

	ret = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
						   &format, sizeof(format), 0);
	if (ret)
		return;

	if (format == 6) {
		board_info_len = sizeof(board_info_v6);

		ret =
			smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
					  &board_info_v6,
					  board_info_len);
		if (ret)
			return;

		board.platform = board_info_v6.board_info_v3.msm_id;
		board.platform_hw =
			board_info_v6.board_info_v3.hw_platform;
		board.platform_subtype =
			board_info_v6.platform_subtype;
	} else {
		dprintf(CRITICAL, "Unsupported board info format.\n");
	}
}

static void target_detect()
{
	unsigned platform_id;
	unsigned platform_hw;
	unsigned target_id;

	platform_id = board.platform;
	platform_hw = board.platform_hw;

	/* Detect the board we are running on */
	if ((platform_id == MSM8960) || (platform_id == MSM8660A)
	    || (platform_id == MSM8260A) || (platform_id == APQ8060A)) {
		switch (platform_hw) {
		case HW_PLATFORM_SURF:
			target_id = LINUX_MACHTYPE_8960_CDP;
			break;
		case HW_PLATFORM_MTP:
			target_id = LINUX_MACHTYPE_8960_MTP;
			break;
		case HW_PLATFORM_FLUID:
			target_id = LINUX_MACHTYPE_8960_FLUID;
			break;
		case HW_PLATFORM_LIQUID:
			target_id = LINUX_MACHTYPE_8960_LIQUID;
			break;
		default:
			target_id = LINUX_MACHTYPE_8960_CDP;
		}
	} else if ((platform_id == MSM8230) || (platform_id == MSM8630)
		   || (platform_id == MSM8930) || (platform_id == APQ8030)) {
		switch (platform_hw) {
		case HW_PLATFORM_SURF:
			target_id = LINUX_MACHTYPE_8930_CDP;
			break;
		case HW_PLATFORM_MTP:
			target_id = LINUX_MACHTYPE_8930_MTP;
			break;
		case HW_PLATFORM_FLUID:
			target_id = LINUX_MACHTYPE_8930_FLUID;
			break;
		default:
			target_id = LINUX_MACHTYPE_8930_CDP;
		}
	} else if ((platform_id == MSM8227) || (platform_id == MSM8627)) {
		switch (platform_hw) {
		case HW_PLATFORM_SURF:
			target_id = LINUX_MACHTYPE_8627_CDP;
			break;
		case HW_PLATFORM_MTP:
			target_id = LINUX_MACHTYPE_8627_MTP;
			break;
		default:
			target_id = LINUX_MACHTYPE_8627_CDP;
		}
	} else if (platform_id == MPQ8064) {
		switch (platform_hw) {
		case HW_PLATFORM_SURF:
			target_id = LINUX_MACHTYPE_8064_MPQ_CDP;
			break;
		case HW_PLATFORM_HRD:
			target_id = LINUX_MACHTYPE_8064_HRD;
			break;
		case HW_PLATFORM_DTV:
			target_id = LINUX_MACHTYPE_8064_DTV;
			break;
		default:
			target_id = LINUX_MACHTYPE_8064_MPQ_CDP;
		}
	} else if ((platform_id == APQ8064)) {
		switch (platform_hw) {
		case HW_PLATFORM_SURF:
			target_id = LINUX_MACHTYPE_8064_CDP;
			break;
		case HW_PLATFORM_MTP:
			target_id = LINUX_MACHTYPE_8064_MTP;
			break;
		case HW_PLATFORM_LIQUID:
			target_id = LINUX_MACHTYPE_8064_LIQUID;
			break;
		default:
			target_id = LINUX_MACHTYPE_8064_CDP;
		}
	} else {
		dprintf(CRITICAL, "platform_id (%d) is not identified.\n",
			platform_id);
		ASSERT(0);
	}
	board.target = target_id;
}

static void baseband_detect()
{
	unsigned baseband = BASEBAND_MSM;
	unsigned platform_subtype;
	unsigned platform_id;

	platform_id = board.platform;
	platform_subtype = board.platform_subtype;

	/* Check for MDM or APQ baseband variants.  Default to MSM */
	if (platform_subtype == HW_PLATFORM_SUBTYPE_MDM)
		baseband = BASEBAND_MDM;
	else if (platform_id == APQ8060)
		baseband = BASEBAND_APQ;
	else if (platform_id == APQ8064)
		baseband = BASEBAND_APQ;
	else if (platform_id == MPQ8064)
		baseband = BASEBAND_APQ;
	else
		baseband = BASEBAND_MSM;

	board.baseband = baseband;
}

void board_init()
{
	platform_detect();
	target_detect();
	baseband_detect();
}

unsigned board_platform_id(void)
{
	return board.platform;
}

unsigned board_target_id()
{
	return board.target;
}

unsigned board_baseband()
{
	return board.baseband;
}
