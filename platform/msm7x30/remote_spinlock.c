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
#include <platform/iomap.h>
#include <platform/remote_spinlock.h>
#include <debug.h>
#include <smem.h>
#include <reg.h>

static struct smem *smem = (void *)(MSM_SHARED_BASE);

void *smem_rlock_get_entry(unsigned id, unsigned *size)
{
	struct smem_alloc_info *ainfo;
	void *ret = 0;
	unsigned long flags = 0;

	ainfo = &smem->alloc_info[id];

	if (readl(&ainfo->allocated) == 0) {
		dprintf(SPEW,"Shared memory is not allocated\n");
		return ret;
	}
	/* Read the DAL area */
	*size = readl(&ainfo->size);
	ret = (void *) (MSM_SHARED_BASE +  readl(&ainfo->offset));

	return ret;
}

int remote_spinlock_init(remote_spinlock_t *lock)
{
	struct rlock_chunk_header *cur_header;
	void *rlock_smem_start, *rlock_smem_end;
	uint32_t rlock_smem_size = 0;
	int smem_status = 0;

	rlock_smem_start = smem_rlock_get_entry(SMEM_RLOCK_AREA, &rlock_smem_size);

	if (!rlock_smem_start) {
		dprintf(CRITICAL,"Failed to get smem entry for remote spin lock\n");
		return 1;
	}

	rlock_smem_end = rlock_smem_start + rlock_smem_size;

	cur_header = (struct rlock_chunk_header *)
			(((uint32_t)rlock_smem_start + (4095)) & ~4095);
	*lock = NULL;
	/* Find the lock from the list */
	while (cur_header->size != 0
		&& ((uint32_t)(cur_header + 1) < (uint32_t)rlock_smem_end)) {
		/* If we found the lock, get the address */
		if (!strncmp(cur_header->name, RLOCK_CHUNK_NAME,
				RLOCK_CHUNK_NAME_LENGTH)) {
			*lock = (remote_spinlock_t)&cur_header->lock;
			return 0;
		}
		cur_header = (void *)cur_header + cur_header->size;
	}
	dprintf(CRITICAL,"%s: remote lock not found: %s\n",RLOCK_CHUNK_NAME,__FILE__);
	return 1;
}

void remote_spin_lock(raw_remote_spinlock_t *lock)
{
	lock->dek.self_lock = DEK_LOCK_REQUEST;

	/* check if the lock is available*/
	while(lock->dek.other_lock) {
		if(lock->dek.next_yield == DEK_YIELD_TURN_SELF)
		while (lock->dek.other_lock);
		lock->dek.self_lock = DEK_LOCK_REQUEST;
	}
	/* We acquired the lock */
	lock->dek.next_yield = DEK_YIELD_TURN_SELF;
	dmb();
}
void remote_spin_unlock(raw_remote_spinlock_t *lock)
{
	dmb();
	lock->dek.self_lock = DEK_LOCK_YIELD;
}
