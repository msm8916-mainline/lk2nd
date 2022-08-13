/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include <dev/keys.h>
#include <dev/ssbi.h>
#include <dev/gpio.h>
#include <lib/ptable.h>
#include <dev/flash.h>
#include <smem.h>
#include <mmc.h>
#include <platform/timer.h>
#include <platform/iomap.h>
#include <platform/gpio.h>
#include <baseband.h>
#include <reg.h>
#include <platform.h>
#include <gsbi.h>
#include <platform/scm-io.h>
#include <platform/machtype.h>
#include <crypto_hash.h>
#include <board.h>
#include <target.h>

static const uint8_t uart_gsbi_id = GSBI_ID_12;

/* Setting this variable to different values defines the
 * behavior of CE engine:
 * platform_ce_type = CRYPTO_ENGINE_TYPE_NONE : No CE engine
 * platform_ce_type = CRYPTO_ENGINE_TYPE_SW : Software CE engine
 * platform_ce_type = CRYPTO_ENGINE_TYPE_HW : Hardware CE engine
 * Behavior is determined in the target code.
 */
static crypto_engine_type platform_ce_type = CRYPTO_ENGINE_TYPE_SW;

void keypad_init(void);
extern void dmb(void);

int target_is_emmc_boot(void);
static void debug_led_write(char);
static char debug_led_read();
uint32_t platform_id_read(void);
static void setup_fpga(void);
int pm8901_reset_pwr_off(int reset);
int pm8058_reset_pwr_off(int reset);
int pm8058_rtc0_alarm_irq_disable(void);
static void target_shutdown_for_rtc_alarm(void);
void target_init(void)
{
	target_shutdown_for_rtc_alarm();
	dprintf(INFO, "target_init()\n");

	setup_fpga();

	/* Setting Debug LEDs ON */
	debug_led_write(0xFF);
#if (!ENABLE_NANDWRITE)
	keys_init();
	keypad_init();
#endif

	switch (board_platform_id()) {
		case APQ8060:
		case MSM8660:
		case MSM8260:
			platform_ce_type = CRYPTO_ENGINE_TYPE_HW;
			break;
	}

	if (mmc_boot_main(MMC_SLOT, MSM_SDC1_BASE)) {
		dprintf(CRITICAL, "mmc init failed!");
		ASSERT(0);
	}
}

unsigned board_machtype(void)
{
	return board_target_id();
}

