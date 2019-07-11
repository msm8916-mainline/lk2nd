/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
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
#include <string.h>
#include <stdlib.h>
#include <platform.h>
#include <target.h>
#include <kernel/thread.h>
#include <kernel/event.h>
#include <dev/udc.h>
#include "fastboot.h"

#ifdef USB30_SUPPORT
#include <usb30_udc.h>
#endif

typedef struct
{
	int (*udc_init)(struct udc_device *devinfo);
	int (*udc_register_gadget)(struct udc_gadget *gadget);
	int (*udc_start)(void);
	int (*udc_stop)(void);

	struct udc_endpoint *(*udc_endpoint_alloc)(unsigned type, unsigned maxpkt);
	void (*udc_endpoint_free)(struct udc_endpoint *ept);
	struct udc_request *(*udc_request_alloc)(void);
	void (*udc_request_free)(struct udc_request *req);

	int (*usb_read)(void *buf, unsigned len);
	int (*usb_write)(void *buf, unsigned len);
} usb_controller_interface_t;

usb_controller_interface_t usb_if;

#define MAX_USBFS_BULK_SIZE (32 * 1024)

void boot_linux(void *bootimg, unsigned sz);
static void fastboot_notify(struct udc_gadget *gadget, unsigned event);
static struct udc_endpoint *fastboot_endpoints[2];

static struct udc_device surf_udc_device = {
	.vendor_id    = 0x18d1,
	.product_id   = 0xD00D,
	.version_id   = 0x0100,
	.manufacturer = "Google",
	.product      = "Android",
};

static struct udc_gadget fastboot_gadget = {
	.notify        = fastboot_notify,
	.ifc_class     = 0xff,
	.ifc_subclass  = 0x42,
	.ifc_protocol  = 0x03,
	.ifc_endpoints = 2,
	.ifc_string    = "fastboot",
	.ept           = fastboot_endpoints,
};

/* todo: give lk strtoul and nuke this */
static unsigned hex2unsigned(const char *x)
{
    unsigned n = 0;

    while(*x) {
        switch(*x) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            n = (n << 4) | (*x - '0');
            break;
        case 'a': case 'b': case 'c':
        case 'd': case 'e': case 'f':
            n = (n << 4) | (*x - 'a' + 10);
            break;
        case 'A': case 'B': case 'C':
        case 'D': case 'E': case 'F':
            n = (n << 4) | (*x - 'A' + 10);
            break;
        default:
            return n;
        }
        x++;
    }

    return n;
}

struct fastboot_cmd {
	struct fastboot_cmd *next;
	const char *prefix;
	unsigned prefix_len;
	void (*handle)(const char *arg, void *data, unsigned sz);
};

struct fastboot_var {
	struct fastboot_var *next;
	const char *name;
	const char *value;
};

static struct fastboot_cmd *cmdlist;

void fastboot_register(const char *prefix,
		       void (*handle)(const char *arg, void *data, unsigned sz))
{
	struct fastboot_cmd *cmd;
	cmd = malloc(sizeof(*cmd));
	if (cmd) {
		cmd->prefix = prefix;
		cmd->prefix_len = strlen(prefix);
		cmd->handle = handle;
		cmd->next = cmdlist;
		cmdlist = cmd;
	}
}

static struct fastboot_var *varlist;

void fastboot_publish(const char *name, const char *value)
{
	struct fastboot_var *var;
	var = malloc(sizeof(*var));
	if (var) {
		var->name = name;
		var->value = value;
		var->next = varlist;
		varlist = var;
	}
}


static event_t usb_online;
static event_t txn_done;
static struct udc_endpoint *in, *out;
static struct udc_request *req;
int txn_status;

static void *download_base;
static unsigned download_max;
static unsigned download_size;

#define STATE_OFFLINE	0
#define STATE_COMMAND	1
#define STATE_COMPLETE	2
#define STATE_ERROR	3

static unsigned fastboot_state = STATE_OFFLINE;

static void req_complete(struct udc_request *req, unsigned actual, int status)
{
	txn_status = status;
	req->length = actual;

	event_signal(&txn_done, 0);
}

