/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.

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

#ifndef KM_MAIN_H
#define KM_MAIN_H

#include <sys/types.h>
/**
 * Commands supported
 */
#define KEYMASTER_CMD_ID  0x100UL
#define KEYMASTER_UTILS_CMD_ID  0x200UL

#define UINT32_MAX  (0xffffffff)

typedef enum {
    /*
     * List the commands supportedin by the hardware.
     */
    KEYMASTER_READ_LK_DEVICE_STATE			= (KEYMASTER_UTILS_CMD_ID + 2UL),
    KEYMASTER_WRITE_LK_DEVICE_STATE			= (KEYMASTER_UTILS_CMD_ID + 3UL),
    KEYMASTER_MILESTONE_CALL				= (KEYMASTER_UTILS_CMD_ID + 4UL),

    KEYMASTER_LAST_CMD_ENTRY				= (int)0xFFFFFFFFULL
} keymaster_cmd_t;


/*
 * Utils Api struct
 */

/**
  @brief
    Data structure

  @param[in]   cmd_id      Requested command
  @param[in]   data        information (could be data or a pointer to the memory that holds the data
  @param[in]   len         if data is ptr to some buffer, len indicates length of the buffer
*/
typedef struct send_cmd{
	uint32 cmd_id;
	uint32 data;
	uint32 len;
} __attribute__ ((packed)) send_cmd_t;

/*
typedef struct send_cmd_rsp{
	uint32 cmd_id;
	uint32 data;
	int32 status;
}  __attribute__ ((packed)) send_cmd_rsp_t; */

/**
  @brief
    Data structure

  @param[in]   cmd_id      Requested command
*/
typedef struct _km_set_milestone_req_t {
	uint32 cmd_id;
}__attribute__ ((packed)) km_set_milestone_req_t;

/**
  @brief
    Data structure

  @param[out]   status      Status of the request
*/
typedef struct _km_set_milestone_rsp_t {
	int status;
}__attribute__ ((packed)) km_set_milestone_rsp_t;

#endif /* KM_MAIN_H */
