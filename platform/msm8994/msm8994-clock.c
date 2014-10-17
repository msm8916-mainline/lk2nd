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

#include <assert.h>
#include <reg.h>
#include <err.h>
#include <clock.h>
#include <clock_pll.h>
#include <clock_lib2.h>
#include <platform/clock.h>
#include <platform/iomap.h>


/* Mux source select values */
#define cxo_source_val    0
#define gpll0_source_val  1
#define gpll4_source_val  5
#define cxo_mm_source_val 0
#define mmpll0_mm_source_val 1
#define mmpll1_mm_source_val 2
#define mmpll3_mm_source_val 3
#define gpll0_mm_source_val 5
#define edppll_270_mm_source_val 4
#define edppll_350_mm_source_val 4

struct clk_freq_tbl rcg_dummy_freq = F_END;


/* Clock Operations */
static struct clk_ops clk_ops_rst =
{
	.reset     = clock_lib2_reset_clk_reset,
};

static struct clk_ops clk_ops_branch =
{
	.enable     = clock_lib2_branch_clk_enable,
	.disable    = clock_lib2_branch_clk_disable,
	.set_rate   = clock_lib2_branch_set_rate,
	.reset      = clock_lib2_branch_clk_reset,
};

static struct clk_ops clk_ops_rcg_mnd =
{
	.enable     = clock_lib2_rcg_enable,
	.set_rate   = clock_lib2_rcg_set_rate,
};

static struct clk_ops clk_ops_rcg =
{
	.enable     = clock_lib2_rcg_enable,
	.set_rate   = clock_lib2_rcg_set_rate,
};

static struct clk_ops clk_ops_cxo =
{
	.enable     = cxo_clk_enable,
	.disable    = cxo_clk_disable,
};

static struct clk_ops clk_ops_pll_vote =
{
	.enable     = pll_vote_clk_enable,
	.disable    = pll_vote_clk_disable,
	.auto_off   = pll_vote_clk_disable,
	.is_enabled = pll_vote_clk_is_enabled,
};

static struct clk_ops clk_ops_vote =
{
	.enable     = clock_lib2_vote_clk_enable,
	.disable    = clock_lib2_vote_clk_disable,
};

/* Clock Sources */
static struct fixed_clk cxo_clk_src =
{
	.c = {
		.rate     = 19200000,
		.dbg_name = "cxo_clk_src",
		.ops      = &clk_ops_cxo,
	},
};

static struct pll_vote_clk gpll0_clk_src =
{
	.en_reg       = (void *) APCS_GPLL_ENA_VOTE,
	.en_mask      = BIT(0),
	.status_reg     = (void *) GPLL0_MODE,
	.status_mask    = BIT(30),
	.parent       = &cxo_clk_src.c,

	.c = {
		.rate     = 600000000,
		.dbg_name = "gpll0_clk_src",
		.ops      = &clk_ops_pll_vote,
	},
};

static struct pll_vote_clk gpll4_clk_src =
{
	.en_reg       = (void *) APCS_GPLL_ENA_VOTE,
	.en_mask      = BIT(4),
	.status_reg   = (void *) GPLL4_MODE,
	.status_mask  = BIT(30),
	.parent       = &cxo_clk_src.c,

	.c = {
		.rate     = 1600000000,
		.dbg_name = "gpll4_clk_src",
		.ops      = &clk_ops_pll_vote,
	},
};

/* UART Clocks */
static struct clk_freq_tbl ftbl_gcc_blsp1_2_uart1_6_apps_clk[] =
{
	F( 3686400,  gpll0,    1,  96,  15625),
	F( 7372800,  gpll0,    1, 192,  15625),
	F(14745600,  gpll0,    1, 384,  15625),
	F(16000000,  gpll0,    5,   2,     15),
	F(19200000,    cxo,    1,   0,      0),
	F(24000000,  gpll0,    5,   1,      5),
	F(32000000,  gpll0,    1,   4,     75),
	F(40000000,  gpll0,   15,   0,      0),
	F(46400000,  gpll0,    1,  29,    375),
	F(48000000,  gpll0, 12.5,   0,      0),
	F(51200000,  gpll0,    1,  32,    375),
	F(56000000,  gpll0,    1,   7,     75),
	F(58982400,  gpll0,    1, 1536, 15625),
	F(60000000,  gpll0,   10,   0,      0),
	F(63160000,  gpll0,  9.5,   0,      0),
	F_END
};

