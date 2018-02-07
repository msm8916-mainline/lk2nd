/* Copyright (c) 2012-2015,2017-2018 The Linux Foundation. All rights reserved.
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

#define pr_fmt(fmt) "QSEECOM: %s: " fmt, __func__

#include <partition_parser.h>
#include <qseecom_lk.h>
#include <scm.h>
#include <qseecomi_lk.h>
#include "qseecom_lk_api.h"
#include <debug.h>
#include <kernel/mutex.h>
#include <malloc.h>
#include <stdlib.h>
#include <arch/defines.h>
#include <string.h>
#include <platform/iomap.h>
#include <platform.h>

#define QSEOS_CHECK_VERSION_CMD  0x00001803

#define MAX_SCM_ARGS 10
#define N_EXT_SCM_ARGS 7
#define FIRST_EXT_ARG_IDX 3
#define MAX_TA_PARTITION_SIZE 0x00100000
#define N_REGISTER_ARGS (MAX_SCM_ARGS - N_EXT_SCM_ARGS + 1)

#define QSEE_LOG_BUF_SIZE (4096)
#define GENERIC_ERROR -1
#define LISTENER_ALREADY_PRESENT_ERROR -2

#define TZ_FVER_QSEE 10
#define TZ_CALL 6
enum qseecom_client_handle_type {
	QSEECOM_CLIENT_APP = 1,
	QSEECOM_LISTENER_SERVICE,
	QSEECOM_SECURE_SERVICE,
	QSEECOM_GENERIC,
	QSEECOM_UNAVAILABLE_CLIENT_APP,
};

struct qseecom_registered_listener_list {
	struct list_node node;
	struct qseecom_register_listener_req svc;
	ListenerCallback CallbackFn;
};

struct qseecom_registered_app_list {
	struct list_node node;
	uint32_t  app_id;
	uint32_t  ref_cnt;
	char app_name[MAX_APP_NAME_SIZE];
	int  handle;
};

struct qseecom_control {
	struct list_node  registered_listener_list_head;
	mutex_t           registered_listener_list_lock;

	struct list_node  registered_app_list_head;
	mutex_t           registered_app_list_lock;

	uint32_t          qseos_version;
	uint32_t          qsee_version;
	int               handle;
	bool              commonlib_loaded;
	mutex_t           global_data_lock;
	uint32_t          cmnlib_loaded;
	uint32_t          qseecom_init_done;
	uint32_t          qseecom_tz_init_done;
};

struct qseecom_listener_handle {
	uint32_t               id;
};

static struct qseecom_reg_log_buf_ireq logbuf_req = {0};
static struct qseecom_control qseecom;
static int __qseecom_process_incomplete_cmd(struct qseecom_command_scm_resp *resp,
          struct qseecom_client_listener_data_irsp *send_data_rsp);

/*
 * Appsbl runs in Aarch32 when this is ported for Aarch64,
 * change return type for uint64_t.
 */
static uint32_t __qseecom_uvirt_to_kphys(uint64_t virt)
{
	dprintf(SPEW, "%s called\n", __func__);
	return (uint32_t)platform_get_virt_to_phys_mapping((addr_t)virt);
}

static int _disp_log_stats(struct tzdbg_log_t *log, uint32_t log_len,
		uint32_t startOffset, uint32_t endOffset)
{
	uint32_t MaxBufSize = 0;
	uint32_t LogBufSize = 0;
	uint32_t LogBufFirstHalf = 0;
	uint32_t len = 0;
	char *pCurPos, *pPrintPos = NULL;
	void *pLogBuf = NULL;
	int ret = GENERIC_ERROR;

	MaxBufSize = QSEE_LOG_BUF_SIZE - sizeof(struct tzdbg_log_pos_t);

	dprintf(SPEW, "%s called\n", __func__);
	if (startOffset < endOffset)
	{
		LogBufSize = endOffset - startOffset;
		pLogBuf = malloc(LogBufSize);
		if (NULL == pLogBuf)
		{
			ret = GENERIC_ERROR;
			dprintf(CRITICAL, "Failed to alloc buffer to print TZ Log:%u\n", LogBufSize);
			goto err;
		}
		memset(pLogBuf, 0, LogBufSize);
		memscpy(pLogBuf, LogBufSize, (char *)((uint32_t)log->log_buf + startOffset), LogBufSize);
	}
	else if ( endOffset < startOffset)
	{
		LogBufSize =  MaxBufSize - (startOffset - endOffset);
		LogBufFirstHalf = MaxBufSize - startOffset;
		pLogBuf = malloc(LogBufSize);
		if (NULL == pLogBuf)
		{
			ret = GENERIC_ERROR;
			dprintf(CRITICAL, "Failed to alloc buffer to print TZ Log:%u\n", LogBufSize);
			goto err;
		}
		memset(pLogBuf, 0, LogBufSize);
		memscpy(pLogBuf, LogBufSize, (char *)((uint32_t)log->log_buf + startOffset), LogBufFirstHalf);
		memscpy((char *)((uint32_t)pLogBuf+ LogBufFirstHalf), (LogBufSize - LogBufFirstHalf), log->log_buf, endOffset);
	}
	else //endOffset == startOffset
	{
		ret = 0;
		goto err;
	}

	/*
	 *  Read from ring buff while there is data and space in return buff
	 */
	pCurPos = pLogBuf;
	pPrintPos = pCurPos;
	while (len < LogBufSize)
	{
			//QSEE separate each line by "\r \n"
			if ((*pCurPos == '\r')&&(*(pCurPos+1) == '\n'))
			{
				//update the line to dump
				*pCurPos = '\0';
				len++;
				pCurPos++;
				*pCurPos = '\0';
				len++;
				pCurPos++;
				dprintf(ALWAYS, "%s\n", pPrintPos);
				pPrintPos = pCurPos;
				continue;
			}
			len++;
			pCurPos++;
	}
	ret = 0;
	free(pLogBuf);
err:
	return ret;
}

