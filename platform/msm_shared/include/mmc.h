/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#ifndef __MMC_H__
#define __MMC_H__

#ifndef MMC_SLOT
#define MMC_SLOT            0
#endif

extern unsigned int mmc_boot_mci_base;

#define MMC_BOOT_MCI_REG(offset)          ((mmc_boot_mci_base) + offset)

/*
 * Define Macros for SDCC Registers
 */
#define MMC_BOOT_MCI_POWER                MMC_BOOT_MCI_REG(0x000)	/* 8 bit */

/* MCICMD output control - 6th bit */
#ifdef PLATFORM_MSM7X30
#define MMC_BOOT_MCI_OPEN_DRAIN           (1 << 6)
#define MMC_BOOT_MCI_PWR_OFF              0x00
#define MMC_BOOT_MCI_PWR_UP               0x01
#define MMC_BOOT_MCI_PWR_ON               0x01
#else
#define MMC_BOOT_MCI_OPEN_DRAIN           (1 << 6)
#define MMC_BOOT_MCI_PWR_OFF              0x00
#define MMC_BOOT_MCI_PWR_UP               0x02
#define MMC_BOOT_MCI_PWR_ON               0x03
#endif

#define MMC_BOOT_MCI_CLK                  MMC_BOOT_MCI_REG(0x004)	/* 16 bits */
/* Enable MCI bus clock - 0: clock disabled 1: enabled */
#define MMC_BOOT_MCI_CLK_ENABLE           (1 << 8)
/* Disable clk o/p when bus idle- 0:always enabled 1:enabled when bus active */
#define MMC_BOOT_MCI_CLK_PWRSAVE          (1 << 9)
/* Enable Widebus mode - 00: 1 bit mode 10:4 bit mode 01/11: 8 bit mode */
#define MMC_BOOT_MCI_CLK_WIDEBUS_MODE     (3 << 10)
#define MMC_BOOT_MCI_CLK_WIDEBUS_1_BIT    0
#define MMC_BOOT_MCI_CLK_WIDEBUS_4_BIT    (2 << 10)
#define MMC_BOOT_MCI_CLK_WIDEBUS_8_BIT    (1 << 10)
/* Enable flow control- 0: disable 1: enable */
#define MMC_BOOT_MCI_CLK_ENA_FLOW         (1 << 12)
/* Set/clear to select rising/falling edge for data/cmd output */
#define MMC_BOOT_MCI_CLK_INVERT_OUT       (1 << 13)
/* Select to lach data/cmd coming in falling/rising/feedbk/loopbk of MCIclk */
#define MMC_BOOT_MCI_CLK_IN_FALLING       0x0
#define MMC_BOOT_MCI_CLK_IN_RISING        (1 << 14)
#define MMC_BOOT_MCI_CLK_IN_FEEDBACK      (2 << 14)
#define MMC_BOOT_MCI_CLK_IN_LOOPBACK      (3 << 14)

/* Bus Width */
#define MMC_BOOT_BUS_WIDTH_1_BIT          0
#define MMC_BOOT_BUS_WIDTH_4_BIT          2
#define MMC_BOOT_BUS_WIDTH_8_BIT          3

#define MMC_BOOT_MCI_ARGUMENT             MMC_BOOT_MCI_REG(0x008)	/* 32 bits */

#define MMC_BOOT_MCI_CMD                  MMC_BOOT_MCI_REG(0x00C)	/* 16 bits */
/* Command Index: 0 -5 */
/* Waits for response if set */
#define MMC_BOOT_MCI_CMD_RESPONSE         (1 << 6)
/* Receives a 136-bit long response if set */
#define MMC_BOOT_MCI_CMD_LONGRSP          (1 << 7)
/* If set, CPSM disables command timer and waits for interrupt */
#define MMC_BOOT_MCI_CMD_INTERRUPT        (1 << 8)
/* If set waits for CmdPend before starting to send a command */
#define MMC_BOOT_MCI_CMD_PENDING          (1 << 9)
/* CPSM is enabled if set */
#define MMC_BOOT_MCI_CMD_ENABLE           (1 << 10)
/* If set PROG_DONE status bit asserted when busy is de-asserted */
#define MMC_BOOT_MCI_CMD_PROG_ENA         (1 << 11)
/* To indicate that this is a Command with Data (for SDIO interrupts) */
#define MMC_BOOT_MCI_CMD_DAT_CMD          (1 << 12)
/* Signals the next command to be an abort (stop) command. Always read 0 */
#define MMC_BOOT_MCI_CMD_MCIABORT         (1 << 13)
/* Waits for Command Completion Signal if set */
#define MMC_BOOT_MCI_CMD_CCS_ENABLE       (1 << 14)
/* If set sends CCS disable sequence */
#define MMC_BOOT_MCI_CMD_CCS_DISABLE      (1 << 15)

