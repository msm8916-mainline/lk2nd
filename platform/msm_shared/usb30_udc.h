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
#ifndef _USB30_UDC_H
#define _USB30_UDC_H

#include <bits.h>
#include <usb30_dwc.h>
#include <usb30_wrapper.h>

#define UDC_DESC_SIZE_CONFIGURATION   9
#define UDC_DESC_SIZE_INTERFACE       9
#define UDC_DESC_SIZE_ENDPOINT        7
#define UDC_DESC_SIZE_ENDPOINT_COMP   6

typedef enum
{
	UDC_DESC_SPEC_20 = BIT(0),
	UDC_DESC_SPEC_30 = BIT(1),
} udc_desc_spec_t;

typedef enum
{
	UDC_SPEED_LS,
	UDC_SPEED_FS,
	UDC_SPEED_HS,
	UDC_SPEED_SS
} udc_device_speed_t;

typedef struct
{
	usb_wrapper_dev_t     *wrapper_dev;     /* store the wrapper device ptr */
	dwc_dev_t             *dwc;             /* store the dwc device ptr */

	struct udc_gadget     *gadget;          /* store the registered gadget. */
	struct udc_descriptor *desc_list;       /* linked list of all descriptors: device, config, string, language table, bos etc. */
	struct udc_endpoint   *ept_list;        /* linked list of all eps */
	uint32_t               ept_alloc_table; /* keep track of which usb EPs are allocated. Initialized to assume EP0 In/OUT are already allocated.*/
	uint32_t               next_string_id;  /* keeps track of string id for string descriptor */
	void                  *ctrl_rx_buf;     /* buffer pointer to receive ctrl data. */
	void                  *ctrl_tx_buf;     /* buffer pointer to send ctrl data. */


	udc_device_speed_t     speed;           /* keeps track of usb connection speed. */
	uint8_t                config_selected; /* keeps track of the selected configuration */

	struct udc_request    *queued_req;      /* pointer to the currently queued request. NULL indicates no request is queued. */

} udc_t;


struct udc_descriptor {
	struct udc_descriptor *next;
	uint16_t               tag;     /* ((TYPE << 8) | NUM) */
	uint16_t               len;     /* total length */
	udc_desc_spec_t        spec;    /* bit map of spec that this desc complies with. */
	uint8_t                data[0]; /* descriptor contents. */
};

struct udc_endpoint {
	struct udc_endpoint *next;
	uint8_t              num;
	uint8_t              in;
	uint16_t             maxpkt;
	uint32_t             maxburst;  /* max pkts that this ep can transfer before waiting for ack. */

	dwc_trb_t           *trb;       /* pointer to buffer used for TRB chain */
	uint32_t             trb_count; /* size of TRB chain. */
};

struct udc_request *usb30_udc_request_alloc(void);
struct udc_endpoint *usb30_udc_endpoint_alloc(unsigned type, unsigned maxpkt);
void usb30_udc_request_free(struct udc_request *req);
int usb30_udc_request_queue(struct udc_endpoint *ept, struct udc_request *req);
int usb30_udc_request_cancel(struct udc_endpoint *ept, struct udc_request *req);

int usb30_udc_init(struct udc_device *devinfo);
int usb30_udc_register_gadget(struct udc_gadget *gadget);
int usb30_udc_start(void);
int usb30_udc_stop(void);
#endif
