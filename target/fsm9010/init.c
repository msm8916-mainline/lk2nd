/* Copyright (c) 2013-2016, The Linux Foundation. All rights reserved.
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
#include <platform/iomap.h>
#include <platform/irqs.h>
#include <platform/gpio.h>
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <dload_util.h>
#include <uart_dm.h>
#include <mmc.h>
#include <spmi.h>
#include <board.h>
#include <smem.h>
#include <baseband.h>
#include <dev/keys.h>
#include <crypto5_wrapper.h>
#include <hsusb.h>
#include <clock.h>
#include <partition_parser.h>
#include <scm.h>
#include <platform/clock.h>
#include <platform/gpio.h>
#include <platform/timer.h>
#include <stdlib.h>
#include <string.h>
#include <sdhci_msm.h>

extern  bool target_use_signed_kernel(void);
static void set_sdc_power_ctrl(void);

static unsigned int target_id;

#if MMC_SDHCI_SUPPORT
struct mmc_device *dev;
#endif

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0

#define WDOG_DEBUG_DISABLE_BIT  17

#define CE_INSTANCE             2
#define CE_EE                   1
#define CE_FIFO_SIZE            64
#define CE_READ_PIPE            3
#define CE_WRITE_PIPE           2
#define CE_READ_PIPE_LOCK_GRP   0
#define CE_WRITE_PIPE_LOCK_GRP  0
#define CE_ARRAY_SIZE           20

#define FASTBOOT_MODE           0x77665500

#define BOARD_SOC_VERSION1(soc_rev) (soc_rev >= 0x10000 && soc_rev < 0x20000)

#if MMC_SDHCI_SUPPORT
static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE };
static uint32_t mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ };
#endif

static uint32_t mmc_sdc_base[] =
	{ MSM_SDC1_BASE };

void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(3, 0, BLSP1_UART3_BASE);
#endif
}

/* Return 1 if vol_up pressed */
int target_volume_up(void)
{
	return 0;
}

/* Return 1 if vol_down pressed */
uint32_t target_volume_down(void)
{
	return 0;
}

static void target_keystatus(void)
{
	keys_init();

	if (target_volume_down())
		keys_post_event(KEY_VOLUMEDOWN, 1);

	if (target_volume_up())
		keys_post_event(KEY_VOLUMEUP, 1);
}

/* Set up params for h/w CE. */
void target_crypto_init_params(void)
{
	struct crypto_init_params ce_params;

	/* Set up base addresses and instance. */
	ce_params.crypto_instance  = CE_INSTANCE;
	ce_params.crypto_base      = MSM_CE2_BASE;
	ce_params.bam_base         = MSM_CE2_BAM_BASE;

	/* Set up BAM config. */
	ce_params.bam_ee               = CE_EE;
	ce_params.pipes.read_pipe      = CE_READ_PIPE;
	ce_params.pipes.write_pipe     = CE_WRITE_PIPE;
	ce_params.pipes.read_pipe_grp  = CE_READ_PIPE_LOCK_GRP;
	ce_params.pipes.write_pipe_grp = CE_WRITE_PIPE_LOCK_GRP;

	/* Assign buffer sizes. */
	ce_params.num_ce           = CE_ARRAY_SIZE;
	ce_params.read_fifo_size   = CE_FIFO_SIZE;
	ce_params.write_fifo_size  = CE_FIFO_SIZE;

	/* BAM is initialized by TZ for this platform.
	 * Do not do it again as the initialization address space
	 * is locked.
	 */
	ce_params.do_bam_init      = 0;

	crypto_init_params(&ce_params);
}

crypto_engine_type board_ce_type(void)
{
	return CRYPTO_ENGINE_TYPE_HW;
}

#if MMC_SDHCI_SUPPORT

