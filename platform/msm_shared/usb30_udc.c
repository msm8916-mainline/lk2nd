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

/* This file implements the UDC (usb device controller) layer to be used with
 * the new dwc controller.
 * It exposes APIs to initialize UDC (and thus usb) and perform data transfer
 * over usb.
 */

#include <reg.h>
#include <debug.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <arch/defines.h>
#include <dev/udc.h>
#include <platform/iomap.h>
#include <usb30_dwc.h>
#include <usb30_wrapper.h>
#include <usb30_udc.h>
#include <smem.h>
#include <board.h>
#include <platform/timer.h>

//#define DEBUG_USB

#ifdef DEBUG_USB
#define DBG(...) dprintf(ALWAYS, __VA_ARGS__)
#else
#define DBG(...)
#endif

#define ERR(...) dprintf(ALWAYS, __VA_ARGS__)

/* control data transfer is max 512 bytes */
#define UDC_CONTROL_RX_BUF_SIZE     512
#define UDC_CONTROL_TX_BUF_SIZE     512

/* Buffer used by dwc driver to process events.
 * Must be multiple of 4: snps 6.2.7.2.
 */
#define UDC_DWC_EVENT_BUF_SIZE     4096

/* macro to parse setup request */
#define SETUP(type,request) (((type) << 8) | (request))

/* macro to generate bit representation of an EP */
#define EPT_TX(n) (1 << ((n) + 16))
#define EPT_RX(n) (1 << (n))

/* Local functions */
static struct udc_descriptor *udc_descriptor_alloc(uint32_t type,
												   uint32_t num,
												   uint32_t len,
												   udc_desc_spec_t spec);
static uint8_t udc_string_desc_alloc(udc_t *udc, const char *str);

static void udc_descriptor_register(udc_t *udc, struct udc_descriptor *desc);
static void udc_register_language_desc(udc_t *udc);
static void udc_register_bos_desc(udc_t *udc);
static void udc_register_device_desc_usb_20(udc_t *udc, struct udc_device *dev_info);
static void udc_register_device_desc_usb_30(udc_t *udc, struct udc_device *dev_info);
static void udc_register_config_desc_usb20(udc_t *udc, struct udc_gadget *gadget);
static void udc_register_config_desc_usb30(udc_t *udc, struct udc_gadget *gadget);

static void udc_ept_desc_fill(struct udc_endpoint *ept, uint8_t *data);
static void udc_ept_comp_desc_fill(struct udc_endpoint *ept, uint8_t *data);

static void udc_dwc_notify(void *context, dwc_notify_event_t event);
static int udc_handle_setup(void *context, uint8_t *data);

/* TODO: This must be the only global var in this file, for now.
 * Ideally, all APIs should be sending
 * this to us and this ptr should be kept outside of this code.
 * This needs change in the common udc APIs and thus keeping it here until that
 * is done.
 */
static udc_t *udc_dev = NULL;


__WEAK int platform_is_8974()
{
	return 0;
}

__WEAK int platform_is_8974Pro()
{
	return 0;
}

static void phy_reset(usb_wrapper_dev_t *wrapper, struct udc_device *dev_info)
{
	/* phy reset is different for some platforms. */
	if (platform_is_8974() || platform_is_8974Pro())
	{
		/* SS PHY */
		usb_wrapper_ss_phy_reset(wrapper);

		/* For 8974: hs phy is reset as part of soft reset.
		 * No need for explicit reset.
		 */
	}
	else
	{
		if (dev_info->t_usb_if->phy_reset)
			dev_info->t_usb_if->phy_reset();

		/* On some CDPs PHY_COMMON reset does not set
		 * reset values in the phy_ctrl_common register.
		 * Due to this USB does not get enumerated in fastboot
		 * Force write the reset value
		 */
		if (board_platform_id() == APQ8084)
			usb_wrapper_hs_phy_ctrl_force_write(wrapper);
	}
}

/* Initialize HS phy */
void hs_phy_init(udc_t *dev)
{
	/* only for 8974 */
	if (platform_is_8974() || platform_is_8974Pro())
	{
		/* 5.a, 5.b */
		usb_wrapper_hs_phy_init(dev->wrapper_dev);

		/* 5.d */
		dwc_usb2_phy_soft_reset(dev->dwc);
	}
}

/* vbus override */
void vbus_override(udc_t *dev)
{
	/* when vbus signal is not available directly to the controller,
	 * simulate vbus presense.
	 */
	usb_wrapper_vbus_override(dev->wrapper_dev);
}


