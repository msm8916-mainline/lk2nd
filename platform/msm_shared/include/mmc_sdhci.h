/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

#ifndef __MMC_SDHCI_H__
#define __MMC_SDHCI_H__

#include <sdhci.h>

/* Emmc Card bus commands */
#define CMD0_GO_IDLE_STATE                        0
#define CMD1_SEND_OP_COND                         1
#define CMD2_ALL_SEND_CID                         2
#define CMD3_SEND_RELATIVE_ADDR                   3
#define CMD4_SET_DSR                              4
#define CMD5_SLEEP_AWAKE                          5
#define CMD6_SWITCH_FUNC                          6
#define CMD7_SELECT_DESELECT_CARD                 7
#define CMD8_SEND_EXT_CSD                         8
#define CMD9_SEND_CSD                             9
#define CMD10_SEND_CID                            10
#define CMD12_STOP_TRANSMISSION                   12
#define CMD13_SEND_STATUS                         13
#define CMD15_GO_INACTIVE_STATUS                  15
#define CMD16_SET_BLOCKLEN                        16
#define CMD17_READ_SINGLE_BLOCK                   17
#define CMD18_READ_MULTIPLE_BLOCK                 18
#define CMD23_SET_BLOCK_COUNT                     23
#define CMD24_WRITE_SINGLE_BLOCK                  24
#define CMD25_WRITE_MULTIPLE_BLOCK                25
#define CMD28_SET_WRITE_PROTECT                   28
#define CMD29_CLEAR_WRITE_PROTECT                 29
#define CMD31_SEND_WRITE_PROT_TYPE                31
#define CMD32_ERASE_WR_BLK_START                  32
#define CMD33_ERASE_WR_BLK_END                    33
#define CMD35_ERASE_GROUP_START                   35
#define CMD36_ERASE_GROUP_END                     36
#define CMD38_ERASE                               38

/* Card type */
#define MMC_TYPE_STD_SD                           0
#define MMC_TYPE_SDHC                             1
#define MMC_TYPE_SDIO                             2
#define MMC_TYPE_MMCHC                            3
#define MMC_TYPE_STD_MMC                          4

/* OCR Register */
#define MMC_OCR_17_19                             (1 << 7)
#define MMC_OCR_27_36                             (0x1FF << 15)
#define MMC_OCR_SEC_MODE                          (2 << 29)
#define MMC_OCR_BUSY                              (1 << 31)

/* Card status */
#define MMC_CARD_STATUS(x)                        ((x >> 9) & 0x0F)
#define MMC_TRAN_STATE                            4
#define MMC_PROG_STATE                            7
#define MMC_SWITCH_FUNC_ERR_FLAG                  (1 << 7)
#define MMC_STATUS_INACTIVE                       0
#define MMC_STATUS_ACTIVE                         1
#define MMC_READY_FOR_DATA                        (1 << 8)

/* EXT_CSD */
/* Offsets in the ext csd */
#define MMC_EXT_MMC_BUS_WIDTH                     183
#define MMC_EXT_MMC_HS_TIMING                     185
#define MMC_DEVICE_TYPE                           196
#define MMC_EXT_HC_WP_GRP_SIZE                    221
#define MMC_SEC_COUNT4                            215
#define MMC_SEC_COUNT3                            214
#define MMC_SEC_COUNT2                            213
#define MMC_SEC_COUNT1                            212
#define MMC_PART_CONFIG                           179
#define MMC_ERASE_GRP_DEF                         175
#define MMC_USR_WP                                171
#define MMC_HC_ERASE_GRP_SIZE                     224

/* Values for ext csd fields */
#define MMC_HS_TIMING                             0x1
#define MMC_HS200_TIMING                          0x2
#define MMC_ACCESS_WRITE                          0x3
#define MMC_SET_BIT                               0x1
#define MMC_HS_DDR_MODE                           (BIT(2) | BIT(3))
#define MMC_HS_HS200_MODE                         (BIT(4) | BIT(5))
#define MMC_SEC_COUNT4_SHIFT                      24
#define MMC_SEC_COUNT3_SHIFT                      16
#define MMC_SEC_COUNT2_SHIFT                      8
#define MMC_HC_ERASE_MULT                         (512 * 1024)

/* Command related */
#define MMC_MAX_COMMAND_RETRY                     1000
#define MMC_MAX_CARD_STAT_RETRY                   10000
#define MMC_RD_BLOCK_LEN                          512
#define MMC_WR_BLOCK_LEN                          512
#define MMC_R1_BLOCK_LEN_ERR                      (1 << 29)
#define MMC_R1_ADDR_ERR                           (1 << 30)
#define MMC_R1_WP_ERASE_SKIP                      BIT(15)
#define MMC_US_PERM_WP_DIS                        BIT(4)
#define MMC_US_PWR_WP_DIS                         BIT(3)
#define MMC_US_PERM_WP_EN                         BIT(2)
#define MMC_US_PWR_WP_EN                          BIT(0)

/* RCA of the card */
#define MMC_RCA                                   2
#define MMC_CARD_RCA_BIT                          16

