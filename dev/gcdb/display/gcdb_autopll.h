/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#ifndef _GCDB_AUTOPLL_H_
#define _GCDB_AUTOPLL_H_

/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include <debug.h>

#define VCO_MIN_CLOCK 350000000
#define VCO_MAX_CLOCK 750000000

#define HALFBIT_CLOCK1 175000000 /* VCO min clock div by 2 */
#define HALFBIT_CLOCK2 88000000  /* VCO min clock div by 4 */
#define HALFBIT_CLOCK3 44000000  /* VCO min clock div by 8 */
#define HALFBIT_CLOCK4 40000000  /* VCO min clock div by 9 */

#define VCO_MIN_CLOCK_20NM 	1000000000
#define VCO_MAX_CLOCK_20NM 	2000000000

#define HALF_VCO_MIN_CLOCK_20NM (VCO_MIN_CLOCK_20NM >> 1)

#define HALFBIT_CLOCK1_20NM 	500000000 /* VCO min clock div by 2 */
#define HALFBIT_CLOCK2_20NM 	250000000  /* VCO min clock div by 4 */
#define HALFBIT_CLOCK3_20NM 	125000000  /* VCO min clock div by 8 */
#define HALFBIT_CLOCK4_20NM 	120000000  /* VCO min clock div by 9 */

#define BITS_24 24
#define BITS_18 18
#define BITS_16 16

/*---------------------------------------------------------------------------*/
/* Structure definition                                                      */
/*---------------------------------------------------------------------------*/

uint32_t calculate_clock_config(struct msm_panel_info *pinfo);

#endif /*_GCDB_AUTOPLL_H_ */
