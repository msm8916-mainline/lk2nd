/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
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
#define CMD21_SEND_TUNING_BLOCK                   21
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
#define MMC_EXT_CSD_RST_N_FUNC                    162
#define MMC_EXT_MMC_BUS_WIDTH                     183
#define MMC_EXT_MMC_HS_TIMING                     185
#define MMC_EXT_CSD_REV                           192
#define MMC_DEVICE_TYPE                           196
#define MMC_EXT_MMC_DRV_STRENGTH                  197
#define MMC_EXT_HC_WP_GRP_SIZE                    221
#define MMC_SEC_COUNT4                            215
#define MMC_SEC_COUNT3                            214
#define MMC_SEC_COUNT2                            213
#define MMC_SEC_COUNT1                            212
#define MMC_PART_CONFIG                           179
#define MMC_EXT_CSD_EN_RPMB_REL_WR                166 //emmc 5.1 and above
#define MMC_ERASE_GRP_DEF                         175
#define MMC_USR_WP                                171
#define MMC_ERASE_TIMEOUT_MULT                    223
#define MMC_HC_ERASE_GRP_SIZE                     224
#define MMC_PARTITION_CONFIG                      179
#define MMC_EXT_CSD_EN_RPMB_REL_WR                166 //emmc 5.1 and above

/* Values for ext csd fields */
#define MMC_HS_TIMING                             0x1
#define MMC_HS200_TIMING                          0x2
#define MMC_HS400_TIMING                          0x3
#define MMC_ACCESS_WRITE                          0x3
#define MMC_SET_BIT                               0x1
#define MMC_HS_DDR_MODE                           (BIT(2) | BIT(3))
#define MMC_HS_HS200_MODE                         (BIT(4) | BIT(5))
#define MMC_HS_HS400_MODE                         (BIT(6) | BIT(7))
#define MMC_SEC_COUNT4_SHIFT                      24
#define MMC_SEC_COUNT3_SHIFT                      16
#define MMC_SEC_COUNT2_SHIFT                      8
#define MMC_HC_ERASE_MULT                         (512 * 1024)
#define RST_N_FUNC_ENABLE                         BIT(0)

/* RPMB Related */
#define RPMB_PART_MIN_SIZE                        (128 * 2014)
#define RPMB_SIZE_MULT                            168
#define REL_WR_SEC_C                              222
#define PARTITION_ACCESS_MASK                     0x7
#define MAX_RPMB_CMDS                             0x3

/* Command related */
#define MMC_MAX_COMMAND_RETRY                     1000
#define MMC_MAX_CARD_STAT_RETRY                   10000
#define MMC_RD_BLOCK_LEN                          512
#define MMC_WR_BLOCK_LEN                          512
#define MMC_R1_WP_ERASE_SKIP                      BIT(15)
#define MMC_US_PERM_WP_DIS                        BIT(4)
#define MMC_US_PWR_WP_DIS                         BIT(3)
#define MMC_US_PERM_WP_EN                         BIT(2)
#define MMC_US_PWR_WP_EN                          BIT(0)

/* MMC errors */
#define MMC_R1_BLOCK_LEN_ERR                      (1 << 29)
#define MMC_R1_ADDR_ERR                           (1 << 30)
#define MMC_R1_GENERIC_ERR                        (1 << 19)
#define MMC_R1_CC_ERROR                           (1 << 20)
#define MMC_R1_WP_VIOLATION                       (1 << 26)
#define MMC_R1_ADDR_OUT_OF_RANGE                  (1 << 31)

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
#define MMC_CLK_177MHZ                            177770000
#define MMC_CLK_200MHZ                            200000000
#define MMC_CLK_192MHZ                            192000000
#define MMC_CLK_400MHZ                            400000000

#define MMC_ADDR_OUT_OF_RANGE(resp)              ((resp >> 31) & 0x01)

/* SD card related Macros */
/* Arguments for commands */
#define MMC_SD_HC_VOLT_SUPPLIED                   0x000001AA
#define MMC_SD_OCR                                0x00FF8000
#define MMC_SD_HC_HCS                             0x40000000
#define MMC_SD_DEV_READY                          0x80000000
#define MMC_CARD_TYPE_SDHC                        0x1
#define MMC_CARD_TYPE_STD_SD                      0x0
#define SD_CARD_RCA                               0x0
#define MMC_SD_SWITCH_HS                          0x80FFFFF1

#define SD_CMD8_MAX_RETRY                         0x3
#define SD_ACMD41_MAX_RETRY                       0x14

