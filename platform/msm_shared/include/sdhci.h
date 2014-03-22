/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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

#ifndef __PLATFORM_SDHCI_H_
#define __PLATFORM_SDHCI_H_

#include <reg.h>
#include <bits.h>
#include <kernel/event.h>

/*
 * Capabilities for the host controller
 * These values are read from the capabilities
 * register in the controller
 */
struct host_caps {
	uint32_t base_clk_rate;  /* Max clock rate supported */
	uint32_t max_blk_len;    /* Max block len supported */
	uint8_t bus_width_8bit;  /* 8 Bit mode supported */
	uint8_t adma_support;    /* Adma support */
	uint8_t voltage;         /* Supported voltage */
	uint8_t sdr_support;     /* Single Data rate */
	uint8_t ddr_support;     /* Dual Data rate */
	uint8_t sdr50_support;   /* UHS mode, with 100 MHZ clock */
	uint8_t sdr104_support;  /* UHS mode, with 200 MHZ clock */
	uint8_t hs400_support;   /* Hs400 mode, with 400 MHZ clock */
};

/*
 * sdhci host structure, holding information about host
 * controller parameters
 */
struct sdhci_host {
	uint32_t base;           /* Base address for the host */
	uint32_t cur_clk_rate;   /* Running clock rate */
	uint32_t timing;         /* current timing for the host */
	bool tuning_in_progress; /* Tuning is being executed */
	event_t* sdhc_event;     /* Event for power control irqs */
	struct host_caps caps;   /* Host capabilities */
	struct sdhci_msm_data *msm_host; /* MSM specific host info */
};

/*
 * Data pointer to be read/written
 */
struct mmc_data {
	void *data_ptr;      /* Points to stream of data */
	uint32_t blk_sz;     /* Block size for the data */
	uint32_t num_blocks; /* num of blocks, each always of size SDHCI_MMC_BLK_SZ */
};

/*
 * mmc command structure as per the spec
 */
struct mmc_command {
	uint16_t cmd_index;     /* Command index */
	uint32_t argument;      /* Command argument */
	uint8_t data_present;   /* Command has data */
	uint8_t cmd_type;       /* command type */
	uint16_t resp_type;     /* Response type of the command */
	uint32_t resp[4];       /* 128 bit response value */
	uint32_t trans_mode;    /* Transfer mode, read/write */
	uint32_t cmd_retry;     /* Retry the command, if card is busy */
	uint32_t cmd23_support; /* If card supports cmd23 */
	uint64_t cmd_timeout;   /* Command timeout in ms */
	struct mmc_data data;   /* Data pointer */
};

/*
 * Descriptor table for adma
 */
struct desc_entry {
	uint16_t tran_att;   /* Attribute for transfer data */
	uint16_t len;        /* Length of data */
	uint32_t addr;       /* Address of the data */
};

/*
 * Command types for sdhci
 */
enum {
	SDHCI_CMD_TYPE_NORMAL = 0,
	SDHCI_CMD_TYPE_SUSPEND,
	SDHCI_CMD_TYPE_RESUME,
	SDHCI_CMD_TYPE_ABORT,
} sdhci_cmd_type;

/*
 * Response type values for sdhci
 */
enum {
	SDHCI_CMD_RESP_NONE = 0,
	SDHCI_CMD_RESP_136,
	SDHCI_CMD_RESP_48,
	SDHCI_CMD_RESP_48_BUSY,
} sdhci_resp_type;


/*
 * Helper macros for writing byte, word & long registers
 */
#define REG_READ8(host, a)                        readb(host->base + a);
#define REG_WRITE8(host, v, a)                    writeb(v, (host->base + a))

#define REG_READ32(host, a)                       readl(host->base + a)
#define REG_WRITE32(host, v, a)                   writel(v, (host->base + a))
#define REG_RMW32(host, a, s, w, v)               RMWREG32((host->base + a), s, w, v)

