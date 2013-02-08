/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation, Inc. nor the names of its
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
 */

#include <pm8x41_adc.h>
#include <debug.h>

void adc_test()
{
	uint32_t vadc_chan1;
	uint32_t vadc_chan2;
	uint16_t iusb = 3250000;
	uint16_t ibat = 2500000;
	uint16_t batt_id_chan = 49;
	uint16_t vbat_sns_chan = 6;

	/*
	 * TEST: Read the voltage on batt_id & vbat_sns channels
	 */
	vadc_chan1 = pm8x41_adc_channel_read(batt_id_chan);
	dprintf(INFO, "The channel [%ud] voltage is :%ud\n",batt_id_chan, vadc_chan1);

	vadc_chan2 = pm8x41_adc_channel_read(vbat_sns_chan);
	dprintf(INFO, "The channel [%ud] voltage is :%ud\n",vbat_sns_chan, vadc_chan2);

	/*
	 * TEST: Set the IUSB & IBAT max values
	 */

	if (!pm8x41_iusb_max_config(iusb))
		dprintf(INFO, "Iusb max current is set\n");

	if (!pm8x41_ibat_max_config(ibat))
		dprintf(INFO, "Ibat max current is set\n");

}
