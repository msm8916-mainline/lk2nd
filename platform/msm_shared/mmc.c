/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.

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

#include <string.h>
#include <stdlib.h>
#include <debug.h>
#include <reg.h>
#include "mmc.h"
#include <platform/iomap.h>

#ifndef NULL
#define NULL        0
#endif

/* data access time unit in ns */
static const unsigned int taac_unit[] =
{ 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000 };
/* data access time value x 10 */
static const unsigned int taac_value[] =
{ 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

/* data transfer rate in kbit/s */
static const unsigned int xfer_rate_unit[] =
{ 100, 1000, 10000, 100000, 0, 0, 0, 0 };
/* data transfer rate value x 10*/
static const unsigned int xfer_rate_value[] =
{ 0, 10, 12, 13, 15, 20, 26, 30, 35, 40, 45, 52, 55, 60, 70, 80 };

char *ext3_partitions[] = {"system", "userdata"};
unsigned int ext3_count = 0;

static unsigned mmc_sdc_clk[] = { SDC1_CLK, SDC2_CLK, SDC3_CLK, SDC4_CLK};
static unsigned mmc_sdc_pclk[] = { SDC1_PCLK, SDC2_PCLK, SDC3_PCLK, SDC4_PCLK};

int mmc_clock_enable_disable(unsigned id, unsigned enable);
int mmc_clock_get_rate(unsigned id);
int mmc_clock_set_rate(unsigned id, unsigned rate);
void mdelay(unsigned msecs);

struct mmc_boot_host mmc_host;
struct mmc_boot_card mmc_card;
struct mbr_entry mbr[MAX_PARTITIONS];
unsigned mmc_partition_count = 0;
static void mbr_fill_name (struct mbr_entry *mbr_ent, unsigned int type);
unsigned int mmc_read (unsigned long long data_addr, unsigned int* out, unsigned int data_len);


unsigned int SWAP_ENDIAN(unsigned int val)
{
    return ((val & 0xFF) << 24) |
        (((val >> 8) & 0xFF) << 16) |
        (((val >> 16) & 0xFF) << 8) |
        (val >> 24);
}

/*
 * Function to enable and set master and peripheral clock for
 * MMC card.
 */
static unsigned int mmc_boot_enable_clock( struct mmc_boot_host* host,
        unsigned int mclk)
{
    unsigned int mmc_clk = 0;

#ifndef PLATFORM_MSM8X60
    int mmc_signed_ret = 0;
    unsigned SDC_CLK = mmc_sdc_clk[MMC_SLOT - 1];
    unsigned SDC_PCLK = mmc_sdc_pclk[MMC_SLOT - 1];

    if( host == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    if( !host->clk_enabled )
    {
        /* set clock */
        if( mmc_clock_enable_disable(SDC_PCLK, MMC_CLK_ENABLE) < 0 )
        {
            dprintf(CRITICAL,  "Failure enabling PCLK!\n");
            goto error_pclk;
        }

        if( mmc_clock_enable_disable(SDC_CLK, MMC_CLK_ENABLE) < 0 )
        {
            dprintf(CRITICAL,  "Failure enabling MMC Clock!\n");
            goto error;
        }
        host->clk_enabled = 1;
    }
    if( host->mclk_rate != mclk )
    {
        if( mmc_clock_set_rate(SDC_CLK, mclk) < 0 )
        {
            dprintf(CRITICAL, "Failure setting clock rate for MCLK - clk_rate: %d\n!", mclk );
            goto error_mclk;
        }

        if( ( mmc_signed_ret = mmc_clock_get_rate(SDC_CLK) ) < 0 )
        {
            dprintf(CRITICAL, "Failure getting clock rate for MCLK - clk_rate: %d\n!", host->mclk_rate );
            goto error_mclk;
        }

        host->mclk_rate = (unsigned int)mmc_signed_ret;
    }

    if( ( mmc_signed_ret = mmc_clock_get_rate(SDC_PCLK) ) < 0 )
    {
        dprintf(CRITICAL, "Failure getting clock rate for PCLK - clk_rate: %d\n!", host->pclk_rate );
        goto error_pclk;
    }

    host->pclk_rate = ( unsigned int )mmc_signed_ret;
    dprintf(INFO,  "Clock rate - mclk: %dHz    pclk: %dHz\n", host->mclk_rate, host->pclk_rate );
#else
    clock_set_enable(mclk);
    host->mclk_rate = mclk;
    host->pclk_rate = mclk;
    host->clk_enabled = 1;
#endif
    //enable mci clock
    mmc_clk |= MMC_BOOT_MCI_CLK_ENABLE;
    //enable flow control
    mmc_clk |= MMC_BOOT_MCI_CLK_ENA_FLOW;
    //latch data and command using feedback clock
    mmc_clk |= MMC_BOOT_MCI_CLK_IN_FEEDBACK;
    writel( mmc_clk, MMC_BOOT_MCI_CLK );
    return MMC_BOOT_E_SUCCESS;

#ifndef PLATFORM_MSM8X60
error_pclk:
    mmc_clock_enable_disable(SDC_PCLK, MMC_CLK_DISABLE);
error_mclk:
    mmc_clock_enable_disable(SDC_CLK, MMC_CLK_DISABLE);
error:
    return MMC_BOOT_E_CLK_ENABLE_FAIL;
#endif
}


/* Sets a timeout for read operation.
 */
static unsigned int mmc_boot_set_read_timeout( struct mmc_boot_host* host,
        struct mmc_boot_card* card )
{
    unsigned int timeout_ns = 0;

    if( ( host == NULL ) || ( card == NULL ) )
    {
        return MMC_BOOT_E_INVAL;
    }

    if( card->type == MMC_BOOT_TYPE_SDHC )
    {
        card->rd_timeout_ns = 100000000;
    }
    else if( card->type == MMC_BOOT_TYPE_STD_SD )
    {
        timeout_ns = 10 * ( (card->csd.taac_ns ) +
                ( card->csd.nsac_clk_cycle / (host->mclk_rate/1000000000)));
    }
    else
    {
        return MMC_BOOT_E_NOT_SUPPORTED;
    }

    dprintf(INFO, " Read timeout set: %d ns\n", card->rd_timeout_ns );

    return MMC_BOOT_E_SUCCESS;
}

/* Sets a timeout for write operation.
 */
static unsigned int mmc_boot_set_write_timeout( struct mmc_boot_host* host,
        struct mmc_boot_card* card )
{
    unsigned int timeout_ns = 0;

    if( ( host == NULL ) || ( card == NULL ) )
    {
        return MMC_BOOT_E_INVAL;
    }

    if( card->type == MMC_BOOT_TYPE_SDHC )
    {
        card->wr_timeout_ns = 100000000;
    }
    else if( card->type == MMC_BOOT_TYPE_STD_SD )
    {
        timeout_ns = 10 * ( (  card->csd.taac_ns ) +
                ( card->csd.nsac_clk_cycle / ( host->mclk_rate/1000000000 ) ) );
        timeout_ns = timeout_ns << card->csd.r2w_factor;
    }
    else
    {
        return MMC_BOOT_E_NOT_SUPPORTED;
    }

    dprintf(INFO, " Write timeout set: %d ns\n", card->wr_timeout_ns );

    return MMC_BOOT_E_SUCCESS;
}


/*
 * Decodes CSD response received from the card. Note that we have defined only
 * few of the CSD elements in csd structure. We'll only decode those values.
 */
static unsigned int mmc_boot_decode_and_save_csd( struct mmc_boot_card* card,
        unsigned int* raw_csd )
{
    unsigned int mmc_sizeof = 0;
    unsigned int mmc_unit = 0;
    unsigned int mmc_value = 0;
    unsigned int mmc_temp = 0;

    struct mmc_boot_csd mmc_csd;

    if( ( card == NULL ) || ( raw_csd == NULL ) )
    {
        return MMC_BOOT_E_INVAL;
    }

    /* CSD register is little bit differnet for CSD version 2.0 High Capacity
     * and CSD version 1.0/2.0 Standard memory cards. In Version 2.0 some of
     * the fields have fixed values and it's not necessary for host to refer
     * these fields in CSD sent by card */

    mmc_sizeof = sizeof(unsigned int) * 8;

    mmc_csd.cmmc_structure = UNPACK_BITS( raw_csd, 126, 2, mmc_sizeof );

    /* cmmc_structure- 0: Version 1.0     1: Version 2.0 */
    if( mmc_csd.cmmc_structure )
    {
        mmc_csd.card_cmd_class = UNPACK_BITS( raw_csd, 84, 12, mmc_sizeof );
        mmc_csd.write_blk_len = 512; /* Fixed value is 9 = 2^9 = 512 */
        mmc_csd.read_blk_len = 512;  /* Fixed value is 9 = 512 */
        mmc_csd.r2w_factor = UNPACK_BITS( raw_csd, 26, 3, mmc_sizeof );      /* Fixed value: 010b */
        mmc_csd.c_size_mult = 0;     /* not there in version 2.0 */
        mmc_csd.c_size = UNPACK_BITS( raw_csd, 62, 12, mmc_sizeof );
        mmc_csd.nsac_clk_cycle = UNPACK_BITS( raw_csd, 104, 8, mmc_sizeof) * 100;

        mmc_unit = UNPACK_BITS( raw_csd, 112, 3, mmc_sizeof );
        mmc_value = UNPACK_BITS( raw_csd, 115, 4, mmc_sizeof );
        mmc_csd.taac_ns = ( taac_value[mmc_value] * taac_unit[mmc_unit]) / 10;

        mmc_csd.erase_blk_len = 1;
        mmc_csd.read_blk_misalign = 0;
        mmc_csd.write_blk_misalign = 0;
        mmc_csd.read_blk_partial = 0;
        mmc_csd.write_blk_partial = 0;

        mmc_unit = UNPACK_BITS( raw_csd, 96, 3, mmc_sizeof );
        mmc_value = UNPACK_BITS( raw_csd, 99, 4, mmc_sizeof );
        mmc_csd.tran_speed = ( xfer_rate_value[mmc_value] * xfer_rate_unit[mmc_unit]) / 10;

        /* Calculate card capcity now itself */
        card->capacity = ( 1 + mmc_csd.c_size ) * 512000;
    }
    else
    {
        mmc_csd.card_cmd_class = UNPACK_BITS( raw_csd, 84, 12, mmc_sizeof );

        mmc_temp = UNPACK_BITS( raw_csd, 22, 4, mmc_sizeof );
        mmc_csd.write_blk_len = ( mmc_temp > 8 && mmc_temp < 12 )? ( 1 << mmc_temp ) : 512;

        mmc_temp = UNPACK_BITS( raw_csd, 80, 4, mmc_sizeof );
        mmc_csd.read_blk_len = ( mmc_temp > 8 && mmc_temp < 12 )? ( 1 << mmc_temp ) : 512;

        mmc_unit = UNPACK_BITS( raw_csd, 112, 3, mmc_sizeof );
        mmc_value = UNPACK_BITS( raw_csd, 115, 4, mmc_sizeof );
        mmc_csd.taac_ns = ( taac_value[mmc_value] * taac_unit[mmc_unit]) / 10;

        mmc_unit = UNPACK_BITS( raw_csd, 96, 3, mmc_sizeof );
        mmc_value = UNPACK_BITS( raw_csd, 99, 4, mmc_sizeof );
        mmc_csd.tran_speed = ( xfer_rate_value[mmc_value] * xfer_rate_unit[mmc_unit]) / 10;

        mmc_csd.nsac_clk_cycle = UNPACK_BITS( raw_csd, 104, 8, mmc_sizeof ) * 100;

        mmc_csd.r2w_factor = UNPACK_BITS( raw_csd, 26, 3, mmc_sizeof );
        mmc_csd.sector_size = UNPACK_BITS( raw_csd, 39, 7, mmc_sizeof ) + 1;

        mmc_csd.erase_blk_len = UNPACK_BITS( raw_csd, 46, 1, mmc_sizeof );
        mmc_csd.read_blk_misalign = UNPACK_BITS( raw_csd, 77, 1, mmc_sizeof );
        mmc_csd.write_blk_misalign = UNPACK_BITS( raw_csd, 78, 1, mmc_sizeof );
        mmc_csd.read_blk_partial = UNPACK_BITS( raw_csd, 79, 1, mmc_sizeof );
        mmc_csd.write_blk_partial = UNPACK_BITS( raw_csd, 21, 1, mmc_sizeof );

        mmc_csd.c_size_mult = UNPACK_BITS( raw_csd, 47, 3, mmc_sizeof );
        mmc_csd.c_size = UNPACK_BITS( raw_csd, 62, 12, mmc_sizeof );
        mmc_temp = ( 1 << ( mmc_csd.c_size_mult + 2 ) ) * ( mmc_csd.c_size + 1 );
        card->capacity = mmc_temp * mmc_csd.read_blk_len;
    }

    /* save the information in card structure */
    memcpy( (struct mmc_boot_csd *)&card->csd, (struct mmc_boot_csd *)&mmc_csd,
            sizeof(struct mmc_boot_csd) );

    dprintf(INFO,  "Decoded CSD fields:\n" );
    dprintf(INFO,  "cmmc_structure: %d\n", mmc_csd.cmmc_structure );
    dprintf(INFO, "card_cmd_class: %x\n", mmc_csd.card_cmd_class );
    dprintf(INFO, "write_blk_len: %d\n", mmc_csd.write_blk_len );
    dprintf(INFO, "read_blk_len: %d\n", mmc_csd.read_blk_len );
    dprintf(INFO, "r2w_factor: %d\n", mmc_csd.r2w_factor );
    dprintf(INFO, "sector_size: %d\n", mmc_csd.sector_size );
    dprintf(INFO, "c_size_mult:%d\n", mmc_csd.c_size_mult );
    dprintf(INFO, "c_size: %d\n", mmc_csd.c_size );
    dprintf(INFO, "nsac_clk_cycle: %d\n", mmc_csd.nsac_clk_cycle );
    dprintf(INFO, "taac_ns: %d\n", mmc_csd.taac_ns );
    dprintf(INFO, "tran_speed: %d kbps\n", mmc_csd.tran_speed );
    dprintf(INFO, "erase_blk_len: %d\n", mmc_csd.erase_blk_len );
    dprintf(INFO, "read_blk_misalign: %d\n", mmc_csd.read_blk_misalign );
    dprintf(INFO, "write_blk_misalign: %d\n", mmc_csd.write_blk_misalign );
    dprintf(INFO, "read_blk_partial: %d\n", mmc_csd.read_blk_partial );
    dprintf(INFO, "write_blk_partial: %d\n", mmc_csd.write_blk_partial );
    dprintf(INFO, "Card Capacity: %d Bytes\n", card->capacity );

    return MMC_BOOT_E_SUCCESS;

}

/*
 * Decode CID sent by the card.
 */
static unsigned int mmc_boot_decode_and_save_cid( struct mmc_boot_card* card,
        unsigned int* raw_cid )
{
    struct mmc_boot_cid mmc_cid;
    unsigned int mmc_sizeof = 0;
    int i = 0;

    if( ( card == NULL ) || ( raw_cid == NULL ) )
    {
        return MMC_BOOT_E_INVAL;
    }

    mmc_sizeof = sizeof( unsigned int ) * 8;
    mmc_cid.mid = UNPACK_BITS( raw_cid, 120, 8, mmc_sizeof );
    mmc_cid.oid = UNPACK_BITS( raw_cid, 104, 16, mmc_sizeof );

    for( i = 0; i < 6; i++ )
    {
        mmc_cid.pnm[i] = (unsigned char) UNPACK_BITS(raw_cid, \
                            (104 - 8 * (i+1)), 8, mmc_sizeof );
    }
    mmc_cid.pnm[6] = 0;

    mmc_cid.prv = UNPACK_BITS( raw_cid, 48, 8, mmc_sizeof );
    mmc_cid.psn = UNPACK_BITS( raw_cid, 16, 32, mmc_sizeof );
    mmc_cid.month = UNPACK_BITS( raw_cid, 12, 4, mmc_sizeof );
    mmc_cid.year = UNPACK_BITS( raw_cid, 8, 4, mmc_sizeof );

    /* save it in card database */
    memcpy( ( struct mmc_boot_cid * )&card->cid, \
            ( struct mmc_boot_cid * )&mmc_cid,   \
            sizeof( struct mmc_boot_cid ) );

    dprintf(INFO, "Decoded CID fields:\n" );
    dprintf(INFO, "Manufacturer ID: %x\n", mmc_cid.mid );
    dprintf(INFO, "OEM ID: 0x%x\n", mmc_cid.oid );
    dprintf(INFO, "Product Name: %s\n", mmc_cid.pnm );
    dprintf(INFO, "Product revision: %d.%d\n", (mmc_cid.prv >> 4), (mmc_cid.prv & 0xF) );
    dprintf(INFO, "Product serial number: %X\n", mmc_cid.psn );
    dprintf(INFO, "Manufacturing date: %d %d\n", mmc_cid.month, mmc_cid.year + 1997 );

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Sends specified command to a card and waits for a response.
 */
static unsigned int mmc_boot_send_command( struct mmc_boot_command* cmd )
{
    unsigned int mmc_cmd = 0;
    unsigned int mmc_status = 0;
    unsigned int mmc_resp = 0;
    unsigned int mmc_return = MMC_BOOT_E_SUCCESS;
    unsigned int cmd_index = 0;
    int i = 0;

    /* basic check */
    if( cmd == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    /* 1. Write command argument to MMC_BOOT_MCI_ARGUMENT register */
    writel( cmd->argument, MMC_BOOT_MCI_ARGUMENT );

    /* 2. Set appropriate fields and write MMC_BOOT_MCI_CMD */
    /* 2a. Write command index in CMD_INDEX field */
    cmd_index = cmd->cmd_index;
    mmc_cmd |= cmd->cmd_index;
    /* 2b. Set RESPONSE bit to 1 for all cmds except CMD0 */
    if( cmd_index != CMD0_GO_IDLE_STATE )
    {
        mmc_cmd |= MMC_BOOT_MCI_CMD_RESPONSE;
    }

    /* 2c. Set LONGRESP bit to 1 for CMD2, CMD9 and CMD10 */
    if( IS_RESP_136_BITS(cmd->resp_type) )
    {
        mmc_cmd |= MMC_BOOT_MCI_CMD_LONGRSP;
    }

    /* 2d. Set INTERRUPT bit to 1 to disable command timeout */

    /* 2e. Set PENDING bit to 1 for CMD12 in the beginning of stream
       mode data transfer*/
    if( cmd->xfer_mode == MMC_BOOT_XFER_MODE_STREAM )
    {
        mmc_cmd |= MMC_BOOT_MCI_CMD_PENDING;
    }

    /* 2f. Set ENABLE bit to 1 */
    mmc_cmd |= MMC_BOT_MCI_CMD_ENABLE;

    /* 2g. Set PROG_ENA bit to 1 for CMD12, CMD13 issued at the end of
       write data transfer */
    if( ( cmd_index == CMD12_STOP_TRANSMISSION ||
                cmd_index == CMD13_SEND_STATUS ) && cmd->prg_enabled )
    {
        mmc_cmd |= MMC_BOOT_MCI_CMD_PROG_ENA;
    }

    /* 2h. Set MCIABORT bit to 1 for CMD12 when working with SDIO card */
    /* 2i. Set CCS_ENABLE bit to 1 for CMD61 when Command Completion Signal
       of CE-ATA device is enabled */

    /* 2j. clear all static status bits */
    writel( MMC_BOOT_MCI_STATIC_STATUS, MMC_BOOT_MCI_CLEAR );

    /* 2k. Write to MMC_BOOT_MCI_CMD register */
    writel( mmc_cmd, MMC_BOOT_MCI_CMD );

    dprintf(INFO, "Command sent: CMD%d MCI_CMD_REG:%x MCI_ARG:%x\n",
            cmd_index, mmc_cmd, cmd->argument );

    /* 3. Wait for interrupt or poll on the following bits of MCI_STATUS
       register */
    do{
        /* 3a. Read MCI_STATUS register */
        while(readl( MMC_BOOT_MCI_STATUS ) \
                & MMC_BOOT_MCI_STAT_CMD_ACTIVE);

        mmc_status = readl( MMC_BOOT_MCI_STATUS );

        /* 3b. CMD_SENT bit supposed to be set to 1 only after CMD0 is sent -
           no response required. */
        if( ( cmd->resp_type == MMC_BOOT_RESP_NONE ) &&
                (mmc_status & MMC_BOOT_MCI_STAT_CMD_SENT ) )
        {
            break;
        }

        /* 3c. If CMD_TIMEOUT bit is set then no response was received */
        else if( mmc_status & MMC_BOOT_MCI_STAT_CMD_TIMEOUT )
        {
            mmc_return = MMC_BOOT_E_TIMEOUT;
            break;
        }

        /* 3d. If CMD_RESPONSE_END bit is set to 1 then command's response was
           received and CRC check passed
           Spcial case for ACMD41: it seems to always fail CRC even if
           the response is valid
           */
        else if (( mmc_status & MMC_BOOT_MCI_STAT_CMD_RESP_END ) || (cmd_index == CMD1_SEND_OP_COND))
        {
            /* 3i. Read MCI_RESP_CMD register to verify that response index is
               equal to command index */
            mmc_resp = readl( MMC_BOOT_MCI_RESP_CMD ) & 0x3F;

            /* However, long response does not contain the command index field.
             * In that case, response index field must be set to 111111b (0x3F) */
            if( ( mmc_resp == cmd_index ) ||
                    ( cmd->resp_type == MMC_BOOT_RESP_R2 ||
                      cmd->resp_type == MMC_BOOT_RESP_R3 ||
                      cmd->resp_type == MMC_BOOT_RESP_R6 ) )
            {
                /* 3j. If resp index is equal to cmd index, read command resp
                   from MCI_RESPn registers
                   - MCI_RESP0/1/2/3 for CMD2/9/10
                   - MCI_RESP0 for all other registers */
                if( IS_RESP_136_BITS( cmd->resp_type ) )
                {
                    for( i = 0; i < 4; i++ )
                    {
                        cmd->resp[3-i] = readl( MMC_BOOT_MCI_RESP_0 + ( i * 4 ) );

                    }
                }
                else
                {
                    cmd->resp[0] = readl( MMC_BOOT_MCI_RESP_0 );
                }
            }
            else
            {
                /* command index mis-match */
                mmc_return = MMC_BOOT_E_CMD_INDX_MISMATCH;
            }

            dprintf(INFO, "Command response received: %X\n", cmd->resp[0] );
            break;
        }

        /* 3e. If CMD_CRC_FAIL bit is set to 1 then cmd's response was recvd,
           but CRC check failed. */
        else if( ( mmc_status & MMC_BOOT_MCI_STAT_CMD_CRC_FAIL )  )
        {
            mmc_return = MMC_BOOT_E_CRC_FAIL;
            break;
        }

    }while(1);

    return mmc_return;
}

/*
 * Reset all the cards to idle condition (CMD 0)
 */
static unsigned int mmc_boot_reset_cards( void )
{
    struct mmc_boot_command cmd;

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    cmd.cmd_index = CMD0_GO_IDLE_STATE;
    cmd.argument = 0; // stuff bits - ignored
    cmd.cmd_type = MMC_BOOT_CMD_BCAST;
    cmd.resp_type = MMC_BOOT_RESP_NONE;

    /* send command */
    return mmc_boot_send_command( &cmd );
}

/*
 * Send CMD1 to know whether the card supports host VDD profile or not.
 */
static unsigned int mmc_boot_send_op_cond( struct mmc_boot_host* host,
        struct mmc_boot_card* card )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_resp = 0;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( ( host == NULL ) || ( card == NULL ) )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD1 format:
     * [31] Busy bit
     * [30:29] Access mode
     * [28:24] reserved
     * [23:15] 2.7-3.6
     * [14:8]  2.0-2.6
     * [7]     1.7-1.95
     * [6:0]   reserved
     */

    cmd.cmd_index = CMD1_SEND_OP_COND;
    cmd.argument = host->ocr;
    cmd.cmd_type = MMC_BOOT_CMD_BCAST_W_RESP;
    cmd.resp_type = MMC_BOOT_RESP_R3;

    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    /* Now it's time to examine response */
    mmc_resp = cmd.resp[0];

    /* Response contains card's ocr. Update card's information */
    card->ocr = mmc_resp;

    /* Check the response for busy status */
    if( !( mmc_resp & MMC_BOOT_OCR_BUSY ) )
    {
        return MMC_BOOT_E_CARD_BUSY;
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Request any card to send its uniquie card identification (CID) number (CMD2).
 */
static unsigned int mmc_boot_all_send_cid( struct mmc_boot_card* card )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD2 Format:
     * [31:0] stuff bits
     */
    cmd.cmd_index = CMD2_ALL_SEND_CID;
    cmd.argument = 0;
    cmd.cmd_type = MMC_BOOT_CMD_BCAST_W_RESP;
    cmd.resp_type = MMC_BOOT_RESP_R2;

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    /* Response contains card's 128 bits CID register */
    mmc_ret = mmc_boot_decode_and_save_cid( card, cmd.resp );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }
    return MMC_BOOT_E_SUCCESS;
}

/*
 * Ask any card to send it's relative card address (RCA).This RCA number is
 * shorter than CID and is used by the host to address the card in future (CMD3)
 */
static unsigned int mmc_boot_send_relative_address( struct mmc_boot_card* card )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD3 Format:
     * [31:0] stuff bits
     */
    cmd.cmd_index = CMD3_SEND_RELATIVE_ADDR;
    cmd.argument = (MMC_RCA << 16);
    card->rca =  (cmd.argument >> 16);
    cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
    cmd.resp_type = MMC_BOOT_RESP_R1;

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Requests card to send it's CSD register's contents. (CMD9)
 */
static unsigned int mmc_boot_send_csd( struct mmc_boot_card* card )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_arg = 0;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD9 Format:
     * [31:16] RCA
     * [15:0] stuff bits
     */
    mmc_arg |= card->rca << 16;

    cmd.cmd_index = CMD9_SEND_CSD;
    cmd.argument = mmc_arg;
    cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
    cmd.resp_type = MMC_BOOT_RESP_R2;

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    /* Response contains card's 128 bits CSD register */
    /* Decode and save the register */
    mmc_ret = mmc_boot_decode_and_save_csd( card, cmd.resp );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Selects a card by sending CMD7 to the card with its RCA.
 * If RCA field is set as 0 ( or any other address ),
 * the card will be de-selected. (CMD7)
 */
static unsigned int mmc_boot_select_card( struct mmc_boot_card* card,
        unsigned int rca )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_arg = 0;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD7 Format:
     * [31:16] RCA
     * [15:0] stuff bits
     */
    mmc_arg |= rca << 16;

    cmd.cmd_index = CMD7_SELECT_DESELECT_CARD;
    cmd.argument = mmc_arg;
    cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
    /* If we are deselecting card, we do not get response */
    if( rca == card->rca && rca)
    {
        cmd.resp_type = MMC_BOOT_RESP_R1;
    }
    else
    {
        cmd.resp_type = MMC_BOOT_RESP_NONE;
    }

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    /* As of now no need to look into a response. If it's required
     * we'll explore later on */

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Send command to set block length.
 */
static unsigned int mmc_boot_set_block_len( struct mmc_boot_card* card,
        unsigned int block_len )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD16 Format:
     * [31:0] block length
     */

    cmd.cmd_index = CMD16_SET_BLOCKLEN;
    cmd.argument = block_len;
    cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
    cmd.resp_type = MMC_BOOT_RESP_R1;

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    /* If blocklength is larger than 512 bytes,
     * the card sets BLOCK_LEN_ERROR bit. */
    if( cmd.resp[0] & MMC_BOOT_R1_BLOCK_LEN_ERR )
    {
        return MMC_BOOT_E_BLOCKLEN_ERR;
    }
    return MMC_BOOT_E_SUCCESS;
}

/*
 * Requests the card to stop transmission of data.
 */
static unsigned int mmc_boot_send_stop_transmission( struct mmc_boot_card* card,
        unsigned int prg_enabled )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD12 Format:
     * [31:0] stuff bits
     */

    cmd.cmd_index = CMD12_STOP_TRANSMISSION;
    cmd.argument = 0;
    cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
    cmd.resp_type = MMC_BOOT_RESP_R1B;
    cmd.xfer_mode = MMC_BOOT_XFER_MODE_BLOCK;
    cmd.prg_enabled = prg_enabled;

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }
    return MMC_BOOT_E_SUCCESS;
}

