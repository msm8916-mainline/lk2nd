/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2015, The Linux Foundation. All rights reserved.
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

#include <string.h>
#include <stdlib.h>
#include <debug.h>
#include <platform.h>
#include <platform/iomap.h>
#include <platform/irqs.h>
#include <platform/interrupts.h>
#include <platform/timer.h>
#include <kernel/thread.h>
#include <reg.h>
#include <dev/udc.h>
#include "hsusb.h"

#define MAX_TD_XFER_SIZE  (16 * 1024)


/* common code - factor out into a shared file */

struct udc_descriptor {
	struct udc_descriptor *next;
	unsigned short tag;	/* ((TYPE << 8) | NUM) */
	unsigned short len;	/* total length */
	unsigned char data[0];
};

struct udc_descriptor *udc_descriptor_alloc(unsigned type, unsigned num,
					    unsigned len)
{
	struct udc_descriptor *desc;
	if ((len > 255) || (len < 2) || (num > 255) || (type > 255))
		return 0;

	if (!(desc = malloc(sizeof(struct udc_descriptor) + len)))
		return 0;

	desc->next = 0;
	desc->tag = (type << 8) | num;
	desc->len = len;
	desc->data[0] = len;
	desc->data[1] = type;

	return desc;
}

static struct udc_descriptor *desc_list = 0;
static unsigned next_string_id = 1;

void udc_descriptor_register(struct udc_descriptor *desc)
{
	desc->next = desc_list;
	desc_list = desc;
}

unsigned udc_string_desc_alloc(const char *str)
{
	unsigned len;
	struct udc_descriptor *desc;
	unsigned char *data;

	if (next_string_id > 255)
		return 0;

	if (!str)
		return 0;

	len = strlen(str);
	desc = udc_descriptor_alloc(TYPE_STRING, next_string_id, len * 2 + 2);
	if (!desc)
		return 0;
	next_string_id++;

	/* expand ascii string to utf16 */
	data = desc->data + 2;
	while (len-- > 0) {
		*data++ = *str++;
		*data++ = 0;
	}

	udc_descriptor_register(desc);
	return desc->tag & 0xff;
}

/* end of common code */

__WEAK void hsusb_clock_init(void)
{
	return;
}

#if 1
#define DBG(x...) do {} while(0)
#else
#define DBG(x...) dprintf(ALWAYS, x)
#endif

#define usb_status(a,b)

struct usb_request {
	struct udc_request req;
	struct ept_queue_item *item;
};

struct udc_endpoint {
	struct udc_endpoint *next;
	unsigned bit;
	struct ept_queue_head *head;
	struct usb_request *req;
	unsigned char num;
	unsigned char in;
	unsigned short maxpkt;
};

struct udc_endpoint *ept_list = 0;
struct ept_queue_head *epts = 0;

static int usb_online = 0;
static int usb_highspeed = 0;

static struct udc_device *the_device;
static struct udc_gadget *the_gadget;
static unsigned test_mode = 0;

struct udc_endpoint *_udc_endpoint_alloc(unsigned num, unsigned in,
					 unsigned max_pkt)
{
	struct udc_endpoint *ept;
	unsigned cfg;

	ept = memalign(CACHE_LINE, ROUNDUP(sizeof(*ept), CACHE_LINE));
	ASSERT(ept);

	ept->maxpkt = max_pkt;
	ept->num = num;
	ept->in = !!in;
	ept->req = 0;

	cfg = CONFIG_MAX_PKT(max_pkt) | CONFIG_ZLT;

	if (ept->in) {
		ept->bit = EPT_TX(ept->num);
	} else {
		ept->bit = EPT_RX(ept->num);
		if (num == 0)
			cfg |= CONFIG_IOS;
	}

	ept->head = epts + (num * 2) + (ept->in);
	ept->head->config = cfg;

	ept->next = ept_list;
	ept_list = ept;

	arch_clean_invalidate_cache_range((addr_t) ept,
					  sizeof(struct udc_endpoint));
	arch_clean_invalidate_cache_range((addr_t) ept->head,
					  sizeof(struct ept_queue_head));

	DBG("ept%d %s @%p/%p max=%d bit=%x\n",
	    num, in ? "in" : "out", ept, ept->head, max_pkt, ept->bit);

	return ept;
}

