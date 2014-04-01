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
#ifndef _USB30_DWC_H
#define _USB30_DWC_H

#include <bits.h>


/********************* START: h/w defined values ******************************/
/* device command ids */
typedef enum
{
	DWC_DEV_CMD_TX_SET_LINK_FN_LMP_VAL     = 0x01,
	DWC_DEV_CMD_SET_PERIODIC_PARAMS_VAL    = 0x02,
	DWC_DEV_CMD_TX_FN_WAKE_DEV_NOTIFY_VAL  = 0x03,
	DWC_DEV_CMD_SET_SCRATCHPAD_BUF_LO_VAL  = 0x04,
	DWC_DEV_CMD_SET_SCRATCHPAD_BUF_HI_VAL  = 0x05,
	DWC_DEV_CMD_TX_FN_HOST_REQ_NOTIFY_VAL  = 0x06,
	DWC_DEV_CMD_TX_DEVICE_NOTIFY_VAL       = 0x07,
	DWC_DEV_CMD_SELECTED_FIFO_FLUSH_VAL    = 0x09,
	DWC_DEV_CMD_ALL_FIFO_FLUSH_VAL         = 0x0A,
	DWC_DEV_CMD_SET_EP_NRDY_VAL            = 0x0C,
	DWC_DEV_CMD_RUN_SOC_LOOPBACK_TEST_VAL  = 0x10,
} dwc_dev_cmd_t;

/* ep command ids */
typedef enum
{
	DEPCMD_CMD_SET_EP_CONF     = 0x1,
	DEPCMD_CMD_SET_TR_CONF     = 0x2,
	DEPCMD_CMD_GET_EP_STATE    = 0x3,
	DEPCMD_CMD_SET_STALL       = 0x4,
	DEPCMD_CMD_CLEAR_STALL     = 0x5,
	DEPCMD_CMD_START_TRANSFER  = 0x6,
	DEPCMD_CMD_UPDATE_TRANSFER = 0x7,
	DEPCMD_CMD_END_TRANSFER    = 0x8,
	DEPCMD_CMD_START_NEW_CONF  = 0x9,
} dwc_dep_cmd_id_t;

/* ep type */
typedef enum {
   EP_TYPE_CONTROL     = 0x0,
   EP_TYPE_ISOCHRONOUS = 0x1,
   EP_TYPE_BULK        = 0x2,
   EP_TYPE_INTERRUPT   = 0x3,
} dwc_ep_type_t;

/* ep direction */
typedef enum
{
	DWC_EP_DIRECTION_OUT = 0x0,
	DWC_EP_DIRECTION_IN  = 0x1
} dwc_ep_direction_t;


/* macros to parse event information */
#define DWC_EVENT_IS_DEVICE_EVENT(_event)                   BIT_SHIFT(_event, 0)

/* parse device events */
#define DWC_EVENT_DEVICE_EVENT_ID(_event)                   BITS_SHIFT(_event, 11, 8)
#define DWC_EVENT_DEVICE_EVENT_INFO(_event)                 BITS_SHIFT(_event, 24, 16)

#define DWC_EVENT_DEVICE_EVENT_INFO_SS_EVENT(_event_info)   BIT_SHIFT(_event_info, 4)
#define DWC_EVENT_DEVICE_EVENT_INFO_LINK_STATE(_event_info) BITS_SHIFT(_event_info, 3, 0)

/* parse ep events */
#define DWC_EVENT_EP_EVENT_PARAM(_event)                    BITS_SHIFT(_event, 31, 16)
#define DWC_EVENT_EP_EVENT_CMD_TYPE(_event)                 BITS_SHIFT(_event, 27, 24)
#define DWC_EVENT_EP_EVENT_XFER_RES_IDX(_event)             BITS_SHIFT(_event, 22, 16)
#define DWC_EVENT_EP_EVENT_STATUS(_event)                   BITS_SHIFT(_event, 15, 12)
#define DWC_EVENT_EP_EVENT_CTRL_STAGE(_event)               BITS_SHIFT(_event, 13, 12)
#define DWC_EVENT_EP_EVENT_ID(_event)                       BITS_SHIFT(_event,  9,  6)
#define DWC_EVENT_EP_EVENT_EP_NUM(_event)                   BITS_SHIFT(_event,  5,  1)

