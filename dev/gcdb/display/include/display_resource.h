/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _DISPLAY_RESOURCE_H_
#define _DISPLAY_RESOURCE_H_

/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include <debug.h>
#include <smem.h>
#include <board.h>

#define TOTAL_LDO_CTRL 5

/*---------------------------------------------------------------------------*/
/* Structure definition                                                      */
/*---------------------------------------------------------------------------*/

/*GPIO pin structure to define reset pin, enable pin, te pin, etc. */
typedef struct gpio_pin{

	char    *pin_source;
	uint32_t pin_id;
	uint32_t pin_strength;
	uint32_t pin_direction;
	uint32_t pin_pull;
	uint32_t pin_state;
};

/*LDO entry structure for different LDO entries. */
typedef struct ldo_entry{
	char    *ldo_name;
	uint32_t ldo_id;
	uint32_t ldo_type;
	uint32_t ldo_voltage;
	uint32_t ldo_enable_load;
	uint32_t ldo_disable_load;
	uint32_t ldo_preon_sleep;
	uint32_t ldo_poston_sleep;
	uint32_t ldo_preoff_sleep;
	uint32_t ldo_postoff_sleep;
};

#endif /*_DISPLAY_RESOURCE_H_ */