static void target_mmc_sdhci_init(void)
{
	static uint32_t mmc_clks[] = {
		MMC_CLK_200MHZ, MMC_CLK_96MHZ, MMC_CLK_50MHZ };

	struct mmc_config_data config;
	unsigned int i;

	memset(&config, 0, sizeof config);
	config.bus_width = DATA_BUS_WIDTH_8BIT;

	/* Trying Slot 1*/
	config.slot = 1;
	config.sdhc_base = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base = mmc_sdc_base[config.slot - 1];
	config.pwr_irq     = mmc_sdc_pwrctl_irq[config.slot - 1];
	config.hs400_support = 0;

	for (i = 0; i < ARRAY_SIZE(mmc_clks); ++i) {
		config.max_clk_rate = mmc_clks[i];
		dprintf(INFO, "SDHC Running at %u MHz\n",
			config.max_clk_rate / 1000000);
		dev = mmc_init(&config);
		if (dev && partition_read_table() == 0)
			return;
	}

	if (dev == NULL)
		dprintf(CRITICAL, "mmc init failed!");
	else
		dprintf(CRITICAL, "Error reading the partition table info\n");
	ASSERT(0);
}

void *target_mmc_device(void)
{
	return (void *) dev;
}

#else

static void target_mmc_mci_init(void)
{
	uint32_t base_addr;
	uint8_t slot;

	/* Trying Slot 1 */
	slot = 1;
	base_addr = mmc_sdc_base[slot - 1];

	if (mmc_boot_main(slot, base_addr))
	{
		dprintf(CRITICAL, "mmc init failed!");
		ASSERT(0);
	}
}

/*
 * Function to set the capabilities for the host
 */
void target_mmc_caps(struct mmc_host *host)
{
	host->caps.bus_width = MMC_BOOT_BUS_WIDTH_8_BIT;
	host->caps.ddr_mode = 0;
	host->caps.hs200_mode = 1;
	host->caps.hs_clk_rate = MMC_CLK_96MHZ;
}

#endif

void target_init(void)
{
	dprintf(INFO, "target_init()\n");

#if (!WITH_LK2ND_DEVICE)
	target_keystatus();
#endif

	if (target_use_signed_kernel())
		target_crypto_init_params();

	/*
	 * Set drive strength & pull ctrl for
	 * emmc
	 */
	set_sdc_power_ctrl();

#if MMC_SDHCI_SUPPORT
	target_mmc_sdhci_init();
#else
	target_mmc_mci_init();
#endif
}

unsigned board_machtype(void)
{
	return target_id;
}

void target_fastboot_init(void)
{
}

/* Detect the target type */
void target_detect(struct board_data *board)
{
	/* This property is filled as part of board.c */
}

/* Detect the modem type */
void target_baseband_detect(struct board_data *board)
{
	uint32_t platform;
	uint32_t platform_subtype;

	platform = board->platform;
	platform_subtype = board->platform_subtype;

	/*
	 * Look for platform subtype if present, else
	 * check for platform type to decide on the
	 * baseband type
	 */
	switch (platform_subtype) {
	case HW_PLATFORM_SUBTYPE_UNKNOWN:
		break;
	default:
		dprintf(CRITICAL, "Platform Subtype : %u is not supported\n",platform_subtype);
		ASSERT(0);
	};

	switch (platform) {
	case FSM9008:
	case FSM9010:
	case FSM9016:
	case FSM9055:
		board->baseband = BASEBAND_MSM;
		break;
	default:
		dprintf(CRITICAL, "Platform type: %u is not supported\n",platform);
		ASSERT(0);
	};
}

unsigned target_baseband(void)
{
	return board_baseband();
}

void target_serialno(unsigned char *buf)
{
	unsigned int serialno;
	if (target_is_emmc_boot()) {
		serialno = mmc_get_psn();
		snprintf((char *)buf, 13, "%x", serialno);
	}
}

unsigned check_reboot_mode(void)
{
	uint32_t restart_reason = 0;
	uint32_t restart_reason_addr;

	restart_reason_addr = RESTART_REASON_ADDR_V2;

	/* Read reboot reason and scrub it */
	restart_reason = readl(restart_reason_addr);
	writel(0x00, restart_reason_addr);

	return restart_reason;
}

