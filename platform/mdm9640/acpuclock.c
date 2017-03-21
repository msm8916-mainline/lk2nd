/*
 * Copyright (c) 2013-2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Linux Foundation nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <assert.h>
#include <debug.h>
#include <reg.h>
#include <clock.h>
#include <mmc.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <platform.h>
#include <rpm-smd.h>
#include <regulator.h>

#define RPM_CE_CLK_TYPE    0x6563
#define CE1_CLK_ID         0x0
#define RPM_SMD_KEY_RATE   0x007A484B

uint32_t CE1_CLK[][8]=
{
    {
        RPM_CE_CLK_TYPE, CE1_CLK_ID,
        KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
        RPM_SMD_KEY_RATE, 4, 0,
    },
    {
        RPM_CE_CLK_TYPE, CE1_CLK_ID,
        KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
        RPM_SMD_KEY_RATE, 4, 171430, /* clk rate in KHZ */
    },
};

void clock_config_uart_dm(uint8_t id)
{
	int ret;
	char clk_name[64];

	snprintf(clk_name, sizeof(clk_name), "uart%u_iface_clk", id);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if (ret)
	{
		dprintf(CRITICAL, "failed to set %s ret = %d\n", clk_name, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "uart%u_core_clk", id);
	ret = clk_get_set_enable(clk_name, 7372800, 1);
	if (ret)
	{
		dprintf(CRITICAL, "failed to set %s ret = %d\n", clk_name, ret);
		ASSERT(0);
	}
}

/*
 * Disable power collapse using GDSCR:
 * Globally Distributed Switch Controller Register
 */
void clock_usb30_gdsc_enable(void)
{
	uint32_t reg = readl(GCC_USB30_GDSCR);

	reg &= ~(0x1);

	writel(reg, GCC_USB30_GDSCR);
}

/* enables usb30 clocks */
void clock_usb30_init(void)
{
	int ret;

	ret = clk_get_set_enable("usb30_iface_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_iface_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	clock_usb30_gdsc_enable();

	if (platform_is_sdx20())
		ret = clk_get_set_enable("usb30_master_clk_sdx20", 200000000, 1);
	else
		ret = clk_get_set_enable("usb30_master_clk", 125000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_master_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	if (platform_is_mdm9650())
		ret = clk_get_set_enable("usb30_pipe_clk_mdm9650", 0, 1);
	else if (platform_is_sdx20())
		ret = clk_get_set_enable("usb30_pipe_clk_sdx20", 0, 1);
	else
		ret = clk_get_set_enable("usb30_pipe_clk", 19200000, 1);

	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_pipe_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb30_aux_clk", 1000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_aux_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	if (platform_is_sdx20())
		ret = clk_get_set_enable("usb30_mock_utmi_clk_sdx20", 19200000, 1);
	else
		ret = clk_get_set_enable("usb30_mock_utmi_clk", 60000000, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_mock_utmi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb30_sleep_clk", 0, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_sleep_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb_phy_cfg_ahb_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb_phy_cfg_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void clock_init_mmc(uint32_t interface)
{
	char clk_name[64];
	int ret;

	snprintf(clk_name, sizeof(clk_name), "sdc%u_iface_clk", interface);

	/* enable interface clock */
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set sdc1_iface_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

/* Configure MMC clock */
void clock_config_mmc(uint32_t interface, uint32_t freq)
{
	int ret = 0;
	char clk_name[64];

	if(platform_is_sdx20())
		snprintf(clk_name, sizeof(clk_name), "sdc%u_core_clk_sdx20", interface);
	else
		snprintf(clk_name, sizeof(clk_name), "sdc%u_core_clk", interface);

	if(freq == MMC_CLK_400KHZ)
	{
		ret = clk_get_set_enable(clk_name, 400000, 1);
	}
	else if(freq == MMC_CLK_50MHZ)
	{
		ret = clk_get_set_enable(clk_name, 50000000, 1);
	}
	else if(freq == MMC_CLK_200MHZ)
	{
		ret = clk_get_set_enable(clk_name, 200000000, 1);
	}
	else if(freq == MMC_CLK_171MHZ)
	{
		ret = clk_get_set_enable(clk_name, 171430000, 1);
	}
	else
	{
		dprintf(CRITICAL, "sdc frequency (%u) is not supported\n", freq);
		ASSERT(0);
	}

	if(ret)
	{
		dprintf(CRITICAL, "failed to set %s ret = %d\n", clk_name, ret);
		ASSERT(0);
	}
}

void clock_bumpup_pipe3_clk()
{
	int ret =0;

	if (platform_is_mdm9650())
		ret = clk_get_set_enable("usb30_pipe_clk", 0, true);
	else
		ret = clk_get_set_enable("usb30_pipe_clk", 125000000, true);

	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_pipe_clk. ret = %d\n", ret);
		ASSERT(0);
	}
}

/*
 * This is the clock reset function for USB3
 */
void clock_reset_usb_phy()
{
	int ret;

	struct clk *phy_reset_clk = NULL;
	struct clk *pipe_reset_clk = NULL;
	struct clk *master_clk = NULL;

	master_clk = clk_get("usb30_master_clk");
	ASSERT(master_clk);

	/* Look if phy com clock is present */
	phy_reset_clk = clk_get("usb30_phy_reset");
	ASSERT(phy_reset_clk);

	if(platform_is_sdx20()){
		pipe_reset_clk = clk_get("usb30_pipe_clk_sdx20");
		ASSERT(pipe_reset_clk);
	}
	else{
		pipe_reset_clk = clk_get("usb30_pipe_clk");
		ASSERT(pipe_reset_clk);
	}

	/* ASSERT */
	ret = clk_reset(master_clk, CLK_RESET_ASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to assert usb30_master_reset clk\n");
		return;
	}
	ret = clk_reset(phy_reset_clk, CLK_RESET_ASSERT);

	if (ret)
	{
		dprintf(CRITICAL, "Failed to assert usb30_phy_reset clk\n");
		goto deassert_master_clk;
	}

	ret = clk_reset(pipe_reset_clk, CLK_RESET_ASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to assert usb30_pipe_clk\n");
		goto deassert_phy_clk;
	}

	udelay(100);

	/* DEASSERT */
	ret = clk_reset(pipe_reset_clk, CLK_RESET_DEASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to deassert usb_pipe_clk\n");
		return;
	}

deassert_phy_clk:

	ret = clk_reset(phy_reset_clk, CLK_RESET_DEASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to deassert usb30_phy_com_reset clk\n");
		return;
	}
deassert_master_clk:

	ret = clk_reset(master_clk, CLK_RESET_DEASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to deassert usb30_master clk\n");
		return;
	}

}

void clock_ce_enable(uint8_t instance)
{
	int ret;
	char clk_name[64];

	if (platform_is_mdm9650())
	{
		if (instance == 1)
			rpm_send_data(&CE1_CLK[GENERIC_ENABLE][0], 24, RPM_REQUEST_TYPE);
		else
		{
			dprintf(CRITICAL, "Unsupported CE instance\n");
			ASSERT(0);
		}
		return;
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_src_clk", instance);
	ret = clk_get_set_enable(clk_name, 160000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce%u_src_clk ret = %d\n", instance, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_core_clk", instance);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce%u_core_clk ret = %d\n", instance, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_ahb_clk", instance);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce%u_ahb_clk ret = %d\n", instance, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_axi_clk", instance);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce%u_axi_clk ret = %d\n", instance, ret);
		ASSERT(0);
	}

	/* Wait for 48 * #pipes cycles.
	* This is necessary as immediately after an access control reset (boot up)
	* or a debug re-enable, the Crypto core sequentially clears its internal
	* pipe key storage memory. If pipe key initialization writes are attempted
	* during this time, they may be overwritten by the internal clearing logic.
	*/
	udelay(1);
}

void clock_ce_disable(uint8_t instance)
{
	struct clk *ahb_clk;
	struct clk *cclk;
	struct clk *axi_clk;
	struct clk *src_clk;
	char clk_name[64];

	if (platform_is_mdm9650())
	{
		if (instance == 1)
		rpm_send_data(&CE1_CLK[GENERIC_DISABLE][0], 24, RPM_REQUEST_TYPE);
		else
		{
			dprintf(CRITICAL, "Unsupported CE instance\n");
			ASSERT(0);
		}
		return;
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_src_clk", instance);
	src_clk = clk_get(clk_name);

	snprintf(clk_name, sizeof(clk_name), "ce%u_ahb_clk", instance);
	ahb_clk = clk_get(clk_name);

	snprintf(clk_name, sizeof(clk_name), "ce%u_axi_clk", instance);
	axi_clk = clk_get(clk_name);

	snprintf(clk_name, sizeof(clk_name), "ce%u_core_clk", instance);
	cclk    = clk_get(clk_name);

	clk_disable(ahb_clk);
	clk_disable(axi_clk);
	clk_disable(cclk);
	clk_disable(src_clk);

	/* Some delay for the clocks to stabalize. */
	udelay(1);
}
/* Function to asynchronously reset CE.
 * Function assumes that all the CE clocks are off.
 */
static void ce_async_reset(uint8_t instance)
{
	/* Start the block reset for CE */
	writel(1, GCC_CRYPTO_BCR);

	udelay(2);

	/* Take CE block out of reset */
	writel(0, GCC_CRYPTO_BCR);

	udelay(2);
}

void clock_config_ce(uint8_t instance)
{
	/* Need to enable the clock before disabling since the clk_disable()
	 * has a check to default to nop when the clk_enable() is not called
	 * on that particular clock.
	 */
	clock_ce_enable(instance);

	clock_ce_disable(instance);

	ce_async_reset(instance);

	clock_ce_enable(instance);
}