#define MMC_BOOT_MCI_RESP_CMD             MMC_BOOT_MCI_REG(0x010)

#define MMC_BOOT_MCI_RESP_0               MMC_BOOT_MCI_REG(0x014)
#define MMC_BOOT_MCI_RESP_1               MMC_BOOT_MCI_REG(0x018)
#define MMC_BOOT_MCI_RESP_2               MMC_BOOT_MCI_REG(0x01C)
#define MMC_BOOT_MCI_RESP_3               MMC_BOOT_MCI_REG(0x020)

#define MMC_BOOT_MCI_DATA_TIMER           MMC_BOOT_MCI_REG(0x024)
#define MMC_BOOT_MCI_DATA_LENGTH          MMC_BOOT_MCI_REG(0x028)
#define MMC_BOOT_MCI_DATA_CTL             MMC_BOOT_MCI_REG(0x02C)	/* 16 bits */
/* Data transfer enabled */
#define MMC_BOOT_MCI_DATA_ENABLE          (1 << 0)
/* Data transfer direction - 0: controller to card 1:card to controller */
#define MMC_BOOT_MCI_DATA_DIR             (1 << 1)
/* Data transfer mode - 0: block data transfer 1: stream data transfer */
#define MMC_BOOT_MCI_DATA_MODE            (1 << 2)
/* Enable DM interface - 0: DM disabled 1: DM enabled */
#define MMC_BOOT_MCI_DATA_DM_ENABLE       (1 << 3)
/* Data block length in bytes (1-4096) */
#define MMC_BOOT_MCI_BLKSIZE_POS          4
#define MMC_BOOT_MCI_DATA_COUNT           MMC_BOOT_MCI_REG(0x030)
#define MMC_BOOT_MCI_STATUS               MMC_BOOT_MCI_REG(0x034)
/* Command response received - CRC check failed */
#define MMC_BOOT_MCI_STAT_CMD_CRC_FAIL    (1 << 0)
/* Data block sent/received - CRC check failed */
#define MMC_BOOT_MCI_STAT_DATA_CRC_FAIL   (1 << 1)
/* Command resonse timeout */
#define MMC_BOOT_MCI_STAT_CMD_TIMEOUT     (1 << 2)
/* Data timeout */
#define MMC_BOOT_MCI_STAT_DATA_TIMEOUT    (1 << 3)
/* Transmit FIFO underrun error */
#define MMC_BOOT_MCI_STAT_TX_UNDRUN       (1 << 4)
/* Receive FIFO overrun error */
#define MMC_BOOT_MCI_STAT_RX_OVRRUN       (1 << 5)
/* Command response received - CRC check passed */
#define MMC_BOOT_MCI_STAT_CMD_RESP_END    (1 << 6)
/* Command sent - no response required */
#define MMC_BOOT_MCI_STAT_CMD_SENT        (1 << 7)
/* Data end - data counter zero */
#define MMC_BOOT_MCI_STAT_DATA_END        (1 << 8)
/* Start bit not detected on all data signals in wide bus mode */
#define MMC_BOOT_MCI_STAT_START_BIT_ERR   (1 << 9)
/* Data block sent/received - CRC check passed */
#define MMC_BOOT_MCI_STAT_DATA_BLK_END    (1 << 10)
/* Command transfer in progress */
#define MMC_BOOT_MCI_STAT_CMD_ACTIVE      (1 << 11)
/* Data transmit in progress */
#define MMC_BOOT_MCI_STAT_TX_ACTIVE       (1 << 12)
/* Data receive in progress */
#define MMC_BOOT_MCI_STAT_RX_ACTIVE       (1 << 13)
/* Transmit FIFO half full */
#define MMC_BOOT_MCI_STAT_TX_FIFO_HFULL   (1 << 14)
/* Receive FIFO half full */
#define MMC_BOOT_MCI_STAT_RX_FIFO_HFULL   (1 << 15)
/* Transmit FIFO full */
#define MMC_BOOT_MCI_STAT_TX_FIFO_FULL    (1 << 16)
/* Receive FIFO full */
#define MMC_BOOT_MCI_STAT_RX_FIFO_FULL    (1 << 17)
/* Transmit FIFO empty */
#define MMC_BOOT_MCI_STAT_TX_FIFO_EMPTY   (1 << 18)
/* Receive FIFO empty */
#define MMC_BOOT_MCI_STAT_RX_FIFO_EMPTY   (1 << 19)
/* Data available in transmit FIFO */
#define MMC_BOOT_MCI_STAT_TX_DATA_AVLBL   (1 << 20)
/* Data available in receive FIFO */
#define MMC_BOOT_MCI_STAT_RX_DATA_AVLBL   (1 << 21)
/* SDIO interrupt indicator for wake-up */
#define MMC_BOOT_MCI_STAT_SDIO_INTR       (1 << 22)
/* Programming done */
#define MMC_BOOT_MCI_STAT_PROG_DONE       (1 << 23)
/* CE-ATA command completion signal detected */
#define MMC_BOOT_MCI_STAT_ATA_CMD_CMPL    (1 << 24)
/* SDIO interrupt indicator for normal operation */
#define MMC_BOOT_MCI_STAT_SDIO_INTR_OP    (1 << 25)
/* Commpand completion signal timeout */
#define MMC_BOOT_MCI_STAT_CCS_TIMEOUT     (1 << 26)

