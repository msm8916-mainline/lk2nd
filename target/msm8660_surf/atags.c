/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <reg.h>
#include <debug.h>
#include <smem.h>

#define EBI1_SIZE_60M   0x03C00000
#define EBI1_SIZE_128M  0x08000000
#define EBI1_SIZE_60M  (254 * 1024 * 1024)
#define EBI1_ADDR_60M   0x40200000
#define EBI1_ADDR_128M  0x48000000

unsigned* target_atag_mem(unsigned* ptr)
{
    /* ATAG_MEM for 51MB + 128MB setup */
    *ptr++ = 4;
    *ptr++ = 0x54410002;
    *ptr++ = EBI1_SIZE_60M;
    *ptr++ = EBI1_ADDR_60M;

    /* ATAG_MEM */
    /*
    *ptr++ = 4;
    *ptr++ = 0x54410002;
    *ptr++ = EBI1_SIZE_128M;
    *ptr++ = EBI1_ADDR_128M;
    */
    return ptr;
}

void *target_get_scratch_address(void)
{
    return ((void *)SCRATCH_ADDR);
}
