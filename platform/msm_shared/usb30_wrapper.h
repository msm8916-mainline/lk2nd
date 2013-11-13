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
#ifndef _USB30_WRAPPER_H
#define _USB30_WRAPPER_H

typedef enum
{
	DBM_MODE_BYPASS = 0,
	DBM_MODE_ENABLED
} dbm_mode_t;


typedef struct {
	void     *qscratch_base;

} usb_wrapper_config_t;

typedef struct
{
   void      *base;

} usb_wrapper_dev_t;


usb_wrapper_dev_t *usb_wrapper_init(usb_wrapper_config_t *config);

void usb_wrapper_dbm_mode(usb_wrapper_dev_t *dev, dbm_mode_t mode);
void usb_wrapper_ram_configure(usb_wrapper_dev_t *dev);

void usb_wrapper_ss_phy_reset(usb_wrapper_dev_t *dev);
void usb_wrapper_ss_phy_configure(usb_wrapper_dev_t *dev);
void usb_wrapper_ss_phy_electrical_config(usb_wrapper_dev_t *dev);

void usb_wrapper_hs_phy_init(usb_wrapper_dev_t *dev);
void usb_wrapper_hs_phy_configure(usb_wrapper_dev_t *dev);

void usb_wrapper_vbus_override(usb_wrapper_dev_t *dev);

void usb_wrapper_workaround_10(usb_wrapper_dev_t *dev);
void usb_wrapper_workaround_11(usb_wrapper_dev_t *dev);
void usb_wrapper_workaround_13(usb_wrapper_dev_t *dev);
void usb_wrapper_hs_phy_ctrl_force_write(usb_wrapper_dev_t *dev);

#endif
