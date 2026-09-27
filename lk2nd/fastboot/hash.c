// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Stephan Gerhold <stephan@gerhold.net> */

#include <crypto_hash.h>
#include <debug.h>
#include <fastboot.h>
#include <string.h>
#include <target.h>

static void buf2hex(const uint8_t *buf, unsigned size, char *out)
{
	static const char hex[] = "0123456789abcdef";
	unsigned i;

	for (i = 0; i < size; ++i) {
		*out++ = hex[(buf[i] >>  4) & 0xf];
		*out++ = hex[(buf[i] >>  0) & 0xf];
	}
	*out = 0;
}

static void cmd_oem_hash(const char *arg, void *data, unsigned sz)
{
	/* Two chars per byte for hexadecimal and null terminator */
	char response[SHA256_INIT_VECTOR_SIZE * sizeof(uint32_t) * 2 + 1];
	uint32_t digest[SHA256_INIT_VECTOR_SIZE];
	crypto_auth_alg_type alg;
	unsigned digest_size;

	if (strcmp(arg, "sha1") == 0) {
		alg = CRYPTO_AUTH_ALG_SHA1;
		digest_size = SHA1_INIT_VECTOR_SIZE * sizeof(digest[0]);
	} else if (strcmp(arg, "sha256") == 0) {
		alg = CRYPTO_AUTH_ALG_SHA256;
		digest_size = SHA256_INIT_VECTOR_SIZE * sizeof(digest[0]);
	} else {
		fastboot_fail("usage: fastboot oem hash <sha1|sha256>");
		return;
	}

	if (!sz) {
		fastboot_fail("no data staged to hash");
		return;
	}

	target_crypto_init_params();
	if (hash_find(data, sz, (void *)digest, alg) != CRYPTO_SHA_ERR_NONE) {
		fastboot_fail("failed to compute hash");
		return;
	}

	buf2hex((void *)digest, digest_size, response);
	fastboot_info(response);
	fastboot_okay("");
}
FASTBOOT_REGISTER("oem hash", cmd_oem_hash);
