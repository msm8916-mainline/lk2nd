/* Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
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

#include <bits.h>
#include <debug.h>
#include <reg.h>

#define QPNP_LCDB_BASE				0x3ec00
#define QPNP_LCDB_BST_OUTPUT_VOLTAGE_REG	0x41
#define QPNP_LCDB_LDO_OUTPUT_VOLTAGE_REG	0x71
#define QPNP_LCDB_NCP_OUTPUT_VOLTAGE_REG	0x81
#define QPNP_LCDB_ENABLE_CTL_REG		0x46
#define QPNP_LCDB_PWRUP_PWRDN_CTL_REG		0x66
#define QPNP_LCDB_MODULE_RDY_REG		0x45

#define QPNP_LCDB_BST_OUTPUT_VOLTAGE_MASK	0x3F
#define QPNP_LCDB_LDO_OUTPUT_VOLTAGE_MASK	0x1F
#define QPNP_LCDB_NCP_OUTPUT_VOLTAGE_MASK	0x1F
#define QPNP_LCDB_ENABLE_CTL_MASK		0x80
#define QPNP_LCDB_PWRUP_DLY_MASK		0x0C
#define QPNP_LCDB_PWRDN_DLY_MASK		0x03
#define QPNP_LCDB_MODULE_RDY_MASK		0x80

#define LCDB_LDO_INIT_VOLTAGE_UV		4000000
#define LCDB_LDO_MAX_VOLTAGE_UV			6000000
#define LCDB_NCP_INIT_VOLTAGE_UV		4000000
#define LCDB_NCP_MAX_VOLTAGE_UV			6000000
#define LCDB_BOOST_INIT_VOLTAGE_UV		4700000
#define LDO_VREG_STEP_SIZE_UV			50000
#define NCP_VREG_STEP_SIZE_UV			50000
#define BST_VREG_STEP_SIZE_UV			25000
#define LCDB_BOOST_HEADROOM_VOLT_UV		150000
#define QPNP_LCDB_PWRUP_DLY_MAX_MS		3
#define QPNP_LCDB_PWRDN_DLY_MAX_MS		3
#define LCDB_MODULE_RDY				BIT(7)

#define LCDB_ENABLE_SHIFT			7
#define LCDB_PWRUP_DLY_SHIFT			2
#define LCDB_STEP_UV				500000

#define min(a, b) (a) < (b) ? a : b

struct qpnp_lcdb {
	uint32_t lcdb_base;
	uint16_t lcdb_pwrup_dly_ms;
	uint16_t lcdb_pwrdn_dly_ms;
	uint32_t ldo_min_volt;
	uint32_t ldo_max_volt;
	uint32_t ldo_init_volt;
	uint32_t ncp_min_volt;
	uint32_t ncp_max_volt;
	uint32_t ncp_init_volt;
	uint32_t bst_init_volt;
};

/* LCDB Init */
int qpnp_lcdb_init(struct qpnp_wled_config_data *config);

/* LCDB Enable */
int qpnp_lcdb_enable(bool enable);