static unsigned ept_alloc_table = EPT_TX(0) | EPT_RX(0);

struct udc_endpoint *udc_endpoint_alloc(unsigned type, unsigned maxpkt)
{
	struct udc_endpoint *ept;
	unsigned n;
	unsigned in;

	if (type == UDC_TYPE_BULK_IN) {
		in = 1;
	} else if (type == UDC_TYPE_BULK_OUT) {
		in = 0;
	} else {
		return 0;
	}

	for (n = 1; n < 16; n++) {
		unsigned bit = in ? EPT_TX(n) : EPT_RX(n);
		if (ept_alloc_table & bit)
			continue;
		ept = _udc_endpoint_alloc(n, in, maxpkt);
		if (ept)
			ept_alloc_table |= bit;
		return ept;
	}
	return 0;
}

void udc_endpoint_free(struct udc_endpoint *ept)
{
	/* todo */
}

static void endpoint_enable(struct udc_endpoint *ept, unsigned yes)
{
	unsigned n = readl(USB_ENDPTCTRL(ept->num));

	if (yes) {
		if (ept->in) {
			n |= (CTRL_TXE | CTRL_TXR | CTRL_TXT_BULK);
		} else {
			n |= (CTRL_RXE | CTRL_RXR | CTRL_RXT_BULK);
		}

		if (ept->num != 0) {
			/* XXX should be more dynamic... */
			if (usb_highspeed) {
				ept->head->config =
				    CONFIG_MAX_PKT(512) | CONFIG_ZLT;
			} else {
				ept->head->config =
				    CONFIG_MAX_PKT(64) | CONFIG_ZLT;
			}
		}
	}
	writel(n, USB_ENDPTCTRL(ept->num));
}

struct udc_request *udc_request_alloc(void)
{
	struct usb_request *req;
	req = memalign(CACHE_LINE, ROUNDUP(sizeof(*req), CACHE_LINE));
	ASSERT(req);
	req->req.buf = 0;
	req->req.length = 0;
	req->item = memalign(CACHE_LINE, ROUNDUP(sizeof(struct ept_queue_item),
								CACHE_LINE));
	return &req->req;
}

void udc_request_free(struct udc_request *req)
{
	free(req);
}

/*
 * Assumes that TDs allocated already are not freed.
 * But it can handle case where TDs are freed as well.
 */
