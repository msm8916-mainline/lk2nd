# SPDX-License-Identifier: BSD-3-Clause
TARGET := msm8960
LK2ND_BUNDLE_DTB ?= bundle.dtb

DEFINES += ENABLE_KASLRSEED_SUPPORT=1

include lk2nd/project/lk2nd.mk
