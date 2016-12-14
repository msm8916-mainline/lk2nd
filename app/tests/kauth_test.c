/*
 * Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation, Inc. nor the names of its
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

#include <app/kauth_test.h>
#include <crypto_hash.h>
#include <partition_parser.h>
#include <arch/defines.h>
#include <debug.h>
#include <stdlib.h>
#include <string.h>
#if VERIFIED_BOOT
#include <boot_verifier.h>
#endif
/* The test assumes the image and its signature to be verified
 * are flashed in the system and userdata partitions respectively.
 * The test reads the images and validates the decrypted signature
 * with the hash of the image. Customers can modify the partition names
 * and image size to custom values as per their requirements.
 */

void kauth_test(const char *arg, void *data, unsigned sz)
{
	int ret = 0;
#if VERIFIED_BOOT
	int i;
	int vboot_ret[12];
	int vboot_expected[12] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	const char *image = "Test Input File";
	unsigned int image_len = strlen(image);
	KEYSTORE *ks = boot_gerity_get_oem_keystore();
	bool test_pass = true;
#else
	unsigned long long ptn = 0;
	int index = INVALID_PTN;
	unsigned char *image = NULL;
	unsigned char *signature = NULL;
	uint32_t page_size = mmc_page_size();
#endif
#if !VERIFIED_BOOT
#if IMAGE_VERIF_ALGO_SHA1
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA1;
#else
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA256;
#endif
#endif

#if !VERIFIED_BOOT
	index = partition_get_index("system");
	ptn = partition_get_offset(index);
	mmc_set_lun(partition_get_lun(index));

	if(!ptn)
	{
		dprintf(CRITICAL, "partition system not found\n");
		return;
	}

	image = (unsigned char*) memalign(CACHE_LINE, ROUNDUP(IMAGE_SIZE, CACHE_LINE));

	ASSERT(image);

	if(mmc_read(ptn, (unsigned int*) image, IMAGE_SIZE))
	{
		dprintf(CRITICAL,"mmc_read system failed\n");
		goto err;
	}
	signature = (unsigned char*) memalign(CACHE_LINE, ROUNDUP(page_size, CACHE_LINE));

	ASSERT(signature);

	index = partition_get_index("userdata");
	ptn = partition_get_offset(index);

	if(!ptn)
	{
		dprintf(CRITICAL,"partition userdata not found\n");
		goto err;
	}

	if(mmc_read(ptn, (unsigned int*) signature, page_size))
	{
		dprintf(CRITICAL,"mmc_read signature failed\n");
		goto err;
	}
#endif

	dprintf(INFO, "kauth_test: Authenticating boot image (%d): start\n", IMAGE_SIZE);

#if VERIFIED_BOOT
	for (i = 0; i < 12; i++)
	{
		ret = boot_verify_compare_sha256((unsigned char *)image, image_len, vboot_signatures[i], ks->mykeybag->mykey->key_material);
		vboot_ret[i] = ret;
	}
	for (i = 0; i < 12; i++)
	{
		if (vboot_ret[i] != vboot_expected[i])
			test_pass = false;
			ret = i;
	}

	if (test_pass)
		dprintf(INFO, "Kauth_test: PASS for all usecases\n");
	else
		dprintf(INFO, "Kauth_test failed for : %s\n", vboot_signatures_str[ret]);
#else
	ret = image_verify(image, signature, IMAGE_SIZE, auth_algo);
	dprintf(INFO, "kauth_test: Authenticating boot image: done return value = %d\n", ret);
#endif

#if !VERIFIED_BOOT
err:
	if(image)
		free(image);
	if(signature)
		free(signature);
#endif
}
