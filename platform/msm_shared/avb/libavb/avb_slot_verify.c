/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "avb_slot_verify.h"
#include "avb_chain_partition_descriptor.h"
#include "avb_footer.h"
#include "avb_hash_descriptor.h"
#include "avb_kernel_cmdline_descriptor.h"
#include "avb_sha.h"
#include "avb_util.h"
#include "avb_vbmeta_image.h"
#include "avb_version.h"

/* Maximum allow length (in bytes) of a partition name, including
 * ab_suffix.
 */
#define PART_NAME_MAX_SIZE 32

/* Maximum number of partitions that can be loaded with avb_slot_verify(). */
#define MAX_NUMBER_OF_LOADED_PARTITIONS 32

/* Maximum number of vbmeta images that can be loaded with avb_slot_verify(). */
#define MAX_NUMBER_OF_VBMETA_IMAGES 32

/* Maximum size of a vbmeta image - 64 KiB. */
#define VBMETA_MAX_SIZE (64 * 1024)

/* Helper function to see if we should continue with verification in
 * allow_verification_error=true mode if something goes wrong. See the
 * comments for the avb_slot_verify() function for more information.
 */
static inline bool result_should_continue(AvbSlotVerifyResult result) {
  switch (result) {
    case AVB_SLOT_VERIFY_RESULT_ERROR_OOM:
    case AVB_SLOT_VERIFY_RESULT_ERROR_IO:
    case AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA:
    case AVB_SLOT_VERIFY_RESULT_ERROR_UNSUPPORTED_VERSION:
    case AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT:
      return false;

    case AVB_SLOT_VERIFY_RESULT_OK:
    case AVB_SLOT_VERIFY_RESULT_ERROR_VERIFICATION:
    case AVB_SLOT_VERIFY_RESULT_ERROR_ROLLBACK_INDEX:
    case AVB_SLOT_VERIFY_RESULT_ERROR_PUBLIC_KEY_REJECTED:
      return true;
  }

  return false;
}

static AvbSlotVerifyResult load_and_verify_hash_partition(
    AvbOps* ops,
    const char* const* requested_partitions,
    const char* ab_suffix,
    bool allow_verification_error,
    const AvbDescriptor* descriptor,
    AvbSlotVerifyData* slot_data) {
  AvbHashDescriptor hash_desc;
  const uint8_t* desc_partition_name = NULL;
  const uint8_t* desc_salt;
  const uint8_t* desc_digest;
  char part_name[PART_NAME_MAX_SIZE];
  AvbSlotVerifyResult ret;
  AvbIOResult io_ret;
  uint8_t* image_buf = NULL;
  size_t part_num_read;
  uint8_t* digest = NULL;
  size_t digest_len;
  const char* found = NULL;
  uint64_t image_size;

  if (!avb_hash_descriptor_validate_and_byteswap(
          (const AvbHashDescriptor*)descriptor, &hash_desc)) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
    goto out;
  }
  desc_partition_name =
      ((const uint8_t*)descriptor) + sizeof(AvbHashDescriptor);
  desc_salt = desc_partition_name + hash_desc.partition_name_len;
  desc_digest = desc_salt + hash_desc.salt_len;

  if (!avb_validate_utf8(desc_partition_name, hash_desc.partition_name_len)) {
    avb_error("Partition name is not valid UTF-8.\n");
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
    goto out;
  }

  /* Don't bother loading or validating unless the partition was
   * requested in the first place.
   */
  found = avb_strv_find_str(requested_partitions,
                            (const char*)desc_partition_name,
                            hash_desc.partition_name_len);
  if (found == NULL) {
    ret = AVB_SLOT_VERIFY_RESULT_OK;
    goto out;
  }

  if (!avb_str_concat(part_name,
                      sizeof part_name,
                      (const char*)desc_partition_name,
                      hash_desc.partition_name_len,
                      ab_suffix,
                      avb_strlen(ab_suffix))) {
    avb_error("Partition name and suffix does not fit.\n");
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
    goto out;
  }

  /* If we're allowing verification errors then hash_desc.image_size
   * may no longer match what's in the partition... so in this case
   * just load the entire partition.
   *
   * For example, this can happen if a developer does 'fastboot flash
   * boot /path/to/new/and/bigger/boot.img'. We want this to work
   * since it's such a common workflow.
   */
  image_size = hash_desc.image_size;
  if (allow_verification_error) {
    if (ops->get_size_of_partition == NULL) {
      avb_errorv(part_name,
                 ": The get_size_of_partition() operation is "
                 "not implemented so we may not load the entire partition. "
                 "Please implement.",
                 NULL);
    } else {
      io_ret = ops->get_size_of_partition(ops, part_name, &image_size);
      if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
        goto out;
      } else if (io_ret != AVB_IO_RESULT_OK) {
        avb_errorv(part_name, ": Error determining partition size.\n", NULL);
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
        goto out;
      }
      avb_debugv(part_name, ": Loading entire partition.\n", NULL);
    }
  }

  if (!strncmp(part_name, "boot", strlen("boot"))) {
      image_size = hash_desc.image_size;
  }

  io_ret = ops->read_from_partition(
            ops, found, 0 /* offset */, image_size, &image_buf, &part_num_read);

  if (image_buf == NULL) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  }
  if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  } else if (io_ret != AVB_IO_RESULT_OK) {
    avb_errorv(part_name, ": Error loading data from partition.\n", NULL);
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
    goto out;
  }
  if (part_num_read < image_size) {
    avb_errorv(part_name, ": Read fewer than requested bytes.\n", NULL);
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
    goto out;
  }

  if (avb_strcmp((const char*)hash_desc.hash_algorithm, "sha256") == 0) {
    uint32_t complete_len = hash_desc.salt_len + hash_desc.image_size;
    digest = avb_malloc(AVB_SHA256_DIGEST_SIZE);
    if(digest == NULL || hash_desc.salt_len > SALT_BUFF_OFFSET )
    {
        avb_errorv(part_name, ": Failed to allocate memory\n", NULL);
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
        goto out;
    }
    image_buf = ADD_SALT_BUFF_OFFSET(image_buf) - hash_desc.salt_len;
    avb_memcpy(image_buf, desc_salt, hash_desc.salt_len);
    hash_find(image_buf, complete_len, digest, CRYPTO_AUTH_ALG_SHA256);
    image_buf = SUB_SALT_BUFF_OFFSET(image_buf) +  hash_desc.salt_len;
    digest_len = AVB_SHA256_DIGEST_SIZE;
  } else if (avb_strcmp((const char*)hash_desc.hash_algorithm, "sha512") == 0) {
    AvbSHA512Ctx sha512_ctx;
    uint8_t *dig;
    digest = avb_malloc(AVB_SHA512_DIGEST_SIZE);
    if(digest == NULL)
    {
        avb_errorv(part_name, ": Failed to allocate memory\n", NULL);
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
        goto out;
    }
    avb_sha512_init(&sha512_ctx);
    avb_sha512_update(&sha512_ctx, desc_salt, hash_desc.salt_len);
    avb_sha512_update(&sha512_ctx, image_buf, hash_desc.image_size);
    dig = avb_sha512_final(&sha512_ctx);
    digest_len = AVB_SHA512_DIGEST_SIZE;
    avb_memcpy(digest, dig, AVB_SHA512_DIGEST_SIZE);
  } else {
    avb_errorv(part_name, ": Unsupported hash algorithm.\n", NULL);
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
    goto out;
  }

  if (digest_len != hash_desc.digest_len) {
    avb_errorv(
        part_name, ": Digest in descriptor not of expected size.\n", NULL);
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
    goto out;
  }

  if (avb_safe_memcmp(digest, desc_digest, digest_len) != 0) {
    avb_errorv(part_name,
               ": Hash of data does not match digest in descriptor.\n",
               NULL);
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_VERIFICATION;
    goto out;
  }

  ret = AVB_SLOT_VERIFY_RESULT_OK;

