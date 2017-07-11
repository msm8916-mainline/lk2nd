/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of The Linux Foundation nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <fastboot.h>

extern const char *suffix_slot[];
extern const char *suffix_delimiter;

#define SUFFIX_SLOT(part_slot) suffix_slot[(part_slot)]

enum
{
 SLOT_A = 0,
 SLOT_B = 1,
 AB_SUPPORTED_SLOTS = 2,
 INVALID = -1
};

/* Structure to print get var info */
struct ab_slot_info
{
	char slot_is_unbootable[MAX_GET_VAR_NAME_SIZE];
	char slot_is_unbootable_rsp[MAX_RSP_SIZE];
	char slot_is_active[MAX_GET_VAR_NAME_SIZE];
	char slot_is_active_rsp[MAX_RSP_SIZE];
	char slot_is_succesful[MAX_GET_VAR_NAME_SIZE];
	char slot_is_succesful_rsp[MAX_RSP_SIZE];
	char slot_retry_count[MAX_GET_VAR_NAME_SIZE];
	char slot_retry_count_rsp[MAX_RSP_SIZE];
};

/* A/B support API(s) */
bool partition_multislot_is_supported();/* Check Multislot is supported */
bool partition_scan_for_multislot();	/* Calling to scan part. table. */
void partition_mark_active_slot();	/* Marking slot active */
void partition_reset_attributes();	/* Resetting slot attr. */
void partition_fill_slot_meta();	/* Fill slot meta infomation */
void partition_switch_slots();		/* Switching slots */
int partition_find_boot_slot();		/* Find bootable partition */
int partition_find_active_slot();	/* Find current active partition*/
int partition_fill_partition_meta();	/* Fill partition slot info meta*/

