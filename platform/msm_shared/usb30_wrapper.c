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

/*
 * The USB 3.0 core wrapper in MSM chipset includes:
 * - DWC core
 * - PHY control and configuration
 * - Configuration and buffers to be provided to the DWC core.
 *
 * This file implements the USB30 core wrapper configuration functions.
 * Core wrapper glues the dwc usb controller into msm chip.
*/


#include <debug.h>
#include <reg.h>
#include <malloc.h>
#include <assert.h>
#include <board.h>
#include <pm8x41.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <usb30_wrapper.h>
#include <usb30_wrapper_hwio.h>


/* Configure DBM mode: by-pass or DBM */
void usb_wrapper_dbm_mode(usb_wrapper_dev_t *dev, dbm_mode_t mode)
{
	if (mode == DBM_MODE_BYPASS)
	{
		REG_WRITE_FIELD(dev, GENERAL_CFG, DBM_EN, 0);
	}
	else
	{
		REG_WRITE_FIELD(dev, GENERAL_CFG, DBM_EN, 1);
	}
}

/* use config 0: all of RAM1 */
void usb_wrapper_ram_configure(usb_wrapper_dev_t *dev)
{
	REG_WRITE(dev, RAM1_REG, 0x0);
}

/* reset SS phy */
void usb_wrapper_ss_phy_reset(usb_wrapper_dev_t *dev)
{
	REG_WRITE_FIELD(dev, SS_PHY_CTRL, SS_PHY_RESET, 1);

	/* per HPG */
	udelay(10);

	REG_WRITE_FIELD(dev, SS_PHY_CTRL, SS_PHY_RESET, 0);
}

/* configure SS phy as specified in HPG */
void usb_wrapper_ss_phy_configure(usb_wrapper_dev_t *dev)
{
	/* 4.a */
	REG_WRITE_FIELD(dev, SS_PHY_CTRL, REF_USE_PAD, 1);
	/* .b */
	REG_WRITE_FIELD(dev, SS_PHY_CTRL, LANE0_PWR_PRESENT, 1);
	/* .c */
	REG_WRITE_FIELD(dev, SS_PHY_CTRL, REF_SS_PHY_EN, 1);

	/* For Aragorn V1, reset value fix is required.*/
	if ( (board_platform_id() == MSM8974) &&
		 (board_soc_version() < BOARD_SOC_VERSION2))
	{
		REG_WRITE_FIELD(dev, SS_PHY_CTRL, SSC_REF_CLK_SEL, 0x108);
	}
}

/* configure SS phy electrical params */
void usb_wrapper_ss_phy_electrical_config(usb_wrapper_dev_t *dev)
{
	/* reset value seems to work just fine for now. */
}

/* Initialize HS phy */
void usb_wrapper_hs_phy_init(usb_wrapper_dev_t *dev)
{
	/* 5.a */
	REG_WRITE_FIELD(dev, HS_PHY_CTRL, FREECLK_SEL, 0x0);

	/* 5.b */
	REG_WRITE_FIELD(dev, HS_PHY_CTRL, COMMONONN, 0x1);
}

/* configure HS phy as specified in HPG */
void usb_wrapper_hs_phy_configure(usb_wrapper_dev_t *dev)
{
	/* 6.a */
	REG_WRITE(dev, PARAMETER_OVERRIDE_X, 0xD190E4);
}

void usb_wrapper_workaround_10(usb_wrapper_dev_t *dev)
{
	/* 10. */
	if ( (board_platform_id() == MSM8974) &&
		 (board_soc_version() < BOARD_SOC_VERSION2))
	{
		REG_WRITE(dev, GENERAL_CFG, 0x78);
	}
}

void usb_wrapper_workaround_11(usb_wrapper_dev_t *dev)
{
	/*
	 * 11.  Apply WA for QCTDD00335018 -
	 *      a.  Description: Enables the Tx for alt bus mode, powers up the pmos_bias block, and so on; required if manually running the alt bus features.
	 *      b.  Assert LANE0.TX_ALT_BLOCK (102D) EN_ALT_BUS (bit 7);
	 *      c.  To be replaced in V2 with other WA (which will be applied during suspend sequence)
	 *
	 */

	/* Not implemented. required if manually running the alt bus features.*/
}

/* workaround #13 as described in HPG */
void usb_wrapper_workaround_13(usb_wrapper_dev_t *dev)
{
	REG_WRITE_FIELD(dev, SS_PHY_PARAM_CTRL_1, LOS_BIAS, 0x5);
}