/* device event ids */
typedef enum
{
	DWC_EVENT_DEVICE_EVENT_ID_VENDOR_DEVICE_TEST_LMP = 12,
	DWC_EVENT_DEVICE_EVENT_ID_BUFFER_OVERFLOW        = 11,
	DWC_EVENT_DEVICE_EVENT_ID_GENERIC_CMD_COMPLETE   = 10,
	DWC_EVENT_DEVICE_EVENT_ID_ERRATIC_ERROR          = 9,
	DWC_EVENT_DEVICE_EVENT_ID_SOF                    = 7,
	DWC_EVENT_DEVICE_EVENT_ID_SUSPEND_ENTRY          = 6,
	DWC_EVENT_DEVICE_EVENT_ID_HIBER                  = 5,
	DWC_EVENT_DEVICE_EVENT_ID_WAKEUP                 = 4,
	DWC_EVENT_DEVICE_EVENT_ID_USB_LINK_STATUS_CHANGE = 3,
	DWC_EVENT_DEVICE_EVENT_ID_CONNECT_DONE           = 2,
	DWC_EVENT_DEVICE_EVENT_ID_USB_RESET              = 1,
	DWC_EVENT_DEVICE_EVENT_ID_DISCONNECT             = 0,
	DWC_EVENT_DEVICE_EVENTS_ALL                      =  BITS(0xFFFFFFFF, 12, 0)
} dwc_event_device_event_id_t;

/* ep event ids */
typedef enum
{
	DWC_EVENT_EP_CMD_COMPLETE     = 7,
	DWC_EVENT_EP_XFER_NOT_READY   = 3,
	DWC_EVENT_EP_XFER_IN_PROGRESS = 2,
	DWC_EVENT_EP_XFER_COMPLETE    = 1,
} dwc_event_ep_event_id_t;

/* values for control stage in ep events */
#define CONTROL_DATA_REQUEST     1
#define CONTROL_STATUS_REQUEST   2

/* values for event status field for transfer complete event */
#define DWC_XFER_COMPLETE_EVT_STATUS_SHORT_PKT 0x2
#define DWC_XFER_COMPLETE_EVT_STATUS_IOC       0x4
#define DWC_XFER_COMPLETE_EVT_STATUS_LST       0x8

/* master bus data width (DWC_USB3_MDWIDTH in snps data book) */
#define DWC_MASTER_BUS_WIDTH   8

/* super speed link states */
typedef enum
{
   U0 = 0x0,
   U1,
   U2,
   U3,
   SS_DIS,
   RX_DET,
   SS_INACT,
   POLL,
   RECOV,
   HRESET,
   CMPLY,
   LPBK,
   RESUME_RESET = 0xF,
} dwc_event_device_ss_link_state_t;

/* high speed link states */
typedef enum
{
   ON            = 0x0,
   L1            = 0x2,
   L2            = 0x3,
   DISCONNECTED  = 0x4,
   EARLY_SUSPEND = 0x5,
   RESET         = 0xE,
   RESUME        = 0xF,
} dwc_event_device_hs_link_state_t;

/* action for set config*/
enum
{
	SET_CONFIG_ACTION_INIT    = 0x0,
	SET_CONFIG_ACTION_RESTORE = 0x1,
	SET_CONFIG_ACTION_MODIFY  = 0x2,
};

/* EP Cmd Param bits */
#define DEPCMDPAR1_USB_EP_NUM_BIT     26
#define DEPCMDPAR1_USB_EP_DIR_BIT     25

#define DEPCMDPAR0_ACTION_BIT         30
#define DEPCMDPAR0_BURST_SIZE_BIT     22
#define DEPCMDPAR0_FIFO_NUM_BIT       17
#define DEPCMDPAR0_MAX_PKT_SIZE_BIT   3
#define DEPCMDPAR0_EP_TYPE_BIT        1

#define DEPCMDPAR2_XFER_N_RDY_BIT     10
#define DEPCMDPAR2_XFER_IN_PROG_BIT   9
#define DEPCMDPAR2_XFER_COMPLETE_BIT  8