/*
 * Get the card's current status
 */
static unsigned int mmc_boot_get_card_status( struct mmc_boot_card* card,
        unsigned int prg_enabled )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD13 Format:
     * [31:16] RCA
     * [15:0] stuff bits
     */
    cmd.cmd_index = CMD13_SEND_STATUS;
    cmd.argument = card->rca << 16;
    cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
    cmd.resp_type = MMC_BOOT_RESP_R1;
    cmd.prg_enabled = prg_enabled;

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    return MMC_BOOT_E_SUCCESS;
}
/*
 * Send ext csd command.
 */
static unsigned int mmc_boot_send_ext_cmd (struct mmc_boot_card* card)
{
    struct mmc_boot_command cmd;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
    unsigned int mmc_reg = 0;
    unsigned char buf[512];
    unsigned int mmc_status = 0;
    unsigned int* mmc_ptr = (unsigned int *)buf;
    unsigned int mmc_count = 0;

    // start from the back
    mmc_ptr += ( (512/sizeof(int)) - 1 );

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    /* set block len */
    mmc_ret = mmc_boot_set_block_len( card, 512);
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure setting block length for Card (RCA:%s)\n",
                mmc_ret, (char *)(card->rca) );
        return mmc_ret;
    }

    /* Set the FLOW_ENA bit of MCI_CLK register to 1 */
    mmc_reg = readl( MMC_BOOT_MCI_CLK );
    mmc_reg |= MMC_BOOT_MCI_CLK_ENA_FLOW ;
    writel( mmc_reg, MMC_BOOT_MCI_CLK );

    /* Write data timeout period to MCI_DATA_TIMER register. */
    /* Data timeout period should be in card bus clock periods */
    mmc_reg =0xFFFFFFFF;
    writel( mmc_reg, MMC_BOOT_MCI_DATA_TIMER );
    writel( 512, MMC_BOOT_MCI_DATA_LENGTH );

    /* Set appropriate fields and write the MCI_DATA_CTL register. */
    /* Set ENABLE bit to 1 to enable the data transfer. */
    mmc_reg = MMC_BOOT_MCI_DATA_ENABLE | MMC_BOOT_MCI_DATA_DIR | (512 << MMC_BOOT_MCI_BLKSIZE_POS);
    writel( mmc_reg, MMC_BOOT_MCI_DATA_CTL );

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );
    /* CMD8 */
    cmd.cmd_index = CMD8_SEND_EXT_CSD;
    cmd.cmd_type  = MMC_BOOT_CMD_ADDRESS;
    cmd.resp_type = MMC_BOOT_RESP_R1;
    cmd.xfer_mode = MMC_BOOT_XFER_MODE_BLOCK;

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    do
    {
        mmc_ret = MMC_BOOT_E_SUCCESS;
        mmc_status = readl( MMC_BOOT_MCI_STATUS );

        /* If DATA_CRC_FAIL bit is set to 1 then CRC error was detected by
           card/device during the data transfer */
        if( mmc_status & MMC_BOOT_MCI_STAT_DATA_CRC_FAIL )
        {
            mmc_ret = MMC_BOOT_E_DATA_CRC_FAIL;
            break;
        }
        /* If DATA_TIMEOUT bit is set to 1 then the data transfer time exceeded
           the data timeout period without completing the transfer */
        else if( mmc_status & MMC_BOOT_MCI_STAT_DATA_TIMEOUT )
        {
            mmc_ret = MMC_BOOT_E_DATA_TIMEOUT;
            break;
        }
        /* If RX_OVERRUN bit is set to 1 then SDCC2 tried to receive data from
           the card before empty storage for new received data was available.
           Verify that bit FLOW_ENA in MCI_CLK is set to 1 during the data xfer.*/
        else if( mmc_status & MMC_BOOT_MCI_STAT_RX_OVRRUN )
        {
            /* Note: We've set FLOW_ENA bit in MCI_CLK to 1. so no need to verify
               for now */
            mmc_ret = MMC_BOOT_E_RX_OVRRUN;
            break;
        }

        if( mmc_status & MMC_BOOT_MCI_STAT_RX_DATA_AVLBL )
        {
            /* FIFO contains 16 32-bit data buffer on 16 sequential addresses*/
            *mmc_ptr = SWAP_ENDIAN(readl( MMC_BOOT_MCI_FIFO +
                        ( mmc_count % MMC_BOOT_MCI_FIFO_SIZE ) ));
            mmc_ptr--;

            /* increase mmc_count by word size */
            mmc_count += sizeof( unsigned int );

            /* quit if we have read enough of data */
            if (mmc_count >= 512)
                break;
        }
        else if( mmc_status & MMC_BOOT_MCI_STAT_DATA_END )
        {
            break;
        }

    }while(1);

    return MMC_BOOT_E_SUCCESS;

}