/* SCR(SD Card Register) related */
#define SD_SCR_BUS_WIDTH                          16
#define SD_SCR_SD_SPEC                            24
#define SD_SCR_SD_SPEC3                           15
#define SD_SCR_BUS_WIDTH_MASK                     0xf0000
#define SD_SCR_SD_SPEC_MASK                       0x0f000000
#define SD_SCR_SD_SPEC3_MASK                      0x8000
#define SD_SCR_CMD23_SUPPORT                      BIT(1)
#define SD_SCR_WIDTH_4BIT                         BIT(2)

/* SSR related macros */
#define MMC_SD_AU_SIZE_BIT                        428
#define MMC_SD_AU_SIZE_LEN                        4
#define MMC_SD_ERASE_SIZE_BIT                     408
#define MMC_SD_ERASE_SIZE_LEN                     16

/* Commands for SD card */
#define CMD8_SEND_IF_COND                         8
#define ACMD6_SET_BUS_WIDTH                       6
#define ACMD13_SEND_SD_STATUS                     13
#define ACMD41_SEND_OP_COND                       41
#define ACMD51_READ_CARD_SCR                      51
#define CMD55_APP_CMD                             55

#define MMC_SAVE_TIMING(host, TIMING)              host->timing = TIMING

/* Can be used to unpack array of upto 32 bits data */
#define UNPACK_BITS(array, start, len, size_of)           \
    ({                                                    \
     uint32_t indx = (start) / (size_of);                 \
     uint32_t offset = (start) % (size_of);               \
     unsigned long long mask = (((len)<(size_of))? 1ULL<<(len):0) - 1; \
     uint32_t unpck = array[indx] >> offset;              \
     uint32_t indx2 = ((start) + (len) - 1) / (size_of);  \
     if(indx2 > indx)                                     \
     unpck |= array[indx2] << ((size_of) - offset);       \
     unpck & mask;                                        \
     })

#define swap_endian32(x) \
	((uint32_t)( \
	(((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) | \
	(((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) | \
	(((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) | \
	(((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24) ))


#define MMC_CARD_SD(card) ((card->type == MMC_CARD_TYPE_SDHC) || \
						   (card->type == MMC_CARD_TYPE_STD_SD))

#define MMC_CARD_MMC(card) ((card->type == MMC_TYPE_STD_MMC) || \
							(card->type == MMC_TYPE_MMCHC))

enum part_access_type
{
	PART_ACCESS_DEFAULT = 0x0,
	PART_ACCESS_RPMB = 0x3,
};

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

/* SCR register for SD card */
struct mmc_sd_scr {
	uint32_t bus_widths;  /* Bus width support, 8 or 1 bit */
	uint32_t sd_spec;     /* sd spec version */
	uint32_t sd3_spec;    /* sd spec 3 version */
	uint32_t cmd23_support; /* cmd23 supported or not */
};

/* SD Status Register */
struct mmc_sd_ssr {
	uint32_t au_size;     /* Allocation unit (AU) size */
	uint32_t num_aus;      /* Number of AUs */
};

/* mmc card register */
struct mmc_card {
	uint32_t rca;            /* Relative addres of the card*/
	uint32_t ocr;            /* Operating range of the card*/
	uint32_t block_size;     /* Block size for the card */
	uint32_t wp_grp_size;    /* WP group size for the card */
	uint64_t capacity;       /* card capacity */
	uint32_t type;           /* Type of the card */
	uint32_t status;         /* Card status */
	uint8_t *ext_csd;        /* Ext CSD for the card info */
	uint32_t raw_csd[4];     /* Raw CSD for the card */
	uint32_t raw_scr[2];     /* SCR for SD card */
	uint32_t rpmb_size;      /* Size of rpmb partition */
	uint32_t rel_wr_count;   /* Reliable write count */
	struct mmc_cid cid;      /* CID structure */
	struct mmc_csd csd;      /* CSD structure */
	struct mmc_sd_scr scr;   /* SCR structure */
	struct mmc_sd_ssr ssr;   /* SSR Register */
};

/* mmc device config data */
struct mmc_config_data {
	uint8_t slot;          /* Sdcc slot used */
	uint32_t pwr_irq;       /* Power Irq from card to host */
	uint32_t sdhc_base;    /* Base address for the sdhc */
	uint32_t pwrctl_base;  /* Base address for power control registers */
	uint16_t bus_width;    /* Bus width used */
	uint32_t max_clk_rate; /* Max clock rate supported */
	uint8_t hs400_support; /* SDHC HS400 mode supported or not */
	uint8_t use_io_switch; /* IO pad switch flag for shared sdc controller */
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
/* API: Change the driver type of the card */
bool mmc_set_drv_type(struct sdhci_host *host, struct mmc_card *card, uint8_t drv_type);
/* API: Send the read & write command sequence to rpmb */
uint32_t mmc_sdhci_rpmb_send(struct mmc_device *dev, struct mmc_command *cmd);
#endif
