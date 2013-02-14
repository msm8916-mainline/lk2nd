/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
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
#include <spmi.h>
#include <platform/iomap.h>
#include <platform/irqs.h>
#include <platform/interrupts.h>

static uint32_t pmic_arb_chnl_num;
static uint32_t pmic_arb_owner_id;
static uint8_t pmic_irq_perph_id;
static spmi_callback callback;

/* Function to initialize SPMI controller.
 * chnl_num : Channel number to be used by this EE.
 */
void spmi_init(uint32_t chnl_num, uint32_t owner_id)
{
	/* Initialize PMIC Arbiter Channel Number */
	pmic_arb_chnl_num = chnl_num;
	pmic_arb_owner_id = owner_id;
}

static void write_wdata_from_array(uint8_t *array,
	                               uint8_t reg_num,
	                               uint8_t array_size,
	                               uint8_t* bytes_written)
{
	uint32_t shift_value[] = {0, 8, 16, 24};
	int i;
	int j;
	uint32_t val = 0;

	/* Write to WDATA */
	for (i = 0; (*bytes_written < array_size) && (i < 4); i++)
	{
		val |= (uint32_t)(array[*bytes_written]) << shift_value[i];
		(*bytes_written)++;
	}

	writel(val, PMIC_ARB_CHNLn_WDATA(pmic_arb_chnl_num, reg_num));
}


/* Initiate a write cmd by writing to cmd register.
 * Commands are written according to cmd parameters
 * cmd->opcode   : SPMI opcode for the command
 * cmd->priority : Priority of the command
 *                 High priority : 1
 *                 Low Priority : 0
 * cmd->address  : SPMI Peripheral Address.
 * cmd->offset   : Offset Address for the command.
 * cmd->bytecnt  : Number of bytes to be written.
 *
 * param is the parameter to the command
 * param->buffer : Value to be written
 * param->size   : Size of the buffer.
 *
 * return value : 0 if success, the error bit set on error
 */
unsigned int pmic_arb_write_cmd(struct pmic_arb_cmd *cmd,
                                struct pmic_arb_param *param)
{
	uint32_t bytes_written = 0;
	uint32_t error;
	uint32_t val = 0;

	/* Disable IRQ mode for the current channel*/
	writel(0x0, PMIC_ARB_CHNLn_CONFIG(pmic_arb_chnl_num));

	/* Write parameters for the cmd */
	if (cmd == NULL)
	{
		dprintf(CRITICAL,"PMIC arbiter error, no command provided\n");
		return 1;
	}

	/* Write the data bytes according to the param->size
	 * Can write upto 8 bytes.
	 */

	/* Write first 4 bytes to WDATA0 */
	write_wdata_from_array(param->buffer, 0, param->size, &bytes_written);

	if (bytes_written < param->size)
	{
		/* Write next 4 bytes to WDATA1 */
		write_wdata_from_array(param->buffer, 1, param->size, &bytes_written);
	}

	/* Fill in the byte count for the command
	 * Note: Byte count is one less than the number of bytes transferred.
	 */
	cmd->byte_cnt = param->size - 1;
	/* Fill in the Write cmd opcode. */
	cmd->opcode = SPMI_CMD_EXT_REG_WRTIE_LONG;

	/* Write the command */
	val = 0;
	val |= ((uint32_t)(cmd->opcode) << PMIC_ARB_CMD_OPCODE_SHIFT);
	val |= ((uint32_t)(cmd->priority) << PMIC_ARB_CMD_PRIORITY_SHIFT);
	val |= ((uint32_t)(cmd->slave_id) << PMIC_ARB_CMD_SLAVE_ID_SHIFT);
	val |= ((uint32_t)(cmd->address) << PMIC_ARB_CMD_ADDR_SHIFT);
	val |= ((uint32_t)(cmd->offset) << PMIC_ARB_CMD_ADDR_OFFSET_SHIFT);
	val |= ((uint32_t)(cmd->byte_cnt));

	writel(val, PMIC_ARB_CHNLn_CMD0(pmic_arb_chnl_num));

	/* Wait till CMD DONE status */
	while (!(val = readl(PMIC_ARB_CHNLn_STATUS(pmic_arb_chnl_num))));

	/* Check for errors */
	error = val ^ (1 << PMIC_ARB_CMD_DONE);
	if (error)
	{
		dprintf(CRITICAL, "SPMI write command failure: \
			cmd_id = %u, error = %u\n", cmd->opcode, error);
		return error;
	}
	else
		return 0;
}

static void read_rdata_into_array(uint8_t *array,
                                  uint8_t reg_num,
                                  uint8_t array_size,
                                  uint8_t* bytes_read)
{
	uint32_t val = 0;
	uint32_t mask_value[] = {0xFF, 0xFF00, 0xFF0000, 0xFF000000};
	uint8_t shift_value[] = {0, 8, 16, 24};
	int i;

	val = readl(PMIC_ARB_CHNLn_RDATA(pmic_arb_chnl_num, reg_num));

	/* Read at most 4 bytes */
	for (i = 0; (i < 4) && (*bytes_read < array_size); i++)
	{
		array[*bytes_read] = (val & mask_value[i]) >> shift_value[i];
		(*bytes_read)++;
	}
}

