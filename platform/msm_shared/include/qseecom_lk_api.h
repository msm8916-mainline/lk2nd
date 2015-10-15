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

#ifndef __QSEECOM_LK_API_H_
#define __QSEECOM_LK_API_H_

#define MAX_APP_NAME_LEN 32

#include <stdint.h>
/**
* Qseecom Init
*	To be called before any calls to qsee secure apps.
*
* @return int
*   Success:	Init succeeded.
*   Failure:	Error code (negative only).
*/
int qseecom_init();

/**
 * Qseecom set app address regions
 *	To be called before any calls to tz init.
 *
 * @return void
 *   Success:	Not applicable
 *   Failure:	Not applicable
 */
void qseecom_lk_set_app_region(uint32_t addr, uint32_t size);

/**
 * Qseecom get version.
 *	To be called before calls to set app region.
 *
 * @return unsigned int
 *   Success:	Valid version
 *   Failure:	Garbage value
 */
unsigned int qseecom_get_version();

/**
* Qseecom Tz Init
*	To be called before any calls to qsee secure apps.
*
* @return int
*   Success:	Tz init succeeded.
*   Failure:	Error code (negative only).
*/
int qseecom_tz_init();

/**
* Qseecom Exit
*	To be called before exit of lk.
*	Once this is called no calls to
*	Qsee Apps.
*
* @return int
*   Success:	Exit succeeded.
*   Failure:	Error code (negative only).
*/
int qseecom_exit();

/**
* Start a Secure App
*
* @param char* app_name
*   App name of the Secure App to be started
*   The app_name provided should be the same
*   name as the partition/ file and should
*   be the same name mentioned in TZ_APP_NAME
*   in the secure app.
*
* @return int
*   Success:	handle to be used for all calls to
*   			Secure app. Always greater than zero.
*   Failure:	Error code (negative only).
*/
int qseecom_start_app(char *app_name);

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
int qseecom_shutdown_app(int handle);

/**
* Shutdown a Secure App
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
			uint32_t sbuf_len, void *resp_buf, uint32_t rbuf_len);

typedef int (*ListenerCallback)(void*, uint32_t);

/**
 * @param char* service_name
 * 	 The name of the listener service.
 * @param uint32_t  id
 * 	 The id used to register a listener with
 * 	 QSEE in the secure side. This id should
 * 	 be unique.
 * @param ListenerCallback service_cmd_handler
 *   This is the service cmd handler, this cb
 *   is called for a specific service request.
 *   The input params are,
 * 		@param void *buf
 * 		  The shared buffer which contains the
 * 		  request from the secure side. The service
 * 		  also updates this buffer with the response.
 * 		@param uint32_t size
 * 		  The size of the shared buffer.
 *      @return int
 *        Success:	Exit succeeded.
 *        Failure:	Error code (negative only).
 */
struct qseecom_listener_services {
	char *service_name;
	uint32_t  id;
	uint32_t sb_size;
	ListenerCallback service_cmd_handler;
};

/**
* Registers a Listener Service with QSEE
*  This api should be called after all
*  service specific initialization is
*  completed, once this is called the
*  service_cmd_handler for the service can
*  be called.
*
* @param struct qseecom_listener_services listnr
*   Listener structure that contains all the info
*   to register a listener service.
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
int qseecom_register_listener(struct qseecom_listener_services *listnr);

/**
* De-Registers a Listener Service with QSEE
*  This api should be called before exiting lk and
*  all service de-init should be done before calling
*  the api. service_cmd_handler will not be called
*  after this api is called.
*
* @param uint32_t listnr_id
*   Pre-defined Listener ID to be de-registered
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
int qseecom_deregister_listener(uint32_t listnr_id);

#endif /* __QSEECOM_API_LK_H_ */
