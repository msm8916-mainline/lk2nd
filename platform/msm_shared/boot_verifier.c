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

#include <stdlib.h>
#include <stdint.h>
#include <crypto_hash.h>
#include <boot_verifier.h>
#include <image_verify.h>
#include <mmc.h>
#include <oem_keystore.h>
#include <openssl/asn1t.h>
#include <openssl/x509.h>
#include <partition_parser.h>
#include <rsa.h>
#include <string.h>
#include <openssl/err.h>
#include <platform.h>
#include <qseecom_lk_api.h>
#include <secapp_loader.h>
#include <target.h>
#include "bootimg.h"

#define ASN1_ENCODED_SHA256_SIZE 0x33
#define ASN1_ENCODED_SHA256_OFFSET 0x13
#define ASN1_SIGNATURE_BUFFER_SZ   mmc_page_size()

static KEYSTORE *oem_keystore;
static KEYSTORE *user_keystore;
static uint32_t dev_boot_state = RED;
char KEYSTORE_PTN_NAME[] = "keystore";
RSA *rsa_from_cert = NULL;
unsigned char fp[EVP_MAX_MD_SIZE];
uint32_t fp_size;
#if OSVERSION_IN_BOOTIMAGE
km_boot_state_t boot_state_info;
#endif

ASN1_SEQUENCE(AUTH_ATTR) ={
	ASN1_SIMPLE(AUTH_ATTR, target, ASN1_PRINTABLESTRING),
	ASN1_SIMPLE(AUTH_ATTR, len, ASN1_INTEGER)
} ASN1_SEQUENCE_END(AUTH_ATTR)
IMPLEMENT_ASN1_FUNCTIONS(AUTH_ATTR)

	ASN1_SEQUENCE(VERIFIED_BOOT_SIG) = {
		ASN1_SIMPLE(VERIFIED_BOOT_SIG, version, ASN1_INTEGER),
		ASN1_SIMPLE(VERIFIED_BOOT_SIG, certificate, X509),
		ASN1_SIMPLE(VERIFIED_BOOT_SIG, algor, X509_ALGOR),
		ASN1_SIMPLE(VERIFIED_BOOT_SIG, auth_attr, AUTH_ATTR),
		ASN1_SIMPLE(VERIFIED_BOOT_SIG, sig, ASN1_OCTET_STRING)
	} ASN1_SEQUENCE_END(VERIFIED_BOOT_SIG)
IMPLEMENT_ASN1_FUNCTIONS(VERIFIED_BOOT_SIG)

	ASN1_SEQUENCE(KEY) = {
		ASN1_SIMPLE(KEY, algorithm_id, X509_ALGOR),
		ASN1_SIMPLE(KEY, key_material, RSAPublicKey)
	}ASN1_SEQUENCE_END(KEY)
IMPLEMENT_ASN1_FUNCTIONS(KEY);

ASN1_SEQUENCE(KEYBAG) = {
	ASN1_SIMPLE(KEYBAG, mykey, KEY)
}ASN1_SEQUENCE_END(KEYBAG)
IMPLEMENT_ASN1_FUNCTIONS(KEYBAG)

	ASN1_SEQUENCE(KEYSTORE_INNER) = {
		ASN1_SIMPLE(KEYSTORE_INNER, version, ASN1_INTEGER),
		ASN1_SIMPLE(KEYSTORE_INNER, mykeybag, KEYBAG)
	} ASN1_SEQUENCE_END(KEYSTORE_INNER)
IMPLEMENT_ASN1_FUNCTIONS(KEYSTORE_INNER)

	ASN1_SEQUENCE(KEYSTORE) = {
		ASN1_SIMPLE(KEYSTORE, version, ASN1_INTEGER),
		ASN1_SIMPLE(KEYSTORE, mykeybag, KEYBAG),
		ASN1_SIMPLE(KEYSTORE, sig, VERIFIED_BOOT_SIG)
	} ASN1_SEQUENCE_END(KEYSTORE)