/* Initiate a read cmd by writing to cmd register.
 * Commands are written according to cmd parameters
 * cmd->opcode   : SPMI opcode for the command
 * cmd->priority : Priority of the command
 *                 High priority : 1
 *                 Low Priority : 0
 * cmd->address  : SPMI Peripheral Address.
 * cmd->offset   : Offset Address for the command.
 * cmd->bytecnt  : Number of bytes to be read.
 *
 * param is the buffer to the save command data.
 * param->buffer : Buffer to store the bytes returned.
 * param->size   : Size of the buffer.
 *
 * return value : 0 if success, the error bit set on error
 */
unsigned int pmic_arb_read_cmd(struct pmic_arb_cmd *cmd,
                               struct pmic_arb_param *param)
{
	uint32_t val = 0;
	uint32_t error;
	uint32_t addr;
	uint8_t bytes_read = 0;

	/* Disable IRQ mode for the current channel*/
	writel(0x0, PMIC_ARB_CHNLn_CONFIG(pmic_arb_chnl_num));

	/* Fill in the byte count for the command
	 * Note: Byte count is one less than the number of bytes transferred.
	 */
	cmd->byte_cnt = param->size - 1;
	/* Fill in the Write cmd opcode. */
	cmd->opcode = SPMI_CMD_EXT_REG_READ_LONG;

	val |= ((uint32_t)(cmd->opcode) << PMIC_ARB_CMD_OPCODE_SHIFT);
	val |= ((uint32_t)(cmd->priority) << PMIC_ARB_CMD_PRIORITY_SHIFT);
	val |= ((uint32_t)(cmd->slave_id) << PMIC_ARB_CMD_SLAVE_ID_SHIFT);
	val |= ((uint32_t)(cmd->address) << PMIC_ARB_CMD_ADDR_SHIFT);
	val |= ((uint32_t)(cmd->offset) << PMIC_ARB_CMD_ADDR_OFFSET_SHIFT);
	val |= ((uint32_t)(cmd->byte_cnt));

	writel(val, PMIC_ARB_CHNLn_CMD0(pmic_arb_chnl_num));

	/* Wait till CMD DONE status */
	while (!(val = readl(PMIC_ARB_CHNLn_STATUS(pmic_arb_chnl_num))));

	/* Check for errors */
	error = val ^ (1 << PMIC_ARB_CMD_DONE);

	if (error)
	{
		dprintf(CRITICAL, "SPMI read command failure: \
			cmd_id = %u, error = %u\n", cmd->opcode, error);
		return error;
	}

	/* Read the RDATA0 */
	read_rdata_into_array(param->buffer, 0, param->size , &bytes_read);

	if (bytes_read < param->size)
	{
		/* Read the RDATA1 */
		read_rdata_into_array(param->buffer, 1, param->size , &bytes_read);

	}

	if (bytes_read < param->size)
	{
		/* Read the RDATA2 */
		read_rdata_into_array(param->buffer, 2, param->size , &bytes_read);

	}

	return 0;
}


/* Funtion to determine if the peripheral that caused the interrupt
 * is of interest.
 * Also handles callback function and interrupt clearing if the
 * correct interrupt is fired.
 * periph_acc_irq: SPMI_PIC_OWNERm_ACC_STATUSn register id.
 * status: Bits of the periph_acc_irq.
 * return 1 if the peripheral is of interest,
 * 0 otherwise.
 */
int spmi_acc_irq(uint32_t periph_acc_irq, uint32_t status)
{
	uint8_t reg_id;
	uint8_t offset;

	/* Narrow down the correct register for the peripheral*/
	reg_id = pmic_irq_perph_id / 32;
	if (periph_acc_irq * 8 != reg_id)
		return 0;

	/* Narrow down the correct interrupt within the register */
	offset = pmic_irq_perph_id & 31;
	if ((status & offset))
	{
		/* Clear the interrupt */
		writel(offset ^ status, SPMI_PIC_IRQ_CLEARn(reg_id));

		/* Confirm that the interrupt has been cleared */
		while(readl(SPMI_PIC_IRQ_STATUSn(reg_id)) & offset);

		/* Call the callback */
		callback();
		return 1;
	}
	else
		return 0;
}

void spmi_irq()
{
	int i;
	uint32_t status;

	/* Go through the Peripheral list to figure out the periperal
	 * that caused the interrupt
	 */
	for (i = 0; i < 8; i++)
	{
		status = readl(SPMI_PIC_OWNERm_ACC_STATUSn(pmic_arb_owner_id, i));
		if (status)
			if (!spmi_acc_irq(i, status))
				/* Not the correct interrupt, continue to wait */
				return;
	}
	mask_interrupt(EE0_KRAIT_HLOS_SPMI_PERIPH_IRQ);
}

/* Enable interrupts on a particular peripheral: periph_id */
void spmi_enable_periph_interrupts(uint8_t periph_id)
{
	pmic_irq_perph_id = periph_id;

	register_int_handler(EE0_KRAIT_HLOS_SPMI_PERIPH_IRQ , spmi_irq, 0);
	unmask_interrupt(EE0_KRAIT_HLOS_SPMI_PERIPH_IRQ);

}

void spmi_uninit()
{
	mask_interrupt(EE0_KRAIT_HLOS_SPMI_PERIPH_IRQ);
}
