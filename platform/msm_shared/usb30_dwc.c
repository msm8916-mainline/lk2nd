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

/* This code implements the DesignWare Cores USB 3.0 driver. This file only
 * implements the state machine and higher level logic as described in the
 * SNPS DesignWare Cores SS USB3.0 Controller databook.
 * Another file (dwc_hw.c) file implements the functions to interact with
 * the dwc hardware registers directly.
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
#include <usb30_dwc.h>
#include <usb30_dwc_hw.h>
#include <usb30_dwc_hwio.h>

//#define DEBUG_USB

#ifdef DEBUG_USB
#define DBG(...) dprintf(ALWAYS, __VA_ARGS__)
#else
#define DBG(...)
#endif

#define ERR(...) dprintf(ALWAYS, __VA_ARGS__)

/* debug only: enum string lookup for debug purpose */
char* ss_link_state_lookup[20];
char* hs_link_state_lookup[20];
char* event_lookup_device[20];
char* event_lookup_ep[20];
char* dev_ctrl_state_lookup[20];
char* ep_state_lookup[20];
char* speed_lookup[20];
char* cmd_lookup[20];

/* debug only: initialize the enum lookup */
void dwc_debug_lookup_init(void)
{
	/* EP event */
	event_lookup_ep[DWC_EVENT_EP_CMD_COMPLETE]     = "DWC_EVENT_EP_CMD_COMPLETE    ";
	event_lookup_ep[DWC_EVENT_EP_XFER_NOT_READY]   = "DWC_EVENT_EP_XFER_NOT_READY  ";
	event_lookup_ep[DWC_EVENT_EP_XFER_IN_PROGRESS] = "DWC_EVENT_EP_XFER_IN_PROGRESS";
	event_lookup_ep[DWC_EVENT_EP_XFER_COMPLETE]    = "DWC_EVENT_EP_XFER_COMPLETE   ";

	/* Device event */
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_VENDOR_DEVICE_TEST_LMP] = "DWC_EVENT_DEVICE_EVENT_ID_VENDOR_DEVICE_TEST_LMP";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_BUFFER_OVERFLOW]        = "DWC_EVENT_DEVICE_EVENT_ID_BUFFER_OVERFLOW       ";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_GENERIC_CMD_COMPLETE]   = "DWC_EVENT_DEVICE_EVENT_ID_GENERIC_CMD_COMPLETE  ";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_ERRATIC_ERROR]          = "DWC_EVENT_DEVICE_EVENT_ID_ERRATIC_ERROR         ";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_SOF]                    = "DWC_EVENT_DEVICE_EVENT_ID_SOF                   ";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_SUSPEND_ENTRY]          = "DWC_EVENT_DEVICE_EVENT_ID_SUSPEND_ENTRY         ";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_HIBER]                  = "DWC_EVENT_DEVICE_EVENT_ID_HIBER                 ";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_WAKEUP]                 = "DWC_EVENT_DEVICE_EVENT_ID_WAKEUP                ";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_USB_LINK_STATUS_CHANGE] = "DWC_EVENT_DEVICE_EVENT_ID_USB_LINK_STATUS_CHANGE";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_CONNECT_DONE]           = "DWC_EVENT_DEVICE_EVENT_ID_CONNECT_DONE          ";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_USB_RESET]              = "DWC_EVENT_DEVICE_EVENT_ID_USB_RESET             ";
	event_lookup_device[DWC_EVENT_DEVICE_EVENT_ID_DISCONNECT]             = "DWC_EVENT_DEVICE_EVENT_ID_DISCONNECT            ";

	/* Control state */
	dev_ctrl_state_lookup[EP_FSM_INIT]            = "EP_FSM_INIT           ";
	dev_ctrl_state_lookup[EP_FSM_SETUP]           = "EP_FSM_SETUP          ";
	dev_ctrl_state_lookup[EP_FSM_CTRL_DATA]       = "EP_FSM_CTRL_DATA      ";
	dev_ctrl_state_lookup[EP_FSM_WAIT_FOR_HOST_2] = "EP_FSM_WAIT_FOR_HOST_2";
	dev_ctrl_state_lookup[EP_FSM_WAIT_FOR_HOST_3] = "EP_FSM_WAIT_FOR_HOST_3";
	dev_ctrl_state_lookup[EP_FSM_STATUS_2]        = "EP_FSM_STATUS_2       ";
	dev_ctrl_state_lookup[EP_FSM_STATUS_3]        = "EP_FSM_STATUS_3       ";
	dev_ctrl_state_lookup[EP_FSM_STALL]           = "EP_FSM_STALL          ";

	/* EP state */
	ep_state_lookup[EP_STATE_INIT]                = "EP_STATE_INIT";
	ep_state_lookup[EP_STATE_INACTIVE]            = "EP_STATE_INACTIVE";
	ep_state_lookup[EP_STATE_START_TRANSFER]      = "EP_STATE_START_TRANSFER";
	ep_state_lookup[EP_STATE_XFER_IN_PROG]        = "EP_STATE_XFER_IN_PROG";

	/* HS link status */
	hs_link_state_lookup[ON]            = "ON           ";
	hs_link_state_lookup[L1]            = "L1           ";
	hs_link_state_lookup[L2]            = "L2           ";
	hs_link_state_lookup[DISCONNECTED]  = "DISCONNECTED ";
	hs_link_state_lookup[EARLY_SUSPEND] = "EARLY_SUSPEND";
	hs_link_state_lookup[RESET]         = "RESET        ";
	hs_link_state_lookup[RESUME]        = "RESUME       ";

	/* SS link status */
	ss_link_state_lookup[U0]            = "U0          ";
	ss_link_state_lookup[U1]            = "U1          ";
	ss_link_state_lookup[U2]            = "U2          ";
	ss_link_state_lookup[U3]            = "U3          ";
	ss_link_state_lookup[SS_DIS]        = "SS_DIS      ";
	ss_link_state_lookup[RX_DET]        = "RX_DET      ";
	ss_link_state_lookup[SS_INACT]      = "SS_INACT    ";
	ss_link_state_lookup[POLL]          = "POLL        ";
	ss_link_state_lookup[RECOV]         = "RECOV       ";
	ss_link_state_lookup[HRESET]        = "HRESET      ";
	ss_link_state_lookup[CMPLY]         = "CMPLY       ";
	ss_link_state_lookup[LPBK]          = "LPBK        ";
	ss_link_state_lookup[RESUME_RESET]  = "RESUME_RESET";

	/* connection speed */
	speed_lookup[DSTS_CONNECTSPD_HS]  = "DSTS_CONNECTSPD_HS ";
	speed_lookup[DSTS_CONNECTSPD_FS1] = "DSTS_CONNECTSPD_FS1";
	speed_lookup[DSTS_CONNECTSPD_LS]  = "DSTS_CONNECTSPD_LS ";
	speed_lookup[DSTS_CONNECTSPD_FS2] = "DSTS_CONNECTSPD_FS1";
	speed_lookup[DSTS_CONNECTSPD_SS]  = "DSTS_CONNECTSPD_SS ";

	/* dwc command */
	cmd_lookup[DEPCMD_CMD_SET_EP_CONF]     = "DEPCMD_CMD_SET_EP_CONF    ";
	cmd_lookup[DEPCMD_CMD_SET_TR_CONF]     = "DEPCMD_CMD_SET_TR_CONF    ";
	cmd_lookup[DEPCMD_CMD_GET_EP_STATE]    = "DEPCMD_CMD_GET_EP_STATE   ";
	cmd_lookup[DEPCMD_CMD_SET_STALL]       = "DEPCMD_CMD_SET_STALL      ";
	cmd_lookup[DEPCMD_CMD_CLEAR_STALL]     = "DEPCMD_CMD_CLEAR_STALL    ";
	cmd_lookup[DEPCMD_CMD_START_TRANSFER]  = "DEPCMD_CMD_START_TRANSFER ";
	cmd_lookup[DEPCMD_CMD_UPDATE_TRANSFER] = "DEPCMD_CMD_UPDATE_TRANSFER";
	cmd_lookup[DEPCMD_CMD_END_TRANSFER]    = "DEPCMD_CMD_END_TRANSFER   ";
	cmd_lookup[DEPCMD_CMD_START_NEW_CONF]  = "DEPCMD_CMD_START_NEW_CONF ";
}