/* Initialize usb wrapper and dwc h/w blocks. */
static void usb30_init(struct udc_device *dev_info)
{
	usb_wrapper_dev_t* wrapper;
	usb_wrapper_config_t wrapper_config;

	dwc_dev_t *dwc;
	dwc_config_t dwc_config;

	/* initialize usb clocks */
	if (dev_info->t_usb_if->clock_init)
		dev_info->t_usb_if->clock_init();

	/* initialize the usb wrapper h/w block */
	wrapper_config.qscratch_base = (void*) MSM_USB30_QSCRATCH_BASE;

	wrapper = usb_wrapper_init(&wrapper_config);
	ASSERT(wrapper);

	/* save the wrapper ptr */
	udc_dev->wrapper_dev = wrapper;

	/* initialize the dwc device block */
	dwc_config.base           = (void*) MSM_USB30_BASE;

	/* buffer must be aligned to buf size. snps 8.2.2 */
	dwc_config.event_buf      = memalign(lcm(CACHE_LINE, UDC_DWC_EVENT_BUF_SIZE),
										 ROUNDUP(UDC_DWC_EVENT_BUF_SIZE, CACHE_LINE));
	ASSERT(dwc_config.event_buf);

	dwc_config.event_buf_size = UDC_DWC_EVENT_BUF_SIZE;

	/* notify handler */
	dwc_config.notify_context = udc_dev;
	dwc_config.notify         = udc_dwc_notify;

	/* setup handler */
	dwc_config.setup_context = udc_dev;
	dwc_config.setup_handler = udc_handle_setup;

	dwc = dwc_init(&dwc_config);
	ASSERT(dwc);

	/* save the dwc dev ptr */
	udc_dev->dwc = dwc;


	/* USB3.0 core and phy initialization as described in HPG */

	/* section 4.4.1 Control sequence */
	usb_wrapper_dbm_mode(wrapper, DBM_MODE_BYPASS);

	/* section 4.4.1: use config 0 - all of RAM1 */
	usb_wrapper_ram_configure(wrapper);

	/* section 4.4.2: Initialization and configuration sequences */

	/* 1. UTMI Mux configuration */
	if (dev_info->t_usb_if->mux_config)
		dev_info->t_usb_if->mux_config();

	/* 2. Put controller in reset */
	dwc_reset(dwc, 1);

	/* Steps 3 - 7 must be done while dwc is in reset condition */

	/* 3. Reset PHY */
	phy_reset(wrapper, dev_info);

	/* 4. SS phy config */
	usb_wrapper_ss_phy_configure(wrapper);

	/* 5. HS phy init */
	usb_wrapper_hs_phy_init(wrapper);

	/* 6. hs phy config */
	usb_wrapper_hs_phy_configure(wrapper);

	/* 7. Reset PHY digital interface */
	dwc_phy_digital_reset(dwc);

	/* 8. Bring dwc controller out of reset */
	dwc_reset(dwc, 0);

	/* 9. */
	usb_wrapper_ss_phy_electrical_config(wrapper);

	/* Perform phy init */
	if (dev_info->t_usb_if->phy_init)
		dev_info->t_usb_if->phy_init();

	/* 10. */
	usb_wrapper_workaround_10(wrapper);

	/* 11. */
	usb_wrapper_workaround_11(wrapper);

	/* 12. */
	dwc_ss_phy_workaround_12(dwc);

	/* 13. */
	usb_wrapper_workaround_13(wrapper);

	/* 14. needed only for host mode. ignored. */

	/* If the target does not support vbus detection in controller,
	 * simulate vbus presence.
	 */
	if (dev_info->t_usb_if->vbus_override)
		vbus_override(udc_dev);

	/* 15 - 20 */
	dwc_device_init(dwc);
}

/* udc_init: creates and registers various usb descriptor */
int usb30_udc_init(struct udc_device *dev_info)
{
	/* create and initialize udc instance */
	udc_dev = (udc_t*) malloc(sizeof(udc_t));
	ASSERT(udc_dev);

	/* initialize everything to 0 */
	memset(udc_dev, 0 , sizeof(udc_t));

	/* malloc control data buffers */
	udc_dev->ctrl_rx_buf = memalign(CACHE_LINE, ROUNDUP(UDC_CONTROL_RX_BUF_SIZE, CACHE_LINE));
	ASSERT(udc_dev->ctrl_rx_buf);

	udc_dev->ctrl_tx_buf = memalign(CACHE_LINE, ROUNDUP(UDC_CONTROL_TX_BUF_SIZE, CACHE_LINE));
	ASSERT(udc_dev->ctrl_tx_buf);

	/* initialize string id */
	udc_dev->next_string_id  = 1;

	/* Initialize ept data */
	/* alloc table to assume EP0 In/OUT are already allocated.*/
	udc_dev->ept_alloc_table = EPT_TX(0) | EPT_RX(0);
	udc_dev->ept_list        = NULL;

	usb30_init(dev_info);

	/* register descriptors */
	udc_register_language_desc(udc_dev);
	udc_register_device_desc_usb_20(udc_dev, dev_info);
	udc_register_device_desc_usb_30(udc_dev, dev_info);
	udc_register_bos_desc(udc_dev);

	return 0;
}

