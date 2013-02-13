/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
 *
 */

/*
 * Toshiba MIPI-DSI-to-LVDS Bridge driver.
 * Device Model TC358764XBG/65XBG.
 * Reference document: TC358764XBG_65XBG_V119.pdf
 *
 * The Host sends a DSI Generic Long Write packet (Data ID = 0x29) over the
 * DSI link for each write access transaction to the chip configuration
 * registers.
 * Payload of this packet is 16-bit register address and 32-bit data.
 * Multiple data values are allowed for sequential addresses.
 *
 * The Host sends a DSI Generic Read packet (Data ID = 0x24) over the DSI
 * link for each read request transaction to the chip configuration
 * registers. Payload of this packet is further defined as follows:
 * 16-bit address followed by a 32-bit value (Generic Long Read Response
 * packet).
 *
 * The bridge supports 5 GPIO lines controlled via the GPC register.
 *
 * The bridge support I2C Master/Slave.
 * The I2C slave can be used for read/write to the bridge register instead of
 * using the DSI interface.
 * I2C slave address is 0x0F (read/write 0x1F/0x1E).
 * The I2C Master can be used for communication with the panel if
 * it has an I2C slave.
 *
 * NOTE: The I2C interface is not used in this driver.
 * Only the DSI interface is used for read/write the bridge registers.
 *
 * Pixel data can be transmitted in non-burst or burst fashion.
 * Non-burst refers to pixel data packet transmission time on DSI link
 * being roughly the same (to account for packet overhead time)
 * as active video line time on LVDS output (i.e. DE = 1).
 * And burst refers to pixel data packet transmission time on DSI link
 * being less than the active video line time on LVDS output.
 * Video mode transmission is further differentiated by the types of
 * timing events being transmitted.
 * Video pulse mode refers to the case where both sync start and sync end
 * events (for frame and line) are transmitted.
 * Video event mode refers to the case where only sync start events
 * are transmitted.
 * This is configured via register bit VPCTRL.EVTMODE.
 *
 */

#include <stdint.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <sys/types.h>
#include <err.h>
#include <reg.h>
#include <debug.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <target/display.h>
#include <dev/gpio.h>
#include <dev/pm8921.h>
#include <dev/pm8921_pwm.h>

#ifndef u32
	#define u32 uint32_t
#endif
#ifndef u16
	#define u16 uint16_t
#endif

/* Registers definition */

/* DSI D-PHY Layer Registers */
#define D0W_DPHYCONTTX	0x0004	/* Data Lane 0 DPHY Tx Control */
#define CLW_DPHYCONTRX	0x0020	/* Clock Lane DPHY Rx Control */
#define D0W_DPHYCONTRX	0x0024	/* Data Lane 0 DPHY Rx Control */
#define D1W_DPHYCONTRX	0x0028	/* Data Lane 1 DPHY Rx Control */
#define D2W_DPHYCONTRX	0x002C	/* Data Lane 2 DPHY Rx Control */
#define D3W_DPHYCONTRX	0x0030	/* Data Lane 3 DPHY Rx Control */
#define COM_DPHYCONTRX	0x0038	/* DPHY Rx Common Control */
#define CLW_CNTRL	0x0040	/* Clock Lane Control */
#define D0W_CNTRL	0x0044	/* Data Lane 0 Control */
#define D1W_CNTRL	0x0048	/* Data Lane 1 Control */
#define D2W_CNTRL	0x004C	/* Data Lane 2 Control */
#define D3W_CNTRL	0x0050	/* Data Lane 3 Control */
#define DFTMODE_CNTRL	0x0054	/* DFT Mode Control */

/* DSI PPI Layer Registers */
#define PPI_STARTPPI	0x0104	/* START control bit of PPI-TX function. */
#define PPI_BUSYPPI	0x0108
#define PPI_LINEINITCNT	0x0110	/* Line Initialization Wait Counter  */
#define PPI_LPTXTIMECNT	0x0114
#define PPI_LANEENABLE	0x0134	/* Enables each lane at the PPI layer. */
#define PPI_TX_RX_TA	0x013C	/* DSI Bus Turn Around timing parameters */