#define MMC_BOOT_MCI_STATIC_STATUS        (MMC_BOOT_MCI_STAT_CMD_CRC_FAIL| \
        MMC_BOOT_MCI_STAT_DATA_CRC_FAIL| \
        MMC_BOOT_MCI_STAT_CMD_TIMEOUT| \
        MMC_BOOT_MCI_STAT_DATA_TIMEOUT| \
        MMC_BOOT_MCI_STAT_TX_UNDRUN| \
        MMC_BOOT_MCI_STAT_RX_OVRRUN| \
        MMC_BOOT_MCI_STAT_CMD_RESP_END| \
        MMC_BOOT_MCI_STAT_CMD_SENT| \
        MMC_BOOT_MCI_STAT_DATA_END| \
        MMC_BOOT_MCI_STAT_START_BIT_ERR| \
        MMC_BOOT_MCI_STAT_DATA_BLK_END| \
        MMC_BOOT_MCI_SDIO_INTR_CLR| \
        MMC_BOOT_MCI_STAT_PROG_DONE| \
        MMC_BOOT_MCI_STAT_ATA_CMD_CMPL |\
        MMC_BOOT_MCI_STAT_CCS_TIMEOUT)

#define MMC_BOOT_MCI_CLEAR                MMC_BOOT_MCI_REG(0x038)
#define MMC_BOOT_MCI_CMD_CRC_FAIL_CLR     (1 << 0)
#define MMC_BOOT_MCI_DATA_CRC_FAIL_CLR    (1 << 1)
#define MMC_BOOT_MCI_CMD_TIMEOUT_CLR      (1 << 2)
#define MMC_BOOT_MCI_DATA_TIMEOUT_CLR     (1 << 3)
#define MMC_BOOT_MCI_TX_UNDERRUN_CLR      (1 << 4)
#define MMC_BOOT_MCI_RX_OVERRUN_CLR       (1 << 5)
#define MMC_BOOT_MCI_CMD_RESP_END_CLR     (1 << 6)
#define MMC_BOOT_MCI_CMD_SENT_CLR         (1 << 7)
#define MMC_BOOT_MCI_DATA_END_CLR         (1 << 8)
#define MMC_BOOT_MCI_START_BIT_ERR_CLR    (1 << 9)
#define MMC_BOOT_MCI_DATA_BLK_END_CLR     (1 << 10)
#define MMC_BOOT_MCI_SDIO_INTR_CLR        (1 << 22)
#define MMC_BOOT_MCI_PROG_DONE_CLR        (1 << 23)
#define MMC_BOOT_MCI_ATA_CMD_COMPLR_CLR   (1 << 24)
#define MMC_BOOT_MCI_CCS_TIMEOUT_CLR      (1 << 25)