IMPLEMENT_ASN1_FUNCTIONS(KEYSTORE)

uint32_t read_der_message_length(unsigned char* input, unsigned sz)
{
	uint32_t len = 0;
	uint32_t pos = 0;
	uint8_t len_bytes = 1;

	/* Check if input starts with Sequence id (0X30) */
	if(sz < 3 || input[pos] != 0x30)
		return len;
	pos++;

	/* A length of 0xAABBCCDD in DER encoded messages would be sequence of
	   following octets 0xAA, 0xBB, 0XCC, 0XDD.

	   To read length - read each octet and shift left by 1 octect before
	   reading next octet.
	*/
	/* check if short or long length form */
	if(input[pos] & 0x80)
	{
		len_bytes = (input[pos] & ~(0x80));
		pos++;
	}

	while(len_bytes)
	{
		/* Shift len by 1 octet, make sure to check unsigned int overflow */
		if (len <= (UINT_MAX >> 8))
			len <<= 8;
		else
		{
			dprintf(CRITICAL, "Error: Length exceeding max size of uintmax\n");
			return 0;
		}

		/* Read next octet */
		if (pos < (uint32_t) ASN1_SIGNATURE_BUFFER_SZ && pos < sz)
			len = len | input[pos];
		else
		{
			dprintf(CRITICAL, "Error: Pos index exceeding the input buffer size\n");
			return 0;
		}

		pos++; len_bytes--;
	}

	/* Add number of octets representing sequence id and length  */
	if ((UINT_MAX - pos) > len)
		len += pos;
	else
	{
		dprintf(CRITICAL, "Error: Len overflows UINT_MAX value\n");
		return 0;
	}

	return len;
}

static int add_attribute_to_img(unsigned char *ptr, AUTH_ATTR *input)
{
	return i2d_AUTH_ATTR(input, &ptr);
}

bool boot_verify_compare_sha256(unsigned char *image_ptr,
		unsigned int image_size, unsigned char *signature_ptr, RSA *rsa)
{
	int ret = -1;
	bool auth = false;
	unsigned char *plain_text = NULL;

	/* The magic numbers here are drawn from the PKCS#1 standard and are the ASN.1
	 *encoding of the SHA256 object identifier that is required for a PKCS#1
	* signature.*/
	uint8_t digest[ASN1_ENCODED_SHA256_SIZE] = {0x30, 0x31, 0x30, 0x0d, 0x06,
												0x09, 0x60, 0x86, 0x48, 0x01,
												0x65, 0x03, 0x04, 0x02, 0x01,
												0x05, 0x00, 0x04, 0x20};

	plain_text = (unsigned char *)calloc(sizeof(char), SIGNATURE_SIZE);
	if (plain_text == NULL) {
		dprintf(CRITICAL, "boot_verifier: Calloc failed during verification\n");
		goto cleanup;
	}

	/* Calculate SHA256 of image and place it into the ASN.1 structure*/
	image_find_digest(image_ptr, image_size, CRYPTO_AUTH_ALG_SHA256,
			digest + ASN1_ENCODED_SHA256_OFFSET);

	/* Find digest from the image. This performs the PKCS#1 padding checks up to
	 * but not including the ASN.1 OID and hash function check. The return value
	 * is not positive for a failure or the length of the part after the padding */
	ret = image_decrypt_signature_rsa(signature_ptr, plain_text, rsa);

	/* Make sure the length returned from rsa decrypt is same as x509 signature format
	 * otherwise the signature is invalid and we fail
	 */
	if (ret != ASN1_ENCODED_SHA256_SIZE)
	{
		dprintf(CRITICAL, "boot_verifier: Signature decrypt failed! Signature invalid = %d\n",
			ret);
		goto cleanup;
	}
	/* So plain_text contains the ASN.1 encoded hash from the signature and
	* digest contains the value that this should be for the image that we're
	* verifying, so compare them.*/

	ret = memcmp(plain_text, digest, ASN1_ENCODED_SHA256_SIZE);
	if(ret == 0)
	{
		auth = true;
#ifdef TZ_SAVE_KERNEL_HASH
		save_kernel_hash((unsigned char *) digest + ASN1_ENCODED_SHA256_OFFSET, CRYPTO_AUTH_ALG_SHA256);
#endif
	}

cleanup:
	if (plain_text != NULL)
		free(plain_text);
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
	ERR_remove_thread_state(NULL);
	return auth;

}

