/* Copyright (c) 2013-2016, The Linux Foundation. All rights reserved.
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

#include <debug.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <mdp5.h>

#include "gcdb_display.h"
#include "target/display.h"
#include "fastboot_oem_display.h"

struct oem_panel_data oem_data = {{'\0'}, {'\0'}, false, false, false, SIM_NONE,
	"single_dsi", DSI_PLL_DEFAULT, {-1, -1}};

static int panel_name_to_dt_string(struct panel_lookup_list supp_panels[],
			  uint32_t supp_panels_size,
			  const char *panel_name, char **panel_node)
{
	uint32_t i;

	if (!panel_name) {
		dprintf(CRITICAL, "Invalid panel name\n");
		return ERR_NOT_VALID;
	}

	for (i = 0; i < supp_panels_size; i++) {
		if (!strncmp(panel_name, supp_panels[i].name,
			MAX_PANEL_ID_LEN)) {
			*panel_node = supp_panels[i].panel_dt_string;
			return supp_panels[i].is_split_dsi;
		}
	}

	dprintf(CRITICAL, "Panel_name:%s not found in lookup table\n",
		panel_name);
	return ERR_NOT_FOUND;
}

void sim_override_to_cmdline(struct sim_lookup_list sim[],
			  uint32_t sim_size, uint32_t sim_mode,
			  char **sim_string)
{
	uint32_t i;

	for (i = 0; i < sim_size; i++) {
		if (sim_mode == sim[i].sim_mode) {
			*sim_string = sim[i].override_string;
			break;
		}
	}

	if (i == sim_size)
		dprintf(CRITICAL, "Sim_mode not found in lookup table\n");
}

struct oem_panel_data mdss_dsi_get_oem_data(void)
{
	return oem_data;
}

struct oem_panel_data *mdss_dsi_get_oem_data_ptr(void)
{
	return &oem_data;
}

static char *get_panel_token_end(const char *string)
{
	char *ch_hash = NULL, *ch_col = NULL;

	/* ':' and '#' are delimiters in the string */
	ch_col = strchr((char *) string, ':');
	ch_hash = strchr((char *) string, '#');

	if (ch_col && ch_hash)
		return ((ch_col < ch_hash) ? ch_col : ch_hash);
	else if (ch_col)
		return ch_col;
	else if (ch_hash)
		return ch_hash;
	return NULL;
}

