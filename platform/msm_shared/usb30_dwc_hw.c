/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
#include <reg.h>
#include <bits.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <arch/defines.h>
#include <platform/timer.h>
#include <platform/interrupts.h>
#include <platform/irqs.h>
#include <kernel/event.h>
#include <usb30_dwc_hwio.h>
#include <usb30_dwc.h>
#include <usb30_dwc_hw.h>
#include <smem.h>
#include <board.h>

extern char* ss_link_state_lookup[20];
extern char* hs_link_state_lookup[20];
extern char* event_lookup_device[20];
extern char* event_lookup_ep[20];
extern char* dev_ctrl_state_lookup[20];
extern char* ep_state_lookup[20];
extern char* dev_state_lookup[20];
extern char* speed_lookup[20];

//#define DEBUG_USB

#ifdef DEBUG_USB
#define DBG(...) dprintf(ALWAYS, __VA_ARGS__)
#else
#define DBG(...)
#endif

#define ERR(...) dprintf(ALWAYS, __VA_ARGS__)

__WEAK int platform_is_8974()
{
	return 0;
}

/* This file provides interface to interact with DWC hardware. This code
 * does not maintain any soft states. It programs the h/w as requested by the
 * APIs.
 */

/* generic api to send endpoint command */
static void dwc_ep_cmd(dwc_dev_t *dev, uint8_t ep_phy_num, dwc_ep_cmd_t *ep_cmd)
{
	if(REG_READ_FIELDI(dev, GUSB2PHYCFG, 0, SUSPENDUSB20))
	{
		/* this must be 0. see snps 6.3.2.5.8 */
		ASSERT(0);
	}

	/* wait until previous command is in-active */
	while( REG_READ_FIELDI(dev, DEPCMD, ep_phy_num, CMDACT) == 1);

	/* clear cmd reg */
	REG_WRITEI(dev, DEPCMD, ep_phy_num, 0);

	/* write the command parameters */
	REG_WRITEI(dev, DEPCMDPAR2, ep_phy_num, ep_cmd->param2);
	REG_WRITEI(dev, DEPCMDPAR1, ep_phy_num, ep_cmd->param1);
	REG_WRITEI(dev, DEPCMDPAR0, ep_phy_num, ep_cmd->param0);

	/* command */
	REG_WRITE_FIELDI(dev, DEPCMD, ep_phy_num, CMDTYP, ep_cmd->cmd);

	if ((ep_cmd->cmd == DEPCMD_CMD_UPDATE_TRANSFER) ||
		(ep_cmd->cmd == DEPCMD_CMD_END_TRANSFER) ||
		(ep_cmd->cmd == DEPCMD_CMD_START_NEW_CONF))
	{
		/* set the transfer resource index */
		REG_WRITE_FIELDI(dev,
						 DEPCMD,
						 ep_phy_num,
						 COMMANDPARAM,
						 ep_cmd->xfer_resource_index);
	}

	/* command interrupt can be set only if device is in running state. */
	if(dwc_device_run_status(dev))
	{
			REG_WRITE_FIELDI(dev, DEPCMD, ep_phy_num, CMDIOC, 1);
	}

	DBG("\nEP CMD: ep = %d : 0x%05x  "
		"pram0 = 0x%08x param1 = 0x%08x param2 = 0x%08x",
		ep_phy_num,
		REG_READI(dev, DEPCMD, ep_phy_num) | (1 << 10),
		ep_cmd->param0,
		ep_cmd->param1,
		ep_cmd->param2);

	/* set active */
	REG_WRITE_FIELDI(dev, DEPCMD, ep_phy_num, CMDACT, 1);

	/* Wait until active bit is cleared.
	 * This does not necessarily mean that command is executed.
	 * It only means a new command can be issued.
	 * We get an interrupt when command execution is complete.
	 */
	while( REG_READ_FIELDI(dev, DEPCMD, ep_phy_num, CMDACT) == 1);
}

/* send start transfer command to the specified ep.
 * assumes the trb are already populated.
 */
