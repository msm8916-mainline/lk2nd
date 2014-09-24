/* Copyright (c) 2014, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include <bits.h>
#include <debug.h>
#include <reg.h>

/* base addresses */
#define QPNP_WLED_CTRL_BASE                    0xd800
#define QPNP_WLED_SINK_BASE                    0xd900
#define QPNP_WLED_IBB_BASE                     0xdc00
#define QPNP_WLED_LAB_BASE                     0xde00

/* ctrl registers */
#define QPNP_WLED_EN_REG(b)                    (b + 0x46)
#define QPNP_WLED_FDBK_OP_REG(b)               (b + 0x48)
#define QPNP_WLED_VREF_REG(b)                  (b + 0x49)
#define QPNP_WLED_BOOST_DUTY_REG(b)            (b + 0x4B)
#define QPNP_WLED_SWITCH_FREQ_REG(b)           (b + 0x4C)
#define QPNP_WLED_OVP_REG(b)                   (b + 0x4D)
#define QPNP_WLED_ILIM_REG(b)                  (b + 0x4E)

#define QPNP_WLED_EN_MASK                      0x7F
#define QPNP_WLED_EN_SHIFT                     7
#define QPNP_WLED_FDBK_OP_MASK                 0xF8
#define QPNP_WLED_VREF_MASK                    0xF0
#define QPNP_WLED_VREF_STEP_MV                 25
#define QPNP_WLED_VREF_MIN_MV                  300
#define QPNP_WLED_VREF_MAX_MV                  675
#define QPNP_WLED_DFLT_VREF_MV                 350
#define QPNP_WLED_ILIM_MASK                    0xF8
#define QPNP_WLED_ILIM_MIN_MA                  105
#define QPNP_WLED_ILIM_MAX_MA                  1980
#define QPNP_WLED_ILIM_STEP_MA                 280
#define QPNP_WLED_DFLT_ILIM_MA                 980
#define QPNP_WLED_BOOST_DUTY_MASK              0xFC
#define QPNP_WLED_BOOST_DUTY_STEP_NS           52
#define QPNP_WLED_BOOST_DUTY_MIN_NS            26
#define QPNP_WLED_BOOST_DUTY_MAX_NS            156
#define QPNP_WLED_DEF_BOOST_DUTY_NS            104
#define QPNP_WLED_DFLT_HYB_THRES               625
#define QPNP_WLED_SWITCH_FREQ_MASK             0xF0
#define QPNP_WLED_SWITCH_FREQ_800_KHZ          800
#define QPNP_WLED_SWITCH_FREQ_1600_KHZ         1600
#define QPNP_WLED_OVP_MASK                     0xFC
#define QPNP_WLED_OVP_17800_MV                 17800
#define QPNP_WLED_OVP_19400_MV                 19400
#define QPNP_WLED_OVP_29500_MV                 29500
#define QPNP_WLED_OVP_31000_MV                 31000

/* sink registers */
#define QPNP_WLED_CURR_SINK_REG(b)             (b + 0x46)
#define QPNP_WLED_SYNC_REG(b)                  (b + 0x47)
#define QPNP_WLED_MOD_REG(b)                   (b + 0x4A)
#define QPNP_WLED_HYB_THRES_REG(b)             (b + 0x4B)
#define QPNP_WLED_MOD_EN_REG(b, n)             (b + 0x50 + (n * 0x10))
#define QPNP_WLED_SYNC_DLY_REG(b, n)           (QPNP_WLED_MOD_EN_REG(b, n) + 0x01)
#define QPNP_WLED_FS_CURR_REG(b, n)            (QPNP_WLED_MOD_EN_REG(b, n) + 0x02)
#define QPNP_WLED_CABC_REG(b, n)               (QPNP_WLED_MOD_EN_REG(b, n) + 0x06)
#define QPNP_WLED_BRIGHT_LSB_REG(b, n)         (QPNP_WLED_MOD_EN_REG(b, n) + 0x07)
#define QPNP_WLED_BRIGHT_MSB_REG(b, n)         (QPNP_WLED_MOD_EN_REG(b, n) + 0x08)