/* Detect the target type */
void target_detect(struct board_data *board)
{
	struct smem_board_info_v5 board_info_v5;
	struct smem_board_info_v6 board_info_v6;
	unsigned int board_info_len = 0;
	unsigned smem_status = 0;
	unsigned format = 0;
	unsigned id = 0;
	unsigned hw_platform = 0;
	unsigned fused_chip = 0;
	unsigned platform_subtype = 0;
	unsigned mach_id = 0xFFFFFFFF;

	/* Detect external msm if this is a "fusion" */
	smem_status = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
						   &format, sizeof(format), 0);
	if (!smem_status) {
		if (format == 5) {
			board_info_len = sizeof(board_info_v5);

			smem_status =
			    smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
						  &board_info_v5,
						  board_info_len);
			if (!smem_status) {
				fused_chip = board_info_v5.fused_chip;
				id = board_info_v5.board_info_v3.hw_platform;
			}
		} else if (format == 6) {
			board_info_len = sizeof(board_info_v6);

			smem_status =
			    smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
						  &board_info_v6,
						  board_info_len);
			if (!smem_status) {
				fused_chip = board_info_v6.fused_chip;
				id = board_info_v6.board_info_v3.hw_platform;
				platform_subtype =
				    board_info_v6.platform_subtype;
			}
		}
	}

	/* Detect SURF v/s FFA v/s Fluid */
	switch (id) {
	case 0x1:
		hw_platform = HW_PLATFORM_SURF;
		break;
	case 0x2:
		hw_platform = HW_PLATFORM_FFA;
		break;
	case 0x3:
		hw_platform = HW_PLATFORM_FLUID;
		break;
	case 0x6:
		hw_platform = HW_PLATFORM_QT;
		break;
	case 0xA:
		hw_platform = HW_PLATFORM_DRAGON;
		break;
	default:
		/* Writing to Debug LED register and reading back to auto detect
		   SURF and FFA. If we read back, it is SURF */
		debug_led_write(0xA5);

		if ((debug_led_read() & 0xFF) == 0xA5) {
			debug_led_write(0);
			hw_platform = HW_PLATFORM_SURF;
		} else
			hw_platform = HW_PLATFORM_FFA;
	};

	/* Use platform_subtype or fused_chip information to determine machine id */
	if (format >= 6) {
		switch (platform_subtype) {
		case HW_PLATFORM_SUBTYPE_CSFB:
		case HW_PLATFORM_SUBTYPE_SVLTE2A:
			if (hw_platform == HW_PLATFORM_SURF)
				mach_id = LINUX_MACHTYPE_8660_CHARM_SURF;
			else if (hw_platform == HW_PLATFORM_FFA)
				mach_id = LINUX_MACHTYPE_8660_CHARM_FFA;
			break;
		default:
			if (hw_platform == HW_PLATFORM_SURF)
				mach_id = LINUX_MACHTYPE_8660_SURF;
			else if (hw_platform == HW_PLATFORM_FFA)
				mach_id = LINUX_MACHTYPE_8660_FFA;
			else if (hw_platform == HW_PLATFORM_FLUID)
				mach_id = LINUX_MACHTYPE_8660_FLUID;
			else if (hw_platform == HW_PLATFORM_QT)
				mach_id = LINUX_MACHTYPE_8660_QT;
			else if (hw_platform == HW_PLATFORM_DRAGON)
				mach_id = LINUX_MACHTYPE_8x60_DRAGON;
		}
	} else if (format == 5) {
		switch (fused_chip) {
		case UNKNOWN:
			if (hw_platform == HW_PLATFORM_SURF)
				mach_id = LINUX_MACHTYPE_8660_SURF;
			else if (hw_platform == HW_PLATFORM_FFA)
				mach_id = LINUX_MACHTYPE_8660_FFA;
			else if (hw_platform == HW_PLATFORM_FLUID)
				mach_id = LINUX_MACHTYPE_8660_FLUID;
			else if (hw_platform == HW_PLATFORM_QT)
				mach_id = LINUX_MACHTYPE_8660_QT;
			else if (hw_platform == HW_PLATFORM_DRAGON)
				mach_id = LINUX_MACHTYPE_8x60_DRAGON;
			break;

		case MDM9200:
		case MDM9600:
			if (hw_platform == HW_PLATFORM_SURF)
				mach_id = LINUX_MACHTYPE_8660_CHARM_SURF;
			else if (hw_platform == HW_PLATFORM_FFA)
				mach_id = LINUX_MACHTYPE_8660_CHARM_FFA;
			break;

		default:
			mach_id = LINUX_MACHTYPE_8660_FFA;
		}
	}
	board->target = mach_id;
}

void shutdown_device()
{
	gpio_config_pshold();
	pm8058_reset_pwr_off(0);
	pm8901_reset_pwr_off(0);

	writel(0, MSM_PSHOLD_CTL_SU);
	mdelay(10000);
	dprintf(CRITICAL, "Shutdown failed\n");
}

void reboot_device(unsigned reboot_reason)
{
	/* Reset WDG0 counter */
	writel(1, MSM_WDT0_RST);
	/* Disable WDG0 */
	writel(0, MSM_WDT0_EN);
	/* Set WDG0 bark time */
	writel(0x31F3, MSM_WDT0_BT);
	/* Enable WDG0 */
	writel(3, MSM_WDT0_EN);
	dmb();
	/* Enable WDG output */
	secure_writel(3, MSM_TCSR_BASE + TCSR_WDOG_CFG);
	mdelay(10000);
	dprintf(CRITICAL, "Rebooting failed\n");
	return;
}

unsigned check_reboot_mode(void)
{
	unsigned restart_reason = 0;
	void *restart_reason_addr = (void *)0x2A05F65C;

	/* Read reboot reason and scrub it */
	restart_reason = readl(restart_reason_addr);
	writel(0x00, restart_reason_addr);

	return restart_reason;
}

void target_battery_charging_enable(unsigned enable, unsigned disconnect)
{
}

