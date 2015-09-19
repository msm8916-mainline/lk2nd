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

#include "mdtp_defs.h"

struct mdtp_ui_defs mdtp_ui_defs_msm8952 = {
        // Image dimensions
        952,      // error_msg_width;
        143,      // error_msg_height;
        920,      // main_text_width;
        194,      // main_text_height;
        120,      // pin_digit_width;
        120,      // pin_digit_height;
        432,      // ok_button_width;
        106,      // ok_button_height;
        932,      // digits_instructions_width;
        112,      // digits_instructions_height;
        620,      // pin_instructions_width;
        137,      // pin_instructions_height;

        // Image offsets
        0x1000,   // error_msg_offset;
        0x65000,  // initial_delay_offset;
        0xE8000,  // enter_pin_offset;
        0x16B000, // invalid_pin_offset;
        0x1EE000, // pin_digit_0_offset;
        0xB000,   // pin_digits_offset;
        0x25C000, // pin_selected_digit_0_offset;
        0x2CA000, // ok_button_offset;
        0x2EC000, // selected_ok_button_offset;
        0x30E000, // digits_instructions_offset;
        0x35B000, // pin_instructions_offset;

        //Display settings
        8         // digit_space;
};

struct mdtp_ui_defs mdtp_get_target_ui_defs()
{
    return mdtp_ui_defs_msm8952;
}