static bool verify_image_with_sig(unsigned char* img_addr, uint32_t img_size,
		char *pname, VERIFIED_BOOT_SIG *sig, KEYSTORE *ks)
{
	bool ret = false;
	uint32_t len;
	int shift_bytes;
	RSA *rsa = NULL;
	bool keystore_verification = false;
	EVP_PKEY* key = NULL;
	int attr = 0;

	if(!strcmp(pname, "keystore"))
		keystore_verification = true;

	/* Verify target name */
	if(strncmp((char*)(sig->auth_attr->target->data), pname,
				sig->auth_attr->target->length) ||
				(strlen(pname) != sig->auth_attr->target->length))
	{
		dprintf(CRITICAL,
				"boot_verifier: verification failure due to target name mismatch\n");
		goto verify_image_with_sig_error;
	}
	/* Read image size from signature */
	/* A len = 0xAABBCC (represented by 3 octets) would be stored in
	   len->data as 0X00CCBBAA and len->length as 3(octets).

	   To read len we need to left shift data to number of missing octets and
	   then change it to host long
	 */
	len = *((uint32_t*)sig->auth_attr->len->data);
	shift_bytes = sizeof(uint32_t) - sig->auth_attr->len->length;
	if(shift_bytes > 0) {
		len = len << (shift_bytes*8);
	}
	len = ntohl(len);

	/* Verify image size*/
	if(len != img_size)
	{
		dprintf(CRITICAL,
				"boot_verifier: image length is different. (%d vs %d)\n",
				len, img_size);
		goto verify_image_with_sig_error;
	}

	/* append attribute to image */
	if(!keystore_verification)
	{
		// verifying a non keystore partition
		attr = add_attribute_to_img((unsigned char*)(img_addr + img_size),
				sig->auth_attr);
		if (img_size > (UINT_MAX - attr))
		{
			dprintf(CRITICAL,"Interger overflow detected\n");
			ASSERT(0);
		}
		else img_size += attr;
	}

	/* compare SHA256SUM of image with value in signature */
	if(ks != NULL)
	{
		// use rsa from keystore
		rsa = ks->mykeybag->mykey->key_material;
	}
	else
	{
		dprintf(CRITICAL, "%s:%d: Keystore is null\n", __func__, __LINE__);
		ASSERT(0);
	}

	// verify boot.img with rsa from oem keystore
	if((ret = boot_verify_compare_sha256(img_addr, img_size,
			(unsigned char*)sig->sig->data, rsa)))

	{
		dprintf(SPEW, "Verified boot.img with oem keystore\n");
		boot_verify_send_event(BOOTIMG_KEYSTORE_VERIFICATION_PASS);
		goto verify_image_with_sig_done;
	}
	else
	{
		dprintf(INFO, "Verification with oem keystore failed. Use embedded certificate for verification\n");
		// get the public key from certificate in boot.img
		if ((key = X509_get_pubkey(sig->certificate)))
		{
			// convert to rsa key format
			dprintf(INFO, "RSA KEY found from the embedded certificate\n");
			rsa = EVP_PKEY_get1_RSA(key);
			rsa_from_cert = rsa;
		}
		else
		{
			dprintf(CRITICAL, "Unable to extract public key from certificate\n");
			ASSERT(0);
		}
	}

	// verify boot.img with rsa from embedded certificate
	if ((ret = boot_verify_compare_sha256(img_addr, img_size,
			(unsigned char*)sig->sig->data, rsa)))
	{
		dprintf(SPEW, "Verified boot.img with embedded certificate in boot image\n");
		boot_verify_send_event(BOOTIMG_EMBEDDED_CERT_VERIFICATION_PASS);
		goto verify_image_with_sig_done;
	}
	else
	{
		dprintf(INFO, "verified for red state\n");
		boot_verify_send_event(BOOTIMG_VERIFICATION_FAIL);
		goto verify_image_with_sig_done;
	}

verify_image_with_sig_error:
	boot_verify_send_event(BOOTIMG_VERIFICATION_FAIL);
verify_image_with_sig_done:
	return ret;
}

