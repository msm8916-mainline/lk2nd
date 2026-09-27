/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
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

#ifndef __PLATFORM_MSM_SHARED_NAND_H
#define __PLATFORM_MSM_SHARED_NAND_H

#ifdef PLATFORM_MSM7X30
#define MSM_NAND_BASE 0xA0200000
#elif PLATFORM_MDM9X15
#define MSM_NAND_BASE 0x1B400000
#else
#define MSM_NAND_BASE 0xA0A00000
#endif

#define MSM_NAND_NC01_BASE 	0xA0240000
#define MSM_NAND_NC10_BASE 	0xA0280000
#define MSM_NAND_NC11_BASE 	0xA02C0000
#define EBI2_REG_BASE 		0xA0000000

#define NC01(off) (MSM_NAND_NC01_BASE + (off))
#define NC10(off) (MSM_NAND_NC10_BASE + (off))
#define NC11(off) (MSM_NAND_NC11_BASE + (off))
#define EBI2_REG(off) (EBI2_REG_BASE + (off))

#define NAND_REG(off) (MSM_NAND_BASE + (off))

#define NAND_FLASH_CMD            NAND_REG(0x0000)
#define NAND_ADDR0                NAND_REG(0x0004)
#define NAND_ADDR1                NAND_REG(0x0008)
#define NAND_FLASH_CHIP_SELECT    NAND_REG(0x000C)
#define NAND_EXEC_CMD             NAND_REG(0x0010)
#define NAND_FLASH_STATUS         NAND_REG(0x0014)
#define NAND_BUFFER_STATUS        NAND_REG(0x0018)
#define NAND_DEV0_CFG0            NAND_REG(0x0020)
#define NAND_DEV0_CFG1            NAND_REG(0x0024)
#define NAND_DEV1_CFG0            NAND_REG(0x0030)
#define NAND_DEV1_CFG1            NAND_REG(0x0034)
#define NAND_SFLASHC_CMD          NAND_REG(0x0038)
#define NAND_SFLASHC_EXEC_CMD     NAND_REG(0x003C)
#define NAND_READ_ID              NAND_REG(0x0040)
#define NAND_READ_STATUS          NAND_REG(0x0044)
#define NAND_CONFIG_DATA          NAND_REG(0x0050)
#define NAND_CONFIG               NAND_REG(0x0054)
#define NAND_CONFIG_MODE          NAND_REG(0x0058)
#define NAND_CONFIG_STATUS        NAND_REG(0x0060)
#define NAND_MACRO1_REG           NAND_REG(0x0064)
#define NAND_XFR_STEP1            NAND_REG(0x0070)
#define NAND_XFR_STEP2            NAND_REG(0x0074)
#define NAND_XFR_STEP3            NAND_REG(0x0078)
#define NAND_XFR_STEP4            NAND_REG(0x007C)
#define NAND_XFR_STEP5            NAND_REG(0x0080)
#define NAND_XFR_STEP6            NAND_REG(0x0084)
#define NAND_XFR_STEP7            NAND_REG(0x0088)
#define NAND_GENP_REG0            NAND_REG(0x0090)
#define NAND_GENP_REG1            NAND_REG(0x0094)
#define NAND_GENP_REG2            NAND_REG(0x0098)
#define NAND_GENP_REG3            NAND_REG(0x009C)
#define NAND_SFLASHC_STATUS       NAND_REG(0x001C)
#define NAND_DEV_CMD0             NAND_REG(0x00A0)
#define NAND_DEV_CMD1             NAND_REG(0x00A4)
#define NAND_DEV_CMD2             NAND_REG(0x00A8)
#define NAND_DEV_CMD_VLD          NAND_REG(0x00AC)
#define NAND_EBI2_MISR_SIG_REG    NAND_REG(0x00B0)
#define NAND_ADDR2                NAND_REG(0x00C0)
#define NAND_ADDR3                NAND_REG(0x00C4)
#define NAND_ADDR4                NAND_REG(0x00C8)
#define NAND_ADDR5                NAND_REG(0x00CC)
#define NAND_DEV_CMD3             NAND_REG(0x00D0)
#define NAND_DEV_CMD4             NAND_REG(0x00D4)
#define NAND_DEV_CMD5             NAND_REG(0x00D8)
#define NAND_DEV_CMD6             NAND_REG(0x00DC)
#define NAND_SFLASHC_BURST_CFG    NAND_REG(0x00E0)
#define NAND_ADDR6                NAND_REG(0x00E4)
#define NAND_EBI2_ECC_BUF_CFG     NAND_REG(0x00F0)
#define NAND_FLASH_BUFFER         NAND_REG(0x0100)
#define NAND_HW_INFO              NAND_REG(0x00FC)

/* device commands */