static void dwc_print_ep_event_details(dwc_dev_t *dev, uint32_t *event)
{
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t  ep_phy_num                = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	uint8_t  event_ctrl_stage          = DWC_EVENT_EP_EVENT_CTRL_STAGE(*event);
	uint8_t  event_status              = DWC_EVENT_EP_EVENT_STATUS(*event);
	uint8_t  xfer_res_idx              = DWC_EVENT_EP_EVENT_XFER_RES_IDX(*event);
	uint16_t event_param               = DWC_EVENT_EP_EVENT_PARAM(*event);
	dwc_dep_cmd_id_t cmd               = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);

	ERR("\n\n");
	ERR("EP event (0x%x) details\n", *event);
	ERR("event_id         = %d, %s\n", event_id, event_lookup_ep[event_id]);
	ERR("ep_phy_num       = %d\n", ep_phy_num);
	ERR("event_ctrl_stage = %d, %s\n", event_ctrl_stage, dev_ctrl_state_lookup[event_ctrl_stage]);
	ERR("event_status     = %d\n", event_status);
	ERR("xfer_res_idx     = %d\n", xfer_res_idx);
	ERR("event_param      = %d\n", event_param);
	ERR("event_cmd_type   = %d, %s\n", cmd, cmd_lookup[cmd]);
	ERR("\n");
}

static void dwc_print_ep_details(dwc_dev_t *dev, uint8_t ep_index)
{
	ERR("\n");
	ERR("EP ( index = %d) status: \n", ep_index);
	ERR("phy_num      = %d \n", dev->ep[ep_index].phy_num);
	ERR("usb ep num   = %d \n", dev->ep[ep_index].number);
	ERR("dir          = %d \n", dev->ep[ep_index].dir);
	ERR("type         = %d \n", dev->ep[ep_index].type);
	ERR("resource_idx = %d \n", dev->ep[ep_index].resource_idx);
	ERR("trb_queued   = %d \n", dev->ep[ep_index].trb_queued);
	ERR("bytes_queued = %d \n", dev->ep[ep_index].bytes_queued);
	ERR("state        = %d, %s \n", dev->ep[ep_index].state, ep_state_lookup[dev->ep[ep_index].state]);
	ERR("ep req len   = %d trbctl = %d\n", dev->ep[ep_index].req.len, dev->ep[ep_index].req.trbctl);
	ERR("\n");
}

static void dwc_print_current_state(dwc_dev_t *dev)
{
	uint8_t i = 0;

	ERR("\n");
	ERR("dwc core = 0x%x\n", dev->core_id);
	ERR("ctrl_state = %d, %s\n", dev->ctrl_state, dev_ctrl_state_lookup[dev->ctrl_state]);

	for (i = 0; i < DWC_MAX_NUM_OF_EP; i++)
	{
		dwc_print_ep_details(dev, i);
	}

	ERR("\n");
}

/******************************** DWC global **********************************/
/* Initialize DWC driver. */
dwc_dev_t* dwc_init(dwc_config_t *config)
{
	dwc_dev_t *dev = (dwc_dev_t*) malloc(sizeof(dwc_dev_t));
	ASSERT(dev);

	memset(dev, 0, sizeof(dwc_dev_t));

	/* save config info */
	dev->base                = config->base;
	dev->event_buf.buf       = config->event_buf;
	dev->event_buf.buf_size  = config->event_buf_size;
	dev->event_buf.index     = 0;
	dev->event_buf.max_index = (config->event_buf_size)/4 - 1; /* (max num of 4 byte events) - 1 */

	dev->notify_context      = config->notify_context;
	dev->notify              = config->notify;

	/* allocate buffer for receiving setup packet */
	dev->setup_pkt           =  memalign(CACHE_LINE, ROUNDUP(DWC_SETUP_PKT_LEN, CACHE_LINE));
	ASSERT(dev->setup_pkt);

	/* callback function to handler setup packet */
	dev->setup_context       = config->setup_context;
	dev->setup_handler       = config->setup_handler;

	/* read core version from h/w */
	dev->core_id = dwc_coreid(dev);

	/* register for interrupt */
	register_int_handler(USB30_EE1_IRQ, dwc_irq_handler_ee1, dev);

	/* note: only for debug */
	dwc_debug_lookup_init();

	return dev;
}

/* interrupt handler */
static enum handler_return dwc_irq_handler_ee1(void* context)
{
	dwc_dev_t *dev;
	uint16_t   event_size; /* number of bytes used by the event */
	uint32_t   event[3] = {0x0, 0x0, 0x0};

	/* get the device on which this interrupt occurred */
	dev = (dwc_dev_t *) context;

	/* while there are events to be processed */
	while((event_size = dwc_event_get_next(dev, event)))
	{
		/* device event? */
		if(DWC_EVENT_IS_DEVICE_EVENT(*event))
		{
			/* handle device events */
			dwc_event_handler_device(dev, event);
		}
		else
		{
			/* endpoint event */
			uint8_t ep_phy_num = DWC_EVENT_EP_EVENT_EP_NUM(*event);

			if ((ep_phy_num == 0) ||
				(ep_phy_num == 1))
			{
				/* handle control ep event */
				dwc_event_handler_ep_ctrl(dev, event);
			}
			else
			{
				/* handle non-control ep event. only bulk ep is supported.*/
				dwc_event_handler_ep_bulk(dev, event);
			}
		}

		/* update number of bytes processed */
		dwc_event_processed(dev, event_size);
	}

	return INT_NO_RESCHEDULE;
}

/*====================== DWC Event configuration/handling functions========== */

/* handles all device specific events */
void dwc_event_handler_device(dwc_dev_t *dev, uint32_t *event)
{
	dwc_event_device_event_id_t event_id = DWC_EVENT_DEVICE_EVENT_ID(*event);

	DBG("\nDEVICE_EVENT: %s in %s \n", event_lookup_device[event_id],
									   dev_ctrl_state_lookup[dev->ctrl_state]);

	switch (event_id)
	{
	case DWC_EVENT_DEVICE_EVENT_ID_VENDOR_DEVICE_TEST_LMP:
	case DWC_EVENT_DEVICE_EVENT_ID_BUFFER_OVERFLOW:
	case DWC_EVENT_DEVICE_EVENT_ID_GENERIC_CMD_COMPLETE:
	case DWC_EVENT_DEVICE_EVENT_ID_ERRATIC_ERROR:
	case DWC_EVENT_DEVICE_EVENT_ID_SOF:
	case DWC_EVENT_DEVICE_EVENT_ID_SUSPEND_ENTRY:
	case DWC_EVENT_DEVICE_EVENT_ID_HIBER:
	case DWC_EVENT_DEVICE_EVENT_ID_WAKEUP:
		break;
	case DWC_EVENT_DEVICE_EVENT_ID_USB_LINK_STATUS_CHANGE:
		{
			dwc_event_device_link_status_change(dev, event);
		}
		break;
	case DWC_EVENT_DEVICE_EVENT_ID_CONNECT_DONE:
		{
			dwc_event_device_connect_done(dev);
		}
		break;
	case DWC_EVENT_DEVICE_EVENT_ID_USB_RESET:
		{
			dwc_event_device_reset(dev);
		}
		break;
	case DWC_EVENT_DEVICE_EVENT_ID_DISCONNECT:
		{
			dwc_event_device_disconnect(dev);
		}
		break;

	default:
		ERR("\nUnhandled DEVICE_EVENT: %s in %s \n",
			event_lookup_device[event_id],
			dev_ctrl_state_lookup[dev->ctrl_state]);
		dwc_print_current_state(dev);
		ASSERT(0);
	}
}

/* handle link status change event: does nothing for now.
 * only for debug purpose.
 */
static void dwc_event_device_link_status_change(dwc_dev_t *dev, uint32_t *event)
{
#ifdef DEBUG_USB
	uint8_t event_info = DWC_EVENT_DEVICE_EVENT_INFO(*event);
	uint8_t ss_event   = DWC_EVENT_DEVICE_EVENT_INFO_SS_EVENT(*event);
	uint8_t link_state = DWC_EVENT_DEVICE_EVENT_INFO_LINK_STATE(event_info);

	if(ss_event)
	{
		DBG("\n SS link state = %s (%d)\n", ss_link_state_lookup[link_state], link_state);
	}
	else
	{
		DBG("\n HS link state = %s (%d)\n", hs_link_state_lookup[link_state], link_state);
	}
#endif
}

/* handle disconnect event */
static void dwc_event_device_disconnect(dwc_dev_t *dev)
{
	/* inform client that device is disconnected */
	if (dev->notify)
		dev->notify(dev->notify_context, DWC_NOTIFY_EVENT_DISCONNECTED);
}

