# SPDX-License-Identifier: BSD-3-Clause
TARGET := mdm9640
LK2ND_DISPLAY ?= # Continuous splash does not work on mdm9640 at the moment
include lk2nd/project/lk2nd.mk