enum
{
	DSTS_CONNECTSPD_HS  = 0,
	DSTS_CONNECTSPD_FS1 = 1, /* phy clk @ 30 or 60 MHz */
	DSTS_CONNECTSPD_LS  = 2,
	DSTS_CONNECTSPD_FS2 = 3, /* phy clk @ 48 MHz */
	DSTS_CONNECTSPD_SS  = 4,
};

/**************************** TRB (Transfer Request Block)*********************/
#define DWC_TRB_F1_PTR_LOW_BMSK  0xFFFFFFFF
#define DWC_TRB_F1_PTR_LOW_SHFT  0

#define DWC_TRB_F2_PTR_HIGH_BMSK 0xFFFFFFFF
#define DWC_TRB_F2_PTR_HIGH_SHFT 0

#define DWC_TRB_F3_BUFSIZ_BMSK   0x00FFFFFF
#define DWC_TRB_F3_BUFSIZ_SHFT   0
#define DWC_TRB_F3_PCM1_BMSK     0x03000000
#define DWC_TRB_F3_PCM1_SHFT     24
#define DWC_TRB_F3_TRBSTS_BMSK   0xF0000000
#define DWC_TRB_F3_TRBSTS_SHFT   28

#define DWC_TRB_F4_IOC_BMSK      0x800
#define DWC_TRB_F4_IOC_SHFT      11

#define DWC_TRB_F4_ISP_BMSK      0x400
#define DWC_TRB_F4_ISP_SHFT      10

#define DWC_TRB_F4_TRBCTL_BMSK   0x3F0
#define DWC_TRB_F4_TRBCTL_SHFT   4

#define DWC_TRB_F4_CSP_BMSK      0x8
#define DWC_TRB_F4_CSP_SHFT      3

#define DWC_TRB_F4_CHN_BMSK      0x4
#define DWC_TRB_F4_CHN_SHFT      2

#define DWC_TRB_F4_LST_BMSK      0x2
#define DWC_TRB_F4_LST_SHFT      1

#define DWC_TRB_F4_HWO_BMSK      0x1
#define DWC_TRB_F4_HWO_SHFT      0
/**************************** END - TRB ***************************************/

#define DWC_MAX_BYTES_PER_TRB  0x00FFFFFF

/********************* END: h/w defined values ********************************/



/******************** START: local data not needed by external APIs ***********/

/* event buffer: used to manage various controller events */
typedef struct {
   uint32_t *buf;       /* ptr to event buffer. */
   uint16_t  buf_size;  /* size of buf. */
   uint16_t  max_index; /* max index value. initialized once. used to track rollover. */
   uint16_t  index;     /* index into the buf for reading next event */
} dwc_event_buf_t;

/* device command */
typedef struct {
	uint32_t cmd;
	uint32_t param;
} dwc_device_cmd_t;

/* ep command */
typedef struct {
	uint32_t cmd;
	uint8_t  xfer_resource_index;
	uint32_t param2;
	uint32_t param1;
	uint32_t param0;
} dwc_ep_cmd_t;

/* state of data transfer on an ep */
typedef enum
{
	EP_STATE_INIT           = 0x0, /* initial state. cannot start transfer. */
	EP_STATE_INACTIVE       = 0x1, /* start xfer has not been issued. transfer is NOT in progress. start transfer can be issued ONLY in this state. */
	EP_STATE_START_TRANSFER = 0x2, /* start xfer is issued but cmd is not completed yet. */
	EP_STATE_XFER_IN_PROG   = 0x3, /* start xfer is issued and xfer is in progress. */
} dwc_ep_state_t;

/* control fsm states: states to manage control transfers */
typedef enum
{
	EP_FSM_INIT            = 0,
	EP_FSM_SETUP           = 1,
	EP_FSM_CTRL_DATA       = 2,
	EP_FSM_WAIT_FOR_HOST_2 = 3, /* 2-stage transfer wait-for-host stage */
	EP_FSM_WAIT_FOR_HOST_3 = 4, /* 3-stage transfer wait-for-host stage */
	EP_FSM_STATUS_2        = 5,
	EP_FSM_STATUS_3        = 6,
	EP_FSM_STALL           = 7,
} dwc_ctrl_fsm_t;

