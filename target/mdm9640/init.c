/* Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
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
#include <malloc.h>
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
#include <rpm-smd.h>
#include <scm.h>
#include <spmi.h>
#include <partition_parser.h>
#include <sdhci_msm.h>
#include <uart_dm.h>
#include <boot_device.h>
#include <qmp_phy.h>
#include <crypto5_wrapper.h>
#include <rpm-glink.h>

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

#define EXT4_CMDLINE  " rootwait rootfstype=ext4 root=/dev/mmcblk0p"
#define UBI_CMDLINE " rootfstype=ubifs rootflags=bulk_read"

#define CE1_INSTANCE            1
#define CE_EE                   1
#define CE_FIFO_SIZE            64
#define CE_READ_PIPE            3
#define CE_WRITE_PIPE           2
#define CE_READ_PIPE_LOCK_GRP   0
#define CE_WRITE_PIPE_LOCK_GRP  0
#define CE_ARRAY_SIZE           20
#define SUB_TYPE_SKUT           0x0A

struct qpic_nand_init_config config;

void update_ptable_names(void)
{
	uint32_t ptn_index;
	struct ptentry *ptentry_ptr = flash_ptable.parts;
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

#if ENABLE_EARLY_ETHERNET
void toggle_neutrino(void)
{
	struct pm8x41_gpio gpio = {
	.direction = PM_GPIO_DIR_OUT,
	.function = PM_GPIO_FUNC_HIGH,
	.vin_sel = 1,   /* VIN_1 */
	.output_buffer = PM_GPIO_OUT_CMOS,
	.out_strength = PM_GPIO_OUT_DRIVE_LOW,
	};

	pm8x41_gpio_config(4, &gpio);
	pm8x41_gpio_set(4, 1);
	mdelay(10);
	pm8x41_gpio_set(4, 0);
}
#endif

/* init */
void target_init(void)
{
	dprintf(INFO, "target_init()\n");

	pmic_info_populate();

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);
	if(!platform_is_sdx20())
	{
		rpm_smd_init();
	}
	else
	{
		/* Initialize Glink */
		rpm_glink_init();
	}

#if ENABLE_EARLY_ETHERNET
	/*enable pmic gpio 4*/
	toggle_neutrino();
#endif
	if (platform_boot_dev_isemmc()) {
		target_sdc_init();
		if (partition_read_table()) {
			dprintf(CRITICAL, "Error reading the partition table info\n");
			ASSERT(0);
		}
		/* Below setting is to enable EBI2 function selection in TLMM so
		   that GPIOs can be used for display */
		writel((readl(TLMM_EBI2_EMMC_GPIO_CFG) | EBI2_BOOT_SELECT), TLMM_EBI2_EMMC_GPIO_CFG);
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

	if (target_use_signed_kernel())
		target_crypto_init_params();
}

static int scm_clear_boot_partition_select()
{
	int ret = 0;

	ret = scm_call_atomic2(SCM_SVC_BOOT, WDOG_DEBUG_DISABLE, 1, 0);
	if (ret)
		dprintf(CRITICAL, "Failed to disable the wdog debug \n");

	return ret;
}

