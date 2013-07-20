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

#include <platform/iomap.h>
#include <platform/irqs.h>
#include <platform/interrupts.h>
#include <platform/timer.h>
#include <target.h>
#include <string.h>
#include <stdlib.h>
#include <bits.h>
#include <debug.h>
#include <sdhci.h>
#include <sdhci_msm.h>


/*
 * Function: sdhci int handler
 * Arg     : MSM specific data for sdhci
 * Return  : 0
 * Flow:   : 1. Read the power control mask register
 *           2. Check if bus is ON
 *           3. Write success to ack regiser
 * Details : This is power control interrupt handler.
 *           Once we receive the interrupt, we will ack the power control
 *           register that we have successfully completed pmic transactions
 */
static enum handler_return sdhci_int_handler(struct sdhci_msm_data *data)
{
	uint32_t ack;
	uint32_t status;

	/*
	 * Read the mask register to check if BUS & IO level
	 * interrupts are enabled
	 */
	status = readl(data->pwrctl_base + SDCC_HC_PWRCTL_MASK_REG);

	if (status & (SDCC_HC_BUS_ON | SDCC_HC_BUS_OFF))
		ack = SDCC_HC_BUS_ON_OFF_SUCC;
	if (status & (SDCC_HC_IO_SIG_LOW | SDCC_HC_IO_SIG_HIGH))
		ack |= SDCC_HC_IO_SIG_SUCC;

	/* Write success to power control register */
	writel(ack, (data->pwrctl_base + SDCC_HC_PWRCTL_CTL_REG));

	event_signal(data->sdhc_event, false);

	return 0;
}

/*
 * Function: sdhci clear pending interrupts
 * Arg     : MSM specific data for sdhci
 * Return  : None
 * Flow:   : Clear pending interrupts
 */
static void sdhci_clear_power_ctrl_irq(struct sdhci_msm_data *data)
{
	uint32_t irq_ctl;
	uint32_t irq_stat;

	/*
	 * Read the power control status register to know
	 * the status of BUS & IO_HIGH_V
	 */
	irq_stat = readl(data->pwrctl_base + SDCC_HC_PWRCTL_STATUS_REG);

	/* Clear the power control status */
	writel(irq_stat, (data->pwrctl_base + SDCC_HC_PWRCTL_CLEAR_REG));

	/*
	 * Handle the pending irq by ack'ing the bus & IO switch
	 */
	irq_ctl = readl(data->pwrctl_base + SDCC_HC_PWRCTL_CTL_REG);

	if (irq_stat & (SDCC_HC_BUS_ON | SDCC_HC_BUS_OFF))
		irq_ctl |= SDCC_HC_BUS_ON_OFF_SUCC;
	if (irq_stat & (SDCC_HC_IO_SIG_LOW | SDCC_HC_IO_SIG_HIGH))
		irq_ctl |= SDCC_HC_IO_SIG_SUCC;

	writel(irq_ctl, (data->pwrctl_base + SDCC_HC_PWRCTL_CTL_REG));
}

/*
 * Function: sdhci msm init
 * Arg     : MSM specific config data for sdhci
 * Return  : None
 * Flow:   : Enable sdhci mode & do msm specific init
 */
void sdhci_msm_init(struct sdhci_msm_data *config)
{
	/* Enable sdhc mode */
	writel(SDHCI_HC_MODE_EN, (config->pwrctl_base + SDCC_MCI_HC_MODE));

	/*
	 * CORE_SW_RST may trigger power irq if previous status of PWRCTL
	 * was either BUS_ON or IO_HIGH. So before we enable the power irq
	 * interrupt in GIC (by registering the interrupt handler), we need to
	 * ensure that any pending power irq interrupt status is acknowledged
	 * otherwise power irq interrupt handler would be fired prematurely.
	 */
	sdhci_clear_power_ctrl_irq(config);

	/*
	 * Register the interrupt handler for pwr irq
	 */
	register_int_handler(config->pwr_irq, sdhci_int_handler, (void *)config);

	unmask_interrupt(config->pwr_irq);

	/* Enable pwr control interrupt */
	writel(SDCC_HC_PWR_CTRL_INT, (config->pwrctl_base + SDCC_HC_PWRCTL_MASK_REG));
}