/* application registers its gadget by calling this func.
 * gadget == interface descriptor
 */
int usb30_udc_register_gadget(struct udc_gadget *gadget)
{
	ASSERT(gadget);

	/* check if already registered */
	if (udc_dev->gadget)
	{
		ERR("\nonly one gadget supported\n");
		return -1;
	}

	/* create our configuration descriptors based on this gadget data */
	udc_register_config_desc_usb20(udc_dev, gadget);
	udc_register_config_desc_usb30(udc_dev, gadget);

	/* save the gadget */
	udc_dev->gadget = gadget;

	return 0;
}

/* udc_start: */
int usb30_udc_start(void)
{
	/* 19. run
	 * enable device to receive SOF packets and
	 * respond to control transfers on EP0 and generate events.
	 */
	dwc_device_run(udc_dev->dwc, 1);

	return 0;
}

/* Control data rx callback. Called by DWC layer when it receives control
 * data from host.
 */
void udc_control_rx_callback(void *context, unsigned actual, int status)
{
	udc_t *udc = (udc_t *) context;

	/* Force reload of buffer update by controller from memory */
	arch_invalidate_cache_range((addr_t) udc->ctrl_rx_buf, actual);

	/* TODO: for now, there is only one 3-stage write during 3.0 enumeration
	 * (SET_SEL), which causes this callback. Ideally, set_periodic() must
	 * be based on which control rx just happened.
	 * Also, the value of 0x65 should depend on the data received for SET_SEL.
	 * For now, this value works just fine.
	 */
	dwc_device_set_periodic_param(udc->dwc, 0x65);
}

/* lookup request name for debug purposes */
static const char *reqname(uint32_t r)
{
	switch (r) {
	case GET_STATUS:
		return "GET_STATUS";
	case CLEAR_FEATURE:
		return "CLEAR_FEATURE";
	case SET_FEATURE:
		return "SET_FEATURE";
	case SET_ADDRESS:
		return "SET_ADDRESS";
	case GET_DESCRIPTOR:
		return "GET_DESCRIPTOR";
	case SET_DESCRIPTOR:
		return "SET_DESCRIPTOR";
	case GET_CONFIGURATION:
		return "GET_CONFIGURATION";
	case SET_CONFIGURATION:
		return "SET_CONFIGURATION";
	case GET_INTERFACE:
		return "GET_INTERFACE";
	case SET_INTERFACE:
		return "SET_INTERFACE";
	case SET_SEL:
		return "SET_SEL";
	default:
		return "*UNKNOWN*";
	}
}

/* callback function called by DWC layer when a setup packed is received.
 * the return value tells dwc layer whether this setup pkt results in
 * a 2-stage or a 3-stage control transfer or stall.
 */
