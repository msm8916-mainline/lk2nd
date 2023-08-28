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

#if TARGET_MSM8916
static int target_tps65132_ctrl(uint8_t enable)
{
	if (enable) {
		/* for tps65132 ENP pin */
		gpio_tlmm_config(enp_gpio.pin_id, 0,
			enp_gpio.pin_direction, enp_gpio.pin_pull,
			enp_gpio.pin_strength,
			enp_gpio.pin_state);
		gpio_set_dir(enp_gpio.pin_id, 2);

		/* for tps65132 ENN pin*/
		gpio_tlmm_config(enn_gpio.pin_id, 0,
			enn_gpio.pin_direction, enn_gpio.pin_pull,
			enn_gpio.pin_strength,
			enn_gpio.pin_state);
		gpio_set_dir(enn_gpio.pin_id, 2);
	} else {
		gpio_set_dir(enp_gpio.pin_id, 0); /* ENP */
		gpio_set_dir(enn_gpio.pin_id, 0); /* ENN */
	}
	return 0;
}

int target_ldo_ctrl(uint8_t enable, struct msm_panel_info *pinfo)
{
	if (panel_select(LK1ST_PANEL) == panel_tianma_nt35521_5p5_720p_video_select ||
	    panel_select(LK1ST_PANEL) == panel_cmi_nt35532_5p5_1080pxa_video_select)
		return target_tps65132_ctrl(enable);

	return NO_ERROR;
}
#endif

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
