/*
 * * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#include <debug.h>
#include <reg.h>
#include <mipi_dsi.h>
#include <mdp4.h>
#include <dev/pm8921.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <target/display.h>

void panel_backlight_on(void)
{
    struct pm8921_gpio backlight_pwm = {
        .direction = PM_GPIO_DIR_OUT,
        .output_buffer = 0,
        .output_value = 0,
        .pull = PM_GPIO_PULL_NO,
        .vin_sel = 2,
        .out_strength = PM_GPIO_STRENGTH_HIGH,
        .function = PM_GPIO_FUNC_1,
        .inv_int_pol = 0,
    };

    int rc = pm8921_gpio_config(GPIO_24, &backlight_pwm);
    if (rc) {
        dprintf(CRITICAL, "FAIL pm8921_gpio_config(): rc=%d.\n", rc);
     }
}

/* Pull DISP_RST_N high to get panel out of reset */
void mipi_panel_reset(void){

    struct pm8921_gpio gpio43_param = {
        .direction = PM_GPIO_DIR_OUT,
        .output_buffer = 0,
        .output_value = 1,
        .pull = PM_GPIO_PULL_UP0,
        .vin_sel = 2,
        .out_strength = PM_GPIO_STRENGTH_HIGH,
        .function = PM_GPIO_FUNC_PAIRED,
        .inv_int_pol = 0,
        .disable_pin = 0,
    };
    pm8921_gpio_config(GPIO_43, &gpio43_param);
}

void mipi_dsi_panel_power_on(void){

    /* Turn on LDO8 for lcd1 mipi vdd */
    pm8921_ldo_set_voltage(LDO_8, LDO_VOLTAGE_3_0V);

    /* Turn on LDO23 for lcd1 mipi vddio */
    pm8921_ldo_set_voltage(LDO_23, LDO_VOLTAGE_1_8V);

    /* Turn on LDO2 for vdda_mipi_dsi */
    pm8921_ldo_set_voltage(LDO_2, LDO_VOLTAGE_1_2V);

}

static void mipi_dsi_calibration(void)
{
    int32_t i = 0;
    uint32_t term_cnt = 5000;
    int32_t cal_busy = readl(MIPI_DSI_BASE + 0x550);

    /* DSI1_DSIPHY_REGULATOR_CAL_PWR_CFG */
    writel(0x01, MIPI_DSI_BASE + 0x0518);

    /* DSI1_DSIPHY_CAL_SW_CFG2 */
    writel(0x0, MIPI_DSI_BASE + 0x0534);
    /* DSI1_DSIPHY_CAL_HW_CFG1 */
    writel(0x5a, MIPI_DSI_BASE + 0x053c);
    /* DSI1_DSIPHY_CAL_HW_CFG3 */
    writel(0x10, MIPI_DSI_BASE + 0x0544);
    /* DSI1_DSIPHY_CAL_HW_CFG4 */
    writel(0x01, MIPI_DSI_BASE + 0x0548);
    /* DSI1_DSIPHY_CAL_HW_CFG0 */
    writel(0x01, MIPI_DSI_BASE + 0x0538);

    /* DSI1_DSIPHY_CAL_HW_TRIGGER */
    writel(0x01, MIPI_DSI_BASE + 0x0528);

    /* DSI1_DSIPHY_CAL_HW_TRIGGER */
    writel(0x00, MIPI_DSI_BASE + 0x0528);

    cal_busy = readl(MIPI_DSI_BASE + 0x550);
    while (cal_busy & 0x10) {
        i++;
        if (i > term_cnt) {
                dprintf(CRITICAL, "DSI1 PHY REGULATOR NOT READY,"
                "exceeded polling TIMEOUT!\n");
            break;
        }
        cal_busy = readl(MIPI_DSI_BASE + 0x550);
    }
}

