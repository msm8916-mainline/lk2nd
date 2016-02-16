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

#ifndef _FASTBOOT_OEM_DISPLAY_H_
#define _FASTBOOT_OEM_DISPLAY_H_

#define SIM_OVERRIDE_LEN 10

enum {
    SIM_NONE,
    SIM_MODE,
    SIM_SWTE,
    SIM_HWTE,
};

struct panel_lookup_list {
	char name[MAX_PANEL_ID_LEN];
	char panel_dt_string[MAX_PANEL_ID_LEN];
	bool is_split_dsi;
};

struct sim_lookup_list {
	uint32_t sim_mode;
	char override_string[SIM_OVERRIDE_LEN];
};

/*---------------------------------------------------------------------------*/
/* Lookup table for skip panels                                              */
/*---------------------------------------------------------------------------*/

struct panel_lookup_list lookup_skip_panels[] = {
	{"adv7533_1080p_video", "qcom,mdss_dsi_adv7533_1080p60_video", false},
	{"adv7533_720p_video", "qcom,mdss_dsi_adv7533_720p60_video", false},
	{"auo_qvga_cmd", "qcom,mdss_dsi_auo_qvga_cmd", false},
	{"generic_720p_cmd", "qcom,mdss_dsi_generic_720p_cmd", false},
	{"hx8279a_wsvga_video", "qcom,mdss_dsi_hx8279a_wsvga_video", false},
	{"hx8379a_wvga_video", "qcom,mdss_dsi_hx8379a_fwvga_video", false},
	{"hx8379a_truly_fwvga_video", "qcom,mdss_dsi_hx8379a_truly_fwvga_video", false},
	{"hx8379a_wvga_video", "qcom,mdss_dsi_hx8379a_wvga_video", false},
	{"hx8379c_fwvga_video", "qcom,mdss_dsi_hx8379c_fwvga_video", false},
	{"hx8389b_qhd_video", "qcom,mdss_dsi_hx8389b_qhd_video", false},
	{"hx8394a_720p_video", "qcom,mdss_dsi_hx8394a_720p_video", false},
	{"hx8394d_720p_video", "qcom,mdss_dsi_hx8394d_720p_video", false},
	{"hx8394d_qhd_video", "qcom,mdss_dsi_hx8394d_qhd_video", false},
	{"ili9806e_fwvga_video", "qcom,mdss_dsi_ili9806e_fwvga_video", false},
	{"innolux_720p_video", "qcom,mdss_dsi_innolux_720p_video", false},
	{"jdi_4k_dualdsi_video", "qcom,dsi_jdi_4k_video", true},
	{"jdi_1080p_video", "qcom,mdss_dsi_jdi_1080p_video", false},
	{"jdi_a216_fhd_video", "qcom,mdss_dsi_jdi_a216_fhd_video", false},
	{"jdi_fhd_video", "qcom,mdss_dsi_jdi_fhd_video", false},
	{"jdi_qhd_dualdsi_cmd", "qcom,mdss_dsi_jdi_qhd_dualmipi_cmd", true},
	{"jdi_qhd_dualdsi_video", "qcom,dsi_jdi_qhd_video", true},
	{"jdi_4k_dualdsi_video_nofbc", "qcom,dsi_jdi_4k_nofbc_video", true},
	{"nt35521_720p_video", "qcom,mdss_dsi_nt35521_720p_video", false},
	{"nt35521_wxga_video", "qcom,mdss_dsi_nt35521_wxga_video", false},
	{"nt35590_720p_cmd", "qcom,mdss_dsi_nt35590_720p_cmd", false},
	{"nt35590_720p_video", "qcom,mdss_dsi_nt35590_720p_video", false},
	{"nt35590_qvga_cmd", "qcom,mdss_dsi_nt35590_qvga_cmd", false},
	{"nt35596_1080p_video", "qcom,mdss_dsi_nt35596_1080p_skuk_video", false},
	{"nt35597_wqxga_cmd", "qcom,dsi_nt35597_wqxga_cmd", false},
	{"nt35597_wqxga_video", "qcom,dsi_nt35597_wqxga_video", false},
	{"nt35597_wqxga_dualdsi_video", "qcom,mdss_dsi_nt35597_wqxga_video", true},
	{"otm1283a_720p_video", "qcom,mdss_dsi_otm1283a_720p_video", false},
	{"otm1906c_1080p_cmd", "qcom,mdss_dsi_otm1906c_1080p_cmd", false},
	{"otm8018b_fwvga_video", "qcom,mdss_dsi_otm8018b_fwvga_video", false},
	{"otm8019a_fwvga_video", "qcom,mdss_dsi_otm8019a_fwvga_video", false},
	{"r61318_hd_video", "qcom,mdss_dsi_r61318_hd_video", false},
	{"r63417_1080p_video", "qcom,mdss_dsi_r63417_1080p_video", false},
	{"samsung_wxga_video", "qcom,mdss_dsi_samsung_wxga_video", false},
	{"sharp_1080p_cmd", "qcom,mdss_dsi_sharp_1080p_cmd", false},
	{"sharp_4k_dsc_video", "qcom,mdss_dsi_sharp_4k_dsc_video", true},
	{"sharp_4k_dsc_cmd", "qcom,mdss_dsi_sharp_4k_dsc_cmd", true},
	{"sharp_qhd_video", "qcom,mdss_dsi_sharp_qhd_video", false},
	{"sharp_wqxga_dualdsi_video", "qcom,mdss_dsi_sharp_wqxga_video", true},
	{"ssd2080m_720p_video", "qcom,mdss_dsi_ssd2080m_720p_video", false},
	{"toshiba_720p_video", "qcom,mdss_dsi_toshiba_720p_video", false},
	{"truly_1080p_cmd", "qcom,mdss_dsi_truly_1080p_cmd", false},
	{"truly_1080p_video", "qcom,mdss_dsi_truly_1080p_video", false},
	{"truly_wvga_cmd", "qcom,mdss_dsi_truly_wvga_cmd", false},
	{"truly_wvga_video", "qcom,mdss_dsi_truly_wvga_video", false},
	{"adv16", "qcom,mdss_dsi_adv7533_1080p", false},
	{"adv4", "qcom,mdss_dsi_adv7533_720p", false},
	{"nt35950_4k_dsc_cmd", "qcom,mdss_dsi_nt35950_4k_dsc_cmd", true},
	{"sharp_1080p_cmd", "qcom,mdss_dsi_sharp_1080p_cmd", false},
	{"sharp_120hz_1080p_cmd", "qcom,mdss_dual_sharp_1080p_120hz_cmd",
		true},
};

struct sim_lookup_list lookup_sim[] = {
	{SIM_MODE, "sim"},
	{SIM_SWTE, "sim-swte"},
	{SIM_HWTE, "sim-hwte"},
};

#endif /*_FASTBOOT_OEM_DISPLAY_H_ */