#define REG_READ16(host, a)                      readhw(host->base + a)
#define REG_WRITE16(host, v, a)                  writehw(v, (host->base + a))

/*
 * SDHCI registers, as per the host controller spec v 3.0
 */
#define SDHCI_ARG2_REG                            (0x000)
#define SDHCI_BLKSZ_REG                           (0x004)
#define SDHCI_BLK_CNT_REG                         (0x006)
#define SDHCI_ARGUMENT_REG                        (0x008)
#define SDHCI_TRANS_MODE_REG                      (0x00C)
#define SDHCI_CMD_REG                             (0x00E)
#define SDHCI_RESP_REG                            (0x010)
#define SDHCI_PRESENT_STATE_REG                   (0x024)
#define SDHCI_HOST_CTRL1_REG                      (0x028)
#define SDHCI_PWR_CTRL_REG                        (0x029)
#define SDHCI_CLK_CTRL_REG                        (0x02C)
#define SDHCI_TIMEOUT_REG                         (0x02E)
#define SDHCI_RESET_REG                           (0x02F)
#define SDHCI_NRML_INT_STS_REG                    (0x030)
#define SDHCI_ERR_INT_STS_REG                     (0x032)
#define SDHCI_NRML_INT_STS_EN_REG                 (0x034)
#define SDHCI_ERR_INT_STS_EN_REG                  (0x036)
#define SDHCI_NRML_INT_SIG_EN_REG                 (0x038)
#define SDHCI_ERR_INT_SIG_EN_REG                  (0x03A)
#define SDHCI_HOST_CTRL2_REG                      (0x03E)
#define SDHCI_CAPS_REG1                           (0x040)
#define SDHCI_CAPS_REG2                           (0x044)
#define SDHCI_ADM_ADDR_REG                        (0x058)

/*
 * Helper macros for register writes
 */
#define SDHCI_SOFT_RESET                          BIT(0)
#define SOFT_RESET_CMD                            BIT(1)
#define SOFT_RESET_DATA                           BIT(2)
#define SDHCI_RESET_MAX_TIMEOUT                   0x64
#define SDHCI_1_8_VOL_SET                         BIT(3)

/*
 * Interrupt related
 */
#define SDHCI_NRML_INT_STS_EN                     0x000B
#define SDHCI_ERR_INT_STS_EN                      0xFFFF
#define SDHCI_NRML_INT_SIG_EN                     0x000B
#define SDHCI_ERR_INT_SIG_EN                      0xFFFF

#define SDCC_HC_INT_CARD_REMOVE                   BIT(7)
#define SDCC_HC_INT_CARD_INSERT                   BIT(6)

/*
 * HC mode enable/disable
 */
#define SDHCI_HC_MODE_EN                          BIT(0)
#define SDHCI_HC_MODE_DIS                         (0 << 1)

/*
 * Clk control related
 */
#define SDHCI_CLK_MAX_DIV                         2046
#define SDHCI_SDCLK_FREQ_SEL                      8
#define SDHCI_SDCLK_UP_BIT_SEL                    6
#define SDHCI_SDCLK_FREQ_MASK                     0xFF
#define SDHC_SDCLK_UP_BIT_MASK                    0x300
#define SDHCI_INT_CLK_EN                          BIT(0)
#define SDHCI_CLK_STABLE_MASK                     BIT(1)
#define SDHCI_CLK_STABLE                          BIT(1)
#define SDHCI_CLK_EN                              BIT(2)
#define SDHCI_CLK_DIS                             (0 << 2)
#define SDHCI_CLK_RATE_MASK                       0x0000FF00
#define SDHCI_CLK_RATE_BIT                        8

#define SDHCI_CMD_ACT                             BIT(0)
#define SDHCI_DAT_ACT                             BIT(1)

/*
 * Bus voltage related macros
 */
