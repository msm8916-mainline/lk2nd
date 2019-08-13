/* Copyright (c) 2015, 2018-2019, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Fundation, Inc. nor the names of its
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
 *
 */

#include <regulator.h>
#include <rpm-smd.h>
#include <bits.h>
#include <debug.h>
#include <platform.h>
#include <board.h>


static uint32_t ldo1[][11]=
{
	{
		LDOA_RES_TYPE, 1,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 1,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 1200000,
		KEY_CURRENT, 4, 40,
	},
};

static uint32_t ldo2[][11]=
{
	{
		LDOA_RES_TYPE, 2,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 2,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 1200000,
		KEY_CURRENT, 4, 40,
	},
};

static uint32_t ldo5[][11] = {
	{
		LDOA_RES_TYPE, 5,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 5,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 1800000,
		KEY_CURRENT, 4, 150,
	},
};

static uint32_t ldo6[][11]=
{
	{
		LDOA_RES_TYPE, 6,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 6,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 1800000,
		KEY_CURRENT, 4, 150,
	},
};

static uint32_t ldo6_pm660[][11]=
{
	{
		LDOA_RES_TYPE, 6,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 6,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 800000,
		KEY_CURRENT, 4, 150,
	},
};

static uint32_t ldo13_pm660[][11]=
{
	{
		LDOA_RES_TYPE, 13,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 13,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 1800000,
		KEY_CURRENT, 4, 40,
	},
};

static uint32_t ldo15_pm660[][11]=
{
	{
		LDOA_RES_TYPE, 15,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 15,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 2800000,
		KEY_CURRENT, 4, 40,
	},
};

static uint32_t ldo17[][11]=
{
	{
		LDOA_RES_TYPE, 17,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 17,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 2850000,
		KEY_CURRENT, 4, 40,
	},
};

void regulator_enable(uint32_t enable)
{
	uint32_t hw_subtype = board_hardware_subtype();

	if (platform_is_msm8956()) {
		if (enable & REG_LDO1)
			rpm_send_data(&ldo1[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);

	} else if (platform_is_sdm439() || platform_is_sdm429() || platform_is_sdm429w()) {
		if (enable & REG_LDO5)
			rpm_send_data(&ldo5[GENERIC_ENABLE][0],
				36, RPM_REQUEST_TYPE);
	} else {
		if (enable & REG_LDO2)
			rpm_send_data(&ldo2[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);
	}

	if ((platform_is_sdm429() && (board_hardware_subtype() == HW_PLATFORM_SUBTYPE_429W_PM660)) || platform_is_sdm429w()) {
		if (enable & REG_LDO13)
			rpm_send_data(&ldo13_pm660[GENERIC_ENABLE][0],
				36, RPM_REQUEST_TYPE);
		if (enable & REG_LDO15)
			rpm_send_data(&ldo15_pm660[GENERIC_ENABLE][0],
				36, RPM_REQUEST_TYPE);
	}

	if (enable & REG_LDO17)
		rpm_send_data(&ldo17[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);

	if (enable & REG_LDO6) {
		if ((platform_is_sdm429() || platform_is_sdm429w()) && hw_subtype
				== HW_PLATFORM_SUBTYPE_429W_PM660)
			rpm_send_data(&ldo6_pm660[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);
		else
			rpm_send_data(&ldo6[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);
	}
}

void regulator_disable(uint32_t enable)
{
	if (platform_is_msm8956()) {
		if (enable & REG_LDO1)
			rpm_send_data(&ldo1[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);

	} else if (platform_is_sdm439() || platform_is_sdm429() || platform_is_sdm429w()) {
		if (enable & REG_LDO5)
			rpm_send_data(&ldo5[GENERIC_DISABLE][0],
				36, RPM_REQUEST_TYPE);
	} else {
		if (enable & REG_LDO2)
			rpm_send_data(&ldo2[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);
	}

	if (enable & REG_LDO17)
		rpm_send_data(&ldo17[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);

	if (enable & REG_LDO6)
		rpm_send_data(&ldo6[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);
}
