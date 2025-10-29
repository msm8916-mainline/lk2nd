/*
 * Copyright (c) 2017, 2019, The Linux Foundation. All rights reserved.
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
#include <err.h>
#include <target.h>

extern const char *suffix_slot[];
extern const char *suffix_delimiter;

#define SUFFIX_SLOT(part_slot) suffix_slot[(part_slot)]
#define MAX_SLOT_SUFFIX_SZ      3
#define BOOT_DEV_NAME_SIZE_MAX 10

typedef struct {
	char Suffix[MAX_SLOT_SUFFIX_SZ];
}Slot;
#define SET_BIT(p,n) ((p) |= ((uint64_t)0x1 << (n)))
#define CLR_BIT(p,n) ((p) &= (~(((uint64_t)0x1) << (n))))

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
bool _partition_multislot_is_supported(void);/* Check Multislot is supported */
bool partition_scan_for_multislot(void);	/* Calling to scan part. table. */
void partition_mark_active_slot(signed slot);	/* Marking slot active */
void partition_reset_attributes(unsigned index);	/* Resetting slot attr. */
void partition_fill_slot_meta(struct ab_slot_info *slot_info);	/* Fill slot meta infomation */
void partition_switch_slots(int old_slot, int new_slot, boolean reset_success_bit); /* Switching slots */
void partition_deactivate_slot(int slot); /* Mark slot unbootable and reset other attributes*/
void partition_activate_slot(int slot);	 /* Mark slot bootable and set other attributes*/
int partition_find_boot_slot(void);		/* Find bootable partition */
int partition_find_active_slot(void);	/* Find current active partition*/
int partition_fill_partition_meta(char has_slot_pname[][MAX_GET_VAR_NAME_SIZE],
				  char has_slot_reply[][MAX_RSP_SIZE],
				  int array_size);	/* Fill partition slot info meta*/

static inline bool partition_multislot_is_supported(void){
#if SLOTS_SUPPORTED
	return true;
#else
	return false;
#endif
}
				  
