/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
#include <hsusb.h>
#include <mmc.h>
#include <platform/timer.h>
#include <platform/irqs.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <qmp_phy.h>
#include <qusb2_phy.h>

extern void smem_ptable_init(void);
extern void smem_add_modem_partitions(struct ptable *flash_ptable);
void target_sdc_init();

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

#define EXT4_CMDLINE  " rootfstype=ext4 root=/dev/mmcblk0p"
#define UBI_CMDLINE " rootfstype=ubifs rootflags=bulk_read ubi.fm_autoconvert=1"

struct qpic_nand_init_config config;

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
	uart_dm_init(3, 0, BLSP1_UART2_BASE);
#endif
}

int target_is_emmc_boot(void)
{
	return platform_boot_dev_isemmc();
}

/* init */
void target_init(void)
{
	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	if (platform_boot_dev_isemmc()) {
		target_sdc_init();
		if (partition_read_table()) {
			dprintf(CRITICAL, "Error reading the partition table info\n");
			ASSERT(0);
		}
	} else {
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
}

/* reboot */
void reboot_device(unsigned reboot_reason)
{
	/* Write the reboot reason */
	writel(reboot_reason, RESTART_REASON_ADDR);

	/* Configure PMIC for warm reset */
	/* PM 8019 v1 aligns with PM8941 v2.
	 * This call should be based on the pmic version
	 * when PM8019 v2 is available.
	 */
	pm8x41_v2_reset_configure(PON_PSHOLD_WARM_RESET);

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
	board->baseband = BASEBAND_MSM;
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

	if (!strstr(cmdline, "root=/dev/ram")) /* This check is to handle kdev boot */
	{
		if (!target_is_emmc_boot()) {
			/* Below is for NAND boot */
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
			/* Adding command line parameters according to target boot type */
			snprintf(buf, buflen, UBI_CMDLINE);
			snprintf(buf+strlen(buf), buflen, " root=ubi0:rootfs ubi.mtd=%d", system_ptn_index);
		}
		else {
			/* Below is for emmc boot */
			system_ptn_index = partition_get_index(part) + 1; /* Adding +1 as offsets for eMMC start at 1 and NAND at 0 */
			if (system_ptn_index < 0) {
				dprintf(CRITICAL,
						"WARN: Cannot get partition index for %s\n", part);
				return -1;
			}
			snprintf(buf, buflen, EXT4_CMDLINE"%d", system_ptn_index);
		}

		return 0;
	}
}

const char * target_usb_controller()
{
	return "dwc";
}

static void set_sdc_power_ctrl()
{
	/* Drive strength configs for sdc pins */
	struct tlmm_cfgs sdc1_hdrv_cfg[] =
	{
		{ SDC1_CLK_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK },
		{ SDC1_CMD_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK },
		{ SDC1_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_6MA, TLMM_HDRV_MASK },
	};

	/* Pull configs for sdc pins */
	struct tlmm_cfgs sdc1_pull_cfg[] =
	{
		{ SDC1_CLK_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK },
		{ SDC1_CMD_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK },
		{ SDC1_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK },
	};

	/* Set the drive strength & pull control values */
	tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
	tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));
}

static struct mmc_device *dev;

void *target_mmc_device()
{
	return (void *) dev;
}

void target_sdc_init()
{
	struct mmc_config_data config;

	/* Set drive strength & pull ctrl values */
	set_sdc_power_ctrl();

	config.slot = 1;
	config.bus_width = DATA_BUS_WIDTH_4BIT;
	config.max_clk_rate = MMC_CLK_200MHZ;
	config.sdhc_base    = MSM_SDC1_SDHCI_BASE;
	config.pwrctl_base  = MSM_SDC1_BASE;
	config.pwr_irq      = SDCC1_PWRCTL_IRQ;
	config.hs400_support = 0;
	config.use_io_switch = 1;

	if (!(dev = mmc_init(&config))) {
		dprintf(CRITICAL, "mmc init failed!");
		ASSERT(0);
	}
}

void target_uninit(void)
{
	if (platform_boot_dev_isemmc())
	{
		mmc_put_card_to_sleep(dev);
		sdhci_mode_disable(&dev->host);
	}
}

void target_usb_phy_reset(void)
{
	usb30_qmp_phy_reset();
	qusb2_phy_reset();
}

target_usb_iface_t* target_usb30_init()
{
	target_usb_iface_t *t_usb_iface;

	t_usb_iface = calloc(1, sizeof(target_usb_iface_t));
	ASSERT(t_usb_iface);

	t_usb_iface->mux_config = NULL;
	t_usb_iface->phy_init   = usb30_qmp_phy_init;
	t_usb_iface->phy_reset  = target_usb_phy_reset;
	t_usb_iface->clock_init = clock_usb30_init;
	t_usb_iface->vbus_override = 1;

	return t_usb_iface;
}

uint32_t target_override_pll()
{
	return 1;
}

uint32_t target_get_hlos_subtype()
{
	return board_hlos_subtype();
}