int udc_request_queue(struct udc_endpoint *ept, struct udc_request *_req)
{
	unsigned xfer = 0;
	struct ept_queue_item *item, *curr_item;
	struct usb_request *req = (struct usb_request *)_req;
	unsigned phys = (unsigned)req->req.buf;
	unsigned len = req->req.length;
	unsigned int count = 0;

	curr_item = NULL;
	xfer = (len > MAX_TD_XFER_SIZE) ? MAX_TD_XFER_SIZE : len;
	/*
	 * First TD allocated during request allocation
	 */
	item = req->item;
	item->info = INFO_BYTES(xfer) | INFO_ACTIVE;
	item->page0 = phys;
	item->page1 = (phys & 0xfffff000) + 0x1000;
	item->page2 = (phys & 0xfffff000) + 0x2000;
	item->page3 = (phys & 0xfffff000) + 0x3000;
	item->page4 = (phys & 0xfffff000) + 0x4000;
	phys += xfer;
	curr_item = item;
	len -= xfer;

	/*
	 * If transfer length is more then
	 * accomodate by 1 TD
	 * we add more transfer descriptors
	 */
	while (len > 0) {
		xfer = (len > MAX_TD_XFER_SIZE) ? MAX_TD_XFER_SIZE : len;
		if (curr_item->next == TERMINATE) {
			/*
			 * Allocate new TD only if chain doesnot
			 * exist already
			 */
			item = memalign(CACHE_LINE,
					ROUNDUP(sizeof(struct ept_queue_item), CACHE_LINE));
			if (!item) {
				dprintf(ALWAYS, "allocate USB item fail ept%d"
							"%s queue\n",
							"td count = %d\n",
							ept->num,
							ept->in ? "in" : "out",
							count);
				return -1;
			} else {
				count ++;
				curr_item->next = PA(item);
				item->next = TERMINATE;
			}
		} else
			/* Since next TD in chain already exists */
			item = VA(curr_item->next);

		/* Update TD with transfer information */
		item->info = INFO_BYTES(xfer) | INFO_ACTIVE;
		item->page0 = phys;
		item->page1 = (phys & 0xfffff000) + 0x1000;
		item->page2 = (phys & 0xfffff000) + 0x2000;
		item->page3 = (phys & 0xfffff000) + 0x3000;
		item->page4 = (phys & 0xfffff000) + 0x4000;

		curr_item = item;
		len -= xfer;
		phys += xfer;
	}

	/* Terminate and set interrupt for last TD */
	curr_item->next = TERMINATE;
	curr_item->info |= INFO_IOC;
	enter_critical_section();
	ept->head->next = PA(req->item);
	ept->head->info = 0;
	ept->req = req;
	arch_clean_invalidate_cache_range((addr_t) ept,
					  sizeof(struct udc_endpoint));
	arch_clean_invalidate_cache_range((addr_t) ept->head,
					  sizeof(struct ept_queue_head));
	arch_clean_invalidate_cache_range((addr_t) ept->req,
					  sizeof(struct usb_request));
	arch_clean_invalidate_cache_range((addr_t) VA(req->req.buf),
					  req->req.length);

	item = req->item;
	/* Write all TD's to memory from cache */
	while (item != NULL) {
		curr_item = item;
		if (curr_item->next == TERMINATE)
			item = NULL;
		else
			item = curr_item->next;
		arch_clean_invalidate_cache_range((addr_t) curr_item,
					  sizeof(struct ept_queue_item));
	}

	DBG("ept%d %s queue req=%p\n", ept->num, ept->in ? "in" : "out", req);
	writel(ept->bit, USB_ENDPTPRIME);
	exit_critical_section();
	return 0;
}

static void handle_ept_complete(struct udc_endpoint *ept)
{
	struct ept_queue_item *item;
	unsigned actual, total_len;
	int status, len;
	struct usb_request *req=NULL;
	void *buf;

	DBG("ept%d %s complete req=%p\n",
	    ept->num, ept->in ? "in" : "out", ept->req);

	arch_invalidate_cache_range((addr_t) ept,
					  sizeof(struct udc_endpoint));

	if(ept->req)
	{
		req = VA(ept->req);
		arch_invalidate_cache_range((addr_t) ept->req,
						sizeof(struct usb_request));
	}

	if (req) {
		item = VA(req->item);
		/* total transfer length for transacation */
		total_len = req->req.length;
		ept->req = 0;
		actual = 0;
		while(1) {

			do {
				/*
				 * Must clean/invalidate cached item
				 * data before checking the status
				 * every time.
				 */
				arch_invalidate_cache_range((addr_t)(item),
							sizeof(
							struct ept_queue_item));

			} while(readl(&item->info) & INFO_ACTIVE);

			if ((item->info) & 0xff) {
				/* error */
				status = -1;
				dprintf(INFO, "EP%d/%s FAIL nfo=%x pg0=%x\n",
					ept->num, ept->in ? "in" : "out",
					item->info,
					item->page0);
				goto out;
			}

			/* Check if we are processing last TD */
			if (item->next == TERMINATE) {
				/*
				 * Record the data transferred for the last TD
				 */
				actual += total_len - (item->info >> 16)
								& 0x7FFF;
				total_len = 0;
				break;
			} else {
				/*
				 * Since we are not in last TD
				 * the total assumed transfer ascribed to this
				 * TD woulb the max possible TD transfer size
				 * (16K)
				 */
				actual += MAX_TD_XFER_SIZE - (item->info >> 16) & 0x7FFF;
				total_len -= MAX_TD_XFER_SIZE - (item->info >> 16) & 0x7FFF;
				/*Move to next item in chain*/
				item = VA(item->next);
			}
		}
		status = 0;
out:
		if (req->req.complete)
			req->req.complete(&req->req, actual, status);
	}
}

