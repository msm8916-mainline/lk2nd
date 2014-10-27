/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

/* RPMB request and response types */
enum rpmb_rr_type {
	KEY_PROVISION = 0x1,
	READ_WRITE_COUNTER,
	AUTH_WRITE,
	AUTH_READ,
	READ_RESULT_FLAG,
};

/* List of all return codes for rpmb frame verification from response */
enum rpmb_verify_return_codes
{
	NONCE_MISMATCH = 0x100,
};

/* These are error codes returned for RPMB operations */
enum rpmb_verify_error_codes
{
	OPERATION_OK = 0,
	GENERAL_FAILURE,
	AUTH_FAILURE,
	COUNTER_FAILURE,
	ADDRESS_FAILURE,
	WRITE_FAILURE,
	READ_FAILURE,
	KEY_NOT_PROG,
	MAXED_WR_COUNTER = 0x80,
};

/* RPMB Frame */
struct rpmb_frame
{
	uint8_t  stuff_bytes[196];
	uint8_t  keyMAC[32];
	uint8_t  data[256];
	uint8_t  nonce[16];
	uint32_t writecounter;
	uint16_t address;
	uint16_t blockcount;
	uint8_t  result[2];
	uint16_t requestresponse;
};
