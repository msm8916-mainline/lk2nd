/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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
#include <platform.h>

/* Mux source select values */
#define cxo_source_val    0
#define gpll0_source_val  1
#define gpll2_source_val  4
#define gpll4_source_val  2
#define cxo_mm_source_val 0
#define gpll0_mm_source_val 6
#define gpll6_mm_source_val 3

struct clk_freq_tbl rcg_dummy_freq = F_END;


/* Clock Operations */
static struct clk_ops clk_ops_branch =
{
	.enable     = clock_lib2_branch_clk_enable,
	.disable    = clock_lib2_branch_clk_disable,
	.set_rate   = clock_lib2_branch_set_rate,
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
	.status_reg   = (void *) GPLL0_STATUS,
	.status_mask  = BIT(17),
	.parent       = &cxo_clk_src.c,

	.c = {
		.rate     = 800000000,
		.dbg_name = "gpll0_clk_src",
		.ops      = &clk_ops_pll_vote,
	},
};

static struct pll_vote_clk gpll2_clk_src =
{
	.en_reg       = (void *) APCS_GPLL_ENA_VOTE,
	.en_mask      = BIT(2),
	.status_reg   = (void *) GPLL2_STATUS,
	.status_mask  = BIT(17),
	.parent       = &cxo_clk_src.c,

	.c = {
		.rate     = 932000000,
		.dbg_name = "gpll2_clk_src",
		.ops      = &clk_ops_pll_vote,
	},
};

static struct pll_vote_clk gpll4_clk_src =
{
	.en_reg       = (void *) APCS_GPLL_ENA_VOTE,
	.en_mask      = BIT(5),
	.status_reg   = (void *) GPLL4_MODE,
	.status_mask  = BIT(30),
	.parent       = &cxo_clk_src.c,

	.c = {
		.rate     = 1152000000,
		.dbg_name = "gpll4_clk_src",
		.ops      = &clk_ops_pll_vote,
	},
};

static struct pll_vote_clk gpll6_clk_src =
{
	.en_reg       = (void *) APCS_GPLL_ENA_VOTE,
	.en_mask      = BIT(7),
	.status_reg   = (void *) GPLL6_STATUS,
	.status_mask  = BIT(17),
	.parent       = &cxo_clk_src.c,

	.c = {
		.rate     = 1080000000,
		.dbg_name = "gpll6_clk_src",
		.ops      = &clk_ops_pll_vote,
	},
};

/* SDCC Clocks */
static struct clk_freq_tbl ftbl_gcc_sdcc1_apps_clk[] =
{
	F(   144000,    cxo,  16,   3,  25),
	F(   400000,    cxo,  12,   1,   4),
	F( 20000000,  gpll0,  10,   1,   4),
	F( 25000000,  gpll0,  16,   1,   2),
	F( 50000000,  gpll0,  16,   0,   0),
	F(100000000,  gpll0,   8,   0,   0),
	F(177770000,  gpll0, 4.5,   0,   0),
	F(192000000,  gpll4,   6,   0,   0),
	F(384000000,  gpll4,   3,   0,   0),
	F_END
};

