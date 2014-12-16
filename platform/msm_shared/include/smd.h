/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Fundation, Inc. nor the names of its
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

#ifndef __SMD_H
#define __SMD_H

#include <arch/defines.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <kernel/event.h>

#define SMD_CHANNEL_ALLOC_MAX 2048
#define SMD_CHANNEL_NAME_SIZE_MAX 20

/* Stream related states */
#define SMD_SS_CLOSED            0x0 /* Closed: must be 0 */
#define SMD_SS_OPENING           0x1 /* Stream is opening */
#define SMD_SS_OPENED            0x2 /* Stream is opened */
#define SMD_SS_FLUSHING          0x3 /* Stream is flushing */
#define SMD_SS_CLOSING           0x4 /* Stream is closing */
#define SMD_SS_RESET             0x5 /* Stream is resetting */
#define SMD_SS_RESET_OPENING     0x6 /* Stream reset on local */

typedef enum
{
	SMD_APPS_RPM=0x0F,
} smd_channel_type;

typedef struct
{
	uint32_t stream_state;
	uint32_t DTR_DSR;
	uint32_t CTS_RTS;
	uint32_t CD;
	uint32_t RI;
	uint32_t data_written;
	uint32_t data_read;
	uint32_t state_updated;
	uint32_t mask_recv_intr;
	uint32_t read_index;
	uint32_t write_index;
} smd_shared_stream_info_type;

//Every port has 2 FIFOs, one in each direction
typedef struct
{
	char name[SMD_CHANNEL_NAME_SIZE_MAX];
	uint32_t cid;
	uint32_t ctype;
	uint32_t ref_count;
} smd_channel_alloc_entry_t;

typedef enum
{
	SMD_STREAMING_BUFFER,
} smd_protocol_type;

typedef struct
{
	smd_protocol_type protocol;
	uint32_t port_id;
	smd_channel_type ch_type;
} smd_port_to_info_type;

typedef struct
{
	uint32_t pkt_size;
	uint32_t app_field;
	uint32_t app_ptr;
	uint32_t kind;
	uint32_t priority;
} smd_pkt_hdr;

typedef struct
{
	smd_shared_stream_info_type ch0;
	smd_shared_stream_info_type ch1;
} smd_port_ctrl_info;

typedef struct
{
	smd_channel_alloc_entry_t alloc_entry;
	uint8_t *send_buf;
	uint8_t *recv_buf;
	uint32_t fifo_size;
	smd_port_ctrl_info *port_info;
	uint32_t current_state;
	event_t wevt;
	event_t revt;
} smd_channel_info_t;

int smd_init(smd_channel_info_t *ch, uint32_t ch_type);
void smd_uninit(smd_channel_info_t *ch);
void smd_read(smd_channel_info_t *ch, uint32_t *len, int ch_type, uint32_t *response);
int smd_write(smd_channel_info_t *ch, void *data, uint32_t len, int type);
int smd_get_channel_info(smd_channel_info_t *ch, uint32_t ch_type);
int smd_get_channel_entry(smd_channel_info_t *ch, uint32_t ch_type);
void smd_notify_rpm();
enum handler_return smd_irq_handler(void* data);
void smd_set_state(smd_channel_info_t *ch, uint32_t state, uint32_t flag);
void smd_signal_read_complete(smd_channel_info_t *ch, uint32_t len);
#endif