static const char *reqname(unsigned r)
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
	default:
		return "*UNKNOWN*";
	}
}

static struct udc_endpoint *ep0in, *ep0out;
static struct udc_request *ep0req;

static void
ep0_setup_ack_complete()
{
	uint32_t mode;

	if (!test_mode)
		return;

	switch (test_mode) {
	case TEST_PACKET:
		dprintf(INFO, "Entering test mode for TST_PKT\n");
		mode = readl(USB_PORTSC) & (~PORTSC_PTC);
		writel(mode | PORTSC_PTC_TST_PKT, USB_PORTSC);
		break;

	case TEST_SE0_NAK:
		dprintf(INFO, "Entering test mode for SE0-NAK\n");
		mode = readl(USB_PORTSC) & (~PORTSC_PTC);
		writel(mode | PORTSC_PTC_SE0_NAK, USB_PORTSC);
		break;
	}

}

static void setup_ack(void)
{
	ep0req->complete = ep0_setup_ack_complete;
	ep0req->length = 0;
	udc_request_queue(ep0in, ep0req);
}

static void ep0in_complete(struct udc_request *req, unsigned actual, int status)
{
	DBG("ep0in_complete %p %d %d\n", req, actual, status);
	if (status == 0) {
		req->length = 0;
		req->complete = 0;
		udc_request_queue(ep0out, req);
	}
}

static void setup_tx(void *buf, unsigned len)
{
	DBG("setup_tx %p %d\n", buf, len);
	memcpy(ep0req->buf, buf, len);
	ep0req->buf = (void *)PA((addr_t)ep0req->buf);
	arch_clean_invalidate_cache_range((addr_t)ep0req->buf, len);
	ep0req->complete = ep0in_complete;
	ep0req->length = len;
	udc_request_queue(ep0in, ep0req);
}

static unsigned char usb_config_value = 0;

#define SETUP(type,request) (((type) << 8) | (request))

static void handle_setup(struct udc_endpoint *ept)
{
	struct setup_packet s;

	arch_clean_invalidate_cache_range((addr_t) ept->head->setup_data,
					  sizeof(struct ept_queue_head));
	memcpy(&s, ept->head->setup_data, sizeof(s));
	arch_clean_invalidate_cache_range((addr_t)&s, sizeof(s));
	writel(ept->bit, USB_ENDPTSETUPSTAT);

	DBG("handle_setup type=0x%02x req=0x%02x val=%d idx=%d len=%d (%s)\n",
	    s.type, s.request, s.value, s.index, s.length, reqname(s.request));

	switch (SETUP(s.type, s.request)) {
	case SETUP(DEVICE_READ, GET_STATUS):
		{
			unsigned zero = 0;
			if (s.length == 2) {
				setup_tx(&zero, 2);
				return;
			}
			break;
		}
	case SETUP(DEVICE_READ, GET_DESCRIPTOR):
		{
			struct udc_descriptor *desc;
			unsigned char* data = NULL;
			unsigned n;
			/* usb_highspeed? */
			for (desc = desc_list; desc; desc = desc->next) {
				if (desc->tag == s.value) {
					/*Check for configuration type of descriptor*/
					if (desc->tag == (TYPE_CONFIGURATION << 8)) {
						data = desc->data;
						data+= 9; /* skip config desc */
						data+= 9; /* skip interface desc */
						/* taking the max packet size based on the USB host speed connected */
						for (n = 0; n < 2; n++) {
							data[4] = usb_highspeed ? 512:64;
							data[5] = (usb_highspeed ? 512:64)>>8;
							data += 7;
						}
					}
					unsigned len = desc->len;
					if (len > s.length)
						len = s.length;
					setup_tx(desc->data, len);
					return;
				}
			}
			break;
		}
	case SETUP(DEVICE_READ, GET_CONFIGURATION):
		/* disabling this causes data transaction failures on OSX. Why? */
		if ((s.value == 0) && (s.index == 0) && (s.length == 1)) {
			setup_tx(&usb_config_value, 1);
			return;
		}
		break;
	case SETUP(DEVICE_WRITE, SET_CONFIGURATION):
		if (s.value == 1) {
			struct udc_endpoint *ept;
			/* enable endpoints */
			for (ept = ept_list; ept; ept = ept->next) {
				if (ept->num == 0)
					continue;
				endpoint_enable(ept, s.value);
			}
			usb_config_value = 1;
			the_gadget->notify(the_gadget, UDC_EVENT_ONLINE);
		} else {
			writel(0, USB_ENDPTCTRL(1));
			usb_config_value = 0;
			the_gadget->notify(the_gadget, UDC_EVENT_OFFLINE);
		}
		setup_ack();
		usb_online = s.value ? 1 : 0;
		usb_status(s.value ? 1 : 0, usb_highspeed);
		return;
	case SETUP(DEVICE_WRITE, SET_ADDRESS):
		/* write address delayed (will take effect
		 ** after the next IN txn)
		 */
		writel((s.value << 25) | (1 << 24), USB_DEVICEADDR);
		setup_ack();
		return;
	case SETUP(INTERFACE_WRITE, SET_INTERFACE):
		/* if we ack this everything hangs */
		/* per spec, STALL is valid if there is not alt func */
		goto stall;
	case SETUP(DEVICE_WRITE, SET_FEATURE):
		test_mode = s.index;
		setup_ack();
		return;
	case SETUP(ENDPOINT_WRITE, CLEAR_FEATURE):
		{
			struct udc_endpoint *ept;
			unsigned num = s.index & 15;
			unsigned in = !!(s.index & 0x80);

			if ((s.value == 0) && (s.length == 0)) {
				DBG("clr feat %d %d\n", num, in);
				for (ept = ept_list; ept; ept = ept->next) {
					if ((ept->num == num)
					    && (ept->in == in)) {
						endpoint_enable(ept, 1);
						setup_ack();
						return;
					}
				}
			}
			break;
		}
	}

	dprintf(INFO, "STALL %s %d %d %d %d %d\n",
		reqname(s.request),
		s.type, s.request, s.value, s.index, s.length);

 stall:
	writel((1 << 16) | (1 << 0), USB_ENDPTCTRL(ept->num));
}