/*
 * Switch command
 */
static unsigned int mmc_boot_switch_cmd (struct mmc_boot_card* card,
        unsigned access,
        unsigned index,
        unsigned value)
{

    struct mmc_boot_command cmd;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD6 Format:
     * [31:26] set to 0
     * [25:24] access
     * [23:16] index
     * [15:8] value
     * [7:3] set to 0
     * [2:0] cmd set
     */
    cmd.cmd_index = CMD6_SWITCH_FUNC;
    cmd.argument |= (access << 24);
    cmd.argument |= (index << 16);
    cmd.argument |= (value << 8);
    cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
    cmd.resp_type = MMC_BOOT_RESP_R1B;

    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * A command to set the data bus width for card. Set width to either
 */
static unsigned int mmc_boot_set_bus_width( struct mmc_boot_card* card,
        unsigned int width )
{
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
    unsigned int mmc_reg = 0;
    unsigned int mmc_width = 0;

    if( width != MMC_BOOT_BUS_WIDTH_1_BIT)
    {
        mmc_width = width-1;
    }

    mmc_ret = mmc_boot_switch_cmd(card, MMC_BOOT_ACCESS_WRITE, MMC_BOOT_EXT_CMMC_BUS_WIDTH, mmc_width);

    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    /* set MCI_CLK accordingly */
    mmc_reg = readl( MMC_BOOT_MCI_CLK );
    mmc_reg &= ~MMC_BOOT_MCI_CLK_WIDEBUS_MODE;
    if ( width == MMC_BOOT_BUS_WIDTH_1_BIT )
    {
        mmc_reg |=  MMC_BOOT_MCI_CLK_WIDEBUS_1_BIT;
    }
    else if (width == MMC_BOOT_BUS_WIDTH_4_BIT )
    {
        mmc_reg |=  MMC_BOOT_MCI_CLK_WIDEBUS_4_BIT;
    }
    else if (width == MMC_BOOT_BUS_WIDTH_8_BIT )
    {
        mmc_reg |=  MMC_BOOT_MCI_CLK_WIDEBUS_8_BIT;
    }
    writel( mmc_reg, MMC_BOOT_MCI_CLK );

    mdelay(10); // Giving some time to card to stabilize.

    return MMC_BOOT_E_SUCCESS;
}


/*
 * A command to start data read from card. Either a single block or
 * multiple blocks can be read. Multiple blocks read will continuously
 * transfer data from card to host unless requested to stop by issuing
 * CMD12 - STOP_TRANSMISSION.
 */
static unsigned int mmc_boot_send_read_command( struct mmc_boot_card* card,
        unsigned int xfer_type,
        unsigned int data_addr )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD17/18 Format:
     * [31:0] Data Address
     */
    if( xfer_type == MMC_BOOT_XFER_MULTI_BLOCK )
    {
        cmd.cmd_index = CMD18_READ_MULTIPLE_BLOCK;
    }
    else
    {
        cmd.cmd_index = CMD17_READ_SINGLE_BLOCK;
    }

    cmd.argument = data_addr;
    cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
    cmd.resp_type = MMC_BOOT_RESP_R1;

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    /* Response contains 32 bit Card status. Here we'll check
       BLOCK_LEN_ERROR and ADDRESS_ERROR */
    if( cmd.resp[0] & MMC_BOOT_R1_BLOCK_LEN_ERR )
    {
        return MMC_BOOT_E_BLOCKLEN_ERR;
    }
    /* Misaligned address not matching block length */
    if( cmd.resp[0] & MMC_BOOT_R1_ADDR_ERR )
    {
        return MMC_BOOT_E_ADDRESS_ERR;
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * A command to start data write to card. Either a single block or
 * multiple blocks can be written. Multiple block write will continuously
 * transfer data from host to card unless requested to stop by issuing
 * CMD12 - STOP_TRANSMISSION.
 */
static unsigned int mmc_boot_send_write_command( struct mmc_boot_card* card,
        unsigned int xfer_type,
        unsigned int data_addr )
{
    struct mmc_boot_command cmd;
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( card == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    memset( (struct mmc_boot_command *)&cmd, 0,
            sizeof(struct mmc_boot_command) );

    /* CMD24/25 Format:
     * [31:0] Data Address
     */
    if( xfer_type == MMC_BOOT_XFER_MULTI_BLOCK )
    {
        cmd.cmd_index = CMD25_WRITE_MULTIPLE_BLOCK;
    }
    else
    {
        cmd.cmd_index = CMD24_WRITE_SINGLE_BLOCK;
    }

    cmd.argument = data_addr;
    cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
    cmd.resp_type = MMC_BOOT_RESP_R1;

    /* send command */
    mmc_ret = mmc_boot_send_command( &cmd );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return mmc_ret;
    }

    /* Response contains 32 bit Card status. Here we'll check
       BLOCK_LEN_ERROR and ADDRESS_ERROR */
    if( cmd.resp[0] & MMC_BOOT_R1_BLOCK_LEN_ERR )
    {
        return MMC_BOOT_E_BLOCKLEN_ERR;
    }
    /* Misaligned address not matching block length */
    if( cmd.resp[0] & MMC_BOOT_R1_ADDR_ERR )
    {
        return MMC_BOOT_E_ADDRESS_ERR;
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Decode type of error caused during read and write
 */
static unsigned int mmc_boot_status_error(unsigned mmc_status)
{
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    /* If DATA_CRC_FAIL bit is set to 1 then CRC error was detected by
       card/device during the data transfer */
    if( mmc_status & MMC_BOOT_MCI_STAT_DATA_CRC_FAIL )
    {
        mmc_ret = MMC_BOOT_E_DATA_CRC_FAIL;
    }
    /* If DATA_TIMEOUT bit is set to 1 then the data transfer time exceeded
       the data timeout period without completing the transfer */
    else if( mmc_status & MMC_BOOT_MCI_STAT_DATA_TIMEOUT )
    {
        mmc_ret = MMC_BOOT_E_DATA_TIMEOUT;
    }
    /* If RX_OVERRUN bit is set to 1 then SDCC2 tried to receive data from
       the card before empty storage for new received data was available.
       Verify that bit FLOW_ENA in MCI_CLK is set to 1 during the data xfer.*/
    else if( mmc_status & MMC_BOOT_MCI_STAT_RX_OVRRUN )
    {
        /* Note: We've set FLOW_ENA bit in MCI_CLK to 1. so no need to verify
           for now */
        mmc_ret = MMC_BOOT_E_RX_OVRRUN;
    }
    /* If TX_UNDERRUN bit is set to 1 then SDCC2 tried to send data to
       the card before new data for sending was available. Verify that bit
       FLOW_ENA in MCI_CLK is set to 1 during the data xfer.*/
    else if( mmc_status & MMC_BOOT_MCI_STAT_TX_UNDRUN )
    {
        /* Note: We've set FLOW_ENA bit in MCI_CLK to 1.so skipping it now*/
        mmc_ret = MMC_BOOT_E_RX_OVRRUN;
    }
    return mmc_ret;
}

/*
 * Write data_len data to address specified by data_addr. data_len is
 * multiple of blocks for block data transfer.
 */
static unsigned int mmc_boot_write_to_card( struct mmc_boot_host* host,
        struct mmc_boot_card* card,
        unsigned long long data_addr,
        unsigned int data_len,
        unsigned int* in )
{
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
    unsigned int mmc_status = 0;
    unsigned int* mmc_ptr = in;
    unsigned int mmc_count = 0;
    unsigned int mmc_reg = 0;
    unsigned int addr;
    unsigned int xfer_type;
    unsigned int write_error;

    if( ( host == NULL ) || ( card == NULL ) )
    {
        return MMC_BOOT_E_INVAL;
    }

    /* Set block length. High Capacity MMC card uses fixed 512 bytes block
       length. So no need to send CMD16. */
    if( card->type != MMC_BOOT_TYPE_SDHC )
    {
        mmc_ret = mmc_boot_set_block_len( card, card->wr_block_len );
        if( mmc_ret != MMC_BOOT_E_SUCCESS )
        {
            dprintf(CRITICAL, "Error No.%d: Failure setting block length for Card\
                    (RCA:%s)\n", mmc_ret, (char *)(card->rca) );
            return mmc_ret;
        }
    }

    /* use multi-block mode to transfer for data larger than a block */
    xfer_type = (data_len > card->rd_block_len) ? MMC_BOOT_XFER_MULTI_BLOCK :
        MMC_BOOT_XFER_SINGLE_BLOCK;

    /* For SDHC data address is specified in unit of 512B */
    addr = ( card->type != MMC_BOOT_TYPE_SDHC ) ? (unsigned int) data_addr :
        (unsigned int) (data_addr / 512);

    /* Set the FLOW_ENA bit of MCI_CLK register to 1 */
    mmc_reg = readl( MMC_BOOT_MCI_CLK );
    mmc_reg |= MMC_BOOT_MCI_CLK_ENA_FLOW ;
    writel( mmc_reg, MMC_BOOT_MCI_CLK );

    /* Write data timeout period to MCI_DATA_TIMER register */
    /* Data timeout period should be in card bus clock periods */
    /*TODO: Fix timeout value*/
    mmc_reg = 0xFFFFFFFF;
    writel( mmc_reg, MMC_BOOT_MCI_DATA_TIMER );

    /* Write the total size of the transfer data to MCI_DATA_LENGTH register */
    writel( data_len, MMC_BOOT_MCI_DATA_LENGTH );

    /* Send command to the card/device in order to start the write data xfer.
       The possible commands are CMD24/25/53/60/61 */
    mmc_ret = mmc_boot_send_write_command( card, xfer_type, addr );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure sending write command to the\
                Card(RCA:%x)\n", mmc_ret, card->rca );
        return mmc_ret;
    }

    /* Set appropriate fields and write the MCI_DATA_CTL register */
    /* Set ENABLE bit to 1 to enable the data transfer. */
    mmc_reg = 0;
    mmc_reg |= MMC_BOOT_MCI_DATA_ENABLE;
    /* Clear DIRECTION bit to 0 to enable transfer from host to card */
    /* Clear MODE bit to 0 to enable block oriented data transfer. For
       MMC cards only, if stream data transfer mode is desired, set
       MODE bit to 1. */
    /* Set DM_ENABLE bit to 1 in order to enable DMA, otherwise set 0 */
    /* Write size of block to be used during the data transfer to
       BLOCKSIZE field */
    mmc_reg |= card->wr_block_len << MMC_BOOT_MCI_BLKSIZE_POS;
    writel( mmc_reg, MMC_BOOT_MCI_DATA_CTL );

    write_error = MMC_BOOT_MCI_STAT_DATA_CRC_FAIL | \
                  MMC_BOOT_MCI_STAT_DATA_TIMEOUT  | \
                  MMC_BOOT_MCI_STAT_TX_UNDRUN;

    /* Write the transfer data to SDCC3 FIFO */
    /* If Data Mover is used for data transfer, prepare a command list entry
       and enable the Data Mover to work with SDCC2 */
    /* If Data Mover is NOT used for data xfer: */
    do
    {
        mmc_ret = MMC_BOOT_E_SUCCESS;
        mmc_status = readl( MMC_BOOT_MCI_STATUS );

        if( mmc_status & write_error )
        {
            mmc_ret = mmc_boot_status_error(mmc_status);
            break;
        }

        /* Write the data in MCI_FIFO register as long as TXFIFO_FULL bit of
           MCI_STATUS register is 0. Continue the writes until the whole
           transfer data is written. */
        if (((data_len-mmc_count) >= MMC_BOOT_MCI_FIFO_SIZE/2) &&
                ( mmc_status & MMC_BOOT_MCI_STAT_TX_FIFO_HFULL ))
        {
            for (int i=0; i < MMC_BOOT_MCI_HFIFO_COUNT; i++ )
            {
                /* FIFO contains 16 32-bit data buffer on 16 sequential addresses*/
                writel( *mmc_ptr, MMC_BOOT_MCI_FIFO +
                        ( mmc_count % MMC_BOOT_MCI_FIFO_SIZE ) );
                mmc_ptr++;
                /* increase mmc_count by word size */
                mmc_count += sizeof( unsigned int );
            }

        }
        else if( !( mmc_status & MMC_BOOT_MCI_STAT_TX_FIFO_FULL ) && (mmc_count != data_len))
        {
            /* FIFO contains 16 32-bit data buffer on 16 sequential addresses*/
            writel( *mmc_ptr, MMC_BOOT_MCI_FIFO +
                    ( mmc_count % MMC_BOOT_MCI_FIFO_SIZE ) );
            mmc_ptr++;
            /* increase mmc_count by word size */
            mmc_count += sizeof( unsigned int );
        }
        else if((mmc_status & MMC_BOOT_MCI_STAT_DATA_END))
        {
            break; //success
        }

    } while(1);

    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure on data transfer from the \
                Card(RCA:%x)\n", mmc_ret, card->rca );
        return mmc_ret;
    }

    /* Send command to the card/device in order to poll the de-assertion of
       card/device BUSY condition. It is important to set PROG_ENA bit in
       MCI_CLK register before sending the command. Possible commands are
       CMD12/13. */
    if( xfer_type == MMC_BOOT_XFER_MULTI_BLOCK )
    {
        mmc_ret = mmc_boot_send_stop_transmission( card, 1 );
        if( mmc_ret != MMC_BOOT_E_SUCCESS )
        {
            dprintf(CRITICAL, "Error No.%d: Failure sending Stop Transmission \
                    command to the Card(RCA:%x)\n", mmc_ret, card->rca );
            return mmc_ret;
        }
    }
    else
    {
        mmc_ret = mmc_boot_get_card_status( card, 1 );
        if( mmc_ret != MMC_BOOT_E_SUCCESS )
        {
            dprintf(CRITICAL, "Error No.%d: Failure getting card status of Card(RCA:%x)\n",
                    mmc_ret, card->rca );
            return mmc_ret;
        }
    }

    /* Wait for interrupt or poll on PROG_DONE bit of MCI_STATUS register. If
       PROG_DONE bit is set to 1 it means that the card finished it programming
       and stopped driving DAT0 line to 0 */
    do
    {
        mmc_status = readl( MMC_BOOT_MCI_STATUS );
        if( mmc_status & MMC_BOOT_MCI_STAT_PROG_DONE )
        {
            break;
        }
    } while(1);

    return MMC_BOOT_E_SUCCESS;
}


/*
 * Adjust the interface speed to optimal speed
 */
static unsigned int mmc_boot_adjust_interface_speed( struct mmc_boot_host* host,
        struct mmc_boot_card* card )
{
    int mmc_ret;

    mmc_boot_send_ext_cmd (card);

    /* Setting HS_TIMING in EXT_CSD (CMD6) */
    mmc_ret = mmc_boot_switch_cmd(card, MMC_BOOT_ACCESS_WRITE, MMC_BOOT_EXT_CMMC_HS_TIMING, 1);

    mmc_boot_send_ext_cmd (card);
    if(mmc_ret!= MMC_BOOT_E_SUCCESS)
    {
        return mmc_ret;
    }
#ifdef PLATFORM_MSM8X60
    mmc_ret = mmc_boot_enable_clock( host, MMC_CLK_48MHZ);
#else
    mmc_ret = mmc_boot_enable_clock( host, MMC_CLK_50MHZ);
#endif
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return MMC_BOOT_E_CLK_ENABLE_FAIL;
    }
    return MMC_BOOT_E_SUCCESS;
}

/*
 * Reads a data of data_len from the address specified. data_len
 * should be multiple of block size for block data transfer.
 */
static unsigned int mmc_boot_read_from_card( struct mmc_boot_host* host,
        struct mmc_boot_card* card,
        unsigned long long data_addr,
        unsigned int data_len,
        unsigned int* out )
{
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
    unsigned int mmc_status = 0;
    unsigned int* mmc_ptr = out;
    unsigned int mmc_count = 0;
    unsigned int mmc_reg = 0;
    unsigned int xfer_type;
    unsigned int addr = 0;
    unsigned int read_error;

    if( ( host == NULL ) || ( card == NULL ) )
    {
        return MMC_BOOT_E_INVAL;
    }

    /* Set block length. High Capacity MMC card uses fixed 512 bytes block
       length. So no need to send CMD16. */
    if( card->type != MMC_BOOT_TYPE_SDHC )
    {
        mmc_ret = mmc_boot_set_block_len( card, card->rd_block_len );
        if( mmc_ret != MMC_BOOT_E_SUCCESS )
        {
            dprintf(CRITICAL, "Error No.%d: Failure setting block length for Card (RCA:%s)\n",
                    mmc_ret, (char *)(card->rca) );
            return mmc_ret;
        }
    }

    /* use multi-block mode to transfer for data larger than a block */
    xfer_type = (data_len > card->rd_block_len) ? MMC_BOOT_XFER_MULTI_BLOCK :
        MMC_BOOT_XFER_SINGLE_BLOCK;

    /* Set the FLOW_ENA bit of MCI_CLK register to 1 */
    /* Note: It's already enabled */

    /* If Data Mover is used for data transfer then prepare Command
       List Entry and enable the Data mover to work with SDCC2 */
    /* Note: Data Mover not used */

    /* Write data timeout period to MCI_DATA_TIMER register. */
    /* Data timeout period should be in card bus clock periods */
    mmc_reg = (unsigned long)(card->rd_timeout_ns / 1000000) *
        (host->mclk_rate / 1000);
    mmc_reg += 1000; // add some extra clock cycles to be safe
    mmc_reg = mmc_reg/2;
    writel( mmc_reg, MMC_BOOT_MCI_DATA_TIMER );

    /* Write the total size of the transfer data to MCI_DATA_LENGTH
       register. For block xfer it must be multiple of the block
       size. */
    writel( data_len, MMC_BOOT_MCI_DATA_LENGTH );

    /* For SDHC data address is specified in unit of 512B */
    addr = ( card->type != MMC_BOOT_TYPE_SDHC ) ? (unsigned int) data_addr :
        (unsigned int) (data_addr / 512);

    /* Set appropriate fields and write the MCI_DATA_CTL register. */
    /* Set ENABLE bit to 1 to enable the data transfer. */
    mmc_reg = 0;
    mmc_reg |= MMC_BOOT_MCI_DATA_ENABLE;
    /* Clear DIRECTION bit to 1 to enable transfer from card to host */
    mmc_reg |= MMC_BOOT_MCI_DATA_DIR;
    /* Clear MODE bit to 0 to enable block oriented data transfer. For
       MMC cards only, if stream data transfer mode is desired, set
       MODE bit to 1. */
    /* Set DM_ENABLE bit to 1 in order to enable DMA, otherwise set 0 */
    /* Write size of block to be used during the data transfer to
       BLOCKSIZE field */
    mmc_reg |= (card->rd_block_len << MMC_BOOT_MCI_BLKSIZE_POS);
    writel( mmc_reg, MMC_BOOT_MCI_DATA_CTL );

    /* Send command to the card/device in order to start the read data
       transfer. Possible commands: CMD17/18/53/60/61. */
    mmc_ret = mmc_boot_send_read_command( card, xfer_type, addr );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure sending read command to the Card(RCA:%x)\n",
                mmc_ret, card->rca );
        return mmc_ret;
    }

    read_error = MMC_BOOT_MCI_STAT_DATA_CRC_FAIL | \
                 MMC_BOOT_MCI_STAT_DATA_TIMEOUT  | \
                 MMC_BOOT_MCI_STAT_RX_OVRRUN;

    /* Read the transfer data from SDCC2 FIFO. If Data Mover is not used
       read the data from the MCI_FIFO register as long as RXDATA_AVLBL
       bit of MCI_STATUS register is set to 1 and bits DATA_CRC_FAIL,
       DATA_TIMEOUT, RX_OVERRUN of MCI_STATUS register are cleared to 0.
       Continue the reads until the whole transfer data is received */

    do
    {
        mmc_ret = MMC_BOOT_E_SUCCESS;
        mmc_status = readl( MMC_BOOT_MCI_STATUS );

        if( mmc_status & read_error )
        {
            mmc_ret = mmc_boot_status_error(mmc_status);
            break;
        }

        if( mmc_status & MMC_BOOT_MCI_STAT_RX_DATA_AVLBL )
        {
            unsigned read_count = 1;
            if ( mmc_status & MMC_BOOT_MCI_STAT_RX_FIFO_HFULL)
            {
                read_count = MMC_BOOT_MCI_HFIFO_COUNT;
            }

            for (int i=0; i<read_count; i++)
            {
                /* FIFO contains 16 32-bit data buffer on 16 sequential addresses*/
                *mmc_ptr = readl( MMC_BOOT_MCI_FIFO +
                        ( mmc_count % MMC_BOOT_MCI_FIFO_SIZE ) );
                mmc_ptr++;
                /* increase mmc_count by word size */
                mmc_count += sizeof( unsigned int );
            }
            /* quit if we have read enough of data */
            if (mmc_count == data_len)
                break;
        }
        else if( mmc_status & MMC_BOOT_MCI_STAT_DATA_END )
        {
            break;
        }
    }while(1);

    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure on data transfer from the \
                Card(RCA:%x)\n", mmc_ret, card->rca );
        return mmc_ret;
    }

    /* In case a multiple block transfer was performed, send CMD12 to the
       card/device in order to indicate the end of read data transfer */
    if( xfer_type == MMC_BOOT_XFER_MULTI_BLOCK )
    {
        mmc_ret = mmc_boot_send_stop_transmission( card, 0 );
        if( mmc_ret != MMC_BOOT_E_SUCCESS )
        {
            dprintf(CRITICAL, "Error No.%d: Failure sending Stop Transmission \
                    command to the Card(RCA:%x)\n", mmc_ret, card->rca );
            return mmc_ret;
        }
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Initialize host structure, set and enable clock-rate and power mode.
 */
unsigned int mmc_boot_init( struct mmc_boot_host* host )
{
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
    unsigned int mmc_pwr = 0;


    host->ocr = MMC_BOOT_OCR_27_36 | MMC_BOOT_OCR_SEC_MODE;
    host->cmd_retry = MMC_BOOT_MAX_COMMAND_RETRY;
    host->clk_enabled = 0;

    /* clock frequency should be less than 400KHz in identification mode */
    mmc_ret = mmc_boot_enable_clock( host, MMC_CLK_400KHZ);

    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        return MMC_BOOT_E_CLK_ENABLE_FAIL;
    }

    /* set power mode*/
    /* give some time to reach minimum voltate */
    mdelay(2);
    mmc_pwr &= ~MMC_BOOT_MCI_PWR_UP;
    mmc_pwr |= MMC_BOOT_MCI_PWR_ON;
    mmc_pwr |= MMC_BOOT_MCI_PWR_UP;
    writel( mmc_pwr, MMC_BOOT_MCI_POWER );
    /* some more time to stabilize voltage */
    mdelay(2);

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Performs card identification process by getting card's unique identification
 * number (CID) and relative card address (RCA). After that card will be in
 * stand-by state.
 */
static unsigned int mmc_boot_identify_card( struct mmc_boot_host* host,
        struct mmc_boot_card* card)
{
    unsigned int mmc_return = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( ( host == NULL ) || ( card == NULL ) )
    {
        return MMC_BOOT_E_INVAL;
    }

    /* Ask card to send its unique card identification (CID) number (CMD2) */
    mmc_return = mmc_boot_all_send_cid( card );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No. %d: Failure getting card's CID number!\n",
                mmc_return );
        return mmc_return;
    }

    /* Ask card to send a relative card address (RCA) (CMD3) */
    mmc_return = mmc_boot_send_relative_address( card );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No. %d: Failure getting card's RCA!\n",
                mmc_return );
        return mmc_return;
    }

    /* Set the card status as active */
    card->status = MMC_BOOT_STATUS_ACTIVE;

    /* Get card's CSD register (CMD9) */
    mmc_return = mmc_boot_send_csd( card );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure getting card's CSD information!\n",
                mmc_return );
        return mmc_return;
    }

    /* Once CSD is received, set read and write timeout value now itself */
    mmc_return = mmc_boot_set_read_timeout( host, card );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure setting Read Timeout value!\n",
                mmc_return );
        return mmc_return;
    }

    mmc_return = mmc_boot_set_write_timeout( host, card );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure setting Write Timeout value!\n",
                mmc_return );
        return mmc_return;
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Routine to initialize MMC card. It resets a card to idle state, verify operating
 * voltage and set the card inready state.
 */