static int encode_inner_keystore(unsigned char *ptr, KEYSTORE *ks)
{
	int ret = 0;
	KEYSTORE_INNER  *ks_inner = KEYSTORE_INNER_new();
	if (ks_inner == NULL)
		return ret;
	ASN1_INTEGER *tmp_version = ks_inner->version;
	KEYBAG *tmp_mykeybag = ks_inner->mykeybag;

	ks_inner->version = ks->version;
	ks_inner->mykeybag = ks->mykeybag;
	ret = i2d_KEYSTORE_INNER(ks_inner, &ptr);

	ks_inner->version = tmp_version;
	ks_inner->mykeybag = tmp_mykeybag;

	if(ks_inner != NULL)
		KEYSTORE_INNER_free(ks_inner);
	return ret;
}

static bool verify_keystore(unsigned char * ks_addr, KEYSTORE *ks)
{
	bool ret = false;
	unsigned char * ptr = ks_addr;
	uint32_t inner_len = encode_inner_keystore(ptr, ks);
	ret = verify_image_with_sig(ks_addr, inner_len, "keystore", ks->sig,
			oem_keystore);
	return ret;
}

static void read_oem_keystore()
{
	KEYSTORE *ks = NULL;
	uint32_t len = sizeof(OEM_KEYSTORE);
	unsigned char *input = OEM_KEYSTORE;

	if(oem_keystore != NULL)
		return;

	ks = d2i_KEYSTORE(NULL, &input, len);
	if(ks != NULL)
	{
		oem_keystore = ks;
		user_keystore = ks;
	}
}

uint32_t boot_verify_keystore_init()
{
	/* Read OEM Keystore */
	read_oem_keystore();

	return dev_boot_state;
}

#if OSVERSION_IN_BOOTIMAGE
static void boot_verify_send_boot_state(km_boot_state_t *boot_state)
{
	km_get_version_req_t version_req;
	km_get_version_rsp_t version_rsp;
	int ret;
	int app_handle = get_secapp_handle();
	km_set_boot_state_req_t *bs_req = NULL;
	km_set_boot_state_rsp_t boot_state_rsp;
	uint8_t *boot_state_ptr;

	version_req.cmd_id = KEYMASTER_GET_VERSION;
	ret = qseecom_send_command(app_handle, (void*) &version_req, sizeof(version_req), (void*) &version_rsp, sizeof(version_rsp));
	if (ret < 0 || version_rsp.status < 0)
	{
		dprintf(CRITICAL, "QSEEcom command for getting keymaster version returned error: %d\n", version_rsp.status);
		ASSERT(0);
	}

	if (version_rsp.major_version >= 0x2)
	{
		bs_req = malloc(sizeof(km_set_boot_state_req_t) + sizeof(km_boot_state_t));
		ASSERT(bs_req);

		boot_state_ptr = (uint8_t *) bs_req + sizeof(km_set_boot_state_req_t);
		/* copy the boot state data */
		memscpy(boot_state_ptr, sizeof(km_boot_state_t), &boot_state_info, sizeof(boot_state_info));

		bs_req->cmd_id = KEYMASTER_SET_BOOT_STATE;
		bs_req->version = 0x0;
		bs_req->boot_state_offset = sizeof(km_set_boot_state_req_t);
		bs_req->boot_state_size = sizeof(km_boot_state_t);

		ret = qseecom_send_command(app_handle, (void *)bs_req, sizeof(*bs_req) + sizeof(km_boot_state_t), (void *) &boot_state_rsp, sizeof(boot_state_rsp));
		if (ret < 0 || boot_state_rsp.status < 0)
		{
			dprintf(CRITICAL, "QSEEcom command for Sending boot state returned error: %d\n", boot_state_rsp.status);
			free(bs_req);
			ASSERT(0);
		}
	}

	if (bs_req)
		free(bs_req);
}
#endif