unsigned ulpi_read(unsigned reg)
{
	/* initiate read operation */
	writel(ULPI_RUN | ULPI_READ | ULPI_ADDR(reg), USB_ULPI_VIEWPORT);

	/* wait for completion */
	while (readl(USB_ULPI_VIEWPORT) & ULPI_RUN) ;

	return ULPI_DATA_READ(readl(USB_ULPI_VIEWPORT));
}

void ulpi_write(unsigned val, unsigned reg)
{
	/* initiate write operation */
	writel(ULPI_RUN | ULPI_WRITE |
	       ULPI_ADDR(reg) | ULPI_DATA(val), USB_ULPI_VIEWPORT);

	/* wait for completion */
	while (readl(USB_ULPI_VIEWPORT) & ULPI_RUN) ;
}


int udc_init(struct udc_device *dev)
{
	DBG("udc_init():\n");

	hsusb_clock_init();

	/* RESET */
	writel(0x00080002, USB_USBCMD);

	thread_sleep(20);

	while((readl(USB_USBCMD)&2));

	/* select ULPI phy */
	writel(0x80000000, USB_PORTSC);

	/* Do any target specific intialization like GPIO settings,
	 * LDO, PHY configuration etc. needed before USB port can be used.
	 */
	target_usb_init();

	/* USB_OTG_HS_AHB_BURST */
	writel(0x0, USB_SBUSCFG);

	/* USB_OTG_HS_AHB_MODE: HPROT_MODE */
	/* Bus access related config. */
	writel(0x08, USB_AHB_MODE);

	epts = memalign(lcm(4096, CACHE_LINE), ROUNDUP(4096, CACHE_LINE));
	ASSERT(epts);

	dprintf(INFO, "USB init ept @ %p\n", epts);
	memset(epts, 0, 32 * sizeof(struct ept_queue_head));
	arch_clean_invalidate_cache_range((addr_t) epts,
					  32 * sizeof(struct ept_queue_head));

	writel((unsigned)PA((addr_t)epts), USB_ENDPOINTLISTADDR);

	/* select DEVICE mode */
	writel(0x02, USB_USBMODE);

	writel(0xffffffff, USB_ENDPTFLUSH);
	thread_sleep(20);

	ep0out = _udc_endpoint_alloc(0, 0, 64);
	ep0in = _udc_endpoint_alloc(0, 1, 64);
	ep0req = udc_request_alloc();
	ep0req->buf = memalign(CACHE_LINE, ROUNDUP(4096, CACHE_LINE));

	{
		/* create and register a language table descriptor */
		/* language 0x0409 is US English */
		struct udc_descriptor *desc =
		    udc_descriptor_alloc(TYPE_STRING, 0, 4);
		desc->data[2] = 0x09;
		desc->data[3] = 0x04;
		udc_descriptor_register(desc);
	}

	the_device = dev;
	return 0;
}