void dwc_ep_cmd_start_transfer(dwc_dev_t *dev, uint8_t ep_phy_num)
{
	dwc_ep_cmd_t ep_cmd;
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];

	uint32_t td_addr_low  = (uint32_t) ep->trb;
	uint32_t td_addr_high = (uint32_t) 0x0;

	/* transfer descriptor (aka TRB list) address must be on 16 byte boundary.*/
	ASSERT((td_addr_low  & 0xF) == 0);
	ASSERT((td_addr_high & 0xF) == 0);

	/* set command */
	ep_cmd.cmd = DEPCMD_CMD_START_TRANSFER;

	/* set params */
	ep_cmd.param2 = 0;
	ep_cmd.param1 = td_addr_low;
	ep_cmd.param0 = td_addr_high;

	dwc_ep_cmd(dev, ep_phy_num, &ep_cmd);

	/* Note: On execution of this cmd, a ep command complete event occurs.
	 * this DEPEVT ep event returns a XferRscIdx - transfer resource
	 * index. That must be used to Update or End this transfer.
	 */
	DBG("\n START_TRANSFER: new EP phy_num = %d state is = %s",
		ep_phy_num, ep_state_lookup[ep->state]);
}

/* end transfer on a particular endpoint */
void dwc_ep_cmd_end_transfer(dwc_dev_t *dev, uint8_t ep_phy_num)
{
	dwc_ep_cmd_t ep_cmd;

	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];

	ep_cmd.cmd = 0;

	/* set cmd and the resource index */
	ep_cmd.cmd                 = DEPCMD_CMD_END_TRANSFER;
	ep_cmd.xfer_resource_index = ep->resource_idx;

	/* params */
	ep_cmd.param2 = 0;
	ep_cmd.param1 = 0;
	ep_cmd.param0 = 0;

	/* note: TRB status is not updated by the h/w when end transfer is issued.
	 * snps: 6.3.2.5.7
	 */
	dwc_ep_cmd(dev, ep_phy_num, &ep_cmd);
}

/* set number of transfer resources to be used for the ep. */
void dwc_ep_cmd_set_transfer_resource(dwc_dev_t *dev, uint8_t ep_phy_num)
{
	dwc_ep_cmd_t ep_cmd;

	/* set command */
	ep_cmd.cmd = DEPCMD_CMD_SET_TR_CONF;

	ep_cmd.param2 = 0;
	ep_cmd.param1 = 0;
	ep_cmd.param0 = 1; /* number of transfer resources: always set to 1 */

	dwc_ep_cmd(dev, ep_phy_num, &ep_cmd);
}

/* Configure end point in the core before starting to use it. The following
   parameters need to be configured:
   - usb ep number
   - ep direction
   - ep type
   - mak pkt size
   - burst size
   - transfer events to be generated for this ep
   - for IN ep, tx fifo to be used
*/
void dwc_ep_cmd_set_config(dwc_dev_t *dev, uint8_t index, uint8_t action)
{
	uint8_t  ep_phy_num;
	uint8_t  ep_usb_num;
	uint8_t  ep_direction;
	uint16_t max_pkt_size;
	uint32_t burst_size;
	uint8_t  tx_fifo_num;

	dwc_ep_t      ep;
	dwc_ep_cmd_t  ep_cmd;
	dwc_ep_type_t ep_type;

	ep = dev->ep[index];

	/* get the corresponding physical ep number */
	ep_phy_num    = ep.phy_num;
	ep_usb_num    = ep.number;
	ep_direction  = ep.dir;
	ep_type       = ep.type;
	max_pkt_size  = ep.max_pkt_size;
	burst_size    = ep.burst_size;
	tx_fifo_num   = ep.tx_fifo_num;

	/* set command */
	ep_cmd.cmd    = DEPCMD_CMD_SET_EP_CONF;
	ep_cmd.param2 = 0x0;
	ep_cmd.param1 = 0x0;
	ep_cmd.param0 = 0x0;

	/* TODO: set bInterval according to ep value.
	 * ignored since it is not used for bulk.
	 */

	/* map this usb ep to the ep_phy_num */
	ep_cmd.param1 |= ep_usb_num   << DEPCMDPAR1_USB_EP_NUM_BIT;
	ep_cmd.param1 |= ep_direction << DEPCMDPAR1_USB_EP_DIR_BIT;

	/* enable event generation */
	ep_cmd.param1 |= BIT(DEPCMDPAR2_XFER_N_RDY_BIT);
	ep_cmd.param1 |= BIT(DEPCMDPAR2_XFER_COMPLETE_BIT);

	/* interrupt number: which event buffer to be used. */
	ep_cmd.param1 |= 0;

	/* action: 0 = initialize */
	ep_cmd.param0 |= (action << DEPCMDPAR0_ACTION_BIT);
	/* burst size */
	ep_cmd.param0 |= (burst_size << DEPCMDPAR0_BURST_SIZE_BIT);

	ep_cmd.param0 |= tx_fifo_num  << DEPCMDPAR0_FIFO_NUM_BIT;
	ep_cmd.param0 |= ep_type      << DEPCMDPAR0_EP_TYPE_BIT;
	ep_cmd.param0 |= max_pkt_size << DEPCMDPAR0_MAX_PKT_SIZE_BIT;

	dwc_ep_cmd(dev, ep_phy_num, &ep_cmd);
}