int set_download_mode(enum reboot_reason mode)
{
	if (mode == NORMAL_DLOAD || mode == EMERGENCY_DLOAD)
		dload_util_write_cookie(mode == NORMAL_DLOAD ?
			DLOAD_MODE_ADDR_V2 : EMERGENCY_DLOAD_MODE_ADDR_V2, mode);

	return 0;
}

void reboot_device(unsigned reboot_reason)
{
	/* Set cookie for dload mode */
	if(set_download_mode(reboot_reason)) {
		dprintf(CRITICAL, "HALT: set_download_mode not supported\n");
		return;
	}

	/* Write the reboot reason */
	writel(reboot_reason, RESTART_REASON_ADDR_V2);

	/* Disable Watchdog Debug.
	 * Required becuase of a H/W bug which causes the system to
	 * reset partially even for non watchdog resets.
	 */
	writel(readl(GCC_WDOG_DEBUG) & ~(1 << WDOG_DEBUG_DISABLE_BIT), GCC_WDOG_DEBUG);

	dsb();

	/* Wait until the write takes effect. */
	while(readl(GCC_WDOG_DEBUG) & (1 << WDOG_DEBUG_DISABLE_BIT));

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Rebooting failed\n");
}

/* Returns 1 if target supports continuous splash screen. */
int target_cont_splash_screen(void)
{
	return 0;
}

unsigned target_pause_for_battery_charge(void)
{
	return 0;
}

void target_uninit(void)
{
#if MMC_SDHCI_SUPPORT
	mmc_put_card_to_sleep(dev);
	sdhci_mode_disable(&dev->host);
#else
	mmc_put_card_to_sleep();
#endif
}

void shutdown_device(void)
{
	dprintf(CRITICAL, "Going down for shutdown.\n");

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Shutdown failed\n");
}

static void set_sdc_power_ctrl(void)
{
	/* Drive strength configs for sdc pins */
	struct tlmm_cfgs sdc1_hdrv_cfg[] =
	{
		{
			off: SDC1_CLK_HDRV_CTL_OFF,
			val: TLMM_CUR_VAL_10MA,
			mask: TLMM_HDRV_MASK
		},
		{
			off: SDC1_CMD_HDRV_CTL_OFF,
			val: TLMM_CUR_VAL_10MA,
			mask: TLMM_HDRV_MASK
		},
		{
			off: SDC1_DATA_HDRV_CTL_OFF,
			val: TLMM_CUR_VAL_10MA,
			mask: TLMM_HDRV_MASK
		},
	};

	/* Pull configs for sdc pins */
	struct tlmm_cfgs sdc1_pull_cfg[] =
	{
		{
			off: SDC1_CLK_PULL_CTL_OFF,
			val: TLMM_NO_PULL,
			mask: TLMM_PULL_MASK
		},
		{
			off: SDC1_CMD_PULL_CTL_OFF,
			val: TLMM_PULL_UP,
			mask: TLMM_PULL_MASK
		},
		{
			off: SDC1_DATA_PULL_CTL_OFF,
			val: TLMM_PULL_UP,
			mask: TLMM_PULL_MASK
		},
	};

	/* Set the drive strength & pull control values */
	tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
	tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));
}

int emmc_recovery_init(void)
{
	extern int _emmc_recovery_init(void);

	return _emmc_recovery_init();
}

#define USB30_QSCRATCH_GENERAL_CFG			(MSM_USB30_QSCRATCH_BASE + 0x08)
#define USB30_QSCRATCH_GENERAL_CFG_PIPE_UTMI_CLK_SEL	(1 << 0)
#define USB30_QSCRATCH_GENERAL_CFG_PIPE3_PHYSTATUS_SW	(1 << 3)
#define USB30_QSCRATCH_GENERAL_CFG_PIPE_UTMI_CLK_DIS	(1 << 8)