void setup_fpga()
{
#if TARGET_MSM8660_SURF
	writel(0x147, GPIO_CFG133_ADDR);
	writel(0x144, GPIO_CFG135_ADDR);
	writel(0x144, GPIO_CFG136_ADDR);
	writel(0x144, GPIO_CFG137_ADDR);
	writel(0x144, GPIO_CFG138_ADDR);
	writel(0x144, GPIO_CFG139_ADDR);
	writel(0x144, GPIO_CFG140_ADDR);
	writel(0x144, GPIO_CFG141_ADDR);
	writel(0x144, GPIO_CFG142_ADDR);
	writel(0x144, GPIO_CFG143_ADDR);
	writel(0x144, GPIO_CFG144_ADDR);
	writel(0x144, GPIO_CFG145_ADDR);
	writel(0x144, GPIO_CFG146_ADDR);
	writel(0x144, GPIO_CFG147_ADDR);
	writel(0x144, GPIO_CFG148_ADDR);
	writel(0x144, GPIO_CFG149_ADDR);
	writel(0x144, GPIO_CFG150_ADDR);
	writel(0x147, GPIO_CFG151_ADDR);
	writel(0x147, GPIO_CFG152_ADDR);
	writel(0x147, GPIO_CFG153_ADDR);
	writel(0x3, GPIO_CFG154_ADDR);
	writel(0x147, GPIO_CFG155_ADDR);
	writel(0x147, GPIO_CFG156_ADDR);
	writel(0x147, GPIO_CFG157_ADDR);
	writel(0x3, GPIO_CFG158_ADDR);

	writel(0x00000B31, EBI2_CHIP_SELECT_CFG0);
	writel(0xA3030020, EBI2_XMEM_CS3_CFG1);
#endif
}

void debug_led_write(char val)
{
#if TARGET_MSM8660_SURF
	writeb(val, SURF_DEBUG_LED_ADDR);
#endif
}

char debug_led_read()
{
#if TARGET_MSM8660_SURF
	return readb(SURF_DEBUG_LED_ADDR);
#else
	return 0;
#endif
}

unsigned target_baseband()
{
	return board_baseband();
}

/* Detect the modem type */
void target_baseband_detect(struct board_data *board)
{
	struct smem_board_info_v5 board_info_v5;
	struct smem_board_info_v6 board_info_v6;
	unsigned int board_info_len = 0;
	unsigned smem_status = 0;
	unsigned format = 0;
	unsigned baseband = BASEBAND_MSM;

	smem_status = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
						   &format, sizeof(format), 0);
	if (!smem_status) {
		if (format == 5) {
			board_info_len = sizeof(board_info_v5);

			smem_status =
			    smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
						  &board_info_v5,
						  board_info_len);
			if (!smem_status) {
				/* Check for LTE fused targets or APQ.  Default to MSM */
				if (board_info_v5.fused_chip == MDM9200)
					baseband = BASEBAND_CSFB;
				else if (board_info_v5.fused_chip == MDM9600)
					baseband = BASEBAND_SVLTE2A;
				else if (board_info_v5.board_info_v3.msm_id ==
					 APQ8060)
					baseband = BASEBAND_APQ;
				else
					baseband = BASEBAND_MSM;
			}
		} else if (format >= 6) {
			board_info_len = sizeof(board_info_v6);

			smem_status =
			    smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
						  &board_info_v6,
						  board_info_len);
			if (!smem_status) {
				/* Check for LTE fused targets or APQ.  Default to MSM */
				if (board_info_v6.platform_subtype ==
				    HW_PLATFORM_SUBTYPE_CSFB)
					baseband = BASEBAND_CSFB;
				else if (board_info_v6.platform_subtype ==
					 HW_PLATFORM_SUBTYPE_SVLTE2A)
					baseband = BASEBAND_SVLTE2A;
				else if (board_info_v6.board_info_v3.msm_id ==
					 APQ8060)
					baseband = BASEBAND_APQ;
				else
					baseband = BASEBAND_MSM;
			}
		}
	}
	board->baseband = baseband;
}

crypto_engine_type board_ce_type(void)
{
	return platform_ce_type;
}

