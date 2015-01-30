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

#include <partition_parser.h>
#include <qseecom_lk.h>
#include <scm.h>

//#include "qseecom_legacy.h"
#include "qseecom_lk_api.h"
#include <debug.h>
#include <kernel/mutex.h>
#include <malloc.h>
#include <stdlib.h>
#include <arch/defines.h>
#include <string.h>

/* commands supported from sample client */
#define CLIENT_CMD0_GET_VERSION             0
#define CLIENT_CMD1_BASIC_DATA              1
#define CLIENT_CMD2_REGISTER_SB_PTR         2
#define CLIENT_CMD3_RUN_CRYTPO_TEST         3
#define CLIENT_CMD4_RUN_CRYTPO_PERF         4
#define CLIENT_CMD5_RUN_SFS_TEST            5
#define CLIENT_CMD6_RUN_BUF_ALIGNMENT_TEST  6
#define CLIENT_CMD7_RUN_CRYPTO_RSA_TEST     7
#define CLIENT_CMD8_RUN_RSA_PERF_TEST       8
#define CLIENT_CMD9_RUN_CMNLIB_TEST         9
#define CLIENT_CMD10_RUN_CORE_TEST          10
#define CLIENT_CMD11_RUN_SECURECHANNEL_TEST 11
#define CLIENT_CMD12_RUN_SERVICES_TEST      12
#define CLIENT_CMD13_RUN_MISC_TEST          13
#define CLIENT_CMD17_RUN_STORAGE_TEST       17
#define CLIENT_APPSBL_QSEECOM_RUN_LISTENER_FRAMEWORK_TEST_1       50

#define __64KB__ 0x10000
#define __32KB__ 0x8000
#define __16KB__ 0x4000
#define __8KB__ 0x2000
#define __4KB__ 0x1000
#define __2KB__ 0x800
#define __1KB__ 0x400

#define LISTENER_TEST_SERVICE			0x100

struct qsc_send_cmd {
     unsigned int cmd_id;
     unsigned int data;
     unsigned int data2;
     unsigned int len;
     unsigned int start_pkt;
     unsigned int end_pkt;
     unsigned int test_buf_size;
};

struct qsc_send_cmd_rsp {
  unsigned int data;
  int status;
};


int listener_test_cmd_handler (void* buf, uint32_t size)
{
	uint32 *req = 0;
	uint32 rsp = 0;

	if ((!buf) || (size < 4)) {
		dprintf(CRITICAL, "Invalid listener cmd handler inputs\n");
		return -1;
	}

	req = (uint32 *)buf;
	if (*req == (100 * 100)) {
		dprintf(CRITICAL, "Listener Success\n");
		rsp = *req * 100;
		dprintf(CRITICAL, "rsp = %u\n", rsp);
		memcpy (buf, &rsp, sizeof(uint32));
		return 0;
	} else {
		dprintf(CRITICAL, "Listener Failure, req:%u\n", *req);
		rsp = 0;
		memcpy (buf, &rsp, sizeof(uint32));
		return 0;
	}
}

static struct qseecom_listener_services listeners[] = {
		{
			.service_name = "Listener Test service",
			.id = LISTENER_TEST_SERVICE,
			.sb_size = (20 * 1024),
			.service_cmd_handler = listener_test_cmd_handler,
		},
};

