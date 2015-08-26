/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
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
 *
 */

#include <stdlib.h>

struct mdtp_ui_defs {

    // Image dimensions
    uint32_t error_msg_width;
    uint32_t error_msg_height;
    uint32_t main_text_width;
    uint32_t main_text_height;
    uint32_t pin_digit_width;
    uint32_t pin_digit_height;
    uint32_t ok_button_width;
    uint32_t ok_button_height;
    uint32_t digits_instructions_width;
    uint32_t digits_instructions_height;
    uint32_t pin_instructions_width;
    uint32_t pin_instructions_height;

    // Image offsets
    uint32_t error_msg_offset;
    uint32_t initial_delay_offset;
    uint32_t enter_pin_offset;
    uint32_t invalid_pin_offset;
    uint32_t pin_digit_0_offset;
    uint32_t pin_digits_offset;
    uint32_t pin_selected_digit_0_offset;
    uint32_t ok_button_offset;
    uint32_t selected_ok_button_offset;
    uint32_t digits_instructions_offset;
    uint32_t pin_instructions_offset;

    //Display settings
    uint32_t digit_space;
};

struct mdtp_target_efuse {
    uint32_t address;
    uint32_t start;
};

struct mdtp_ui_defs mdtp_get_target_ui_defs();

int mdtp_get_target_efuse(struct mdtp_target_efuse* target_efuse);
