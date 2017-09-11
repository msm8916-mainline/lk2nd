/* Copyright (c) 2015-2017, The Linux Foundation. All rights reserved.
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
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <uart_dm.h>
#include <mmc.h>
#include <platform/gpio.h>
#include <dev/keys.h>
#include <spmi_v2.h>
#include <pm8x41.h>
#include <pm8x41_hw.h>
#include <board.h>
#include <baseband.h>
#include <hsusb.h>
#include <scm.h>
#include <platform/irqs.h>
#include <platform/clock.h>
#include <platform/timer.h>
#include <crypto5_wrapper.h>
#include <partition_parser.h>
#include <stdlib.h>
#include <rpm-smd.h>
#include <spmi.h>
#include <sdhci_msm.h>
#include <clock.h>
#include <boot_device.h>
#include <secapp_loader.h>
#include <rpmb.h>
#include <smem.h>
#include <qmp_phy.h>
#include <qusb2_phy.h>
#include "target/display.h"

#if LONG_PRESS_POWER_ON
#include <shutdown_detect.h>
#endif

#if PON_VIB_SUPPORT
#include <vibrator.h>
#define VIBRATE_TIME 250
#endif

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0
#define TLMM_VOL_UP_BTN_GPIO    85

#define FASTBOOT_MODE           0x77665500
#define RECOVERY_MODE           0x77665502
#define PON_SOFT_RB_SPARE       0x88F
#define EXT4_CMDLINE  " rootfstype=ext4 root=/dev/mmcblk0p"

#define CE1_INSTANCE            1
#define CE_EE                   1
#define CE_FIFO_SIZE            64
#define CE_READ_PIPE            3
#define CE_WRITE_PIPE           2
#define CE_READ_PIPE_LOCK_GRP   0
#define CE_WRITE_PIPE_LOCK_GRP  0
#define CE_ARRAY_SIZE           20

#define SMBCHG_USB_RT_STS 0x21310
#define USBIN_UV_RT_STS BIT(0)

struct mmc_device *dev;

static uint32_t mmc_pwrctl_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE };

static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE, MSM_SDC2_SDHCI_BASE };

static uint32_t  mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ, SDCC2_PWRCTL_IRQ };

void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(1, 0, BLSP1_UART0_BASE);
#endif
}

#if _APPEND_CMDLINE
int get_target_boot_params(const char *cmdline, const char *part, char **buf)
{
	int system_ptn_index = -1;
	uint32_t buflen;
	int ret = -1;

	if (!cmdline || !part ) {
		dprintf(CRITICAL, "WARN: Invalid input param\n");
		return -1;
	}

	if (!strstr(cmdline, "root=/dev/ram")) /* This check is to handle kdev boot */
	{
		if (target_is_emmc_boot()) {
			buflen = strlen(EXT4_CMDLINE) + sizeof(int) +1;
			*buf = (char *)malloc(buflen);
			if(!(*buf)) {
				dprintf(CRITICAL,"Unable to allocate memory for boot params\n");
				return -1;
			}
			/* Below is for emmc boot */
			system_ptn_index = partition_get_index(part) + 1; /* Adding +1 as offsets for eMMC start at 1 and NAND at 0 */
			if (system_ptn_index < 0) {
				dprintf(CRITICAL,
						"WARN: Cannot get partition index for %s\n", part);
				free(*buf);
				return -1;
			}
			snprintf(*buf, buflen, EXT4_CMDLINE"%d", system_ptn_index);
			ret = 0;
		}
	}
	/*in success case buf will be freed in the calling function of this*/
	return ret;
}
#endif

static void set_sdc_power_ctrl()
{
	/* Drive strength configs for sdc pins */
	struct tlmm_cfgs sdc1_hdrv_cfg[] =
	{
		{ SDC1_CLK_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, 0},
		{ SDC1_CMD_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, 0},
		{ SDC1_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK , 0},
	};

	/* Pull configs for sdc pins */
	struct tlmm_cfgs sdc1_pull_cfg[] =
	{
		{ SDC1_CLK_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK, 0},
		{ SDC1_CMD_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, 0},
		{ SDC1_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK, 0},
	};

	struct tlmm_cfgs sdc1_rclk_cfg[] =
	{
		{ SDC1_RCLK_PULL_CTL_OFF, TLMM_PULL_DOWN, TLMM_PULL_MASK, 0},
	};

	/* Set the drive strength & pull control values */
	tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
	tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));
	tlmm_set_pull_ctrl(sdc1_rclk_cfg, ARRAY_SIZE(sdc1_rclk_cfg));
}

