/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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
#ifndef _USB30_DWC_HW_
#define _USB30_DWC_HW_

void dwc_ep_cmd_start_transfer(dwc_dev_t *dev, uint8_t ep_phy_num);
void dwc_ep_cmd_end_transfer(dwc_dev_t *dev, uint8_t ep_phy_num);
void dwc_ep_cmd_set_config(dwc_dev_t *dev, uint8_t index, uint8_t action);
void dwc_ep_cmd_set_transfer_resource(dwc_dev_t *dev, uint8_t index);
void dwc_ep_cmd_stall(dwc_dev_t *dev, uint8_t ep_phy_num);
void dwc_ep_cmd_start_new_config(dwc_dev_t *dev,
								 uint8_t ep_phy_num,
								 uint8_t resource_idx);

void dwc_ep_enable(dwc_dev_t *dev, uint8_t ep_phy_num);
void dwc_ep_disable(dwc_dev_t *dev, uint8_t ep_phy_num);
void dwc_ep_disable_non_control(dwc_dev_t *dev);

void dwc_event_init(dwc_dev_t *dev);
uint16_t dwc_event_get_next(dwc_dev_t *dev, uint32_t *event);
void dwc_event_processed(dwc_dev_t *dev, uint16_t count);
void dwc_event_device_enable(dwc_dev_t *dev, uint32_t events);

uint32_t dwc_coreid(dwc_dev_t *dev);
uint8_t dwc_connectspeed(dwc_dev_t *dev);
uint8_t dwc_device_run_status(dwc_dev_t *dev);
void dwc_gctl_init(dwc_dev_t *dev);
void dwc_axi_master_config(dwc_dev_t *dev);
#endif