#define MMC_BOOT_MCI_INT_MASK0            MMC_BOOT_MCI_REG(0x03C)
#define MMC_BOOT_MCI_CMD_CRC_FAIL_MASK    (1 << 0)
#define MMC_BOOT_MCI_DATA_CRC_FAIL_MASK   (1 << 1)
#define MMC_BOOT_MCI_CMD_TIMEOUT_MASK     (1 << 2)
#define MMC_BOOT_MCI_DATA_TIMEOUT_MASK    (1 << 3)
#define MMC_BOOT_MCI_TX_OVERRUN_MASK      (1 << 4)
#define MMC_BOOT_MCI_RX_OVERRUN_MASK      (1 << 5)
#define MMC_BOOT_MCI_CMD_RESP_END_MASK    (1 << 6)
#define MMC_BOOT_MCI_CMD_SENT_MASK        (1 << 7)
#define MMC_BOOT_MCI_DATA_END_MASK        (1 << 8)
#define MMC_BOOT_MCI_START_BIT_ERR_MASK   (1 << 9)
#define MMC_BOOT_MCI_DATA_BLK_END_MASK    (1 << 10)
#define MMC_BOOT_MCI_CMD_ACTIVE_MASK      (1 << 11)
#define MMC_BOOT_MCI_TX_ACTIVE_MASK       (1 << 12)
#define MMC_BOOT_MCI_RX_ACTIVE_MASK       (1 << 13)
#define MMC_BOOT_MCI_TX_FIFO_HFULL_MASK   (1 << 14)
#define MMC_BOOT_MCI_RX_FIFO_HFULL_MASK   (1 << 15)
#define MMC_BOOT_MCI_TX_FIFO_FULL_MASK    (1 << 16)
#define MMC_BOOT_MCI_RX_FIFO_FULL_MASK    (1 << 17)
#define MMC_BOOT_MCI_TX_FIFO_EMPTY_MASK   (1 << 18)
#define MMC_BOOT_MCI_RX_FIFO_EMPTY_MASK   (1 << 19)
#define MMC_BOOT_MCI_TX_DATA_AVLBL_MASK   (1 << 20)
#define MMC_BOOT_MCI_RX_DATA_AVLBL_MASK   (1 << 21)
#define MMC_BOOT_MCI_SDIO_INT_MASK        (1 << 22)
#define MMC_BOOT_MCI_PROG_DONE_MASK       (1 << 23)
#define MMC_BOOT_MCI_ATA_CMD_COMPL_MASK   (1 << 24)
#define MMC_BOOT_MCI_SDIO_INT_OPER_MASK   (1 << 25)
#define MMC_BOOT_MCI_CCS_TIME_OUT_MASK    (1 << 26)

#define MMC_BOOT_MCI_INT_MASK1            MMC_BOOT_MCI_REG(0x040)

#define MMC_BOOT_MCI_FIFO_COUNT           MMC_BOOT_MCI_REG(0x044)

#define MMC_BOOT_MCI_CCS_TIMER            MMC_BOOT_MCI_REG(0x0058)

#define MMC_BOOT_MCI_FIFO                 MMC_BOOT_MCI_REG(0x080)

/* Card status */
#define MMC_BOOT_CARD_STATUS(x)          ((x>>9) & 0x0F)
#define MMC_BOOT_TRAN_STATE              4
#define MMC_BOOT_PROG_STATE              7