enum handler_return udc_interrupt(void *arg)
{
	struct udc_endpoint *ept;
	unsigned ret = INT_NO_RESCHEDULE;
	unsigned n = readl(USB_USBSTS);
	writel(n, USB_USBSTS);

	DBG("\nudc_interrupt(): status = 0x%x\n", n);

	n &= (STS_SLI | STS_URI | STS_PCI | STS_UI | STS_UEI);

	if (n == 0) {
		DBG("n = 0\n");
		return ret;
	}

	if (n & STS_URI) {
		DBG("STS_URI\n");

		writel(readl(USB_ENDPTCOMPLETE), USB_ENDPTCOMPLETE);
		writel(readl(USB_ENDPTSETUPSTAT), USB_ENDPTSETUPSTAT);
		writel(0xffffffff, USB_ENDPTFLUSH);
		writel(0, USB_ENDPTCTRL(1));
		dprintf(INFO, "-- reset --\n");
		usb_online = 0;
		usb_config_value = 0;
		the_gadget->notify(the_gadget, UDC_EVENT_OFFLINE);

		/* error out any pending reqs */
		for (ept = ept_list; ept; ept = ept->next) {
			/* ensure that ept_complete considers
			 * this to be an error state
			 */
			if (ept->req) {
				ept->req->item->info = INFO_HALTED;
				handle_ept_complete(ept);
			}
		}
		usb_status(0, usb_highspeed);
	}
	if (n & STS_SLI) {
		DBG("-- suspend --\n");
	}
	if (n & STS_PCI) {
		dprintf(INFO, "-- portchange --\n");
		unsigned spd = (readl(USB_PORTSC) >> 26) & 3;
		if (spd == 2) {
			usb_highspeed = 1;
		} else {
			usb_highspeed = 0;
		}
	}
	if (n & STS_UEI) {
		DBG("STS_UEI\n");
		dprintf(INFO, "<UEI %x>\n", readl(USB_ENDPTCOMPLETE));
	}
	if ((n & STS_UI) || (n & STS_UEI)) {

		if (n & STS_UEI)
			DBG("ERROR ");
		if (n & STS_UI)
			DBG("USB ");

		n = readl(USB_ENDPTSETUPSTAT);
		if (n & EPT_RX(0)) {
			handle_setup(ep0out);
			ret = INT_RESCHEDULE;
		}

		n = readl(USB_ENDPTCOMPLETE);
		if (n != 0) {
			writel(n, USB_ENDPTCOMPLETE);
		}

		for (ept = ept_list; ept; ept = ept->next) {
			if (n & ept->bit) {
				handle_ept_complete(ept);
				ret = INT_RESCHEDULE;
			}
		}
	}
	return ret;
}

int udc_register_gadget(struct udc_gadget *gadget)
{
	if (the_gadget) {
		dprintf(CRITICAL, "only one gadget supported\n");
		return -1;
	}
	the_gadget = gadget;
	return 0;
}

static void udc_ept_desc_fill(struct udc_endpoint *ept, unsigned char *data)
{
	data[0] = 7;
	data[1] = TYPE_ENDPOINT;
	data[2] = ept->num | (ept->in ? 0x80 : 0x00);
	data[3] = 0x02;		/* bulk -- the only kind we support */
	data[4] = ept->maxpkt;
	data[5] = ept->maxpkt >> 8;
	data[6] = ept->in ? 0x00 : 0x01;
}