/* handle connect event: snps 8.1.3 */
static void dwc_event_device_connect_done(dwc_dev_t *dev)
{
	uint8_t speed;
	uint16_t max_pkt_size = 0;
	dwc_notify_event_t dwc_event = DWC_NOTIFY_EVENT_DISCONNECTED;

	/* get connection speed */
	speed = dwc_connectspeed(dev);

	switch (speed)
	{
	case DSTS_CONNECTSPD_SS:
		{
			max_pkt_size = 512;
			dwc_event    = DWC_NOTIFY_EVENT_CONNECTED_SS;
		}
		break;
	case DSTS_CONNECTSPD_HS:
		{
			max_pkt_size = 64;
			dwc_event    = DWC_NOTIFY_EVENT_CONNECTED_HS;
		}
		break;
	case DSTS_CONNECTSPD_FS1:
	case DSTS_CONNECTSPD_FS2:
		{
			max_pkt_size = 64;
			dwc_event    = DWC_NOTIFY_EVENT_CONNECTED_FS;
		}
		break;
	case DSTS_CONNECTSPD_LS:
		{
			max_pkt_size = 8;
			dwc_event    = DWC_NOTIFY_EVENT_CONNECTED_LS;
		}
		break;
	default:
		ASSERT(0);
	}

	DBG("\nspeed = %d : %s max_pkt_size %d \n", speed,
												speed_lookup[speed],
												max_pkt_size);


	/* save max pkt size for control endpoints */
	dev->ep[0].max_pkt_size = max_pkt_size;
	dev->ep[1].max_pkt_size = max_pkt_size;

	/* Issue a DEPCFG command (with Config Action set to "Modify") for
	 * physical endpoints 0 & 1 using the same endpoint characteristics from
	 * Power-On Reset, but set
	 * MaxPacketSize to 512 (SuperSpeed), 64 (High-Speed),
	 * 8/16/32/64 (Full-Speed), or 8 (Low-Speed).
	 */
	dwc_ep_cmd_set_config(dev, 0, SET_CONFIG_ACTION_MODIFY);
	dwc_ep_cmd_set_config(dev, 1, SET_CONFIG_ACTION_MODIFY);

	/* TODO: future optimization:
	 * GUSB2CFG/GUSB3PIPECTL
	 * Depending on the connected speed, write to the other PHY's control
	 * register to suspend it.
	 * GTXFIFOSIZn (optional) Based on the new MaxPacketSize of IN endpoint 0,
	 * software may choose to re-allocate
	 * the TX FIFO sizes by writing to these registers.
	 */

	/* inform client that device is connected */
	if (dev->notify)
		dev->notify(dev->notify_context, dwc_event);
}

/* handle usb reset event:
 * snps 8.1.2:
 * Set DevAddr to 0
 * end transfer for any active transfers (except for the default control EP)
 */
void dwc_event_device_reset(dwc_dev_t *dev)
{
	/* set dev address to 0 */
	dwc_device_set_addr(dev, 0x0);

	/* Send "stop transfer" on any non-control ep
	 * which has a transfer in progress: snps 8.2.5
	 */
	for (uint8_t ep_index = 2; ep_index < DWC_MAX_NUM_OF_EP; ep_index++)
	{
		dwc_ep_t *ep = &dev->ep[ep_index];
		ASSERT(ep != NULL);

		DBG("\n RESET on EP = %d while state = %s", ep_index,
													ep_state_lookup[ep->state]);

		if ((ep->state == EP_STATE_START_TRANSFER) ||
			(ep->state == EP_STATE_XFER_IN_PROG))
		{
			DBG("\n NEED to do end transfer");

			dwc_ep_cmd_end_transfer(dev, ep->phy_num);
		}
	}

	/* inform client that device is offline */
	if (dev->notify)
	{
		DBG("\n calling Notify for OFFLINE event.\n");
		dev->notify(dev->notify_context, DWC_NOTIFY_EVENT_OFFLINE);
	}
}

/* handle control endpoint specific events:
 * implements the control transfer programming model as described
 * in snps chapter 8.4, figure 8-2.
 */
void dwc_event_handler_ep_ctrl(dwc_dev_t *dev, uint32_t *event)
{
#ifdef DEBUG_USB
	uint8_t                 ep_phy_num   = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id     = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t                 event_status = DWC_EVENT_EP_EVENT_STATUS(*event);
	uint16_t                event_param  = DWC_EVENT_EP_EVENT_PARAM(*event);

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	DBG("\n\n\n++EP_EVENT: %s in ctrl_state: %s ep_state[%d]: %s",
		event_lookup_ep[event_id],
		dev_ctrl_state_lookup[dev->ctrl_state],
		ep_phy_num,
		ep_state_lookup[ep->state]);

	DBG("\n ep_phy_num = %d param = 0x%x status = 0x%x", ep_phy_num,
														 event_param,
														 event_status);
#endif

	/* call the handler for the current control state */
	switch (dev->ctrl_state)
	{
	case EP_FSM_SETUP:
		{
			dwc_event_handler_ep_ctrl_state_setup(dev, event);
		}
		break;
	case EP_FSM_CTRL_DATA:
		{
			dwc_event_handler_ep_ctrl_state_data(dev, event);
		}
		break;
	case EP_FSM_WAIT_FOR_HOST_2:
		{
			dwc_event_handler_ep_ctrl_state_wait_for_host_2(dev, event);
		}
		break;
	case EP_FSM_WAIT_FOR_HOST_3:
		{
			dwc_event_handler_ep_ctrl_state_wait_for_host_3(dev, event);
		}
		break;
	case EP_FSM_STATUS_2:
		{
			dwc_event_handler_ep_ctrl_state_status_2(dev, event);
		}
		break;
	case EP_FSM_STATUS_3:
		{
			dwc_event_handler_ep_ctrl_state_status_3(dev, event);
		}
		break;
	case EP_FSM_STALL:
		{
			dwc_event_handler_ep_ctrl_state_stall(dev, event);
		}
		break;

	default:
		ERR("Invalid ctrl_state = %d\n", dev->ctrl_state);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
		ASSERT(0);
	}

#ifdef DEBUG_USB
	DBG("\n--EP_EVENT: %s in ctrl_state: %s ep_state[%d]: %s",
		event_lookup_ep[event_id],
		dev_ctrl_state_lookup[dev->ctrl_state],
		ep_phy_num,
		ep_state_lookup[ep->state]);
#endif
}

/* check status of transfer:
 * returns TRB status: non-zero value indicates failure to complete transfer.
 * Also updates the "bytes_in_buf". This field indicates the number of bytes
 * still remaining to be transferred. This field will be zero when all the
 * requested data is transferred.
 */
uint8_t dwc_event_check_trb_status(dwc_dev_t *dev,
								   uint32_t  *event,
								   uint8_t    index,
								   uint32_t  *bytes_in_buf)
{
	uint8_t status        = 0;
	uint8_t trb_updated   = 0;
	uint8_t event_status  = DWC_EVENT_EP_EVENT_STATUS(*event);

	ASSERT(index < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep          = &dev->ep[index];
	ASSERT(ep != NULL);

	dwc_trb_t *trb        = ep->trb;
	uint32_t num_of_trb   = ep->trb_queued;
	uint32_t bytes_remaining = 0;

	/* sanity ck. */
	ASSERT(num_of_trb);

	/* invalidate trb data before reading */
	arch_invalidate_cache_range((addr_t) trb, sizeof(dwc_trb_t)*num_of_trb);

	while (num_of_trb)
	{
		bytes_remaining += REG_READ_FIELD_LOCAL(&trb->f3, TRB_F3, BUFSIZ);

		/* The first non-zero status indicates the transfer status. Update
		 * "status" only once but still go through all the TRBs to find out
		 * the bytes still remaining to be transferred.
		 */
		if (!status)
		{
			status  = REG_READ_FIELD_LOCAL(&trb->f3, TRB_F3, TRBSTS);
		}

		if ((event_status & DWC_XFER_COMPLETE_EVT_STATUS_SHORT_PKT) &&
			(REG_READ_FIELD_LOCAL(&trb->f4, TRB_F4, HWO)))
		{
			/* This TRB needs to be reclaimed since transfer completed due to
			 * reception of a short pkt.
			 * "fast-forward" condition as described in snps 8.2.3.2.
			 */
			DBG("\n TRB needs to be reclaimed by sw. trb = 0x%x\n", (uint32_t) trb);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, HWO, 0x0);
			trb_updated = 1;
		}

		/* point to next trb */
		trb++;

		/* decrement trb count */
		num_of_trb--;
	}

	/* flush out any updates to trb before continuing */
	if (trb_updated)
	{
		arch_clean_invalidate_cache_range((addr_t) ep->trb,
										  sizeof(dwc_trb_t)*ep->trb_queued);
	}

	/* reset the EP's queued trb count */
	ep->trb_queued = 0;

	*bytes_in_buf = bytes_remaining;

	DBG("\n trb_status: %d total buf size = 0x%x \n", status, *bytes_in_buf);

	return status;
}

