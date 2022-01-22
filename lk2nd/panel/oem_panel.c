// SPDX-License-Identifier: GPL-2.0-only

#include <err.h>
#include <lk2nd.h>
#include <mipi_dsi.h>
#include <mipi_dsi_i2c.h>
#include <string.h>
#include <target/display.h>

#include <panel.h>
#include <panel_display.h>

#include "panel_adv7533_720p60_video.h"
#include "panel_adv7533_1080p60_video.h"
#include "generated/panels.h"

#if TARGET_MSM8916
uint32_t panel_regulator_settings[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

int oem_panel_rotation()
{
	return NO_ERROR;
}

int oem_panel_on()
{
	return NO_ERROR;
}

int oem_panel_off()
{
	return NO_ERROR;
}

uint32_t oem_panel_max_auto_detect_panels()
{
	return 0;
}

#define _panel_select(panel)	panel_##panel##_select
#define panel_select(panel)	_panel_select(panel)

int oem_panel_select(const char *panel_name, struct panel_struct *panel,
		     struct msm_panel_info *pinfo, struct mdss_dsi_phy_ctrl *phy_db)
{
	panel_select(LK1ST_PANEL)(panel, pinfo, phy_db);
	lk2nd_dev.panel.name = panel->paneldata->panel_node_id;

#if TARGET_MSM8916
	if (phy_db->regulator_mode == DSI_PHY_REGULATOR_LDO_MODE)
		memcpy(panel_regulator_settings, ldo_regulator_settings, REGULATOR_SIZE);
	else
		memcpy(panel_regulator_settings, dcdc_regulator_settings, REGULATOR_SIZE);
#endif

	return PANEL_TYPE_DSI;
}