/* SDCC Clocks for version 8976 v 1.1*/
static struct clk_freq_tbl ftbl_gcc_sdcc1_apps_clk_8976_v_1_1[] =
{
	F(   144000,    cxo,  16,   3,  25),
	F(   400000,    cxo,  12,   1,   4),
	F( 20000000,  gpll0,  10,   1,   4),
	F( 25000000,  gpll0,  16,   1,   2),
	F( 50000000,  gpll0,  16,   0,   0),
	F(100000000,  gpll0,   8,   0,   0),
	F(177770000,  gpll0, 4.5,   0,   0),
	F(186400000,  gpll2,   5,   0,   0),
	F(372800000,  gpll2, 2.5,   0,   0),
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

static struct clk_freq_tbl ftbl_gcc_sdcc2_apps_clk[] =
{
	F(   144000,    cxo,  16,   3,  25),
	F(   400000,    cxo,  12,   1,   4),
	F( 20000000,  gpll0,  10,   1,   4),
	F( 25000000,  gpll0,  16,   1,   2),
	F( 50000000,  gpll0,  16,   0,   0),
	F(100000000,  gpll0,   8,   0,   0),
	F(177770000,  gpll0, 4.5,   0,   0),
	F(200000000,  gpll0,   4,   0,   0),
	F_END
};

static struct rcg_clk sdcc2_apps_clk_src =
{
	.cmd_reg      = (uint32_t *) SDCC2_CMD_RCGR,
	.cfg_reg      = (uint32_t *) SDCC2_CFG_RCGR,
	.m_reg        = (uint32_t *) SDCC2_M,
	.n_reg        = (uint32_t *) SDCC2_N,
	.d_reg        = (uint32_t *) SDCC2_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_sdcc2_apps_clk,
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

/* UART Clocks */
static struct clk_freq_tbl ftbl_gcc_blsp1_2_uart1_2_apps_clk[] =
{
	F( 3686400,  gpll0,    1,  72,  15625),
	F( 7372800,  gpll0,    1, 144,  15625),
	F(14745600,  gpll0,    1, 288,  15625),
	F(16000000,  gpll0,   10,   1,      5),
	F(19200000,    cxo,    1,   0,      0),
	F(24000000,  gpll0,    1,   3,    100),
	F(25000000,  gpll0,   16,   1,      2),
	F(32000000,  gpll0,    1,   1,     25),
	F(40000000,  gpll0,    1,   1,     20),
	F(46400000,  gpll0,    1,  29,    500),
	F(48000000,  gpll0,    1,   3,     50),
	F(51200000,  gpll0,    1,   8,    125),
	F(56000000,  gpll0,    1,   7,    100),
	F(58982400,  gpll0,    1,1152,  15625),
	F(60000000,  gpll0,    1,   3,     40),
	F_END
};

static struct rcg_clk blsp1_uart2_apps_clk_src =
{
	.cmd_reg      = (uint32_t *) BLSP1_UART2_APPS_CMD_RCGR,
	.cfg_reg      = (uint32_t *) BLSP1_UART2_APPS_CFG_RCGR,
	.m_reg        = (uint32_t *) BLSP1_UART2_APPS_M,
	.n_reg        = (uint32_t *) BLSP1_UART2_APPS_N,
	.d_reg        = (uint32_t *) BLSP1_UART2_APPS_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_blsp1_2_uart1_2_apps_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "blsp1_uart2_apps_clk",
		.ops      = &clk_ops_rcg_mnd,
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
	.en_mask      = BIT(10),

	.c = {
		.dbg_name = "gcc_blsp1_ahb_clk",
		.ops      = &clk_ops_vote,
	},
};

/* USB Clocks */
static struct clk_freq_tbl ftbl_gcc_usb_hs_system_clk[] =
{
	F(100000000,  gpll0,   10,   0,   0),
	F(133330000,  gpll0,    6,   0,   0),
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

/* Display clocks */
static struct clk_freq_tbl ftbl_mdss_esc0_1_clk[] = {
	F_MM(19200000,    cxo,   1,   0,   0),
	F_END
};

static struct clk_freq_tbl ftbl_mdss_esc1_1_clk[] = {
	F_MM(19200000,    cxo,   1,   0,   0),
	F_END
};

static struct clk_freq_tbl ftbl_mdp_clk[] = {
	F( 50000000,    gpll0,  16,     0,      0),
	F( 80000000,    gpll0,  10,     0,      0),
	F( 100000000,   gpll0,  8,      0,      0),
	F( 145450000,   gpll0,  5.5,    0,      0),
	F( 160000000,   gpll0,  5,      0,      0),
	F( 177780000,   gpll0,  4.5,    0,      0),
	F( 200000000,   gpll0,  4,      0,      0),
	F( 266670000,   gpll0,  3,      0,      0),
	F( 320000000,   gpll0,  2.5,    0,      0),
	F_END
};

static struct clk_freq_tbl ftbl_mdp_clk_8956[] = {
	F_MM(  50000000, gpll0,   16,    0,     0),
	F_MM(  80000000, gpll0,   10,    0,     0),
	F_MM( 100000000, gpll0,    8,    0,     0),
	F_MM( 145454545, gpll0,  5.5,    0,     0),
	F_MM( 160000000, gpll0,    5,    0,     0),
	F_MM( 177777778, gpll0,  4.5,    0,     0),
	F_MM( 200000000, gpll0,    4,    0,     0),
	F_MM( 270000000, gpll6,    4,    0,     0),
	F_MM( 320000000, gpll0,  2.5,    0,     0),
	F_MM( 360000000, gpll6,    3,    0,     0),
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
	.has_sibling = 1,