out:
  if (digest)
    avb_free(digest);
  /* If it worked and something was loaded, copy to slot_data. */
  if ((ret == AVB_SLOT_VERIFY_RESULT_OK || result_should_continue(ret)) &&
      image_buf != NULL) {
    AvbPartitionData* loaded_partition;
    if (slot_data->num_loaded_partitions == MAX_NUMBER_OF_LOADED_PARTITIONS) {
      avb_errorv(part_name, ": Too many loaded partitions.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
      goto fail;
    }
    loaded_partition =
        &slot_data->loaded_partitions[slot_data->num_loaded_partitions++];
    loaded_partition->partition_name = avb_strdup(found);
    loaded_partition->data_size = image_size;
    loaded_partition->data = image_buf;
    image_buf = NULL;
  }

fail:
  //remove avb_free() as memory allocated from scratch region
  return ret;
}

static AvbSlotVerifyResult load_requested_partitions(
    AvbOps* ops,
    const char* const* requested_partitions,
    const char* ab_suffix,
    AvbSlotVerifyData* slot_data) {
  AvbSlotVerifyResult ret;
  uint8_t* image_buf = NULL;
  size_t n;

  if (ops->get_size_of_partition == NULL) {
    avb_error("get_size_of_partition() not implemented.\n");
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT;
    goto out;
  }

  for (n = 0; requested_partitions[n] != NULL; n++) {
    char part_name[PART_NAME_MAX_SIZE];
    AvbIOResult io_ret;
    uint64_t image_size;
    size_t part_num_read;
    AvbPartitionData* loaded_partition;

    if (!avb_str_concat(part_name,
                        sizeof part_name,
                        requested_partitions[n],
                        avb_strlen(requested_partitions[n]),
                        ab_suffix,
                        avb_strlen(ab_suffix))) {
      avb_error("Partition name and suffix does not fit.\n");
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
      goto out;
    }

    io_ret = ops->get_size_of_partition(ops, part_name, &image_size);
    if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
      goto out;
    } else if (io_ret != AVB_IO_RESULT_OK) {
      avb_errorv(part_name, ": Error determining partition size.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
      goto out;
    }
    avb_debugv(part_name, ": Loading entire partition.\n", NULL);

    io_ret = ops->read_from_partition(
        ops, part_name, 0 /* offset */, image_size, &image_buf, &part_num_read);
    if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
      goto out;
    } else if (io_ret != AVB_IO_RESULT_OK) {
      avb_errorv(part_name, ": Error loading data from partition.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
      goto out;
    }
    if (part_num_read != image_size) {
      avb_errorv(part_name, ": Read fewer than requested bytes.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
      goto out;
    }

    /* Move to slot_data. */
    if (slot_data->num_loaded_partitions == MAX_NUMBER_OF_LOADED_PARTITIONS) {
      avb_errorv(part_name, ": Too many loaded partitions.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
      goto out;
    }
    loaded_partition =
        &slot_data->loaded_partitions[slot_data->num_loaded_partitions++];
    loaded_partition->partition_name = avb_strdup(requested_partitions[n]);
    if (loaded_partition->partition_name == NULL) {
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
      goto out;
    }
    loaded_partition->data_size = image_size;
    loaded_partition->data = image_buf;
    image_buf = NULL;
  }

  ret = AVB_SLOT_VERIFY_RESULT_OK;

out:
  if (image_buf != NULL) {
    avb_free(image_buf);
  }
  return ret;
}

static AvbSlotVerifyResult load_and_verify_vbmeta(
    AvbOps* ops,
    const char* const* requested_partitions,
    const char* ab_suffix,
    bool allow_verification_error,
    AvbVBMetaImageFlags toplevel_vbmeta_flags,
    int rollback_index_location,
    const char* partition_name,
    size_t partition_name_len,
    const uint8_t* expected_public_key,
    size_t expected_public_key_length,
    AvbSlotVerifyData* slot_data,
    AvbAlgorithmType* out_algorithm_type) {
  char full_partition_name[PART_NAME_MAX_SIZE];
  AvbSlotVerifyResult ret;
  AvbIOResult io_ret;
  UINTN vbmeta_offset;
  size_t vbmeta_size;
  uint8_t* vbmeta_buf = NULL;
  size_t vbmeta_num_read;
  AvbVBMetaVerifyResult vbmeta_ret;
  const uint8_t* pk_data;
  size_t pk_len;
  AvbVBMetaImageHeader vbmeta_header;
  uint64_t stored_rollback_index;
  const AvbDescriptor** descriptors = NULL;
  size_t num_descriptors;
  size_t n;
  bool is_main_vbmeta;
  bool is_vbmeta_partition;
  AvbVBMetaData* vbmeta_image_data = NULL;

  ret = AVB_SLOT_VERIFY_RESULT_OK;

  avb_assert(slot_data != NULL);

  /* Since we allow top-level vbmeta in 'boot', use
   * rollback_index_location to determine whether we're the main
   * vbmeta struct.
   */
  is_main_vbmeta = (rollback_index_location == 0);
  is_vbmeta_partition = (avb_strcmp(partition_name, "vbmeta") == 0);

  if (!avb_validate_utf8((const uint8_t*)partition_name, partition_name_len)) {
    avb_error("Partition name is not valid UTF-8.\n");
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
    goto out;
  }

  /* Construct full partition name. */
  if (!avb_str_concat(full_partition_name,
                      sizeof full_partition_name,
                      partition_name,
                      partition_name_len,
                      ab_suffix,
                      avb_strlen(ab_suffix))) {
    avb_error("Partition name and suffix does not fit.\n");
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
    goto out;
  }

  avb_debugv("Loading vbmeta struct from partition '",
             full_partition_name,
             "'.\n",
             NULL);

  /* If we're loading from the main vbmeta partition, the vbmeta
   * struct is in the beginning. Otherwise we have to locate it via a
   * footer.
   */
  if (is_vbmeta_partition) {
    vbmeta_offset = 0;
    vbmeta_size = VBMETA_MAX_SIZE;
    io_ret = ops->read_from_partition(ops,
                                    partition_name,
                                    vbmeta_offset,
                                    vbmeta_size,
                                    &vbmeta_buf,
                                    &vbmeta_num_read);
    if (vbmeta_buf == NULL) {
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
      goto out;
    }
  } else {
    uint8_t footer_buf[AVB_FOOTER_SIZE];
    size_t footer_num_read;
    AvbFooter footer;

    io_ret = ops->read_from_partition(ops,
                                      full_partition_name,
                                      -AVB_FOOTER_SIZE,
                                      AVB_FOOTER_SIZE,
                                      footer_buf,
                                      &footer_num_read);
    if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
      goto out;
    } else if (io_ret != AVB_IO_RESULT_OK) {
      avb_errorv(full_partition_name, ": Error loading footer.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
      goto out;
    }
    avb_assert(footer_num_read == AVB_FOOTER_SIZE);

    if (!avb_footer_validate_and_byteswap((const AvbFooter*)footer_buf,
                                          &footer)) {
      avb_errorv(full_partition_name, ": Error validating footer.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
      goto out;
    }

    /* Basic footer sanity check since the data is untrusted. */
    if (footer.vbmeta_size > VBMETA_MAX_SIZE) {
      avb_errorv(
          full_partition_name, ": Invalid vbmeta size in footer.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
      goto out;
    }
    vbmeta_offset = footer.vbmeta_offset;
    vbmeta_size = footer.vbmeta_size;
    vbmeta_buf = avb_malloc(vbmeta_size); // for chain partitions
    if (vbmeta_buf == NULL) {
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
        goto out;
    }
    io_ret = ops->read_from_partition(ops,
                                    full_partition_name,
                                    vbmeta_offset,
                                    vbmeta_size,
                                    vbmeta_buf,
                                    &vbmeta_num_read);
  }

  if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  } else if (io_ret != AVB_IO_RESULT_OK) {
    /* If we're looking for 'vbmeta' but there is no such partition,
     * go try to get it from the boot partition instead.
     */
    if (is_main_vbmeta && io_ret == AVB_IO_RESULT_ERROR_NO_SUCH_PARTITION &&
        is_vbmeta_partition) {
      avb_debugv(full_partition_name,
                 ": No such partition. Trying 'boot' instead.\n",
                 NULL);
      ret = load_and_verify_vbmeta(ops,
                                   requested_partitions,
                                   ab_suffix,
                                   allow_verification_error,
                                   0 /* toplevel_vbmeta_flags */,
                                   0 /* rollback_index_location */,
                                   "boot",
                                   avb_strlen("boot"),
                                   NULL /* expected_public_key */,
                                   0 /* expected_public_key_length */,
                                   slot_data,
                                   out_algorithm_type);
      goto out;
    } else {
      avb_errorv(full_partition_name, ": Error loading vbmeta data.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
      goto out;
    }
  }
  avb_assert(vbmeta_num_read <= vbmeta_size);

  /* Check if the image is properly signed and get the public key used
   * to sign the image.
   */
  vbmeta_ret =
      avb_vbmeta_image_verify(vbmeta_buf, vbmeta_num_read, &pk_data, &pk_len);
  switch (vbmeta_ret) {
    case AVB_VBMETA_VERIFY_RESULT_OK:
      avb_assert(pk_data != NULL && pk_len > 0);
      break;

    case AVB_VBMETA_VERIFY_RESULT_OK_NOT_SIGNED:
    case AVB_VBMETA_VERIFY_RESULT_HASH_MISMATCH:
    case AVB_VBMETA_VERIFY_RESULT_SIGNATURE_MISMATCH:
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_VERIFICATION;
      avb_errorv(full_partition_name,
                 ": Error verifying vbmeta image: ",
                 avb_vbmeta_verify_result_to_string(vbmeta_ret),
                 "\n",
                 NULL);
      if (!allow_verification_error) {
        goto out;
      }
      break;

    case AVB_VBMETA_VERIFY_RESULT_INVALID_VBMETA_HEADER:
      /* No way to continue this case. */
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
      avb_errorv(full_partition_name,
                 ": Error verifying vbmeta image: invalid vbmeta header\n",
                 NULL);
      goto out;

    case AVB_VBMETA_VERIFY_RESULT_UNSUPPORTED_VERSION:
      /* No way to continue this case. */
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_UNSUPPORTED_VERSION;
      avb_errorv(full_partition_name,
                 ": Error verifying vbmeta image: unsupported AVB version\n",
                 NULL);
      goto out;
  }

  /* Byteswap the header. */
  avb_vbmeta_image_header_to_host_byte_order((AvbVBMetaImageHeader*)vbmeta_buf,
                                             &vbmeta_header);

  /* If we're the toplevel, assign flags so they'll be passed down. */
  if (is_main_vbmeta) {
    toplevel_vbmeta_flags = (AvbVBMetaImageFlags)vbmeta_header.flags;
  } else {
    if (vbmeta_header.flags != 0) {
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
      avb_errorv(full_partition_name,
                 ": chained vbmeta image has non-zero flags\n",
                 NULL);
      goto out;
    }
  }

  /* Check if key used to make signature matches what is expected. */
  if (pk_data != NULL) {
    if (expected_public_key != NULL) {
      avb_assert(!is_main_vbmeta);
      if (expected_public_key_length != pk_len ||
          avb_safe_memcmp(expected_public_key, pk_data, pk_len) != 0) {
        avb_errorv(full_partition_name,
                   ": Public key used to sign data does not match key in chain "
                   "partition descriptor.\n",
                   NULL);
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_PUBLIC_KEY_REJECTED;
        if (!allow_verification_error) {
          goto out;
        }
      }
    } else {
      bool key_is_trusted = false;
      const uint8_t* pk_metadata = NULL;
      size_t pk_metadata_len = 0;

      if (vbmeta_header.public_key_metadata_size > 0) {
        pk_metadata = vbmeta_buf + sizeof(AvbVBMetaImageHeader) +
                      vbmeta_header.authentication_data_block_size +
                      vbmeta_header.public_key_metadata_offset;
        pk_metadata_len = vbmeta_header.public_key_metadata_size;
      }

      avb_assert(is_main_vbmeta);
      io_ret = ops->validate_vbmeta_public_key(
          ops, pk_data, pk_len, pk_metadata, pk_metadata_len, &key_is_trusted);
      if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
        goto out;
      } else if (io_ret != AVB_IO_RESULT_OK) {
        avb_errorv(full_partition_name,
                   ": Error while checking public key used to sign data.\n",
                   NULL);
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
        goto out;
      }
      if (!key_is_trusted) {
        avb_errorv(full_partition_name,
                   ": Public key used to sign data rejected.\n",
                   NULL);
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_PUBLIC_KEY_REJECTED;
        if (!allow_verification_error) {
          goto out;
        }
      }
    }
  }

  /* Check rollback index. */
  io_ret = ops->read_rollback_index(
      ops, rollback_index_location, &stored_rollback_index);
  if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  } else if (io_ret != AVB_IO_RESULT_OK) {
    avb_errorv(full_partition_name,
               ": Error getting rollback index for location.\n",
               NULL);
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
    goto out;
  }
  if (vbmeta_header.rollback_index < stored_rollback_index) {
    avb_errorv(
        full_partition_name,
        ": Image rollback index is less than the stored rollback index.\n",
        NULL);
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_ROLLBACK_INDEX;
    if (!allow_verification_error) {
      goto out;
    }
  }

  if (stored_rollback_index < vbmeta_header.rollback_index) {
    io_ret = ops->write_rollback_index(
        ops, rollback_index_location, vbmeta_header.rollback_index);
    if (io_ret != AVB_IO_RESULT_OK) {
      avb_errorv(full_partition_name,
                 ": Error storing rollback index for location.\n",
                 NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
      goto out;
    }
  }

  /* Copy vbmeta to vbmeta_images before recursing. */
  if (is_main_vbmeta) {
    avb_assert(slot_data->num_vbmeta_images == 0);
  } else {
    avb_assert(slot_data->num_vbmeta_images > 0);
  }
  if (slot_data->num_vbmeta_images == MAX_NUMBER_OF_VBMETA_IMAGES) {
    avb_errorv(full_partition_name, ": Too many vbmeta images.\n", NULL);
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  }
  vbmeta_image_data = &slot_data->vbmeta_images[slot_data->num_vbmeta_images++];
  vbmeta_image_data->partition_name = avb_strdup(partition_name);
  vbmeta_image_data->vbmeta_data = vbmeta_buf;
  /* Note that |vbmeta_buf| is actually |vbmeta_num_read| bytes long
   * and this includes data past the end of the image. Pass the
   * actual size of the vbmeta image. Also, no need to use
   * avb_safe_add() since the header has already been verified.
   */
  vbmeta_image_data->vbmeta_size =
      sizeof(AvbVBMetaImageHeader) +
      vbmeta_header.authentication_data_block_size +
      vbmeta_header.auxiliary_data_block_size;
  vbmeta_image_data->verify_result = vbmeta_ret;

  /* If verification has been disabled by setting a bit in the image,
   * we're done... except that we need to load the entirety of the
   * requested partitions.
   */
  if (vbmeta_header.flags & AVB_VBMETA_IMAGE_FLAGS_VERIFICATION_DISABLED) {
    AvbSlotVerifyResult sub_ret;
    avb_debugv(
        full_partition_name, ": VERIFICATION_DISABLED bit is set.\n", NULL);
    /* If load_requested_partitions() fail it is always a fatal
     * failure (e.g. ERROR_INVALID_ARGUMENT, ERROR_OOM, etc.) rather
     * than recoverable (e.g. one where result_should_continue()
     * returns true) and we want to convey that error.
     */
    sub_ret = load_requested_partitions(
        ops, requested_partitions, ab_suffix, slot_data);
    if (sub_ret != AVB_SLOT_VERIFY_RESULT_OK) {
      ret = sub_ret;
    }
    goto out;
  }

  /* Now go through all descriptors and take the appropriate action:
   *
   * - hash descriptor: Load data from partition, calculate hash, and
   *   checks that it matches what's in the hash descriptor.
   *
   * - hashtree descriptor: Do nothing since verification happens
   *   on-the-fly from within the OS.
   *
   * - chained partition descriptor: Load the footer, load the vbmeta
   *   image, verify vbmeta image (includes rollback checks, hash
   *   checks, bail on chained partitions).
   */
  descriptors =
      avb_descriptor_get_all(vbmeta_buf, vbmeta_num_read, &num_descriptors);
  if (descriptors == NULL) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
    goto out;
  }

  for (n = 0; n < num_descriptors; n++) {
    AvbDescriptor desc;

    if (!avb_descriptor_validate_and_byteswap(descriptors[n], &desc)) {
      avb_errorv(full_partition_name, ": Descriptor is invalid.\n", NULL);
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
      goto out;
    }

    switch (desc.tag) {
      case AVB_DESCRIPTOR_TAG_HASH: {
        AvbSlotVerifyResult sub_ret;
        sub_ret = load_and_verify_hash_partition(ops,
                                                 requested_partitions,
                                                 ab_suffix,
                                                 allow_verification_error,
                                                 descriptors[n],
                                                 slot_data);
        if (sub_ret != AVB_SLOT_VERIFY_RESULT_OK) {
          ret = sub_ret;
          if (!allow_verification_error || !result_should_continue(ret)) {
            goto out;
          }
        }
      } break;

      case AVB_DESCRIPTOR_TAG_CHAIN_PARTITION: {
        AvbSlotVerifyResult sub_ret;
        AvbChainPartitionDescriptor chain_desc;
        const uint8_t* chain_partition_name;
        const uint8_t* chain_public_key;

        /* Only allow CHAIN_PARTITION descriptors in the main vbmeta image. */
        if (!is_main_vbmeta) {
          avb_errorv(full_partition_name,
                     ": Encountered chain descriptor not in main image.\n",
                     NULL);
          ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
          goto out;
        }

        if (!avb_chain_partition_descriptor_validate_and_byteswap(
                (AvbChainPartitionDescriptor*)descriptors[n], &chain_desc)) {
          avb_errorv(full_partition_name,
                     ": Chain partition descriptor is invalid.\n",
                     NULL);
          ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
          goto out;
        }

        if (chain_desc.rollback_index_location == 0) {
          avb_errorv(full_partition_name,
                     ": Chain partition has invalid "
                     "rollback_index_location field.\n",
                     NULL);
          ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
          goto out;
        }

        chain_partition_name = ((const uint8_t*)descriptors[n]) +
                               sizeof(AvbChainPartitionDescriptor);
        chain_public_key = chain_partition_name + chain_desc.partition_name_len;

        sub_ret = load_and_verify_vbmeta(ops,
                                         requested_partitions,
                                         ab_suffix,
                                         allow_verification_error,
                                         toplevel_vbmeta_flags,
                                         chain_desc.rollback_index_location,
                                         (const char*)chain_partition_name,
                                         chain_desc.partition_name_len,
                                         chain_public_key,
                                         chain_desc.public_key_len,
                                         slot_data,
                                         NULL /* out_algorithm_type */);
        if (sub_ret != AVB_SLOT_VERIFY_RESULT_OK) {
          ret = sub_ret;
          if (!result_should_continue(ret)) {
            goto out;
          }
        }
      } break;

      case AVB_DESCRIPTOR_TAG_KERNEL_CMDLINE: {
        const uint8_t* kernel_cmdline;
        AvbKernelCmdlineDescriptor kernel_cmdline_desc;
        bool apply_cmdline;

        if (!avb_kernel_cmdline_descriptor_validate_and_byteswap(
                (AvbKernelCmdlineDescriptor*)descriptors[n],
                &kernel_cmdline_desc)) {
          avb_errorv(full_partition_name,
                     ": Kernel cmdline descriptor is invalid.\n",
                     NULL);
          ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
          goto out;
        }

        kernel_cmdline = ((const uint8_t*)descriptors[n]) +
                         sizeof(AvbKernelCmdlineDescriptor);

        if (!avb_validate_utf8(kernel_cmdline,
                               kernel_cmdline_desc.kernel_cmdline_length)) {
          avb_errorv(full_partition_name,
                     ": Kernel cmdline is not valid UTF-8.\n",
                     NULL);
          ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
          goto out;
        }

        /* Compare the flags for top-level VBMeta struct with flags in
         * the command-line descriptor so command-line snippets only
         * intended for a certain mode (dm-verity enabled/disabled)
         * are skipped if applicable.
         */
        apply_cmdline = true;
        if (toplevel_vbmeta_flags & AVB_VBMETA_IMAGE_FLAGS_HASHTREE_DISABLED) {
          if (kernel_cmdline_desc.flags &
              AVB_KERNEL_CMDLINE_FLAGS_USE_ONLY_IF_HASHTREE_NOT_DISABLED) {
            apply_cmdline = false;
          }
        } else {
          if (kernel_cmdline_desc.flags &
              AVB_KERNEL_CMDLINE_FLAGS_USE_ONLY_IF_HASHTREE_DISABLED) {
            apply_cmdline = false;
          }
        }

        if (apply_cmdline) {
          if (slot_data->cmdline == NULL) {
            slot_data->cmdline =
                avb_calloc(kernel_cmdline_desc.kernel_cmdline_length + 1);
            if (slot_data->cmdline == NULL) {
              ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
              goto out;
            }
            avb_memcpy(slot_data->cmdline,
                       kernel_cmdline,
                       kernel_cmdline_desc.kernel_cmdline_length);
          } else {
            /* new cmdline is: <existing_cmdline> + ' ' + <newcmdline> + '\0' */
            size_t orig_size = avb_strlen(slot_data->cmdline);
            size_t new_size =
                orig_size + 1 + kernel_cmdline_desc.kernel_cmdline_length + 1;
            char* new_cmdline = avb_calloc(new_size);
            if (new_cmdline == NULL) {
              ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
              goto out;
            }
            avb_memcpy(new_cmdline, slot_data->cmdline, orig_size);
            new_cmdline[orig_size] = ' ';
            avb_memcpy(new_cmdline + orig_size + 1,
                       kernel_cmdline,
                       kernel_cmdline_desc.kernel_cmdline_length);
            avb_free(slot_data->cmdline);
            slot_data->cmdline = new_cmdline;
          }
        }
      } break;

      /* Explicit fall-through */
      case AVB_DESCRIPTOR_TAG_PROPERTY:
      case AVB_DESCRIPTOR_TAG_HASHTREE:
        /* Do nothing. */
        break;
    }
  }

  if (rollback_index_location >= AVB_MAX_NUMBER_OF_ROLLBACK_INDEX_LOCATIONS) {
    avb_errorv(
        full_partition_name, ": Invalid rollback_index_location.\n", NULL);
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA;
    goto out;
  }

  slot_data->rollback_indexes[rollback_index_location] =
      vbmeta_header.rollback_index;

  if (out_algorithm_type != NULL) {
    *out_algorithm_type = (AvbAlgorithmType)vbmeta_header.algorithm_type;
  }

out:
  /* If |vbmeta_image_data| isn't NULL it means that it adopted
   * |vbmeta_buf| so in that case don't free it here.
   */

  /* remove avb_free() as memory allocated from scratch region */

  if (descriptors != NULL) {
    avb_free(descriptors);
  }
  return ret;
}

#define NUM_GUIDS 3

/* Substitutes all variables (e.g. $(ANDROID_SYSTEM_PARTUUID)) with
 * values. Returns NULL on OOM, otherwise the cmdline with values
 * replaced.
 */
static char* sub_cmdline(AvbOps* ops,
                         const char* cmdline,
                         const char* ab_suffix,
                         bool using_boot_for_vbmeta) {
  const char* part_name_str[NUM_GUIDS] = {"system", "boot", "vbmeta"};
  const char* replace_str[NUM_GUIDS] = {"$(ANDROID_SYSTEM_PARTUUID)",
                                        "$(ANDROID_BOOT_PARTUUID)",
                                        "$(ANDROID_VBMETA_PARTUUID)"};
  char* ret = NULL;
  AvbIOResult io_ret;

  /* Special-case for when the top-level vbmeta struct is in the boot
   * partition.
   */
  if (using_boot_for_vbmeta) {
    part_name_str[2] = "boot";
  }

  /* Replace unique partition GUIDs */
  for (size_t n = 0; n < NUM_GUIDS; n++) {
    char part_name[PART_NAME_MAX_SIZE];
    char guid_buf[37];

    if (!avb_str_concat(part_name,
                        sizeof part_name,
                        part_name_str[n],
                        avb_strlen(part_name_str[n]),
                        ab_suffix,
                        avb_strlen(ab_suffix))) {
      avb_error("Partition name and suffix does not fit.\n");
      goto fail;
    }

    io_ret = ops->get_unique_guid_for_partition(
        ops, part_name, guid_buf, sizeof guid_buf);
    if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
      return NULL;
    } else if (io_ret != AVB_IO_RESULT_OK) {
      avb_error("Error getting unique GUID for partition.\n");
      goto fail;
    }

    if (ret == NULL) {
      ret = avb_replace(cmdline, replace_str[n], guid_buf);
    } else {
      char* new_ret = avb_replace(ret, replace_str[n], guid_buf);
      avb_free(ret);
      ret = new_ret;
    }
    if (ret == NULL) {
      goto fail;
    }
  }

  return ret;

fail:
  if (ret != NULL) {
    avb_free(ret);
  }
  return NULL;
}

static int cmdline_append_option(AvbSlotVerifyData* slot_data,
                                 const char* key,
                                 const char* value) {
  size_t offset, key_len, value_len;
  char* new_cmdline;

  key_len = avb_strlen(key);
  value_len = avb_strlen(value);

  offset = 0;
  if (slot_data->cmdline != NULL) {
    offset = avb_strlen(slot_data->cmdline);
    if (offset > 0) {
      offset += 1;
    }
  }

  new_cmdline = avb_calloc(offset + key_len + value_len + 2);
  if (new_cmdline == NULL) {
    return 0;
  }
  if (offset > 0) {
    avb_memcpy(new_cmdline, slot_data->cmdline, offset - 1);
    new_cmdline[offset - 1] = ' ';
  }
  avb_memcpy(new_cmdline + offset, key, key_len);
  new_cmdline[offset + key_len] = '=';
  avb_memcpy(new_cmdline + offset + key_len + 1, value, value_len);
  if (slot_data->cmdline != NULL) {
    avb_free(slot_data->cmdline);
  }
  slot_data->cmdline = new_cmdline;

  return 1;
}

#define AVB_MAX_DIGITS_UINT64 32

/* Writes |value| to |digits| in base 10 followed by a NUL byte.
 * Returns number of characters written excluding the NUL byte.
 */
static size_t uint64_to_base10(uint64_t value,
                               char digits[AVB_MAX_DIGITS_UINT64]) {
  char rev_digits[AVB_MAX_DIGITS_UINT64];
  size_t n, num_digits;

  for (num_digits = 0; num_digits < AVB_MAX_DIGITS_UINT64 - 1;) {
    rev_digits[num_digits++] = (value % 10) + '0';
    value /= 10;
    if (value == 0) {
      break;
    }
  }

  for (n = 0; n < num_digits; n++) {
    digits[n] = rev_digits[num_digits - 1 - n];
  }
  digits[n] = '\0';
  return n;
}

static int cmdline_append_version(AvbSlotVerifyData* slot_data,
                                  const char* key,
                                  uint64_t major_version,
                                  uint64_t minor_version) {
  char major_digits[AVB_MAX_DIGITS_UINT64];
  char minor_digits[AVB_MAX_DIGITS_UINT64];
  char combined[AVB_MAX_DIGITS_UINT64 * 2 + 1];
  size_t num_major_digits, num_minor_digits;

  num_major_digits = uint64_to_base10(major_version, major_digits);
  num_minor_digits = uint64_to_base10(minor_version, minor_digits);
  avb_memcpy(combined, major_digits, num_major_digits);
  combined[num_major_digits] = '.';
  avb_memcpy(combined + num_major_digits + 1, minor_digits, num_minor_digits);
  combined[num_major_digits + 1 + num_minor_digits] = '\0';

  return cmdline_append_option(slot_data, key, combined);
}

static int cmdline_append_uint64_base10(AvbSlotVerifyData* slot_data,
                                        const char* key,
                                        uint64_t value) {
  char digits[AVB_MAX_DIGITS_UINT64];
  uint64_to_base10(value, digits);
  return cmdline_append_option(slot_data, key, digits);
}

static int cmdline_append_hex(AvbSlotVerifyData* slot_data,
                              const char* key,
                              const uint8_t* data,
                              size_t data_len) {
  char hex_digits[17] = "0123456789abcdef";
  char* hex_data;
  int ret;
  size_t n;

  hex_data = avb_malloc(data_len * 2 + 1);
  if (hex_data == NULL) {
    return 0;
  }

  for (n = 0; n < data_len; n++) {
    hex_data[n * 2] = hex_digits[data[n] >> 4];
    hex_data[n * 2 + 1] = hex_digits[data[n] & 0x0f];
  }
  hex_data[n * 2] = '\0';

  ret = cmdline_append_option(slot_data, key, hex_data);
  avb_free(hex_data);
  return ret;
}

static AvbSlotVerifyResult append_options(
    AvbOps* ops,
    AvbSlotVerifyData* slot_data,
    AvbVBMetaImageHeader* toplevel_vbmeta,
    AvbAlgorithmType algorithm_type,
    AvbHashtreeErrorMode hashtree_error_mode) {
  AvbSlotVerifyResult ret;
  const char* verity_mode = NULL;
  bool is_device_unlocked;
  AvbIOResult io_ret;

  /* Add androidboot.vbmeta.device option. */
  if (!cmdline_append_option(slot_data,
                             "androidboot.vbmeta.device",
                             "PARTUUID=$(ANDROID_VBMETA_PARTUUID)")) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  }

  /* Add androidboot.vbmeta.avb_version option. */
  if (!cmdline_append_version(slot_data,
                              "androidboot.vbmeta.avb_version",
                              AVB_VERSION_MAJOR,
                              AVB_VERSION_MINOR)) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  }

  /* Set androidboot.avb.device_state to "locked" or "unlocked". */
  io_ret = ops->read_is_device_unlocked(ops, &is_device_unlocked);
  if (io_ret == AVB_IO_RESULT_ERROR_OOM) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  } else if (io_ret != AVB_IO_RESULT_OK) {
    avb_error("Error getting device state.\n");
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
    goto out;
  }
  if (!cmdline_append_option(slot_data,
                             "androidboot.vbmeta.device_state",
                             is_device_unlocked ? "unlocked" : "locked")) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  }

  /* Set androidboot.vbmeta.{hash_alg, size, digest} - use same hash
   * function as is used to sign vbmeta.
   */
  switch (algorithm_type) {
    /* Explicit fallthrough. */
    case AVB_ALGORITHM_TYPE_NONE:
    case AVB_ALGORITHM_TYPE_SHA256_RSA2048:
    case AVB_ALGORITHM_TYPE_SHA256_RSA4096:
    case AVB_ALGORITHM_TYPE_SHA256_RSA8192: {
      size_t n, prev_sz = 0, total_size = 0;
      uint8_t* digest = NULL;
      uint8_t* tbuf = NULL;

      digest = avb_malloc(AVB_SHA256_DIGEST_SIZE);
      if(digest == NULL)
      {
        avb_error("Failed to allocate memory\n");
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_IO;
        goto out;
      }
      for (n = 0; n < slot_data->num_vbmeta_images; n++) {
        total_size += slot_data->vbmeta_images[n].vbmeta_size;
      }
      tbuf = avb_malloc(total_size);

      for (n = 0; n < slot_data->num_vbmeta_images; n++) {
        avb_memcpy(tbuf + prev_sz, slot_data->vbmeta_images[n].vbmeta_data,
                                   slot_data->vbmeta_images[n].vbmeta_size);
        prev_sz = slot_data->vbmeta_images[n].vbmeta_size;
      }
      hash_find((unsigned char *)tbuf, total_size, digest, CRYPTO_AUTH_ALG_SHA256);
      avb_free(tbuf);

      if (!cmdline_append_option(
              slot_data, "androidboot.vbmeta.hash_alg", "sha256") ||
          !cmdline_append_uint64_base10(
              slot_data, "androidboot.vbmeta.size", total_size) ||
          !cmdline_append_hex(slot_data,
                              "androidboot.vbmeta.digest",
                              digest,
                              AVB_SHA256_DIGEST_SIZE)) {
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
        if (digest)
          avb_free(digest);
        goto out;
      }
      if (digest)
        avb_free(digest);
    } break;
    /* Explicit fallthrough. */
    case AVB_ALGORITHM_TYPE_SHA512_RSA2048:
    case AVB_ALGORITHM_TYPE_SHA512_RSA4096:
    case AVB_ALGORITHM_TYPE_SHA512_RSA8192: {
      AvbSHA512Ctx ctx;
      size_t n, total_size = 0;
      avb_sha512_init(&ctx);
      for (n = 0; n < slot_data->num_vbmeta_images; n++) {
        avb_sha512_update(&ctx,
                          slot_data->vbmeta_images[n].vbmeta_data,
                          slot_data->vbmeta_images[n].vbmeta_size);
        total_size += slot_data->vbmeta_images[n].vbmeta_size;
      }
      if (!cmdline_append_option(
              slot_data, "androidboot.vbmeta.hash_alg", "sha512") ||
          !cmdline_append_uint64_base10(
              slot_data, "androidboot.vbmeta.size", total_size) ||
          !cmdline_append_hex(slot_data,
                              "androidboot.vbmeta.digest",
                              avb_sha512_final(&ctx),
                              AVB_SHA512_DIGEST_SIZE)) {
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
        goto out;
      }
    } break;
    case _AVB_ALGORITHM_NUM_TYPES:
      avb_assert_not_reached();
      break;
  }

  /* Set androidboot.veritymode and androidboot.vbmeta.invalidate_on_error */
  if (toplevel_vbmeta->flags & AVB_VBMETA_IMAGE_FLAGS_HASHTREE_DISABLED) {
    verity_mode = "disabled";
  } else {
    const char* dm_verity_mode = NULL;
    char* new_ret;

    switch (hashtree_error_mode) {
      case AVB_HASHTREE_ERROR_MODE_RESTART_AND_INVALIDATE:
        if (!cmdline_append_option(
                slot_data, "androidboot.vbmeta.invalidate_on_error", "yes")) {
          ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
          goto out;
        }
        verity_mode = "enforcing";
        dm_verity_mode = "restart_on_corruption";
        break;
      case AVB_HASHTREE_ERROR_MODE_RESTART:
        verity_mode = "enforcing";
        dm_verity_mode = "restart_on_corruption";
        break;
      case AVB_HASHTREE_ERROR_MODE_EIO:
        verity_mode = "eio";
        /* For now there's no option to specify the EIO mode. So
         * just use 'ignore_zero_blocks' since that's already set
         * and dm-verity-target.c supports specifying this multiple
         * times.
         */
        dm_verity_mode = "ignore_zero_blocks";
        break;
      case AVB_HASHTREE_ERROR_MODE_LOGGING:
        verity_mode = "logging";
        dm_verity_mode = "ignore_corruption";
        break;
    }
    new_ret = avb_replace(
        slot_data->cmdline, "$(ANDROID_VERITY_MODE)", dm_verity_mode);
    avb_free(slot_data->cmdline);
    slot_data->cmdline = new_ret;
    if (slot_data->cmdline == NULL) {
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
      goto out;
    }
  }
  if (!cmdline_append_option(
          slot_data, "androidboot.veritymode", verity_mode)) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto out;
  }

  ret = AVB_SLOT_VERIFY_RESULT_OK;