#define NAND_CMD_SOFT_RESET         0x01
#define NAND_CMD_PAGE_READ          0x32
#define NAND_CMD_PAGE_READ_ECC      0x33
#define NAND_CMD_PAGE_READ_ALL      0x34
#define NAND_CMD_SEQ_PAGE_READ      0x15
#define NAND_CMD_PRG_PAGE           0x36
#define NAND_CMD_PRG_PAGE_ECC       0x37
#define NAND_CMD_PRG_PAGE_ALL       0x39
#define NAND_CMD_BLOCK_ERASE        0x3A
#define NAND_CMD_FETCH_ID           0x0B
#define NAND_CMD_STATUS             0x0C
#define NAND_CMD_RESET              0x0D

/* Sflash Commands */

#define NAND_SFCMD_DATXS          0x0
#define NAND_SFCMD_CMDXS          0x1
#define NAND_SFCMD_BURST          0x0
#define NAND_SFCMD_ASYNC          0x1
#define NAND_SFCMD_ABORT          0x1
#define NAND_SFCMD_REGRD          0x2
#define NAND_SFCMD_REGWR          0x3
#define NAND_SFCMD_INTLO          0x4
#define NAND_SFCMD_INTHI          0x5
#define NAND_SFCMD_DATRD          0x6
#define NAND_SFCMD_DATWR          0x7

#define SFLASH_PREPCMD(numxfr, offval, delval, trnstp, mode, opcode) \
   ((numxfr<<20)|(offval<<12)|(delval<<6)|(trnstp<<5)|(mode<<4)|opcode)

#define SFLASH_BCFG               0x20100327

#define CLEAN_DATA_32             0xFFFFFFFF
#define CLEAN_DATA_16             0xFFFF

/* Onenand addresses */

#define ONENAND_MANUFACTURER_ID     0xF000
#define ONENAND_DEVICE_ID           0xF001
#define ONENAND_VERSION_ID          0xF002
#define ONENAND_DATA_BUFFER_SIZE    0xF003
#define ONENAND_BOOT_BUFFER_SIZE    0xF004
#define ONENAND_AMOUNT_OF_BUFFERS   0xF005
#define ONENAND_TECHNOLOGY          0xF006
#define ONENAND_START_ADDRESS_1     0xF100
#define ONENAND_START_ADDRESS_2     0xF101
#define ONENAND_START_ADDRESS_3     0xF102
#define ONENAND_START_ADDRESS_4     0xF103
#define ONENAND_START_ADDRESS_5     0xF104
#define ONENAND_START_ADDRESS_6     0xF105
#define ONENAND_START_ADDRESS_7     0xF106
#define ONENAND_START_ADDRESS_8     0xF107
#define ONENAND_START_BUFFER        0xF200
#define ONENAND_COMMAND             0xF220
#define ONENAND_SYSTEM_CONFIG_1     0xF221
#define ONENAND_SYSTEM_CONFIG_2     0xF222
#define ONENAND_CONTROLLER_STATUS   0xF240
#define ONENAND_INTERRUPT_STATUS    0xF241
#define ONENAND_START_BLOCK_ADDRESS 0xF24C
#define ONENAND_WRITE_PROT_STATUS   0xF24E
#define ONENAND_ECC_STATUS          0xFF00
#define ONENAND_ECC_ERRPOS_MAIN0    0xFF01
#define ONENAND_ECC_ERRPOS_SPARE0   0xFF02
#define ONENAND_ECC_ERRPOS_MAIN1    0xFF03
#define ONENAND_ECC_ERRPOS_SPARE1   0xFF04
#define ONENAND_ECC_ERRPOS_MAIN2    0xFF05
#define ONENAND_ECC_ERRPOS_SPARE2   0xFF06
#define ONENAND_ECC_ERRPOS_MAIN3    0xFF07
#define ONENAND_ECC_ERRPOS_SPARE3   0xFF08

/* Onenand commands */

#define ONENAND_CMDLOAD          0x0000
#define ONENAND_CMDLOADSPARE     0x0013
#define ONENAND_CMDPROG          0x0080
#define ONENAND_CMDPROGSPARE     0x001A
#define ONENAND_CMDERAS          0x0094

#define ONENAND_SYSCFG1_ECCENA   0x40E0
#define ONENAND_SYSCFG1_ECCDIS   0x41E0
#define ONENAND_CLRINTR          0x0000
#define ONENAND_STARTADDR1_RES   0x07FF
#define ONENAND_STARTADDR3_RES   0x07FF

#define DEVICE_FLASHCORE_0       0
#define DEVICE_BUFFERRAM_0       0
#define DATARAM0_0               0x8

/* Flash type */
#define FLASH_UNKNOWN_DEVICE        0x00
#define FLASH_NAND_DEVICE           0x01
#define FLASH_8BIT_NAND_DEVICE      0x01
#define FLASH_16BIT_NAND_DEVICE     0x02
#define FLASH_ONENAND_DEVICE        0x03

#define EBI2_CFG_REG		       	EBI2_REG(0x0004)
#define EBI2_NAND_ADM_MUX       	EBI2_REG(0x005C)
#define EBI2_CHIP_SELECT_CFG0		EBI2_REG(0x0000)

#endif				/* __PLATFORM_MSM_SHARED_NAND_H */