bool send_rot_command(uint32_t is_unlocked)
{
	int ret = 0;
	unsigned char *input = NULL;
	char *rot_input = NULL;
	unsigned int digest[9] = {0}, final_digest[8] = {0};
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA256;
	uint32_t boot_device_state = boot_verify_get_state();
	int app_handle = 0;
	uint32_t len_oem_rsa = 0, len_from_cert = 0;
	km_set_rot_req_t *read_req;
	km_set_rot_rsp_t read_rsp;
	app_handle = get_secapp_handle();
	int n = 0, e = 0;
	switch (boot_device_state)
	{
		case GREEN:
			// Locked device and boot.img verified against OEM keystore.
			// Send hash of key from OEM KEYSTORE + Boot device state
			n = BN_num_bytes(oem_keystore->mykeybag->mykey->key_material->n);
			e = BN_num_bytes(oem_keystore->mykeybag->mykey->key_material->e);
			/*this assumes a valid acceptable range for RSA, including 4096 bits of modulo n. */
			if (n<0 || n>1024)
			{
				dprintf(CRITICAL, "Invalid n value from key_material\n");
				ASSERT(0);
			}
			/* e can assumes 3,5,17,257,65537 as valid values, which should be 1 byte long only, we accept 2 bytes or 16 bits long */
			if( e < 0 || e >16)
			{
				dprintf(CRITICAL, "Invalid e value from key_material\n");
				ASSERT(0);
			}
			len_oem_rsa = n + e;
			if(!(input = malloc(len_oem_rsa)))
			{
				dprintf(CRITICAL, "Failed to allocate memory for ROT structure\n");
				ASSERT(0);
			}
			BN_bn2bin(oem_keystore->mykeybag->mykey->key_material->n, input);
			BN_bn2bin(oem_keystore->mykeybag->mykey->key_material->e, input+n);
			hash_find((unsigned char *)input, len_oem_rsa, (unsigned char *) &digest, auth_algo);
			digest[8] = is_unlocked;
			break;
		case YELLOW:
		case RED:
			// Locked device and boot.img passed (yellow) or failed (red) verification with the certificate embedded to the boot.img.
			if (!rsa_from_cert)
			{
				dprintf(CRITICAL, "RSA is null from the embedded certificate\n");
				ASSERT(0);
			}
			// Send hash of key from certificate in boot image + boot device state
			n = BN_num_bytes(rsa_from_cert->n);
			e = BN_num_bytes(rsa_from_cert->e);
			/*this assumes a valid acceptable range for RSA, including 4096 bits of modulo n. */
			if (n<0 || n>1024)
			{
				dprintf(CRITICAL, "Invalid n value from rsa_from_cert\n");
				ASSERT(0);
			}
			/* e can assumes 3,5,17,257,65537 as valid values, which should be 1 byte long only, we accept 2 bytes or 16 bits long */
			if( e < 0 || e >16)
			{
				dprintf(CRITICAL, "Invalid e value from rsa_from_cert\n");
				ASSERT(0);
			}
			len_from_cert = n + e;
			if(!(input = malloc(len_from_cert)))
			{
				dprintf(CRITICAL, "Failed to allocate memory for ROT structure\n");
				ASSERT(0);
			}
			BN_bn2bin(rsa_from_cert->n, input);
			BN_bn2bin(rsa_from_cert->e, input+n);
			hash_find((unsigned char *)input, len_from_cert, (unsigned char *) &digest, auth_algo);
			digest[8] = is_unlocked;
			break;
		case ORANGE:
			// Unlocked device and no verification done.
			// Send the hash of boot device state
			input = NULL;
			digest[0] = is_unlocked;
			break;
	}

	hash_find((unsigned char *) digest, sizeof(digest), (unsigned char *)&final_digest, auth_algo);
	dprintf(SPEW, "Digest: ");
	for(uint8_t i = 0; i < 8; i++)
		dprintf(SPEW, "0x%x ", final_digest[i]);
	dprintf(SPEW, "\n");
	if(!(read_req = malloc(sizeof(km_set_rot_req_t) + sizeof(final_digest))))
	{
		dprintf(CRITICAL, "Failed to allocate memory for ROT structure\n");
		ASSERT(0);
	}

	void *cpy_ptr = (uint8_t *) read_req + sizeof(km_set_rot_req_t);
	// set ROT stucture
	read_req->cmd_id = KEYMASTER_SET_ROT;
	read_req->rot_ofset = (uint32_t) sizeof(km_set_rot_req_t);
	read_req->rot_size  = sizeof(final_digest);
	// copy the digest
	memcpy(cpy_ptr, (void *) &final_digest, sizeof(final_digest));
	dprintf(SPEW, "Sending Root of Trust to trustzone: start\n");

	ret = qseecom_send_command(app_handle, (void*) read_req, sizeof(km_set_rot_req_t) + sizeof(final_digest), (void*) &read_rsp, sizeof(read_rsp));
	if (ret < 0 || read_rsp.status < 0)
	{
		dprintf(CRITICAL, "QSEEcom command for Sending Root of Trust returned error: %d\n", read_rsp.status);
		if(input)
			free(input);
		free(read_req);
		free(rot_input);
		return false;
	}

#if OSVERSION_IN_BOOTIMAGE
	boot_state_info.is_unlocked = is_unlocked;
	boot_state_info.color = boot_verify_get_state();
	memscpy(boot_state_info.public_key, sizeof(boot_state_info.public_key), digest, 32);
	boot_verify_send_boot_state(&boot_state_info);
#endif
	dprintf(SPEW, "Sending Root of Trust to trustzone: end\n");
	if(input)
		free(input);
	free(read_req);
	free(rot_input);
	return true;
}