static int udc_handle_setup(void *context, uint8_t *data)
{
	udc_t *udc = (udc_t *) context;
	uint32_t len;

	ASSERT(udc);

	dwc_dev_t *dwc = udc->dwc;
	ASSERT(dwc);

	struct setup_packet s = *((struct setup_packet*) data);

	DBG("\n SETUP request: \n type    = 0x%x \n request = 0x%x \n value  = 0x%x"
		" \n index   = 0x%x \n length  = 0x%x\n",
		s.type, s.request, s.value, s.index, s.length);

	switch (SETUP(s.type, s.request))
	{
	case SETUP(DEVICE_READ, GET_STATUS):
		{
			DBG("\n DEVICE_READ : GET_STATUS: value = %d index = %d"
				" length = %d", s.value, s.index, s.length);

			if (s.length == 2) {

				uint16_t zero = 0;
				len = 2;

				/* copy to tx buffer */
				memcpy(udc->ctrl_tx_buf, &zero, len);

				/* flush buffer to main memory before queueing the request */
				arch_clean_invalidate_cache_range((addr_t) udc->ctrl_tx_buf, len);

				dwc_transfer_request(udc->dwc,
									 0,
									 DWC_EP_DIRECTION_IN,
									 udc->ctrl_tx_buf,
									 len,
									 NULL,
									 NULL);

				return DWC_SETUP_3_STAGE;
			}
		}
		break;
	case SETUP(DEVICE_READ, GET_DESCRIPTOR):
		{
			DBG("\n DEVICE_READ : GET_DESCRIPTOR: value = %d", s.value);

			/* setup usb ep0-IN to send our device descriptor */
			struct udc_descriptor *desc;

			for (desc = udc->desc_list; desc; desc = desc->next)
			{
				/* tag must match the value AND
				 * if speed is SS, desc must comply with 30 spec OR
				 * if speed is not SS, desc must comply with 20 spec.
				 */
				if ((desc->tag == s.value) &&
					(((udc->speed == UDC_SPEED_SS) && (desc->spec & UDC_DESC_SPEC_30)) ||
					  ((udc->speed != UDC_SPEED_SS) && (desc->spec & UDC_DESC_SPEC_20)))
					)
				{
					if (desc->len > s.length)
						len = s.length;
					else
						len = desc->len;

					/* copy to tx buffer */
					memcpy(udc->ctrl_tx_buf, desc->data, len);

					/* flush buffer to main memory before queueing the request */
					arch_clean_invalidate_cache_range((addr_t) udc->ctrl_tx_buf, len);

					dwc_transfer_request(udc->dwc,
										 0,
										 DWC_EP_DIRECTION_IN,
										 udc->ctrl_tx_buf,
										 len,
										 NULL,
										 NULL);

					return DWC_SETUP_3_STAGE;
				}
			}
			DBG("\n Did not find matching descriptor: = 0x%x", s.value);
		}
		break;
	case SETUP(DEVICE_READ, GET_CONFIGURATION):
		{
			DBG("\n DEVICE_READ : GET_CONFIGURATION");

			if ((s.value == 0) && (s.index == 0) && (s.length == 1)) {

				len = 1;

				/* copy to tx buffer */
				memcpy(udc->ctrl_tx_buf, &udc->config_selected, len);

				/* flush buffer to main memory before queueing the request */
				arch_clean_invalidate_cache_range((addr_t) udc->ctrl_tx_buf, len);

				dwc_transfer_request(udc->dwc,
									 0,
									 DWC_EP_DIRECTION_IN,
									 udc->ctrl_tx_buf,
									 len,
									 NULL,
									 NULL);

				return DWC_SETUP_3_STAGE;
			}
			else
			{
				ASSERT(0);
			}
		}
		break;
	case SETUP(DEVICE_WRITE, SET_CONFIGURATION):
		{
			DBG("\n DEVICE_WRITE : SET_CONFIGURATION");

			/* select configuration 1 */
			if (s.value == 1) {
				struct udc_endpoint *ept;
				/* enable endpoints */
				for (ept = udc->ept_list; ept; ept = ept->next) {
					if (ept->num == 0)
						continue;
					else
					{
						/* add this ep to dwc ep list */
						dwc_ep_t *ep = (dwc_ep_t *) malloc(sizeof(dwc_ep_t));

						if(!ep)
						{
							dprintf(CRITICAL, "udc_handle_setup: DEVICE_WRITE : SET_CONFIGURATION malloc failed for ep\n");
							ASSERT(0);
						}

						ep->number        = ept->num;
						ep->dir           = ept->in;
						ep->type          = EP_TYPE_BULK; /* the only one supported */
						ep->max_pkt_size  = ept->maxpkt;
						ep->burst_size    = ept->maxburst;
						ep->zlp           = 0;             /* TODO: zlp could be made part of ept */
						ep->trb_count     = ept->trb_count;
						ep->trb           = ept->trb;

						dwc_device_add_ep(dwc, ep);

						if(ep)
							free(ep);
					}
				}

				/* now that we have saved the non-control EP details, set config */
				dwc_device_set_configuration(dwc);

				/* inform client that we are configured. */
				udc->gadget->notify(udc_dev->gadget, UDC_EVENT_ONLINE);

				udc->config_selected = 1;

				return DWC_SETUP_2_STAGE;
			}
			else if (s.value == 0)
			{
				/* 0 == de-configure. */
				udc->config_selected = 0;
				DBG("\n\n CONFIG = 0 !!!!!!!!!\n\n");
				return DWC_SETUP_2_STAGE;
				/* TODO: do proper handling for de-config */
			}
			else
			{
				ERR("\n CONFIG = %d not supported\n", s.value);
				ASSERT(0);
			}
		}
		break;
	case SETUP(DEVICE_WRITE, SET_ADDRESS):
		{
			DBG("\n DEVICE_WRITE : SET_ADDRESS");

			dwc_device_set_addr(dwc, s.value);
			return DWC_SETUP_2_STAGE;
		}
		break;
	case SETUP(INTERFACE_WRITE, SET_INTERFACE):
		{
			DBG("\n DEVICE_WRITE : SET_INTERFACE");
			/* if we ack this everything hangs */
			/* per spec, STALL is valid if there is not alt func */
			goto stall;
		}
		break;
	case SETUP(DEVICE_WRITE, SET_FEATURE):
		{
			DBG("\n DEVICE_WRITE : SET_FEATURE");
			goto stall;
		}
		break;
	case SETUP(DEVICE_WRITE, CLEAR_FEATURE):
		{
			DBG("\n DEVICE_WRITE : CLEAR_FEATURE");
			goto stall;
		}
		break;
	case SETUP(ENDPOINT_WRITE, CLEAR_FEATURE):
		{
			uint8_t usb_epnum;
			uint8_t dir;

			DBG("\n ENDPOINT_WRITE : CLEAR_FEATURE");

			/*
			 * setup packet received from the host has
			 * index field containing information about the USB
			 * endpoint as below:
			 *  __________________________________
			 * | (7)  |   (6 - 4)   |  (3 - 0)    |
			 * |DIR   |  Reserved   |  EP number  |
			 * |______|_____________|_____________|
			 */
			usb_epnum = (s.index & USB_EP_NUM_MASK);
			dir = (s.index & USB_EP_DIR_MASK == USB_EP_DIR_IN) ? 0x1 : 0x0;

			/*
			 * Convert the logical ep number to physical before
			 * sending the clear stall command.
			 * As per the data book we use fixed mapping as
			 * below:
			 * physical ep 0 --> logical ep0 OUT
			 * physical ep 1 --> logical ep0 IN
			 * physical ep 2 --> logical ep 1 OUT
			 * physical ep 3 --> logical ep 1 IN
			 * :
			 * :
			 * physical ep 30 --> logical ep 15 OUT
			 * physical ep 31 --> logical ep 15 IN
			 */
			dwc_ep_cmd_clear_stall(dwc, DWC_EP_PHY_NUM(usb_epnum, dir));

			return DWC_SETUP_2_STAGE;
		}
		break;
	case SETUP(DEVICE_WRITE, SET_SEL):
		{
			DBG("\n DEVICE_WRITE : SET_SEL");

			/* this is 3-stage write. need to receive data of s.length size. */
			if (s.length > 0) {
				dwc_transfer_request(udc->dwc,
									 0,
									 DWC_EP_DIRECTION_OUT,
									 udc->ctrl_rx_buf,
									 UDC_CONTROL_RX_BUF_SIZE,
									 udc_control_rx_callback,
									 (void *) udc);
				return DWC_SETUP_3_STAGE;
			}
			else
			{
				/* length must be non-zero */
				ASSERT(0);
			}
		}
		break;

	default:
		ERR("\n Unknown setup req.\n type = 0x%x value = %d index = %d"
			" length = %d\n", s.type, s.value, s.index, s.length);
		ASSERT(0);
	}

stall:
	ERR("\nSTALL. Unsupported setup req: %s %d %d %d %d %d\n",
		reqname(s.request), s.type, s.request, s.value, s.index, s.length);

	return DWC_SETUP_ERROR;
}