static unsigned int mmc_boot_init_card( struct mmc_boot_host* host,
        struct mmc_boot_card* card )
{
    unsigned int mmc_retry = 0;
    unsigned int mmc_return = MMC_BOOT_E_SUCCESS;

    /* basic check */
    if( ( host == NULL ) || ( card == NULL ) )
    {
        return MMC_BOOT_E_INVAL;
    }

    /* 1. Card Reset - not necessary*/
    mmc_return = mmc_boot_reset_cards();
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.:%d: Failure resetting MMC cards!\n", mmc_return);
        return mmc_return;
    }

    /* 2. Card Initialization process */

    /* Send CMD1 to identify and reject cards that do not match host's VDD range
       profile. Cards sends its OCR register in response.
       */
    mmc_retry = 0;
    do
    {
        mmc_return = mmc_boot_send_op_cond( host, card );
        /* Card returns busy status. We'll retry again! */
        if( mmc_return == MMC_BOOT_E_CARD_BUSY )
        {
            mmc_retry++;
            mdelay(200);
            continue;
        }
        else if( mmc_return == MMC_BOOT_E_SUCCESS )
        {
            break;
        }
        else
        {
            dprintf(CRITICAL, "Error No. %d: Failure Initializing MMC Card!\n",
                    mmc_return );
            return mmc_return;
        }
    }while( mmc_retry < host->cmd_retry );

    /* If card still returned busy status we are out of luck.
     * Card cannot be initialized */
    if( mmc_return == MMC_BOOT_E_CARD_BUSY )
    {
        dprintf(CRITICAL, "Error No. %d: Card has busy status set. \
                Initialization not completed\n", mmc_return );
        return MMC_BOOT_E_CARD_BUSY;
    }

    /*Assuming high capacity mmc card*/
    card->type = MMC_BOOT_TYPE_SDHC;

    return MMC_BOOT_E_SUCCESS;
}