static unsigned udc_ifc_desc_size(struct udc_gadget *g)
{
	return 9 + g->ifc_endpoints * 7;
}

static void udc_ifc_desc_fill(struct udc_gadget *g, unsigned char *data)
{
	unsigned n;

	data[0] = 0x09;
	data[1] = TYPE_INTERFACE;
	data[2] = 0x00;		/* ifc number */
	data[3] = 0x00;		/* alt number */
	data[4] = g->ifc_endpoints;
	data[5] = g->ifc_class;
	data[6] = g->ifc_subclass;
	data[7] = g->ifc_protocol;
	data[8] = udc_string_desc_alloc(g->ifc_string);

	data += 9;
	for (n = 0; n < g->ifc_endpoints; n++) {
		udc_ept_desc_fill(g->ept[n], data);
		data += 7;
	}
}

int udc_start(void)
{
	struct udc_descriptor *desc;
	unsigned char *data;
	unsigned size;
	uint32_t val;

	dprintf(ALWAYS, "udc_start()\n");

	if (!the_device) {
		dprintf(CRITICAL, "udc cannot start before init\n");
		return -1;
	}
	if (!the_gadget) {
		dprintf(CRITICAL, "udc has no gadget registered\n");
		return -1;
	}

	/* create our device descriptor */
	desc = udc_descriptor_alloc(TYPE_DEVICE, 0, 18);
	data = desc->data;
	data[2] = 0x00;		/* usb spec minor rev */
	data[3] = 0x02;		/* usb spec major rev */
	data[4] = 0x00;		/* class */
	data[5] = 0x00;		/* subclass */
	data[6] = 0x00;		/* protocol */
	data[7] = 0x40;		/* max packet size on ept 0 */
	memcpy(data + 8, &the_device->vendor_id, sizeof(short));
	memcpy(data + 10, &the_device->product_id, sizeof(short));
	memcpy(data + 12, &the_device->version_id, sizeof(short));
	data[14] = udc_string_desc_alloc(the_device->manufacturer);
	data[15] = udc_string_desc_alloc(the_device->product);
	data[16] = udc_string_desc_alloc(the_device->serialno);
	data[17] = 1;		/* number of configurations */
	udc_descriptor_register(desc);

	/* create our configuration descriptor */
	size = 9 + udc_ifc_desc_size(the_gadget);
	desc = udc_descriptor_alloc(TYPE_CONFIGURATION, 0, size);
	data = desc->data;
	data[0] = 0x09;
	data[2] = size;
	data[3] = size >> 8;
	data[4] = 0x01;		/* number of interfaces */
	data[5] = 0x01;		/* configuration value */
	data[6] = 0x00;		/* configuration string */
	data[7] = 0x80;		/* attributes */
	data[8] = 0x80;		/* max power (250ma) -- todo fix this */
	udc_ifc_desc_fill(the_gadget, data + 9);
	udc_descriptor_register(desc);

	register_int_handler(INT_USB_HS, udc_interrupt, (void *)0);
	writel(STS_URI | STS_SLI | STS_UI | STS_PCI, USB_USBINTR);
	unmask_interrupt(INT_USB_HS);

	/* go to RUN mode (D+ pullup enable) */
	val = readl(USB_USBCMD);

	writel(val | 0x00080001, USB_USBCMD);

	return 0;
}

int udc_stop(void)
{
	uint32_t val;

	/* Flush all primed end points. */
	writel(0xffffffff, USB_ENDPTFLUSH);

	/* Stop controller. */
	val = readl(USB_USBCMD);
	writel(val & ~USBCMD_ATTACH, USB_USBCMD);

	/* Mask the interrupts. */
	writel(0, USB_USBINTR);
	mask_interrupt(INT_USB_HS);

	/* Perform any target specific clean up. */
	target_usb_stop();

	/* Reset the controller. */
	writel(USBCMD_RESET, USB_USBCMD);
	/* Wait until reset completes. */
	while(readl(USB_USBCMD) & USBCMD_RESET);

	return 0;
}