/* Callback function called by DWC layer when a request to transfer data
 * on non-control EP is completed.
 */
void udc_request_complete(void *context, uint32_t actual, int status)
{
	struct udc_request *req = ((udc_t *) context)->queued_req;

	DBG("\n UDC: udc_request_callback: xferred %d bytes status = %d\n",
		actual, status);

	/* clear the queued request. */
	((udc_t *) context)->queued_req = NULL;

	if (req->complete)
	{
		req->complete(req, actual, status);
	}

	DBG("\n UDC: udc_request_callback: done fastboot callback\n");
}

/* App interface to queue in data transfer requests for control and data ep */
int usb30_udc_request_queue(struct udc_endpoint *ept, struct udc_request *req)
{
	int ret;
    dwc_dev_t *dwc_dev = udc_dev->dwc;

	/* ensure device is initialized before queuing request */
	ASSERT(dwc_dev);

	/* if device is not configured, return error */
	if(udc_dev->config_selected == 0)
	{
		return -1;
	}

	/* only one request at a time is supported.
	 * check if a request is already queued.
	 */
	if(udc_dev->queued_req)
	{
		return -1;
	}

	DBG("\n udc_request_queue: entry: ep_usb_num = %d", ept->num);

	/* save the queued request. */
	udc_dev->queued_req = req;

	ret = dwc_transfer_request(dwc_dev,
							   ept->num,
							   ept->in ? DWC_EP_DIRECTION_IN : DWC_EP_DIRECTION_OUT,
							   req->buf,
							   req->length,
							   udc_request_complete,
							   (void *) udc_dev);

	DBG("\n udc_request_queue: exit: ep_usb_num = %d", ept->num);

	return ret;
}

/* callback function called by dwc layer if any dwc event occurs */
void udc_dwc_notify(void *context, dwc_notify_event_t event)
{
	udc_t *udc = (udc_t *) context;

	switch (event)
	{
	case DWC_NOTIFY_EVENT_CONNECTED_LS:
		udc->speed = UDC_SPEED_LS;
		break;
	case DWC_NOTIFY_EVENT_CONNECTED_FS:
		udc->speed = UDC_SPEED_FS;
		break;
	case DWC_NOTIFY_EVENT_CONNECTED_HS:
		udc->speed = UDC_SPEED_HS;
		break;
	case DWC_NOTIFY_EVENT_CONNECTED_SS:
		udc->speed = UDC_SPEED_SS;
		break;
	case DWC_NOTIFY_EVENT_DISCONNECTED:
	case DWC_NOTIFY_EVENT_OFFLINE:
		udc->config_selected = 0;
		if (udc->gadget && udc->gadget->notify)
			udc->gadget->notify(udc->gadget, UDC_EVENT_OFFLINE);
		break;
	default:
		ASSERT(0);
	}
}