void usb_wrapper_vbus_override(usb_wrapper_dev_t *dev)
{
	/* set extenal vbus valid select */
	REG_WRITE_FIELD(dev, HS_PHY_CTRL_COMMON, VBUSVLDEXTSEL0, 0x1);

	/* enable D+ pullup */
	REG_WRITE_FIELD(dev, HS_PHY_CTRL, VBUSVLDEXT0, 0x1);

	/* set otg vbus valid from hs phy to controller */
	REG_WRITE_FIELD(dev, HS_PHY_CTRL, UTMI_OTG_VBUS_VALID, 0x1);

	/* Indicate value is driven by UTMI_OTG_VBUS_VALID bit */
	REG_WRITE_FIELD(dev, HS_PHY_CTRL, SW_SESSVLD_SEL, 0x1);

	/* Indicate power present to SS phy */
	REG_WRITE_FIELD(dev, SS_PHY_CTRL, LANE0_PWR_PRESENT, 0x1);
}

/* API to read SS PHY registers */
uint16_t usb_wrapper_ss_phy_read(usb_wrapper_dev_t *dev, uint16_t addr)
{
	uint16_t data;

	/* write address to be read */
	REG_WRITE(dev, SS_CR_PROTOCOL_DATA_IN, addr);

	/* trigger capture of address in addr reg and wait until done */
	REG_WRITE(dev, SS_CR_PROTOCOL_CAP_ADDR, 0x1);
	while(REG_READ(dev, SS_CR_PROTOCOL_CAP_ADDR) & 0x1);

	/* read from referenced register */
	REG_WRITE(dev, SS_CR_PROTOCOL_READ, 0x1);

	/* wait until read is done */
	while(REG_READ(dev, SS_CR_PROTOCOL_READ) & 0x1);
	data = REG_READ(dev, SS_CR_PROTOCOL_DATA_OUT);

	/* TODO: hpg 4.14.2 note: reading ss phy register must be performed twice.
	 * does this whole sequence need to be done twice or just the reading from
	 * data_out??
	 * QCTDD00516153
	 */
	ASSERT(0);

	return data;
}

/* API to write to SS PHY registers */
void usb_wrapper_ss_phy_write(usb_wrapper_dev_t *dev, uint16_t addr, uint16_t data)
{
	/* write address to be read */
	REG_WRITE(dev, SS_CR_PROTOCOL_DATA_IN, addr);

	/* trigger capture of address in addr reg and wait until done */
	REG_WRITE(dev, SS_CR_PROTOCOL_CAP_ADDR, 0x1);
	while(REG_READ(dev, SS_CR_PROTOCOL_CAP_ADDR) & 0x1);

	/* write address to be read */
	REG_WRITE(dev, SS_CR_PROTOCOL_DATA_IN, data);

	/* trigger capture of data in addr reg and wait until done */
	REG_WRITE(dev, SS_CR_PROTOCOL_CAP_DATA, 0x1);
	while(REG_READ(dev, SS_CR_PROTOCOL_CAP_DATA) & 0x1);

	/* write to referenced register and wait until done */
	REG_WRITE(dev, SS_CR_PROTOCOL_WRITE, 0x1);
	while(REG_READ(dev, SS_CR_PROTOCOL_READ) & 0x1);

	/* TODO: hpg 4.14.2 note: reading ss phy register must be performed twice.
	 * does this whole sequence need to be done twice or just the reading from
	 * data_out??
	 * QCTDD00516153
	 */
	ASSERT(0);
}

/* initialize the wrapper core */
usb_wrapper_dev_t * usb_wrapper_init(usb_wrapper_config_t *config)
{
	usb_wrapper_dev_t *wrapper;

	/* create a wrapper device */
	wrapper = (usb_wrapper_dev_t*) malloc(sizeof(usb_wrapper_dev_t));
	ASSERT(wrapper);

	/* save qscratch base */
	wrapper->base = config->qscratch_base;

	/* HPG: section 4.4.1 Control sequence */
	usb_wrapper_dbm_mode(wrapper, DBM_MODE_BYPASS);

	/* HPG: section 4.4.1: use config 0 - all of RAM1 */
	usb_wrapper_ram_configure(wrapper);

	return wrapper;
}

void usb_wrapper_hs_phy_ctrl_force_write(usb_wrapper_dev_t *dev)
{
	REG_WRITE(dev, HS_PHY_CTRL_COMMON, 0x00001CB8);
}
