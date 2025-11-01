// SPDX-License-Identifier: GPL-2.0-only

#ifndef _LK2ND_GENERATED_PANELS_H_
#define _LK2ND_GENERATED_PANELS_H_

#include "lk_panel_boent35523b_800p_video.h"
#include "lk_panel_boe_otm8019a_5p0_fwvga_video.h"
#include "lk_panel_boehx8389c_qhd_video.h"
#include "lk_panel_hx8394d_720p_video.h"
#include "lk_panel_ili9806e_boyi_byt45mp41t1m_4p5cpt.h"
#include "lk_panel_ili9806e_fwvga_hsd_hlt_video.h"
#include "lk_panel_ili9881c_hd_video.h"
#include "lk_panel_innolux_qhd_video.h"
#include "lk_panel_r61308_720p_video.h"
#include "lk_panel_r69431_720p_video.h"

int oem_panel_select(const char *panel_name, struct panel_struct *panel,
		     struct msm_panel_info *pinfo, struct mdss_dsi_phy_ctrl *phy_db);

#endif /* _LK2ND_GENERATED_PANELS_H_ */