/* SD Memory Card bus commands */
#define CMD0_GO_IDLE_STATE               0
#define CMD1_SEND_OP_COND                1
#define CMD2_ALL_SEND_CID                2
#define CMD3_SEND_RELATIVE_ADDR          3
#define CMD4_SET_DSR                     4
#define CMD6_SWITCH_FUNC                 6
#define ACMD6_SET_BUS_WIDTH              6	/* SD card */
#define CMD7_SELECT_DESELECT_CARD        7
#define CMD8_SEND_EXT_CSD                8
#define CMD8_SEND_IF_COND                8	/* SD card */
#define CMD9_SEND_CSD                    9
#define CMD10_SEND_CID                   10
#define CMD12_STOP_TRANSMISSION          12
#define CMD13_SEND_STATUS                13
#define CMD15_GO_INACTIVE_STATUS         15
#define CMD16_SET_BLOCKLEN               16
#define CMD17_READ_SINGLE_BLOCK          17
#define CMD18_READ_MULTIPLE_BLOCK        18
#define CMD23_SET_BLOCK_COUNT            23
#define CMD24_WRITE_SINGLE_BLOCK         24
#define CMD25_WRITE_MULTIPLE_BLOCK       25
#define CMD28_SET_WRITE_PROTECT          28
#define CMD29_CLEAR_WRITE_PROTECT        29
#define CMD31_SEND_WRITE_PROT_TYPE       31
#define CMD32_ERASE_WR_BLK_START         32
#define CMD33_ERASE_WR_BLK_END           33
#define CMD35_ERASE_GROUP_START          35
#define CMD36_ERASE_GROUP_END            36
#define CMD38_ERASE                      38
#define ACMD41_SEND_OP_COND              41	/* SD card */
#define ACMD51_SEND_SCR                  51	/* SD card */
#define CMD55_APP_CMD                    55	/* SD card */

/* Switch Function Modes */
#define MMC_BOOT_SWITCH_FUNC_CHECK        0
#define MMC_BOOT_SWITCH_FUNC_SET          1

/* OCR Register */
#define MMC_BOOT_OCR_17_19                (1 << 7)
#define MMC_BOOT_OCR_27_36                (0x1FF << 15)
#define MMC_BOOT_OCR_SEC_MODE             (2 << 29)
#define MMC_BOOT_OCR_BUSY                 (1 << 31)

/* Commands type */
#define MMC_BOOT_CMD_BCAST                (1 << 0)
#define MMC_BOOT_CMD_BCAST_W_RESP         (1 << 1)
#define MMC_BOOT_CMD_ADDRESS              (1 << 2)
#define MMC_BOOT_CMD_ADDR_DATA_XFER       (1 << 3)

/* Response types */
#define MMC_BOOT_RESP_NONE                0
#define MMC_BOOT_RESP_R1                  (1 << 0)
#define MMC_BOOT_RESP_R1B                 (1 << 1)
#define MMC_BOOT_RESP_R2                  (1 << 2)
#define MMC_BOOT_RESP_R3                  (1 << 3)
#define MMC_BOOT_RESP_R6                  (1 << 6)
#define MMC_BOOT_RESP_R7                  (1 << 7)

#define IS_RESP_136_BITS(x)              (x & MMC_BOOT_RESP_R2)
#define CHECK_FOR_BUSY_AT_RESP(x)