/*
 * Performs initialization and identification of all the MMC cards connected
 * to the host.
 */

static unsigned int mmc_boot_init_and_identify_cards( struct mmc_boot_host* host, struct mmc_boot_card* card )
{
    unsigned int mmc_return = MMC_BOOT_E_SUCCESS;

    /* Basic check */
    if( host == NULL )
    {
        return MMC_BOOT_E_INVAL;
    }

    /* Initialize MMC card structure */
    card->status = MMC_BOOT_STATUS_INACTIVE;
    card->rd_block_len = MMC_BOOT_RD_BLOCK_LEN;
    card->wr_block_len = MMC_BOOT_WR_BLOCK_LEN;

    /* Start initialization process (CMD0 & CMD1) */
    mmc_return = mmc_boot_init_card( host, card );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        return mmc_return;
    }

    /* Start card identification process (CMD2, CMD3 & CMD9)*/
    mmc_return = mmc_boot_identify_card( host, card );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        return mmc_return;
    }

    /* Select the card (CMD7) */
    mmc_return = mmc_boot_select_card( card, card->rca );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure selecting the Card with RCA: %x\n",
                mmc_return, card->rca );
        return mmc_return;
    }

    /* set interface speed */
    mmc_return = mmc_boot_adjust_interface_speed( host, card );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Error adjusting interface speed!\n",
                mmc_return );
        return mmc_return;
    }

    /* enable wide bus */
    mmc_return = mmc_boot_set_bus_width( card, MMC_BOOT_BUS_WIDTH_4_BIT );
    if( mmc_return != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL, "Error No.%d: Failure to set wide bus for Card(RCA:%x)\n",
                mmc_return, card->rca );
        return mmc_return;
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * Read MBR from MMC card and fill partition table.
 */