#ifdef USB30_SUPPORT
static int usb30_usb_read(void *buf, unsigned len)
{
	int r;
	struct udc_request req;

	ASSERT(buf);
	ASSERT(len);

	if (fastboot_state == STATE_ERROR)
		goto oops;

	dprintf(SPEW, "usb_read(): len = %d\n", len);

	req.buf      = (void*) PA((addr_t)buf);
	req.length   = len;
	req.complete = req_complete;

	r = usb30_udc_request_queue(out, &req);
	if (r < 0)
	{
		dprintf(CRITICAL, "usb_read() queue failed. r = %d\n", r);
		goto oops;
	}
	event_wait(&txn_done);

	if (txn_status < 0)
	{
		dprintf(CRITICAL, "usb_read() transaction failed. txn_status = %d\n",
				txn_status);
		goto oops;
	}

	/* note: req->length is update by callback to reflect the amount of data
	 * actually read.
	 */
	dprintf(SPEW, "usb_read(): DONE. req.length = %d\n", req.length);

	/* invalidate any cached buf data (controller updates main memory) */
	arch_invalidate_cache_range((addr_t) buf, len);

	return req.length;

oops:
	fastboot_state = STATE_ERROR;
	dprintf(CRITICAL, "usb_read(): DONE: ERROR: len = %d\n", len);
	return -1;
}

static int usb30_usb_write(void *buf, unsigned len)
{
	int r;
	struct udc_request req;

	ASSERT(buf);
	ASSERT(len);

	if (fastboot_state == STATE_ERROR)
		goto oops;

	dprintf(SPEW, "usb_write(): len = %d str = %s\n", len, (char *) buf);

	/* flush buffer to main memory before giving to udc */
	arch_clean_invalidate_cache_range((addr_t) buf, len);

	req.buf      = (void*) PA((addr_t)buf);
	req.length   = len;
	req.complete = req_complete;

	r = usb30_udc_request_queue(in, &req);
	if (r < 0) {
		dprintf(CRITICAL, "usb_write() queue failed. r = %d\n", r);
		goto oops;
	}
	event_wait(&txn_done);

	dprintf(SPEW, "usb_write(): DONE: len = %d req->length = %d str = %s\n",
			len, req.length, (char *) buf);

	if (txn_status < 0) {
		dprintf(CRITICAL, "usb_write() transaction failed. txn_status = %d\n",
				txn_status);
		goto oops;
	}

	return req.length;

oops:
	fastboot_state = STATE_ERROR;
	dprintf(CRITICAL, "usb_write(): DONE: ERROR: len = %d\n", len);
	return -1;
}
#endif

static int hsusb_usb_read(void *_buf, unsigned len)
{
	int r;
	unsigned xfer;
	unsigned char *buf = _buf;
	int count = 0;

	if (fastboot_state == STATE_ERROR)
		goto oops;

	while (len > 0) {
		xfer = (len > MAX_USBFS_BULK_SIZE) ? MAX_USBFS_BULK_SIZE : len;
		req->buf = PA((addr_t)buf);
		req->length = xfer;
		req->complete = req_complete;
		r = udc_request_queue(out, req);
		if (r < 0) {
			dprintf(INFO, "usb_read() queue failed\n");
			goto oops;
		}
		event_wait(&txn_done);

		if (txn_status < 0) {
			dprintf(INFO, "usb_read() transaction failed\n");
			goto oops;
		}

		count += req->length;
		buf += req->length;
		len -= req->length;

		/* short transfer? */
		if (req->length != xfer) break;
	}
	/*
	 * Force reload of buffer from memory
	 * since transaction is complete now.
	 */
	arch_invalidate_cache_range(_buf, count);
	return count;

oops:
	fastboot_state = STATE_ERROR;
	return -1;
}

static int hsusb_usb_write(void *buf, unsigned len)
{
	int r;
	uint32_t xfer;
	unsigned char *_buf = buf;
	int count = 0;

	if (fastboot_state == STATE_ERROR)
		goto oops;

	while (len > 0) {
		xfer = (len > MAX_USBFS_BULK_SIZE) ? MAX_USBFS_BULK_SIZE : len;
		req->buf = PA((addr_t)_buf);
		req->length = xfer;
		req->complete = req_complete;
		r = udc_request_queue(in, req);
		if (r < 0) {
			dprintf(INFO, "usb_write() queue failed\n");
			goto oops;
		}
		event_wait(&txn_done);
		if (txn_status < 0) {
			dprintf(INFO, "usb_write() transaction failed\n");
			goto oops;
		}

		count += req->length;
		_buf += req->length;
		len -= req->length;

		/* short transfer? */
		if (req->length != xfer) break;
	}

	return count;

oops:
	fastboot_state = STATE_ERROR;
	return -1;
}

void fastboot_ack(const char *code, const char *reason)
{
	STACKBUF_DMA_ALIGN(response, MAX_RSP_SIZE);

	if (fastboot_state != STATE_COMMAND)
		return;

	if (reason == 0)
		reason = "";

	snprintf(response, MAX_RSP_SIZE, "%s%s", code, reason);
	fastboot_state = STATE_COMPLETE;

	usb_if.usb_write(response, strlen(response));

}

