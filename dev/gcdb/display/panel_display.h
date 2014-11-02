/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#ifndef _PANEL_DISPLAY_H_
#define _PANEL_DISPLAY_H_

/*---------------------------------------------------------------------------*/
/* MACRO definition                                                          */
/*---------------------------------------------------------------------------*/

#define BPP_16 16
#define BPP_18 18
#define BPP_24 24

#define TIMING_SIZE 48
#define REGULATOR_SIZE 28

#define DUAL_DSI_FLAG 0x1
#define DUAL_PIPE_FLAG 0x2
#define PIPE_SWAP_FLAG 0x4
#define SPLIT_DISPLAY_FLAG 0x8
#define DST_SPLIT_FLAG 0x10

#define MAX_PANEL_ID_LEN 64
#include "panel.h"
/*---------------------------------------------------------------------------*/
/* struct definition                                                         */
/*---------------------------------------------------------------------------*/
typedef struct panel_struct{
	struct panel_config         *paneldata;
	struct panel_resolution     *panelres;
	struct color_info           *color;
	struct videopanel_info      *videopanel;
	struct commandpanel_info    *commandpanel;
	struct command_state        *state;
	struct lane_configuration   *laneconfig;
	struct panel_timing         *paneltiminginfo;
	struct panel_reset_sequence *panelresetseq;
	struct backlight            *backlightinfo;
	struct fb_compression	    fbcinfo;
};

struct panel_list {
	char name[MAX_PANEL_ID_LEN];
	uint32_t id;
};

/*---------------------------------------------------------------------------*/
/* API                                                                       */
/*---------------------------------------------------------------------------*/
int dsi_panel_init(struct msm_panel_info *pinfo,
		struct panel_struct *pstruct);

int dsi_panel_pre_on();
int dsi_panel_post_on();
int dsi_panel_pre_off();
int dsi_panel_post_off();
int dsi_panel_rotation();
int dsi_panel_config(void *);

/* This should be implemented by oem */
int oem_panel_rotation();
int oem_panel_on();
int oem_panel_off();

/* OEM support API */
int32_t panel_name_to_id(struct panel_list supp_panels[],
	uint32_t supp_panels_size, const char *panel_name);
#endif /*_PLATFORM_DISPLAY_H_ */