void target_sdc_init()
{
	struct mmc_config_data config;

	/* Set drive strength & pull ctrl values */
	set_sdc_power_ctrl();

	config.slot          = MMC_SLOT;
	config.bus_width     = DATA_BUS_WIDTH_8BIT;
	config.max_clk_rate  = MMC_CLK_192MHZ;
	config.sdhc_base     = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base   = mmc_pwrctl_base[config.slot - 1];
	config.pwr_irq       = mmc_sdc_pwrctl_irq[config.slot - 1];
	config.hs400_support = 1;

	if (!(dev = mmc_init(&config))) {
	/* Try different config. values */
		config.max_clk_rate  = MMC_CLK_200MHZ;
		config.sdhc_base     = mmc_sdhci_base[config.slot - 1];
		config.pwrctl_base   = mmc_pwrctl_base[config.slot - 1];
		config.pwr_irq       = mmc_sdc_pwrctl_irq[config.slot - 1];
		config.hs400_support = 0;

		if (!(dev = mmc_init(&config))) {
			dprintf(CRITICAL, "mmc init failed!");
			ASSERT(0);
		}
	}
}

void *target_mmc_device()
{
	return (void *) dev;
}

/* Return 1 if vol_up pressed */
int target_volume_up()
{
	uint8_t status = 0;

	gpio_tlmm_config(TLMM_VOL_UP_BTN_GPIO, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA, GPIO_ENABLE);

	/* Wait for the gpio config to take effect - debounce time */
	thread_sleep(10);

	/* Get status of GPIO */
	status = gpio_status(TLMM_VOL_UP_BTN_GPIO);

	/* Active high signal. */
	return !status;
}

/* Return 1 if vol_down pressed */
uint32_t target_volume_down()
{
	/* Volume down button tied in with PMIC RESIN. */
	return pm8x41_resin_status();
}

uint32_t target_is_pwrkey_pon_reason()
{
	uint8_t pon_reason = pm8950_get_pon_reason();
	bool usb_present_sts = !(USBIN_UV_RT_STS &
				pm8x41_reg_read(SMBCHG_USB_RT_STS));
	if (pm8x41_get_is_cold_boot() && ((pon_reason == KPDPWR_N) || (pon_reason == (KPDPWR_N|PON1))))
		return 1;
	else if ((pon_reason == PON1) && (!usb_present_sts))
		return 1;
	else
		return 0;
}

static void target_keystatus()
{
	keys_init();

	if(target_volume_down())
		keys_post_event(KEY_VOLUMEDOWN, 1);

	if(target_volume_up())
		keys_post_event(KEY_VOLUMEUP, 1);
}

void target_init(void)
{
	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	target_keystatus();

	target_sdc_init();
	if (partition_read_table())
	{
		dprintf(CRITICAL, "Error reading the partition table info\n");
		ASSERT(0);
	}

#if LONG_PRESS_POWER_ON
	shutdown_detect();
#endif

#if PON_VIB_SUPPORT
	vib_timed_turn_on(VIBRATE_TIME);
#endif


	if (target_use_signed_kernel())
		target_crypto_init_params();

#if VERIFIED_BOOT
	if (VB_V2 == target_get_vb_version())
	{
		clock_ce_enable(CE1_INSTANCE);

		/* Initialize Qseecom */
		if (qseecom_init() < 0)
		{
			dprintf(CRITICAL, "Failed to initialize qseecom\n");
			ASSERT(0);
		}

		/* Start Qseecom */
		if (qseecom_tz_init() < 0)
		{
			dprintf(CRITICAL, "Failed to start qseecom\n");
			ASSERT(0);
		}

		if (rpmb_init() < 0)
		{
			dprintf(CRITICAL, "RPMB init failed\n");
			ASSERT(0);
		}

		/*
		 * Load the sec app for first time
		 */
		if (load_sec_app() < 0)
		{
			dprintf(CRITICAL, "Failed to load App for verified\n");
			ASSERT(0);
		}
	}
#endif

#if SMD_SUPPORT
	rpm_smd_init();
#endif
}

void target_serialno(unsigned char *buf)
{
	uint32_t serialno;
	if (target_is_emmc_boot()) {
		serialno = mmc_get_psn();
		snprintf((char *)buf, 13, "%x", serialno);
	}
}

unsigned board_machtype(void)
{
	return LINUX_MACHTYPE_UNKNOWN;
}

/* Detect the target type */
void target_detect(struct board_data *board)
{
	/* This is already filled as part of board.c */
}