void set_panel_cmd_string(const char *panel_name)
{
	char *ch = NULL, *ch_tmp = NULL;
	int i;

	panel_name += strspn(panel_name, " ");

	/* Primary panel string */
	ch = strstr((char *) panel_name, "prim:");
	if (ch) {
		/*
		 * Parse the primary panel for cases where 'prim' prefix
		 * is present in the fastboot oem command before primary
		 * panel string.
		 * Examples:
		 * 1.) fastboot oem select-display-panel prim:jdi_1080p_video:sec:sharp_1080p_cmd
		 * 2.) fastboot oem select-display-panel prim:jdi_1080p_video:skip:sec:sharp_1080p_cmd
		 * 3.) fastboot oem select-display-panel prim:jdi_1080p_video:disable:sec:sharp_1080p_cmd
		 * 4.) fastboot oem select-display-panel prim:jdi_1080p_video:skip#sim:sec:sharp_1080p_cmd
		 */
		ch += 5;
		ch_tmp = get_panel_token_end((const char*) ch);
		if (!ch_tmp)
			ch_tmp = ch + strlen(ch);
		for (i = 0; (ch + i) < ch_tmp; i++)
			oem_data.panel[i] = *(ch + i);
		oem_data.panel[i] = '\0';
	} else {
		/*
		 * Check if secondary panel string is present.
		 * The 'prim' prefix definitely needs to be present
		 * to specify primary panel for cases where secondary panel
		 * is also specified in fastboot oem command. Otherwise, it
		 * becomes tough to parse the fastboot oem command for primary
		 * panel. If 'sec' prefix is used without 'prim' prefix, it
		 * means the default panel needs to be picked as primary panel.
		 * Example:
		 * fastboot oem select-display-panel sec:sharp_1080p_cmd
		 */
		ch = strstr((char *) panel_name, "sec:");
		if (!ch) {
			/*
			 * This code will be executed for cases where the
			 * secondary panel is not specified i.e., single/split
			 * DSI cases.
			 * Examples:
			 * 1.) fastboot oem select-display-panel jdi_1080p_video
			 * 2.) fastboot oem select-display-panel sharp_1080p_cmd:skip
			 * 3.) fastboot oem select-display-panel sharp_1080p_cmd:disable
			 * 4.) fastboot oem select-display-panel sim_cmd_panel#sim-swte
			 */
			ch = get_panel_token_end(panel_name);
			if (ch) {
				for (i = 0; (panel_name + i) < ch; i++)
					oem_data.panel[i] =
						*(panel_name + i);
				oem_data.panel[i] = '\0';
			} else {
				strlcpy(oem_data.panel, panel_name,
					MAX_PANEL_ID_LEN);
			}
		}
	}

	/*
	 * Secondary panel string.
	 * This is relatively simple. The secondary panel string gets
	 * parsed if the 'sec' prefix is present.
	 */
	ch = strstr((char *) panel_name, "sec:");
	if (ch) {
		ch += 4;
		ch_tmp = get_panel_token_end((const char*) ch);
		if (!ch_tmp)
			ch_tmp = ch + strlen(ch);
		for (i = 0; (ch + i) < ch_tmp; i++)
			oem_data.sec_panel[i] = *(ch + i);
		oem_data.sec_panel[i] = '\0';

		/* Topology configuration for secondary panel */
		ch_tmp = strstr((char *) ch, ":cfg");
		if (ch_tmp)
			oem_data.cfg_num[1] = atoi((const char*)(ch_tmp + 4));
	} else {
		oem_data.sec_panel[0] = '\0';
	}

	/* Topology configuration for primary panel */
	ch_tmp = strstr((char *) panel_name, ":cfg");
	if (ch_tmp && (!ch || (ch && (ch_tmp < ch))))
		oem_data.cfg_num[0] = atoi((const char*)(ch_tmp + 4));

	/* Skip LK configuration */
	ch = strstr((char *) panel_name, ":skip");
	oem_data.skip = ch ? true : false;

	/* Cont. splash status */
	ch = strstr((char *) panel_name, ":disable");
	oem_data.cont_splash = ch ? false : true;

	/* Interposer card to swap DSI0 and DSI1 lanes */
	ch = strstr((char *) panel_name, ":swap");
	oem_data.swap_dsi_ctrl = ch ? true : false;

	/* DSI PLL source */
	ch = strstr((char *) panel_name, ":pll0");
	if (ch) {
		oem_data.dsi_pll_src = DSI_PLL0;
	} else {
		ch = strstr((char *) panel_name, ":pll1");
		if (ch)
			oem_data.dsi_pll_src = DSI_PLL1;
	}

	/* Simulator status */
	oem_data.sim_mode = SIM_NONE;
	if (strstr((char *) panel_name, "#sim-hwte"))
		oem_data.sim_mode = SIM_HWTE;
	else if (strstr((char *) panel_name, "#sim-swte"))
		oem_data.sim_mode = SIM_SWTE;
	else if (strstr((char *) panel_name, "#sim"))
		oem_data.sim_mode = SIM_MODE;

	/* disable cont splash when booting in simulator mode */
	if (oem_data.sim_mode)
		oem_data.cont_splash = false;
}

