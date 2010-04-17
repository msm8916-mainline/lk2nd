/*
 * Copyright (c) 2007, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>

int main(int argc, char *argv[])
{
    struct stat s;
    unsigned size, base;
    int unified_boot = 0;
    unsigned unified_boot_magic[20];
    unsigned non_unified_boot_magic[10];
    unsigned magic_len = 0;
    unsigned *magic;
    int fd;

    if(argc < 3) {
        fprintf(stderr,"usage: mkheader <bin> <hdr>\n");
        return -1;
    }

    if (argc == 4) {
        if(!strcmp("unified-boot",argv[3])) {
            unified_boot = 1;
        }
    }

    if(stat(argv[1], &s)) {
        perror("cannot stat binary");
        return -1;
    }

    if(unified_boot) {
        magic = unified_boot_magic;
        magic_len = sizeof(unified_boot_magic);
    } else {
        magic = non_unified_boot_magic;
        magic_len = sizeof(non_unified_boot_magic);
    }

    size = s.st_size;
#if MEMBASE
    base = MEMBASE;
#else
    base = 0;
#endif

    magic[0] = 0x00000005; /* appsbl */
    magic[1] = 0x00000002; /* nand */
    magic[2] = 0x00000000;
    magic[3] = base;
    magic[4] = size;
    magic[5] = size;
    magic[6] = size + base;
    magic[7] = 0x00000000;
    magic[8] = size + base;
    magic[9] = 0x00000000;

    if (unified_boot == 1)
    {
        magic[10] = 0x33836685; /* cookie magic number */
        magic[11] = 0x00000001; /* cookie version */
        magic[12] = 0x00000002; /* file formats */
        magic[13] = 0x00000000;
        magic[14] = 0x00500000; /* 5M for boot.img */
        magic[15] = 0x00000000;
        magic[16] = 0x00000000;
        magic[17] = 0x00000000;
        magic[18] = 0x00000000;
        magic[19] = 0x00000000;
    }

    fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd < 0) {
        perror("cannot open header for writing");
        return -1;
    }
    if(write(fd, magic, magic_len) != magic_len) {
        perror("cannot write header");
        close(fd);
        unlink(argv[2]);
        return -1;
    }
    close(fd);

    return 0;
}
