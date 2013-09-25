/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#ifndef EDP_H
#define EDP_H

#include <reg.h>
#include <debug.h>
#include <err.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <platform/timer.h>

#include "msm_panel.h"

#define edp_read(offset) readl_relaxed((offset))
#define edp_write(offset, data) writel_relaxed((data), (offset))

#define EDP_MAX_LANE            4

#define AUX_CMD_FIFO_LEN        144
#define AUX_CMD_MAX             16
#define AUX_CMD_I2C_MAX         128

#define EDP_PORT_MAX            1
#define EDP_SINK_CAP_LEN        16

#define EDP_AUX_ERR_NONE        0
#define EDP_AUX_ERR_ADDR        -1
#define EDP_AUX_ERR_TOUT        -2
#define EDP_AUX_ERR_NACK        -3

/* 4 bits of aux command */
#define EDP_CMD_AUX_WRITE       0x8
#define EDP_CMD_AUX_READ        0x9

/* 4 bits of i2c command */
#define EDP_CMD_I2C_MOT         0x4     /* i2c middle of transaction */
#define EDP_CMD_I2C_WRITE       0x0
#define EDP_CMD_I2C_READ        0x1
#define EDP_CMD_I2C_STATUS      0x2     /* i2c write status request */

/* cmd reply: bit 0, 1 for aux */
#define EDP_AUX_ACK             0x0
#define EDP_AUX_NACK    0x1
#define EDP_AUX_DEFER   0x2

/* cmd reply: bit 2, 3 for i2c */
#define EDP_I2C_ACK             0x0
#define EDP_I2C_NACK    0x4
#define EDP_I2C_DEFER   0x8

#define EDP_CMD_TIMEOUT 400     /* us */
#define EDP_CMD_LEN             16


/* isr */
#define EDP_INTR_HPD            BIT(0)
#define EDP_INTR_AUX_I2C_DONE   BIT(3)
#define EDP_INTR_WRONG_ADDR     BIT(6)
#define EDP_INTR_TIMEOUT        BIT(9)
#define EDP_INTR_NACK_DEFER     BIT(12)
#define EDP_INTR_WRONG_DATA_CNT BIT(15)
#define EDP_INTR_I2C_NACK       BIT(18)
#define EDP_INTR_I2C_DEFER      BIT(21)
#define EDP_INTR_PLL_UNLOCKED   BIT(24)
#define EDP_INTR_AUX_ERROR      BIT(27)


#define EDP_INTR_STATUS1 \
        (EDP_INTR_HPD | EDP_INTR_AUX_I2C_DONE| \
        EDP_INTR_WRONG_ADDR | EDP_INTR_TIMEOUT | \
        EDP_INTR_NACK_DEFER | EDP_INTR_WRONG_DATA_CNT | \
        EDP_INTR_I2C_NACK | EDP_INTR_I2C_DEFER | \
        EDP_INTR_PLL_UNLOCKED | EDP_INTR_AUX_ERROR)

#define EDP_INTR_MASK1          (EDP_INTR_STATUS1 << 2)


#define EDP_INTR_READY_FOR_VIDEO        BIT(0)
#define EDP_INTR_IDLE_PATTERNs_SENT     BIT(3)
#define EDP_INTR_FRAME_END              BIT(6)
#define EDP_INTR_CRC_UPDATED            BIT(9)

#define EDP_INTR_STATUS2 \
        (EDP_INTR_READY_FOR_VIDEO | EDP_INTR_IDLE_PATTERNs_SENT | \
        EDP_INTR_FRAME_END | EDP_INTR_CRC_UPDATED)

#define EDP_INTR_MASK2          (EDP_INTR_STATUS2 << 2)





#define EDP_MAINLINK_CTRL       0x004
#define EDP_STATE_CTRL          0x008
#define EDP_MAINLINK_READY      0x084

#define EDP_AUX_CTRL            0x300
#define EDP_INTERRUPT_STATUS    0x308
#define EDP_INTERRUPT_STATUS_2  0x30c
#define EDP_AUX_DATA            0x314
#define EDP_AUX_TRANS_CTRL      0x318
#define EDP_AUX_STATUS          0x324

#define EDP_PHY_EDPPHY_GLB_VM_CFG0      0x510
#define EDP_PHY_EDPPHY_GLB_VM_CFG1      0x514

struct edp_cmd{
        char read;      /* 1 == read, 0 == write */
        char i2c;       /* 1 == i2c cmd, 0 == native cmd */
        int addr;       /* 20 bits */
        char *datap;
        int len;        /* len to be tx OR len to be rx for read */
        char next;      /* next command */
};

struct edp_buf {
        char *start;    /* buffer start addr */
        char *end;      /* buffer end addr */
        int size;       /* size of buffer */
        char *data;     /* data pointer */
        int len;        /* dara length */
        char trans_num; /* transaction number */
        char i2c;       /* 1 == i2c cmd, 0 == native cmd */
};