/* Detect the modem type */
void target_baseband_detect(struct board_data *board)
{
	uint32_t platform;

	platform = board->platform;

	switch(platform) {
	case MSM8953:
	case SDM450:
		board->baseband = BASEBAND_MSM;
		break;
	case APQ8053:
		board->baseband = BASEBAND_APQ;
		break;
	default:
		dprintf(CRITICAL, "Platform type: %u is not supported\n",platform);
		ASSERT(0);
	};
}

unsigned target_baseband()
{
	return board_baseband();
}

int set_download_mode(enum reboot_reason mode)
{
	int ret = 0;
	ret = scm_dload_mode(mode);

	pm8x41_clear_pmic_watchdog();

	return ret;
}

int emmc_recovery_init(void)
{
	return _emmc_recovery_init();
}

unsigned target_pause_for_battery_charge(void)
{
	uint8_t pon_reason = pm8x41_get_pon_reason();
	uint8_t is_cold_boot = pm8x41_get_is_cold_boot();
	bool usb_present_sts = !(USBIN_UV_RT_STS &
				pm8x41_reg_read(SMBCHG_USB_RT_STS));
	dprintf(INFO, "%s : pon_reason is:0x%x cold_boot:%d usb_sts:%d\n", __func__,
		pon_reason, is_cold_boot, usb_present_sts);
	/* In case of fastboot reboot,adb reboot or if we see the power key
	* pressed we do not want go into charger mode.
	* fastboot reboot is warm boot with PON hard reset bit not set
	* adb reboot is a cold boot with PON hard reset bit set
	*/
	if (is_cold_boot &&
			(!(pon_reason & HARD_RST)) &&
			(!(pon_reason & KPDPWR_N)) &&
			usb_present_sts)
		return 1;
	else
		return 0;
}

void target_uninit(void)
{
	mmc_put_card_to_sleep(dev);
	sdhci_mode_disable(&dev->host);
	if (crypto_initialized())
		crypto_eng_cleanup();

	if (target_is_ssd_enabled())
		clock_ce_disable(CE1_INSTANCE);

#if VERIFIED_BOOT
	if (VB_V2 == target_get_vb_version())
	{
		if (is_sec_app_loaded())
		{
			if (send_milestone_call_to_tz() < 0)
			{
				dprintf(CRITICAL, "Failed to unload App for rpmb\n");
				ASSERT(0);
			}
		}

		if (rpmb_uninit() < 0)
		{
			dprintf(CRITICAL, "RPMB uninit failed\n");
			ASSERT(0);
		}

		clock_ce_disable(CE1_INSTANCE);
	}
#endif

#if SMD_SUPPORT
	rpm_smd_uninit();
#endif
}

/* UTMI MUX configuration to connect PHY to SNPS controller:
 * Configure primary HS phy mux to use UTMI interface
 * (connected to usb30 controller).
 */
static void tcsr_hs_phy_mux_configure(void)
{
	uint32_t reg;

	reg = readl(USB2_PHY_SEL);

	writel(reg | 0x1, USB2_PHY_SEL);
}

/* configure hs phy mux if using dwc controller */
void target_usb_phy_mux_configure(void)
{
	if(!strcmp(target_usb_controller(), "dwc"))
	{
		tcsr_hs_phy_mux_configure();
	}
}

void target_usb_phy_reset()
{

        usb30_qmp_phy_reset();
        qusb2_phy_reset();
}

/* Initialize target specific USB handlers */
target_usb_iface_t* target_usb30_init()
{
	target_usb_iface_t *t_usb_iface;

	t_usb_iface = (target_usb_iface_t *) calloc(1, sizeof(target_usb_iface_t));
	ASSERT(t_usb_iface);

	t_usb_iface->mux_config = NULL;
	t_usb_iface->phy_init   = usb30_qmp_phy_init;
	t_usb_iface->phy_reset  = target_usb_phy_reset;
	t_usb_iface->clock_init = clock_usb30_init;
	t_usb_iface->vbus_override = 1;

	return t_usb_iface;
}

/* identify the usb controller to be used for the target */
const char * target_usb_controller()
{
	return "dwc";
}

/* Do any target specific intialization needed before entering fastboot mode */
void target_fastboot_init(void)
{
	if (target_is_ssd_enabled()) {
		clock_ce_enable(CE1_INSTANCE);
		target_load_ssd_keystore();
	}
}