#define QPNP_WLED_MOD_FREQ_1200_KHZ            1200
#define QPNP_WLED_MOD_FREQ_2400_KHZ            2400
#define QPNP_WLED_MOD_FREQ_9600_KHZ            9600
#define QPNP_WLED_MOD_FREQ_19200_KHZ           19200
#define QPNP_WLED_MOD_FREQ_MASK                0x3F
#define QPNP_WLED_MOD_FREQ_SHIFT               6
#define QPNP_WLED_PHASE_STAG_MASK              0xDF
#define QPNP_WLED_PHASE_STAG_SHIFT             5
#define QPNP_WLED_DIM_RES_MASK                 0xFD
#define QPNP_WLED_DIM_RES_SHIFT                1
#define QPNP_WLED_DIM_HYB_MASK                 0xFB
#define QPNP_WLED_DIM_HYB_SHIFT                2
#define QPNP_WLED_DIM_ANA_MASK                 0xFE
#define QPNP_WLED_HYB_THRES_MASK               0xF8
#define QPNP_WLED_HYB_THRES_MIN                78
#define QPNP_WLED_DEF_HYB_THRES                625
#define QPNP_WLED_HYB_THRES_MAX                10000
#define QPNP_WLED_MOD_EN_MASK                  0x7F
#define QPNP_WLED_MOD_EN_SHFT                  7
#define QPNP_WLED_MOD_EN                       1
#define QPNP_WLED_SYNC_DLY_MASK                0xF8
#define QPNP_WLED_SYNC_DLY_MIN_US              0
#define QPNP_WLED_SYNC_DLY_MAX_US              1400
#define QPNP_WLED_SYNC_DLY_STEP_US             200
#define QPNP_WLED_DEF_SYNC_DLY_US              400
#define QPNP_WLED_FS_CURR_MASK                 0xF0
#define QPNP_WLED_FS_CURR_MIN_UA               0
#define QPNP_WLED_FS_CURR_MAX_UA               30000
#define QPNP_WLED_FS_CURR_STEP_UA              2500
#define QPNP_WLED_CABC_MASK                    0x7F
#define QPNP_WLED_CABC_SHIFT                   7
#define QPNP_WLED_CURR_SINK_SHIFT              4
#define QPNP_WLED_BRIGHT_LSB_MASK              0xFF
#define QPNP_WLED_BRIGHT_MSB_SHIFT             8
#define QPNP_WLED_BRIGHT_MSB_MASK              0x0F
#define QPNP_WLED_SYNC                         0x0F
#define QPNP_WLED_SYNC_RESET                   0x00

#define QPNP_WLED_SWITCH_FREQ_800_KHZ_CODE     0x0B
#define QPNP_WLED_SWITCH_FREQ_1600_KHZ_CODE    0x05

#define QPNP_WLED_DISP_SEL_REG(b)              (b + 0x44)
#define QPNP_WLED_MODULE_RDY_REG(b)            (b + 0x45)
#define QPNP_WLED_MODULE_EN_REG(b)             (b + 0x46)
#define QPNP_WLED_MODULE_RDY_MASK              0x7F
#define QPNP_WLED_MODULE_RDY_SHIFT             7
#define QPNP_WLED_MODULE_EN_MASK               0x7F
#define QPNP_WLED_MODULE_EN_SHIFT              7
#define QPNP_WLED_DISP_SEL_MASK                0x7F
#define QPNP_WLED_DISP_SEL_SHIFT               7

#define QPNP_WLED_IBB_BIAS_REG(b)              (b + 0x58)
#define QPNP_WLED_IBB_BIAS_MASK                0x7F
#define QPNP_WLED_IBB_BIAS_SHIFT               7
#define QPNP_WLED_IBB_PWRUP_DLY_MASK           0xCF
#define QPNP_WLED_IBB_PWRUP_DLY_SHIFT          4
#define QPNP_WLED_IBB_PWRUP_DLY_MIN_MS         1
#define QPNP_WLED_IBB_PWRUP_DLY_MAX_MS         8

#define QPNP_WLED_LAB_IBB_RDY_REG(b)           (b + 0x49)
#define QPNP_WLED_LAB_FAST_PC_REG(b)           (b + 0x5E)
#define QPNP_WLED_LAB_FAST_PC_MASK             0xFB
#define QPNP_WLED_LAB_START_DLY_US             8
#define QPNP_WLED_LAB_FAST_PC_SHIFT            2