out:

  return ret;
}

AvbSlotVerifyResult avb_slot_verify(AvbOps* ops,
                                    const char* const* requested_partitions,
                                    const char* ab_suffix,
                                    AvbSlotVerifyFlags flags,
                                    AvbHashtreeErrorMode hashtree_error_mode,
                                    AvbSlotVerifyData** out_data) {
  AvbSlotVerifyResult ret;
  AvbSlotVerifyData* slot_data = NULL;
  AvbAlgorithmType algorithm_type = AVB_ALGORITHM_TYPE_NONE;
  bool using_boot_for_vbmeta = false;
  AvbVBMetaImageHeader toplevel_vbmeta;
  bool allow_verification_error =
      (flags & AVB_SLOT_VERIFY_FLAGS_ALLOW_VERIFICATION_ERROR);

  /* Fail early if we're missing the AvbOps needed for slot verification.
   *
   * For now, handle get_size_of_partition() not being implemented. In
   * a later release we may change that.
   */
  avb_assert(ops->read_is_device_unlocked != NULL);
  avb_assert(ops->read_from_partition != NULL);
  avb_assert(ops->validate_vbmeta_public_key != NULL);
  avb_assert(ops->read_rollback_index != NULL);
  avb_assert(ops->get_unique_guid_for_partition != NULL);
  /* avb_assert(ops->get_size_of_partition != NULL); */

  if (out_data != NULL) {
    *out_data = NULL;
  }

  /* Allowing dm-verity errors defeats the purpose of verified boot so
   * only allow this if set up to allow verification errors
   * (e.g. typically only UNLOCKED mode).
   */
  if (hashtree_error_mode == AVB_HASHTREE_ERROR_MODE_LOGGING &&
      !allow_verification_error) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT;
    goto fail;
  }

  slot_data = avb_calloc(sizeof(AvbSlotVerifyData));
  if (slot_data == NULL) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto fail;
  }
  slot_data->vbmeta_images =
      avb_calloc(sizeof(AvbVBMetaData) * MAX_NUMBER_OF_VBMETA_IMAGES);
  if (slot_data->vbmeta_images == NULL) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto fail;
  }
  slot_data->loaded_partitions =
      avb_calloc(sizeof(AvbPartitionData) * MAX_NUMBER_OF_LOADED_PARTITIONS);
  if (slot_data->loaded_partitions == NULL) {
    ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
    goto fail;
  }

  ret = load_and_verify_vbmeta(ops,
                               requested_partitions,
                               ab_suffix,
                               allow_verification_error,
                               0 /* toplevel_vbmeta_flags */,
                               0 /* rollback_index_location */,
                               "vbmeta",
                               avb_strlen("vbmeta"),
                               NULL /* expected_public_key */,
                               0 /* expected_public_key_length */,
                               slot_data,
                               &algorithm_type);
  if (!allow_verification_error && ret != AVB_SLOT_VERIFY_RESULT_OK) {
    goto fail;
  }

  /* If things check out, mangle the kernel command-line as needed. */
  if (result_should_continue(ret)) {
    if (avb_strcmp(slot_data->vbmeta_images[0].partition_name, "vbmeta") != 0) {
      avb_assert(
          avb_strcmp(slot_data->vbmeta_images[0].partition_name, "boot") == 0);
      using_boot_for_vbmeta = true;
    }

    /* Byteswap top-level vbmeta header since we'll need it below. */
    avb_vbmeta_image_header_to_host_byte_order(
        (const AvbVBMetaImageHeader*)slot_data->vbmeta_images[0].vbmeta_data,
        &toplevel_vbmeta);

    /* Fill in |ab_suffix| field. */
    slot_data->ab_suffix = avb_strdup(ab_suffix);
    if (slot_data->ab_suffix == NULL) {
      ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
      goto fail;
    }

    /* If verification is disabled, we are done ... we specifically
     * don't want to add any androidboot.* options since verification
     * is disabled.
     */
    if (toplevel_vbmeta.flags & AVB_VBMETA_IMAGE_FLAGS_VERIFICATION_DISABLED) {
      /* Since verification is disabled we didn't process any
       * descriptors and thus there's no cmdline... so set root= such
       * that the system partition is mounted.
       */
      avb_assert(slot_data->cmdline == NULL);
      slot_data->cmdline =
          avb_strdup("root=PARTUUID=$(ANDROID_SYSTEM_PARTUUID)");
      if (slot_data->cmdline == NULL) {
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
        goto fail;
      }
    } else {
      /* Add options - any failure in append_options() is either an
       * I/O or OOM error.
       */
      AvbSlotVerifyResult sub_ret = append_options(ops,
                                                   slot_data,
                                                   &toplevel_vbmeta,
                                                   algorithm_type,
                                                   hashtree_error_mode);
      if (sub_ret != AVB_SLOT_VERIFY_RESULT_OK) {
        ret = sub_ret;
        goto fail;
      }
    }

    /* Substitute $(ANDROID_SYSTEM_PARTUUID) and friends. */
    if (slot_data->cmdline != NULL) {
      char* new_cmdline;
      new_cmdline = sub_cmdline(
          ops, slot_data->cmdline, ab_suffix, using_boot_for_vbmeta);
      if (new_cmdline == NULL) {
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
        goto fail;
      }
      avb_free(slot_data->cmdline);
      slot_data->cmdline = new_cmdline;
    }

    if (out_data != NULL) {
      *out_data = slot_data;
    } else {
      avb_slot_verify_data_free(slot_data);
    }
  }

  if (!allow_verification_error) {
    avb_assert(ret == AVB_SLOT_VERIFY_RESULT_OK);
  }

  return ret;