void target_load_ssd_keystore(void)
{
	uint64_t ptn;
	int      index;
	uint64_t size;
	uint32_t *buffer = NULL;

	if (!target_is_ssd_enabled())
		return;

	index = partition_get_index("ssd");

	ptn = partition_get_offset(index);
	if (ptn == 0){
		dprintf(CRITICAL, "Error: ssd partition not found\n");
		return;
	}

	size = partition_get_size(index);
	if (size == 0) {
		dprintf(CRITICAL, "Error: invalid ssd partition size\n");
		return;
	}

	buffer = memalign(CACHE_LINE, ROUNDUP(size, CACHE_LINE));
	if (!buffer) {
		dprintf(CRITICAL, "Error: allocating memory for ssd buffer\n");
		return;
	}

	if (mmc_read(ptn, buffer, size)) {
		dprintf(CRITICAL, "Error: cannot read data\n");
		free(buffer);
		return;
	}

	clock_ce_enable(CE1_INSTANCE);
	scm_protect_keystore(buffer, size);
	clock_ce_disable(CE1_INSTANCE);
	free(buffer);
}

crypto_engine_type board_ce_type(void)
{
	return CRYPTO_ENGINE_TYPE_HW;
}

/* Set up params for h/w CE. */
void target_crypto_init_params()
{
	struct crypto_init_params ce_params;

	/* Set up base addresses and instance. */
	ce_params.crypto_instance  = CE1_INSTANCE;
	ce_params.crypto_base      = MSM_CE1_BASE;
	ce_params.bam_base         = MSM_CE1_BAM_BASE;

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

void pmic_reset_configure(uint8_t reset_type)
{
	pm8994_reset_configure(reset_type);
}

uint32_t target_get_pmic()
{
	return PMIC_IS_PMI8950;
}

struct qmp_reg qmp_settings[] =
{
	{0x804, 0x01}, /* USB3PHY_PCIE_USB3_PCS_POWER_DOWN_CONTROL */

	/* Common block settings */
	{0xAC,  0x14}, /* QSERDES_COM_SYSCLK_EN_SEL */
	{0x34,  0x08}, /* QSERDES_COM_BIAS_EN_CLKBUFLR_EN */
	{0x174, 0x30}, /* QSERDES_COM_CLK_SELECT */
	{0x70,  0x0F}, /* USB3PHY_QSERDES_COM_BG_TRIM */
	{0x19c, 0x01}, /* QSERDES_COM_SVS_MODE_CLK_SEL */
	{0x178, 0x00}, /* QSERDES_COM_HSCLK_SEL */
	{0x194, 0x06}, /* QSERDES_COM_CMN_CONFIG */
	{0x48,  0x0F}, /* USB3PHY_QSERDES_COM_PLL_IVCO */
	{0x3C,  0x02}, /* QSERDES_COM_SYS_CLK_CTRL */

	/* PLL & Loop filter settings */
	{0xd0, 0x82}, /* QSERDES_COM_DEC_START_MODE0 */
	{0xdc, 0x55}, /* QSERDES_COM_DIV_FRAC_START1_MODE0 */
	{0xe0, 0x55}, /* QSERDES_COM_DIV_FRAC_START2_MODE0 */
	{0xe4, 0x03}, /* QSERDES_COM_DIV_FRAC_START3_MODE0 */
	{0x78, 0x0b}, /* QSERDES_COM_CP_CTRL_MODE0 */
	{0x84, 0x16}, /* QSERDES_COM_PLL_RCTRL_MODE0 */
	{0x90, 0x28}, /* QSERDES_COM_PLL_CCTRL_MODE0 */
	{0x108, 0x80}, /* QSERDES_COM_INTEGLOOP_GAIN0_MODE0 */
	{0x4c, 0x15}, /* QSERDES_COM_LOCK_CMP1_MODE0 */
	{0x50, 0x34}, /* QSERDES_COM_LOCK_CMP2_MODE0 */
	{0x54, 0x00}, /* QSERDES_COM_LOCK_CMP3_MODE0 */
	{0x18c, 0x00}, /* QSERDES_COM_CORE_CLK_EN */
	{0xcc, 0x00}, /* QSERDES_COM_LOCK_CMP_CFG */
	{0x128, 0x00}, /* QSERDES_COM_VCO_TUNE_MAP */
	{0x0C, 0x0A}, /* QSERDES_COM_BG_TIMER */

	/* SSC Settings */
	{0x10, 0x01}, /* QSERDES_COM_SSC_EN_CENTER */
	{0x1c, 0x31}, /* QSERDES_COM_SSC_PER1 */
	{0x20, 0x01}, /* QSERDES_COM_SSC_PER2 */
	{0x14, 0x00}, /* QSERDES_COM_SSC_ADJ_PER1 */
	{0x18, 0x00}, /* QSERDES_COM_SSC_ADJ_PER2 */
	{0x24, 0xde}, /* QSERDES_COM_SSC_STEP_SIZE1 */
	{0x28, 0x07}, /* QSERDES_COM_SSC_STEP_SIZE2 */

	/* Rx Settings */
	{0x41C, 0x06}, /* QSERDES_RX_UCDR_SO_GAIN */
	{0x4d8, 0x02}, /* QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2 */
	{0x4dc, 0x4c}, /* QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3 */
	{0x4e0, 0xb8}, /* QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4 */
	{0x508, 0x77}, /* QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1 */
	{0x50c, 0x80}, /* QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2 */
	{0x514, 0x03}, /* QSERDES_RX_SIGDET_CNTRL */
	{0x51c, 0x16}, /* QSERDES_RX_SIGDET_DEGLITCH_CNTRL */
	{0x510, 0x0C}, /* QSERDES_RX_SIGDET_ENABLES */

	/* Tx settings */
	{0x268, 0x45}, /* QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN */
	{0x2ac, 0x12}, /* QSERDES_TX_RCV_DETECT_LVL_2 */
	{0x294, 0x06}, /* QSERDES_TX_LANE_MODE */
	{0x824, 0x15}, /* PCIE_USB3_PCS_TXDEEMPH_M6DB_V0 */
	{0x828, 0x0E}, /* PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0 */

	/* FLL settings */
	{0x8c8, 0x83}, /* PCIE_USB3_PCS_FLL_CNTRL2 */
	{0x8c4, 0x02}, /* PCIE_USB3_PCS_FLL_CNTRL1 */
	{0x8cc, 0x09}, /* PCIE_USB3_PCS_FLL_CNT_VAL_L */
	{0x8D0, 0xA2}, /* PCIE_USB3_PCS_FLL_CNT_VAL_H_TOL */
	{0x8D4, 0x85}, /* PCIE_USB3_PCS_FLL_MAN_CODE */

	/* PCS Settings */
	{0x880, 0xD1}, /* PCIE_USB3_PCS_LOCK_DETECT_CONFIG1 */
	{0x884, 0x1F}, /* PCIE_USB3_PCS_LOCK_DETECT_CONFIG2 */
	{0x888, 0x47}, /* PCIE_USB3_PCS_LOCK_DETECT_CONFIG3 */
	{0x864, 0x1B}, /* PCIE_USB3_PCS_POWER_STATE_CONFIG2 */
	{0x8B8, 0x75}, /* PCIE_USB3_PCS_RXEQTRAINING_WAIT_TIME */
	{0x8BC, 0x13}, /* PCIE_USB3_PCS_RXEQTRAINING_RUN_TIME */
	{0x8B0, 0x86}, /* PCIE_USB3_PCS_LFPS_TX_ECSTART_EQTLOCK */
	{0x8A0, 0x04}, /* PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK */
	{0x88C, 0x44}, /* PCIE_USB3_PCS_TSYNC_RSYNC_TIME */
	{0x870, 0xE7}, /* PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_L */
	{0x874, 0x03}, /* PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_H */
	{0x878, 0x40}, /* PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_L */
	{0x87c, 0x00}, /* PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_H */
	{0x9D8, 0x88}, /* PCIE_USB3_PCS_RX_SIGDET_LVL */
	{0x808, 0x03}, /* PCIE_USB3_PCS_START_CONTROL */
	{0x800, 0x00}, /* PCIE_USB3_PCS_SW_RESET */
};

struct qmp_reg *target_get_qmp_settings()
{
	return qmp_settings;
}

int target_get_qmp_regsize()
{
	return ARRAY_SIZE(qmp_settings);
}
static uint8_t splash_override;
/* Returns 1 if target supports continuous splash screen. */
int target_cont_splash_screen()
{
	uint8_t splash_screen = 0;
	if (!splash_override) {
		switch (board_hardware_id()) {
		case HW_PLATFORM_MTP:
		case HW_PLATFORM_SURF:
		case HW_PLATFORM_RCM:
		case HW_PLATFORM_QRD:
			splash_screen = 1;
			break;
		default:
			splash_screen = 0;
			break;
		}
		dprintf(SPEW, "Target_cont_splash=%d\n", splash_screen);
	}
	return splash_screen;
}

void target_force_cont_splash_disable(uint8_t override)
{
        splash_override = override;
}