static bool mdss_dsi_set_panel_node(char *panel_name, char **dsi_id,
		char **panel_node, char **slave_panel_node, int *panel_mode)
{
	int rc = 0;

	if (!strcmp(panel_name, SIM_VIDEO_PANEL)) {
		*dsi_id = SIM_DSI_ID;
		*panel_node = SIM_VIDEO_PANEL_NODE;
		*panel_mode = 0;
	} else if (!strcmp(panel_name, SIM_DUALDSI_VIDEO_PANEL)) {
		*dsi_id = SIM_DSI_ID;
		*panel_node = SIM_DUALDSI_VIDEO_PANEL_NODE;
		*slave_panel_node = SIM_DUALDSI_VIDEO_SLAVE_PANEL_NODE;
		*panel_mode = DUAL_DSI_FLAG;
	} else if (!strcmp(panel_name, SIM_CMD_PANEL)) {
		*dsi_id = SIM_DSI_ID;
		*panel_node = SIM_CMD_PANEL_NODE;
		*panel_mode = 0;
	} else if (!strcmp(panel_name, SIM_DUALDSI_CMD_PANEL)) {
		*dsi_id = SIM_DSI_ID;
		*panel_node = SIM_DUALDSI_CMD_PANEL_NODE;
		*slave_panel_node = SIM_DUALDSI_CMD_SLAVE_PANEL_NODE;
		*panel_mode = DUAL_DSI_FLAG;
	} else if (oem_data.skip) {
		/* For skip panel case, check the lookup table */
		*dsi_id = SIM_DSI_ID;
		rc = panel_name_to_dt_string(lookup_skip_panels,
			ARRAY_SIZE(lookup_skip_panels), panel_name,
			panel_node);
		if (rc < 0) {
			return false;
		} else if (rc == 1) {
			*slave_panel_node = *panel_node;
			*panel_mode = DUAL_DSI_FLAG;
		} else {
			*panel_mode = 0;
		}
	} else {
		return false;
	}

	return true;
}