static int allocate_extra_arg_buffer(uint32_t fn_id, struct scm_desc *desc)
{
	int i;
	int ret = GENERIC_ERROR;
	scmcall_arg arg = {0};
	scmcall_ret ret_arg = {0};
	int arglen = 0;

	if (!desc) {
		dprintf(CRITICAL, "%s: Invalid input\n", __func__);
		return GENERIC_ERROR;
	}

	dprintf(SPEW, "%s called\n", __func__);
	arglen = desc->arginfo & 0xf;

	dprintf(SPEW, "%s:fn_id:%u, desc->arginfo:%u desc->args[0]:%u desc->args[1]:%u desc->args[2]:%u desc->args[3]:%u desc->args[4]:%u\n",
			__func__, fn_id, desc->arginfo, desc->args[0], desc->args[1], desc->args[2], desc->args[3], desc->args[4]);

	arg.x0 = fn_id;
	arg.x1 = desc->arginfo;
	arg.x2 = desc->args[0];
	arg.x3 = desc->args[1];
	arg.x4 = desc->args[2];

	if (arglen > FIRST_EXT_ARG_IDX) {
		for (i = 0; i < (arglen - FIRST_EXT_ARG_IDX); i++) {
			arg.x5[i] = desc->args[i + FIRST_EXT_ARG_IDX];
		}
	}
	ret = scm_call2(&arg, &ret_arg);
	desc->ret[0] = ret_arg.x1;
	desc->ret[1] = ret_arg.x2;
	desc->ret[2] = ret_arg.x3;

	dprintf(SPEW, "%s:ret:%d, desc->ret[0]]:%u desc->ret[1]:%u desc->ret[2]:%u\n",
			__func__, ret, desc->ret[0], desc->ret[1], desc->ret[2]);
	return ret;
}