static unsigned target_check_power_on_reason(void)
{
	unsigned power_on_status = 0;
	unsigned int status_len = sizeof(power_on_status);
	unsigned smem_status;

	smem_status = smem_read_alloc_entry(SMEM_POWER_ON_STATUS_INFO,
					    &power_on_status, status_len);

	if (smem_status) {
		dprintf(CRITICAL,
			"ERROR: unable to read shared memory for power on reason\n");
	}
	dprintf(INFO, "Power on reason %u\n", power_on_status);
	return power_on_status;
}

static void target_shutdown_for_rtc_alarm(void)
{
	if (target_check_power_on_reason() == PWR_ON_EVENT_RTC_ALARM) {
		dprintf(CRITICAL,
			"Power on due to RTC alarm. Going to shutdown!!\n");
		pm8058_rtc0_alarm_irq_disable();
		shutdown_device();
	}
}

unsigned target_pause_for_battery_charge(void)
{
	if (target_check_power_on_reason() == PWR_ON_EVENT_WALL_CHG)
		return 1;

	return 0;
}

void target_serialno(unsigned char *buf)
{
	unsigned int serialno;
	if (target_is_emmc_boot()) {
		serialno = mmc_get_psn();
		snprintf((char *)buf, 13, "%x", serialno);
	}
}

static void hsusb_gpio_init(void)
{
#if TARGET_MSM8660_SURF
	uint32_t func;
	uint32_t pull;
	uint32_t dir;
	uint32_t enable;
	uint32_t drv;

	/* GPIO 131 and 132 need to be configured for connecting to USB HS PHY */

	func = 0;
	enable = 1;
	pull = GPIO_NO_PULL;
	dir = 2;
	drv = GPIO_2MA;
	gpio_tlmm_config(131, func, dir, pull, drv, enable);
	gpio_set(131, dir);

	func = 0;
	enable = 1;
	pull = GPIO_NO_PULL;
	dir = 1;
	drv = GPIO_2MA;
	gpio_tlmm_config(132, func, dir, pull, drv, enable);
	gpio_set(132, dir);

	return;
#endif
}

#define USB_CLK             0x00902910
#define USB_PHY_CLK         0x00902E20
#define CLK_RESET_ASSERT    0x1
#define CLK_RESET_DEASSERT  0x0
#define CLK_RESET(x,y)  writel((y), (x));

static int msm_otg_xceiv_reset()
{
	CLK_RESET(USB_CLK, CLK_RESET_ASSERT);
	CLK_RESET(USB_PHY_CLK, CLK_RESET_ASSERT);
	mdelay(20);
	CLK_RESET(USB_PHY_CLK, CLK_RESET_DEASSERT);
	CLK_RESET(USB_CLK, CLK_RESET_DEASSERT);
	mdelay(20);

	return 0;
}

static void target_ulpi_init(void)
{
	unsigned int reg;

	reg = ulpi_read(0x32);
	dprintf(INFO, " Value of ulpi read 0x32 is %08x\n", reg);
	ulpi_write(0x30, 0x32);
	reg = ulpi_read(0x32);
	dprintf(INFO, " Value of ulpi read 0x32 after write is %08x\n", reg);

	reg = ulpi_read(0x36);
	dprintf(INFO, " Value of ulpi read 0x36 is %08x\n", reg);
	ulpi_write(reg | 0x2, 0x36);
	reg = ulpi_read(0x36);
	dprintf(INFO, " Value of ulpi read 0x36 aafter write is %08x\n", reg);
}

/* Do target specific usb initialization */
void target_usb_init(void)
{
	hsusb_gpio_init();

	msm_otg_xceiv_reset();

	target_ulpi_init();
}

void target_usb_stop(void)
{
	int val;
	/* Voting down PLL8 */
	val = readl(0x009034C0);
	val &= ~(1 << 8);
	writel(val, 0x009034C0);
}

uint8_t target_uart_gsbi(void)
{
	return uart_gsbi_id;
}

int emmc_recovery_init(void)
{
	int rc;
	rc = _emmc_recovery_init();
	return rc;
}

/*
 * Function to set the capabilities for the host
 */
void target_mmc_caps(struct mmc_host *host)
{
	host->caps.ddr_mode = 0;
	host->caps.hs200_mode = 0;
	host->caps.bus_width = MMC_BOOT_BUS_WIDTH_8_BIT;
	host->caps.hs_clk_rate = MMC_CLK_50MHZ;
}