/* Misc card macros */
#define MMC_BLK_SZ                                512
#define MMC_CARD_SLEEP                            (1 << 15)

/* Clock rates */
#define MMC_CLK_400KHZ                            400000
#define MMC_CLK_144KHZ                            144000
#define MMC_CLK_20MHZ                             20000000
#define MMC_CLK_25MHZ                             25000000
#define MMC_CLK_48MHZ                             48000000
#define MMC_CLK_50MHZ                             49152000
#define MMC_CLK_96MHZ                             96000000
#define MMC_CLK_200MHZ                            200000000

#define MMC_ADDR_OUT_OF_RANGE(resp)              ((resp >> 31) & 0x01)

/* Can be used to unpack array of upto 32 bits data */
#define UNPACK_BITS(array, start, len, size_of)           \
    ({                                                    \
     uint32_t indx = (start) / (size_of);                 \
     uint32_t offset = (start) % (size_of);               \
     uint32_t mask = (((len)<(size_of))? 1<<(len):0) - 1; \
     uint32_t unpck = array[indx] >> offset;              \
     uint32_t indx2 = ((start) + (len) - 1) / (size_of);  \
     if(indx2 > indx)                                     \
     unpck |= array[indx2] << ((size_of) - offset);       \
     unpck & mask;                                        \
     })

/* CSD Register.
 * Note: not all the fields have been defined here
 */
struct mmc_csd {
	uint32_t cmmc_structure;
	uint32_t spec_vers;
	uint32_t card_cmd_class;
	uint32_t write_blk_len;
	uint32_t read_blk_len;
	uint32_t r2w_factor;
	uint32_t sector_size;
	uint32_t c_size_mult;
	uint32_t c_size;
	uint32_t nsac_clk_cycle;
	uint32_t taac_ns;
	uint32_t tran_speed;
	uint32_t erase_grp_size;
	uint32_t erase_grp_mult;
	uint32_t wp_grp_size;
	uint32_t wp_grp_enable:1;
	uint32_t perm_wp:1;
	uint32_t temp_wp:1;
	uint32_t erase_blk_len:1;
	uint32_t read_blk_misalign:1;
	uint32_t write_blk_misalign:1;
	uint32_t read_blk_partial:1;
	uint32_t write_blk_partial:1;
};

/* CID Register */
struct mmc_cid {
	uint32_t mid;    /* 8 bit manufacturer id */
	uint32_t oid;    /* 16 bits 2 character ASCII - OEM ID */
	uint8_t  pnm[7]; /* 6 character ASCII -  product name */
	uint32_t prv;    /* 8 bits - product revision */
	uint32_t psn;    /* 32 bits - product serial number */
	uint32_t month;  /* 4 bits manufacturing month */
	uint32_t year;  /* 4 bits manufacturing year */
};

/* mmc card register */
struct mmc_card {
	uint32_t rca;            /* Relative addres of the card*/
	uint32_t ocr;            /* Operating range of the card*/
	uint64_t capacity;       /* card capacity */
	uint32_t type;           /* Type of the card */
	uint32_t status;         /* Card status */
	uint8_t *ext_csd;        /* Ext CSD for the card info */
	uint32_t raw_csd[4];     /* Raw CSD for the card */
	struct mmc_cid cid;      /* CID structure */
	struct mmc_csd csd;      /* CSD structure */
};

/* mmc device config data */
struct mmc_config_data {
	uint8_t slot;          /* Sdcc slot used */
	uint32_t base;         /* Based address for the sdcc */
	uint16_t bus_width;    /* Bus width used */
	uint32_t max_clk_rate; /* Max clock rate supported */
};

/* mmc device structure */
struct mmc_device {
	struct sdhci_host host;          /* Handle to host controller */
	struct mmc_card card;            /* Handle to mmc card */
	struct mmc_config_data config;   /* Handle for the mmc config data */
};

/*
 * APIS exposed to block level driver
 */
/* API: Initialize the mmc card */
struct mmc_device *mmc_init(struct mmc_config_data *);
/* API: Read required number of blocks from card into destination */
uint32_t mmc_sdhci_read(struct mmc_device *dev, void *dest, uint64_t blk_addr, uint32_t num_blocks);
/* API: Write requried number of blocks from source to card */
uint32_t mmc_sdhci_write(struct mmc_device *dev, void *src, uint64_t blk_addr, uint32_t num_blocks);
/* API: Erase len bytes (after converting to number of erase groups), from specified address */
uint32_t mmc_sdhci_erase(struct mmc_device *dev, uint32_t blk_addr, uint64_t len);
/* API: Write protect or release len bytes (after converting to number of write protect groups) from specified start address*/
uint32_t mmc_set_clr_power_on_wp_user(struct mmc_device *dev, uint32_t addr, uint64_t len, uint8_t set_clr);
/* API: Get the WP status of write protect groups starting at addr */
uint32_t mmc_get_wp_status(struct mmc_device *dev, uint32_t addr, uint8_t *wp_status);
/* API: Put the mmc card in sleep mode */
void mmc_put_card_to_sleep(struct mmc_device *dev);
#endif