/* Analog timer function enable */
#define PPI_CLS_ATMR	0x0140	/* Delay for Clock Lane in LPRX  */
#define PPI_D0S_ATMR	0x0144	/* Delay for Data Lane 0 in LPRX */
#define PPI_D1S_ATMR	0x0148	/* Delay for Data Lane 1 in LPRX */
#define PPI_D2S_ATMR	0x014C	/* Delay for Data Lane 2 in LPRX */
#define PPI_D3S_ATMR	0x0150	/* Delay for Data Lane 3 in LPRX */
#define PPI_D0S_CLRSIPOCOUNT	0x0164

#define PPI_D1S_CLRSIPOCOUNT	0x0168	/* For lane 1 */
#define PPI_D2S_CLRSIPOCOUNT	0x016C	/* For lane 2 */
#define PPI_D3S_CLRSIPOCOUNT	0x0170	/* For lane 3 */

#define CLS_PRE		0x0180	/* Digital Counter inside of PHY IO */
#define D0S_PRE		0x0184	/* Digital Counter inside of PHY IO */
#define D1S_PRE		0x0188	/* Digital Counter inside of PHY IO */
#define D2S_PRE		0x018C	/* Digital Counter inside of PHY IO */
#define D3S_PRE		0x0190	/* Digital Counter inside of PHY IO */
#define CLS_PREP	0x01A0	/* Digital Counter inside of PHY IO */
#define D0S_PREP	0x01A4	/* Digital Counter inside of PHY IO */
#define D1S_PREP	0x01A8	/* Digital Counter inside of PHY IO */
#define D2S_PREP	0x01AC	/* Digital Counter inside of PHY IO */
#define D3S_PREP	0x01B0	/* Digital Counter inside of PHY IO */
#define CLS_ZERO	0x01C0	/* Digital Counter inside of PHY IO */
#define D0S_ZERO	0x01C4	/* Digital Counter inside of PHY IO */
#define D1S_ZERO	0x01C8	/* Digital Counter inside of PHY IO */
#define D2S_ZERO	0x01CC	/* Digital Counter inside of PHY IO */
#define D3S_ZERO	0x01D0	/* Digital Counter inside of PHY IO */

#define PPI_CLRFLG	0x01E0	/* PRE Counters has reached set values */
#define PPI_CLRSIPO	0x01E4	/* Clear SIPO values, Slave mode use only. */
#define HSTIMEOUT	0x01F0	/* HS Rx Time Out Counter */
#define HSTIMEOUTENABLE	0x01F4	/* Enable HS Rx Time Out Counter */
#define DSI_STARTDSI	0x0204	/* START control bit of DSI-TX function */
#define DSI_BUSYDSI	0x0208
#define DSI_LANEENABLE	0x0210	/* Enables each lane at the Protocol layer. */
#define DSI_LANESTATUS0	0x0214	/* Displays lane is in HS RX mode. */
#define DSI_LANESTATUS1	0x0218	/* Displays lane is in ULPS or STOP state */

#define DSI_INTSTATUS	0x0220	/* Interrupt Status */
#define DSI_INTMASK	0x0224	/* Interrupt Mask */
#define DSI_INTCLR	0x0228	/* Interrupt Clear */
#define DSI_LPTXTO	0x0230	/* Low Power Tx Time Out Counter */

#define DSIERRCNT	0x0300	/* DSI Error Count */
#define APLCTRL		0x0400	/* Application Layer Control */
#define RDPKTLN		0x0404	/* Command Read Packet Length */
#define VPCTRL		0x0450	/* Video Path Control */
#define HTIM1		0x0454	/* Horizontal Timing Control 1 */
#define HTIM2		0x0458	/* Horizontal Timing Control 2 */
#define VTIM1		0x045C	/* Vertical Timing Control 1 */
#define VTIM2		0x0460	/* Vertical Timing Control 2 */
#define VFUEN		0x0464	/* Video Frame Timing Update Enable */