static int qseecom_scm_call2(uint32_t svc_id, uint32_t tz_cmd_id,
			const void *req_buf, void *resp_buf)
{
	int      ret = 0;
	uint32_t smc_id = 0;
	uint32_t qseos_cmd_id = 0;
	struct scm_desc desc = {0};
	struct qseecom_command_scm_resp *scm_resp = NULL;

	if (!req_buf || !resp_buf) {
		dprintf(CRITICAL, "Invalid buffer pointer\n");
		return GENERIC_ERROR;
	}
	dprintf(SPEW, "%s called\n", __func__);

	qseos_cmd_id = *(uint32_t *)req_buf;
	scm_resp = (struct qseecom_command_scm_resp *)resp_buf;

	switch (svc_id) {
		case TZ_CALL: {
			if (tz_cmd_id == 1) {
				smc_id = TZ_INFO_IS_SVC_AVAILABLE_ID;
				desc.arginfo = TZ_INFO_IS_SVC_AVAILABLE_ID_PARAM_ID;
				desc.args[0] = TZ_INFO_GET_FEATURE_VERSION_ID;
			} else if (tz_cmd_id == 3) {
				smc_id = TZ_INFO_GET_FEATURE_VERSION_ID;
				desc.arginfo = TZ_INFO_GET_FEATURE_VERSION_ID_PARAM_ID;
				desc.args[0] = *(uint32_t *)req_buf;
			}
			ret = allocate_extra_arg_buffer(smc_id, &desc);
			break;
		}

		case SCM_SVC_TZSCHEDULER: {
			switch (qseos_cmd_id) {
				case QSEE_APP_START_COMMAND: {
					struct qseecom_load_app_ireq *req;
					req = (struct qseecom_load_app_ireq *)req_buf;
					smc_id = TZ_OS_APP_START_ID;
					desc.arginfo = TZ_OS_APP_START_ID_PARAM_ID;
					desc.args[0] = req->mdt_len;
					desc.args[1] = req->img_len;
					desc.args[2] = req->phy_addr;
					dprintf(SPEW, "args[0]:%u args[1]:%u args[2]:%u\n",
							desc.args[0], desc.args[1], desc.args[2]);
					dprintf(SPEW, "mdt_len:%u img_len:%u phy_addr:%u\n",
							req->mdt_len, req->img_len, req->phy_addr);
					ret = allocate_extra_arg_buffer(smc_id, &desc);

					break;
				}
				case QSEE_APP_SHUTDOWN_COMMAND: {
					struct qseecom_unload_app_ireq *req;
					req = (struct qseecom_unload_app_ireq *)req_buf;
					smc_id = TZ_OS_APP_SHUTDOWN_ID;
					desc.arginfo = TZ_OS_APP_SHUTDOWN_ID_PARAM_ID;
					desc.args[0] = req->app_id;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				case QSEE_APP_REGION_NOTIFICATION: {
					struct qsee_apps_region_info_ireq *req;
					req = (struct qsee_apps_region_info_ireq *)req_buf;
					smc_id = TZ_OS_APP_REGION_NOTIFICATION_ID;
					desc.arginfo =
						TZ_OS_APP_REGION_NOTIFICATION_ID_PARAM_ID;
					desc.args[0] = req->addr;
					desc.args[1] = req->size;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				case QSEE_LOAD_SERV_IMAGE_COMMAND: {
					struct qseecom_load_app_ireq *req;
					req = (struct qseecom_load_app_ireq *)req_buf;
					smc_id = TZ_OS_LOAD_SERVICES_IMAGE_ID;
					desc.arginfo = TZ_OS_LOAD_SERVICES_IMAGE_ID_PARAM_ID;
					desc.args[0] = req->mdt_len;
					desc.args[1] = req->img_len;
					desc.args[2] = req->phy_addr;
					dprintf(SPEW, "QSEE_LOAD_SERV_IMAGE_COMMAND mdt_len:%u img_len:%u phy_addr:%u\n",
							req->mdt_len, req->img_len, req->phy_addr);
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				case QSEE_UNLOAD_SERV_IMAGE_COMMAND: {
					smc_id = TZ_OS_UNLOAD_SERVICES_IMAGE_ID;
					desc.arginfo = TZ_OS_UNLOAD_SERVICES_IMAGE_ID_PARAM_ID;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				case QSEE_REGISTER_LISTENER: {
					struct qseecom_register_listener_ireq *req;
					req = (struct qseecom_register_listener_ireq *)req_buf;
					smc_id = TZ_OS_REGISTER_LISTENER_ID;
					desc.arginfo =
						TZ_OS_REGISTER_LISTENER_ID_PARAM_ID;
					desc.args[0] = req->listener_id;
					desc.args[1] = req->sb_ptr;
					desc.args[2] = req->sb_len;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				case QSEE_DEREGISTER_LISTENER: {
					struct qseecom_unregister_listener_ireq *req;
					req = (struct qseecom_unregister_listener_ireq *)
						req_buf;
					smc_id = TZ_OS_DEREGISTER_LISTENER_ID;
					desc.arginfo = TZ_OS_DEREGISTER_LISTENER_ID_PARAM_ID;
					desc.args[0] = req->listener_id;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				case QSEE_LISTENER_DATA_RSP_COMMAND: {
					struct qseecom_client_listener_data_irsp *req;
					req = (struct qseecom_client_listener_data_irsp *)
						req_buf;
					smc_id = TZ_OS_LISTENER_RESPONSE_HANDLER_ID;
					desc.arginfo =
						TZ_OS_LISTENER_RESPONSE_HANDLER_ID_PARAM_ID;
					desc.args[0] = req->listener_id;
					desc.args[1] = req->status;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				case QSEE_CLIENT_SEND_DATA_COMMAND: {
					struct qseecom_client_send_data_ireq *req;
					req = (struct qseecom_client_send_data_ireq *)req_buf;
					smc_id = TZ_APP_QSAPP_SEND_DATA_ID;
					desc.arginfo = TZ_APP_QSAPP_SEND_DATA_ID_PARAM_ID;
					desc.args[0] = req->app_id;
					desc.args[1] = req->req_ptr;
					desc.args[2] = req->req_len;
					desc.args[3] = req->rsp_ptr;
					desc.args[4] = req->rsp_len;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				case QSEE_RPMB_PROVISION_KEY_COMMAND: {
					struct qseecom_client_send_service_ireq *req;
					req = (struct qseecom_client_send_service_ireq *)
						req_buf;
					smc_id = TZ_OS_RPMB_PROVISION_KEY_ID;
					desc.arginfo = TZ_OS_RPMB_PROVISION_KEY_ID_PARAM_ID;
					desc.args[0] = req->key_type;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				case QSEE_RPMB_ERASE_COMMAND: {
					smc_id = TZ_OS_RPMB_ERASE_ID;
					desc.arginfo = TZ_OS_RPMB_ERASE_ID_PARAM_ID;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}

				case QSEE_REGISTER_LOG_BUF_COMMAND: {
					struct qseecom_reg_log_buf_ireq *req;
					req = (struct qseecom_reg_log_buf_ireq *)req_buf;
					smc_id = TZ_OS_REGISTER_LOG_BUFFER_ID;
					desc.arginfo = TZ_OS_REGISTER_LOG_BUFFER_ID_PARAM_ID;
					desc.args[0] = req->phy_addr;
					desc.args[1] = req->len;
					ret = allocate_extra_arg_buffer(smc_id, &desc);
					break;
				}
				default: {
					dprintf(CRITICAL, "qseos_cmd_id 0x%d is not supported by armv8 scm_call2.\n",
								qseos_cmd_id);
					ret = GENERIC_ERROR;
					break;
				}
			} /*end of switch (qsee_cmd_id)  */
			break;
		} /*end of case SCM_SVC_TZSCHEDULER*/
		default: {
			dprintf(CRITICAL, "svc_id 0x%x is not supported by armv8 scm_call2.\n",
						svc_id);
			ret = GENERIC_ERROR;
			break;
		}
	} /*end of switch svc_id */
	scm_resp->result = desc.ret[0];
	scm_resp->resp_type = desc.ret[1];
	scm_resp->data = desc.ret[2];
	dprintf(SPEW, "svc_id = 0x%x, tz_cmd_id = 0x%x, qseos_cmd_id = 0x%x, smc_id = 0x%x, param_id = 0x%x\n",
		svc_id, tz_cmd_id, qseos_cmd_id, smc_id, desc.arginfo);
	dprintf(SPEW, "scm_resp->result = 0x%x, scm_resp->resp_type = 0x%x, scm_resp->data = 0x%x\n",
		scm_resp->result, scm_resp->resp_type, scm_resp->data);
	return ret;
}

static int qseecom_scm_call(uint32_t svc_id, uint32_t tz_cmd_id, void *cmd_buf,
		size_t cmd_len, void *resp_buf, size_t resp_len)
{
	void *req = NULL;
	struct qseecom_command_scm_resp *resp = NULL;
	struct qseecom_client_listener_data_irsp send_data_rsp = {0};
	int ret = GENERIC_ERROR;
	uint32_t qseos_cmd_id = 0;
	struct tzdbg_log_t *log = NULL;
	uint32_t QseeLogStart = 0;
	uint32_t QseeLogNewStart = 0;

	if ((!cmd_buf) || (!resp_buf))
			return GENERIC_ERROR;

	dprintf(SPEW, "%s called\n", __func__);
	mutex_acquire(&qseecom.registered_app_list_lock);
	req = cmd_buf;
	qseos_cmd_id = *(uint32_t *)req;
	resp = (struct qseecom_command_scm_resp *) resp_buf;

	do {
		if (!is_scm_armv8_support()) {
			ret = scm_call(svc_id, tz_cmd_id, req, cmd_len,
					resp_buf, resp_len);
		} else {
			if(logbuf_req.phy_addr)
			{
				log = (struct tzdbg_log_t *)logbuf_req.phy_addr;
				arch_invalidate_cache_range((addr_t) logbuf_req.phy_addr, logbuf_req.len);
				QseeLogStart = (uint32_t) log->log_pos.offset;
			}

			ret = qseecom_scm_call2(svc_id, tz_cmd_id, req, resp);
			if(logbuf_req.phy_addr)
			{
				arch_invalidate_cache_range((addr_t) logbuf_req.phy_addr, logbuf_req.len);
				QseeLogNewStart = (uint32_t) log->log_pos.offset;
				_disp_log_stats((struct tzdbg_log_t *) logbuf_req.phy_addr,
					QSEE_LOG_BUF_SIZE - sizeof(struct tzdbg_log_pos_t),
					QseeLogStart, QseeLogNewStart);
			}
		}

		if (ret) {
			dprintf(CRITICAL, "ERROR: scm_call to load failed : ret %d\n", ret);
			ret = GENERIC_ERROR;
			goto err;
		}

		if (svc_id == TZ_CALL) {
			goto err;
		}

		switch (resp->result) {
			case QSEOS_RESULT_SUCCESS:
				if(((resp->resp_type != QSEOS_APP_ID) || (resp->data <= 0)) &&
					(qseos_cmd_id == QSEE_CLIENT_SEND_DATA_COMMAND))
				{
					dprintf(CRITICAL, "ERROR: Resp type %d or Resp Data %d incorrect\n",
							resp->resp_type, resp->data);
					ret = GENERIC_ERROR;
					goto err;
				}
				goto err;
			case QSEOS_RESULT_FAILURE:
				dprintf(CRITICAL, "scm call failed w/response result%d\n", resp->result);
				ret = GENERIC_ERROR;
				goto err;
			case  QSEOS_RESULT_INCOMPLETE:
				if(resp->resp_type != QSEOS_LISTENER_ID)
				{
					ret = GENERIC_ERROR;
					dprintf(CRITICAL, "Listener service incorrect resp->result:%d resp->resp_type:%d\n",
							resp->result, resp->resp_type);
					goto err;
				}
				__qseecom_process_incomplete_cmd(resp, &send_data_rsp);
				req = (void *)&send_data_rsp;
				qseos_cmd_id = QSEE_LISTENER_DATA_RSP_COMMAND;
				break;
			default:
				dprintf(CRITICAL, "scm call return unknown response %d\n",	resp->result);
				ret = GENERIC_ERROR;
				goto err;
		}
	} while(true);

err:
	mutex_release(&qseecom.registered_app_list_lock);
	return ret;

}

static int __qseecom_process_incomplete_cmd(struct qseecom_command_scm_resp *resp,
		struct qseecom_client_listener_data_irsp *send_data_rsp)
{
	int ret = 0;
	struct qseecom_registered_listener_list *entry;

	if ((!resp) || (!send_data_rsp))
	{
		return GENERIC_ERROR;
	}

	dprintf(SPEW, "%s called\n", __func__);
	mutex_acquire(&qseecom.global_data_lock);

	list_for_every_entry(&qseecom.registered_listener_list_head,
			entry, struct qseecom_registered_listener_list, node) {
		if (resp->data == entry->svc.listener_id) {
			arch_invalidate_cache_range((addr_t) entry->svc.virt_sb_base, entry->svc.sb_size);
			entry->CallbackFn(entry->svc.virt_sb_base, entry->svc.sb_size);
			arch_clean_invalidate_cache_range((addr_t) entry->svc.virt_sb_base, entry->svc.sb_size);
			break;
		}
	}
	send_data_rsp->qsee_cmd_id = QSEE_LISTENER_DATA_RSP_COMMAND;
	send_data_rsp->listener_id  = entry->svc.listener_id;
	send_data_rsp->status  = 0;
	mutex_release(&qseecom.global_data_lock);
	return ret;
}

static int __qseecom_load_app(const char *app_name, unsigned int *app_id)
{
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	unsigned long long size = 0;
	unsigned long rounded_size = 0;
	void *buf = NULL;
	void *req = NULL;
	struct qseecom_load_app_ireq load_req = {0};
	struct qseecom_command_scm_resp resp;

	int ret = GENERIC_ERROR;
	uint8_t lun = 0;

	if (!app_name)
		return GENERIC_ERROR;

	dprintf(SPEW, "%s called\n", __func__);
	index = partition_get_index(app_name);
	lun = partition_get_lun(index);
	mmc_set_lun(lun);

	size = partition_get_size(index);
	if (MAX_TA_PARTITION_SIZE < size) {
		dprintf(CRITICAL, "Invalid TA partition size!");
		ret = GENERIC_ERROR;
		goto err;
	}
	rounded_size = ROUNDUP(size, PAGE_SIZE);
	buf = memalign(PAGE_SIZE, rounded_size);
	if (!buf) {
		dprintf(CRITICAL, "%s: Aloc failed for %s image\n",
				__func__, app_name);
		ret = GENERIC_ERROR;
		goto err;
	}

	ptn = partition_get_offset(index);
	if(ptn == 0) {
		dprintf(CRITICAL, "ERROR: No %s found\n", app_name);
		ret = GENERIC_ERROR;
		goto err;
	}
	if (mmc_read(ptn, (unsigned int *) buf, size)) {
		dprintf(CRITICAL, "ERROR: Cannot read %s image\n", app_name);
		ret = GENERIC_ERROR;
		goto err;
	}

	/* Currently on 8994 only 32-bit phy addr is supported
	 * Hence downcasting is okay
	 */
	load_req.phy_addr = (uint32_t)__qseecom_uvirt_to_kphys((uint32_t) buf);
	load_req.qsee_cmd_id = QSEE_APP_START_COMMAND;
	load_req.img_len = size;
	load_req.mdt_len = 0;
	dprintf(SPEW, "phy_addr:%u img_len:%u\n", load_req.phy_addr, load_req.img_len);

	memscpy(&load_req.app_name, MAX_APP_NAME_SIZE, app_name, MAX_APP_NAME_SIZE);
	req = (void *)&load_req;

	arch_clean_invalidate_cache_range((addr_t) load_req.phy_addr, load_req.img_len);
	ret = qseecom_scm_call(SCM_SVC_TZSCHEDULER, 1, req,
				sizeof(struct qseecom_load_lib_image_ireq),
							&resp, sizeof(resp));
	if(ret == 0)
		*app_id = resp.data;
	else
		*app_id = 0;
err:
	if (buf)
		free(buf);
	return ret;
}

static int qseecom_load_commonlib_image(char * app_name)
{
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	unsigned long long size = 0;
	unsigned  long rounded_size = 0;
	void *buf = NULL;
	void *req = NULL;
	struct qseecom_load_app_ireq load_req = {0};
	struct qseecom_command_scm_resp resp = {0};
	int ret = GENERIC_ERROR;
	uint8_t lun = 0;

	dprintf(SPEW, "%s called\n", __func__);
	index = partition_get_index(app_name);
	lun = partition_get_lun(index);
	mmc_set_lun(lun);

	size = partition_get_size(index);
	if (MAX_TA_PARTITION_SIZE < size) {
		dprintf(CRITICAL, "Invalid cmnlib partition size!");
		ret = GENERIC_ERROR;
		goto err;
	}
	rounded_size = ROUNDUP(size, PAGE_SIZE);
	buf = memalign(PAGE_SIZE, rounded_size);
	if (!buf) {
		dprintf(CRITICAL, "%s: Aloc failed for %s image\n",
				__func__, app_name);
		ret = GENERIC_ERROR;
		goto err;
	}

	ptn = partition_get_offset(index);
	if(ptn == 0) {
		dprintf(CRITICAL, "ERROR: No %s found\n", app_name);
		ret = GENERIC_ERROR;
		goto err;
	}
	if (mmc_read(ptn, (unsigned int *) buf, size)) {
		dprintf(CRITICAL, "ERROR: Cannot read %s image\n", app_name);
		ret = GENERIC_ERROR;
		goto err;
	}

	/* Currently on 8994 only 32-bit phy addr is supported
	 * Hence downcasting is okay
	 */
	load_req.phy_addr = (uint32_t)__qseecom_uvirt_to_kphys((uint32_t) buf);
	load_req.qsee_cmd_id = QSEE_LOAD_SERV_IMAGE_COMMAND;
	load_req.img_len = size;
	load_req.mdt_len = 0;

	memscpy(load_req.app_name, MAX_APP_NAME_SIZE, app_name, MAX_APP_NAME_SIZE);
	req = (void *)&load_req;

	arch_clean_invalidate_cache_range((addr_t) load_req.phy_addr, load_req.img_len);
	ret = qseecom_scm_call(SCM_SVC_TZSCHEDULER, 1, req,
				sizeof(struct qseecom_load_lib_image_ireq),
							&resp, sizeof(resp));
	if(ret == 0)
		ret = resp.data;

err:
	if (buf)
		free(buf);
	return ret;
}

static int qseecom_unload_commonlib_image(void)
{
	int ret = GENERIC_ERROR;
	struct qseecom_unload_lib_image_ireq unload_req = {0};
	struct qseecom_command_scm_resp resp;

	dprintf(SPEW, "%s called\n", __func__);
	/* Populate the remaining parameters */
	unload_req.qsee_cmd_id = QSEE_UNLOAD_SERV_IMAGE_COMMAND;
	/* SCM_CALL to load the image */
	ret = qseecom_scm_call(SCM_SVC_TZSCHEDULER, 1, &unload_req,
			sizeof(struct qseecom_unload_lib_image_ireq),
						&resp, sizeof(resp));
	return ret;
}

/*
 * This function is called with the global
 * data mutex acquired.
 */
static struct qseecom_registered_app_list *
	__qseecom_add_app_entry(char *app_name, uint32_t app_id)
{
	struct qseecom_registered_app_list *entry = NULL;
	int32_t ret = GENERIC_ERROR;

	if ((!app_name) || (app_id == 0)) {
		dprintf(CRITICAL, "%s: Invalid Input\n", __func__);
		return NULL;
	}
	dprintf(SPEW, "%s called\n", __func__);

	entry = malloc(sizeof(*entry));
	if (!entry) {
		dprintf(CRITICAL, "malloc for app entry failed\n");
		ret =  GENERIC_ERROR;
		goto err;
	}
	entry->app_id = app_id;
	entry->ref_cnt = 1;
	strlcpy(entry->app_name, app_name, MAX_APP_NAME_SIZE);

	dprintf(SPEW, "%s: Adding app:%s app_id:%u to list\n", __func__, entry->app_name, entry->app_id);
	list_add_tail(&qseecom.registered_app_list_head, &entry->node);
	ret = 0;
err:
	if (entry && (ret < 0)) {
		free(entry);
		return NULL;
	}
	return entry;
}

/*
 * This function is called with the global
 * data mutex acquired.
 */
static int
	__qseecom_remove_app_entry(struct qseecom_registered_app_list *entry)
{
	if (!entry) {
		dprintf(CRITICAL, "%s: Invalid Input\n", __func__);
		return GENERIC_ERROR;
	}
	dprintf(SPEW, "%s called\n", __func__);
	list_delete(&entry->node);
	free(entry);

	return 0;
}

/*
 * This function is called with the global
 * data mutex acquired.
 */
struct qseecom_registered_listener_list *
	__qseecom_check_listener_exists(uint32_t listener_id)
{
	struct qseecom_registered_listener_list *entry = NULL;
	bool listener_present = false;

	if (!listener_id) {
		dprintf(CRITICAL, "%s: Invalid Input\n", __func__);
		return NULL;
	}
	dprintf(SPEW, "%s called\n", __func__);

	list_for_every_entry(&qseecom.registered_listener_list_head,
			entry, struct qseecom_registered_listener_list, node) {
		if (entry->svc.listener_id == listener_id) {
			listener_present = true;
			break;
		}
	}
	if (listener_present)
		return entry;
	else
		return NULL;
}

/*
 * This function is called with the global
 * data mutex acquired.
 */
static struct qseecom_registered_app_list
	*__qseecom_check_handle_exists(int handle)
{
	struct qseecom_registered_app_list *entry;
	bool app_present = false;

	if (handle <= 0) {
		dprintf(CRITICAL, "%s: Invalid Input\n", __func__);
		return NULL;
	}
	dprintf(SPEW, "%s called\n", __func__);
	list_for_every_entry(&qseecom.registered_app_list_head,
			entry, struct qseecom_registered_app_list, node) {
		if (entry->handle == handle) {
			app_present = true;
			break;
		}
	}

	if (app_present == true)
		return entry;
	else
		return NULL;

}


static struct qseecom_registered_app_list *
	__qseecom_check_app_exists(char *app_name)
{
	struct qseecom_registered_app_list *entry = NULL;

	dprintf(SPEW, "%s called\n", __func__);
	list_for_every_entry(&qseecom.registered_app_list_head,
			entry, struct qseecom_registered_app_list, node) {
		if (!strncmp(app_name, entry->app_name, 32)) {
			dprintf(SPEW, "%s: app_name:%s\n", __func__, app_name);
			return entry;
		}
	}
	return NULL;
}

static int qseecom_unload_app(uint32_t app_id)
{
	int ret = 0;
	struct qseecom_command_scm_resp resp;
	struct qseecom_unload_app_ireq req;

	dprintf(SPEW, "%s called\n", __func__);
	/* Populate the structure for sending scm call to load image */
	req.qsee_cmd_id = QSEE_APP_SHUTDOWN_COMMAND;
	req.app_id = app_id;

	/* SCM_CALL to unload the app */
	ret = qseecom_scm_call(SCM_SVC_TZSCHEDULER, 1, &req,
			sizeof(struct qseecom_unload_app_ireq),
			&resp, sizeof(resp));

	return ret;
}


static int __qseecom_send_cmd(uint32_t app_id, struct qseecom_send_cmd_req *req)
{
	int ret = 0;
	struct qseecom_client_send_data_ireq send_data_req;
	struct qseecom_command_scm_resp resp;
	void *buf = NULL;
	uint32_t size = 0;

	if (req->cmd_req_buf == NULL || req->resp_buf == NULL) {
		dprintf(CRITICAL, "%s: cmd buffer or response buffer is null\n",
				__func__);
		return GENERIC_ERROR;
	}
	dprintf(SPEW, "%s called\n", __func__);

	/* Allocate for req or rsp len whichever is higher, both req and rsp point
	 * to the same buffer
	 */
	size = (req->cmd_req_len > req->resp_len) ? req->cmd_req_len : req->resp_len;

	/* The req rsp buffer will be xPU protected by TZ during a TZ APP call
	 * This will still be protected during a listener call and there is a
	 * possibility of prefetching happening, which will cause xPU violation.
	 * Hence using (device memory with xN set) to prevent I or D prefetching.
	 * This is a contiguous region of 1MB used only for this, hence will not
	 * free this.
	 */
	buf = (void *)RPMB_SND_RCV_BUF;
	if (!buf) {
		dprintf(CRITICAL, "%s: Aloc failed for app_id:%d of size:%d\n",
				__func__, app_id, size);
		return GENERIC_ERROR;
	}

	memscpy(buf, ROUNDUP(size, PAGE_SIZE), req->cmd_req_buf, req->cmd_req_len);

	send_data_req.qsee_cmd_id = QSEE_CLIENT_SEND_DATA_COMMAND;
	send_data_req.app_id = app_id;

	/* Currently on 8994 only 32-bit phy addr is supported
	 * Hence downcasting is okay
	 */
	send_data_req.req_ptr = (uint32_t)__qseecom_uvirt_to_kphys((uint32_t) buf);
	send_data_req.req_len = req->cmd_req_len;
	send_data_req.rsp_ptr = (uint32_t)__qseecom_uvirt_to_kphys((uint32_t) buf);
	send_data_req.rsp_len = req->resp_len;

	ret = qseecom_scm_call(SCM_SVC_TZSCHEDULER, 1,
				(void *)&send_data_req,
				sizeof(send_data_req), (void *)&resp, sizeof(resp));

	memscpy(req->resp_buf, req->resp_len, (void *)send_data_req.rsp_ptr, send_data_req.rsp_len);
	return ret;
}

/**
* Start a Secure App
*
* @param char* app_name
*   App name of the Secure App to be started
*
* @return int
*   Success:	handle to be used for all calls to
*   			Secure app. Always greater than zero.
*   Failure:	Error code (negative only).
*/
int qseecom_start_app(char *app_name)
{
	int32_t ret = GENERIC_ERROR;
	int handle = 0;
	struct qseecom_registered_app_list *entry = NULL;
	unsigned int app_id = 0;

	if (!app_name) {
		dprintf(CRITICAL, "%s: Input error\n", __func__);
		goto err;
	}
	dprintf(SPEW, "%s called\n", __func__);


	mutex_acquire(&qseecom.global_data_lock);
	if ((!qseecom.qseecom_init_done)
			|| (!qseecom.qseecom_tz_init_done)){
		dprintf(CRITICAL, "%s qseecom_init not done\n",
							__func__);
		mutex_release(&qseecom.global_data_lock);
		return ret;
	}
	mutex_release(&qseecom.global_data_lock);
	/* Load commonlib image*/
	if (!qseecom.cmnlib_loaded) {
		ret = qseecom_load_commonlib_image("cmnlib");
		if (ret) {
			dprintf(CRITICAL, "%s qseecom_load_commonlib_image failed with status:%d\n",
					__func__, ret);
			goto err;
		}
		qseecom.cmnlib_loaded = 1;
	}
	/* Check if App already exits, if exits increase ref_cnt
	 * and return handle, else load the app from partition,
	 * call into TZ to load it, add to list and then return
	 * handle.
	 */
	mutex_acquire(&qseecom.global_data_lock);
	entry = __qseecom_check_app_exists(app_name);
	if (!entry) {
		mutex_release(&qseecom.global_data_lock);
		/* load the app and get the app_id  */
		dprintf(INFO, "%s: Loading app %s for the first time'\n",
				__func__, app_name);

		ret = __qseecom_load_app(app_name, &app_id);
		if ((ret < 0) || (app_id ==0)) {
			dprintf(CRITICAL, "%s: __qseecom_load_app failed with err:%d for app:%s\n",
					__func__, ret, app_name);
			ret = GENERIC_ERROR;
			goto err;
		}
		mutex_acquire(&qseecom.global_data_lock);
		entry = __qseecom_add_app_entry(app_name, app_id);
		if (!entry)
		{
			dprintf(CRITICAL, "%s: __qseecom_add_app_entry failed\n", __func__);
			ret = GENERIC_ERROR;
			mutex_release(&qseecom.global_data_lock);
			goto err;
		}
		qseecom.handle++;
		entry->handle = qseecom.handle;
		handle = entry->handle;
		mutex_release(&qseecom.global_data_lock);
	}
	else {
		entry->ref_cnt++;
		handle = entry->handle;
		mutex_release(&qseecom.global_data_lock);
	}
	return handle;
err:
	return ret;
}

/**
* Shutdown a Secure App
*
* @param int handle
*   Handle  of the Secure App to be shutdown
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
int qseecom_shutdown_app(int handle)
{
	int ret = GENERIC_ERROR;
	int ref_cnt = 0;
	struct qseecom_registered_app_list *entry = NULL;

	if (handle <= 0) {
		dprintf(CRITICAL, "%s: Invalid Handle %d\n", __func__, handle);
		goto err;
	}
	dprintf(SPEW, "%s called\n", __func__);
	mutex_acquire(&qseecom.global_data_lock);
	if ((!qseecom.qseecom_init_done)
			|| (!qseecom.qseecom_tz_init_done)) {
		dprintf(CRITICAL, "%s qseecom_init not done\n",
							__func__);
		mutex_release(&qseecom.global_data_lock);
		return ret;
	}
	entry = __qseecom_check_handle_exists(handle);
	if (!entry) {
		dprintf(CRITICAL, "%s: Shutdown on an app that was never loaded handle:%d\n",
				__func__, handle);
		ret = GENERIC_ERROR;
		mutex_release(&qseecom.global_data_lock);
		goto err;
	}

	/* Decrement ref_cnt by 1, if ref_cnt is 0 after
	 * decrementing unload the app by calling into
	 * TZ else just return.
	 */

	if(entry->ref_cnt != 0)
		entry->ref_cnt--;
	ref_cnt = entry->ref_cnt;
	mutex_release(&qseecom.global_data_lock);
	if (ref_cnt == 0) {
		ret = qseecom_unload_app(entry->app_id);
		if(ret) {
			dprintf(CRITICAL, "%s: qseecom_unload_app failed with err:%d for handle:%d\n",
					__func__, ret, handle);
			goto err;
		}
		mutex_acquire(&qseecom.global_data_lock);
		ret = __qseecom_remove_app_entry(entry);
		mutex_release(&qseecom.global_data_lock);
		if(ret) {
			dprintf(CRITICAL, "%s: __qseecom_remove_app_entry failed with err:%d for handle:%d\n",
					__func__, ret, handle);
			goto err;
		}
	}
	ret = 0;
err:
	return ret;
}

/**
* Send cmd to a Secure App
*
* @param int handle
*   Handle  of the Secure App to send the cmd
*
* @param void *send_buf
*   Pointer to the App request buffer
*
* @param uint32_t sbuf_len
*   Size of the request buffer
*
* @param void *resp_buf
*   Pointer to the App response buffer
*
* @param uint32_t rbuf_len
*   Size of the response buffer
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
int qseecom_send_command(int handle, void *send_buf,
			uint32_t sbuf_len, void *resp_buf, uint32_t rbuf_len)
{
	int ret = GENERIC_ERROR;
	uint32_t app_id = 0;
	struct qseecom_registered_app_list *entry = NULL;
	struct qseecom_send_cmd_req req = {0, 0, 0, 0};

	if (handle <= 0) {
		dprintf(CRITICAL, "%s Handle is Invalid\n", __func__);
		return GENERIC_ERROR;
	}

	if((!send_buf) || (!resp_buf)) {
		dprintf(CRITICAL, "%s: Input Buffers invalid\n", __func__);
		return GENERIC_ERROR;
	}
	dprintf(SPEW, "%s called\n", __func__);
	mutex_acquire(&qseecom.global_data_lock);
	if ((!qseecom.qseecom_init_done)
			|| (!qseecom.qseecom_tz_init_done)) {
		dprintf(CRITICAL, "%s qseecom_init not done\n",
							__func__);
		mutex_release(&qseecom.global_data_lock);
		return ret;
	}
	entry = __qseecom_check_handle_exists(handle);
	if (!entry) {
		dprintf(CRITICAL, "%s: Send cmd on an app that was never loaded handle:%d\n",
				__func__, handle);
		ret = GENERIC_ERROR;
		mutex_release(&qseecom.global_data_lock);
		goto err;
	}

	app_id = entry->app_id;
	mutex_release(&qseecom.global_data_lock);

	req.cmd_req_len = sbuf_len;
	req.resp_len = rbuf_len;
	req.cmd_req_buf = send_buf;
	req.resp_buf = resp_buf;

	ret = __qseecom_send_cmd(app_id, &req);
	if (ret) {
		dprintf(CRITICAL, "%s __qseecom_send_cmd failed with err:%d for handle:%d\n",
				__func__, ret, handle);
		goto err;
	}

	ret = 0;
	dprintf(SPEW, "sending cmd_req->rsp size: %u, ptr: 0x%p\n",
			req.resp_len, req.resp_buf);
err:
	return ret;
}

/**
* Registers a Listener Service with QSEE
*
* @param uint32_t listnr_id
*   Pre-defined Listener ID to be registered
*
* @param uint32_t sb_size
*   Shared buffer size required for the listener
*   service.
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
int qseecom_register_listener(struct qseecom_listener_services *listnr)
{
	int ret = GENERIC_ERROR;
	struct qseecom_registered_listener_list *new_entry = NULL;
	struct qseecom_register_listener_ireq req;
	struct qseecom_command_scm_resp resp;

	mutex_acquire(&qseecom.global_data_lock);
	if (!qseecom.qseecom_init_done) {
		dprintf(CRITICAL, "%s qseecom_init not done\n",
							__func__);
		mutex_release(&qseecom.global_data_lock);
		return ret;
	}
	mutex_release(&qseecom.global_data_lock);

	mutex_acquire(&qseecom.registered_listener_list_lock);

	if ((!listnr)) {
		dprintf(CRITICAL, "%s Invalid Input listnr\n", __func__);
		return GENERIC_ERROR;
	}

	if ((!listnr->id) || (!listnr->sb_size) || (!listnr->service_cmd_handler)) {
		dprintf(CRITICAL, "%s Invalid Input listnr_id:%d sb_size:%d\n",
				__func__, listnr->id, listnr->sb_size);
		return GENERIC_ERROR;
	}
	dprintf(SPEW, "%s called\n", __func__);
	new_entry = __qseecom_check_listener_exists(listnr->id);
	if (new_entry) {
		dprintf(CRITICAL, "Service is not unique and is already registered\n");
		ret = LISTENER_ALREADY_PRESENT_ERROR;
		goto err;
	}

	new_entry = malloc(sizeof(*new_entry));
	if (!new_entry) {
		dprintf(CRITICAL, "%s new_entry malloc failed for size:%d\n", __func__, sizeof(*new_entry));
		ret = GENERIC_ERROR;
		goto err;
	}
	memset(new_entry, 0, sizeof(*new_entry));
	new_entry->svc.listener_id = listnr->id;
	new_entry->svc.sb_size = listnr->sb_size;
	new_entry->CallbackFn = listnr->service_cmd_handler;

	new_entry->svc.virt_sb_base = memalign(PAGE_SIZE, ROUNDUP(listnr->sb_size, PAGE_SIZE));
	if (!new_entry->svc.virt_sb_base) {
		dprintf(CRITICAL, "%s virt_sb_base malloc failed for size:%d\n", __func__, listnr->sb_size);
		ret = GENERIC_ERROR;
		goto err;
	}
	memset(new_entry->svc.virt_sb_base, 0, ROUNDUP(listnr->sb_size, PAGE_SIZE));
	arch_clean_invalidate_cache_range((addr_t) new_entry->svc.virt_sb_base, ROUNDUP(listnr->sb_size, PAGE_SIZE));

	req.qsee_cmd_id = QSEE_REGISTER_LISTENER;
	req.listener_id = new_entry->svc.listener_id;
	req.sb_len = new_entry->svc.sb_size;
	/* convert to 32bit addr for tz */
	req.sb_ptr = (uint32_t)__qseecom_uvirt_to_kphys((uint32_t) new_entry->svc.virt_sb_base);

	resp.result = QSEOS_RESULT_INCOMPLETE;

	ret = qseecom_scm_call(SCM_SVC_TZSCHEDULER, 1, &req,
					sizeof(req), &resp, sizeof(resp));
	if (ret) {
		dprintf(CRITICAL, "qseecom_scm_call failed with err: %d\n", ret);
		ret = GENERIC_ERROR;
		goto err;
	}
	/* Add entry to Listener list */
	list_add_tail(&qseecom.registered_listener_list_head, &new_entry->node);
err:
	if ((ret) &&
			(ret != LISTENER_ALREADY_PRESENT_ERROR)) {
		if ((new_entry) &&
				(new_entry->svc.virt_sb_base))
			free(new_entry->svc.virt_sb_base);
		if (new_entry)
			free(new_entry);
	}
	mutex_release(&qseecom.registered_listener_list_lock);
	return ret;
}

/**
* De-Registers a Listener Service with QSEE
*
* @param uint32_t listnr_id
*   Pre-defined Listener ID to be de-registered
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
int qseecom_deregister_listener(uint32_t listnr_id)
{
	int ret = GENERIC_ERROR;
	struct qseecom_registered_listener_list *new_entry = NULL;
	struct qseecom_unregister_listener_ireq req;
	struct qseecom_command_scm_resp resp;

	mutex_acquire(&qseecom.global_data_lock);
	if (!qseecom.qseecom_init_done) {
		dprintf(CRITICAL, "%s qseecom_init not done\n",
							__func__);
		mutex_release(&qseecom.global_data_lock);
		return ret;
	}
	mutex_release(&qseecom.global_data_lock);

	mutex_acquire(&qseecom.registered_listener_list_lock);
	dprintf(SPEW, "%s called\n", __func__);
	new_entry = __qseecom_check_listener_exists(listnr_id);
	if (!new_entry) {
		dprintf(CRITICAL, "Service not present\n");
		ret = GENERIC_ERROR;
		goto err;
	}

	req.qsee_cmd_id = QSEE_DEREGISTER_LISTENER;
	req.listener_id = listnr_id;
	resp.result = QSEOS_RESULT_INCOMPLETE;

	ret = qseecom_scm_call(SCM_SVC_TZSCHEDULER, 1, &req,
					sizeof(req), &resp, sizeof(resp));
	if (ret) {
		dprintf(CRITICAL, "scm_call() failed with err: %d (lstnr id=%d)\n",
				ret, req.listener_id);
		ret = GENERIC_ERROR;
		goto err;
	}

	list_delete(&new_entry->node);

err:
	if (ret == 0) {
		if (new_entry)
			free(new_entry);
	}
	mutex_release(&qseecom.registered_listener_list_lock);
	return ret;
}

static uint32_t app_region_addr = APP_REGION_ADDR;
static uint32_t app_region_size = APP_REGION_SIZE;

void qseecom_lk_set_app_region(uint32_t addr, uint32_t size)
{
	app_region_addr = addr;
	app_region_size = size;
}

int qseecom_tz_init()
{
	struct qsee_apps_region_info_ireq req;
	struct qseecom_command_scm_resp resp;
	int rc = GENERIC_ERROR;
	/* register log buffer scm request */
	void *buf = NULL;
	/* Register app region with TZ */
	req.qsee_cmd_id = QSEE_APP_REGION_NOTIFICATION;
	req.addr = app_region_addr;
	req.size = app_region_size;
	dprintf(ALWAYS, "secure app region addr=0x%x size=0x%x",
					req.addr, req.size);
	rc = qseecom_scm_call(SCM_SVC_TZSCHEDULER, 1,
			&req, sizeof(req),
			&resp, sizeof(resp));
	dprintf(ALWAYS, "TZ App region notif returned with status:%d addr:%x size:%d\n",
			rc, req.addr, req.size);
	if (rc)
		goto err;
	buf = memalign(PAGE_SIZE, ROUNDUP(QSEE_LOG_BUF_SIZE, PAGE_SIZE));
	if (!buf) {
		rc = GENERIC_ERROR;
		goto err;
	}
	memset(buf, 0, ROUNDUP(QSEE_LOG_BUF_SIZE, PAGE_SIZE));
	logbuf_req.qsee_cmd_id = QSEE_REGISTER_LOG_BUF_COMMAND;
	logbuf_req.phy_addr = (uint32_t)__qseecom_uvirt_to_kphys((uint32_t) buf);
	logbuf_req.len = QSEE_LOG_BUF_SIZE;

	rc = qseecom_scm_call(SCM_SVC_TZSCHEDULER, 1,
			&logbuf_req, sizeof(logbuf_req),
			&resp, sizeof(resp));
	dprintf(ALWAYS, "TZ App log region register returned with status:%d addr:%x size:%d\n",
			rc, logbuf_req.phy_addr, logbuf_req.len);
	if (rc)
		goto err;
err:
	if (!rc) {
		qseecom.qseecom_tz_init_done = 1;
		dprintf(ALWAYS, "Qseecom TZ Init Done in Appsbl\n");
	}
	return rc;
}

int qseecom_init()
{
	int rc = GENERIC_ERROR;
	struct qseecom_command_scm_resp resp;
	uint32_t ver = TZ_FVER_QSEE;

	memset (&qseecom, 0, sizeof(struct qseecom_control));
	dprintf(SPEW, "%s called\n", __func__);
	mutex_init(&(qseecom.global_data_lock));
	mutex_init(&(qseecom.registered_app_list_lock));
	mutex_init(&(qseecom.registered_listener_list_lock));

	list_initialize(&(qseecom.registered_app_list_head));
	list_initialize(&(qseecom.registered_listener_list_head));

	rc = qseecom_scm_call(TZ_SVC_INFO, 3,
						  &ver, sizeof(ver),
						  &resp, sizeof(resp));

	if (!rc) {
		qseecom.qseecom_init_done = 1;
		qseecom.qseos_version = resp.result;
		dprintf(ALWAYS, "Qseecom Init Done in Appsbl version is 0x%x\n",resp.result);
	}
	return rc;
}

unsigned int qseecom_get_version()
{
	return qseecom.qseos_version;
}

int qseecom_exit()
{
	dprintf(SPEW, "%s called\n", __func__);

	if (logbuf_req.phy_addr)
		free((void *)logbuf_req.phy_addr);
	qseecom.qseecom_init_done = 0;
	dprintf(ALWAYS, "Qseecom De-Init Done in Appsbl\n");
	return 0;
}