void fastboot_info(const char *reason)
{
	STACKBUF_DMA_ALIGN(response, MAX_RSP_SIZE);

	if (fastboot_state != STATE_COMMAND)
		return;

	if (reason == 0)
		return;

	snprintf(response, MAX_RSP_SIZE, "INFO%s", reason);

	usb_if.usb_write(response, strlen(response));
}

void fastboot_fail(const char *reason)
{
	fastboot_ack("FAIL", reason);
}

void fastboot_okay(const char *info)
{
	fastboot_ack("OKAY", info);
}

void fastboot_stage(const void *data, unsigned sz)
{
	arch_invalidate_cache_range((addr_t) download_base, download_size);
	download_size = 0;

	if (sz > download_max) {
		fastboot_fail("data too large");
		return;
	}

	memcpy(download_base, data, sz);
	download_size = sz;

	fastboot_okay("");
}

static void cmd_getvar(const char *arg, void *data, unsigned sz)
{
	struct fastboot_var *var;

	for (var = varlist; var; var = var->next) {
		if (!strcmp(var->name, arg)) {
			fastboot_okay(var->value);
			return;
		}
	}
	fastboot_okay("");
}

static void cmd_help(const char *arg, void *data, unsigned sz)
{
	struct fastboot_cmd *cmd;
	char response[128];

	// print commands
	fastboot_info("commands:");
	for (cmd = cmdlist; cmd; cmd = cmd->next) {
		char buf[cmd->prefix_len+1];

		if (!memcpy(buf, cmd->prefix, cmd->prefix_len))
				continue;

		buf[cmd->prefix_len] = '\0';

		snprintf(response, sizeof(response), "\t%s", buf);
		fastboot_info(response);
	}

	fastboot_okay("");
}

static void cmd_download(const char *arg, void *data, unsigned sz)
{
	STACKBUF_DMA_ALIGN(response, MAX_RSP_SIZE);
	unsigned len = hex2unsigned(arg);
	int r;

	download_size = 0;
	if (len > download_max) {
		fastboot_fail("data too large");
		return;
	}

	snprintf(response, MAX_RSP_SIZE, "DATA%08x", len);
	if (usb_if.usb_write(response, strlen(response)) < 0)
		return;
	/*
	 * Discard the cache contents before starting the download
	 */
	arch_invalidate_cache_range((addr_t) download_base, sz);

	r = usb_if.usb_read(download_base, len);
	if ((r < 0) || ((unsigned) r != len)) {
		fastboot_state = STATE_ERROR;
		return;
	}
	download_size = len;
	fastboot_okay("");
}

static void cmd_upload(const char *arg, void *data, unsigned sz)
{
	STACKBUF_DMA_ALIGN(response, MAX_RSP_SIZE);
	int r;

	if (!sz) {
		fastboot_fail("no data staged");
		return;
	}

	snprintf(response, MAX_RSP_SIZE, "DATA%08x", sz);
	if (usb_if.usb_write(response, strlen(response)) < 0)
		return;

	r = usb_if.usb_write(data, sz);
	if ((r < 0) || ((unsigned) r != sz)) {
		fastboot_state = STATE_ERROR;
		return;
	}
	fastboot_okay("");
}

static void fastboot_command_loop(void)
{
	struct fastboot_cmd *cmd;
	int r;
	dprintf(INFO,"fastboot: processing commands\n");

	uint8_t *buffer = (uint8_t *)memalign(CACHE_LINE, ROUNDUP(4096, CACHE_LINE));
	if (!buffer)
	{
		dprintf(CRITICAL, "Could not allocate memory for fastboot buffer\n.");
		ASSERT(0);
	}
again:
	while (fastboot_state != STATE_ERROR) {

		/* Read buffer must be cleared first. If buffer is not cleared,
		 * the original data in buf trailing the received command is
		 * interpreted as part of the command.
		 */
		memset(buffer, 0, MAX_RSP_SIZE);
		arch_clean_invalidate_cache_range((addr_t) buffer, MAX_RSP_SIZE);

		r = usb_if.usb_read(buffer, MAX_RSP_SIZE);
		if (r < 0) break;
		buffer[r] = 0;
		dprintf(INFO,"fastboot: %s\n", buffer);

		fastboot_state = STATE_COMMAND;

		for (cmd = cmdlist; cmd; cmd = cmd->next) {
			size_t cmdlen = strlen((char*)buffer);

			if (memcmp(buffer, cmd->prefix, cmd->prefix_len))
				continue;

			if (cmdlen>cmd->prefix_len && buffer[cmd->prefix_len]!=' ' && buffer[cmd->prefix_len-1]!=':')
				continue;

			const char* arg = (const char*) buffer + cmd->prefix_len;
			if(arg[0]==' ')
				arg++;

			cmd->handle(arg,
				    (void*) download_base, download_size);
			if (fastboot_state == STATE_COMMAND)
				fastboot_fail("unknown reason");
			goto again;
		}

		fastboot_info("unknown command");
		fastboot_info("See 'fastboot oem help'");
		fastboot_fail("");

	}
	fastboot_state = STATE_OFFLINE;
	dprintf(INFO,"fastboot: oops!\n");
	free(buffer);
}

