/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
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
#include <reg.h>
#include <dev/gpio.h>

#include <platform/iomap.h>
#define ACPU_CLK           0	/* Applications processor clock */
#define ADM_CLK            1	/* Applications data mover clock */
#define ADSP_CLK           2	/* ADSP clock */
#define EBI1_CLK           3	/* External bus interface 1 clock */
#define EBI2_CLK           4	/* External bus interface 2 clock */
#define ECODEC_CLK         5	/* External CODEC clock */
#define EMDH_CLK           6	/* External MDDI host clock */
#define GP_CLK             7	/* General purpose clock */
#define GRP_CLK            8	/* Graphics clock */
#define I2C_CLK            9	/* I2C clock */
#define ICODEC_RX_CLK     10	/* Internal CODEX RX clock */
#define ICODEC_TX_CLK     11	/* Internal CODEX TX clock */
#define IMEM_CLK          12	/* Internal graphics memory clock */
#define MDC_CLK           13	/* MDDI client clock */
#define MDP_CLK           14	/* Mobile display processor clock */
#define PBUS_CLK          15	/* Peripheral bus clock */
#define PCM_CLK           16	/* PCM clock */
#define PMDH_CLK          17	/* Primary MDDI host clock */
#define SDAC_CLK          18	/* Stereo DAC clock */
#define SDC1_CLK          19	/* Secure Digital Card clocks */
#define SDC1_PCLK         20
#define SDC2_CLK          21
#define SDC2_PCLK         22
#define SDC3_CLK          23
#define SDC3_PCLK         24
#define SDC4_CLK          25
#define SDC4_PCLK         26
#define TSIF_CLK          27	/* Transport Stream Interface clocks */
#define TSIF_REF_CLK      28
#define TV_DAC_CLK        29	/* TV clocks */
#define TV_ENC_CLK        30
#define UART1_CLK         31	/* UART clocks */
#define UART2_CLK         32
#define UART3_CLK         33
#define UART1DM_CLK       34
#define UART2DM_CLK       35
#define USB_HS_CLK        36	/* High speed USB core clock */
#define USB_HS_PCLK       37	/* High speed USB pbus clock */
#define USB_OTG_CLK       38	/* Full speed USB clock */
#define VDC_CLK           39	/* Video controller clock */
#define VFE_CLK           40	/* Camera / Video Front End clock */
#define VFE_MDC_CLK       41	/* VFE MDDI client clock */

/* qsd8k adds... */
#define MDP_LCDC_PCLK_CLK     42
#define MDP_LCDC_PAD_PCLK_CLK 43
#define MDP_VSYNC_CLK         44

#define P_USB_HS_CORE_CLK     53	/* High speed USB 1 core clock */
/* msm7x30 adds... */
#define PMDH_P_CLK            82
#define MDP_P_CLK             86

enum {
	PCOM_CMD_IDLE = 0x0,
	PCOM_CMD_DONE,
	PCOM_RESET_APPS,
	PCOM_RESET_CHIP,
	PCOM_CONFIG_NAND_MPU,
	PCOM_CONFIG_USB_CLKS,
	PCOM_GET_POWER_ON_STATUS,
	PCOM_GET_WAKE_UP_STATUS,
	PCOM_GET_BATT_LEVEL,
	PCOM_CHG_IS_CHARGING,
	PCOM_POWER_DOWN,
	PCOM_USB_PIN_CONFIG,
	PCOM_USB_PIN_SEL,
	PCOM_SET_RTC_ALARM,
	PCOM_NV_READ,
	PCOM_NV_WRITE,
	PCOM_GET_UUID_HIGH,
	PCOM_GET_UUID_LOW,
	PCOM_GET_HW_ENTROPY,
	PCOM_RPC_GPIO_TLMM_CONFIG_REMOTE,
	PCOM_CLKCTL_RPC_ENABLE,
	PCOM_CLKCTL_RPC_DISABLE,
	PCOM_CLKCTL_RPC_RESET,
	PCOM_CLKCTL_RPC_SET_FLAGS,
	PCOM_CLKCTL_RPC_SET_RATE,
	PCOM_CLKCTL_RPC_MIN_RATE,
	PCOM_CLKCTL_RPC_MAX_RATE,
	PCOM_CLKCTL_RPC_RATE,
	PCOM_CLKCTL_RPC_PLL_REQUEST,
	PCOM_CLKCTL_RPC_ENABLED,
	PCOM_VREG_SWITCH,
	PCOM_VREG_SET_LEVEL,
	PCOM_GPIO_TLMM_CONFIG_GROUP,
	PCOM_GPIO_TLMM_UNCONFIG_GROUP,
	PCOM_NV_READ_HIGH_BITS,
	PCOM_NV_WRITE_HIGH_BITS,
	PCOM_RPC_GPIO_TLMM_CONFIG_EX = 0x25,
	PCOM_NUM_CMDS,
	PCOM_KERNEL_SEC_BOOT = 0x7A,
};

enum {
	PCOM_INVALID_STATUS = 0x0,
	PCOM_READY,
	PCOM_CMD_RUNNING,
	PCOM_CMD_SUCCESS,
	PCOM_CMD_FAIL,
};

#ifndef PLATFORM_MSM7X30
#define MSM_A2M_INT(n) (MSM_CSR_BASE + 0x400 + (n) * 4)
#endif
static inline void notify_other_proc_comm(void)
{
#ifndef PLATFORM_MSM7X30
	writel(1, MSM_A2M_INT(6));
#else
	writel(1 << 6, (MSM_GCC_BASE + 0x8));
#endif
}