/* handle all events occurring in Control-Setup state */
static void dwc_event_handler_ep_ctrl_state_setup(dwc_dev_t *dev,
												  uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t event_status               = DWC_EVENT_EP_EVENT_STATUS(*event);

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
		{
			dwc_dep_cmd_id_t cmd = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);

			if (cmd == DEPCMD_CMD_START_TRANSFER)
			{
				ASSERT(ep->state == EP_STATE_START_TRANSFER);

				ASSERT(event_status == 0);

				/* save the resource id assigned to this ep. */
				ep->state        = EP_STATE_XFER_IN_PROG;
				ep->resource_idx = DWC_EVENT_EP_EVENT_XFER_RES_IDX(*event);
			}
			else
			{
				DBG("\n cmd = %s has no action. ignored.", cmd_lookup[cmd]);
			}
		}
		break;

	case DWC_EVENT_EP_XFER_NOT_READY:
		{
			/* attempting to start data/status before setup. snps 8.4.2 #2 */
			DBG("\nattempting to start data/status before setup. stalling..\n");

			dwc_ep_cmd_stall(dev, ep_phy_num);

			/* new state is stall */
			dev->ctrl_state = EP_FSM_STALL;
		}
		break;

	case DWC_EVENT_EP_XFER_COMPLETE:
		{
			uint32_t bytes_remaining = 0;
			uint8_t status = 0;

			/* cannot happen on any other ep */
			ASSERT(ep_phy_num == 0);

			/* Assert if ep state is not xfer_in_prog. fatal error. */
			ASSERT(ep->state == EP_STATE_XFER_IN_PROG);

			/* update ep state to inactive. */
			ep->state = EP_STATE_INACTIVE;

			/* check transfer status. */
			status = dwc_event_check_trb_status(dev,
												event,
												DWC_EP_PHY_TO_INDEX(ep_phy_num),
												&bytes_remaining);

			if (status || bytes_remaining)
			{
				/* transfer failed. queue another transfer. */
				dwc_ep_ctrl_state_setup_enter(dev);
			}
			else
			{
				int ret;
				uint8_t *data = dev->setup_pkt; /* setup pkt data */

				/* invalidate any cached setup data before reading */
				arch_invalidate_cache_range((addr_t) data, DWC_SETUP_PKT_LEN);

				/* call setup handler */
				ret = dev->setup_handler(dev->setup_context, data);

				if (ret == DWC_SETUP_2_STAGE)
				{
					/* this is a 2 stage setup. */
					dev->ctrl_state = EP_FSM_WAIT_FOR_HOST_2;
				}
				else if (ret == DWC_SETUP_3_STAGE)
				{
					/* this is a 3 stage setup. */
					dev->ctrl_state = EP_FSM_CTRL_DATA;
				}
				else
				{
					/* bad setup bytes. stall */
					dwc_ep_cmd_stall(dev, ep_phy_num);

					/* new state is stall */
					dev->ctrl_state = EP_FSM_STALL;
				}
			}
		}
		break;
	case DWC_EVENT_EP_XFER_IN_PROGRESS:
	default:
		/* event is not expected in this state */
		ERR("\n Ignore the unexpected EP event: %s\n", event_lookup_ep[event_id]);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
	}
}


/* handle all events occurring in Control-Data state */
static void dwc_event_handler_ep_ctrl_state_data(dwc_dev_t *dev,
												 uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t event_ctrl_stage           = DWC_EVENT_EP_EVENT_CTRL_STAGE(*event);
	uint8_t event_status               = DWC_EVENT_EP_EVENT_STATUS(*event);

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
		{
			dwc_dep_cmd_id_t cmd = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);

			if (cmd == DEPCMD_CMD_START_TRANSFER)
			{
				ASSERT(ep->state == EP_STATE_START_TRANSFER);

				ASSERT(event_status == 0);

				/* save the resource id assigned to this ep. */
				ep->state        = EP_STATE_XFER_IN_PROG;
				ep->resource_idx = DWC_EVENT_EP_EVENT_XFER_RES_IDX(*event);
			}
			else
			{
				DBG("\n cmd = %s has no action. ignored.", cmd_lookup[cmd]);
			}
		}
		break;

	case DWC_EVENT_EP_XFER_NOT_READY:
		{
			/* confirm that this is control data request.
			 * control_status_request is invalid event in this state.
			 * assert if it ever occurs.
			 * something must be wrong in fsm implementation.
			 */
			ASSERT(event_ctrl_stage == CONTROL_DATA_REQUEST);

			/* In this state, the ep must be in transfer state.
			 * otherwise this came on an ep that we are not expecting any data.
			 */
			if((ep->state == EP_STATE_START_TRANSFER) ||
			   (ep->state == EP_STATE_XFER_IN_PROG))
			{
				DBG("\n Host requested data on ep_phy_num = %d."
					"Transfer already started. No action....", ep_phy_num);
			}
			else
			{
				/* host attempting to move data in wrong direction.
				 * end transfer for the direction that we started and stall.
				 */
				uint8_t end_ep_phy_num;

				/* end the other ep */
				end_ep_phy_num = (ep_phy_num == 0) ? 1 : 0;

				DBG("\nAttempting to move data in wrong direction. stalling. ");

				dwc_ep_cmd_end_transfer(dev, end_ep_phy_num);

				/* stall */
				dwc_ep_cmd_stall(dev, end_ep_phy_num);

				/* move to stall state. */
				dev->ctrl_state  = EP_FSM_STALL;
			}
		}
		break;
	case DWC_EVENT_EP_XFER_COMPLETE:
		{
			uint32_t bytes_remaining;
			uint8_t status;

			/* should never happen in any other state.
			 * something wrong in fsm implementation.
			 */
			ASSERT(ep->state == EP_STATE_XFER_IN_PROG);

			/* transfer is complete */
			ep->state = EP_STATE_INACTIVE;

			/* check transfer status */
			status = dwc_event_check_trb_status(dev,
												event,
												DWC_EP_PHY_TO_INDEX(ep_phy_num),
												&bytes_remaining);

			if (status || bytes_remaining)
			{
				DBG("\n\n ********DATA TRANSFER FAILED ************* "
					"status = %d  bytes_remaining = %d\n\n",
					status, bytes_remaining);
			}

			/* wait for host to request status */
			dev->ctrl_state = EP_FSM_WAIT_FOR_HOST_3;
		}
		break;
	case DWC_EVENT_EP_XFER_IN_PROGRESS:
	default:
		 /* event is not expected in this state */
		ERR("\n Ignore the unexpected EP event: %s\n", event_lookup_ep[event_id]);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
	}
}

/* handle all events occurring in Wait-for-Host-2 state */
static void dwc_event_handler_ep_ctrl_state_wait_for_host_2(dwc_dev_t *dev,
															uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t event_ctrl_stage           = DWC_EVENT_EP_EVENT_CTRL_STAGE(*event);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
	{
		dwc_dep_cmd_id_t cmd = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);
		DBG("\n cmd = %s has no action. ignored.", cmd_lookup[cmd]);
	}
		break;
	case DWC_EVENT_EP_XFER_NOT_READY:
		{
			if (event_ctrl_stage == CONTROL_DATA_REQUEST)
			{
				DBG("\n\n attempting to start data when setup did not indicate"
					"data stage. stall...\n\n");

				dwc_ep_cmd_stall(dev, ep_phy_num);

				/* move to stall state. */
				dev->ctrl_state  = EP_FSM_STALL;
			}
			else if (event_ctrl_stage == CONTROL_STATUS_REQUEST)
			{
				/* status cannot happen on phy = 0 */
				ASSERT(ep_phy_num == 1);

				dwc_request_t req;

				req.callback = 0x0;
				req.context  = 0x0;
				req.data     = 0x0;
				req.len      = 0x0;
				req.trbctl   = TRBCTL_CONTROL_STATUS_2;

				dwc_request_queue(dev, ep_phy_num, &req);

				dev->ctrl_state = EP_FSM_STATUS_2;
			}
			else
			{
				ASSERT(0);
			}
		}
		break;
	case DWC_EVENT_EP_XFER_IN_PROGRESS:
	case DWC_EVENT_EP_XFER_COMPLETE:
	default:
		/* event not expected in this state. */
		ERR("\n Ignore the unexpected EP event: %s\n", event_lookup_ep[event_id]);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
	}
}