static unsigned int mmc_boot_read_MBR(void)
{
    unsigned char buffer[MMC_BOOT_RD_BLOCK_LEN];
    unsigned int dtype;
    unsigned int dfirstsec;
    unsigned int EBR_first_sec;
    unsigned int EBR_current_sec;
    int ret = 0;
    int idx, i;

    /* Print out the MBR first */
    ret = mmc_boot_read_from_card( &mmc_host, &mmc_card, 0, \
                                   MMC_BOOT_RD_BLOCK_LEN,   \
                                   (unsigned int *)buffer);
    if (ret)
    {
        return ret;
    }

    /* Check to see if signature exists */
    if ((buffer[TABLE_SIGNATURE] != 0x55) || \
        (buffer[TABLE_SIGNATURE + 1] != 0xAA))
    {
        return -1;
    }

    /* Print out the first 4 partition */
    idx = TABLE_ENTRY_0;
    for (i = 0; i < 4; i++)
    {
        mbr[mmc_partition_count].dstatus = \
                    buffer[idx + i * TABLE_ENTRY_SIZE + OFFSET_STATUS];
        mbr[mmc_partition_count].dtype   = \
                    buffer[idx + i * TABLE_ENTRY_SIZE + OFFSET_TYPE];
        mbr[mmc_partition_count].dfirstsec = \
                    GET_LWORD_FROM_BYTE(&buffer[idx + \
                                        i * TABLE_ENTRY_SIZE + \
                                        OFFSET_FIRST_SEC]);
        mbr[mmc_partition_count].dsize  = \
                    GET_LWORD_FROM_BYTE(&buffer[idx + \
                                        i * TABLE_ENTRY_SIZE + \
                                        OFFSET_SIZE]);
        dtype  = mbr[mmc_partition_count].dtype;
        dfirstsec = mbr[mmc_partition_count].dfirstsec;
        mbr_fill_name(&mbr[mmc_partition_count],  \
                      mbr[mmc_partition_count].dtype);
        mmc_partition_count++;
        if (mmc_partition_count == MAX_PARTITIONS)
            return ret;
    }

    /* See if the last partition is EBR, if not, parsing is done */
    if (dtype != 0x05)
    {
        return ret;
    }

    EBR_first_sec = dfirstsec;
    EBR_current_sec = dfirstsec;

    ret = mmc_boot_read_from_card( &mmc_host, &mmc_card,  \
                                   (EBR_first_sec * 512), \
                                   MMC_BOOT_RD_BLOCK_LEN, \
                                   (unsigned int *)buffer);
    if (ret)
    {
        return ret;
    }
    /* Loop to parse the EBR */
    for (i = 0;; i++)
    {
        if ((buffer[TABLE_SIGNATURE] != 0x55) || (buffer[TABLE_SIGNATURE + 1] != 0xAA))
        {
            break;
        }
        mbr[mmc_partition_count].dstatus = \
                    buffer[TABLE_ENTRY_0 + OFFSET_STATUS];
        mbr[mmc_partition_count].dtype   = \
                    buffer[TABLE_ENTRY_0 + OFFSET_TYPE];
        mbr[mmc_partition_count].dfirstsec = \
                    GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_0 + \
                                        OFFSET_FIRST_SEC])    + \
                                        EBR_current_sec;
        mbr[mmc_partition_count].dsize = \
                    GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_0 + \
                                        OFFSET_SIZE]);
        mbr_fill_name(&(mbr[mmc_partition_count]), \
                      mbr[mmc_partition_count].dtype);
        mmc_partition_count++;
        if (mmc_partition_count == MAX_PARTITIONS)
            return ret;

        dfirstsec = GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_1 + OFFSET_FIRST_SEC]);
        if(dfirstsec == 0)
        {
            /* Getting to the end of the EBR tables */
            break;
        }
        /* More EBR to follow - read in the next EBR sector */
        ret = mmc_boot_read_from_card( &mmc_host, &mmc_card, \
                                       ((EBR_first_sec + dfirstsec) * 512), \
                                       MMC_BOOT_RD_BLOCK_LEN, \
                                       (unsigned int *)buffer);
        if (ret)
        {
            return ret;
        }
        EBR_current_sec = EBR_first_sec + dfirstsec;
    }
    return ret;
}

