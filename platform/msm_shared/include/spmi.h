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

#ifndef __SPMI_H
#define __SPMI_H

#define SPMI_MSM8974_MASTER_ID               0

#define SPMI_GENI_REG(x)                     (SPMI_GENI_BASE + (x))
#define SPMI_GENI_CFG_REG_BASE               SPMI_GENI_REG(0x100)
#define SPMI_GENI_CFG_RAM_BASE               SPMI_GENI_REG(0x200)

#define SPMI_GENI_CFG_REGn(x)                (SPMI_GENI_CFG_REG_BASE + 4 * (x))
#define SPMI_GENI_CFG_RAM_REGn(x)            (SPMI_GENI_CFG_RAM_BASE + 4 * (x))

#define SPMI_GENI_CLK_CTRL_REG               SPMI_GENI_REG(0x00)
#define SPMI_GENI_OUTPUT_CTRL_REG            SPMI_GENI_REG(0x10)
#define SPMI_GENI_FORCE_DEFAULT_REG          SPMI_GENI_REG(0x0C)
#define SPMI_MID_REG                         SPMI_GENI_REG(0xF00)
#define SPMI_CFG_REG                         SPMI_GENI_REG(0xF04)
#define SPMI_SEC_DISABLE_REG                 SPMI_GENI_REG(0xF08)

#define SPMI_GENI_IRQ_ENABLE                 (SPMI_GENI_BASE + 0x24)
#define SPMI_GENI_IRQ_CLEAR                  (SPMI_GENI_BASE + 0x28)
#define SPMI_GENI_TX_FIFO_BASE               (SPMI_GENI_BASE + 0x40)
#define SPMI_GENI_RX_FIFO_BASE               (SPMI_GENI_BASE + 0x80)

#define SPMI_GENI_TX_FIFOn(x)                (SPMI_GENI_TX_FIFO_BASE + 4 * (x))
#define SPMI_GENI_RX_FIFOn(x)                (SPMI_GENI_RX_FIFO_BASE + 4 * (x))

#define PMIC_ARB_CHNLn_CMD0(x)               (SPMI_BASE + 0xF800 + (x) * 0x80)
#define PMIC_ARB_CMD_OPCODE_SHIFT            27
#define PMIC_ARB_CMD_PRIORITY_SHIFT          26
#define PMIC_ARB_CMD_SLAVE_ID_SHIFT          20
#define PMIC_ARB_CMD_ADDR_SHIFT              12
#define PMIC_ARB_CMD_ADDR_OFFSET_SHIFT       4
#define PMIC_ARB_CMD_BYTE_CNT_SHIFT          0

#define PMIC_ARB_CHNLn_CONFIG(x)             (SPMI_BASE + 0xF804 + (x) * 0x80)
#define PMIC_ARB_CHNLn_STATUS(x)             (SPMI_BASE + 0xF808 + (x) * 0x80)
#define PMIC_ARB_CHNLn_WDATA(x, n)           (SPMI_BASE + 0xF810 + \
	(x) * 0x80 + (n) * 4)
#define PMIC_ARB_CHNLn_RDATA(x,n)            (SPMI_BASE + 0xF818 + \
	(x) * 0x80 + (n) * 4)

/* PIC Registers */
#define SPMI_PIC_OWNERm_ACC_STATUSn(m, n)    (SPMI_PIC_BASE + 32 * (m) + 4 * (n))
#define SPMI_PIC_ACC_ENABLEn(n)              (SPMI_PIC_BASE + 0x200 + 4 * (n))
#define SPMI_PIC_IRQ_STATUSn(n)              (SPMI_PIC_BASE + 0x600 + 0x4 * (n))
#define SPMI_PIC_IRQ_CLEARn(n)               (SPMI_PIC_BASE + 0xA00 + 0x4 * (n))

/* SPMI Commands */
#define SPMI_CMD_EXT_REG_WRTIE_LONG          0x00
#define SPMI_CMD_EXT_REG_READ_LONG           0x01
#define SPMI_CMD_EXT_REG_READ_LONG_DELAYED   0x02
#define SPMI_CMD_TRANSFER_BUS_OWNERSHIP      0x03

/* The commands below are not yet supported */
#define SPMI_CMD_RESET                       0x04
#define SPMI_CMD_SLEEP                       0x05
#define SPMI_CMD_SHUTDOWN                    0x06
#define SPMI_CMD_WAKEUP                      0x07
#define SPMI_CMD_EXT_REG_WRITE               0x08
#define SPMI_CMD_EXT_REG_READ                0x09
#define SPMI_CMD_REG_WRITE                   0x0A
#define SPMI_CMD_REG_READ                    0x0B
#define SPMI_CMD_REG_0_WRITE                 0x0C
#define SPMI_CMD_AUTH                        0x0D
#define SPMI_CMD_MASTER_WRITE                0x0E
#define SPMI_CMD_MASTER_READ                 0x0F
#define SPMI_CMD_DEV_DESC_BLK_MASTER_READ    0x10
#define SPMI_CMD_DEV_DESC_BLK_SLAVE_READ     0x11

enum spmi_geni_cmd_return_value{
	SPMI_CMD_DONE,
	SMPI_CMD_DENIED,
	SPMI_CMD_FAILURE,
	SPMI_ILLEGAL_CMD,
	SPMI_CMD_OVERRUN = 6,
	SPMI_TX_FIFO_RD_ERR,
	SPMI_TX_FIFO_WR_ERR,
	SPMI_RX_FIFO_RD_ERR,
	SPMI_RX_FIFO_WR_ERR
};

enum pmic_arb_chnl_return_values{
	PMIC_ARB_CMD_DONE,
	PMIC_ARB_CMD_FAILURE,
	PMIC_ARB_CMD_DENIED,
	PMIC_ARB_CMD_DROPPED,
};

struct pmic_arb_cmd{
	uint8_t opcode;
	uint8_t priority;
	uint8_t slave_id;
	uint8_t address;
	uint8_t offset;
	uint8_t byte_cnt;
};

struct pmic_arb_param{
	uint8_t *buffer;
	uint8_t size;
};

typedef void (*spmi_callback)();

void spmi_init(uint32_t, uint32_t);
unsigned int pmic_arb_write_cmd(struct pmic_arb_cmd *cmd,
	struct pmic_arb_param *param);
unsigned int pmic_arb_read_cmd(struct pmic_arb_cmd *cmd,
	struct pmic_arb_param *param);

#endif