/* handle all events occurring in Wait-for-Host-3 state */
static void dwc_event_handler_ep_ctrl_state_wait_for_host_3(dwc_dev_t *dev,
															uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t event_ctrl_stage           = DWC_EVENT_EP_EVENT_CTRL_STAGE(*event);

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
	{
		dwc_dep_cmd_id_t cmd = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);
		DBG("\n cmd = %s has no action. ignored.", cmd_lookup[cmd]);
	}
		break;
	case DWC_EVENT_EP_XFER_NOT_READY:
		{
			if (event_ctrl_stage == CONTROL_DATA_REQUEST)/* data request */
			{
				if (ep->state == EP_STATE_START_TRANSFER ||
								ep->state == EP_STATE_XFER_IN_PROG) {
					/*
					 * special case handling when data stage transfer length
					 * was exact multiple of max_pkt_size.
					 * Need to setup a TRB to complete data stage with a zero
					 * length pkt transfer.
					 */

					dwc_request_t req;

					req.callback = 0x0;
					req.context  = 0x0;
					req.data     = 0x0;
					req.len      = 0x0;
					req.trbctl   = TRBCTL_CONTROL_DATA;

					DBG("\n Sending the elp to host as the end of xfer\n");
					dwc_request_queue(dev, ep_phy_num, &req);
					dev->ctrl_state = EP_FSM_CTRL_DATA;
				} else {
					DBG("\n attempting to start data when setup did not indicate"
						"data stage. stall...\n\n");
					dwc_ep_cmd_stall(dev, ep_phy_num);
					dev->ctrl_state  = EP_FSM_STALL;
				}
			}
			else if (event_ctrl_stage ==  CONTROL_STATUS_REQUEST)/* stat req */
			{
				dwc_request_t req;

				req.callback = 0x0;
				req.context  = 0x0;
				req.data     = 0x0;
				req.len      = 0x0;
				req.trbctl   = TRBCTL_CONTROL_STATUS_3;

				dwc_request_queue(dev, ep_phy_num, &req);

				dev->ctrl_state = EP_FSM_STATUS_3;
			}
			else
			{
				ASSERT(0);
			}
		}
		break;
	case DWC_EVENT_EP_XFER_IN_PROGRESS:
	case DWC_EVENT_EP_XFER_COMPLETE:
	default:
		/* event is not expected in this state */
		ERR("\n Ignore the unexpected EP event: %s\n", event_lookup_ep[event_id]);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
	}
}


/* handle all events occurring in Status-2 state */
static void dwc_event_handler_ep_ctrl_state_status_2(dwc_dev_t *dev,
													 uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t event_status               = DWC_EVENT_EP_EVENT_STATUS(*event);

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
		{
			dwc_dep_cmd_id_t cmd = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);

			if (cmd == DEPCMD_CMD_START_TRANSFER)
			{
				ASSERT(ep->state == EP_STATE_START_TRANSFER);

				ASSERT(event_status == 0);

				/* save the resource id assigned to this ep. */
				ep->state        = EP_STATE_XFER_IN_PROG;
				ep->resource_idx = DWC_EVENT_EP_EVENT_XFER_RES_IDX(*event);
			}
			else
			{
				DBG("\n cmd = %s has no action. ignored.", cmd_lookup[cmd]);
			}
		}
		break;
	case DWC_EVENT_EP_XFER_COMPLETE:
		{
			uint32_t bytes_remaining;
			uint8_t status;

			/* cannot happen on ep 0 */
			ASSERT(ep_phy_num == 1);

			/* should never happen in any other state.
			 * something wrong in fsm implementation.
			 */
			ASSERT(ep->state == EP_STATE_XFER_IN_PROG);

			ep->state = EP_STATE_INACTIVE;

			/* check transfer status */
			status = dwc_event_check_trb_status(dev,
												event,
												DWC_EP_PHY_TO_INDEX(ep_phy_num),
												&bytes_remaining);

			if (status || bytes_remaining)
			{
				DBG("\n\n ******** TRANSFER FAILED ************* status ="
					" %d  bytes_remaining = %d\n\n", status, bytes_remaining);
			}

			dwc_ep_ctrl_state_setup_enter(dev);
		}
		break;
	case DWC_EVENT_EP_XFER_NOT_READY:
	case DWC_EVENT_EP_XFER_IN_PROGRESS:
	default:
		/* event is not expected in this state */
		ERR("\n Ignore the unexpected EP event: %s\n", event_lookup_ep[event_id]);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
	}
}

/* handle all events occurring in Status-3 state */
static void dwc_event_handler_ep_ctrl_state_status_3(dwc_dev_t *dev,
													 uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t event_status               = DWC_EVENT_EP_EVENT_STATUS(*event);

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
	{
		dwc_dep_cmd_id_t cmd = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);

		if (cmd == DEPCMD_CMD_START_TRANSFER)
		{
			/* something wrong with fsm. cannot happen in any other ep state.*/
			ASSERT(ep->state == EP_STATE_START_TRANSFER);

			ASSERT(event_status == 0);

			/* save the resource id assigned to this ep. */
			ep->state        = EP_STATE_XFER_IN_PROG;
			ep->resource_idx = DWC_EVENT_EP_EVENT_XFER_RES_IDX(*event);
		}
		else
		{
			DBG("\n cmd = %s has no action. ignored.", cmd_lookup[cmd]);
		}
	}
	break;
	case DWC_EVENT_EP_XFER_COMPLETE:
		{
			uint32_t bytes_remaining;
			uint8_t status;

			/* should never happen in any other state.
			 * something wrong in fsm implementation.
			 */
			ASSERT(ep->state == EP_STATE_XFER_IN_PROG);

			ep->state = EP_STATE_INACTIVE;

			/* check transfer status */
			status = dwc_event_check_trb_status(dev,
												event,
												DWC_EP_PHY_TO_INDEX(ep_phy_num),
												&bytes_remaining);

			if (status || bytes_remaining)
			{
				DBG("\n\n ******** TRANSFER FAILED ************* status ="
					" %d  bytes_remaining = %d\n\n", status, bytes_remaining);

				/* data stage failed. */
				dwc_ep_cmd_stall(dev, ep_phy_num);

				/* move to stall state. */
				dev->ctrl_state  = EP_FSM_STALL;
			}
			else
			{
				dwc_ep_ctrl_state_setup_enter(dev);
			}
		}
		break;
	case DWC_EVENT_EP_XFER_NOT_READY:
	case DWC_EVENT_EP_XFER_IN_PROGRESS:
	default:
		/* event is not expected in this state */
		ERR("\n Ignore the unexpected EP event: %s\n", event_lookup_ep[event_id]);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
	}
}

/* handle all events occurring in stall state */
static void dwc_event_handler_ep_ctrl_state_stall(dwc_dev_t *dev,
												  uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
		{
			dwc_dep_cmd_id_t cmd = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);

			if (cmd == DEPCMD_CMD_SET_STALL)
			{
				/* stall complete. go back to setup state. */
				dwc_ep_ctrl_state_setup_enter(dev);
			}
			else if (cmd == DEPCMD_CMD_END_TRANSFER)
			{
				/* reset state and resource index */
				ep->state        = EP_STATE_INACTIVE;
				ep->resource_idx = 0;
			}
			else
			{
				DBG("\n cmd = %s has no action. ignored.", cmd_lookup[cmd]);
			}
		}
		break;
	default:
		DBG("\n\n ********No Action defined for this event. ignored. \n\n");
		break;
	}
}

/* event handler for INACTIVE state of bulk endpoint */
static void dwc_event_handler_ep_bulk_state_inactive(dwc_dev_t *dev,
													 uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_dep_cmd_id_t cmd               = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
		{
			DBG("\n cmd = %s has no action. ignored.", cmd_lookup[cmd]);
		}
		break;
	case DWC_EVENT_EP_XFER_NOT_READY:
		{
			/* This is a valid scenario where host is requesting data and
			 * our client has not queued the request yet.
			 */
			DBG("\n Host requested data on ep_phy_num = %d. "
				"No action. ignored.", ep_phy_num);
		}
		break;
	case DWC_EVENT_EP_XFER_IN_PROGRESS:
	case DWC_EVENT_EP_XFER_COMPLETE:
	default:
		/* event is not expected in this state */
		ERR("\n Ignore the unexpected EP event: %s\n", event_lookup_ep[event_id]);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
	}
}