#define APP_COMMAND (MSM_SHARED_BASE + 0x00)
#define APP_STATUS  (MSM_SHARED_BASE + 0x04)
#define APP_DATA1   (MSM_SHARED_BASE + 0x08)
#define APP_DATA2   (MSM_SHARED_BASE + 0x0C)

#define MDM_COMMAND (MSM_SHARED_BASE + 0x10)
#define MDM_STATUS  (MSM_SHARED_BASE + 0x14)
#define MDM_DATA1   (MSM_SHARED_BASE + 0x18)
#define MDM_DATA2   (MSM_SHARED_BASE + 0x1C)

int msm_proc_comm(unsigned cmd, unsigned *data1, unsigned *data2)
{
	int ret = -1;
	unsigned status;

//      dprintf(INFO, "proc_comm(%d,%d,%d)\n",
//              cmd, data1 ? *data1 : 0, data2 ? *data2 : 0);
	while (readl(MDM_STATUS) != PCOM_READY) {
		/* XXX check for A9 reset */
	}

	if (data1)
		writel(*data1, APP_DATA1);
	if (data2)
		writel(*data2, APP_DATA2);

	/*
	 * As per the specs write data, cmd, interrupt for
	 * proc comm processing
	 */
	writel(cmd, APP_COMMAND);
//      dprintf(INFO, "proc_comm tx\n");
	notify_other_proc_comm();
	while (readl(APP_COMMAND) != PCOM_CMD_DONE) {
		/* XXX check for A9 reset */
	}

	status = readl(APP_STATUS);
//      dprintf(INFO, "proc_comm status %d\n", status);

	if (status != PCOM_CMD_FAIL) {
		if (data1)
			*data1 = readl(APP_DATA1);
		if (data2)
			*data2 = readl(APP_DATA2);
		ret = 0;
		/*
		 * Write command idle to indicate non HLOS that
		 * apps has finished reading the status & data
		 * of proc comm command
		 */
		writel(PCOM_CMD_IDLE, APP_COMMAND);
	}

	return ret;
}

static int clock_enable(unsigned id)
{
	return msm_proc_comm(PCOM_CLKCTL_RPC_ENABLE, &id, 0);
}

static int clock_disable(unsigned id)
{
	return msm_proc_comm(PCOM_CLKCTL_RPC_DISABLE, &id, 0);
}

static int clock_set_rate(unsigned id, unsigned rate)
{
	return msm_proc_comm(PCOM_CLKCTL_RPC_SET_RATE, &id, &rate);
}

static int clock_get_rate(unsigned id)
{
	if (msm_proc_comm(PCOM_CLKCTL_RPC_RATE, &id, 0)) {
		return -1;
	} else {
		return (int)id;
	}
}

void usb_clock_init()
{
	clock_enable(USB_HS_PCLK);
	clock_enable(USB_HS_CLK);
	clock_enable(P_USB_HS_CORE_CLK);
}

void lcdc_clock_init(unsigned rate)
{
	clock_set_rate(MDP_LCDC_PCLK_CLK, rate);
	clock_enable(MDP_LCDC_PCLK_CLK);
	clock_enable(MDP_LCDC_PAD_PCLK_CLK);
}

void mdp_clock_init(unsigned rate)
{
	clock_set_rate(MDP_CLK, rate);
	clock_enable(MDP_CLK);
	clock_enable(MDP_P_CLK);
}

void uart3_clock_init(void)
{
	clock_enable(UART3_CLK);
	clock_set_rate(UART3_CLK, 19200000 / 4);
}

void uart2_clock_init(void)
{
	clock_enable(UART2_CLK);
	clock_set_rate(UART2_CLK, 19200000);
}

void uart1_clock_init(void)
{
	clock_enable(UART1_CLK);
	clock_set_rate(UART1_CLK, 19200000 / 4);
}

void mddi_clock_init(unsigned num, unsigned rate)
{
	unsigned clock_id;

	if (num == 0)
		clock_id = PMDH_CLK;
	else
		clock_id = EMDH_CLK;

	clock_enable(clock_id);
	clock_set_rate(clock_id, rate);
#ifdef PLATFORM_MSM7X30
	clock_enable(PMDH_P_CLK);
#endif
}

void reboot(unsigned reboot_reason)
{
	msm_proc_comm(PCOM_RESET_CHIP, &reboot_reason, 0);
	for (;;) ;
}

int mmc_clock_enable_disable(unsigned id, unsigned enable)
{
	if (enable) {
		return clock_enable(id);	//Enable mmc clock rate
	} else {
		return clock_disable(id);	//Disable mmc clock rate
	}
}

int mmc_clock_set_rate(unsigned id, unsigned rate)
{
	return clock_set_rate(id, rate);	//Set mmc clock rate
}

int mmc_clock_get_rate(unsigned id)
{
	return clock_get_rate(id);	//Get mmc clock rate
}

int gpio_tlmm_config(unsigned config, unsigned disable)
{
	return msm_proc_comm(PCOM_RPC_GPIO_TLMM_CONFIG_EX, &config, &disable);
}

int vreg_set_level(unsigned id, unsigned mv)
{
	return msm_proc_comm(PCOM_VREG_SET_LEVEL, &id, &mv);
}

int vreg_enable(unsigned id)
{
	int enable = 1;
	return msm_proc_comm(PCOM_VREG_SWITCH, &id, &enable);

}

int vreg_disable(unsigned id)
{
	int enable = 0;
	return msm_proc_comm(PCOM_VREG_SWITCH, &id, &enable);
}

void set_tamper_flag(int tamper)
{
	return msm_proc_comm(PCOM_KERNEL_SEC_BOOT, &tamper, 0);
}