int qsc_run_get_version(char *appname, uint32_t iterations)
{
	struct qsc_send_cmd send_cmd;
	struct qsc_send_cmd_rsp msgrsp={0};	/* response data sent from QSEE */
	uint32_t i = 0, ret = 0, err = -1;

	dprintf(CRITICAL, "%s:Get_version\n", __func__);

	dprintf(CRITICAL, "(This may take a few minutes please wait....)\n");
	/* Start the application */
	for  (i = 0;  i < iterations;  i++) {
		dprintf(CRITICAL, "iteration %d\n", i);
		ret = qseecom_start_app(appname);
		if (ret <= 0) {
			dprintf(CRITICAL, "Start app: fail: ret:%d\n", ret);
			err = -1;
			goto err_ret;
		}
		dprintf(CRITICAL, "apphandle %u\n", ret);
		ret = qseecom_start_app(appname);
		if (ret <= 0) {
			dprintf(CRITICAL, "Start app: fail: ret:%d\n", ret);
			err = -1;
			goto err_ret_shutdown;
		}
		dprintf(CRITICAL, "apphandle %u\n", ret);
		send_cmd.cmd_id = CLIENT_CMD0_GET_VERSION;
		err = qseecom_send_command(ret, &send_cmd, sizeof(struct qsc_send_cmd), &msgrsp, sizeof(struct qsc_send_cmd_rsp));
		if (err) {
			dprintf(CRITICAL, "Send app: fail\n");
			goto err_ret_shutdown;
		}
		dprintf(ALWAYS, "The version of %s is: %u\n", appname, msgrsp.data);
		/* Shutdown the application */
		dprintf(ALWAYS, "Shutting down %s\n", appname);
		err = qseecom_shutdown_app(ret);
		if (err) {
			dprintf(CRITICAL, "Failed to shutdown app: %d\n",err);
			goto err_ret;
		}
		dprintf(ALWAYS, "Shutting down %s\n", appname);
		err = qseecom_shutdown_app(ret);
		if (err) {
			dprintf(CRITICAL, "Failed to shutdown app: %d\n",err);
			goto err_ret;
		}

    }
err_ret_shutdown:
	if (err) {
                 if (qseecom_shutdown_app(ret))
	             dprintf(CRITICAL, "Failed to shutdown app: %d\n",err);
	}
err_ret:
	if (err)
		dprintf(CRITICAL, "Test %u failed with error %d, shutting down %s\n", CLIENT_CMD0_GET_VERSION, err, appname);
	else
		dprintf(CRITICAL, "Test %u passed for iterations %u executing %s\n", CLIENT_CMD0_GET_VERSION, iterations, appname);
	return err;
}


int qsc_run_start_stop_app_basic_test(char *appname, uint32_t iterations)
{
	uint32_t i = 0;
	int ret = 0;			/* return value */
	int err = 0;			/* return value */
	struct qsc_send_cmd send_cmd = {0};
	struct qsc_send_cmd_rsp msgrsp={0};	/* response data sent from QSEE */

	dprintf(CRITICAL, "%s:Basic_start_stop_test\n", __func__);

	dprintf(CRITICAL, "(This may take a few minutes please wait....)\n");
	/* Start the application */
    for  (i = 0;  i < iterations;  i++) {
		dprintf(CRITICAL, "iteration %d\n", i);
		ret = qseecom_start_app(appname);
		if (ret <= 0) {
			dprintf(CRITICAL, "Start app: fail: ret:%d\n", ret);
			err = -1;
			goto err_ret;
		}
		/* Send data using send command to QSEE application */
		send_cmd.cmd_id = CLIENT_CMD1_BASIC_DATA;
		send_cmd.start_pkt = 0;
		send_cmd.end_pkt = 0;
		send_cmd.test_buf_size = 0;
		send_cmd.data = 100;
		err = qseecom_send_command(ret, &send_cmd, sizeof(struct qsc_send_cmd), &msgrsp, sizeof(struct qsc_send_cmd_rsp));
		if (err) {
			dprintf(CRITICAL, "Send app: fail\n");
			goto err_ret_shutdown;
		}
		if (((msgrsp.data)/100) != 10) {
			dprintf(CRITICAL, "App Comm Error:%u\n", msgrsp.data);
			err = -1;
			goto err_ret_shutdown;
		}
		dprintf(CRITICAL, "msg.rsp:%u\n", msgrsp.data);
		/* Shutdown the application */
		err = qseecom_shutdown_app(ret);
		if (err) {
			dprintf(CRITICAL, "Failed to shutdown app: %d\n",err);
			goto err_ret;
		}
    }
err_ret_shutdown:
    if (err) {
		if (qseecom_shutdown_app(ret))
			dprintf(CRITICAL, "Failed to shutdown app: %d\n",err);
    }
err_ret:
	if (err)
		dprintf(CRITICAL, "Test %u failed with error %d, shutting down %s\n", CLIENT_CMD0_GET_VERSION, err, appname);
	else
		dprintf(CRITICAL, "Test %u passed for iterations %u executing %s\n", CLIENT_CMD0_GET_VERSION, iterations, appname);
		return err;
}



