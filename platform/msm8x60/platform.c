/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
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

#include <debug.h>
#include <reg.h>

#include <dev/fbcon.h>
#include <kernel/thread.h>
#include <platform/debug.h>
#include <platform/iomap.h>
#include <i2c_qup.h>

#define CONVERT_ENDIAN_U32(val)                   \
    ((((uint32_t)(val) & 0x000000FF) << 24) |     \
     (((uint32_t)(val) & 0x0000FF00) << 8)  |     \
     (((uint32_t)(val) & 0x00FF0000) >> 8)  |     \
     (((uint32_t)(val) & 0xFF000000) >> 24))

#define CONVERT_ENDIAN_U16(val)              \
    ((((uint16_t)(val) & 0x00FF) << 8) |     \
     (((uint16_t)(val) & 0xFF00) >> 8))

/* Configuration Data Table */
#define CDT_MAGIC_NUMBER    0x43445400
struct cdt_header
{
    uint32_t magic;   /* Magic number */
    uint16_t version; /* Version number */
    uint32_t reserved1;
    uint32_t reserved2;
}__attribute__((packed));

void platform_init_interrupts(void);
void platform_init_timer();

void uart3_clock_init(void);
void uart_init(void);

struct fbcon_config *lcdc_init(void);

void platform_early_init(void)
{
    platform_init_interrupts();
    platform_init_timer();
}

void platform_init(void)
{
    dprintf(INFO, "platform_init()\n");
}

void display_init(void)
{
}

void secondary_core(unsigned sec_entry)
{
    writel(sec_entry, 0x2A040020);
    writel(0x0, 0x009035A0); //VDD_SC1_ARRAY_CLAMP_GFS_CTL
    writel(0x0, 0x00902D80); //SCSS_CPU1CORE_RESET
    writel(0x3, 0x00902E64); //SCSS_DBG_STATUS_CORE_PWRDUP
}

static struct qup_i2c_dev* dev = NULL;

uint32_t eprom_read (uint16_t addr, uint8_t count) {
    uint32_t ret = 0;
    if(!dev){
        return ret;
    }
    /* Create a i2c_msg buffer, that is used to put the controller into
     * read mode and then to read some data.
     */
    struct i2c_msg msg_buf[] = {
        {EEPROM_I2C_ADDRESS, I2C_M_WR, 2, &addr},
        {EEPROM_I2C_ADDRESS, I2C_M_RD, count, &ret}
    };

    qup_i2c_xfer(dev, msg_buf, 2);
    return ret;
}

/* Read EEPROM to find out product id. Return 0 in case of failure */
uint32_t platform_id_read (void)
{
    uint32_t id = 0;
    uint16_t offset = 0;
    dev = qup_i2c_init(GSBI8_BASE, 100000, 24000000);
    if(!dev){
        return id;
    }
    /* Check if EPROM is valid */
    if (CONVERT_ENDIAN_U32(eprom_read(0, 4)) == CDT_MAGIC_NUMBER)
    {
        /* Get offset for platform ID info from Meta Data block 0 */
        offset = eprom_read(CONVERT_ENDIAN_U16(0 +
            sizeof(struct cdt_header)), 2);
        /* Read platform ID */
        id = eprom_read(CONVERT_ENDIAN_U16(offset), 4);
        id = CONVERT_ENDIAN_U32(id);
        id = (id & 0x00FF0000) >> 16;
    }
    return id;
}