/*
 * Entry point to MMC boot process
 */
unsigned int mmc_boot_main(void)
{
    unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

    memset( (struct mmc_boot_host*)&mmc_host, 0, sizeof( struct mmc_boot_host ) );
    memset( (struct mmc_boot_card*)&mmc_card, 0, sizeof(struct mmc_boot_card) );

#ifndef PLATFORM_MSM8X60
    /* Waiting for modem to come up */
    while (readl(MSM_SHARED_BASE + 0x14) != 1);
#endif
    /* Initialize necessary data structure and enable/set clock and power */
    dprintf(INFO," Initializing MMC host data structure and clock!\n" );
    mmc_ret = mmc_boot_init( &mmc_host );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL,  "MMC Boot: Error Initializing MMC Card!!!\n" );
        return MMC_BOOT_E_FAILURE;
    }

    /* Initialize and identify cards connected to host */
    mmc_ret = mmc_boot_init_and_identify_cards( &mmc_host, &mmc_card );
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL,  "MMC Boot: Failure detecting MMC card!!!\n" );
        return MMC_BOOT_E_FAILURE;
    }

    /* Read MBR of the card */
    mmc_ret = mmc_boot_read_MBR();
    if( mmc_ret != MMC_BOOT_E_SUCCESS )
    {
        dprintf(CRITICAL,  "MMC Boot: MBR read failed!\n" );
        return MMC_BOOT_E_FAILURE;
    }

    return MMC_BOOT_E_SUCCESS;
}

