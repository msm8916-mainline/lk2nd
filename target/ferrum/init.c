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
#include <platform/iomap.h>
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <uart_dm.h>
#include <mmc.h>
#include <dev/keys.h>
#include <spmi_v2.h>
#include <pm8x41.h>
#include <board.h>
#include <baseband.h>
#include <hsusb.h>
#include <scm.h>
#include <platform/gpio.h>
#include <platform/irqs.h>
#include <platform/clock.h>
#include <crypto5_wrapper.h>
#include <partition_parser.h>
#include <stdlib.h>
#include <gpio.h>

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0


struct mmc_device *dev;

static uint32_t mmc_pwrctl_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE };

static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE, MSM_SDC2_SDHCI_BASE };

static uint32_t  mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ, SDCC2_PWRCTL_IRQ };

static void set_sdc_power_ctrl(void);

void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(1, 0, BLSP1_UART1_BASE);
#endif
}

void target_sdc_init()
{
	struct mmc_config_data config;

	/* Set drive strength & pull ctrl values */
	set_sdc_power_ctrl();

	config.bus_width = DATA_BUS_WIDTH_8BIT;
	config.max_clk_rate = MMC_CLK_177MHZ;

	/* Try slot 1*/
	config.slot         = 1;
	config.sdhc_base    = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base  = mmc_pwrctl_base[config.slot - 1];
	config.pwr_irq      = mmc_sdc_pwrctl_irq[config.slot - 1];
	config.hs400_support = 0;

	if (!(dev = mmc_init(&config))) {
	/* Try slot 2 */
		config.slot         = 2;
		config.max_clk_rate = MMC_CLK_200MHZ;
		config.sdhc_base    = mmc_sdhci_base[config.slot - 1];
		config.pwrctl_base  = mmc_pwrctl_base[config.slot - 1];
		config.pwr_irq      = mmc_sdc_pwrctl_irq[config.slot - 1];

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

static void target_keystatus()
{
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

void target_init(void)
{
	uint32_t base_addr;
	uint8_t slot;

	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	target_keystatus();

	target_sdc_init();

	if (partition_read_table())
	{
		dprintf(CRITICAL, "Error reading the partition table info\n");
		ASSERT(0);
	}

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
}