/******************* Function related to descriptor allocation etc.************/

static struct udc_endpoint *_udc_endpoint_alloc(uint8_t num,
												uint8_t in,
												uint16_t max_pkt)
{
	struct udc_endpoint *ept;
	udc_t *udc = udc_dev;

	ept = malloc(sizeof(*ept));
	ASSERT(ept);

	ept->maxpkt     = max_pkt;
	ept->num        = num;
	ept->in         = !!in;
	ept->maxburst   = 4;      /* no performance improvement is seen beyond burst size of 4 */
	ept->trb_count  = 66;     /* each trb can transfer (16MB - 1). 65 for 1GB transfer + 1 for roundup/zero length pkt. */
	ept->trb        = memalign(lcm(CACHE_LINE, 16), ROUNDUP(ept->trb_count*sizeof(dwc_trb_t), CACHE_LINE)); /* TRB must be aligned to 16 */
	ASSERT(ept->trb);

	/* push it on top of ept_list */
	ept->next      = udc->ept_list;
	udc->ept_list  = ept;

	return ept;
}

/* Called to create non-control in/out End Point structures by the APP */
struct udc_endpoint *usb30_udc_endpoint_alloc(unsigned type, unsigned maxpkt)
{
	struct udc_endpoint *ept;
	uint8_t in;
	uint8_t n;
	udc_t *udc = udc_dev;

	if (type == UDC_TYPE_BULK_IN) {
		in = 1;
	} else if (type == UDC_TYPE_BULK_OUT) {
		in = 0;
	} else {
		return 0;
	}

	for (n = 1; n < 16; n++) {
		uint32_t bit = in ? EPT_TX(n) : EPT_RX(n);
		if (udc->ept_alloc_table & bit)
			continue;
		ept = _udc_endpoint_alloc(n, in, maxpkt);
		if (ept)
			udc->ept_alloc_table |= bit;
		return ept;
	}
	return 0;
}


/* create config + interface + ep desc for 2.0 */
static void udc_register_config_desc_usb20(udc_t *udc,
										   struct udc_gadget *gadget)
{
	uint8_t  *data;
	uint16_t  size;
	struct udc_descriptor *desc;

	ASSERT(udc);
	ASSERT(gadget);

	/* create our configuration descriptor */

	/* size is the total size of (config + interface + all EPs) descriptor */
	size = UDC_DESC_SIZE_CONFIGURATION +
		   UDC_DESC_SIZE_INTERFACE +
		   (gadget->ifc_endpoints*UDC_DESC_SIZE_ENDPOINT);

	desc = udc_descriptor_alloc(TYPE_CONFIGURATION, 0, size, UDC_DESC_SPEC_20);

	data = desc->data;

	/* Config desc */
	data[0] = 0x09;
	data[1] = TYPE_CONFIGURATION;
	data[2] = size;
	data[3] = size >> 8;
	data[4] = 0x01;     /* number of interfaces */
	data[5] = 0x01;     /* configuration value */
	data[6] = 0x00;     /* configuration string */
	data[7] = 0xC0;     /* attributes: reserved and self-powered set */
	data[8] = 0x00;     /* max power: 0ma since we are self powered */
	data += 9;

	/* Interface desc */
	data[0] = 0x09;
	data[1] = TYPE_INTERFACE;
	data[2] = 0x00;     /* ifc number */
	data[3] = 0x00;     /* alt number */
	data[4] = gadget->ifc_endpoints;
	data[5] = gadget->ifc_class;
	data[6] = gadget->ifc_subclass;
	data[7] = gadget->ifc_protocol;
	data[8] = udc_string_desc_alloc(udc, gadget->ifc_string);
	data += 9;

	for (uint8_t n = 0; n < gadget->ifc_endpoints; n++) {
		udc_ept_desc_fill(gadget->ept[n], data);
		data += UDC_DESC_SIZE_ENDPOINT;
	}

	udc_descriptor_register(udc, desc);
}

/* create config + interface + ep desc for 3.0 */
static void udc_register_config_desc_usb30(udc_t *udc,
										   struct udc_gadget *gadget)
{
	uint8_t *data;
	uint16_t size;
	struct udc_descriptor *desc;

	ASSERT(udc);
	ASSERT(gadget);

	/* create our configuration descriptor */

