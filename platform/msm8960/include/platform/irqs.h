/* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __IRQS_8960_H
#define __IRQS_8960_H

/* MSM ACPU Interrupt Numbers */

/* 0-15:  STI/SGI (software triggered/generated interrupts)
 * 16-31: PPI (private peripheral interrupts)
 * 32+:   SPI (shared peripheral interrupts)
 */

#define GIC_PPI_START 16
#define GIC_SPI_START 32

#define INT_DEBUG_TIMER_EXP         (GIC_PPI_START + 1)

#define USB1_HS_BAM_IRQ             (GIC_SPI_START + 94)
#define USB1_HS_IRQ                 (GIC_SPI_START + 100)
#define USB2_IRQ                    (GIC_SPI_START + 141)
#define USB1_IRQ                    (GIC_SPI_START + 142)
#define GSBI1_QUP_IRQ               (GIC_SPI_START + 147)
#define GSBI2_UART_IRQ              (GIC_SPI_START + 148)
#define GSBI2_QUP_IRQ               (GIC_SPI_START + 149)
#define GSBI3_UART_IRQ              (GIC_SPI_START + 150)
#define GSBI3_QUP_IRQ               (GIC_SPI_START + 151)
#define GSBI4_UART_IRQ              (GIC_SPI_START + 152)
#define GSBI4_QUP_IRQ               (GIC_SPI_START + 153)
#define GSBI5_UART_IRQ              (GIC_SPI_START + 154)
#define GSBI5_QUP_IRQ               (GIC_SPI_START + 155)
#define GSBI6_UART_IRQ              (GIC_SPI_START + 156)
#define GSBI6_QUP_IRQ               (GIC_SPI_START + 157)
#define GSBI7_UART_IRQ              (GIC_SPI_START + 158)
#define GSBI7_QUP_IRQ               (GIC_SPI_START + 159)
#define GSBI8_UART_IRQ              (GIC_SPI_START + 160)
#define GSBI8_QUP_IRQ               (GIC_SPI_START + 161)
#define GSBI9_UART_IRQ              (GIC_SPI_START + 189)
#define GSBI9_QUP_IRQ               (GIC_SPI_START + 190)
#define GSBI10_UART_IRQ             (GIC_SPI_START + 191)
#define GSBI10_QUP_IRQ              (GIC_SPI_START + 192)
#define GSBI11_UART_IRQ             (GIC_SPI_START + 193)
#define GSBI11_QUP_IRQ              (GIC_SPI_START + 194)
#define GSBI12_UART_IRQ             (GIC_SPI_START + 195)
#define GSBI12_QUP_IRQ              (GIC_SPI_START + 196)

/* Retrofit universal macro names */
#define INT_USB_HS                  USB1_HS_IRQ

#define NR_MSM_IRQS                 256
#define NR_GPIO_IRQS                173
#define NR_BOARD_IRQS               0

#define NR_IRQS (NR_MSM_IRQS + NR_GPIO_IRQS + NR_BOARD_IRQS)

#endif /* __IRQS_8960_H */
