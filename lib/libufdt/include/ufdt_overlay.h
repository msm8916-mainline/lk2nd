/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UFDT_OVERLAY_H
#define UFDT_OVERLAY_H

#include <libfdt.h>

/* Given a buffer in RAM containing the contents of a .dtb file,
 * it initializes an FDT in-place and returns a pointer to the
 * given buffer, or NULL in case of error.
 * In case of error, it may printf() diagnostic messages.
 */
struct fdt_header *ufdt_install_blob(void *blob, size_t blob_size);

/* Given a main_fdt_header buffer and an overlay_fdtp buffer containing the
 * contents of a .dtbo file, it creates a new FDT containing the applied
 * overlay_fdtp in a dto_malloc'd buffer and returns it, or NULL in case of
 * error.
 * It is allowed to modify the buffers (both main_fdt_header and overlay_fdtp
 * buffer) passed in.
 * It does not dto_free main_fdt_header and overlay_fdtp buffer passed in.
 */
struct fdt_header *ufdt_apply_overlay(struct fdt_header *main_fdt_header,
                                      size_t main_fdt_size,
                                      void *overlay_fdtp,
                                      size_t overlay_size);

#endif /* UFDT_OVERLAY_H */