/* Mux Input Select for LVDS LINK Input */
#define LVMX0003	0x0480	/* Bit 0 to 3 */
#define LVMX0407	0x0484	/* Bit 4 to 7 */
#define LVMX0811	0x0488	/* Bit 8 to 11 */
#define LVMX1215	0x048C	/* Bit 12 to 15 */
#define LVMX1619	0x0490	/* Bit 16 to 19 */
#define LVMX2023	0x0494	/* Bit 20 to 23 */
#define LVMX2427	0x0498	/* Bit 24 to 27 */

#define LVCFG		0x049C	/* LVDS Configuration  */
#define LVPHY0		0x04A0	/* LVDS PHY 0 */
#define LVPHY1		0x04A4	/* LVDS PHY 1 */
#define SYSSTAT		0x0500	/* System Status  */
#define SYSRST		0x0504	/* System Reset  */

/* GPIO Registers */
#define GPIOC		0x0520	/* GPIO Control  */
#define GPIOO		0x0524	/* GPIO Output  */
#define GPIOI		0x0528	/* GPIO Input  */

/* I2C Registers */
#define I2CTIMCTRL	0x0540	/* I2C IF Timing and Enable Control */
#define I2CMADDR	0x0544	/* I2C Master Addressing */
#define WDATAQ		0x0548	/* Write Data Queue */
#define RDATAQ		0x054C	/* Read Data Queue */

/* Chip ID and Revision ID Register */
#define IDREG		0x0580

#define TC358764XBG_ID	0x00006500

/* Debug Registers */
#define DEBUG00		0x05A0	/* Debug */
#define DEBUG01		0x05A4	/* LVDS Data */

/* PWM */
static u32 d2l_pwm_freq_hz = (3.921*1000);

#define PWM_FREQ_HZ	(d2l_pwm_freq_hz)
#define PWM_PERIOD_USEC (USEC_PER_SEC / PWM_FREQ_HZ)
#define PWM_DUTY_LEVEL (PWM_PERIOD_USEC / PWM_LEVEL)

#define CMD_DELAY 100
#define DSI_MAX_LANES 4
#define KHZ 1000
#define MHZ (1000*1000)

#define BIT(bit) (1 << (bit))
#define DSI_HOST_HDR_SIZE	4
#define DSI_HDR_LAST		BIT(31)
#define DSI_HDR_LONG_PKT	BIT(30)
#define DSI_HDR_BTA		BIT(29)
#define DSI_HDR_VC(vc)		(((vc) & 0x03) << 22)
#define DSI_HDR_DTYPE(dtype)	(((dtype) & 0x03f) << 16)
#define DSI_HDR_DATA2(data)	(((data) & 0x0ff) << 8)
#define DSI_HDR_DATA1(data)	((data) & 0x0ff)
#define DSI_HDR_WC(wc)		((wc) & 0x0ffff)

#define DTYPE_GEN_LWRITE	0x29	/* long write */
#define DTYPE_GEN_READ2		0x24	/* long read, 2 parameter */


/**
 * Command payload for DTYPE_GEN_LWRITE (0x29) / DTYPE_GEN_READ2 (0x24).
 */
struct wr_cmd_payload {
	u32 dsi_hdr;
	u16 addr;
	u16 data0;
	u16 data1;
	u16 align32;
} __packed;

static u32 mipi_d2l_read_reg(u16 reg)
{
	int ret = 0;
	char buf[24];
	char *rp = buf;
	u32 data;
	int len = 4; /* return data size */
	u32 dsi_hdr;
	struct mipi_dsi_cmd mipi_cmd;

	mipi_cmd.size = sizeof(dsi_hdr);
	mipi_cmd.payload = (char *) &dsi_hdr;

	dsi_hdr = 0;
	dsi_hdr |= DSI_HDR_DTYPE(DTYPE_GEN_READ2);
	dsi_hdr |= DSI_HDR_WC(0);
	dsi_hdr |= DSI_HDR_VC(0); /* Virtual Channel */
	dsi_hdr |= DSI_HDR_DATA1((reg & 0xFF));
	dsi_hdr |= DSI_HDR_DATA2((reg >> 8));
	dsi_hdr |= DSI_HDR_LAST;
	dsi_hdr |= DSI_HDR_BTA;

	/* mutex had been acquired at mipi_dsi_on */
	ret = mipi_dsi_cmds_tx(&mipi_cmd, 1);
	len = mipi_dsi_cmds_rx(&rp, len);

	data = *(u32 *)buf;

	dprintf(SPEW, "%s: reg=0x%x.data=0x%08x.\n", __func__, reg, data);

	return data;
}