#define SDHCI_BUS_VOL_SEL                         1
#define SDHCI_BUS_PWR_EN                          BIT(0)
#define SDHCI_VOL_1_8                             5
#define SDHCI_VOL_3_0                             6
#define SDHCI_VOL_3_3                             7
#define SDHCI_3_3_VOL_MASK                        0x01000000
#define SDHCI_3_0_VOL_MASK                        0x02000000
#define SDHCI_1_8_VOL_MASK                        0x04000000

/*
 * Bus width related macros
 */
#define SDHCI_8BIT_WIDTH_MASK                     0x00040000

#define SDHCI_BUS_WITDH_1BIT                      (0)
#define SDHCI_BUS_WITDH_4BIT                      BIT(1)
#define SDHCI_BUS_WITDH_8BIT                      BIT(5)

/*
 * Adma related macros
 */
#define SDHCI_BLK_LEN_MASK                        0x00030000
#define SDHCI_BLK_LEN_BIT                         16
#define SDHCI_BLK_ADMA_MASK                       0x00080000
#define SDHCI_INT_STS_TRANS_COMPLETE              BIT(1)
#define SDHCI_STATE_CMD_DAT_MASK                  0x0003
#define SDHCI_INT_STS_CMD_COMPLETE                BIT(0)
#define SDHCI_ERR_INT_STAT_MASK                   0x8000
#define SDHCI_ADMA_DESC_LINE_SZ                   65536
#define SDHCI_ADMA_MAX_TRANS_SZ                   (65535 * 512)
#define SDHCI_ADMA_TRANS_VALID                    BIT(0)
#define SDHCI_ADMA_TRANS_END                      BIT(1)
#define SDHCI_ADMA_TRANS_DATA                     BIT(5)
#define SDHCI_MMC_BLK_SZ                          512
#define SDHCI_MMC_CUR_BLK_CNT_BIT                 16
#define SDHCI_MMC_BLK_SZ_BIT                      0
#define SDHCI_TRANS_MULTI                         BIT(5)
#define SDHCI_TRANS_SINGLE                        (0 << 5)
#define SDHCI_BLK_CNT_EN                          BIT(1)
#define SDHCI_DMA_EN                              BIT(0)
#define SDHCI_AUTO_CMD23_EN                       BIT(3)
#define SDHCI_AUTO_CMD12_EN                       BIT(2)
#define SDHCI_ADMA_32BIT                          BIT(4)

/*
 * Command related macros
 */
#define SDHCI_CMD_RESP_TYPE_SEL_BIT               0
#define SDHCI_CMD_CRC_CHECK_BIT                   3
#define SDHCI_CMD_IDX_CHECK_BIT                   4
#define SDHCI_CMD_DATA_PRESENT_BIT                5
#define SDHCI_CMD_CMD_TYPE_BIT                    6
#define SDHCI_CMD_CMD_IDX_BIT                     8
#define SDHCI_CMD_TIMEOUT_MASK                    BIT(0)
#define SDHCI_CMD_CRC_MASK                        BIT(1)
#define SDHCI_CMD_END_BIT_MASK                    BIT(2)
#define SDHCI_CMD_IDX_MASK                        BIT(3)
#define SDHCI_DAT_TIMEOUT_MASK                    BIT(4)
#define SDHCI_DAT_CRC_MASK                        BIT(5)
#define SDHCI_DAT_END_BIT_MASK                    BIT(6)
#define SDHCI_CUR_LIM_MASK                        BIT(7)
#define SDHCI_AUTO_CMD12_MASK                     BIT(8)
#define SDHCI_ADMA_MASK                           BIT(9)
#define SDHCI_READ_MODE                           BIT(4)
#define SDHCI_SWITCH_CMD                          6
#define SDHCI_CMD_TIMEOUT                         0xF
#define SDHCI_MAX_CMD_RETRY                       10000
#define SDHCI_MAX_TRANS_RETRY                     10000

