/*
 * * Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include <debug.h>
#include <reg.h>
#include <platform/iomap.h>
#include <platform/pmic.h>

#define TRUE  1
#define FALSE 0

#define PM_IRQ_ID_TO_BLOCK_INDEX(id) (uint8_t)(id / 8)
#define PM_IRQ_ID_TO_BIT_MASK(id)    (uint8_t)(1 << (id % 8))

typedef int (*pm8058_write_func) (unsigned char *, unsigned short,
                                  unsigned short);
extern int pa1_ssbi2_write_bytes(unsigned char *buffer, unsigned short length,
                                 unsigned short slave_addr);

/*PM8058*/
void pm8058_write_one(unsigned data, unsigned address)
{
    pm8058_write_func wr_function = &pa1_ssbi2_write_bytes;
    if (wr_function == NULL)
        return;
    if ((*wr_function) (&data, 1, address))
        dprintf(CRITICAL, "Error in initializing register\n");

}

int pm8058_get_gpio_status( pm_sec_gpio_irq_id_type gpio_irq, bool *rt_status)
{
    unsigned block_index, reg_data, reg_mask;
    int errFlag;

    block_index = PM_IRQ_ID_TO_BLOCK_INDEX(gpio_irq);

    /* select the irq block */
    errFlag =pa1_ssbi2_write_bytes(&block_index,1,IRQ_BLOCK_SEL_USR_ADDR);
    if(errFlag)
    {
        dprintf(INFO,"Device Timeout");
        return 1;
    }

    /* read real time status */
    errFlag =pa1_ssbi2_read_bytes(&reg_data,1,IRQ_STATUS_RT_USR_ADDR);
    if(errFlag)
    {
        dprintf(INFO,"Device Timeout");
        return 1;
    }
    reg_mask = PM_IRQ_ID_TO_BIT_MASK(gpio_irq);

    if ((reg_data & reg_mask) == reg_mask )
    {
        /* The RT Status is high. */
        *rt_status = TRUE;
    }
    else
    {
        /* The RT Status is low. */
         *rt_status = FALSE;
    }
    return 0;
}

bool pm8058_gpio_get(unsigned int gpio)
{
	pm_sec_gpio_irq_id_type gpio_irq;
	bool status;
	int ret;

	gpio_irq = gpio + PM_GPIO01_CHGED_ST_IRQ_ID;
	ret = pm8058_get_gpio_status(gpio_irq, &status);

	if(ret)
		dprintf(CRITICAL,"pm8058_gpio_get failed\n");

	return status;
}

/*PM8901*/
extern int pa2_ssbi2_write_bytes(unsigned char *buffer, unsigned short length,
                                 unsigned short slave_addr);
extern int pa2_ssbi2_read_bytes(unsigned char *buffer, unsigned short length,
                                unsigned short slave_addr);
/*
 * Write to the control registers on PMIC via the SSBI2 interface.
 * Returns : (0) on success and (-1) on error.
 */
int pm8901_write(uint8_t * buffer, uint32_t length, uint32_t slave_addr)
{
    return pa2_ssbi2_write_bytes(buffer, length, slave_addr);
}

/*
 * Read from the control registers on PMIC via the SSBI2 interface.
 * Returns : (0) on success and (-1) on error.
 */
int pm8901_read(uint8_t * buffer, uint32_t length, uint32_t slave_addr)
{
    return pa2_ssbi2_read_bytes(buffer, length, slave_addr);
}

/*
 * PMIC 8901 LDO vreg read.
 */
int pm8901_test_bank_read(uint8_t * buffer, uint8_t bank, uint16_t addr)
{
    int ret = pm8901_write(&bank, 1, addr);
    /* if the write does not work we can't read. */
    if (ret) {
        return ret;
    }

    return pm8901_read(buffer, 1, addr);
}

/*
 * PMIC 8901 LDO vreg write.
 */
int pm8901_vreg_write(uint8_t * buffer, uint8_t mask, uint16_t addr,
                      uint8_t prev_val)
{
    uint8_t reg;

    /* Clear the bits we want to try and set. */
    reg = (prev_val & ~mask);
    /* Set the bits we want to set, before writing them to addr */
    reg |= (*buffer & mask);
    return pm8901_write(&reg, 1, addr);
}
