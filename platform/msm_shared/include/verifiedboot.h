/* Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
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

#ifndef __VERIFIEDBOOT_H__
#define __VERIFIEDBOOT_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <mmc.h>
#include <platform.h>
#include <devinfo.h>
#include <meta_format.h>
#include <boot_device.h>
#include <boot_verifier.h>
#include <target.h>

typedef enum {
	NO_AVB	= 0,
	AVB_1,
	AVB_2,
	AVB_LE
} avb_versions;

#define VB_SHA256_SIZE  32
#define LE_BOOTIMG_SIG_SIZE 256
#define abort() ASSERT(false);
#define MAX_PATH_SIZE 64

#define EFIERR(_a)		(-1 * (_a))

#define SALT_BUFF_OFFSET (1024)
#define ADD_SALT_BUFF_OFFSET(addr) (SALT_BUFF_OFFSET + (addr))
#define SUB_SALT_BUFF_OFFSET(addr) ((addr) - SALT_BUFF_OFFSET)

#define EFI_SUCCESS               0
#define EFI_LOAD_ERROR            EFIERR (1)
#define EFI_INVALID_PARAMETER     EFIERR (2)
#define EFI_UNSUPPORTED           EFIERR (3)
#define EFI_BAD_BUFFER_SIZE       EFIERR (4)
#define EFI_BUFFER_TOO_SMALL      EFIERR (5)
#define EFI_NOT_READY             EFIERR (6)
#define EFI_DEVICE_ERROR          EFIERR (7)
#define EFI_WRITE_PROTECTED       EFIERR (8)
#define EFI_OUT_OF_RESOURCES      EFIERR (9)
#define EFI_VOLUME_CORRUPTED      EFIERR (10)
#define EFI_VOLUME_FULL           EFIERR (11)
#define EFI_NO_MEDIA              EFIERR (12)
#define EFI_MEDIA_CHANGED         EFIERR (13)
#define EFI_NOT_FOUND             EFIERR (14)
#define EFI_ACCESS_DENIED         EFIERR (15)
#define EFI_NO_RESPONSE           EFIERR (16)
#define EFI_NO_MAPPING            EFIERR (17)
#define EFI_TIMEOUT               EFIERR (18)
#define EFI_NOT_STARTED           EFIERR (19)
#define EFI_ALREADY_STARTED       EFIERR (20)
#define EFI_ABORTED               EFIERR (21)
#define EFI_ICMP_ERROR            EFIERR (22)

#define EFI_TFTP_ERROR            EFIERR (23)
#define EFI_PROTOCOL_ERROR        EFIERR (24)
#define EFI_INCOMPATIBLE_VERSION  EFIERR (25)
#define EFI_SECURITY_VIOLATION    EFIERR (26)
#define EFI_CRC_ERROR             EFIERR (27)
#define EFI_END_OF_MEDIA          EFIERR (28)
#define EFI_END_OF_FILE           EFIERR (31)
#define EFI_INVALID_LANGUAGE      EFIERR (32)

#define EFI_WARN_UNKNOWN_GLYPH    EFIWARN (1)
#define EFI_WARN_DELETE_FAILURE   EFIWARN (2)
#define EFI_WARN_WRITE_FAILURE    EFIWARN (3)
#define EFI_WARN_BUFFER_TOO_SMALL EFIWARN (4)

typedef int       EFI_STATUS;
typedef bool      BOOLEAN;
typedef char      CHAR8;
typedef int16_t   CHAR16;
typedef void      VOID;
typedef uint8_t   UINT8;
typedef uint16_t  UINT16;
typedef uint32_t  UINT32;
typedef uint64_t  UINT64;
typedef int16_t   INT16;
typedef int32_t   INT32;
typedef int64_t   INT64;
typedef uint64_t  UINTN;

typedef enum {
	VB_UNDEFINED_HASH	= 0,
	VB_SHA1,
	VB_SHA256,
	VB_UNSUPPORTED_HASH,
	VB_RESERVED_HASH	= 0x7fffffff /* force to 32 bits */
} vb_hash;

#define GUARD(code)                                                            \
    do {                                                                       \
        Status = (code);                                                       \
        if (Status != EFI_SUCCESS) {                                           \
            dprintf(CRITICAL, "Err: line:%d %s() status: %d\n", __LINE__,      \
                   __FUNCTION__, Status);                                      \
            return Status;                                                     \
        }                                                                      \
    } while (0)

#define GUARD_OUT(code)                                                        \
    do {                                                                       \
        Status = (code);                                                       \
        if (Status != EFI_SUCCESS) {                                           \
            dprintf(CRITICAL, "Err: line:%d %s() status: %d\n", __LINE__,      \
                   __FUNCTION__, Status);                                      \
            goto out;                                                          \
        }                                                                      \
    } while (0)

/* forward declare bootinfo */
typedef struct bootinfo bootinfo;

BOOLEAN VerifiedBootEnabled();

/**
 * @return  0 - AVB disabled
 *          1 - VB 1.0
 *          2 - VB 2.0
 *          3 - LE VB
 */
UINT32 GetAVBVersion();

/**
 * Authenticates and loads boot image in
 * Info->Images on EFI_SUCCESS.
 * Also provides Verified Boot command
 * arguments (if any) in Info->vbcmdline
 *
 * @return EFI_STATUS
 */
int load_image_and_auth(bootinfo *Info);

/**
 *  Free resources/memory allocated by
 *  verified boot, image_buffer, vbcmdline
 *  VBData...
 *
 * @return VOID
 */
void free_verified_boot_resource(bootinfo *Info);

#endif /* __VERIFIEDBOOT_H__ */