/* event handler for START_TRANSFER state of bulk endpoint */
static void dwc_event_handler_ep_bulk_state_start_transfer(dwc_dev_t *dev,
														   uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t event_status               = DWC_EVENT_EP_EVENT_STATUS(*event);

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
		{
			dwc_dep_cmd_id_t cmd = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);

			if (cmd == DEPCMD_CMD_START_TRANSFER)
			{
				if (event_status == 0)
				{
					/* save the resource id assigned to this ep. */
					ep->state        = EP_STATE_XFER_IN_PROG;
					ep->resource_idx = DWC_EVENT_EP_EVENT_XFER_RES_IDX(*event);
				}
				else
				{
					/* start transfer failed. inform client */
					if (ep->req.callback)
					{
						ep->req.callback(ep->req.context, 0, -1);
					}

					/* back to inactive state */
					dwc_ep_bulk_state_inactive_enter(dev, ep_phy_num);
				}
			}
			else
			{
				DBG("\n cmd = %s has no action. ignored.\n", cmd_lookup[cmd]);
			}
		}
		break;
	default:
		ERR("\n Ignore the unexpected EP event: %s\n", event_lookup_ep[event_id]);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
	}
}

/* event handler for TRANSFER_IN_PROGRESS state of bulk endpoint */
static void dwc_event_handler_ep_bulk_state_xfer_in_prog(dwc_dev_t *dev,
														 uint32_t *event)
{
	uint8_t ep_phy_num                 = DWC_EVENT_EP_EVENT_EP_NUM(*event);
	dwc_event_ep_event_id_t event_id   = DWC_EVENT_EP_EVENT_ID(*event);

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	switch (event_id)
	{
	case DWC_EVENT_EP_CMD_COMPLETE:
		{
			dwc_dep_cmd_id_t cmd = DWC_EVENT_EP_EVENT_CMD_TYPE(*event);

			if (cmd == DEPCMD_CMD_END_TRANSFER)
			{
				/* transfer was cancelled for some reason. */
				DBG("\n transfer was cancelled on ep_phy_num = %d\n", ep_phy_num);

				/* inform client that transfer failed. */
				if (ep->req.callback)
				{
					ep->req.callback(ep->req.context, 0, -1);
				}

				/* back to inactive state */
				dwc_ep_bulk_state_inactive_enter(dev, ep_phy_num);
			}
			else
			{
				DBG("\n cmd = %s has no action. ignored.\n", cmd_lookup[cmd]);
			}
		}
		break;
	case DWC_EVENT_EP_XFER_NOT_READY:
		{
			/* This is a valid scenario where host is requesting data and
			 * we have not yet moved to start transfer state.
			 */
			DBG("\n Host requested data on ep_phy_num = %d."
				"No action. ignored.", ep_phy_num);
		}
		break;
	case DWC_EVENT_EP_XFER_COMPLETE:
		{
			uint32_t bytes_remaining;
			uint8_t  status;

			/* Check how many TRBs were processed and how much data got
			 * transferred. If there are bytes_remaining, it does not
			 * necessarily mean failed xfer. We could have queued a 512 byte
			 * read and receive say 13 bytes of data which is a valid scenario.
			 */
			status = dwc_event_check_trb_status(dev,
												event,
												DWC_EP_PHY_TO_INDEX(ep_phy_num),
												&bytes_remaining);

			DBG("\n\n ******DATA TRANSFER COMPLETED (ep_phy_num = %d) ********"
				"bytes_remaining = %d\n\n", ep_phy_num, bytes_remaining);

			if (ep->req.callback)
			{
				ep->req.callback(ep->req.context,
								 ep->bytes_queued - bytes_remaining,
								 status ? -1 : 0);
			}

			dwc_ep_bulk_state_inactive_enter(dev, ep_phy_num);
		}
		break;
	default:
		ERR("\n Ignore the unexpected EP event: %s\n", event_lookup_ep[event_id]);
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
	}
}

/* bulk endpoint event handler */
void dwc_event_handler_ep_bulk(dwc_dev_t *dev, uint32_t *event)
{
	uint8_t                 ep_phy_num   = DWC_EVENT_EP_EVENT_EP_NUM(*event);
#ifdef DEBUG_USB
	dwc_event_ep_event_id_t event_id     = DWC_EVENT_EP_EVENT_ID(*event);
	uint8_t                 event_status = DWC_EVENT_EP_EVENT_STATUS(*event);
	uint16_t                event_param  = DWC_EVENT_EP_EVENT_PARAM(*event);
#endif

	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

#ifdef DEBUG_USB
	DBG("\n\n\n++EP_EVENT: %s in ctrl_state: %s ep_state[%d]: %s",
		event_lookup_ep[event_id],
		dev_ctrl_state_lookup[dev->ctrl_state],
		ep_phy_num,
		ep_state_lookup[ep->state]);

	DBG("\n ep_phy_num = %d param = 0x%x status = 0x%x",
			ep_phy_num, event_param, event_status);
#endif

	switch (ep->state)
	{
	case EP_STATE_INACTIVE:
		{
			dwc_event_handler_ep_bulk_state_inactive(dev, event);
		}
		break;
	case EP_STATE_START_TRANSFER:
		{
			dwc_event_handler_ep_bulk_state_start_transfer(dev, event);
		}
		break;
	case EP_STATE_XFER_IN_PROG:
		{
			dwc_event_handler_ep_bulk_state_xfer_in_prog(dev, event);
		}
		break;
	default:
		ERR("\n EP state is invalid. Asserting...\n");
		dwc_print_ep_event_details(dev, event);
		dwc_print_current_state(dev);
		ASSERT(0);
	}

#ifdef DEBUG_USB
	DBG("\n--EP_EVENT: %s in ctrl_state: %s ep_state[%d]: %s",
		event_lookup_ep[event_id],
		dev_ctrl_state_lookup[dev->ctrl_state],
		ep_phy_num,
		ep_state_lookup[ep->state]);
#endif
}




/******************** Endpoint related APIs **********************************/

/* Initialize and enable EP:
 * - set the initial configuration for an endpoint
 * - set transfer resources
 * - enable the endpoint
 */
static void dwc_ep_config_init_enable(dwc_dev_t *dev, uint8_t index)
{
	ASSERT(index < DWC_MAX_NUM_OF_EP);
	uint8_t ep_phy_num = dev->ep[index].phy_num;

	dwc_ep_cmd_set_config(dev, index, SET_CONFIG_ACTION_INIT);

	dev->ep[index].state = EP_STATE_INACTIVE;

	/* Set transfer resource configs for the end points */
	dwc_ep_cmd_set_transfer_resource(dev, ep_phy_num);

	/* enable endpoint */
	dwc_ep_enable(dev, ep_phy_num);
}

/* Initialize control EPs:
 * Do the one time initialization of control EPs
 */
static void dwc_ep_ctrl_init(dwc_dev_t *dev)
{
	uint8_t index;

	/* Control OUT */
	index = DWC_EP_INDEX(0, DWC_EP_DIRECTION_OUT);
	ASSERT(index < DWC_MAX_NUM_OF_EP);

	dev->ep[index].number            = 0;
	dev->ep[index].dir               = DWC_EP_DIRECTION_OUT;
	dev->ep[index].phy_num           = 0;
	dev->ep[index].type              = EP_TYPE_CONTROL;
	dev->ep[index].state             = EP_STATE_INIT;
	dev->ep[index].max_pkt_size      = 512;
	dev->ep[index].burst_size        = 0;
	dev->ep[index].tx_fifo_num       = 0;
	dev->ep[index].zlp               = 0;
	dev->ep[index].trb_count         = 1;
	/* TRB must be aligned to 16 */
	dev->ep[index].trb               = memalign(lcm(CACHE_LINE, 16),
												ROUNDUP(dev->ep[index].trb_count*sizeof(dwc_trb_t), CACHE_LINE));
	ASSERT(dev->ep[index].trb);
	dev->ep[index].trb_queued        = 0;
	dev->ep[index].bytes_queued      = 0;

	/* Control IN */
	index = DWC_EP_INDEX(0, DWC_EP_DIRECTION_IN);

	dev->ep[index].number            = 0;
	dev->ep[index].dir               = DWC_EP_DIRECTION_IN;
	dev->ep[index].phy_num           = 1;
	dev->ep[index].type              = EP_TYPE_CONTROL;
	dev->ep[index].state             = EP_STATE_INIT;
	dev->ep[index].max_pkt_size      = 512;
	dev->ep[index].burst_size        = 0;
	dev->ep[index].tx_fifo_num       = 0;
	dev->ep[index].zlp               = 0;
	dev->ep[index].trb_count         = 1;
	/* TRB must be aligned to 16 */
	dev->ep[index].trb               = memalign(lcm(CACHE_LINE, 16),
												ROUNDUP(dev->ep[index].trb_count*sizeof(dwc_trb_t), CACHE_LINE));
	ASSERT(dev->ep[index].trb);
	dev->ep[index].trb_queued        = 0;
	dev->ep[index].bytes_queued      = 0;

	/* configure and enable the endpoints */
	dwc_ep_config_init_enable(dev, 0);
	dwc_ep_config_init_enable(dev, 1);
}