/* send stall command to ep */
void dwc_ep_cmd_stall(dwc_dev_t *dev, uint8_t ep_phy_num)
{
	dwc_ep_cmd_t ep_cmd;

	/* set command */
	ep_cmd.cmd = DEPCMD_CMD_SET_STALL;

	ep_cmd.param2 = 0;
	ep_cmd.param1 = 0;
	ep_cmd.param0 = 0;

	DBG("\nSTALLING.... ep_phy_num = %d\n", ep_phy_num);

	dwc_ep_cmd(dev, ep_phy_num, &ep_cmd);
}

/* clear stall */
void dwc_ep_cmd_clear_stall(dwc_dev_t *dev, uint8_t ep_phy_num)
{
	dwc_ep_cmd_t ep_cmd;

	/* set command */
	ep_cmd.cmd = DEPCMD_CMD_CLEAR_STALL;

	ep_cmd.param2 = 0;
	ep_cmd.param1 = 0;
	ep_cmd.param0 = 0;

	dwc_ep_cmd(dev, ep_phy_num, &ep_cmd);
}

/* send a start new config command */
void dwc_ep_cmd_start_new_config(dwc_dev_t *dev,
								 uint8_t ep_phy_num,
								 uint8_t resource_idx)
{
	dwc_ep_cmd_t ep_cmd;

	/* set command */
	ep_cmd.cmd                 = DEPCMD_CMD_START_NEW_CONF;
	ep_cmd.xfer_resource_index = resource_idx;

	ep_cmd.param2 = 0;
	ep_cmd.param1 = 0;
	ep_cmd.param0 = 0;

	dwc_ep_cmd(dev, ep_phy_num, &ep_cmd);
}

/******************** DWC Device related APIs *********************************/

/* generic api to send device command */
static void dwc_device_cmd(dwc_dev_t *dev, dwc_device_cmd_t *cmd)
{
	uint8_t active = REG_READ_FIELD(dev, DGCMD, CMDACT);

	ASSERT(active);

	REG_WRITE(dev, DGCMDPAR, cmd->param);
	REG_WRITE_FIELD(dev, DGCMD, CMDTYP, cmd->cmd);

	/* wait until active field is cleared. */
	while(!REG_READ_FIELD(dev, DGCMD, CMDACT));

	if(REG_READ_FIELD(dev, DGCMD, CMDSTATUS))
	{
		ERR("\n\n device command failed. \n\n");
		ASSERT(0);
	}
}

/* set periodic param */
void dwc_device_set_periodic_param(dwc_dev_t *dev, uint32_t val)
{
	dwc_device_cmd_t cmd;

	cmd.cmd   = DWC_DEV_CMD_SET_PERIODIC_PARAMS_VAL;
	cmd.param = val;

	/* send device command to set period param value */
	dwc_device_cmd(dev, &cmd);
}

/* set device address */
void dwc_device_set_addr(dwc_dev_t *dev, uint16_t addr)
{
	REG_WRITE_FIELD(dev, DCFG, DEVADDR, addr);
}

/* reset device */
void dwc_device_reset(dwc_dev_t *dev)
{
	/* start reset */
	REG_WRITE_FIELD(dev, DCTL, CSFTRST, 1);

	/* wait until done */
	while(REG_READ_FIELD(dev, DCTL, CSFTRST));
}

/* Run/Stop device: 1 == run. 0 == stop */
void dwc_device_run(dwc_dev_t *dev, uint8_t run)
{
	REG_WRITE_FIELD(dev, DCTL, RUN_STOP, run);
}

/* is device running? */
uint8_t dwc_device_run_status(dwc_dev_t *dev)
{
	return REG_READ_FIELD(dev, DCTL, RUN_STOP);
}

/******************** Managing various events *********************************/
/* event init:
   program event buffer address, size and reset event count to 0.
 */