static struct rcg_clk blsp2_uart2_apps_clk_src =
{
	.cmd_reg      = (uint32_t *) BLSP2_UART2_APPS_CMD_RCGR,
	.cfg_reg      = (uint32_t *) BLSP2_UART2_APPS_CFG_RCGR,
	.m_reg        = (uint32_t *) BLSP2_UART2_APPS_M,
	.n_reg        = (uint32_t *) BLSP2_UART2_APPS_N,
	.d_reg        = (uint32_t *) BLSP2_UART2_APPS_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_blsp1_2_uart1_6_apps_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "blsp1_uart2_apps_clk",
		.ops      = &clk_ops_rcg_mnd,
	},
};

static struct rcg_clk blsp1_uart2_apps_clk_src =
{
	.cmd_reg      = (uint32_t *) BLSP1_UART2_APPS_CMD_RCGR,
	.cfg_reg      = (uint32_t *) BLSP1_UART2_APPS_CFG_RCGR,
	.m_reg        = (uint32_t *) BLSP1_UART2_APPS_M,
	.n_reg        = (uint32_t *) BLSP1_UART2_APPS_N,
	.d_reg        = (uint32_t *) BLSP1_UART2_APPS_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_blsp1_2_uart1_6_apps_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "blsp1_uart2_apps_clk",
		.ops      = &clk_ops_rcg_mnd,
	},
};

static struct branch_clk gcc_blsp2_uart2_apps_clk =
{
	.cbcr_reg     = (uint32_t *) BLSP2_UART2_APPS_CBCR,
	.parent       = &blsp2_uart2_apps_clk_src.c,

