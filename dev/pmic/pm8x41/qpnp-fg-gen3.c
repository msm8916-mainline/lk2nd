/* Copyright (c) 2015, 2019, The Linux Foundation. All rights reserved.

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
#include <stdlib.h>
#include <pm8x41.h>
#include <pm8x41_hw.h>
#include <qpnp-fg-gen3.h>
#include <reg.h>

#define BATT_INFO_VBATT_LSB	0x41A0
#define BATT_INFO_VBATT_MSB	0x41A1
#define BATT_INFO_CURR_LSB	0x41A2
#define BATT_INFO_CURR_MSB	0x41A3

#define BATT_VOLTAGE_NUMR	122070
#define BATT_VOLTAGE_DENR	1000
#define BATT_CURRENT_NUMR	488281
#define BATT_CURRENT_DENR	1000

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

static int64_t twos_compliment_extend(int64_t val, int sign_bit_pos)
{
	int i, nbytes = DIV_ROUND_UP(sign_bit_pos, 8);
	int64_t mask, val_out;

	val_out = val;
	mask = 1 << sign_bit_pos;
	if (val & mask) {
		for (i = 8; i > nbytes; i--) {
			mask = 0xFFLL << ((i - 1) * 8);
			val_out |= mask;
		}

		if ((nbytes * 8) - 1 > sign_bit_pos) {
			mask = 1 << sign_bit_pos;
			for (i = 1; i <= (nbytes * 8) - sign_bit_pos; i++)
				val_out |= mask << i;
		}
	}

	return val_out;
}

uint32_t fg_gen3_get_battery_voltage(void)
{
	uint8_t buff[2];
	uint16_t temp;
	uint32_t vbat = 0;

	buff[0] = REG_READ(BATT_INFO_VBATT_LSB);
	buff[1] = REG_READ(BATT_INFO_VBATT_MSB);
	temp = buff[1] << 8 | buff[0];
	/* {MSB,LSB} to decode the voltage level, refer register description. */
	vbat = (((uint64_t)temp)*BATT_VOLTAGE_NUMR/BATT_VOLTAGE_DENR);

	return vbat;
}

int fg_gen3_get_battery_current(void)
{
	uint8_t buff[2];
	int current = 0;
	int64_t temp = 0;

	buff[0] = REG_READ(BATT_INFO_CURR_LSB);
	buff[1] = REG_READ(BATT_INFO_CURR_MSB);
	temp = buff[1] << 8 | buff[0];
	/* Sign bit is bit 15 */
	temp = twos_compliment_extend(temp, 15);
	current = (((int64_t)temp)*BATT_CURRENT_NUMR/BATT_CURRENT_DENR);

	return current;
}