fail:
  if (slot_data != NULL) {
    avb_slot_verify_data_free(slot_data);
  }
  return ret;
}

void avb_slot_verify_data_free(AvbSlotVerifyData* data) {
  if (data->ab_suffix != NULL) {
    avb_free(data->ab_suffix);
  }
  if (data->cmdline != NULL) {
    avb_free(data->cmdline);
  }
  if (data->vbmeta_images != NULL) {
    size_t n;
    for (n = 0; n < data->num_vbmeta_images; n++) {
      AvbVBMetaData* vbmeta_image = &data->vbmeta_images[n];
      if (vbmeta_image->partition_name != NULL) {
        avb_free(vbmeta_image->partition_name);
      }
      if (vbmeta_image->vbmeta_data != NULL) {
        vbmeta_image->vbmeta_data = NULL;
      }
    }
    avb_free(data->vbmeta_images);
  }
  if (data->loaded_partitions != NULL) {
    size_t n;
    for (n = 0; n < data->num_loaded_partitions; n++) {
      AvbPartitionData* loaded_partition = &data->loaded_partitions[n];
      if (loaded_partition->partition_name != NULL) {
        avb_free(loaded_partition->partition_name);
      }
      if (loaded_partition->data != NULL) {
         if (data->num_loaded_partitions == 1)
           loaded_partition->data = NULL;
      }
    }
    avb_free(data->loaded_partitions);
  }
  avb_free(data);
}

