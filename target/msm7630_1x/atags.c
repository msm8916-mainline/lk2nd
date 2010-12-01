/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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

#define EBI1_ADDR_128M        0x08000000
#define SIZE_256M             0x10000000
#define SIZE_128M             0x08000000
#define SIZE_1M               0x00100000

static int scratch_addr = -1;
int smem_ram_ptable_init(struct smem_ram_ptable *);

unsigned* target_atag_mem(unsigned* ptr)
{
    struct smem_ram_ptable ram_ptable;
    unsigned i = 0;

    if (smem_ram_ptable_init(&ram_ptable))
    {
        for (i = 0; i < ram_ptable.len; i++)
        {
            if ((ram_ptable.parts[i].attr == READWRITE)
                && (ram_ptable.parts[i].domain == APPS_DOMAIN)
                && (ram_ptable.parts[i].start != 0x0)
                && (!(ram_ptable.parts[i].size < SIZE_1M)))
            {
                /* ATAG_MEM */
                *ptr++ = 4;
                // Tag EBI-1 memory as unstable.
                if(ram_ptable.parts[i].category == EBI1_CS0) {
                    // if EBI-1 CS-0 is 256Mb then this is a 2x256 target and
                    // the kernel can reserve this mem region as unstable.
                    // This memory region can be activated when the kernel
                    // receives a request from Android init scripts.
                    if(ram_ptable.parts[i].size == SIZE_256M)
                        *ptr++ = 0x5441000A; //Deep-Power-Down Tag.

                    //if EBI-1 CS-0 s 128Mb then this is a 2x128 target.
                    //Android + Kernel + PMEM regions account for more than
                    //128Mb and the target will not be able to boot with just
                    //one memory bank active and the second memory bank is reserved.
                    //In the case of 2x128 the tag is set to SelfRefresh Only.
                    else if(ram_ptable.parts[i].size == SIZE_128M)
                        *ptr++ = 0x5441000B; //Self-Refresh Tag.
                }
                else
                    *ptr++ = 0x54410002;

                *ptr++ = ram_ptable.parts[i].size;
                *ptr++ = ram_ptable.parts[i].start;
            }
        }
    }
    else
    {
        dprintf(CRITICAL, "ERROR: Unable to read RAM partition\n");
        ASSERT(0);
    }

    return ptr;
}

void *target_get_scratch_address(void)
{
    struct smem_ram_ptable ram_ptable;
    unsigned i = 0;

    if (smem_ram_ptable_init(&ram_ptable))
    {
        for (i = 0; i < ram_ptable.len; i++)
        {
            if ((ram_ptable.parts[i].attr == READWRITE)
                && (ram_ptable.parts[i].domain == APPS_DOMAIN)
                && (ram_ptable.parts[i].start != 0x0))
            {
                if (ram_ptable.parts[i].size >= FASTBOOT_BUF_SIZE)
                {
                    scratch_addr = ram_ptable.parts[i].start;
                    break;
                }
            }
        }
    }
    else
    {
        dprintf(CRITICAL, "ERROR: Unable to read RAM partition\n");
        ASSERT(0);
    }

    return (void *)((scratch_addr == -1) ? EBI1_ADDR_128M : scratch_addr);
}
