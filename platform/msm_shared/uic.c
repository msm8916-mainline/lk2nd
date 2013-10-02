/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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
#include <malloc.h>
#include <qgic.h>
#include <ufs_hw.h>
#include <ufs_hci.h>
#include <uic.h>
#include <dme.h>
#include <platform/iomap.h>
#include <platform/interrupts.h>
#include <platform/clock.h>
#include <platform/timer.h>

static int uic_check_hci_ucrdy(struct ufs_dev *dev)
{
	uint32_t ret;

	/* Check if the HCS.UCRDY is set to ‘1’. */
	if (ufs_reg_target_val_timeout_loop(UFS_HCS(dev->base), UFS_HCS_UCRDY, HCI_UIC_TIMEOUT))
	{
		ret = -UIC_ERR_TIMEOUT;
	}

	return ret;
}

static void uic_cmd_setup_args(struct ufs_dev *dev, struct uic_cmd *cmd)
{
	if (cmd->num_args & UICCMD_ONE_ARGS)
		writel(cmd->uiccmdarg1, UFS_UICCMDARG1(dev->base));

	if (cmd->num_args & UICCMD_TWO_ARGS)
		writel(cmd->uiccmdarg2, UFS_UICCMDARG2(dev->base));

	if (cmd->num_args & UICCMD_THREE_ARGS)
		writel(cmd->uiccmdarg3, UFS_UICCMDARG3(dev->base));
}

static void uic_cmd_save_result(struct ufs_dev *dev, struct uic_cmd *cmd)
{
	/* Save cmd arg 2 and 3 for use of the commands that require it. */
	cmd->uiccmdarg3 = readl(UFS_UICCMDARG3(dev->base));
	cmd->uiccmdarg2 = readl(UFS_UICCMDARG2(dev->base));
}

int uic_send_cmd(struct ufs_dev *dev, struct uic_cmd *cmd)
{
	uint32_t val;
	int ret = 0;

	ret = mutex_acquire(&(dev->uic_data.uic_mutex));
	if (ret)
	{
		dprintf(CRITICAL, "Mutex acquire failed.\n");
		ret = -UFS_FAILURE;
		goto uic_send_cmd_err;
	}

	ret = uic_check_hci_ucrdy(dev);
	if (ret)
	{
		dprintf(CRITICAL, "Check UCRDY failed.\n");
		ret = -UFS_FAILURE;
		goto uic_send_cmd_err;
	}

	/* Write arguments, if any. */
	uic_cmd_setup_args(dev, cmd);

	event_init(&(dev->uic_data.uic_event), false, 0);

	/* Execute the cmd. */
	writel(cmd->uiccmd, UFS_UICCMD(dev->base));

	ret = event_wait_timeout(&(dev->uic_data.uic_event), cmd->timeout_msecs);
	if (ret)
	{
		dprintf(CRITICAL, "Event wait failed.\n");
		ret = -UFS_FAILURE;
		goto uic_send_cmd_err;
	}

	/* Save Result. */
	uic_cmd_save_result(dev, cmd);

	ret = mutex_release(&(dev->uic_data.uic_mutex));
	if (ret)
	{
		dprintf(CRITICAL, "Mutex release failed.\n");
		ret = -UFS_FAILURE;
		goto uic_send_cmd_err;
	}

	val = cmd->uiccmdarg2;
	/* Read generic error code for the UIC command. */
	cmd->gen_err_code = val & 0xFF;

	return ret;

uic_send_cmd_err:
	dprintf(CRITICAL, "Failed to send the UIC cmd.\n");
	return ret;
}

static int uic_start_link(struct ufs_dev *dev)
{
	uint32_t try_again = 10;
	int      ret;

	do
	{
		try_again--;

		/* Send DME_LINKSTARTUP command to start the link startup procedure */
		ret = dme_send_linkstartup_req(dev);
		if (ret)
		{
			dprintf(CRITICAL, "DME LINKSTARTUP failed.\n");
			goto uic_start_link_err;
		}

		/* Check value of HCS.DP and make sure that there is a device attached to the Link */
		if (!(readl(UFS_HCS(dev->base)) & UFS_HCS_DP))
		{
			mdelay(1000);

			if (!(readl(UFS_IE(dev->base)) & BIT(7)))
				goto uic_start_link_err;
		}
		else
			break;
	} while (try_again);
	if (!try_again)
		ret = -UFS_FAILURE;

uic_start_link_err:
	return ret;
}

int uic_init(struct ufs_dev *dev)
{
	if (!(readl(UFS_HCE(dev->base)) & UFS_HCE_ENABLE))
	{
		dprintf(CRITICAL, "UFS link is not initialized.\n");
		return -UFS_FAILURE;
	}

	return UFS_SUCCESS;
}

