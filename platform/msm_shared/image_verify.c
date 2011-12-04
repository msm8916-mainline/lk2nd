/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <x509.h>
#include <certificate.h>
#include <crypto_hash.h>
#include "image_verify.h"

/*
 * Returns -1 if decryption failed otherwise size of plain_text in bytes
 */
static int
image_decrypt_signature(unsigned char *signature_ptr, unsigned char *plain_text)
{
	/*
	 * Extract Public Key and Decrypt Signature
	 */
	int ret = -1;
	X509 *x509_certificate = NULL;
	unsigned char *cert_ptr = certBuffer;
	unsigned int cert_size = sizeof(certBuffer);
	EVP_PKEY *pub_key = NULL;
	RSA *rsa_key = NULL;

	/*
	 * Get Pubkey and Convert the internal EVP_PKEY to RSA internal struct
	 */
	if ((x509_certificate = d2i_X509(NULL, &cert_ptr, cert_size)) == NULL) {
		dprintf(CRITICAL,
			"ERROR: Image Invalid, X509_Certificate is NULL!\n");
		goto cleanup;
	}
	pub_key = X509_get_pubkey(x509_certificate);
	rsa_key = EVP_PKEY_get1_RSA(pub_key);
	if (rsa_key == NULL) {
		dprintf(CRITICAL, "ERROR: Boot Invalid, RSA_KEY is NULL!\n");
		goto cleanup;
	}

	ret = RSA_public_decrypt(SIGNATURE_SIZE, signature_ptr, plain_text,
				 rsa_key, RSA_PKCS1_PADDING);
	dprintf(SPEW, "DEBUG openssl: Return of RSA_public_decrypt = %d\n",
		ret);

 cleanup:
	if (rsa_key != NULL)
		RSA_free(rsa_key);
	if (x509_certificate != NULL)
		X509_free(x509_certificate);
	if (pub_key != NULL)
		EVP_PKEY_free(pub_key);
	return ret;
}

/*
 * Returns 1 when image is signed and authorized.
 * Returns 0 when image is unauthorized.
 * Expects a pointer to the start of image and pointer to start of sig
 */
int
image_verify(unsigned char *image_ptr,
	     unsigned char *signature_ptr,
	     unsigned int image_size, unsigned hash_type)
{

	int ret = -1;
	int auth = 0;
	unsigned char *plain_text = NULL;
	unsigned int digest[8];
	unsigned int hash_size;

	plain_text = (unsigned char *)calloc(sizeof(char), SIGNATURE_SIZE);
	if (plain_text == NULL) {
		dprintf(CRITICAL, "ERROR: Calloc failed during verification\n");
		goto cleanup;
	}

	ret = image_decrypt_signature(signature_ptr, plain_text);
	if (ret == -1) {
		dprintf(CRITICAL, "ERROR: Image Invalid! Decryption failed!\n");
		goto cleanup;
	}

	/*
	 * Calculate hash of image for comparison
	 */
	hash_size =
	    (hash_type == CRYPTO_AUTH_ALG_SHA256) ? SHA256_SIZE : SHA1_SIZE;
	hash_find(image_ptr, image_size, (unsigned char *)&digest, hash_type);
	if (memcmp(plain_text, digest, hash_size) != 0) {
		dprintf(CRITICAL,
			"ERROR: Image Invalid! Please use another image!\n");
		ret = -1;
		goto cleanup;
	} else {
		/* Authorized image */
		auth = 1;
	}

	/* Cleanup after complete usage of openssl - cached data and objects */
 cleanup:
	if (plain_text != NULL)
		free(plain_text);
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
	ERR_remove_thread_state(NULL);
	return auth;
}