/* Card Status bits (R1 register) */
#define MMC_BOOT_R1_AKE_SEQ_ERROR         (1 << 3)
#define MMC_BOOT_R1_APP_CMD               (1 << 5)
#define MMC_BOOT_R1_RDY_FOR_DATA          (1 << 6)
#define MMC_BOOT_R1_CURR_STATE_IDLE       (0 << 9)
#define MMC_BOOT_R1_CURR_STATE_RDY        (1 << 9)
#define MMC_BOOT_R1_CURR_STATE_IDENT      (2 << 9)
#define MMC_BOOT_R1_CURR_STATE_STBY       (3 << 9)
#define MMC_BOOT_R1_CURR_STATE_TRAN       (4 << 9)
#define MMC_BOOT_R1_CURR_STATE_DATA       (5 << 9)
#define MMC_BOOT_R1_CURR_STATE_RCV        (6 << 9)
#define MMC_BOOT_R1_CURR_STATE_PRG        (7 << 9)
#define MMC_BOOT_R1_CURR_STATE_DIS        (8 << 9)
#define MMC_BOOT_R1_ERASE_RESET           (1 << 13)
#define MMC_BOOT_R1_CARD_ECC_DISABLED     (1 << 14)
#define MMC_BOOT_R1_WP_ERASE_SKIP         (1 << 15)
#define MMC_BOOT_R1_ERROR                 (1 << 19)
#define MMC_BOOT_R1_CC_ERROR              (1 << 20)
#define MMC_BOOT_R1_CARD_ECC_FAILED       (1 << 21)
#define MMC_BOOT_R1_ILLEGAL_CMD           (1 << 22)
#define MMC_BOOT_R1_COM_CRC_ERR           (1 << 23)
#define MMC_BOOT_R1_LOCK_UNLOCK_FAIL      (1 << 24)
#define MMC_BOOT_R1_CARD_IS_LOCKED        (1 << 25)
#define MMC_BOOT_R1_WP_VIOLATION          (1 << 26)
#define MMC_BOOT_R1_ERASE_PARAM           (1 << 27)
#define MMC_BOOT_R1_ERASE_SEQ_ERR         (1 << 28)
#define MMC_BOOT_R1_BLOCK_LEN_ERR         (1 << 29)
#define MMC_BOOT_R1_ADDR_ERR              (1 << 30)
#define MMC_BOOT_R1_OUT_OF_RANGE          (1 << 31)

/* Macros for Common Errors */
#define MMC_BOOT_E_SUCCESS                0
#define MMC_BOOT_E_FAILURE                1
#define MMC_BOOT_E_TIMEOUT                2
#define MMC_BOOT_E_INVAL                  3
#define MMC_BOOT_E_CRC_FAIL               4
#define MMC_BOOT_E_INIT_FAIL              5
#define MMC_BOOT_E_CMD_INDX_MISMATCH      6
#define MMC_BOOT_E_RESP_VERIFY_FAIL       7
#define MMC_BOOT_E_NOT_SUPPORTED          8
#define MMC_BOOT_E_CARD_BUSY              9
#define MMC_BOOT_E_MEM_ALLOC_FAIL         10
#define MMC_BOOT_E_CLK_ENABLE_FAIL        11
#define MMC_BOOT_E_CMMC_DECODE_FAIL       12
#define MMC_BOOT_E_CID_DECODE_FAIL        13
#define MMC_BOOT_E_BLOCKLEN_ERR           14
#define MMC_BOOT_E_ADDRESS_ERR            15
#define MMC_BOOT_E_DATA_CRC_FAIL          16
#define MMC_BOOT_E_DATA_TIMEOUT           17
#define MMC_BOOT_E_RX_OVRRUN              18
#define MMC_BOOT_E_VREG_SET_FAILED        19
#define MMC_BOOT_E_GPIO_CFG_FAIL          20
#define MMC_BOOT_E_DATA_ADM_ERR           21

/* EXT_CSD */
#define MMC_BOOT_ACCESS_WRITE             0x3

#define MMC_BOOT_EXT_USER_WP              171
#define MMC_BOOT_EXT_ERASE_GROUP_DEF      175
#define MMC_BOOT_EXT_ERASE_MEM_CONT       181
#define MMC_BOOT_EXT_CMMC_BUS_WIDTH       183
#define MMC_BOOT_EXT_CMMC_HS_TIMING       185
#define MMC_BOOT_EXT_HC_WP_GRP_SIZE       221
#define MMC_BOOT_EXT_ERASE_TIMEOUT_MULT   223
#define MMC_BOOT_EXT_HC_ERASE_GRP_SIZE    224

#define IS_BIT_SET_EXT_CSD(val, bit)      ((ext_csd_buf[val]) & (1<<(bit)))
#define IS_ADDR_OUT_OF_RANGE(resp)        ((resp >> 31) & 0x01)

#define MMC_BOOT_US_PERM_WP_EN            2
#define MMC_BOOT_US_PWR_WP_DIS            3

#define MMC_BOOT_US_PERM_WP_DIS           (1<<4)
#define MMC_BOOT_US_PWR_WP_EN             1

/* For SD */
#define MMC_BOOT_SD_HC_VOLT_SUPPLIED      0x000001AA
#define MMC_BOOT_SD_NEG_OCR               0x00FF8000
#define MMC_BOOT_SD_HC_HCS                0x40000000
#define MMC_BOOT_SD_DEV_READY             0x80000000
#define MMC_BOOT_SD_SWITCH_HS             0x80FFFFF1