/* entry function into setup state for control fsm */
static void dwc_ep_ctrl_state_setup_enter(dwc_dev_t *dev)
{
	dwc_request_t req;

	/* queue request to receive the first setup pkt from host */
	memset(dev->setup_pkt, 0, DWC_SETUP_PKT_LEN);

	/* flush data */
	arch_clean_invalidate_cache_range((addr_t) dev->setup_pkt, DWC_SETUP_PKT_LEN);

	req.data     = dev->setup_pkt;
	req.len      = DWC_SETUP_PKT_LEN;
	req.trbctl   = TRBCTL_CONTROL_SETUP;
	req.callback = NULL;
	req.context  = NULL;

	dwc_request_queue(dev, 0, &req);

	/* reset control ep state to "setup" state */
	dev->ctrl_state = EP_FSM_SETUP;
}

/* entry function into inactive state for data transfer fsm */
static void dwc_ep_bulk_state_inactive_enter(dwc_dev_t *dev, uint8_t ep_phy_num)
{
	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	/* queue request to receive the first setup pkt from host */
	ep->req.data     = NULL;
	ep->req.len      = 0;
	ep->req.trbctl   = 0;
	ep->req.callback = NULL;
	ep->req.context  = NULL;

	/* inactive state */
	ep->state = EP_STATE_INACTIVE;

	/* reset the resource index, trb and bytes queued */
	ep->resource_idx = 0;
	ep->trb_queued   = 0;
	ep->bytes_queued = 0;
}

/*************************** External APIs ************************************/

/* Initialize controller for device mode operation.
 * Implements sequence as described in HPG.
 * Numbers indicate the step # in HPG.
 */
void dwc_device_init(dwc_dev_t *dev)
{
	/* 15. reset device ctrl. */
	dwc_device_reset(dev);

	/* 16. initialize global control reg for device mode operation */
	dwc_gctl_init(dev);

	/* 17. AXI master config */
	dwc_axi_master_config(dev);

	/* 18. */
	/* a. tx fifo config */
	/* reset value is good. */
	/* b. rx fifo config */
	/* reset value is good. */

	/* 18.c */
	dwc_event_init(dev);

	/* 18.d */
	/* enable device event generation */
	dwc_event_device_enable(dev, BIT(DWC_EVENT_DEVICE_EVENT_ID_DISCONNECT)   |
							     BIT(DWC_EVENT_DEVICE_EVENT_ID_USB_RESET)    |
							     BIT(DWC_EVENT_DEVICE_EVENT_ID_CONNECT_DONE));

	/* 18.e initialize control end point
	 * start new config on end point 0. only needed for ep0.
	 * resource index must be set to 0.
	 */
	dwc_ep_cmd_start_new_config(dev, 0, 0);

	/* steps described in snps 8.1 */
	dwc_ep_ctrl_init(dev);

	/* Unmask interrupts */
	unmask_interrupt(USB30_EE1_IRQ);

	/* start the control ep fsm */
	dwc_ep_ctrl_state_setup_enter(dev);
}

/* add a new non-control endpoint belonging to the selected USB configuration.
 * This is called when "set config" setup is received from host.
 * udc layer first adds the endpoints which are part of the selected
 * configuration by calling this api.
 * Then it calls dwc_device_set_configuration() to enable that configuration.
 * TODO: need better api to manage this. possibly a single api to do both.
 * also, currently this only works as long as same configuration is selected
 * every time. The cleanup during usb reset is not cleanly removing the
 * endpoints added during a set config. This works fine for our usecase.
 */