/* Trigger reboot */
void reboot_device(unsigned reboot_reason)
{
	uint8_t reset_type = 0;

	if (platform_is_mdm9650() || platform_is_sdx20())
	{
		/* Clear the boot partition select cookie to indicate
		 * its a normal reset and avoid going to download mode */
		scm_clear_boot_partition_select();
	}

	/* Write the reboot reason */
	writel(reboot_reason, RESTART_REASON_ADDR);

	if(reboot_reason)
		reset_type = PON_PSHOLD_WARM_RESET;
	else
		reset_type = PON_PSHOLD_HARD_RESET;

	if (platform_is_mdm9650() || platform_is_sdx20())
	{
		/* PMD9655 is the PMIC used for MDM9650 */
		pm8x41_reset_configure(reset_type);
	} else {
		/* Configure PMIC for warm reset */
		/* PM 8019 v1 aligns with PM8941 v2.
		 * This call should be based on the pmic version
		 * when PM8019 v2 is available.
		 */
		pm8x41_v2_reset_configure(reset_type);
	}

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

void target_serialno(unsigned char *buf)
{
	uint32_t serialno;
	serialno = board_chip_serial();
	snprintf((char *)buf, 13, "%x", serialno);
}

unsigned check_reboot_mode(void)
{
	unsigned restart_reason = 0;

	/* Read reboot reason and scrub it */
	restart_reason = readl(RESTART_REASON_ADDR);

	writel(0x00, RESTART_REASON_ADDR);

	return restart_reason;
}

int get_target_boot_params(const char *cmdline, const char *part, char **buf)
{
	struct ptable *ptable;
	int system_ptn_index = -1;
	uint32_t buflen;
	int ret = -1;

	if (!cmdline || !part ) {
		dprintf(CRITICAL, "WARN: Invalid input param\n");
		return -1;
	}

	if (!strstr(cmdline, "root=/dev/ram")) /* This check is to handle kdev boot */
	{
		if (!target_is_emmc_boot()) {

			buflen = strlen(UBI_CMDLINE) + strlen(" root=ubi0:rootfs ubi.mtd=") + sizeof(int) + 1;
			*buf = (char *)malloc(buflen);
			if(!(*buf)) {
				dprintf(CRITICAL,"Unable to allocate memory for boot params\n");
				return -1;
			}
			/* Below is for NAND boot */
			ptable = flash_get_ptable();
			if (!ptable) {
				dprintf(CRITICAL,
						"WARN: Cannot get flash partition table\n");
				free(*buf);
				return -1;
			}

			system_ptn_index = ptable_get_index(ptable, part);
			if (system_ptn_index < 0) {
				dprintf(CRITICAL,
					"WARN: Cannot get partition index for %s\n", part);
				free(*buf);
				return -1;
			}
			/* Adding command line parameters according to target boot type */
			snprintf(*buf, buflen, UBI_CMDLINE);
			snprintf(*buf+strlen(*buf), buflen, " root=ubi0:rootfs ubi.mtd=%d", system_ptn_index);
			ret = 0;
		}
		else {
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

const char * target_usb_controller()
{
	return "dwc";
}

static void set_sdc_power_ctrl()
{
	/* Drive strength configs for sdc pins */
	struct tlmm_cfgs sdc1_hdrv_cfg[] =
	{
		{ SDC1_CLK_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, 0 },
		{ SDC1_CMD_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, 0 },
		{ SDC1_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_6MA, TLMM_HDRV_MASK, 0 },
	};

	/* Pull configs for sdc pins */
	struct tlmm_cfgs sdc1_pull_cfg[] =
	{
		{ SDC1_CLK_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK, 0 },
		{ SDC1_CMD_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, 0 },
		{ SDC1_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK, 0 },
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
	config.bus_width = DATA_BUS_WIDTH_8BIT;
	config.sdhc_base    = MSM_SDC1_SDHCI_BASE;
	config.pwrctl_base  = MSM_SDC1_BASE;
	config.pwr_irq      = SDCC1_PWRCTL_IRQ;
	config.hs400_support = 0;
	config.hs200_support = 0;
	config.use_io_switch = 1;
	if (platform_is_sdx20())
		config.max_clk_rate = MMC_CLK_200MHZ;
	else
		config.max_clk_rate = MMC_CLK_171MHZ;

	if (!(dev = mmc_init(&config))) {
		dprintf(CRITICAL, "mmc init failed!");
		ASSERT(0);
	}
}

int target_cont_splash_screen()
{
	/* FOR OEMs - Set cont_splash_screen to keep the splash enable after LK.*/
	return false;
}

void target_uninit(void)
{
	if (platform_boot_dev_isemmc())
	{
		mmc_put_card_to_sleep(dev);
		sdhci_mode_disable(&dev->host);
	}

	if (crypto_initialized())
		crypto_eng_cleanup();

	if(!platform_is_sdx20())
	{
		rpm_smd_uninit();
	}
	else
	{
		/* Tear down glink channels */
		rpm_glink_uninit();
	}
}
void target_mux_configure(void)
{
	uint32_t val;
	//USB30_GENERAL_CFG_PIPE_UTMI_CLK_DIS
	val = readl(USB30_GENERAL_CFG_PIPE);
	val = val | 0x100;
	writel(val, USB30_GENERAL_CFG_PIPE);
	udelay(100);

	//USB30_GENERAL_CFG_PIPE_UTMI_CLK_SEL
	val = readl(USB30_GENERAL_CFG_PIPE);
	val = val | 0x1;
	writel(val, USB30_GENERAL_CFG_PIPE);
	udelay(100);

	//USB30_GENERAL_CFG_PIPE3_PHYSTATUS_SW
	val = readl(USB30_GENERAL_CFG_PIPE);
	val = val | 0x8;
	writel(val, USB30_GENERAL_CFG_PIPE);
	udelay(100);

	//USB30_GENERAL_CFG_PIPE_UTMI_CLK_ENABLE
	val = readl(USB30_GENERAL_CFG_PIPE);
	val = val & 0xfffffeff;
	writel(val, USB30_GENERAL_CFG_PIPE);
	udelay(100);
}

void target_usb_phy_reset(void)
{
	/* Reset sequence for 9650 is different from 9x40, use the reset sequence
	 * from clock driver
	 */
	if (platform_is_mdm9650() || platform_is_sdx20())
		clock_reset_usb_phy(); // This is the reset function for USB3
	else
		usb30_qmp_phy_reset();

	qusb2_phy_reset();
}

target_usb_iface_t* target_usb30_init()
{
	target_usb_iface_t *t_usb_iface;

	t_usb_iface = (target_usb_iface_t *) calloc(1, sizeof(target_usb_iface_t));
	ASSERT(t_usb_iface);

	t_usb_iface->mux_config = NULL;
	if (platform_is_sdx20()){
		t_usb_iface->mux_config = target_mux_configure;
		t_usb_iface->phy_init   = NULL;
	}
	else
		t_usb_iface->phy_init   = usb30_qmp_phy_init;
	t_usb_iface->phy_reset  = target_usb_phy_reset;
	t_usb_iface->clock_init = clock_usb30_init;
	t_usb_iface->vbus_override = 1;

	return t_usb_iface;
}

uint32_t target_override_pll()
{
	if (platform_is_mdm9650() || platform_is_sdx20())
		return 0;
	else
		return 1;
}

uint32_t target_get_hlos_subtype()
{
	return board_hlos_subtype();
}

/* QMP settings are different from 9650 when compared to v2.0/v1.0 hardware.
 * Use the QMP settings from target code to keep the common driver clean
 */
struct qmp_reg qmp_settings[] =
{
	{0x804, 0x01}, /*USB3PHY_PCIE_USB3_PCS_POWER_DOWN_CONTROL */
	{0xAC, 0x14}, /* QSERDES_COM_SYSCLK_EN_SEL */
	{0x34, 0x08}, /* QSERDES_COM_BIAS_EN_CLKBUFLR_EN */
	{0x174, 0x30}, /* QSERDES_COM_CLK_SELECT */
	{0x3C, 0x06}, /* QSERDES_COM_SYS_CLK_CTRL */
	{0xB4, 0x00}, /* QSERDES_COM_RESETSM_CNTRL */
	{0xB8, 0x08}, /* QSERDES_COM_RESETSM_CNTRL2 */
	{0x194, 0x06}, /* QSERDES_COM_CMN_CONFIG */
	{0x19c, 0x01}, /* QSERDES_COM_SVS_MODE_CLK_SEL */
	{0x178, 0x00}, /* QSERDES_COM_HSCLK_SEL */
	{0xd0, 0x82}, /* QSERDES_COM_DEC_START_MODE0 */
	{0xdc, 0x55}, /* QSERDES_COM_DIV_FRAC_START1_MODE0 */
	{0xe0, 0x55}, /* QSERDES_COM_DIV_FRAC_START2_MODE0 */
	{0xe4, 0x03}, /* QSERDES_COM_DIV_FRAC_START3_MODE0 */
	{0x78, 0x0b}, /* QSERDES_COM_CP_CTRL_MODE0 */
	{0x84, 0x16}, /* QSERDES_COM_PLL_RCTRL_MODE0 */
	{0x90, 0x28}, /* QSERDES_COM_PLL_CCTRL_MODE0 */
	{0x108, 0x80}, /* QSERDES_COM_INTEGLOOP_GAIN0_MODE0 */
	{0x10C, 0x00}, /* QSERDES_COM_INTEGLOOP_GAIN1_MODE0 */
	{0x184, 0x0A}, /* QSERDES_COM_CORECLK_DIV */
	{0x4c, 0x15}, /* QSERDES_COM_LOCK_CMP1_MODE0 */
	{0x50, 0x34}, /* QSERDES_COM_LOCK_CMP2_MODE0 */
	{0x54, 0x00}, /* QSERDES_COM_LOCK_CMP3_MODE0 */
	{0xC8, 0x00}, /* QSERDES_COM_LOCK_CMP_EN */
	{0x18c, 0x00}, /* QSERDES_COM_CORE_CLK_EN */
	{0xcc, 0x00}, /* QSERDES_COM_LOCK_CMP_CFG */
	{0x128, 0x00}, /* QSERDES_COM_VCO_TUNE_MAP */
	{0x0C, 0x0A}, /* QSERDES_COM_BG_TIMER */
	{0x10, 0x01}, /* QSERDES_COM_SSC_EN_CENTER */
	{0x1c, 0x31}, /* QSERDES_COM_SSC_PER1 */
	{0x20, 0x01}, /* QSERDES_COM_SSC_PER2 */
	{0x14, 0x00}, /* QSERDES_COM_SSC_ADJ_PER1 */
	{0x18, 0x00}, /* QSERDES_COM_SSC_ADJ_PER2 */
	{0x24, 0xde}, /* QSERDES_COM_SSC_STEP_SIZE1 */
	{0x28, 0x07}, /* QSERDES_COM_SSC_STEP_SIZE2 */
	{0x48, 0x0F}, /* USB3PHY_QSERDES_COM_PLL_IVCO */
	{0x70, 0x0F}, /* USB3PHY_QSERDES_COM_BG_TRIM */
	{0x100, 0x80}, /* QSERDES_COM_INTEGLOOP_INITVAL */

	/* Rx Settings */
	{0x440, 0x0b}, /* QSERDES_RX_UCDR_FASTLOCK_FO_GAIN */
	{0x4d8, 0x02}, /* QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2 */
	{0x4dc, 0x6c}, /* QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3 */
	{0x4e0, 0xbb}, /* QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4 */
	{0x508, 0x77}, /* QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1 */
	{0x50c, 0x80}, /* QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2 */
	{0x514, 0x03}, /* QSERDES_RX_SIGDET_CNTRL */
	{0x51c, 0x16}, /* QSERDES_RX_SIGDET_DEGLITCH_CNTRL */
	{0x448, 0x75}, /* QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE */
	{0x450, 0x00}, /* QSERDES_RX_UCDR_FASTLOCK_COUNT_LOW */
	{0x454, 0x00}, /* QSERDES_RX_UCDR_FASTLOCK_COUNT_HIGH */
	{0x40C, 0x0a}, /* QSERDES_RX_UCDR_FO_GAIN */
	{0x41C, 0x06}, /* QSERDES_RX_UCDR_SO_GAIN */
	{0x510, 0x00}, /*QSERDES_RX_SIGDET_ENABLES */

	/* Tx settings */
	{0x268, 0x45}, /* QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN */
	{0x2ac, 0x12}, /* QSERDES_TX_RCV_DETECT_LVL_2 */
	{0x294, 0x06}, /* QSERDES_TX_LANE_MODE */
	{0x254, 0x00}, /* QSERDES_TX_RES_CODE_LANE_OFFSET */

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
	{0x80C, 0x9F}, /* PCIE_USB3_PCS_TXMGN_V0 */
	{0x824, 0x17}, /* PCIE_USB3_PCS_TXDEEMPH_M6DB_V0 */
	{0x828, 0x0F}, /* PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0 */
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
	if (platform_is_mdm9650() || platform_is_sdx20())
		return qmp_settings;
	else
		return NULL;
}

int target_get_qmp_regsize()
{
	if (platform_is_mdm9650() || platform_is_sdx20())
		return ARRAY_SIZE(qmp_settings);
	else
		return 0;
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