#define CM_DWC_USB2_USB_PHY_UTMI_CTRL5			(CM_DWC_USB2_CM_DWC_USB2_BASE + 0x74)
#define CM_DWC_USB2_USB_PHY_HS_PHY_CTRL_COMMON0		(CM_DWC_USB2_CM_DWC_USB2_BASE + 0x78)
#define CM_DWC_USB2_USB_PHY_PARAMETER_OVERRIDE_X0	(CM_DWC_USB2_CM_DWC_USB2_BASE + 0x98)
#define CM_DWC_USB2_USB_PHY_PARAMETER_OVERRIDE_X1	(CM_DWC_USB2_CM_DWC_USB2_BASE + 0x9c)
#define CM_DWC_USB2_USB_PHY_PARAMETER_OVERRIDE_X2	(CM_DWC_USB2_CM_DWC_USB2_BASE + 0xa0)
#define CM_DWC_USB2_USB_PHY_PARAMETER_OVERRIDE_X3	(CM_DWC_USB2_CM_DWC_USB2_BASE + 0xa4)
#define CM_DWC_USB2_USB_PHY_REFCLK_CTRL			(CM_DWC_USB2_CM_DWC_USB2_BASE + 0xe8)

void target_usb_phy_mux_configure(void)
{
}

void target_usb_phy_init(void)
{
	uint32_t val;

	/* Disable clock */
	val = readl(USB30_QSCRATCH_GENERAL_CFG);
	val |= USB30_QSCRATCH_GENERAL_CFG_PIPE_UTMI_CLK_DIS;
	writel(val, USB30_QSCRATCH_GENERAL_CFG);
	mdelay(1);

	/* Select UTMI instead of PIPE3 */
	val |= USB30_QSCRATCH_GENERAL_CFG_PIPE_UTMI_CLK_SEL;
	writel(val, USB30_QSCRATCH_GENERAL_CFG);
	val |= USB30_QSCRATCH_GENERAL_CFG_PIPE3_PHYSTATUS_SW;
	writel(val, USB30_QSCRATCH_GENERAL_CFG);
	mdelay(1);

	/* Enable clock */
	val &= ~USB30_QSCRATCH_GENERAL_CFG_PIPE_UTMI_CLK_DIS;
	writel(val, USB30_QSCRATCH_GENERAL_CFG);

	/* Initialize HS PICO PHY */
	writel(0xc4, CM_DWC_USB2_USB_PHY_PARAMETER_OVERRIDE_X0);
	writel(0x88, CM_DWC_USB2_USB_PHY_PARAMETER_OVERRIDE_X1);
	writel(0x11, CM_DWC_USB2_USB_PHY_PARAMETER_OVERRIDE_X2);
	writel(0x03, CM_DWC_USB2_USB_PHY_PARAMETER_OVERRIDE_X3);

	writel(0x02, CM_DWC_USB2_USB_PHY_UTMI_CTRL5);
	mdelay(1);
	writel(0x00, CM_DWC_USB2_USB_PHY_UTMI_CTRL5);

	val = readl(CM_DWC_USB2_USB_PHY_REFCLK_CTRL);
	val &= ~(7 << 1);
	val |= (6 << 1);
	writel(val, CM_DWC_USB2_USB_PHY_REFCLK_CTRL);

	val = readl(CM_DWC_USB2_USB_PHY_HS_PHY_CTRL_COMMON0);
	val &= ~(7 << 4);
	val |= (7 << 4);
	writel(val, CM_DWC_USB2_USB_PHY_HS_PHY_CTRL_COMMON0);
}

void target_usb_phy_reset(void)
{
}

target_usb_iface_t* target_usb30_init(void)
{
	target_usb_iface_t *t_usb_iface;

	t_usb_iface = calloc(1, sizeof(target_usb_iface_t));
	ASSERT(t_usb_iface);

	t_usb_iface->mux_config = target_usb_phy_mux_configure;
	t_usb_iface->phy_init   = target_usb_phy_init;
	t_usb_iface->phy_reset  = target_usb_phy_reset;
	t_usb_iface->clock_init = clock_usb30_init;
	t_usb_iface->vbus_override = 1;

	return t_usb_iface;
}

/* identify the usb controller to be used for the target */
const char * target_usb_controller(void)
{
	return "dwc";
}

/* configure hs phy mux if using dwc controller */
void target_usb_stop(void)
{
}
