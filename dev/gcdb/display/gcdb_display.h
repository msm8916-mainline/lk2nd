/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
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

#ifndef _GCDB_DISPLAY_H_
#define _GCDB_DISPLAY_H_

/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include <debug.h>
#include "include/display_resource.h"
#include "panel_display.h"

#define REGULATOR_SIZE 28
#define PHYSICAL_SIZE 16
#define STRENGTH_SIZE 8
#define BIST_SIZE 6
#define LANE_SIZE 45

#define DSI_CFG_SIZE 15

/*---------------------------------------------------------------------------*/
/* API                                                                       */
/*---------------------------------------------------------------------------*/

int target_backlight_ctrl(struct backlight *bl, uint8_t enable);
int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo);
int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
						struct msm_panel_info *pinfo);
int target_ldo_ctrl(uint8_t enable, struct msm_panel_info *pinfo);
int target_display_dsi2hdmi_config(struct msm_panel_info *pinfo);
int target_dsi_phy_config(struct mdss_dsi_phy_ctrl *phy_db);

int gcdb_display_init(const char *panel_name, uint32_t rev, void *base);
int gcdb_display_cmdline_arg(char *pbuf, uint16_t buf_size);
void gcdb_display_shutdown();
int oem_panel_select(const char *panel_name, struct panel_struct *panelstruct,
	struct msm_panel_info *pinfo, struct mdss_dsi_phy_ctrl *phy_db);
void set_panel_cmd_string(const char *panel_name);
struct oem_panel_data mdss_dsi_get_oem_data(void);
struct oem_panel_data *mdss_dsi_get_oem_data_ptr(void);
struct panel_struct mdss_dsi_get_panel_data(void);

struct oem_panel_data  {
	char panel[MAX_PANEL_ID_LEN];
	char sec_panel[MAX_PANEL_ID_LEN];
	bool cont_splash;
	bool skip;
	bool swap_dsi_ctrl;
	uint32_t sim_mode;
	char dsi_config[DSI_CFG_SIZE];
	uint32_t dsi_pll_src;
	/* If dual-DSI, slave cfg will use 2nd index */
	int cfg_num[2]; /* -ve number means no overide */
};

enum {
    DSI_PLL_DEFAULT,
    DSI_PLL0,
    DSI_PLL1,
};

static inline bool is_dsi_config_split(void)
{
	struct panel_struct panelstruct = mdss_dsi_get_panel_data();

	return panelstruct.paneldata->panel_node_id &&
		panelstruct.paneldata->slave_panel_node_id &&
		(panelstruct.paneldata->panel_operating_mode & (DUAL_DSI_FLAG |
		SPLIT_DISPLAY_FLAG | DST_SPLIT_FLAG));
}

static inline bool is_dsi_config_dual(void)
{
	struct oem_panel_data *oem_data = mdss_dsi_get_oem_data_ptr();

	return !is_dsi_config_split() && oem_data->sec_panel &&
		strcmp(oem_data->sec_panel, "");
}

static inline bool is_dsi_config_single()
{
	struct panel_struct panelstruct = mdss_dsi_get_panel_data();

	return panelstruct.paneldata->panel_node_id && !is_dsi_config_split()
		&& !is_dsi_config_dual();
}

#endif /*_GCDB_DISPLAY_H_ */