	.c           = {
		.dbg_name = "mdss_axi_clk",
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
	.has_sibling = 0,

	.c           = {
		.dbg_name = "mdss_mdp_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdss_vsync_clk = {
	.cbcr_reg    = (uint32_t *) MDSS_VSYNC_CBCR,
	.parent      = &vsync_clk_src.c,
	.has_sibling = 0,

	.c           = {
		.dbg_name = "mdss_vsync_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct clk_freq_tbl ftbl_gcc_ce1_clk[] = {
	F(160000000,  gpll0,   5,   0,   0),
	F_END
};

static struct rcg_clk ce1_clk_src = {
	.cmd_reg      = (uint32_t *) GCC_CRYPTO_CMD_RCGR,
	.cfg_reg      = (uint32_t *) GCC_CRYPTO_CFG_RCGR,
	.set_rate     = clock_lib2_rcg_set_rate_hid,
	.freq_tbl     = ftbl_gcc_ce1_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "ce1_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct vote_clk gcc_ce1_clk = {
	.cbcr_reg      = (uint32_t *) GCC_CRYPTO_CBCR,
	.vote_reg      = (uint32_t *) APCS_CLOCK_BRANCH_ENA_VOTE,
	.en_mask       = BIT(2),

	.c = {
		.dbg_name  = "gcc_ce1_clk",
		.ops       = &clk_ops_vote,
	},
};

static struct vote_clk gcc_ce1_ahb_clk = {
	.cbcr_reg     = (uint32_t *) GCC_CRYPTO_AHB_CBCR,
	.vote_reg     = (uint32_t *) APCS_CLOCK_BRANCH_ENA_VOTE,
	.en_mask      = BIT(0),

	.c = {
		.dbg_name = "gcc_ce1_ahb_clk",
		.ops      = &clk_ops_vote,
	},
};

static struct vote_clk gcc_ce1_axi_clk = {
	.cbcr_reg     = (uint32_t *) GCC_CRYPTO_AXI_CBCR,
	.vote_reg     = (uint32_t *) APCS_CLOCK_BRANCH_ENA_VOTE,
	.en_mask      = BIT(1),

	.c = {
		.dbg_name = "gcc_ce1_axi_clk",
		.ops      = &clk_ops_vote,
	},
};

/* Clock lookup table */
static struct clk_lookup msm_clocks_8952[] =
{
	CLK_LOOKUP("sdc1_iface_clk", gcc_sdcc1_ahb_clk.c),
	CLK_LOOKUP("sdc1_core_clk",  gcc_sdcc1_apps_clk.c),

	CLK_LOOKUP("sdc2_iface_clk", gcc_sdcc2_ahb_clk.c),
	CLK_LOOKUP("sdc2_core_clk",  gcc_sdcc2_apps_clk.c),

	CLK_LOOKUP("uart2_iface_clk", gcc_blsp1_ahb_clk.c),
	CLK_LOOKUP("uart2_core_clk",  gcc_blsp1_uart2_apps_clk.c),

	CLK_LOOKUP("usb_iface_clk",  gcc_usb_hs_ahb_clk.c),
	CLK_LOOKUP("usb_core_clk",   gcc_usb_hs_system_clk.c),

	CLK_LOOKUP("mdp_ahb_clk",          mdp_ahb_clk.c),
	CLK_LOOKUP("mdss_esc0_clk",        mdss_esc0_clk.c),
	CLK_LOOKUP("mdss_esc1_clk",        mdss_esc1_clk.c),
	CLK_LOOKUP("mdss_axi_clk",         mdss_axi_clk.c),
	CLK_LOOKUP("mdss_vsync_clk",       mdss_vsync_clk.c),
	CLK_LOOKUP("mdss_mdp_clk_src",     mdss_mdp_clk_src.c),
	CLK_LOOKUP("mdss_mdp_clk",         mdss_mdp_clk.c),

	CLK_LOOKUP("ce1_ahb_clk",  gcc_ce1_ahb_clk.c),
	CLK_LOOKUP("ce1_axi_clk",  gcc_ce1_axi_clk.c),
	CLK_LOOKUP("ce1_core_clk", gcc_ce1_clk.c),
	CLK_LOOKUP("ce1_src_clk",  ce1_clk_src.c),
};

void msm8956_clock_override()
{
	gpll4_clk_src.status_reg = (void *)GPLL4_STATUS;
	gpll4_clk_src.status_mask = BIT(17);
	mdss_mdp_clk_src.freq_tbl = ftbl_mdp_clk_8956;
}

void msm8976_v_1_1_sdcc_clock_modify()
{
	sdcc1_apps_clk_src.freq_tbl = ftbl_gcc_sdcc1_apps_clk_8976_v_1_1;
}

void msm8937_clock_override()
{
	gpll0_clk_src.status_reg = (void *)GPLL0_MODE;
	gpll0_clk_src.status_mask = BIT(30);
}

void platform_clock_init(void)
{
	if (platform_is_msm8956()) {
		msm8956_clock_override();
		if (platform_is_msm8976_v_1_1())
			/*freq and GPLL change for 8976 v1.1 */
			msm8976_v_1_1_sdcc_clock_modify();
	}

	if (platform_is_msm8937() || platform_is_msm8917())
		msm8937_clock_override();

	clk_init(msm_clocks_8952, ARRAY_SIZE(msm_clocks_8952));
}