	.c = {
		.dbg_name = "gcc_blsp2_uart2_apps_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_blsp1_uart2_apps_clk =
{
	.cbcr_reg     = (uint32_t *) BLSP1_UART2_APPS_CBCR,
	.parent       = &blsp1_uart2_apps_clk_src.c,

	.c = {
		.dbg_name = "gcc_blsp1_uart2_apps_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct vote_clk gcc_blsp1_ahb_clk = {
	.cbcr_reg     = (uint32_t *) BLSP1_AHB_CBCR,
	.vote_reg     = (uint32_t *) APCS_CLOCK_BRANCH_ENA_VOTE,
	.en_mask      = BIT(17),

	.c = {
		.dbg_name = "gcc_blsp1_ahb_clk",
		.ops      = &clk_ops_vote,
	},
};

static struct vote_clk gcc_blsp2_ahb_clk = {
	.cbcr_reg     = (uint32_t *) BLSP2_AHB_CBCR,
	.vote_reg     = (uint32_t *) APCS_CLOCK_BRANCH_ENA_VOTE,
	.en_mask      = BIT(15),

	.c = {
		.dbg_name = "gcc_blsp2_ahb_clk",
		.ops      = &clk_ops_vote,
	},
};

/* USB Clocks */
static struct clk_freq_tbl ftbl_gcc_usb_hs_system_clk[] =
{
	F(75000000,  gpll0,   8,   0,   0),
	F_END
};

static struct rcg_clk usb_hs_system_clk_src =
{
	.cmd_reg      = (uint32_t *) USB_HS_SYSTEM_CMD_RCGR,
	.cfg_reg      = (uint32_t *) USB_HS_SYSTEM_CFG_RCGR,

	.set_rate     = clock_lib2_rcg_set_rate_hid,
	.freq_tbl     = ftbl_gcc_usb_hs_system_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "usb_hs_system_clk",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk gcc_usb_hs_system_clk =
{
	.cbcr_reg     = (uint32_t *) USB_HS_SYSTEM_CBCR,
	.parent       = &usb_hs_system_clk_src.c,

	.c = {
		.dbg_name = "gcc_usb_hs_system_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_usb_hs_ahb_clk =
{
	.cbcr_reg     = (uint32_t *) USB_HS_AHB_CBCR,
	.has_sibling  = 1,

	.c = {
		.dbg_name = "gcc_usb_hs_ahb_clk",
		.ops      = &clk_ops_branch,
	},
};

/* SDCC Clocks */
static struct clk_freq_tbl ftbl_gcc_sdcc1_apps_clk[] =
{
	F(   144000,    cxo,  16,   3,  25),
	F(   400000,    cxo,  12,   1,   4),
	F( 20000000,  gpll0,  15,   1,   2),
	F( 25000000,  gpll0,  12,   1,   2),
	F( 50000000,  gpll0,  12,   0,   0),
	F( 96000000,  gpll4,   6,   0,   0),
	F(192000000,  gpll4,   2,   0,   0),
	F(384000000,  gpll4,   1,   0,   0),
	F_END
};

static struct clk_freq_tbl ftbl_gcc_sdcc2_4_apps_clk[] =
{
	F(   144000,    cxo,  16,   3,  25),
	F(   400000,    cxo,  12,   1,   4),
	F( 20000000,  gpll0,  15,   1,   2),
	F( 25000000,  gpll0,  12,   1,   2),
	F( 50000000,  gpll0,  12,   0,   0),
	F(100000000,  gpll0,   6,   0,   0),
	F(200000000,  gpll0,   3,   0,   0),
	F_END
};

static struct rcg_clk sdcc1_apps_clk_src =
{
	.cmd_reg      = (uint32_t *) SDCC1_CMD_RCGR,
	.cfg_reg      = (uint32_t *) SDCC1_CFG_RCGR,
	.m_reg        = (uint32_t *) SDCC1_M,
	.n_reg        = (uint32_t *) SDCC1_N,
	.d_reg        = (uint32_t *) SDCC1_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_sdcc1_apps_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "sdc1_clk",
		.ops      = &clk_ops_rcg_mnd,
	},
};

static struct branch_clk gcc_sdcc1_apps_clk =
{
	.cbcr_reg     = (uint32_t *) SDCC1_APPS_CBCR,
	.parent       = &sdcc1_apps_clk_src.c,

	.c = {
		.dbg_name = "gcc_sdcc1_apps_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_sdcc1_ahb_clk =
{
	.cbcr_reg     = (uint32_t *) SDCC1_AHB_CBCR,
	.has_sibling  = 1,

	.c = {
		.dbg_name = "gcc_sdcc1_ahb_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct rcg_clk sdcc2_apps_clk_src =
{
	.cmd_reg      = (uint32_t *) SDCC2_CMD_RCGR,
	.cfg_reg      = (uint32_t *) SDCC2_CFG_RCGR,
	.m_reg        = (uint32_t *) SDCC2_M,
	.n_reg        = (uint32_t *) SDCC2_N,
	.d_reg        = (uint32_t *) SDCC2_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_sdcc2_4_apps_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "sdc2_clk",
		.ops      = &clk_ops_rcg_mnd,
	},
};

static struct branch_clk gcc_sdcc2_apps_clk =
{
	.cbcr_reg     = (uint32_t *) SDCC2_APPS_CBCR,
	.parent       = &sdcc2_apps_clk_src.c,

	.c = {
		.dbg_name = "gcc_sdcc2_apps_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_sdcc2_ahb_clk =
{
	.cbcr_reg     = (uint32_t *) SDCC2_AHB_CBCR,
	.has_sibling  = 1,

	.c = {
		.dbg_name = "gcc_sdcc2_ahb_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_sys_noc_usb30_axi_clk = {
	.cbcr_reg    = (uint32_t *) SYS_NOC_USB3_AXI_CBCR,
	.has_sibling = 1,

	.c = {
		.dbg_name = "sys_noc_usb30_axi_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_usb2b_phy_sleep_clk = {
	.cbcr_reg    = (uint32_t *) USB2B_PHY_SLEEP_CBCR,
	.bcr_reg     = (uint32_t *) USB2B_PHY_BCR,
	.has_sibling = 1,

	.c = {
		.dbg_name = "usb2b_phy_sleep_clk",
		.ops      = &clk_ops_branch,
    },
};

static struct clk_freq_tbl ftbl_gcc_usb30_master_clk[] = {
	F( 125000000, gpll0,    1,    5,    24),
	F_END
};

static struct rcg_clk usb30_master_clk_src = {
	.cmd_reg      = (uint32_t *) USB30_MASTER_CMD_RCGR,
	.cfg_reg      = (uint32_t *) USB30_MASTER_CFG_RCGR,
	.m_reg        = (uint32_t *) USB30_MASTER_M,
	.n_reg        = (uint32_t *) USB30_MASTER_N,
	.d_reg        = (uint32_t *) USB30_MASTER_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_usb30_master_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "usb30_master_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk gcc_usb30_master_clk = {
	.cbcr_reg = (uint32_t *) USB30_MASTER_CBCR,
	.bcr_reg  = (uint32_t *) USB_30_BCR,
	.parent   = &usb30_master_clk_src.c,

	.c = {
		.dbg_name = "usb30_master_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct clk_freq_tbl ftbl_gcc_usb30_mock_utmi_clk_src[] = {
	F(  60000000, gpll0,   10,    0,     0),
	F_END
};

static struct rcg_clk usb30_mock_utmi_clk_src = {
	.cmd_reg      = (uint32_t *) USB30_MOCK_UTMI_CMD_RCGR,
	.cfg_reg      = (uint32_t *) USB30_MOCK_UTMI_CFG_RCGR,
	.set_rate     = clock_lib2_rcg_set_rate_hid,
	.freq_tbl     = ftbl_gcc_usb30_mock_utmi_clk_src,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "usb30_mock_utmi_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk gcc_usb30_mock_utmi_clk = {
	.cbcr_reg    = (uint32_t *) USB30_MOCK_UTMI_CBCR,
	.has_sibling = 0,
	.parent      = &usb30_mock_utmi_clk_src.c,

	.c = {
		.dbg_name = "usb30_mock_utmi_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_usb30_sleep_clk = {
	.cbcr_reg    = (uint32_t *) USB30_SLEEP_CBCR,
	.has_sibling = 1,

	.c = {
		.dbg_name = "usb30_sleep_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct clk_freq_tbl ftbl_gcc_usb30_phy_aux_clk_src[] = {
	F(   1200000,         cxo,   16,    0,     0),
	F_END
};

static struct rcg_clk usb30_phy_aux_clk_src = {
	.cmd_reg      = (uint32_t *) USB30_PHY_AUX_CMD_RCGR,
	.cfg_reg      = (uint32_t *) USB30_PHY_AUX_CFG_RCGR,
	.set_rate     = clock_lib2_rcg_set_rate_hid,
	.freq_tbl     = ftbl_gcc_usb30_phy_aux_clk_src,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "usb30_phy_aux_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk gcc_usb30_phy_aux_clk = {
	.cbcr_reg    = (uint32_t *)USB30_PHY_AUX_CBCR,
	.has_sibling = 0,
	.parent      = &usb30_phy_aux_clk_src.c,

	.c = {
		.dbg_name = "usb30_phy_aux_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_usb30_pipe_clk = {
	.bcr_reg      = (uint32_t *) USB30PHY_PHY_BCR,
	.cbcr_reg     = (uint32_t *) USB30_PHY_PIPE_CBCR,
	.has_sibling  = 1,

	.c = {
		.dbg_name = "usb30_pipe_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct reset_clk gcc_usb30_phy_reset = {
	.bcr_reg = (uint32_t )USB30_PHY_BCR,

	.c = {
		.dbg_name = "usb30_phy_reset",
		.ops      = &clk_ops_rst,
	},
};

static struct branch_clk gcc_usb_phy_cfg_ahb2phy_clk = {
	.cbcr_reg = (uint32_t *)USB_PHY_CFG_AHB2PHY_CBCR,
	.has_sibling = 1,

	.c = {
		.dbg_name = "usb_phy_cfg_ahb2phy_clk",
		.ops = &clk_ops_branch,
	},
};

/* Display clocks */
static struct clk_freq_tbl ftbl_mdss_esc0_1_clk[] = {
	F_MM(19200000,    cxo,   1,   0,   0),
	F_END
};

static struct clk_freq_tbl ftbl_mdss_esc1_1_clk[] = {
	F_MM(19200000,    cxo,   1,   0,   0),
	F_END
};

static struct clk_freq_tbl ftbl_mmss_axi_clk[] = {
	F_MM(19200000,     cxo,     1,   0,   0),
	F_MM(100000000,  gpll0,     6,   0,   0),
	F_MM(300000000,  gpll0,     2,   0,   0),
	F_END
};

static struct clk_freq_tbl ftbl_mdp_clk[] = {
	F_MM( 75000000,  gpll0,   8,   0,   0),
	F_MM( 240000000,  gpll0,   2.5,   0,   0),
	F_MM(300000000,  gpll0,     2,   0,   0),
	F_END
};

static struct rcg_clk dsi_esc0_clk_src = {
	.cmd_reg  = (uint32_t *) DSI_ESC0_CMD_RCGR,
	.cfg_reg  = (uint32_t *) DSI_ESC0_CFG_RCGR,
	.set_rate = clock_lib2_rcg_set_rate_hid,
	.freq_tbl = ftbl_mdss_esc0_1_clk,

	.c        = {
		.dbg_name = "dsi_esc0_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct rcg_clk dsi_esc1_clk_src = {
	.cmd_reg  = (uint32_t *) DSI_ESC1_CMD_RCGR,
	.cfg_reg  = (uint32_t *) DSI_ESC1_CFG_RCGR,
	.set_rate = clock_lib2_rcg_set_rate_hid,
	.freq_tbl = ftbl_mdss_esc1_1_clk,

	.c        = {
		.dbg_name = "dsi_esc1_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct clk_freq_tbl ftbl_mdss_vsync_clk[] = {
	F_MM(19200000,    cxo,   1,   0,   0),
	F_END
};

static struct rcg_clk vsync_clk_src = {
	.cmd_reg  = (uint32_t *) VSYNC_CMD_RCGR,
	.cfg_reg  = (uint32_t *) VSYNC_CFG_RCGR,
	.set_rate = clock_lib2_rcg_set_rate_hid,
	.freq_tbl = ftbl_mdss_vsync_clk,

	.c        = {
		.dbg_name = "vsync_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct rcg_clk mdp_axi_clk_src = {
	.cmd_reg  = (uint32_t *) MDP_AXI_CMD_RCGR,
	.cfg_reg  = (uint32_t *) MDP_AXI_CFG_RCGR,
	.set_rate = clock_lib2_rcg_set_rate_hid,
	.freq_tbl = ftbl_mmss_axi_clk,

	.c        = {
		.dbg_name = "mdp_axi_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk mdss_esc0_clk = {
	.cbcr_reg    = (uint32_t *) DSI_ESC0_CBCR,
	.parent      = &dsi_esc0_clk_src.c,
	.has_sibling = 0,

	.c           = {
		.dbg_name = "mdss_esc0_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdss_esc1_clk = {
	.cbcr_reg    = (uint32_t *) DSI_ESC1_CBCR,
	.parent      = &dsi_esc1_clk_src.c,
	.has_sibling = 0,

	.c           = {
		.dbg_name = "mdss_esc1_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdss_axi_clk = {
	.cbcr_reg    = (uint32_t *) MDP_AXI_CBCR,
	.parent      = &mdp_axi_clk_src.c,
	.has_sibling = 0,

	.c           = {
		.dbg_name = "mdss_axi_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mmss_mmssnoc_axi_clk = {
	.cbcr_reg    = (uint32_t *) MMSS_MMSSNOC_AXI_CBCR,
	.parent      = &mdp_axi_clk_src.c,
	.has_sibling = 0,

	.c           = {
		.dbg_name = "mmss_mmssnoc_axi_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mmss_s0_axi_clk = {
	.cbcr_reg    = (uint32_t *) MMSS_S0_AXI_CBCR,
	.parent      = &mdp_axi_clk_src.c,
	.has_sibling = 0,

	.c           = {
		.dbg_name = "mmss_s0_axi_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdp_ahb_clk = {
	.cbcr_reg    = (uint32_t *) MDP_AHB_CBCR,
	.has_sibling = 1,

	.c           = {
		.dbg_name = "mdp_ahb_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct rcg_clk mdss_mdp_clk_src = {
	.cmd_reg      = (uint32_t *) MDP_CMD_RCGR,
	.cfg_reg      = (uint32_t *) MDP_CFG_RCGR,
	.set_rate     = clock_lib2_rcg_set_rate_hid,
	.freq_tbl     = ftbl_mdp_clk,
	.current_freq = &rcg_dummy_freq,

	.c            = {
		.dbg_name = "mdss_mdp_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk mdss_mdp_clk = {
	.cbcr_reg    = (uint32_t *) MDP_CBCR,
	.parent      = &mdss_mdp_clk_src.c,
	.has_sibling = 1,

	.c           = {
		.dbg_name = "mdss_mdp_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdss_mdp_lut_clk = {
	.cbcr_reg    = MDP_LUT_CBCR,
	.parent      = &mdss_mdp_clk_src.c,
	.has_sibling = 1,

	.c           = {
		.dbg_name = "mdss_mdp_lut_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdss_vsync_clk = {
	.cbcr_reg    = MDSS_VSYNC_CBCR,
	.parent      = &vsync_clk_src.c,
	.has_sibling = 0,

	.c           = {
		.dbg_name = "mdss_vsync_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct clk_freq_tbl ftbl_mdss_edpaux_clk[] = {
	F_MM(19200000,    cxo,   1,   0,   0),
	F_END
};

static struct rcg_clk edpaux_clk_src = {
	.cmd_reg  = (uint32_t *) EDPAUX_CMD_RCGR,
	.set_rate = clock_lib2_rcg_set_rate_hid,
	.freq_tbl = ftbl_mdss_edpaux_clk,

	.c        = {
		.dbg_name = "edpaux_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk mdss_edpaux_clk = {
	.cbcr_reg    = MDSS_EDPAUX_CBCR,
	.parent      = &edpaux_clk_src.c,
	.has_sibling = 0,

	.c           = {
		.dbg_name = "mdss_edpaux_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct clk_freq_tbl ftbl_mdss_edplink_clk[] = {
	F_MDSS(162000000, edppll_270,   2,   0,   0),
	F_MDSS(270000000, edppll_270,  11,   0,   0),
	F_END
};

static struct rcg_clk edplink_clk_src = {
	.cmd_reg = (uint32_t *)EDPLINK_CMD_RCGR,
	.set_rate = clock_lib2_rcg_set_rate_hid,
	.freq_tbl = ftbl_mdss_edplink_clk,
	.current_freq = &rcg_dummy_freq,
	.c = {
		.dbg_name = "edplink_clk_src",
		.ops = &clk_ops_rcg,
	},
};

static struct clk_freq_tbl ftbl_mdss_edppixel_clk[] = {
	F_MDSS(138500000, edppll_350,   2,   0,   0),
	F_MDSS(350000000, edppll_350,  11,   0,   0),
	F_END
};

static struct rcg_clk edppixel_clk_src = {
	.cmd_reg = (uint32_t *)EDPPIXEL_CMD_RCGR,
	.set_rate = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl = ftbl_mdss_edppixel_clk,
	.current_freq = &rcg_dummy_freq,
	.c = {
		.dbg_name = "edppixel_clk_src",
		.ops = &clk_ops_rcg_mnd,
	},
};

static struct branch_clk mdss_edplink_clk = {
	.cbcr_reg = (uint32_t *)MDSS_EDPLINK_CBCR,
	.has_sibling = 0,
	.parent = &edplink_clk_src.c,
	.c = {
		.dbg_name = "mdss_edplink_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk mdss_edppixel_clk = {
	.cbcr_reg = (uint32_t *)MDSS_EDPPIXEL_CBCR,
	.has_sibling = 0,
	.parent = &edppixel_clk_src.c,
	.c = {
		.dbg_name = "mdss_edppixel_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk mmss_misc_ahb_clk = {
	.cbcr_reg = MMSS_MISC_AHB_CBCR,
	.has_sibling = 1,

	.c = {
		.dbg_name = "mmss_misc_ahb_clk",
		.ops = &clk_ops_branch,
    },
};

/* Clock lookup table */
static struct clk_lookup msm_8994_clocks[] =
{
	CLK_LOOKUP("sdc1_iface_clk", gcc_sdcc1_ahb_clk.c),
	CLK_LOOKUP("sdc1_core_clk",  gcc_sdcc1_apps_clk.c),

	CLK_LOOKUP("sdc2_iface_clk", gcc_sdcc2_ahb_clk.c),
	CLK_LOOKUP("sdc2_core_clk",  gcc_sdcc2_apps_clk.c),

	CLK_LOOKUP("uart2_iface_clk", gcc_blsp1_ahb_clk.c),
	CLK_LOOKUP("uart2_core_clk",  gcc_blsp1_uart2_apps_clk.c),

	CLK_LOOKUP("usb_iface_clk",  gcc_usb_hs_ahb_clk.c),
	CLK_LOOKUP("usb_core_clk",   gcc_usb_hs_system_clk.c),

	/* USB30 clocks */
	CLK_LOOKUP("usb2b_phy_sleep_clk", gcc_usb2b_phy_sleep_clk.c),
	CLK_LOOKUP("usb30_master_clk",    gcc_usb30_master_clk.c),
	CLK_LOOKUP("usb30_iface_clk",     gcc_sys_noc_usb30_axi_clk.c),
	CLK_LOOKUP("usb30_mock_utmi_clk", gcc_usb30_mock_utmi_clk.c),
	CLK_LOOKUP("usb30_sleep_clk",     gcc_usb30_sleep_clk.c),
	CLK_LOOKUP("usb30_phy_aux_clk",   gcc_usb30_phy_aux_clk.c),
	CLK_LOOKUP("usb30_pipe_clk",      gcc_usb30_pipe_clk.c),
	CLK_LOOKUP("usb30_phy_reset",     gcc_usb30_phy_reset.c),

	CLK_LOOKUP("usb_phy_cfg_ahb2phy_clk",     gcc_usb_phy_cfg_ahb2phy_clk.c),

	/* mdss clocks */
	CLK_LOOKUP("mdp_ahb_clk",          mdp_ahb_clk.c),
	CLK_LOOKUP("mdss_esc0_clk",        mdss_esc0_clk.c),
	CLK_LOOKUP("mdss_esc1_clk",        mdss_esc1_clk.c),
	CLK_LOOKUP("mdss_axi_clk",         mdss_axi_clk.c),
	CLK_LOOKUP("mmss_mmssnoc_axi_clk", mmss_mmssnoc_axi_clk.c),
	CLK_LOOKUP("mmss_s0_axi_clk",      mmss_s0_axi_clk.c),
	CLK_LOOKUP("mdss_vsync_clk",       mdss_vsync_clk.c),
	CLK_LOOKUP("mdss_mdp_clk_src",     mdss_mdp_clk_src.c),
	CLK_LOOKUP("mdss_mdp_clk",         mdss_mdp_clk.c),
	CLK_LOOKUP("mdss_mdp_lut_clk",     mdss_mdp_lut_clk.c),
	CLK_LOOKUP("mmss_misc_ahb_clk",    mmss_misc_ahb_clk.c),

	CLK_LOOKUP("edp_pixel_clk",        mdss_edppixel_clk.c),
	CLK_LOOKUP("edp_link_clk",         mdss_edplink_clk.c),
	CLK_LOOKUP("edp_aux_clk",          mdss_edpaux_clk.c),
};

void platform_clock_init(void)
{
	clk_init(msm_8994_clocks, ARRAY_SIZE(msm_8994_clocks));
}