/*
 * MMC write function
 */
unsigned int mmc_write (unsigned long long data_addr, unsigned int data_len, unsigned int* in)
{
    int val = 0;
    unsigned int write_size = ((unsigned)(0xFFFFFF/512))*512;
    unsigned offset = 0;
    unsigned int *sptr = in;
    while(data_len > write_size)
    {
        val = mmc_boot_write_to_card( &mmc_host, &mmc_card, \
                                      data_addr + offset, \
                                      write_size, sptr);
        if(val)
        {
            return val;
        }

        sptr += (write_size/sizeof(unsigned));
        offset += write_size;
        data_len -= write_size;
    }
    if (data_len)
    {
        val = mmc_boot_write_to_card( &mmc_host, &mmc_card, \
                                      data_addr + offset, \
                                      data_len, sptr);
    }
    return val;
}

/*
 * MMC read function
 */
unsigned int mmc_read (unsigned long long data_addr, unsigned int* out, unsigned int data_len)
{
    int val = 0;
    val = mmc_boot_read_from_card( &mmc_host, &mmc_card, data_addr, data_len, out);
    return val;
}

/*
 * Fill name for android partition found.
 */
static void mbr_fill_name (struct mbr_entry *mbr_ent, unsigned int type)
{
    switch(type)
    {
        memset(mbr_ent->name, 0, 64);
        case MMC_MODEM_TYPE:
        case MMC_MODEM_TYPE2:
            /* if there are more than one with type "modem", mmc_ptn_offset will return the first one */
            memcpy(mbr_ent->name,"modem",5);
            break;
        case MMC_BOOT_TYPE:
            memcpy(mbr_ent->name,"boot",4);
            break;
        case MMC_USERDATA_TYPE:
            strcpy((char *)mbr_ent->name,(const char *)ext3_partitions[ext3_count]);
            ext3_count++;
            break;
    };
}

/*
 * Returns offset of given partition
 */
unsigned long long mmc_ptn_offset (unsigned char * name)
{
    unsigned n;
    for(n = 0; n < mmc_partition_count; n++) {
        if(!strcmp((const char *)mbr[n].name, (const char *)name)) {
            return (mbr[n].dfirstsec * MMC_BOOT_RD_BLOCK_LEN);
        }
    }
    return 0;
}

