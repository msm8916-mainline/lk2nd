/*
 * Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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
#include <boot_verifier.h>
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
    KEYMASTER_GET_SUPPORTED_ALGORITHMS		= (KEYMASTER_CMD_ID + 1UL),
    KEYMASTER_GET_SUPPORTED_BLOCK_MODES		= (KEYMASTER_CMD_ID + 2UL),
    KEYMASTER_GET_SUPPORTED_PADDING_MODES	= (KEYMASTER_CMD_ID + 3UL),
    KEYMASTER_GET_SUPPORTED_DIGESTS			= (KEYMASTER_CMD_ID + 4UL),
    KEYMASTER_GET_SUPPORTED_IMPORT_FORMATS	= (KEYMASTER_CMD_ID + 5UL),
    KEYMASTER_GET_SUPPORTED_EXPORT_FORMATS	= (KEYMASTER_CMD_ID + 6UL),
    KEYMASTER_ADD_RNG_ENTROPY				= (KEYMASTER_CMD_ID + 7UL),
    KEYMASTER_GENERATE_KEY					= (KEYMASTER_CMD_ID + 8UL),
    KEYMASTER_GET_KEY_CHARACTERISTICS		= (KEYMASTER_CMD_ID + 9UL),
    KEYMASTER_RESCOPE						= (KEYMASTER_CMD_ID + 10UL),
    KEYMASTER_IMPORT_KEY					= (KEYMASTER_CMD_ID + 11UL),
    KEYMASTER_EXPORT_KEY					= (KEYMASTER_CMD_ID + 12UL),
    KEYMASTER_DELETE_KEY					= (KEYMASTER_CMD_ID + 13UL),
    KEYMASTER_DELETE_ALL_KEYS				= (KEYMASTER_CMD_ID + 14UL),
    KEYMASTER_BEGIN							= (KEYMASTER_CMD_ID + 15UL),
    KEYMASTER_GET_OUTPUT_SIZE				= (KEYMASTER_CMD_ID + 16UL),
    KEYMASTER_UPDATE						= (KEYMASTER_CMD_ID + 17UL),
    KEYMASTER_FINISH						= (KEYMASTER_CMD_ID + 18UL),
    KEYMASTER_ABORT							= (KEYMASTER_CMD_ID + 19UL),
    KEYMASTER_SET_BOOT_STATE                = (KEYMASTER_UTILS_CMD_ID + 8UL),

    KEYMASTER_GET_VERSION                   = (KEYMASTER_UTILS_CMD_ID + 0UL),
    KEYMASTER_SET_ROT						= (KEYMASTER_UTILS_CMD_ID + 1UL),
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
  @param[in]   rot_ofset   Offset from the top of the struct.
  @param[in]   rot_size    Size of the ROT
*/
typedef struct _km_set_rot_req_t {
	uint32 cmd_id;
	uint32 rot_ofset;
	uint32 rot_size;
}__attribute__ ((packed)) km_set_rot_req_t;

/**
  @brief
    Data structure

  @param[out]   status      Status of the request
*/
typedef struct _km_set_rot_rsp_t {
	int status;
}__attribute__ ((packed)) km_set_rot_rsp_t;

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

/*
 * Structures for delete_all cmd
 */
/*
  @brief
  Data structure

  @param[in]  cmd_id              Requested command
*/
typedef struct _key_op_delete_all_req_t {
	uint32 cmd_id;
}__attribute__ ((packed)) key_op_delete_all_req_t;

/*
  @brief
  Data structure

  @param[out]   status             Status of the request
*/
typedef struct _key_op_delete_all_rsp_t {
	int status;
}__attribute__ ((packed)) key_op_delete_all_rsp_t;

typedef enum _secure_write_prot_op_t
{
	SWP_READ_CONFIG,
	SWP_WRITE_CONFIG,
	SWP_LAST_CMD_ENTRY                    = (int)0xFFFFFFFFULL
} secure_write_prot_op_t;

/*
	@brief
	Data structure

	@param[in]   cmd_id                   Command ID of the request
	@param[in]   op                       Secure write protect operation (enum from secure_write_prot_op_t)
	@param[in]   swp_write_data_offset    Offset of data for SWP operation
	@param[in]   swp_write_data_len       Length of data for SWP operation
*/

typedef struct _secure_write_prot_req_t
{
	uint32 cmd_id;
	uint32 op;
	uint32 swp_write_data_offset;
	uint32 swp_write_data_len;
}__attribute__((packed)) secure_write_prot_req_t;

/*
	@brief
	Data structure

	@param[out]   status                  Status of the request
	@param[out]   swp_read_data_offset    Offset of data for SWP operation
	@param[out]   swp_read_data_len       Length of data for SWP operation
*/

typedef struct _secure_write_prot_rsp_t
{
	int status;
	uint32 swp_read_data_offset;
	uint32 swp_read_data_len;
}__attribute__((packed)) secure_write_prot_rsp_t;

/*
 *  * Structures for get_version
 *   */
typedef struct _km_get_version_req_t
{
	uint32_t cmd_id;
}__attribute__((packed)) km_get_version_req_t;

typedef struct _km_get_version_rsp_t
{
	int status;
	uint32_t major_version;
	uint32_t minor_version;
	uint32_t ta_major_version;
	uint32_t ta_minor_version;
}__attribute__((packed)) km_get_version_rsp_t;

typedef struct _km_boot_state_t
{
	bool                   is_unlocked;
	uint8_t                public_key[32];
	uint32_t               color;
	uint32_t               system_version;
	uint32_t               system_security_level;
}__attribute__((packed))  km_boot_state_t;

/**
 *  @brief
 *  Data structure
 *  @param[in]   cmd_id             Requested command
 *  @param[in]   boot_state_ofset   Offset from the top of the struct.
 *  @param[in]   boot_state_size    Size of the Boot state
 *
 *  The offset contains the following
 *  km_boot_state_t
 **/
typedef struct _km_set_boot_state_req_t
{
	uint32_t        cmd_id;
	uint32_t        version;
	uint32_t        boot_state_offset;
	uint32_t        boot_state_size;
}__attribute__((packed)) km_set_boot_state_req_t;

/**
 *  @brief
 *  Data structure
 *
 *  @param[out]   status      Status of the request
 **/
typedef struct _km_set_boot_state_rsp_t
{
	int status;
}__attribute__((packed)) km_set_boot_state_rsp_t;

#endif /* KM_MAIN_H */
