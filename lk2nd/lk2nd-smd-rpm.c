// SPDX-License-Identifier: GPL-2.0-only

#include <libfdt.h>
#include <debug.h>
#include <smd.h>
#include <smem.h>

static smd_port_ctrl_info *smd_find_rpm(void)
{
	smd_channel_alloc_entry_t *entries;
	smd_port_ctrl_info *info;
	uint32_t size = 0;
	int i;

	entries = smem_get_alloc_entry(SMEM_CHANNEL_ALLOC_TBL, &size);
	if (size != SMD_CHANNEL_ALLOC_MAX)
		return NULL;

	for (i = 0; i < SMEM_NUM_SMD_STREAM_CHANNELS; i++) {
		if ((entries[i].ctype & 0xff) != SMD_APPS_RPM)
			continue;

		size = 0;
		info = smem_get_alloc_entry(SMEM_SMD_BASE_ID + entries[i].cid,
					    &size);
		if (size == sizeof(*info))
			return info;
	}

	return NULL;
}

void lk2nd_smd_rpm_hack_opening(const void *fdt, int offset)
{
	smd_port_ctrl_info *info;
	int len;

	fdt_getprop(fdt, offset, "lk2nd,smd-rpm-hack-opening", &len);
	if (len < 0)
		return;

	info = smd_find_rpm();
	if (!info) {
		dprintf(CRITICAL, "Cannot find SMD RPM channel\n");
		return;
	}

	if (info->ch1.stream_state != SMD_SS_CLOSING) {
		dprintf(CRITICAL, "SMD RPM channel has unexpected state: %u\n",
			info->ch1.stream_state);
		return;
	}

	/* DO NOT LOOK AT THIS. THIS MESSES WITH THE REMOTE STATE AND IS REALLY BAD */
	info->ch1.stream_state = SMD_SS_OPENING;

	dprintf(INFO, "Hacked SMD RPM channel state back to OPENING. Good luck!\n");
}