unsigned char* get_boot_fingerprint(unsigned int* buf_size)
{
	*buf_size = fp_size;

	return fp;
}

bool boot_verify_image(unsigned char* img_addr, uint32_t img_size, char *pname)
{
	bool ret = false;
	X509 *cert = NULL;
	const EVP_MD *fp_type = NULL;
	VERIFIED_BOOT_SIG *sig = NULL;
	unsigned char* sig_addr = (unsigned char*)(img_addr + img_size);
	uint32_t sig_len = 0;
	unsigned char *signature = NULL;
#if OSVERSION_IN_BOOTIMAGE
	struct boot_img_hdr *img_hdr = NULL;
#endif

	if(dev_boot_state == ORANGE)
	{
		dprintf(INFO, "boot_verifier: Device is in ORANGE boot state.\n");
		dprintf(INFO, "boot_verifier: Skipping boot verification.\n");
		return false;
	}

	signature = malloc(ASN1_SIGNATURE_BUFFER_SZ);
	ASSERT(signature);

	/* Copy the signature from scratch memory to buffer */
	memcpy(signature, sig_addr, ASN1_SIGNATURE_BUFFER_SZ);
	sig_len = read_der_message_length(signature, ASN1_SIGNATURE_BUFFER_SZ);

	if(!sig_len)
	{
		dprintf(CRITICAL, "boot_verifier: Error while reading signature length.\n");
		goto verify_image_error;
	}

	if (sig_len > ASN1_SIGNATURE_BUFFER_SZ)
	{
		dprintf(CRITICAL, "boot_verifier: Signature length exceeds size signature buffer\n");
		goto verify_image_error;
	}

	if((sig = d2i_VERIFIED_BOOT_SIG(NULL, (const unsigned char **) &sig_addr, sig_len)) == NULL)
	{
		dprintf(CRITICAL,
				"boot_verifier: verification failure due to target name mismatch\n");
		goto verify_image_error;
	}

	cert = sig->certificate;
	fp_type = EVP_sha1();
	if(!X509_digest(cert, fp_type, (unsigned char *)fp, &fp_size)) {
		dprintf(INFO,"Fail to create certificate fingerprint.\n");
	}

	ret = verify_image_with_sig(img_addr, img_size, pname, sig, user_keystore);

#if OSVERSION_IN_BOOTIMAGE
	/* Extract the os version and patch level */
	img_hdr = (struct boot_img_hdr *)img_addr;
	boot_state_info.system_version = (img_hdr->os_version & 0xFFFFF8) >> 11;
	boot_state_info.system_security_level = (img_hdr->os_version & 0x7FF);
#endif

	if(sig != NULL)
		VERIFIED_BOOT_SIG_free(sig);
verify_image_error:
	free(signature);
	return ret;
}

