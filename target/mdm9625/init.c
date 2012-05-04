/*
 * Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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
 */
#include <debug.h>
#include <board.h>
#include <platform.h>
#include <target.h>
#include <smem.h>
#include <baseband.h>

/* init */
void target_init(void)
{
	dprintf(INFO, "target_init()\n");
}

/* reboot */
void reboot_device(unsigned reboot_reason)
{
	dprintf(CRITICAL, "Rebooting failed\n");
	return;
}

/* Check if we need to trigger fastboot */
int fastboot_trigger(void)
{
	return 0;
}

/* Create ATAGs for this target */
unsigned* target_atag_mem(unsigned* ptr)
{
	return ptr;
}

/* mach type */
unsigned board_machtype(void)
{
	return board_target_id();
}

/* Identify the current target */
void target_detect(struct board_data *board)
{
	switch(board->platform_hw)
	{
	case HW_PLATFORM_SURF:
		board->target = LINUX_MACHTYPE_9625_CDP;
		break;

	case HW_PLATFORM_MTP:
		board->target = LINUX_MACHTYPE_9625_MTP;
		break;

	default:
		board->target = LINUX_MACHTYPE_9625_CDP;
	}
}

/* Identify the baseband being used */
void target_baseband_detect(struct board_data *board)
{
	if (board->platform_subtype == HW_PLATFORM_SUBTYPE_MDM)
	{
		board->baseband = BASEBAND_MDM;
	}
	else
	{
		/* This target will always have MDM. If not, assert. */
		ASSERT(0);
	}
}
