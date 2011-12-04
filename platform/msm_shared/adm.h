/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
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

#ifndef __PLATFORM_MSM_SHARED_ADM_H
#define __PLATFORM_MSM_SHARED_ADM_H

#include <platform/iomap.h>

/* ADM base address for channel (n) and security_domain (s) */
#define ADM_BASE_ADDR(n, s) (MSM_ADM_BASE + 4*(n) + ((MSM_ADM_SD_OFFSET)*(s)))

/* ADM registers */
#define ADM_REG_CMD_PTR(n, s)   (ADM_BASE_ADDR(n, s) + 0x000)
#define ADM_REG_RSLT(n, s)      (ADM_BASE_ADDR(n, s) + 0x040)
#define ADM_REG_STATUS(n, s)    (ADM_BASE_ADDR(n, s) + 0x200)
#define ADM_REG_IRQ(s)          (ADM_BASE_ADDR(0, s) + 0x380)

/* Result reg bit masks */
#define ADM_REG_RSLT__V___M         (1 << 31)
#define ADM_REG_RSLT__ERR___M       (1 << 3)
#define ADM_REG_RSLT__TPD___M       (1 << 1)

/* Status reg bit masks */
#define ADM_REG_STATUS__RSLT_VLD___M    (1 << 1)

/* Command Pointer List Entry bit masks */
#define ADM_CMD_PTR_LP          (1 << 31)	/* Last pointer */
#define ADM_CMD_PTR_CMD_LIST    (0 << 29)	/* Command List */

/* Command List bit masks */
#define ADM_CMD_LIST_LC         (1 << 31)	/* Last command             */
#define ADM_CMD_LIST_OCU        (1 << 21)	/* Other channel unblock    */
#define ADM_CMD_LIST_OCB        (1 << 20)	/* Other channel block      */
#define ADM_CMD_LIST_TCB        (1 << 19)	/* This channel block       */
#define ADM_ADDR_MODE_BOX       (3 << 0)	/* Box address mode         */
#define ADM_ADDR_MODE_SI        (0 << 0)	/* Single item address mode */

/* ADM external inteface */

/* result type */
typedef enum {

	ADM_RESULT_SUCCESS = 0,
	ADM_RESULT_FAILURE = 1,
	ADM_RESULT_TIMEOUT = 2
} adm_result_t;

/* direction type */
typedef enum {

	ADM_MMC_READ = 0,
	ADM_MMC_WRITE
} adm_dir_t;

adm_result_t adm_transfer_mmc_data(unsigned char slot,
				   unsigned char *data_ptr,
				   unsigned int data_len, adm_dir_t dir);
#endif
