/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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
#include <lib/ptable.h>
#include <qpic_nand.h>
#include <ctype.h>
#include <string.h>
#include <pm8x41.h>
#include <reg.h>
#include <platform/timer.h>
#include <platform/clock.h>
#include <hsusb.h>
#include <bits.h>
#include <qmp_phy.h>
#include <scm.h>

extern void smem_ptable_init(void);
extern void smem_add_modem_partitions(struct ptable *flash_ptable);

static struct ptable flash_ptable;

/* PMIC config data */
#define PMIC_ARB_CHANNEL_NUM                          0
#define PMIC_ARB_OWNER_ID                             0

/* NANDc BAM pipe numbers */
#define DATA_CONSUMER_PIPE                            0
#define DATA_PRODUCER_PIPE                            1
#define CMD_PIPE                                      2

/* NANDc BAM pipe groups */
#define DATA_PRODUCER_PIPE_GRP                        0
#define DATA_CONSUMER_PIPE_GRP                        0
#define CMD_PIPE_GRP                                  1

/* NANDc EE */
#define QPIC_NAND_EE                                  0

/* NANDc max desc length. */
#define QPIC_NAND_MAX_DESC_LEN                        0x7FFF

#define LAST_NAND_PTN_LEN_PATTERN                     0xFFFFFFFF

struct qpic_nand_init_config config;

extern void ulpi_write(unsigned val, unsigned reg);

void update_ptable_names(void)
{
	uint32_t ptn_index;
	struct ptentry *ptentry_ptr = flash_ptable.parts;
	struct ptentry *boot_ptn;
	unsigned i;
	uint32_t len;

	/* Change all names to lower case. */
	for (ptn_index = 0; ptn_index != (uint32_t)flash_ptable.count; ptn_index++)
	{
		len = strlen(ptentry_ptr[ptn_index].name);

		for (i = 0; i < len; i++)
		{
			if (isupper(ptentry_ptr[ptn_index].name[i]))
			{
				ptentry_ptr[ptn_index].name[i] = tolower(ptentry_ptr[ptn_index].name[i]);
			}
		}

		/* SBL fills in the last partition length as 0xFFFFFFFF.
		 * Update the length field based on the number of blocks on the flash.
		 */
		if ((uint32_t)(ptentry_ptr[ptn_index].length) == LAST_NAND_PTN_LEN_PATTERN)
		{
			ptentry_ptr[ptn_index].length = flash_num_blocks() - ptentry_ptr[ptn_index].start;
		}
	}
}

void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(3, 0, MSM_UART2_BASE);
#endif
}

/* init */
void target_init(void)
{
	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	config.pipes.read_pipe = DATA_PRODUCER_PIPE;
	config.pipes.write_pipe = DATA_CONSUMER_PIPE;
	config.pipes.cmd_pipe = CMD_PIPE;

	config.pipes.read_pipe_grp = DATA_PRODUCER_PIPE_GRP;
	config.pipes.write_pipe_grp = DATA_CONSUMER_PIPE_GRP;
	config.pipes.cmd_pipe_grp = CMD_PIPE_GRP;

	config.bam_base = MSM_NAND_BAM_BASE;
	config.nand_base = MSM_NAND_BASE;
	config.ee = QPIC_NAND_EE;
	config.max_desc_len = QPIC_NAND_MAX_DESC_LEN;

	qpic_nand_init(&config);

        ptable_init(&flash_ptable);

	smem_ptable_init();

	smem_add_modem_partitions(&flash_ptable);

	update_ptable_names();

	flash_set_ptable(&flash_ptable);
}

/* Do target specific usb initialization */
void target_usb_init(void)
{
	uint32_t val;

	/* Select and enable external configuration with USB PHY */
	ulpi_write(ULPI_MISC_A_VBUSVLDEXTSEL | ULPI_MISC_A_VBUSVLDEXT, ULPI_MISC_A_SET);

	/* Enable sess_vld */
	val = readl(USB_GENCONFIG_2) | GEN2_SESS_VLD_CTRL_EN;
	writel(val, USB_GENCONFIG_2);

	/* Enable external vbus configuration in the LINK */
	val = readl(USB_USBCMD);
	val |= SESS_VLD_CTRL;
	writel(val, USB_USBCMD);
}

target_usb_iface_t* target_usb30_init()
{
	target_usb_iface_t *t_usb_iface;

	t_usb_iface = calloc(1, sizeof(target_usb_iface_t));
	ASSERT(t_usb_iface);

	t_usb_iface->mux_config = target_usb_phy_mux_configure;
	t_usb_iface->phy_init   = usb30_qmp_phy_init;
	t_usb_iface->phy_reset  = usb30_qmp_phy_reset;
	t_usb_iface->clock_init = clock_usb30_init;
	t_usb_iface->vbus_override = 1;

	return t_usb_iface;
}


