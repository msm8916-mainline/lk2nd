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

#include <stdlib.h>
#include <stdint.h>
#include <mmc_sdhci.h>
#include <mmc_wrapper.h>
#include <sdhci.h>
#include <target.h>

/*
 * Function: get mmc card
 * Arg     : None
 * Return  : Pointer to mmc card structure
 * Flow    : Get the card pointer from the device structure
 */
static struct mmc_card *get_mmc_card()
{
	struct mmc_device *dev;
	struct mmc_card *card;

	dev = target_mmc_device();
	card = &dev->card;

	return card;
}

/*
 * Function: mmc_write
 * Arg     : Data address on card, data length, i/p buffer
 * Return  : 0 on Success, non zero on failure
 * Flow    : Write the data from in to the card
 */
uint32_t mmc_write(uint64_t data_addr, uint32_t data_len, void *in)
{
	uint32_t val = 0;
	uint32_t write_size = SDHCI_ADMA_MAX_TRANS_SZ;
	uint8_t *sptr = (uint8_t *)in;
	struct mmc_device *dev;

	dev = target_mmc_device();

	ASSERT(!(data_addr % MMC_BLK_SZ));

	if (data_len % MMC_BLK_SZ)
		data_len = ROUNDUP(data_len, MMC_BLK_SZ);

	/* TODO: This function is aware of max data that can be
	 * tranferred using sdhci adma mode, need to have a cleaner
	 * implementation to keep this function independent of sdhci
	 * limitations
	 */
	while (data_len > write_size) {
		val = mmc_sdhci_write(dev, (void *)sptr, (data_addr / MMC_BLK_SZ), (write_size / MMC_BLK_SZ));
		if (val)
		{
			dprintf(CRITICAL, "Failed Writing block @ %x\n", (data_addr / MMC_BLK_SZ));
			return val;
		}
		sptr += write_size;
		data_addr += write_size;
		data_len -= write_size;
	}

	if (data_len)
		val = mmc_sdhci_write(dev, (void *)sptr, (data_addr / MMC_BLK_SZ), (data_len / MMC_BLK_SZ));

	if (val)
		dprintf(CRITICAL, "Failed Writing block @ %x\n", (data_addr / MMC_BLK_SZ));

	return val;
}

/*
 * Function: mmc_read
 * Arg     : Data address on card, o/p buffer & data length
 * Return  : 0 on Success, non zero on failure
 * Flow    : Read data from the card to out
 */
uint32_t mmc_read(uint64_t data_addr, uint32_t *out, uint32_t data_len)
{
	uint32_t ret = 0;
	uint32_t read_size = SDHCI_ADMA_MAX_TRANS_SZ;
	struct mmc_device *dev;
	uint8_t *sptr = (uint8_t *)out;

	ASSERT(!(data_addr % MMC_BLK_SZ));
	ASSERT(!(data_len % MMC_BLK_SZ));

	dev = target_mmc_device();

	/* TODO: This function is aware of max data that can be
	 * tranferred using sdhci adma mode, need to have a cleaner
	 * implementation to keep this function independent of sdhci
	 * limitations
	 */
	while (data_len > read_size) {
		ret = mmc_sdhci_read(dev, (void *)sptr, (data_addr / MMC_BLK_SZ), (read_size / MMC_BLK_SZ));
		if (ret)
		{
			dprintf(CRITICAL, "Failed Reading block @ %x\n", (data_addr / MMC_BLK_SZ));
			return ret;
		}
		sptr += read_size;
		data_addr += read_size;
		data_len -= read_size;
	}

	if (data_len)
		ret = mmc_sdhci_read(dev, (void *)sptr, (data_addr / MMC_BLK_SZ), (data_len / MMC_BLK_SZ));

	if (ret)
		dprintf(CRITICAL, "Failed Reading block @ %x\n", (data_addr / MMC_BLK_SZ));

	return ret;
}

/*
 * Function: mmc erase card
 * Arg     : Block address & length
 * Return  : Returns 0
 * Flow    : Erase the card from specified addr
 */
uint32_t mmc_erase_card(uint64_t addr, uint64_t len)
{
	struct mmc_device *dev;

	dev = target_mmc_device();

	ASSERT(!(addr % MMC_BLK_SZ));
	ASSERT(!(len % MMC_BLK_SZ));

	if (mmc_sdhci_erase(dev, (addr / MMC_BLK_SZ), len))
	{
		dprintf(CRITICAL, "MMC erase failed\n");
		return 1;
	}
	return 0;
}

/*
 * Function: mmc get psn
 * Arg     : None
 * Return  : Returns the product serial number
 * Flow    : Get the PSN from card
 */
uint32_t mmc_get_psn(void)
{
	struct mmc_card *card;

	card = get_mmc_card();

	return card->cid.psn;
}

/*
 * Function: mmc get capacity
 * Arg     : None
 * Return  : Returns the density of the emmc card
 * Flow    : Get the density from card
 */
uint64_t mmc_get_device_capacity()
{
	struct mmc_card *card;

	card = get_mmc_card();

	return card->capacity;
}

