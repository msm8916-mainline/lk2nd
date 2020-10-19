/*
 * Copyright (c) 2014-2016, The Linux Foundation. All rights reserved.
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

#if !defined(__BOOT_VERIFIER_H) && VERIFIED_BOOT
#define __BOOT_VERIFIER_H

#include <asn1.h>
#include <rsa.h>

/**
 *    AndroidVerifiedBootSignature DEFINITIONS ::=
 *    BEGIN
 *        FormatVersion ::= INTEGER
 *        Certificate ::= Certificate
 *        AlgorithmIdentifier ::=  SEQUENCE {
 *            algorithm OBJECT IDENTIFIER,
 *            parameters ANY DEFINED BY algorithm OPTIONAL
 *        }
 *        AuthenticatedAttributes ::= SEQUENCE {
 *            target CHARACTER STRING,
 *            length INTEGER
 *        }
 *        Signature ::= OCTET STRING
 *     END
 */

typedef struct auth_attr_st
{
	ASN1_PRINTABLESTRING *target;
	ASN1_INTEGER *len;
}AUTH_ATTR;

DECLARE_STACK_OF(AUTH_ATTR)
DECLARE_ASN1_SET_OF(AUTH_ATTR)
DECLARE_ASN1_FUNCTIONS(AUTH_ATTR)

typedef struct verif_boot_sig_st
{
	ASN1_INTEGER *version;
	X509 *certificate;
	X509_ALGOR *algor;
	AUTH_ATTR *auth_attr;
	ASN1_OCTET_STRING *sig;
}VERIFIED_BOOT_SIG;

DECLARE_STACK_OF(VERIFIED_BOOT_SIG)
DECLARE_ASN1_SET_OF(VERIFIED_BOOT_SIG)
DECLARE_ASN1_FUNCTIONS(VERIFIED_BOOT_SIG)

/**
 * AndroidVerifiedBootKeystore DEFINITIONS ::=
 * BEGIN
 *     FormatVersion ::= INTEGER
 *     KeyBag ::= SEQUENCE {
 *         Key  ::= SEQUENCE {
 *             AlgorithmIdentifier  ::=  SEQUENCE {
 *                 algorithm OBJECT IDENTIFIER,
 *                 parameters ANY DEFINED BY algorithm OPTIONAL
 *             }
 *             KeyMaterial ::= RSAPublicKey
 *         }
 *     }
 *     Signature ::= AndroidVerifiedBootSignature
 * END
 */

typedef struct key_st
{
	X509_ALGOR *algorithm_id;
	RSA *key_material;
}KEY;

DECLARE_STACK_OF(KEY)
DECLARE_ASN1_SET_OF(KEY)
DECLARE_ASN1_FUNCTIONS(KEY)

typedef struct keybag_st
{
	KEY *mykey;
}KEYBAG;

DECLARE_STACK_OF(KEYBAG)
DECLARE_ASN1_SET_OF(KEYBAG)
DECLARE_ASN1_FUNCTIONS(KEYBAG)

typedef struct keystore_inner_st
{
	ASN1_INTEGER *version;
	KEYBAG *mykeybag;
}KEYSTORE_INNER;

DECLARE_STACK_OF(KEYSTORE_INNER)
DECLARE_ASN1_SET_OF(KEYSTORE_INNER)
DECLARE_ASN1_FUNCTIONS(KEYSTORE_INNER)

typedef struct keystore_st
{
	ASN1_INTEGER *version;
	KEYBAG *mykeybag;
	VERIFIED_BOOT_SIG *sig;
}KEYSTORE;

DECLARE_STACK_OF(KEYSTORE)
DECLARE_ASN1_SET_OF(KEYSTORE)
DECLARE_ASN1_FUNCTIONS(KEYSTORE)

enum boot_state
{
	GREEN,
	ORANGE,
	YELLOW,
	RED,
};

struct verified_boot_verity_mode
{
	bool verity_mode_enforcing;
	char *name;
};

struct verified_boot_state_name
{
	uint32_t boot_state;
	char *name;
};

enum boot_verfiy_event
{
	BOOT_INIT,
	DEV_UNLOCK,
	BOOTIMG_EMBEDDED_CERT_VERIFICATION_PASS,
	BOOTIMG_KEYSTORE_VERIFICATION_PASS,
	BOOTIMG_VERIFICATION_FAIL,
	USER_DENIES,
};

extern char KEYSTORE_PTN_NAME[];
/* Function to initialize keystore */
uint32_t boot_verify_keystore_init();
/* Function to verify boot/recovery image */
bool boot_verify_image(unsigned char* img_addr, uint32_t img_size, char *pname);
/* Function to send event to boot state machine */
void boot_verify_send_event(uint32_t event);
/* Read current boot state */
uint32_t boot_verify_get_state();
/* Print current boot state */
void boot_verify_print_state();
/* Function to validate keystore */
bool boot_verify_validate_keystore(unsigned char * user_addr, unsigned sz);
/* Function to check if partition is allowed to flash in verified mode */
bool boot_verify_flash_allowed(const char * entry);
bool boot_verify_compare_sha256(unsigned char *image_ptr,
		unsigned int image_size, unsigned char *signature_ptr, RSA *rsa);
KEYSTORE *boot_gerity_get_oem_keystore();
/* Function to send root of trust to trust zone */
bool send_rot_command(uint32_t is_unlocked);
unsigned char* get_boot_fingerprint(unsigned int* buf_size);
uint32_t read_der_message_length(unsigned char* input, unsigned sz);
#endif