bool gcdb_display_cmdline_arg(char *pbuf, uint16_t buf_size)
{
	char *dsi_id = NULL;
	char *panel_node = NULL;
	char *slave_panel_node = NULL;
	char *sim_mode_string = NULL;
	uint16_t dsi_id_len = 0, panel_node_len = 0, slave_panel_node_len = 0;
	uint32_t arg_size = 0;
	bool ret = true, rc;
	int ret_val;
	const char *default_str;
	struct panel_struct panelstruct;
	int panel_mode = SPLIT_DISPLAY_FLAG | DUAL_PIPE_FLAG | DST_SPLIT_FLAG;
	int prefix_string_len = strlen(DISPLAY_CMDLINE_PREFIX);
	char *sctl_string, *pll_src_string = NULL;
	char prim_cfg_name[10]="\0", slave_cfg_name[10]="\0"; /* config[0-99] */
	char *display_cmd_line = pbuf;

	panelstruct = mdss_dsi_get_panel_data();

	rc = mdss_dsi_set_panel_node(oem_data.panel, &dsi_id, &panel_node,
			&slave_panel_node, &panel_mode);

	if (!rc) {
		if (panelstruct.paneldata) {
			dsi_id = panelstruct.paneldata->panel_controller;
			panel_node = panelstruct.paneldata->panel_node_id;
			panel_mode =
				panelstruct.paneldata->panel_operating_mode &
								panel_mode;
			slave_panel_node =
				panelstruct.paneldata->slave_panel_node_id;
		} else if (oem_data.sec_panel &&
			strcmp(oem_data.sec_panel, "")) {
			dsi_id = SIM_DSI_ID;
			panel_node = NO_PANEL_CONFIG;
			panel_mode = 0;
		} else {
			default_str = "0";
			arg_size = prefix_string_len + strlen(default_str);
			if (buf_size < arg_size) {
				dprintf(CRITICAL, "display command line buffer is small\n");
				return false;
			}

			strlcpy(pbuf, DISPLAY_CMDLINE_PREFIX, buf_size);
			pbuf += prefix_string_len;
			buf_size -= prefix_string_len;

			strlcpy(pbuf, default_str, buf_size);
			return true;
		}
	}

	if (dsi_id == NULL || panel_node == NULL) {
		dprintf(CRITICAL, "panel node or dsi ctrl not present\n");
		return false;
	}

	if (((panel_mode & SPLIT_DISPLAY_FLAG) ||
	     (panel_mode & DST_SPLIT_FLAG)) && slave_panel_node == NULL) {
		dprintf(CRITICAL, "slave node not present in split-dsi case\n");
		return false;
	}

	if (oem_data.sec_panel && strcmp(oem_data.sec_panel, "")) {
		if (panel_mode & (DUAL_DSI_FLAG | SPLIT_DISPLAY_FLAG |
			DST_SPLIT_FLAG)) {
			dprintf(CRITICAL, "Invalid config: Primary panel is"
				"split DSI and still secondary panel passed\n");
		} else {
			ret_val = panel_name_to_dt_string(lookup_skip_panels,
				ARRAY_SIZE(lookup_skip_panels), oem_data.sec_panel,
				&slave_panel_node);
			if (ret_val < 0) {
				dprintf(CRITICAL, "Sec. panel not found."
					" Continue with primary panel\n");
			} else if (ret_val == 1) {
				dprintf(CRITICAL, "Invalid config: Secondary panel cant"
					"be split DSI. Continue with primary panel\n");
				slave_panel_node = NULL;
			}
		}
	}

	/* Check for the DSI configuration */
	if (slave_panel_node && (panel_mode & (DUAL_DSI_FLAG |
		SPLIT_DISPLAY_FLAG | DST_SPLIT_FLAG)))
		strlcpy(oem_data.dsi_config, "split_dsi", DSI_CFG_SIZE);
	else if (slave_panel_node)
		strlcpy(oem_data.dsi_config, "dual_dsi", DSI_CFG_SIZE);
	else
		strlcpy(oem_data.dsi_config, "single_dsi", DSI_CFG_SIZE);

	arg_size = DSI_CFG_STRING_LEN + strlen(oem_data.dsi_config);

	dsi_id_len = strlen(dsi_id);
	panel_node_len = strlen(panel_node);
	if (!slave_panel_node || !strcmp(slave_panel_node, ""))
		slave_panel_node = NO_PANEL_CONFIG;
	slave_panel_node_len = strlen(slave_panel_node);

	arg_size += prefix_string_len + dsi_id_len + panel_node_len +
						LK_OVERRIDE_PANEL_LEN + 1;

	if (panelstruct.paneldata &&
		!strcmp(panelstruct.paneldata->panel_destination, "DISPLAY_2"))
		sctl_string = DSI_0_STRING;
	else
		sctl_string = DSI_1_STRING;

	arg_size += strlen(sctl_string) + slave_panel_node_len;

	if (oem_data.skip && !strcmp(oem_data.dsi_config, "dual_dsi") &&
		(oem_data.dsi_pll_src != DSI_PLL_DEFAULT)) {
		dprintf(CRITICAL, "Dual DSI config detected!"
			" Use default PLL\n");
		oem_data.dsi_pll_src = DSI_PLL_DEFAULT;
	}

	if (oem_data.dsi_pll_src != DSI_PLL_DEFAULT) {
		if (oem_data.dsi_pll_src == DSI_PLL0)
			pll_src_string = DSI_PLL0_STRING;
		else
			pll_src_string = DSI_PLL1_STRING;

		arg_size += strlen(pll_src_string);
	}

	if (oem_data.sim_mode != SIM_NONE) {
		sim_override_to_cmdline(lookup_sim,
			ARRAY_SIZE(lookup_sim), oem_data.sim_mode,
			&sim_mode_string);
		if (sim_mode_string) {
			arg_size += LK_SIM_OVERRIDE_LEN +
				strlen(sim_mode_string);
		} else {
			dprintf(CRITICAL, "SIM string NULL but mode is not NONE\n");
			return false;
		}
	}

	dprintf(SPEW, "dsi_cfg:%s mdp_cfg[0]=%d mdp_cfg[1]=%d\n",
		oem_data.dsi_config, oem_data.cfg_num[0], oem_data.cfg_num[1]);

	if ((oem_data.cfg_num[0] >= 0) && (oem_data.cfg_num[0] < 100)) {
		snprintf(prim_cfg_name, sizeof(prim_cfg_name),
			":config%d", oem_data.cfg_num[0]);
		arg_size += strlen(prim_cfg_name);
	} else if (panelstruct.config != NULL) {
		/*
		 * if oem command wasn't set then take topology config
		 * used by per target oem panel driver if available.
		 */
		snprintf(prim_cfg_name, sizeof(prim_cfg_name),
			":%s", panelstruct.config->config_name);
		arg_size += strlen(prim_cfg_name);
	}

	/* in split-dsi, primary and slave panel share same topology config */
	if (!strcmp(oem_data.dsi_config, "split_dsi"))
		snprintf(slave_cfg_name, sizeof(slave_cfg_name),
			"%s", prim_cfg_name);

	if (!strcmp(oem_data.dsi_config, "dual_dsi")) {
		if ((oem_data.cfg_num[1] >= 0) && (oem_data.cfg_num[1] < 100)) {
			snprintf(slave_cfg_name, sizeof(slave_cfg_name),
				":config%d", oem_data.cfg_num[1]);
			arg_size += strlen(slave_cfg_name);
		}
		/*
		 * In dual-dsi, secondary or slave panels isn't supported
		 * in bootloader so "else" case like above is not possible.
		 */
	}

	if (buf_size < arg_size) {
		dprintf(CRITICAL, "display command line buffer is small\n");
		ret = false;
	} else {
		strlcpy(pbuf, DISPLAY_CMDLINE_PREFIX, buf_size);
		pbuf += prefix_string_len;
		buf_size -= prefix_string_len;

		strlcpy(pbuf, LK_OVERRIDE_PANEL, buf_size);
		pbuf += LK_OVERRIDE_PANEL_LEN;
		buf_size -= LK_OVERRIDE_PANEL_LEN;

		strlcpy(pbuf, dsi_id, buf_size);
		pbuf += dsi_id_len;
		buf_size -= dsi_id_len;

		strlcpy(pbuf, panel_node, buf_size);
		pbuf += panel_node_len;
		buf_size -= panel_node_len;

		/* writeout primary topology config */
		if (strlen(prim_cfg_name) > 0) {
			strlcpy(pbuf, prim_cfg_name, buf_size);
			pbuf += strlen(prim_cfg_name);
			buf_size -= strlen(prim_cfg_name);
		}

		strlcpy(pbuf, sctl_string, buf_size);
		pbuf += strlen(sctl_string);
		buf_size -= strlen(sctl_string);

		strlcpy(pbuf, slave_panel_node, buf_size);
		pbuf += slave_panel_node_len;
		buf_size -= slave_panel_node_len;

		/* writeout slave panel, split-dsi, or secondary panel, dual-dsi, topology config */
		if (strlen(slave_cfg_name) > 0) {
			strlcpy(pbuf, slave_cfg_name, buf_size);
			pbuf += strlen(slave_cfg_name);
			buf_size -= strlen(slave_cfg_name);
		}

		strlcpy(pbuf, DSI_CFG_STRING, buf_size);
		pbuf += DSI_CFG_STRING_LEN;
		buf_size -= DSI_CFG_STRING_LEN;

		strlcpy(pbuf, oem_data.dsi_config, buf_size);
		pbuf += strlen(oem_data.dsi_config);
		buf_size -= strlen(oem_data.dsi_config);

		if (pll_src_string) {
			strlcpy(pbuf, pll_src_string, buf_size);
			pbuf += strlen(pll_src_string);
			buf_size -= strlen(pll_src_string);
		}

		if (sim_mode_string) {
			strlcpy(pbuf, LK_SIM_OVERRIDE, buf_size);
			pbuf += LK_SIM_OVERRIDE_LEN;
			buf_size -= LK_SIM_OVERRIDE_LEN;

			strlcpy(pbuf, sim_mode_string, buf_size);
			pbuf += strlen(sim_mode_string);
			buf_size -= strlen(sim_mode_string);
		}

		dprintf(INFO, "display kernel cmdline:%s\n",
			display_cmd_line);
	}
	return ret;
}