	/* size is the total size of (config + interface + all EPs) descriptor */
	size = UDC_DESC_SIZE_CONFIGURATION +
		   UDC_DESC_SIZE_INTERFACE +
		   (gadget->ifc_endpoints*(UDC_DESC_SIZE_ENDPOINT + UDC_DESC_SIZE_ENDPOINT_COMP));

	desc = udc_descriptor_alloc(TYPE_CONFIGURATION, 0, size, UDC_DESC_SPEC_30);

	data = desc->data;

	/* Config desc */
	data[0] = 0x09;
	data[1] = TYPE_CONFIGURATION;
	data[2] = size;
	data[3] = size >> 8;
	data[4] = 0x01;     /* number of interfaces */
	data[5] = 0x01;     /* configuration value */
	data[6] = 0x00;     /* configuration string */
	data[7] = 0xC0;     /* attributes: reserved and self-powered set */
	data[8] = 0x00;     /* max power: 0ma since we are self powered */
	data += 9;

	/* Interface desc */
	data[0] = 0x09;
	data[1] = TYPE_INTERFACE;
	data[2] = 0x00;     /* ifc number */
	data[3] = 0x00;     /* alt number */
	data[4] = gadget->ifc_endpoints;
	data[5] = gadget->ifc_class;
	data[6] = gadget->ifc_subclass;
	data[7] = gadget->ifc_protocol;
	data[8] = udc_string_desc_alloc(udc, gadget->ifc_string);
	data += 9;

	for (uint8_t n = 0; n < gadget->ifc_endpoints; n++)
	{
		/* fill EP desc */
		udc_ept_desc_fill(gadget->ept[n], data);
		data += UDC_DESC_SIZE_ENDPOINT;

		/* fill EP companion desc */
		udc_ept_comp_desc_fill(gadget->ept[n], data);
		data += UDC_DESC_SIZE_ENDPOINT_COMP;
	}

	udc_descriptor_register(udc, desc);
}


static void udc_register_device_desc_usb_20(udc_t *udc,
											struct udc_device *dev_info)
{
	uint8_t *data;
	struct udc_descriptor *desc;

	/* create our device descriptor */
	desc = udc_descriptor_alloc(TYPE_DEVICE, 0, 18, UDC_DESC_SPEC_20);
	data = desc->data;

	/* data 0 and 1 is filled by descriptor alloc routine.
	 * fill in the remaining entries.
	 */
	data[2] = 0x00; /* usb spec minor rev */
	data[3] = 0x02; /* usb spec major rev */
	data[4] = 0x00; /* class */
	data[5] = 0x00; /* subclass */
	data[6] = 0x00; /* protocol */
	data[7] = 0x40; /* max packet size on ept 0 */

	memcpy(data + 8,  &dev_info->vendor_id,  sizeof(short));
	memcpy(data + 10, &dev_info->product_id, sizeof(short));
	memcpy(data + 12, &dev_info->version_id, sizeof(short));

	data[14] = udc_string_desc_alloc(udc, dev_info->manufacturer);
	data[15] = udc_string_desc_alloc(udc, dev_info->product);
	data[16] = udc_string_desc_alloc(udc, dev_info->serialno);
	data[17] = 1; /* number of configurations */

	udc_descriptor_register(udc, desc);
}

static void udc_register_device_desc_usb_30(udc_t *udc, struct udc_device *dev_info)
{
	uint8_t *data;
	struct udc_descriptor *desc;

	/* create our device descriptor */
	desc = udc_descriptor_alloc(TYPE_DEVICE, 0, 18, UDC_DESC_SPEC_30);
	data = desc->data;

	/* data 0 and 1 is filled by descriptor alloc routine.
	 * fill in the remaining entries.
	 */
	data[2] = 0x00; /* usb spec minor rev */
	data[3] = 0x03; /* usb spec major rev */
	data[4] = 0x00; /* class */
	data[5] = 0x00; /* subclass */
	data[6] = 0x00; /* protocol */
	data[7] = 0x09; /* max packet size on ept 0 */
	memcpy(data +  8, &dev_info->vendor_id,  sizeof(short));
	memcpy(data + 10, &dev_info->product_id, sizeof(short));
	memcpy(data + 12, &dev_info->version_id, sizeof(short));
	data[14] = udc_string_desc_alloc(udc, dev_info->manufacturer);
	data[15] = udc_string_desc_alloc(udc, dev_info->product);
	data[16] = udc_string_desc_alloc(udc, dev_info->serialno);
	data[17] = 1; /* number of configurations */

	udc_descriptor_register(udc, desc);
}