static int scm_clear_boot_partition_select()
{
	int ret = 0;

	ret = scm_call_atomic2(SCM_SVC_BOOT, WDOG_DEBUG_DISABLE, 1, 0);
	if (ret)
		dprintf(CRITICAL, "Failed to disable the wdog debug \n");

	return ret;
}

/* reboot */
void reboot_device(unsigned reboot_reason)
{
	uint8_t reset_type = 0;

	/* Clear the boot partition select cookie to indicate
	 * its a normal reset and avoid going to download mode */
	scm_clear_boot_partition_select();

	/* Write the reboot reason */
	writel(reboot_reason, RESTART_REASON_ADDR);

	/* Configure PMIC for warm reset */
	/* PM 8019 v1 aligns with PM8941 v2.
	 * This call should be based on the pmic version
	 * when PM8019 v2 is available.
	 */
	if(reboot_reason)
		reset_type = PON_PSHOLD_WARM_RESET;
	else
		reset_type = PON_PSHOLD_HARD_RESET;

	pm8x41_v2_reset_configure(reset_type);

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Rebooting failed\n");
	return;
}

/* Identify the current target */
void target_detect(struct board_data *board)
{
	/* This property is filled as part of board.c */
}

unsigned board_machtype(void)
{
	return LINUX_MACHTYPE_UNKNOWN;
}

/* Identify the baseband being used */
void target_baseband_detect(struct board_data *board)
{
	/* Check for baseband variants. Default to MSM */
	if (board->platform_subtype == HW_PLATFORM_SUBTYPE_UNKNOWN)
	{
			board->baseband = BASEBAND_MSM;
	}
	else
	{
		dprintf(CRITICAL, "Could not identify baseband id (%d)\n",
				board->platform_subtype);
		ASSERT(0);
	}
}

unsigned check_reboot_mode(void)
{
	unsigned restart_reason = 0;

	/* Read reboot reason and scrub it */
	restart_reason = readl(RESTART_REASON_ADDR);

	writel(0x00, RESTART_REASON_ADDR);

	return restart_reason;
}

int get_target_boot_params(const char *cmdline, const char *part, char *buf,
			   int buflen)
{
	struct ptable *ptable;
	int system_ptn_index = -1;

	if (!cmdline || !part || !buf || buflen < 0) {
		dprintf(CRITICAL, "WARN: Invalid input param\n");
		return -1;
	}

	ptable = flash_get_ptable();
	if (!ptable) {
		dprintf(CRITICAL,
			"WARN: Cannot get flash partition table\n");
		return -1;
	}

	system_ptn_index = ptable_get_index(ptable, part);
	if (system_ptn_index < 0) {
		dprintf(CRITICAL,
			"WARN: Cannot get partition index for %s\n", part);
		return -1;
	}

	/*
	 * check if cmdline contains "root="/"ubi.mtd" at the beginning of buffer or
	 * " root="/"ubi.mtd" in the middle of buffer.
	 */

	if (strstr(cmdline, "rootfstype=yaffs2")) {
		if (((!strncmp(cmdline, "root=", strlen("root="))) ||
			(strstr(cmdline, " root="))))
			dprintf(DEBUG, "DEBUG: cmdline has root=\n");
		else
			snprintf(buf, buflen, " root=/dev/mtdblock%d",
					system_ptn_index);
	} else if (strstr(cmdline, "rootfstype=ubifs")){
		if (((!strncmp(cmdline, "ubi.mtd=", strlen("ubi.mtd="))) ||
			(strstr(cmdline, " ubi.mtd="))))
			dprintf(DEBUG, "DEBUG: cmdline has ubi.mtd=\n");
		else
			snprintf(buf, buflen, " ubi.mtd=%d",
				system_ptn_index);
	}

	return 0;
}

/* identify the usb controller to be used for the target */
const char * target_usb_controller()
{
    return "dwc";
}

/* mux hs phy to route to dwc controller */
static void phy_mux_configure_with_tcsr()
{
	/* As per the hardware team, set the mux for snps controller */
	RMWREG32(TCSR_PHSS_USB2_PHY_SEL, 0x0, 0x1, 0x1);
}

/* configure hs phy mux if using dwc controller */
void target_usb_phy_mux_configure(void)
{
    if(!strcmp(target_usb_controller(), "dwc"))
    {
        phy_mux_configure_with_tcsr();
    }
}

uint32_t target_override_pll()
{
	return board_soc_version() == BOARD_SOC_VERSION2 ? 1 : 0;
}