void dwc_device_add_ep(dwc_dev_t *dev, dwc_ep_t *new_ep)
{
	uint8_t index = DWC_EP_INDEX(new_ep->number, new_ep->dir);

	ASSERT(index < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[index];
	ASSERT(ep != NULL);

	memset(ep, 0, sizeof(ep));

	/* copy client specified params */

	ep->number        = new_ep->number;
	ep->dir           = new_ep->dir;
	ep->type          = new_ep->type;
	ep->max_pkt_size  = new_ep->max_pkt_size;
	ep->burst_size    = new_ep->burst_size;
	ep->zlp           = new_ep->zlp;
	ep->trb_count     = new_ep->trb_count;
	ep->trb           = new_ep->trb;

	ASSERT(ep->trb);

	/* clear out trb memory space. */
	memset(ep->trb, 0, (ep->trb_count)*sizeof(ep->trb));
	arch_clean_invalidate_cache_range((addr_t) ep->trb,
									  (ep->trb_count)*sizeof(ep->trb));

	/* initialize dwc specified params */

	/* map this usb ep to the next available phy ep.
	 * we assume non-flexible endpoint mapping.
	 * so the physical ep number is same as the index into our EP array.
	 */
	ep->phy_num = index;

	if (ep->dir == DWC_EP_DIRECTION_IN)
	{
		/* TODO: this works only as long as we just one IN EP (non-control).
		 * Need to increment this for every new IN ep added.
		 */
		ep->tx_fifo_num = 1;
	}
	else
	{
		ep->tx_fifo_num = 0; /* tx fifo num must be 0 for OUT ep */
	}

	ep->trb_queued    = 0;
	ep->bytes_queued  = 0;
	ep->resource_idx  = 0;
	ep->state         = EP_STATE_INIT;
}

/* Configure and enable non-control endpoints:
 * This is called when "set config" setup is received from host.
 * Implements sequence as described in snps databook 8.1.5.
 */
void dwc_device_set_configuration(dwc_dev_t *dev)
{
	/* disable every ep other than control EPs */
	dwc_ep_disable_non_control(dev);

	/* re-initialize TX FIFO by sending set config cmd to ep-1 */
	dwc_ep_cmd_set_config(dev, 1, SET_CONFIG_ACTION_MODIFY);

	/* re-initialize transfer resource allocation:
	 * only needed for ep0.
	 * resource index must be set to 2 when doing set config
	 */
	dwc_ep_cmd_start_new_config(dev, 0, 2);

	/* Initialize config for each non-control EP in the new configuration */
	for (uint8_t ep_index = 2; ep_index < DWC_MAX_NUM_OF_EP; ep_index++)
	{
		/* non-zero phy ep num indicates that this ep data is initialized
		 * and ready for use.
		 */
		if (dev->ep[ep_index].phy_num)
		{
			dwc_ep_config_init_enable(dev, ep_index);
		}
	}

	/* optional: re-initialize tx FIFO : GTXFIFOSIZn*/
}

/* Enqueue new data transfer request on an endpoint. */
static int dwc_request_queue(dwc_dev_t     *dev,
							 uint8_t        ep_phy_num,
							 dwc_request_t *req)
{
	ASSERT(DWC_EP_PHY_TO_INDEX(ep_phy_num) < DWC_MAX_NUM_OF_EP);
	dwc_ep_t *ep = &dev->ep[DWC_EP_PHY_TO_INDEX(ep_phy_num)];
	ASSERT(ep != NULL);

	dwc_trb_t *trb          = ep->trb;
	uint8_t *data_ptr       = req->data;
	uint32_t transfer_len   = req->len;
	dwc_trb_trbctl_t trbctl = req->trbctl;

	uint32_t pad_len;

	if(ep->state != EP_STATE_INACTIVE)
	{
		DBG("\n EP must be in INACTIVE state to start queue transfer. ep_phy_num = %d current state = %s",
			ep_phy_num, ep_state_lookup[ep->state]);
		return -1;
	}

	/* trb queued must be 0 at this time. */
	ASSERT(ep->trb_queued == 0);

	/* save the original request for this ep */
	ep->req = *req;

	ep->bytes_queued = 0;

	if (ep->type == EP_TYPE_CONTROL)
	{
		memset(trb, 0, sizeof(dwc_trb_t));

		REG_WRITE_FIELD_LOCAL(&trb->f1, TRB_F1, PTR_LOW,  (uint32_t) data_ptr);
		REG_WRITE_FIELD_LOCAL(&trb->f2, TRB_F2, PTR_HIGH, 0x0);
		REG_WRITE_FIELD_LOCAL(&trb->f3, TRB_F3, BUFSIZ,   transfer_len);
		REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, LST,      0x1);
		REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, CHN,      0x0);
		REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, CSP,      0x0);
		REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, TRBCTL,   trbctl);
		REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, IOC,      0x1);
		REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, HWO,      0x1);

		/* increment the queued trb count */
		ep->trb_queued++;
		ep->bytes_queued += transfer_len;
		data_ptr += transfer_len;
	}
	else if (ep->type == EP_TYPE_BULK)
	{
		/* reserve 1 trb for pad/zero-length pkt */
		uint32_t trb_available = ep->trb_count - 1;
		uint32_t max_bytes_per_trb;
		uint32_t offset;
		uint32_t trb_len = 0;

		/* snps 7.2 table 7-1. applies only to older versions of the controller:
		 * - data_ptr in first TRB can be aligned to byte
		 * - but the following TRBs should point to data that is aligned
		 *   to master bus data width.
		 */

		/* align default MAX_BYTES_PER_TRB to DWC_MASTER_BUS_WIDTH */
		max_bytes_per_trb = ROUNDDOWN(DWC_MAX_BYTES_PER_TRB, DWC_MASTER_BUS_WIDTH);

		while (trb_available && transfer_len)
		{
			/* clear out trb fields */
			memset(trb, 0, sizeof(dwc_trb_t));

			if (ep->trb_queued == 0)
			{
				/* first trb: limit the transfer length in this TRB such that
				 * the next trb data_ptr will be aligned to master bus width.
				 */
				offset = ((uint32_t) data_ptr) & (DWC_MASTER_BUS_WIDTH - 1);
				trb_len = (transfer_len <= max_bytes_per_trb) ?
									transfer_len : (max_bytes_per_trb - offset);
			}
			else
			{
				trb_len = (transfer_len <= max_bytes_per_trb) ?
									transfer_len : max_bytes_per_trb;
			}

			REG_WRITE_FIELD_LOCAL(&trb->f1, TRB_F1, PTR_LOW,  (uint32_t) data_ptr);
			REG_WRITE_FIELD_LOCAL(&trb->f2, TRB_F2, PTR_HIGH, 0x0);
			REG_WRITE_FIELD_LOCAL(&trb->f3, TRB_F3, BUFSIZ,   trb_len);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, LST,      0x0);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, CHN,      0x1);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, CSP,      0x0);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, TRBCTL,   trbctl);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, IOC,      0x0);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, HWO,      0x1);

			/* increment the queued trb count */
			ep->trb_queued++;
			ep->bytes_queued += trb_len;
			data_ptr += trb_len;

			/* remaining transfer len */
			transfer_len -= trb_len;

			/* remaining trb */
			trb_available--;

			/* point to the next trb */
			trb++;
		}

		if (transfer_len)
		{
			/* TRBs not available to queue the entire request.
			 * If more data is expected in each request, increase the number
			 * of TRBs allocated for this EP.
			 */
			ERR("\n ERROR: Enough TRBs are not available to setup transfer\n");
			ERR("\n ERROR: Increase TRB chain for the ep.\n");
			ERR("\n ERROR: phy_ep_num = %d xfer len = %d\n", ep_phy_num, req->len);
			ASSERT(0);
		}

		/* snps 8.2.3.3:
		 * For an OUT ep:
		 * (a) The "buffer descriptor" must be exact multiple of max_pkt_size
		 *     "buffer descriptor" consists of one or more TRBs upto the TRB
		 *     with CHN (chain) flag is not set.
		 *     Add a TRB to pad the len if it is not exact multiple.
		 *
		 * (b) If the expected amount of data is exact multiple of max_pkt_size:
		 *     add a max_pkt_size trb to sink in zero-length pkt, only if
		 *     the EP expects it.
		 */
		uint32_t roundup = req->len % ep->max_pkt_size;

		if ( (ep->dir == DWC_EP_DIRECTION_OUT) &&
			 (roundup || ep->zlp))
		{
			if(roundup)
			{
				/* add a TRB to make it exact multiple of max_pkt_size */
				pad_len = ep->max_pkt_size - roundup;
			}
			else
			{
				/* "buffer descriptor" is exact multiple of max_pkt_size and
				 * ep expects a zero-length pkt.
				 * Add a TRB to sink in the zero-length pkt.
				 */
				pad_len = ep->max_pkt_size;
			}

			memset(trb, 0, sizeof(dwc_trb_t));
			memset(ep->zlp_buf, 0, DWC_ZLP_BUF_SIZE);

			REG_WRITE_FIELD_LOCAL(&trb->f1, TRB_F1, PTR_LOW,  (uint32_t) ep->zlp_buf);
			REG_WRITE_FIELD_LOCAL(&trb->f2, TRB_F2, PTR_HIGH, 0x0);
			REG_WRITE_FIELD_LOCAL(&trb->f3, TRB_F3, BUFSIZ,   pad_len);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, LST,      0x1);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, CHN,      0x0);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, CSP,      0x0);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, TRBCTL,   trbctl);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, IOC,      0x1);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, HWO,      0x1);

			/* increment the queued trb count */
			ep->trb_queued++;
			ep->bytes_queued += pad_len;
		}
		else /* pad trb not needed. */
		{
			/* point trb to the last programmed trb */
			trb--;

			/* setup params for last TRB */
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, CHN, 0x0);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, LST, 0x1);
			REG_WRITE_FIELD_LOCAL(&trb->f4, TRB_F4, IOC, 0x1);
		}
	}
	else
	{
		/* invalid EP type */
		ASSERT(0);
	}

	/* flush the trb data to main memory */
	arch_clean_invalidate_cache_range((addr_t) ep->trb,
									  sizeof(dwc_trb_t)*ep->trb_queued);

	DBG("\n Starting new xfer on ep_phy_num = %d TRB_QUEUED = %d \n",
		ep_phy_num, ep->trb_queued);

	/* dwc_request_queue could be triggered from app and thus
	 * outside of interrupt context. Use critical section to make sure all
	 * states are updated properly before we handle other interrupts.
	 */
	enter_critical_section();

	if(ep->state == EP_STATE_INACTIVE)
	{
		dwc_ep_cmd_start_transfer(dev, ep_phy_num);

		if(dwc_device_run_status(dev))
		{
			ep->state = EP_STATE_START_TRANSFER;
		}
		else
		{
			/* no interrupt expected on completion of start transfer.
			 * directly move to xfer in prog state.
			 */
			ep->state = EP_STATE_XFER_IN_PROG;
		}
	}
	else
	{
		ERR("\n Attempting START_TRANSFER in invalid state: %s. .......\n",
			ep_state_lookup[ep->state]);
		dwc_print_current_state(dev);
		ASSERT(0);
	}

	exit_critical_section();

	return 0;
}

/* data transfer request:
 * NOTE: Assumes that the data to be transferred is already in main memory.
 *  	 Any cache management must be done by caller																.
 *  	 For received data, cache mgmt must be done in callback function.
 */
int dwc_transfer_request(dwc_dev_t               *dwc,
						 uint8_t                  usb_ep,
						 dwc_ep_direction_t       dir,
						 void                    *buf,
						 uint32_t                 len,
						 dwc_transfer_callback_t  callback,
						 void                    *callback_context)
{
	uint8_t ep_phy_num;
	dwc_request_t req;

	/* map usb ep to physical ep */
	ep_phy_num = DWC_EP_PHY_NUM(usb_ep, dir);

	req.data     = buf;
	req.len      = len;
	req.callback = callback;
	req.context  = callback_context;

	if (usb_ep == 0)
	{
		/* control EP always has CONTROL_DATA trb */
		req.trbctl = TRBCTL_CONTROL_DATA;
	}
	else
	{
		req.trbctl = TRBCTL_NORMAL;
	}

	return dwc_request_queue(dwc, ep_phy_num, &req);
}