void dwc_event_init(dwc_dev_t *dev)
{
	/* snps 8.2.2 */

	/* event buffer address */
	REG_WRITEI(dev, GEVNTADRLO, 0, (uint32_t) dev->event_buf.buf);
	REG_WRITEI(dev, GEVNTADRHI, 0, 0x0);

	/* set buffer size. assuming interrupt is always needed on new event,
	 * bit 31 is not set.
	 */
	REG_WRITEI(dev, GEVNTSIZ, 0, dev->event_buf.buf_size);

	/* reset count */
	REG_WRITEI(dev, GEVNTCOUNT, 0, 0x0);
}

/* event update index */
static void dwc_event_update_index(uint16_t *index, uint16_t max_count)
{
	if(*index == max_count)
	{
		/* we have read the last entry. Need to roll over for next reading.*/
		*index = 0;
	}
	else
	{
		*index += 1;
	}
}

/* Returns next event from event queue and the size of event
 * Event buffer is a circular buffer that the hardware populates when any of
 * the enabled event occurs. An interrupt is generated if interrupt is enabled
 * for that event.
 * This api returns the next valid event from the event buffer and updates event
 * buffer index.
 * Most events are 4 byte long
 * Note: caller must provide at least 12 bytes buffer in case the
 * next event is the special 12 byte event.
 */
uint16_t dwc_event_get_next(dwc_dev_t *dev, uint32_t *event)
{
	uint16_t count;
	uint16_t event_size = 0;
	uint32_t *buf;

	/* read the number of valid event data in event buffer. */
	count = REG_READI(dev, GEVNTCOUNT, 0);

	if(count == 0)
	{
		/* no events in buffer. */
		return event_size;
	}

	/* each event is at least 4 bytes long.
	 * make sure there is at least one event to read.
	 */
	ASSERT(count >= 4);

	/* get event buffer for this device */
	buf = dev->event_buf.buf;

	/* invalidate cached event buf data */
	arch_invalidate_cache_range((addr_t) (buf + dev->event_buf.index), 4);

	/* read next event */
	*event = buf[dev->event_buf.index];
	event_size += 4;
	dwc_event_update_index(&dev->event_buf.index, dev->event_buf.max_index);


	/* is this buffer overflow event? */
	if((DWC_EVENT_DEVICE_EVENT_ID(*event) == DWC_EVENT_DEVICE_EVENT_ID_BUFFER_OVERFLOW))
	{
		/* ouch... */
		ERR("\n Event buffer is full. Need to increase size.\n");
		ASSERT(0);
	}

	/* check for that special 12 byte event */
	if( DWC_EVENT_IS_DEVICE_EVENT(*event) &
		(DWC_EVENT_DEVICE_EVENT_ID(*event) == DWC_EVENT_DEVICE_EVENT_ID_VENDOR_DEVICE_TEST_LMP))
	{
		/* invalidate cached event buf data */
		arch_invalidate_cache_range((addr_t) (buf + dev->event_buf.index), 4);

		*(event + 1) = buf[dev->event_buf.index];
		event_size += 4;
		dwc_event_update_index(&dev->event_buf.index, dev->event_buf.buf_size);

		/* invalidate cached event buf data */
		arch_invalidate_cache_range((addr_t) (buf + dev->event_buf.index), 4);

		*(event + 1) = buf[dev->event_buf.index];
		event_size += 4;
		dwc_event_update_index(&dev->event_buf.index, dev->event_buf.buf_size);
	}

	return event_size;
}

/* Lets h/w know that we have processed "count" bytes of data from event buffer
 * and it can use that space for new events.
 * This must be done only after the event is processed.
 */
void dwc_event_processed(dwc_dev_t *dev, uint16_t count)
{
	REG_WRITEI(dev, GEVNTCOUNT, 0, count);
}

/* enable device event generation:
 * events - bit map of events defined in dwc_event_device_event_id_t
 */
void dwc_event_device_enable(dwc_dev_t *dev, uint32_t events)
{
	REG_WRITE(dev, DEVTEN, events);
}

/*************** Generic APIs affecting overall controller ********************/

/* reset HS and SS PHY's digital interface: UTMI + PIPE3 */
void dwc_phy_digital_reset(dwc_dev_t *dev)
{
	REG_WRITE_FIELDI(dev, GUSB2PHYCFG,  0, PHYSOFTRST, 1);
	REG_WRITE_FIELDI(dev, GUSB3PIPECTL, 0, PHYSOFTRST, 1);

	/* per HPG */
	udelay(100);

	REG_WRITE_FIELDI(dev, GUSB2PHYCFG,  0, PHYSOFTRST, 0);
	REG_WRITE_FIELDI(dev, GUSB3PIPECTL, 0, PHYSOFTRST, 0);

	/* per HPG */
	udelay(100);
}

