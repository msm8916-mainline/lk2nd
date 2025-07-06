// SPDX-License-Identifier: GPL-2.0-only

#include <err.h>
#include <mipi_dsi.h>
#include <mipi_dsi_i2c.h>
#include <string.h>
#include <target/display.h>

#include <panel.h>
#include <panel_display.h>

#include <lk2nd/panel.h>

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

const char *lk2nd_oem_panel_name(void)
{
	struct panel_struct panel;
	struct msm_panel_info pinfo;
	struct mdss_dsi_phy_ctrl phy_db;

	panel_select(LK2ND_DISPLAY)(&panel, &pinfo, &phy_db);
	return panel.paneldata->panel_node_id;
}

int oem_panel_select(const char *panel_name, struct panel_struct *panel,
		     struct msm_panel_info *pinfo, struct mdss_dsi_phy_ctrl *phy_db)
{
	panel_select(LK2ND_DISPLAY)(panel, pinfo, phy_db);

#if TARGET_MSM8916
	if (phy_db->regulator_mode == DSI_PHY_REGULATOR_LDO_MODE)
		memcpy(panel_regulator_settings, ldo_regulator_settings, REGULATOR_SIZE);
	else
		memcpy(panel_regulator_settings, dcdc_regulator_settings, REGULATOR_SIZE);
#endif

	return PANEL_TYPE_DSI;
}