/* Data structure definitions */
struct mmc_boot_command {
	unsigned int cmd_index;
	unsigned int argument;
	unsigned int cmd_type;

	unsigned int resp[4];
	unsigned int resp_type;
	unsigned int prg_enabled;
	unsigned int xfer_mode;
};

#define MMC_BOOT_XFER_MODE_BLOCK          0
#define MMC_BOOT_XFER_MODE_STREAM         1

/* CSD Register.
 * Note: not all the fields have been defined here
 */
struct mmc_boot_csd {
	unsigned int cmmc_structure;
	unsigned int spec_vers;
	unsigned int card_cmd_class;
	unsigned int write_blk_len;
	unsigned int read_blk_len;
	unsigned int r2w_factor;
	unsigned int sector_size;
	unsigned int c_size_mult;
	unsigned int c_size;
	unsigned int nsac_clk_cycle;
	unsigned int taac_ns;
	unsigned int tran_speed;
	unsigned int erase_grp_size;
	unsigned int erase_grp_mult;
	unsigned int wp_grp_size;
	unsigned int wp_grp_enable:1;
	unsigned int perm_wp:1;
	unsigned int temp_wp:1;
	unsigned int erase_blk_len:1;
	unsigned int read_blk_misalign:1;
	unsigned int write_blk_misalign:1;
	unsigned int read_blk_partial:1;
	unsigned int write_blk_partial:1;
};

/* CID Register */
struct mmc_boot_cid {
	unsigned int mid;	/* 8 bit manufacturer id */
	unsigned int oid;	/* 16 bits 2 character ASCII - OEM ID */
	unsigned char pnm[7];	/* 6 character ASCII -  product name */
	unsigned int prv;	/* 8 bits - product revision */
	unsigned int psn;	/* 32 bits - product serial number */
	unsigned int month;	/* 4 bits manufacturing month */
	unsigned int year;	/* 4 bits manufacturing year */
};

/* SCR Register */
struct mmc_boot_scr {
	unsigned int scr_structure;
	unsigned int mmc_spec;
#define MMC_BOOT_SCR_MMC_SPEC_V1_01       0
#define MMC_BOOT_SCR_MMC_SPEC_V1_10       1
#define MMC_BOOT_SCR_MMC_SPEC_V2_00       2
	unsigned int data_stat_after_erase;
	unsigned int mmc_security;
#define MMC_BOOT_SCR_NO_SECURITY         0
#define MMC_BOOT_SCR_SECURITY_UNUSED     1
#define MMC_BOOT_SCR_SECURITY_V1_01      2
#define MMC_BOOT_SCR_SECURITY_V2_00      3
	unsigned int mmc_bus_width;
#define MMC_BOOT_SCR_BUS_WIDTH_1_BIT     (1<<0)
#define MMC_BOOT_SCR_BUS_WIDTH_4_BIT     (1<<2)
};

struct mmc_boot_card {
	unsigned int rca;
	unsigned int ocr;
	unsigned long long capacity;
	unsigned int type;
#define MMC_BOOT_TYPE_STD_SD             0
#define MMC_BOOT_TYPE_SDHC               1
#define MMC_BOOT_TYPE_SDIO               2
#define MMC_BOOT_TYPE_MMCHC              3
#define MMC_BOOT_TYPE_STD_MMC            4
	unsigned int status;
#define MMC_BOOT_STATUS_INACTIVE         0
#define MMC_BOOT_STATUS_ACTIVE           1
	unsigned int rd_timeout_ns;
	unsigned int wr_timeout_ns;
	unsigned int rd_block_len;
	unsigned int wr_block_len;
	//unsigned int data_xfer_len;
	struct mmc_boot_cid cid;
	struct mmc_boot_csd csd;
	struct mmc_boot_scr scr;
};

#define MMC_BOOT_XFER_MULTI_BLOCK        0
#define MMC_BOOT_XFER_SINGLE_BLOCK       1

