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
#ifndef _UIC_H_
#define _UIC_H_

#include <sys/types.h>

#define HCI_UIC_TIMEOUT                                  100000
#define HCI_ENABLE_TIMEOUT                               100000

enum uic_error_codes
{
	UIC_ERR_UNKNOWN,
	UIC_ERR_TIMEOUT,
};

enum uic_gen_err_code
{
	UICCMD_SUCCESS,
	UICCMD_FAILURE,
};

enum uic_num_cmd_args
{
	UICCMD_NO_ARGS    = 0,
	UICCMD_ONE_ARGS   = 1,
	UICCMD_TWO_ARGS   = 2,
	UICCMD_THREE_ARGS = 4,
};

struct uic_cmd
{
	uint32_t uiccmd;
	uint32_t num_args;
	uint32_t uiccmdarg1;
	uint32_t uiccmdarg2;
	uint32_t uiccmdarg3;
	uint32_t gen_err_code;
	uint32_t timeout_msecs;
};

/* UFS init settings. */
#define UFS_SYS1CLK_1US_VAL                              100
#define UFS_TX_SYMBOL_CLK_1US_VAL                        1
#define UFS_CLK_NS_REG_VAL                               10
#define UFS_PA_LINK_STARTUP_TIMER_VAL                    20000000
#define UFS_LINK_STARTUP_RETRY                           10

#define SHFT_CLK_NS_REG                                  (10)

int uic_init(struct ufs_dev *dev);
int uic_send_cmd(struct ufs_dev *dev, struct uic_cmd *cmd);
int uic_reset(struct ufs_dev *dev);


#endif
