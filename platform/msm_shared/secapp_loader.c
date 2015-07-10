/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
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

#include <stdlib.h>
#include <string.h>

#include <secapp_loader.h>
#include <qseecom_lk_api.h>

static bool lksec_app_loaded;
static int app_handle;

int load_sec_app()
{
	/* start TZ app */
	app_handle = qseecom_start_app("keymaster");

	if (app_handle <= 0)
	{
		dprintf(CRITICAL, "Failure to load TZ app: lksecapp, error: %d\n", app_handle);
		return -1;
	}
	lksec_app_loaded = true;
	return 0;
}

int get_secapp_handle()
{
	dprintf(INFO, "LK SEC APP Handle: 0x%x\n", app_handle);
	return app_handle;
}

int send_delete_keys_to_tz()
{
	int ret = 0;
	key_op_delete_all_req_t req = {0};
	key_op_delete_all_rsp_t rsp = {0};
	req.cmd_id = KEYMASTER_DELETE_ALL_KEYS;

	// send delete all keys command
	ret = qseecom_send_command(app_handle, (void *)&req, sizeof(req), (void *)&rsp, sizeof(rsp));

	if (ret < 0 || rsp.status < 0)
	{
		dprintf(CRITICAL, "Failed to send delete keys command: Error: %x\n", rsp.status);
		return -1;
	}

	return 0;
}

int send_milestone_call_to_tz()
{
	int ret = 0;

	km_set_milestone_req_t req = {0};
	km_set_milestone_rsp_t rsp = {0};

	req.cmd_id = KEYMASTER_MILESTONE_CALL;

	/* Milestone end command */
	ret = qseecom_send_command(app_handle, (void *)&req, sizeof(req), (void *)&rsp, sizeof(rsp));

	if (ret < 0 || rsp.status < 0)
	{
		dprintf(CRITICAL, "Failed to send milestone end command: Error: %x\n", rsp.status);
		return -1;
	}

	return 0;
}

bool is_sec_app_loaded()
{
	return lksec_app_loaded;
}