int qsc_run_start_stop_app_listener_test(char *appname, uint32_t iterations)
{
	uint32_t i = 0;
	int ret = 0;			/* return value */
	int err = 0;			/* return value */
	struct qsc_send_cmd send_cmd;
	struct qsc_send_cmd_rsp msgrsp={0};	/* response data sent from QSEE */

	dprintf(CRITICAL, "%s:qsc_run_start_stop_app_listener_test\n", __func__);

	dprintf(CRITICAL, "(This may take a few minutes please wait....)\n");

	ret = qseecom_register_listener(&listeners[0]);
	if (ret < 0) {
		dprintf(CRITICAL, "Reg Listener: fail: ret:%d\n", ret);
		err = -1;
		goto err_ret;
	}
	/* Start the application */
    for  (i = 0;  i < iterations;  i++) {
		dprintf(CRITICAL, "iteration %d\n", i);
		ret = qseecom_start_app(appname);
		if (ret <= 0) {
			dprintf(CRITICAL, "Start app: fail: ret:%d\n", ret);
			err = -1;
			goto err_dereg;
		}
		/* Send data using send command to QSEE application */
		send_cmd.cmd_id = CLIENT_APPSBL_QSEECOM_RUN_LISTENER_FRAMEWORK_TEST_1;
		send_cmd.start_pkt = 0;
		send_cmd.end_pkt = 0;
		send_cmd.test_buf_size = 0;
		send_cmd.data = 100;
		err = qseecom_send_command(ret, &send_cmd, sizeof(struct qsc_send_cmd), &msgrsp, sizeof(struct qsc_send_cmd_rsp));
		if (err) {
			dprintf(CRITICAL, "Send app: fail\n");
			goto err_ret_shutdown;
		}
		if (((msgrsp.data)/100) != 10) {
			dprintf(CRITICAL, "App Comm Error:%u Status:%d\n", msgrsp.data, msgrsp.status);
			err = -1;
			goto err_ret_shutdown;
		}
		/* Shutdown the application */
		err = qseecom_shutdown_app(ret);
		if (err) {
			dprintf(CRITICAL, "Failed to shutdown app: %d\n",err);
			goto err_dereg;
		}
    }

err_ret_shutdown:
	if (err) {
	   	if (qseecom_shutdown_app(ret))
	   		dprintf(CRITICAL, "Failed to shutdown app: %d\n",err);
	}
err_dereg:
	ret = qseecom_deregister_listener(listeners[0].id);
	if (ret < 0) {
		dprintf(CRITICAL, "DeReg Listener: fail: ret:%d\n", ret);
		err = -1;
	}
err_ret:
	if (err)
		dprintf(CRITICAL, "Test %u failed with error %d, shutting down %s\n", CLIENT_CMD0_GET_VERSION, err, appname);
	else
		dprintf(CRITICAL, "Test %u passed for iterations %u executing %s\n", CLIENT_CMD0_GET_VERSION, iterations, appname);
	return err;
}

int qseecom_test_cmd_handler(const char *arg) {
	char *token = NULL;
	char * appname = NULL;
	uint32_t test = 0;
	uint32_t iterations = 0;

	token = strtok((char *)arg, " ");
	if (!token) {
		dprintf(CRITICAL, "Appname not provided, error\n");
		return -1;
	}
	appname = token;

	token = strtok(NULL, " ");
	if (!token) {
		dprintf(CRITICAL, "Test not provided, error\n");
		return -1;
	}
	test = atoi(token);

	token = strtok(NULL, " ");
	if (token)
		iterations = atoi(token);
	else
		dprintf(CRITICAL, "iterations not provided\n");

	switch (test) {
		case CLIENT_CMD0_GET_VERSION:
		{
			qsc_run_get_version(appname, iterations);
			break;
		}
		case CLIENT_CMD1_BASIC_DATA:
		{
			qsc_run_start_stop_app_basic_test(appname, iterations);
			break;
		}

		case CLIENT_APPSBL_QSEECOM_RUN_LISTENER_FRAMEWORK_TEST_1:
		{
			qsc_run_start_stop_app_listener_test(appname, iterations);
			break;
		}
	}
	dprintf(CRITICAL, "test:%u iterations:%u\n", test, iterations);
	return 0;
}