/**
 * Write a bridge register
 *
 * @param reg
 * @param data
 *
 * @return int
 */
static int mipi_d2l_write_reg(u16 reg, u32 data)
{
	struct wr_cmd_payload payload;
	struct mipi_dsi_cmd mipi_cmd;
	int dlen = sizeof(reg) + sizeof(data);

	mipi_cmd.size = sizeof(payload);
	mipi_cmd.payload = (char *) &payload;

	payload.addr = reg;
	payload.dsi_hdr = 0;
	payload.dsi_hdr |= DSI_HDR_DTYPE(DTYPE_GEN_LWRITE);
	payload.dsi_hdr |= DSI_HDR_WC(dlen);
	payload.dsi_hdr |= DSI_HDR_VC(0); /* Virtual Channel */
	payload.dsi_hdr |= DSI_HDR_LONG_PKT;
	payload.dsi_hdr |= DSI_HDR_LAST;
	payload.align32 = 0xFFFF;

	payload.data0 = data & 0xFFFF;
	payload.data1 = data >> 16;

	mipi_dsi_cmds_tx(&mipi_cmd, 1);

	dprintf(SPEW, "%s: reg=0x%x. data=0x%x.\n", __func__, reg, data);

	return 0;
}

static int mipi_d2l_read_chip_id(void)
{
	u32 chip_id = 0;
	int retry = 0;

	while (chip_id != TC358764XBG_ID) {
		chip_id = mipi_d2l_read_reg(IDREG);
		dprintf(SPEW, "%s: chip_id=0x%x.\n", __func__, chip_id);
		mdelay(100);
		if (retry++ >= 2)
			return ERR_IO;
	}

	return chip_id;
}

/**
 * Init the D2L bridge via the DSI interface for Video.
 *
 * VPCTRL.EVTMODE (0x20) configuration bit is needed to determine whether
 * video timing information is delivered in pulse mode or event mode.
 * In pulse mode, both Sync Start and End packets are required.
 * In event mode, only Sync Start packets are required.
 *
 * @param pinfo
 *
 * @return int
 */