int mipi_dsi_phy_init(struct mipi_dsi_panel_config *pinfo){
        struct mipi_dsi_phy_ctrl *pd;
    uint32_t i, off = 0;

    writel(0x0001, MIPI_DSI_BASE + 0x128);/* start phy sw reset */
    writel(0x0000, MIPI_DSI_BASE + 0x128);/* end phy w reset */
    writel(0x0003, MIPI_DSI_BASE + 0x500);/* regulator_ctrl_0 */
    writel(0x0001, MIPI_DSI_BASE + 0x504);/* regulator_ctrl_1 */
    writel(0x0001, MIPI_DSI_BASE + 0x508);/* regulator_ctrl_2 */
    writel(0x0000, MIPI_DSI_BASE + 0x50c);/* regulator_ctrl_3 */
    writel(0x0100, MIPI_DSI_BASE + 0x510);/* regulator_ctrl_4 */

    pd = (pinfo->dsi_phy_config);

    off = 0x0480;    /* strength 0 - 2 */
    for (i = 0; i < 3; i++) {
            writel(pd->strength[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    off = 0x0470;    /* ctrl 0 - 3 */
    for (i = 0; i < 4; i++) {
            writel(pd->ctrl[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    off = 0x0500;    /* regulator ctrl 0 - 4 */
    for (i = 0; i < 5; i++) {
            writel(pd->regulator[i], MIPI_DSI_BASE + off);
        off += 4;
    }
    mipi_dsi_calibration();

    off = 0x0204;    /* pll ctrl 1 - 19, skip 0 */
    for (i = 1; i < 20; i++) {
            writel(pd->pll[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    /* pll ctrl 0 */
    writel(pd->pll[0], MIPI_DSI_BASE + 0x200);
    writel((pd->pll[0] | 0x01), MIPI_DSI_BASE + 0x200);

    /* Check that PHY is ready */
    while(!(readl(DSIPHY_PLL_RDY) & 0x01));

    writel(0x202D, DSI_CLKOUT_TIMING_CTRL);

    off = 0x0440;    /* phy timing ctrl 0 - 11 */
    for (i = 0; i < 12; i++) {
      writel(pd->timing[i], MIPI_DSI_BASE + off);
        off += 4;
    }
    return 0;
}

/* Calculations specific for the Toshiba MDT61 Panel */
void mdp_setup_mdt61_video_dsi_config(void){
    unsigned long hsync_period;
    unsigned long vsync_period;

    /* For MDT61 display width is 200 more than image width */
    hsync_period = TSH_MDT61_DISPLAY_WIDTH + MIPI_HSYNC_BACK_PORCH_DCLK
                   + MIPI_HSYNC_FRONT_PORCH_DCLK + MIPI_HSYNC_PULSE_WIDTH;

    vsync_period = (TSH_MDT61_MIPI_FB_HEIGHT + MIPI_VSYNC_BACK_PORCH_LINES
            + MIPI_VSYNC_FRONT_PORCH_LINES + MIPI_VSYNC_PULSE_WIDTH) * hsync_period;

    writel(hsync_period<<16 | MIPI_HSYNC_PULSE_WIDTH<<0, MDP_DSI_VIDEO_HSYNC_CTL);
    writel(vsync_period, MDP_DSI_VIDEO_VSYNC_PERIOD);
    writel(MIPI_VSYNC_PULSE_WIDTH * hsync_period, MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH);
    writel((MIPI_HSYNC_BACK_PORCH_DCLK + MIPI_HSYNC_PULSE_WIDTH + TSH_MDT61_DISPLAY_WIDTH - 1)<<16 |
       (MIPI_HSYNC_BACK_PORCH_DCLK + MIPI_HSYNC_PULSE_WIDTH), MDP_DSI_VIDEO_DISPLAY_HCTL);
    writel(hsync_period * (MIPI_VSYNC_BACK_PORCH_LINES + MIPI_VSYNC_PULSE_WIDTH),
       MDP_DSI_VIDEO_DISPLAY_V_START);
    writel(hsync_period *
       (MIPI_VSYNC_BACK_PORCH_LINES + MIPI_VSYNC_PULSE_WIDTH + TSH_MDT61_MIPI_FB_HEIGHT) - 1,
       MDP_DSI_VIDEO_DISPLAY_V_END);

    /* Special Case for Toshiba 4in Panel */
    writel((1<<31) |
       (MIPI_HSYNC_BACK_PORCH_DCLK + MIPI_HSYNC_PULSE_WIDTH + TSH_MDT61_MIPI_FB_WIDTH - 1)<<16 |
       MIPI_HSYNC_PULSE_WIDTH + MIPI_HSYNC_BACK_PORCH_DCLK, MDP_DSI_VIDEO_ACTIVE_HCTL);
    writel(1<<31 | hsync_period * (MIPI_VSYNC_BACK_PORCH_LINES + MIPI_VSYNC_PULSE_WIDTH),
       MDP_DSI_VIDEO_DISPLAY_V_START);
    writel((TSH_MDT61_MIPI_FB_HEIGHT + MIPI_VSYNC_BACK_PORCH_LINES + MIPI_VSYNC_PULSE_WIDTH) *
       hsync_period - 1, MDP_DSI_VIDEO_DISPLAY_V_END);

    writel(0x00000001, MDP_DSI_VIDEO_EN);   // MDP_DSI_EN ENABLE
}

void config_mdt61_dsi_video_mode(void)
{

    unsigned char dst_format = 3; /* RGB888 */
    unsigned char traffic_mode = 1; /* non burst mode with sync start events */
    unsigned char lane_en = 7; /* 3 Lanes -- Enables Data Lane0, 1, 2 */
    unsigned long low_pwr_stop_mode = 0;
    unsigned char eof_bllp_pwr = 0x8; /* Needed or else will have blank line at top of display */
    unsigned char interleav = 0;

    dprintf(SPEW, "DSI_Video_Mode - Dst Format: RGB888\n");
    dprintf(SPEW, "Data Lane: 3 lane\n");
    dprintf(SPEW, "Traffic mode: non burst mode with sync start events\n");

    writel(0x00000000, MDP_DSI_VIDEO_EN);

    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000002, DSI_CLK_CTRL);
    writel(0x00000006, DSI_CLK_CTRL);
    writel(0x0000000e, DSI_CLK_CTRL);
    writel(0x0000001e, DSI_CLK_CTRL);
    writel(0x0000003e, DSI_CLK_CTRL);

    writel(0, DSI_CTRL);

    writel(0, DSI_ERR_INT_MASK0);

    writel(0x02020202, DSI_INT_CTRL);

    writel((MIPI_HSYNC_PULSE_WIDTH + MIPI_HSYNC_BACK_PORCH_DCLK + TSH_MDT61_DISPLAY_WIDTH)<<16 |
       (MIPI_HSYNC_PULSE_WIDTH + MIPI_HSYNC_BACK_PORCH_DCLK), DSI_VIDEO_MODE_ACTIVE_H);

    writel((MIPI_VSYNC_PULSE_WIDTH + MIPI_VSYNC_BACK_PORCH_LINES + TSH_MDT61_MIPI_FB_HEIGHT)<<16 |
       (MIPI_VSYNC_PULSE_WIDTH + MIPI_VSYNC_BACK_PORCH_LINES), DSI_VIDEO_MODE_ACTIVE_V);

    writel((MIPI_VSYNC_PULSE_WIDTH + MIPI_VSYNC_BACK_PORCH_LINES + TSH_MDT61_MIPI_FB_HEIGHT +
        MIPI_VSYNC_FRONT_PORCH_LINES - 1)<<16 |
       (MIPI_HSYNC_PULSE_WIDTH + MIPI_HSYNC_BACK_PORCH_DCLK + TSH_MDT61_DISPLAY_WIDTH +
        MIPI_HSYNC_FRONT_PORCH_DCLK - 1), DSI_VIDEO_MODE_TOTAL);

    writel((MIPI_HSYNC_PULSE_WIDTH)<<16 | 0, DSI_VIDEO_MODE_HSYNC);

    writel(0<<16 | 0, DSI_VIDEO_MODE_VSYNC);

    writel(MIPI_VSYNC_PULSE_WIDTH<<16 | 0, DSI_VIDEO_MODE_VSYNC_VPOS);

    writel(1, DSI_EOT_PACKET_CTRL);

    writel(low_pwr_stop_mode << 16 | eof_bllp_pwr << 12 | traffic_mode << 8
           | dst_format << 4 | 0x0, DSI_VIDEO_MODE_CTRL);

    writel(0x00010100, DSI_INT_CTRL);
    writel(0x02010202, DSI_INT_CTRL);
    writel(0x02030303, DSI_INT_CTRL);

    writel(interleav << 30 | 0 << 24 | 0 << 20 | lane_en << 4
           | 0x103, DSI_CTRL);
}
