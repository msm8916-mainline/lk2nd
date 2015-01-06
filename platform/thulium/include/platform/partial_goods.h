/* Copyright (c) 2014, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include <partial_goods.h>

#define QFPROM_PTE_PART_ADDR    0x0007013C

/* Look up table for partial goods */
struct partial_goods table[] =
{
	{0x00001, "/cpus", {{"cpu@100", "device_type"},}},
	{0x00010, "/cpus", {{"cpu@101", "device_type"},}},
	{0x00011, "/cpus", {{"cpu@100", "device_type"},
                        {"cpu@101","device_type"},}},
	{0x00100, "/cpus", {{"cpu@100", "device_type"},
                        {"cpu@101", "device_type"},
                        {"cpu@1",   "device_type"},}},
	{0x10000, "/soc",  {{"qcom,kgsl-3d0", "status"},}},
	{0x10010, "/soc",  {{"qcom,msm-adsp-loader", "status"},}},
	{0x10001, "/soc",  {{"qcom,vidc", "status"},}},
};
