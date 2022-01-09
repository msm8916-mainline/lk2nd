/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

#ifndef __IRQS_MSM8916_H
#define __IRQS_MSM8916_H

/* MSM ACPU Interrupt Numbers */

/* 0-15:  STI/SGI (software triggered/generated interrupts)
 * 16-31: PPI (private peripheral interrupts)
 * 32+:   SPI (shared peripheral interrupts)
 */
int qtmr_irq();

#define GIC_PPI_START                          16
#define GIC_SPI_START                          32

#define INT_QTMR_NON_SECURE_PHY_TIMER_EXP      (GIC_PPI_START + 3)
#define INT_QTMR_VIRTUAL_TIMER_EXP             (GIC_PPI_START + 4)

#define INT_QTMR_FRM_0_PHYSICAL_TIMER_EXP      qtmr_irq()
#define INT_QTMR_FRM_0_PHYSICAL_TIMER_EXP_8x16 (GIC_SPI_START + 8)
#define INT_QTMR_FRM_0_PHYSICAL_TIMER_EXP_8x39 (GIC_SPI_START + 257)
#define SDCC1_PWRCTL_IRQ                       (GIC_SPI_START + 138)
#define SDCC2_PWRCTL_IRQ                       (GIC_SPI_START + 221)

#define USB1_HS_BAM_IRQ                        (GIC_SPI_START + 135)
#define USB1_HS_IRQ                            (GIC_SPI_START + 134)

#define SMD_IRQ                                (GIC_SPI_START + 168)

/* Retrofit universal macro names */
#define INT_USB_HS                             USB1_HS_IRQ

#define EE0_KRAIT_HLOS_SPMI_PERIPH_IRQ         (GIC_SPI_START + 190)

#define NR_MSM_IRQS                            256
#define NR_GPIO_IRQS                           173
#define NR_BOARD_IRQS                          0

#define NR_IRQS                                (NR_MSM_IRQS + NR_GPIO_IRQS + \
                                               NR_BOARD_IRQS)

#define BLSP_QUP_IRQ(blsp_id, qup_id)          (GIC_SPI_START + 95 + qup_id)
#endif /* __IRQS_MSM8916_H */