int mipi_d2l_dsi_init_sequence(struct msm_panel_info *pinfo)
{
	u32 lanes_enable;
	u32 vpctrl;
	u32 htime1;
	u32 vtime1;
	u32 htime2;
	u32 vtime2;
	u32 ppi_tx_rx_ta; /* BTA Bus-Turn-Around */
	u32 lvcfg;
	u32 hbpr;	/* Horizontal Back Porch */
	u32 hpw;	/* Horizontal Pulse Width */
	u32 vbpr;	/* Vertical Back Porch */
	u32 vpw;	/* Vertical Pulse Width */

	u32 hfpr;	/* Horizontal Front Porch */
	u32 hsize;	/* Horizontal Active size */
	u32 vfpr;	/* Vertical Front Porch */
	u32 vsize;	/* Vertical Active size */
	bool vesa_rgb888 = false;

	lanes_enable = 0x01F; /* clock-lane enable + 4 data lanes */

	vpctrl = 0x01000120; /* DSI_NON_BURST_SYNCH_EVENT */

	dprintf(SPEW, "%s.xres=%d.yres=%d.fps=%d.dst_format=%d.\n",
		__func__,
		 pinfo->xres,
		 pinfo->yres,
		 pinfo->mipi.frame_rate,
		 pinfo->mipi.dst_format);

	hbpr = pinfo->lcdc.h_back_porch;
	hpw	= pinfo->lcdc.h_pulse_width;
	vbpr = pinfo->lcdc.v_back_porch;
	vpw	= pinfo->lcdc.v_pulse_width;

	htime1 = (hbpr << 16) + hpw;
	vtime1 = (vbpr << 16) + vpw;

	hfpr = pinfo->lcdc.h_front_porch;
	hsize = pinfo->xres;
	vfpr = pinfo->lcdc.v_front_porch;
	vsize = pinfo->yres;

	htime2 = (hfpr << 16) + hsize;
	vtime2 = (vfpr << 16) + vsize;

	lvcfg = 0x0003; /* PCLK=DCLK/3, Dual Link, LVEN */
	vpctrl = 0x01000120; /* Output RGB888 , Event-Mode , */
	ppi_tx_rx_ta = 0x00040004;

	if (pinfo->xres == 1366) {
		ppi_tx_rx_ta = 0x00040004;
		lvcfg = 0x01; /* LVEN */
		vesa_rgb888 = true;
	}

	if (pinfo->xres == 1200) {
		lvcfg = 0x0103; /* PCLK=DCLK/4, Dual Link, LVEN */
		vesa_rgb888 = true;
	}

	dprintf(SPEW, "%s.htime1=0x%x.\n", __func__, htime1);
	dprintf(SPEW, "%s.vtime1=0x%x.\n", __func__, vtime1);
	dprintf(SPEW, "%s.vpctrl=0x%x.\n", __func__, vpctrl);
	dprintf(SPEW, "%s.lvcfg=0x%x.\n", __func__, lvcfg);

	mipi_d2l_write_reg(SYSRST, 0xFF);
	mdelay(30);

	if (vesa_rgb888) {
		/* VESA format instead of JEIDA format for RGB888 */
		mipi_d2l_write_reg(LVMX0003, 0x03020100);
		mipi_d2l_write_reg(LVMX0407, 0x08050704);
		mipi_d2l_write_reg(LVMX0811, 0x0F0E0A09);
		mipi_d2l_write_reg(LVMX1215, 0x100D0C0B);
		mipi_d2l_write_reg(LVMX1619, 0x12111716);
		mipi_d2l_write_reg(LVMX2023, 0x1B151413);
		mipi_d2l_write_reg(LVMX2427, 0x061A1918);
	}

	mipi_d2l_write_reg(PPI_TX_RX_TA, ppi_tx_rx_ta); /* BTA */
	mipi_d2l_write_reg(PPI_LPTXTIMECNT, 0x00000004);
	mipi_d2l_write_reg(PPI_D0S_CLRSIPOCOUNT, 0x00000003);
	mipi_d2l_write_reg(PPI_D1S_CLRSIPOCOUNT, 0x00000003);
	mipi_d2l_write_reg(PPI_D2S_CLRSIPOCOUNT, 0x00000003);
	mipi_d2l_write_reg(PPI_D3S_CLRSIPOCOUNT, 0x00000003);
	mipi_d2l_write_reg(PPI_LANEENABLE, lanes_enable);
	mipi_d2l_write_reg(DSI_LANEENABLE, lanes_enable);
	mipi_d2l_write_reg(PPI_STARTPPI, 0x00000001);
	mipi_d2l_write_reg(DSI_STARTDSI, 0x00000001);

	mipi_d2l_write_reg(VPCTRL, vpctrl); /* RGB888 + Event mode */
	mipi_d2l_write_reg(HTIM1, htime1);
	mipi_d2l_write_reg(VTIM1, vtime1);
	mipi_d2l_write_reg(HTIM2, htime2);
	mipi_d2l_write_reg(VTIM2, vtime2);
	mipi_d2l_write_reg(VFUEN, 0x00000001);
	mipi_d2l_write_reg(LVCFG, lvcfg); /* Enables LVDS tx */

	mipi_d2l_write_reg(GPIOC, 0x1F);
	/* Set gpio#4=U/D=0, gpio#3=L/R=1 , gpio#2,1=CABC=0, gpio#0=NA. */
	mipi_d2l_write_reg(GPIOO, 0x08);

	mipi_d2l_read_chip_id();

	return 0;
}


