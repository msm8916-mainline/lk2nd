# SPDX-License-Identifier: BSD-3-Clause
TARGET := msm8960
LK2ND_BUNDLE_DTB ?= bundle.dtb

DEFINES += ENABLE_KASLRSEED_SUPPORT=1
# Constrained by MIPI_FB_ADDR at 0x89000000. ramoops@88d00000 for flo and mako.
# The mmu_section_table doesn't map beyond 0x88000000, which is still plenty.
LK2ND_BOOT_MEM_SIZE := 0x08000000

include lk2nd/project/lk2nd.mk