struct mmc_boot_host {
	unsigned int mclk_rate;
	unsigned int ocr;
	unsigned int cmd_retry;
};

/* MACRO used to evoke regcomp */
#define REGCOMP_CKRTN(regx, str, errhandle)   \
    do {                            \
        if(regcomp(regx, str, REG_EXTENDED) != 0) {    \
            printf("Error building regex: %s\n", str);  \
            goto errhandle;  \
        } \
    } while(0);

#define GET_LWORD_FROM_BYTE(x)    ((unsigned)*(x) | \
        ((unsigned)*(x+1) << 8) | \
        ((unsigned)*(x+2) << 16) | \
        ((unsigned)*(x+3) << 24))

#define PUT_LWORD_TO_BYTE(x, y)   do{*(x) = y & 0xff;     \
    *(x+1) = (y >> 8) & 0xff;     \
    *(x+2) = (y >> 16) & 0xff;     \
    *(x+3) = (y >> 24) & 0xff; }while(0)

#define GET_PAR_NUM_FROM_POS(x) (((x & 0x0000FF00) >> 8) + (x & 0x000000FF))

#define OFFSET_STATUS             0x00
#define OFFSET_TYPE               0x04
#define OFFSET_FIRST_SEC          0x08
#define OFFSET_SIZE               0x0C
#define COPYBUFF_SIZE             (1024 * 16)
#define BINARY_IN_TABLE_SIZE      (16 * 512)
#define MAX_FILE_ENTRIES          20

#define MMC_RCA 2

/* Can be used to unpack array of upto 32 bits data */
#define UNPACK_BITS(array, start, len, size_of)                               \
    ({                                                             \
     unsigned int indx = (start) / (size_of);                  \
     unsigned int offset = (start) % (size_of);                \
     unsigned int mask = (((len)<(size_of))? 1<<(len):0) - 1; \
     unsigned int unpck = array[indx] >> offset;               \
     unsigned int indx2 = ((start) + (len) - 1) / (size_of);       \
     if(indx2 > indx)                                          \
     unpck |= array[indx2] << ((size_of) - offset);          \
     unpck & mask;                                             \
     })

#define MMC_BOOT_MAX_COMMAND_RETRY    1000
#define MMC_BOOT_RD_BLOCK_LEN         512
#define MMC_BOOT_WR_BLOCK_LEN         512

/* We have 16 32-bits FIFO registers */
#define MMC_BOOT_MCI_FIFO_DEPTH       16
#define MMC_BOOT_MCI_HFIFO_COUNT      ( MMC_BOOT_MCI_FIFO_DEPTH / 2 )
#define MMC_BOOT_MCI_FIFO_SIZE        ( MMC_BOOT_MCI_FIFO_DEPTH * 4 )

#define MAX_PARTITIONS 64

#define MMC_BOOT_CHECK_PATTERN        0xAA	/* 10101010b */

#define MMC_CLK_400KHZ                400000
#define MMC_CLK_144KHZ                144000
#define MMC_CLK_20MHZ                 20000000
#define MMC_CLK_25MHZ                 25000000
#define MMC_CLK_48MHZ                 48000000
#define MMC_CLK_50MHZ                 49152000

#define MMC_CLK_ENABLE      1
#define MMC_CLK_DISABLE     0

unsigned int mmc_boot_main(unsigned char slot, unsigned int base);
unsigned int mmc_boot_read_from_card(struct mmc_boot_host *host,
				     struct mmc_boot_card *card,
				     unsigned long long data_addr,
				     unsigned int data_len, unsigned int *out);
unsigned int mmc_write(unsigned long long data_addr,
		       unsigned int data_len, unsigned int *in);

unsigned int mmc_read(unsigned long long data_addr, unsigned int *out,
		      unsigned int data_len);
unsigned mmc_get_psn(void);

unsigned int mmc_boot_write_to_card(struct mmc_boot_host *host,
				    struct mmc_boot_card *card,
				    unsigned long long data_addr,
				    unsigned int data_len, unsigned int *in);

unsigned int mmc_erase_card(unsigned long long data_addr,
			    unsigned long long data_len);

struct mmc_boot_host *get_mmc_host(void);
struct mmc_boot_card *get_mmc_card(void);
#endif