static void udc_register_bos_desc(udc_t *udc)
{
	uint8_t *data;
	struct udc_descriptor *desc;

	/* create our device descriptor */
	desc = udc_descriptor_alloc(TYPE_BOS, 0, 15, UDC_DESC_SPEC_30); /* 15 is total length of bos + other descriptors inside it */
	data = desc->data;

	/* data 0 and 1 is filled by descriptor alloc routine.
	 * fill in the remaining entries.
	 */
	data[0] = 0x05;     /* BOS desc len */
	data[1] = TYPE_BOS; /* BOS desc type */
	data[2] = 0x0F;     /* total len of bos desc and its sub desc */
	data[3] = 0x00;     /* total len of bos desc and its sub desc */
	data[4] = 0x01;     /* num of sub desc inside bos */

	data[5]  = 0x0A;    /* desc len */
	data[6]  = 0x10;    /* Device Capability desc */
	data[7]  = 0x03;    /* 3 == SuperSpeed capable */
	data[8]  = 0x00;    /* Attribute: latency tolerance msg: No */
	data[9]  = 0x0F;    /* Supported Speeds (bit mask): LS, FS, HS, SS */
	data[10] = 0x00;    /* Reserved part of supported wSupportedSpeeds */
	data[11] = 0x01;    /* lowest supported speed with full functionality: FS */
	data[12] = 0x00;    /* U1 device exit latency */
	data[13] = 0x00;    /* U2 device exit latency (lsb) */
	data[14] = 0x00;    /* U2 device exit latency (msb) */

	udc_descriptor_register(udc, desc);
}

static void udc_register_language_desc(udc_t *udc)
{
	/* create and register a language table descriptor */
	/* language 0x0409 is US English */
	struct udc_descriptor *desc = udc_descriptor_alloc(TYPE_STRING,
													   0,
													   4,
													   UDC_DESC_SPEC_20 | UDC_DESC_SPEC_30);
	desc->data[2] = 0x09;
	desc->data[3] = 0x04;
	udc_descriptor_register(udc, desc);
}

static void udc_ept_desc_fill(struct udc_endpoint *ept, uint8_t *data)
{
	data[0] = 7;
	data[1] = TYPE_ENDPOINT;
	data[2] = ept->num | (ept->in ? 0x80 : 0x00);
	data[3] = 0x02; /* bulk -- the only kind we support */
	data[4] = ept->maxpkt;
	data[5] = ept->maxpkt >> 8;
	data[6] = 0; /* bInterval: must be 0 for bulk. */
}

static void udc_ept_comp_desc_fill(struct udc_endpoint *ept, uint8_t *data)
{
	data[0] = 6;               /* bLength */
	data[1] = TYPE_SS_EP_COMP; /* ep type */
	data[2] = ept->maxburst;   /* maxBurst */
	data[3] = 0x0;             /* maxStreams */
	data[4] = 0x0;             /* wBytesPerInterval */
	data[5] = 0x0;             /* wBytesPerInterval */
}

static uint8_t udc_string_desc_alloc(udc_t *udc, const char *str)
{
	uint32_t len;
	struct udc_descriptor *desc;
	uint8_t *data;

	if (udc->next_string_id > 255)
		return 0;

	if (!str)
		return 0;

	len = strlen(str);
	desc = udc_descriptor_alloc(TYPE_STRING,
								udc->next_string_id,
								len * 2 + 2,
								UDC_DESC_SPEC_20 | UDC_DESC_SPEC_30);
	if (!desc)
		return 0;
	udc->next_string_id++;

	/* expand ascii string to utf16 */
	data = desc->data + 2;
	while (len-- > 0) {
		*data++ = *str++;
		*data++ = 0;
	}

	udc_descriptor_register(udc, desc);
	return desc->tag & 0xff;
}


static struct udc_descriptor *udc_descriptor_alloc(uint32_t type,
												   uint32_t num,
												   uint32_t len,
												   udc_desc_spec_t spec)
{
	struct udc_descriptor *desc;
	if ((len > 255) || (len < 2) || (num > 255) || (type > 255))
		return 0;

	if (!(desc = malloc(sizeof(struct udc_descriptor) + len)))
		return 0;

	desc->next    = 0;
	desc->tag     = (type << 8) | num;
	desc->len     = len;
	desc->spec    = spec;

	/* descriptor data */
	desc->data[0] = len;
	desc->data[1] = type;

	return desc;
}

static void udc_descriptor_register(udc_t *udc, struct udc_descriptor *desc)
{
	desc->next     = udc->desc_list;
	udc->desc_list = desc;
}


struct udc_request *usb30_udc_request_alloc(void)
{
	struct udc_request *req;

	req = malloc(sizeof(*req));
	ASSERT(req);

	req->buf      = 0;
	req->length   = 0;
	req->complete = NULL;
	req->context  = 0;

	return req;
}

void usb30_udc_request_free(struct udc_request *req)
{
	free(req);
}

void usb30_udc_endpoint_free(struct udc_endpoint *ept)
{
	/* TODO */
}

int usb30_udc_stop(void)
{
	dwc_device_run(udc_dev->dwc, 0);

	return 0;
}
