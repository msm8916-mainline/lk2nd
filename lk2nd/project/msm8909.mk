# SPDX-License-Identifier: BSD-3-Clause

# lk2nd provides its own mainline-friendly partial-goods implementation
override ENABLE_PARTIAL_GOODS_SUPPORT := 0
MODULES += lk2nd/partial-goods