const char* avb_slot_verify_result_to_string(AvbSlotVerifyResult result) {
  const char* ret = NULL;

  switch (result) {
    case AVB_SLOT_VERIFY_RESULT_OK:
      ret = "OK";
      break;
    case AVB_SLOT_VERIFY_RESULT_ERROR_OOM:
      ret = "ERROR_OOM";
      break;
    case AVB_SLOT_VERIFY_RESULT_ERROR_IO:
      ret = "ERROR_IO";
      break;
    case AVB_SLOT_VERIFY_RESULT_ERROR_VERIFICATION:
      ret = "ERROR_VERIFICATION";
      break;
    case AVB_SLOT_VERIFY_RESULT_ERROR_ROLLBACK_INDEX:
      ret = "ERROR_ROLLBACK_INDEX";
      break;
    case AVB_SLOT_VERIFY_RESULT_ERROR_PUBLIC_KEY_REJECTED:
      ret = "ERROR_PUBLIC_KEY_REJECTED";
      break;
    case AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA:
      ret = "ERROR_INVALID_METADATA";
      break;
    case AVB_SLOT_VERIFY_RESULT_ERROR_UNSUPPORTED_VERSION:
      ret = "ERROR_UNSUPPORTED_VERSION";
      break;
    case AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT:
      ret = "ERROR_INVALID_ARGUMENT";
      break;
      /* Do not add a 'default:' case here because of -Wswitch. */
  }

  if (ret == NULL) {
    avb_error("Unknown AvbSlotVerifyResult value.\n");
    ret = "(unknown)";
  }

  return ret;
}