static int fastboot_handler(void *arg)
{
	for (;;) {
		event_wait(&usb_online);
		fastboot_command_loop();
	}
	return 0;
}

static void fastboot_notify(struct udc_gadget *gadget, unsigned event)
{
	if (event == UDC_EVENT_ONLINE) {
		event_signal(&usb_online, 0);
	}
}

int fastboot_init(void *base, unsigned size)
{
	char sn_buf[13];
	thread_t *thr;
	dprintf(INFO, "fastboot_init()\n");

	download_base = base;
	download_max = size;

	/* target specific initialization before going into fastboot. */
	target_fastboot_init();

	/* setup serialno */
	target_serialno((unsigned char *) sn_buf);
	dprintf(SPEW,"serial number: %s\n",sn_buf);
	surf_udc_device.serialno = sn_buf;

	if(!strcmp(target_usb_controller(), "dwc"))
	{
#ifdef USB30_SUPPORT
		surf_udc_device.t_usb_if = target_usb30_init();

		/* initialize udc functions to use dwc controller */
		usb_if.udc_init            = usb30_udc_init;
		usb_if.udc_register_gadget = usb30_udc_register_gadget;
		usb_if.udc_start           = usb30_udc_start;
		usb_if.udc_stop            = usb30_udc_stop;

		usb_if.udc_endpoint_alloc  = usb30_udc_endpoint_alloc;
		usb_if.udc_request_alloc   = usb30_udc_request_alloc;
		usb_if.udc_request_free    = usb30_udc_request_free;

		usb_if.usb_read            = usb30_usb_read;
		usb_if.usb_write           = usb30_usb_write;
#else
		dprintf(CRITICAL, "USB30 needs to be enabled for this target.\n");
		ASSERT(0);
#endif
	}
	else
	{
		/* initialize udc functions to use the default chipidea controller */
		usb_if.udc_init            = udc_init;
		usb_if.udc_register_gadget = udc_register_gadget;
		usb_if.udc_start           = udc_start;
		usb_if.udc_stop            = udc_stop;

		usb_if.udc_endpoint_alloc  = udc_endpoint_alloc;
		usb_if.udc_request_alloc   = udc_request_alloc;
		usb_if.udc_request_free    = udc_request_free;

		usb_if.usb_read            = hsusb_usb_read;
		usb_if.usb_write           = hsusb_usb_write;
	}

	/* register udc device */
	usb_if.udc_init(&surf_udc_device);

	event_init(&usb_online, 0, EVENT_FLAG_AUTOUNSIGNAL);
	event_init(&txn_done, 0, EVENT_FLAG_AUTOUNSIGNAL);

	in = usb_if.udc_endpoint_alloc(UDC_TYPE_BULK_IN, 512);
	if (!in)
		goto fail_alloc_in;
	out = usb_if.udc_endpoint_alloc(UDC_TYPE_BULK_OUT, 512);
	if (!out)
		goto fail_alloc_out;

	fastboot_endpoints[0] = in;
	fastboot_endpoints[1] = out;

	req = usb_if.udc_request_alloc();
	if (!req)
		goto fail_alloc_req;

	/* register gadget */
	if (usb_if.udc_register_gadget(&fastboot_gadget))
		goto fail_udc_register;

	fastboot_register("oem help", cmd_help);
	fastboot_register("getvar:", cmd_getvar);
	fastboot_register("download:", cmd_download);
	fastboot_register("upload", cmd_upload);
	fastboot_publish("version", "0.5");

	thr = thread_create("fastboot", fastboot_handler, 0, DEFAULT_PRIORITY, 4096);
	if (!thr)
	{
		goto fail_alloc_in;
	}
	thread_resume(thr);

	usb_if.udc_start();

	return 0;

fail_udc_register:
	usb_if.udc_request_free(req);
fail_alloc_req:
	usb_if.udc_endpoint_free(out);
fail_alloc_out:
	usb_if.udc_endpoint_free(in);
fail_alloc_in:
	return -1;
}

void fastboot_stop(void)
{
	usb_if.udc_stop();
}