#define DPCD_ENHANCED_FRAME     BIT(0)
#define DPCD_TPS3       BIT(1)
#define DPCD_MAX_DOWNSPREAD_0_5 BIT(2)
#define DPCD_NO_AUX_HANDSHAKE   BIT(3)
#define DPCD_PORT_0_EDID_PRESENTED      BIT(4)



#define DPCD_LINK_VOLTAGE_MAX   4
#define DPCD_LINK_PRE_EMPHASIS_MAX      4

struct dpcd_cap {
        char major;
        char minor;
        char max_lane_count;
        char num_rx_port;
        char i2c_speed_ctrl;
        char scrambler_reset;
        char enhanced_frame;
        int max_link_rate;  /* 162, 270 and 540 Mb, divided by 10 */
        int flags;
        int rx_port0_buf_size;
        int training_read_interval;/* us */
};


struct display_timing_desc {
	uint32_t pclk;
	uint32_t h_addressable; /* addressable + boder = active */
	uint32_t h_border;
	uint32_t h_blank;	/* fporch + bporch + sync_pulse = blank */
	uint32_t h_fporch;
	uint32_t h_sync_pulse;
	uint32_t v_addressable; /* addressable + boder = active */
	uint32_t v_border;
	uint32_t v_blank;	/* fporch + bporch + sync_pulse = blank */
	uint32_t v_fporch;
	uint32_t v_sync_pulse;
	uint32_t width_mm;
	uint32_t height_mm;
	uint32_t interlaced;
	uint32_t stereo;
	uint32_t sync_type;
	uint32_t sync_separate;
	uint32_t vsync_pol;
	uint32_t hsync_pol;
};


struct edp_edid {
        char id_name[4];
        short id_product;
        char version;
        char revision;
        char video_intf;        /* edp == 0x5 */
        char color_depth;       /* 6, 8, 10, 12 and 14 bits */
        char color_format;      /* RGB 4:4:4, YCrCb 4:4:4, Ycrcb 4:2:2 */
        char dpm;               /* display power management */
        char sync_digital;      /* 1 = digital */
        char sync_separate;     /* 1 = separate */
        char vsync_pol;         /* 0 = negative, 1 = positive */
        char hsync_pol;         /* 0 = negative, 1 = positive */
        char ext_block_cnt;
        struct display_timing_desc timing[4];
};

struct dpcd_link_status {
        char lane_01_status;
        char lane_23_status;
        char interlane_align_done;
        char downstream_port_status_changed;
        char link_status_updated;
        char port_0_in_sync;
        char port_1_in_sync;
        char req_voltage_swing[4];
        char req_pre_emphasis[4];
};

struct edp_aux_ctrl {
	int aux_cmd_busy;
        int aux_cmd_i2c;
        int aux_trans_num;
        int aux_error_num;
        int aux_ctrl_reg;
        struct edp_buf txp;
        struct edp_buf rxp;
        char txbuf[256];
        char rxbuf[256];
        struct dpcd_link_status link_status;
        char link_rate;
        char lane_cnt;
        char v_level;
        char p_level;

	/* transfer unit */
        char tu_desired;
        char valid_boundary;
        char delay_start;
        int bpp;

        struct edp_edid edid;
        struct dpcd_cap dpcd;
};


void edp_phy_pll_reset(void);
void edp_mainlink_reset(void);
void edp_aux_reset(void);
void edp_phy_powerup(int enable);
void edp_lane_power_ctrl(int max_lane, int up);

void edp_phy_sw_reset(void);
void edp_pll_configure(unsigned int rate);
void edp_enable_lane_bist(int lane, int enable);
void edp_enable_mainlink(int enable);
void edp_hw_powerup(int enable);
void edp_config_clk(void);
void edp_unconfig_clk(void);
void edp_phy_misc_cfg(void);
int edp_on(void);
int edp_off(void);
int edp_config(void *pdata);
void mdss_edp_dpcd_cap_read(void);
void mdss_edp_dpcd_status_read(void);
void mdss_edp_edid_read(void);
int mdss_edp_link_train(void);
void mdss_edp_aux_init(void);
void mdss_edp_irq_enable(void);
void mdss_edp_irq_disable(void);
void mdss_edp_wait_for_hpd(void);
void mdss_edp_wait_for_video_ready(void);
void mdss_edp_lane_power_ctrl(int up);
int mdss_edp_phy_pll_ready(void);
void mdss_edp_pll_configure(void);
void edp_cap2pinfo(struct msm_panel_info *pinfo);
void edp_edid2pinfo(struct msm_panel_info *pinfo);
int edp_aux_write_cmds(struct edp_aux_ctrl *ep,
				struct edp_cmd *cmd);
int edp_aux_read_cmds(struct edp_aux_ctrl *ep,
				struct edp_cmd *cmds);
int edp_aux_write_buf(struct edp_aux_ctrl *ep, int addr,
			char *buf, int len, int i2c);
int edp_aux_read_buf(struct edp_aux_ctrl *ep, int addr,
				int len, int i2c);
char *edp_buf_init(struct edp_buf *eb, char *buf, int size);

#endif /* EDP_H */