/* TRB type */
typedef enum
{
	TRBCTL_NORMAL            = 1,
	TRBCTL_CONTROL_SETUP     = 2,
	TRBCTL_CONTROL_STATUS_2  = 3,
	TRBCTL_CONTROL_STATUS_3  = 4,
	TRBCTL_CONTROL_DATA      = 5,
	TRBCTL_LINK_TRB          = 8,
} dwc_trb_trbctl_t;

/* data transfer request */
typedef struct
{
	uint8_t         *data;
	uint32_t         len;
	dwc_trb_trbctl_t trbctl;
	void            *context;
	void (*callback)(void *context, uint32_t actual, int status);
} dwc_request_t;

/******************** END: local data not needed by external APIs *************/


/******************** START: data needed by external APIs *********************/

/* TRB fields */
typedef struct
{
   uint32_t f1;
   uint32_t f2;
   uint32_t f3;
   uint32_t f4;
} dwc_trb_t;

/* index into the ep array of the dwc device */
#define DWC_EP_INDEX(_usb_ep, _direction) (((_usb_ep) << 1) | (_direction))

/* phyical ep number: same as ep index. */
#define DWC_EP_PHY_NUM(_usb_ep, _direction) (((_usb_ep) << 1) | (_direction))

/* since we assume non-flexible mapping, phy_num is same as index. */
#define DWC_EP_PHY_TO_INDEX(_ep_phy_num)  (_ep_phy_num)

typedef void (*dwc_transfer_callback_t)(void *context, uint32_t actual, int status);

/* length of zero-length-packet */
/* TODO: shouldn't this be same a max pkt size for the EP
 * which is specified by udc? fastboot doesn't need this.
 * So this is not verified.
 */
#define DWC_ZLP_BUF_SIZE    512

/* Structure to keep all information about an endpoint */
typedef struct
{
	uint8_t             number;        /* usb ep number */
	dwc_ep_direction_t  dir;           /* usb ep direction */
	dwc_ep_type_t       type;          /* ctrl/blk etc. */
	uint16_t            max_pkt_size;  /* max packet size */
	uint8_t             zlp;           /* uses zero length pkt to terminate xfer */
	uint32_t            burst_size;    /* max packets to transfer before waiting for ack */

	uint8_t             phy_num;       /* physical EP to which this usb ep is mapped */
	uint8_t             tx_fifo_num;   /* which TX FIFO to use. only applies to IN endpoints */

	uint8_t             zlp_buf[DWC_ZLP_BUF_SIZE];  /* buffer needed to pad when OUT requests are not exact multiple of max_pkt_size and for zlp */

	uint8_t             resource_idx;  /* assigned by h/w on each start xfer cmd. Needed to stop/update xfers. */

	dwc_trb_t          *trb;           /* ptr to the first TRB in the chain. */
	uint32_t            trb_count;     /* size of TRB chain. */
	uint32_t            trb_queued;    /* number of TRBs queued in the current request. */
	uint32_t            bytes_queued;  /* number of bytes queued in the current request. */
	dwc_request_t       req;           /* transfer request that is currently queued on this ep. */

	dwc_ep_state_t      state;         /* data transfer state of the ep. */

} dwc_ep_t;


/* dwc device events */
typedef enum
{
	DWC_NOTIFY_EVENT_OFFLINE,
	DWC_NOTIFY_EVENT_CONNECTED_LS,
	DWC_NOTIFY_EVENT_CONNECTED_FS,
	DWC_NOTIFY_EVENT_CONNECTED_HS,
	DWC_NOTIFY_EVENT_CONNECTED_SS,
	DWC_NOTIFY_EVENT_DISCONNECTED,
} dwc_notify_event_t;

/* maximum number of endpoints supported. */
#define DWC_MAX_NUM_OF_EP     4

/* length of setup packet */
#define DWC_SETUP_PKT_LEN    8

enum
{
	DWC_SETUP_ERROR    = -1,
	DWC_SETUP_2_STAGE  =  2,
	DWC_SETUP_3_STAGE  =  3,
};