void dwc_usb2_phy_soft_reset(dwc_dev_t *dev)
{
	REG_WRITE_FIELDI(dev, GUSB2PHYCFG,  0, PHYSOFTRST, 1);

	udelay(100);

	REG_WRITE_FIELDI(dev, GUSB2PHYCFG,  0, PHYSOFTRST, 0);

	udelay(100);
}

/* workaround_12 as described in HPG */
void dwc_ss_phy_workaround_12(dwc_dev_t *dev)
{
	/* 12. */
	if ( platform_is_8974() &&
		 (board_soc_version() < BOARD_SOC_VERSION2))
	{
		REG_WRITEI(dev, GUSB3PIPECTL, 0, 0x30C0003);
	}
}

/*  AXI master config */
void dwc_axi_master_config(dwc_dev_t *dev)
{
	uint32_t reg = 0;

	/* 17. */
	if ( platform_is_8974() &&
		 (board_soc_version() < BOARD_SOC_VERSION2))
	{
		reg = (DWC_GSBUSCFG0_INCR4BRSTENA_BMSK |
			   DWC_GSBUSCFG0_INCR8BRSTENA_BMSK |
			   DWC_GSBUSCFG0_INCR16BRSTENA_BMSK);

		REG_WRITE(dev, GSBUSCFG0, reg);
	}
}

/* read the controller id and version information */
uint32_t dwc_coreid(dwc_dev_t *dev)
{
	return REG_READ(dev, GSNPSID);
}

/* read the current connection speed. */
uint8_t dwc_connectspeed(dwc_dev_t *dev)
{
	return REG_READ_FIELD(dev, DSTS, CONNECTSPD);
}

/* disable all non-control EPs */
void dwc_ep_disable_non_control(dwc_dev_t *dev)
{
	uint32_t reg = REG_READ(dev, DALEPENA);

	/* clear all except the control IN and OUT ep */
	reg &= 0x3;

	REG_WRITE(dev, DALEPENA, reg);
}

/* disable a specific ep */
void dwc_ep_disable(dwc_dev_t *dev, uint8_t ep_phy_num)
{
	uint32_t reg = REG_READ(dev, DALEPENA);

	reg &= ~BIT(ep_phy_num);

	REG_WRITE(dev, DALEPENA, reg);
}

/* enable a specific ep */
void dwc_ep_enable(dwc_dev_t *dev, uint8_t ep_phy_num)
{
	uint32_t reg = REG_READ(dev, DALEPENA);

	reg |= BIT(ep_phy_num);

	REG_WRITE(dev, DALEPENA, reg);
}

/* global reset of controller.
 * 1 == put in reset. 0 == out of reset
 */
void dwc_reset(dwc_dev_t *dev, uint8_t reset)
{
	/* snps databook table 6-11 indicates this field to be used only for debug
	 * purpose. use dctl.softreset instead for devide mode.
	 * but hpg 4.4.2. 8.a says use this.
	 */
	REG_WRITE_FIELD(dev, GCTL, CORESOFTRESET, reset);

	/* per HPG */
	udelay(100);
}

/* initialize global control reg for device mode operation.
 * sequence numbers are as described in HPG.
 */
void dwc_gctl_init(dwc_dev_t *dev)
{
	/* 16. */
	/* a. default value is good for RAM clock */
	/* b. default value is good for Disable Debug Attach */
	REG_WRITE_FIELD(dev, GCTL, DEBUGATTACH, 0);

	/* c & d: disable loopback/local loopback
	 * TODO: possibly for older version. no such fields in GCTL
	 */

	/* e. no soft reset. */
	REG_WRITE_FIELD(dev, GCTL, CORESOFTRESET, 0);

	/* f. set port capability direction: device */
	REG_WRITE_FIELD(dev, GCTL, PRTCAPDIR, 0x2);

	/* g. set scale down value */
	REG_WRITE_FIELD(dev, GCTL, FRMSCLDWN, 0x0);

	/* h. enable multiple attempts for SS connection */
	REG_WRITE_FIELD(dev, GCTL, U2RSTECN, 1);

	/* i. set power down scale of snps phy */
	REG_WRITE_FIELD(dev, GCTL, PWRDNSCALE, 0x2);

	/* j. clear SOFITPSYNC bit */
	REG_WRITE_FIELD(dev, GCTL, SOFITPSYNC, 0);
}