#define QPNP_WLED_SEC_ACCESS_REG(b)            (b + 0xD0)
#define QPNP_WLED_SEC_UNLOCK                   0xA5

#define QPNP_WLED_MAX_STRINGS                  4
#define WLED_MAX_LEVEL_511                     511
#define WLED_MAX_LEVEL_4095                    4095
#define QPNP_WLED_RAMP_DLY_MS                  20
#define QPNP_WLED_TRIGGER_NONE                 "none"
#define QPNP_WLED_STR_SIZE                     20
#define QPNP_WLED_MIN_MSLEEP                   20
#define QPNP_WLED_MAX_BR_LEVEL                 1638

/* output feedback mode */
enum qpnp_wled_fdbk_op {
       QPNP_WLED_FDBK_AUTO,
       QPNP_WLED_FDBK_WLED1,
       QPNP_WLED_FDBK_WLED2,
       QPNP_WLED_FDBK_WLED3,
       QPNP_WLED_FDBK_WLED4,
};

/* dimming modes */
enum qpnp_wled_dim_mode {
       QPNP_WLED_DIM_ANALOG,
       QPNP_WLED_DIM_DIGITAL,
       QPNP_WLED_DIM_HYBRID,
};

/* dimming curve shapes */
enum qpnp_wled_dim_shape {
       QPNP_WLED_DIM_SHAPE_LOG,
       QPNP_WLED_DIM_SHAPE_LINEAR,
       QPNP_WLED_DIM_SHAPE_SQUARE,
};


/**
 *  qpnp_wled - wed data structure
 *  @ fdbk_op - output feedback mode
 *  @ dim_mode - dimming mode
 *  @ dim_shape - dimming curve shape
 *  @ ctrl_base - base address for wled ctrl
 *  @ sink_base - base address for wled sink
 *  @ ibb_base - base address for IBB(Inverting Buck Boost)
 *  @ lab_base - base address for LAB(LCD/AMOLED Boost)
 *  @ mod_freq_khz - modulator frequency in KHZ
 *  @ hyb_thres - threshold for hybrid dimming
 *  @ sync_dly_us - sync delay in us
 *  @ vref_mv - ref voltage in mv
 *  @ switch_freq_khz - switching frequency in KHZ
 *  @ ovp_mv - over voltage protection in mv
 *  @ ilim_ma - current limiter in ma
 *  @ boost_duty_ns - boost duty cycle in ns
 *  @ fs_curr_ua - full scale current in ua
 *  @ ramp_ms - delay between ramp steps in ms
 *  @ ramp_step - ramp step size
 *  @ strings - supported list of strings
 *  @ num_strings - number of strings
 *  @ en_9b_dim_res - enable or disable 9bit dimming
 *  @ en_phase_stag - enable or disable phase staggering
 *  @ en_cabc - enable or disable cabc
 *  @ disp_type_amoled - type of display: LCD/AMOLED
 *  @ ibb_bias_active - activate display bias
 *  @ lab_fast_precharge - fast/slow precharge
 */
struct qpnp_wled {
	enum qpnp_wled_fdbk_op fdbk_op;
	enum qpnp_wled_dim_mode dim_mode;
	enum qpnp_wled_dim_shape dim_shape;
	uint16_t ctrl_base;
	uint16_t sink_base;
	uint16_t ibb_base;
	uint16_t lab_base;
	uint16_t mod_freq_khz;
	uint16_t hyb_thres;
	uint16_t sync_dly_us;
	uint16_t vref_mv;
	uint16_t switch_freq_khz;
	uint16_t ovp_mv;
	uint16_t ilim_ma;
	uint16_t boost_duty_ns;
	uint16_t fs_curr_ua;
	uint16_t ibb_pwrup_dly_ms;
	uint16_t ramp_ms;
	uint16_t ramp_step;
	uint8_t strings[QPNP_WLED_MAX_STRINGS];
	uint8_t num_strings;
	bool en_9b_dim_res;
	bool en_phase_stag;
	bool en_cabc;
	bool disp_type_amoled;
	bool ibb_bias_active;
	bool lab_fast_precharge;
};

/* WLED Initial Setup */
int qpnp_wled_init();

/* Enable IBB */
int qpnp_ibb_enable();
void qpnp_wled_enable_backlight(int enable);