/* Structure to keep all DWC device information. */
typedef struct
{
	void           *base;       /* base address for snps core registers */
	uint32_t        core_id;    /* snps core version. read from h/w during init */

	dwc_ep_t        ep[DWC_MAX_NUM_OF_EP]; /* array of endpoint data */
	dwc_event_buf_t event_buf;  /* event buffer management */
	dwc_ctrl_fsm_t  ctrl_state; /* states to manage control transfers : setup/data/wait/status */

	uint8_t         *setup_pkt; /* Buffer for the received setup packet */

	/* callback into client to notify device events: online/offline/connect speed */
	void *notify_context;
	void (*notify)(void *context, dwc_notify_event_t event);

	/* callback into client to process the setup msgs. */
	void *setup_context;
	int (*setup_handler)(void* context, uint8_t* data);

} dwc_dev_t;


/* config data to initialize dwc layer */
typedef struct
{
   void     *base;            /* dwc base address */
   uint32_t *event_buf;       /* buffer to be used for h/w events */
   uint16_t  event_buf_size;  /* buffer size */

   /* callback for dwc events */
   void *notify_context;
   void (*notify)(void *context, dwc_notify_event_t event);

   /* callback for handling setup packets */
   void *setup_context;
   int (*setup_handler)(void *context, uint8_t *data);

} dwc_config_t;

/********************************* dwc global apis ****************************/

/* generic apis */
dwc_dev_t* dwc_init(dwc_config_t *config);
void dwc_reset(dwc_dev_t *dev, uint8_t reset);

/* phy specific apis */
void dwc_phy_digital_reset(dwc_dev_t *dev);
void dwc_usb2_phy_soft_reset(dwc_dev_t *dev);
void dwc_ss_phy_workaround_12(dwc_dev_t *dev);

/* device specific apis */
void dwc_device_init(dwc_dev_t *dev);
void dwc_device_reset(dwc_dev_t *dev);
void dwc_device_run(dwc_dev_t *dev, uint8_t run);
void dwc_device_set_addr(dwc_dev_t *dev, uint16_t addr);
void dwc_device_set_configuration(dwc_dev_t *dev);
void dwc_device_set_periodic_param(dwc_dev_t *dev, uint32_t val);
void dwc_device_add_ep(dwc_dev_t *dev, dwc_ep_t *new_ep);

/* data transfer apis */
int dwc_transfer_request(dwc_dev_t *dwc,
						 uint8_t usb_ep,
						 dwc_ep_direction_t dir,
						 void *buf,
						 uint32_t len,
						 dwc_transfer_callback_t callback,
						 void *callback_context);

/******************** END: data needed by external APIs *********************/
/* static apis */

/* command complete event handler */
static void dwc_event_handle_cmd_complete(dwc_dev_t *dev, uint32_t *event);

/* device event handler */
static void dwc_event_handler_device(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_device_link_status_change(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_device_reset(dwc_dev_t *dev);
static void dwc_event_device_connect_done(dwc_dev_t *dev);
static void dwc_event_device_disconnect(dwc_dev_t *dev);

/* bulk ep event handling functions */
static void dwc_event_handler_ep_bulk(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_handler_ep_bulk_state_inactive(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_handler_ep_bulk_state_xfer_in_prog(dwc_dev_t *dev, uint32_t *event);
static void dwc_ep_bulk_state_inactive_enter(dwc_dev_t *dev, uint8_t ep_phy_num);

/* control ep event handling functions */
static void dwc_event_handler_ep_ctrl(dwc_dev_t *dev, uint32_t *event);

static void dwc_ep_ctrl_state_setup_enter(dwc_dev_t *dev);
static void dwc_event_handler_ep_ctrl_state_setup(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_handler_ep_ctrl_state_data(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_handler_ep_ctrl_state_wait_for_host_2(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_handler_ep_ctrl_state_wait_for_host_3(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_handler_ep_ctrl_state_status_2(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_handler_ep_ctrl_state_status_3(dwc_dev_t *dev, uint32_t *event);
static void dwc_event_handler_ep_ctrl_state_stall(dwc_dev_t *dev, uint32_t *event);

static enum handler_return dwc_irq_handler_ee1(void* arg);
static void dwc_ep_config_init_enable(dwc_dev_t *dev, uint8_t index);

static int dwc_request_queue(dwc_dev_t *dev, uint8_t ep_phy_num, dwc_request_t *req);
#endif