void boot_verify_send_event(uint32_t event)
{
	switch(event)
	{
		case BOOT_INIT:
			dev_boot_state = GREEN;
			break;
		case BOOTIMG_KEYSTORE_VERIFICATION_PASS:
			dev_boot_state = GREEN;
			break;
		case BOOTIMG_EMBEDDED_CERT_VERIFICATION_PASS:
			if(dev_boot_state == GREEN)
				dev_boot_state = YELLOW;
			break;
		case BOOTIMG_VERIFICATION_FAIL:
			if(dev_boot_state == GREEN || dev_boot_state == YELLOW)
				dev_boot_state = RED;
			break;
		case DEV_UNLOCK:
			dev_boot_state = ORANGE;
			break;
		case USER_DENIES:
			if(dev_boot_state == YELLOW || dev_boot_state == ORANGE)
				dev_boot_state = RED;
			break;
	}
}

uint32_t boot_verify_get_state()
{
	return dev_boot_state;
}

void boot_verify_print_state()
{
	switch(dev_boot_state)
	{
		case GREEN:
			dprintf(INFO, "boot_verifier: Device is in GREEN boot state.\n");
			break;
		case ORANGE:
			dprintf(INFO, "boot_verifier: Device is in ORANGE boot state.\n");
			break;
		case YELLOW:
			dprintf(INFO, "boot_verifier: Device is in YELLOW boot state.\n");
			break;
		case RED:
			display_fbcon_message("Security Error:  This phone has been flashed with unauthorized software & is locked. Call your mobile operator for additional support.Please note that				repair/return for this issue may have additional cost.\n");

			dprintf(INFO, "boot_verifier: Device is in RED boot state.\n");
			break;
	}
}

bool boot_verify_validate_keystore(unsigned char * user_addr, unsigned sz)
{
	bool ret = false;
	unsigned char *input = user_addr;
	KEYSTORE *ks = NULL;
	uint32_t len = read_der_message_length(input, sz);
	if(!len)
	{
		dprintf(CRITICAL, "boot_verifier: keystore length is invalid.\n");
		return ret;
	}

	ks = d2i_KEYSTORE(NULL, &input, len);
	if(ks != NULL)
	{
		ret = true;
	}
	return ret;
}

static bool check_list(char**list, char* entry)
{
	int i = 0;
	if(list == NULL || entry == NULL)
		return false;

	while(*list != NULL)
	{
		if(!strcmp(entry, *list))
			return true;

		list++;
	}

	return false;
}

KEYSTORE *boot_gerity_get_oem_keystore()
{
	read_oem_keystore();
	return oem_keystore;
}