#define SDHCI_PREP_CMD(c, f)                      ((((c) & 0xff) << 8) | ((f) & 0xff))

/*
 * command response related
 */
#define SDHCI_RESP_LSHIFT                         8
#define SDHCI_RESP_RSHIFT                         24

/*
 * Power control relatd macros
 */
#define SDCC_HC_PWR_CTRL_INT                      0xF
#define SDCC_HC_BUS_ON                            BIT(0)
#define SDCC_HC_BUS_OFF                           BIT(1)
#define SDCC_HC_BUS_ON_OFF_SUCC                   BIT(0)
#define SDCC_HC_IO_SIG_LOW                        BIT(2)
#define SDCC_HC_IO_SIG_HIGH                       BIT(3)
#define SDCC_HC_IO_SIG_SUCC                       BIT(2)

/*
 * Command response
 */
#define SDHCI_CMD_RESP_NONE                       0
#define SDHCI_CMD_RESP_R1                         BIT(0)
#define SDHCI_CMD_RESP_R1B                        BIT(1)
#define SDHCI_CMD_RESP_R2                         BIT(2)
#define SDHCI_CMD_RESP_R3                         BIT(3)
#define SDHCI_CMD_RESP_R6                         BIT(6)
#define SDHCI_CMD_RESP_R7                         BIT(7)

/*
 * Clock Divider values
 */
#define SDHCI_CLK_400KHZ                          400000
#define SDHCI_CLK_25MHZ                           25000000
#define SDHCI_CLK_50MHZ                           50000000
#define SDHCI_CLK_100MHZ                          100000000
#define SDHCI_CLK_200MHZ                          200000000
#define SDHCI_CLK_400MHZ                          400000000

/* UHS macros */
#define SDHCI_UHS_MODE_MASK                       0x0007

/* DDR mode related macros */
#define SDHCI_DDR50_MODE_EN                       0x0004
#define SDHCI_DDR50_MODE_MASK                     BIT(2)

/* HS200/SDR50 mode related macros */
#define SDHCI_SDR25_MODE_EN                       0x0001
#define SDHCI_SDR12_MODE_EN                       0x0000
#define SDHCI_SDR50_MODE_MASK                     BIT(0)
#define SDHCI_SDR50_MODE_EN                       0x0002

#define SDHCI_SDR104_MODE_MASK                    BIT(1)
#define SDHCI_SDR104_MODE_EN                      0x0003

#define SDHCI_SDR104_MODE                         0x3
#define SDHCI_SDR50_MODE                          0x2
#define SDHCI_DDR50_MODE                          0x4
#define SDHCI_SDR25_MODE                          0x1
#define SDHCI_SDR12_MODE                          0x0

/*
 * APIs and macros exposed for mmc/sd drivers
 */
#define SDHCI_MMC_WRITE                           0
#define SDHCI_MMC_READ                            1

#define DATA_BUS_WIDTH_1BIT                       0
#define DATA_BUS_WIDTH_4BIT                       1
#define DATA_BUS_WIDTH_8BIT                       2
#define DATA_DDR_BUS_WIDTH_4BIT                   5
#define DATA_DDR_BUS_WIDTH_8BIT                   6

/* API: to initialize the controller */
void     sdhci_init(struct sdhci_host *);
/* API: Send the command & transfer data using adma */
uint32_t sdhci_send_command(struct sdhci_host *, struct mmc_command *);
/* API: Set the bus width for the contoller */
uint8_t  sdhci_set_bus_width(struct sdhci_host *, uint16_t);
/* API: Clock supply for the controller */
uint32_t sdhci_clk_supply(struct sdhci_host *, uint32_t);
/* API: To enable SDR/DDR mode */
void sdhci_set_uhs_mode(struct sdhci_host *, uint32_t);
/* API: Soft reset for the controller */
void sdhci_reset(struct sdhci_host *host, uint8_t mask);
#endif
