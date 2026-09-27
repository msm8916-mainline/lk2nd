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
#ifndef _USB30_DWC_HWIO_H_
#define _USB30_DWC_HWIO_H_

/* Macros to simplify dwc reg read */
#define REG_READ(_dev, _reg)                    readl(DWC_##_reg##_ADDR(_dev->base))
#define REG_READI(_dev, _reg, _index)           readl(DWC_##_reg##_ADDR(_dev->base, _index))

/* Macros to simplify dwc reg write */
#define REG_WRITE(_dev, _reg, _value)           writel(_value, DWC_##_reg##_ADDR(_dev->base))
#define REG_WRITEI(_dev, _reg, _index, _value)  writel(_value, DWC_##_reg##_ADDR(_dev->base, _index))

#define REG_BMSK(_reg, _field)                      DWC_##_reg##_##_field##_BMSK
#define REG_SHFT(_reg, _field)                      DWC_##_reg##_##_field##_SHFT

/* read specified field in the register */
#define REG_READ_FIELD(_dev, _reg, _field)          ((REG_READ(_dev,_reg) & REG_BMSK(_reg, _field)) >> REG_SHFT(_reg, _field))
#define REG_READ_FIELDI(_dev, _reg, _index, _field) ((REG_READI(_dev,_reg, _index) & REG_BMSK(_reg, _field)) >> REG_SHFT(_reg, _field))
#define REG_READ_FIELD_LOCAL(_addr, _reg, _field)   (((*_addr) & REG_BMSK(_reg, _field)) >> REG_SHFT(_reg, _field))


/* write specified field in the register: implements a read/modify/write */
#define REG_WRITE_FIELD(_dev, _reg, _field, _value)          \
			REG_WRITE(_dev, _reg, ((REG_READ(_dev, _reg) & ~REG_BMSK(_reg, _field)) | (_value << REG_SHFT(_reg, _field))))

#define REG_WRITE_FIELDI(_dev, _reg, _index, _field, _value) \
			REG_WRITEI(_dev, _reg, _index, ((REG_READI(_dev, _reg, _index) & ~REG_BMSK(_reg, _field)) | (_value << REG_SHFT(_reg, _field))))

#define REG_WRITE_FIELD_LOCAL(_addr, _reg, _field, _value)   \
			(*(_addr) = ((*(_addr) & ~REG_BMSK(_reg, _field)) | (_value << REG_SHFT(_reg, _field))))



/* The following defines are auto generated. */

/**
  @file usb30_dwc_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    DWC_USB3

  'Include' filters applied:
  'Exclude' filters applied: RESERVED DUMMY
*/
/*----------------------------------------------------------------------------
 * MODULE: DWC_USB3
 *--------------------------------------------------------------------------*/
#define DWC_USB3_REG_BASE_OFFS                                0x00000000
#define DWC_GUSB2PHYCFG_ADDR(base,p)                          ((base) + 0x0000c200 + 0x4 * (p))
#define DWC_GUSB2PHYCFG_OFFS(p)                               (0x0000c200 + 0x4 * (p))
#define DWC_GUSB2PHYCFG_RMSK                                  0xffffffff
#define DWC_GUSB2PHYCFG_MAXp                                           0
#define DWC_GUSB2PHYCFG_POR                                   0x00002500
#define DWC_GUSB2PHYCFG_PHYSOFTRST_BMSK                       0x80000000
#define DWC_GUSB2PHYCFG_PHYSOFTRST_SHFT                             0x1f
#define DWC_GUSB2PHYCFG_RSVD_BMSK                             0x7ff80000
#define DWC_GUSB2PHYCFG_RSVD_SHFT                                   0x13
#define DWC_GUSB2PHYCFG_ULPIEXTVBUSINDIACTOR_BMSK                0x40000
#define DWC_GUSB2PHYCFG_ULPIEXTVBUSINDIACTOR_SHFT                   0x12
#define DWC_GUSB2PHYCFG_ULPIEXTVBUSDRV_BMSK                      0x20000
#define DWC_GUSB2PHYCFG_ULPIEXTVBUSDRV_SHFT                         0x11
#define DWC_GUSB2PHYCFG_ULPICLKSUSM_BMSK                         0x10000
#define DWC_GUSB2PHYCFG_ULPICLKSUSM_SHFT                            0x10
#define DWC_GUSB2PHYCFG_ULPIAUTORES_BMSK                          0x8000
#define DWC_GUSB2PHYCFG_ULPIAUTORES_SHFT                             0xf
#define DWC_GUSB2PHYCFG_RSVD9_BMSK                                0x4000
#define DWC_GUSB2PHYCFG_RSVD9_SHFT                                   0xe
#define DWC_GUSB2PHYCFG_USBTRDTIM_BMSK                            0x3c00
#define DWC_GUSB2PHYCFG_USBTRDTIM_SHFT                               0xa
#define DWC_GUSB2PHYCFG_RSVD0_BMSK                                 0x200
#define DWC_GUSB2PHYCFG_RSVD0_SHFT                                   0x9
#define DWC_GUSB2PHYCFG_ENBLSLPM_BMSK                              0x100
#define DWC_GUSB2PHYCFG_ENBLSLPM_SHFT                                0x8
#define DWC_GUSB2PHYCFG_PHYSEL_BMSK                                 0x80
#define DWC_GUSB2PHYCFG_PHYSEL_SHFT                                  0x7
#define DWC_GUSB2PHYCFG_SUSPENDUSB20_BMSK                           0x40
#define DWC_GUSB2PHYCFG_SUSPENDUSB20_SHFT                            0x6
#define DWC_GUSB2PHYCFG_FSINTF_BMSK                                 0x20
#define DWC_GUSB2PHYCFG_FSINTF_SHFT                                  0x5
#define DWC_GUSB2PHYCFG_ULPI_UTMI_SEL_BMSK                          0x10
#define DWC_GUSB2PHYCFG_ULPI_UTMI_SEL_SHFT                           0x4
#define DWC_GUSB2PHYCFG_PHYIF_BMSK                                   0x8
#define DWC_GUSB2PHYCFG_PHYIF_SHFT                                   0x3
#define DWC_GUSB2PHYCFG_B1L_BMSK                                     0x7
#define DWC_GUSB2PHYCFG_B1L_SHFT                                     0x0

#define DWC_GUSB2I2CCTL_ADDR(base,p)                          ((base) + 0x0000c240 + 0x4 * (p))
#define DWC_GUSB2I2CCTL_OFFS(p)                               (0x0000c240 + 0x4 * (p))
#define DWC_GUSB2I2CCTL_RMSK                                  0xffffffff
#define DWC_GUSB2I2CCTL_MAXp                                           0
#define DWC_GUSB2I2CCTL_POR                                   0x00000000
#define DWC_GUSB2I2CCTL_RSVD_BMSK                             0x80000000
#define DWC_GUSB2I2CCTL_RSVD_SHFT                                   0x1f
#define DWC_GUSB2I2CCTL_BSYDNE_BMSK                           0x40000000
#define DWC_GUSB2I2CCTL_BSYDNE_SHFT                                 0x1e
#define DWC_GUSB2I2CCTL_ACK_BMSK                              0x20000000
#define DWC_GUSB2I2CCTL_ACK_SHFT                                    0x1d
#define DWC_GUSB2I2CCTL_RW_BMSK                               0x10000000
#define DWC_GUSB2I2CCTL_RW_SHFT                                     0x1c
#define DWC_GUSB2I2CCTL_I2CDATSE0_BMSK                         0x8000000
#define DWC_GUSB2I2CCTL_I2CDATSE0_SHFT                              0x1b
#define DWC_GUSB2I2CCTL_I2CDEVADR_BMSK                         0x6000000
#define DWC_GUSB2I2CCTL_I2CDEVADR_SHFT                              0x19
#define DWC_GUSB2I2CCTL_I2CSUSPCTL_BMSK                        0x1000000
#define DWC_GUSB2I2CCTL_I2CSUSPCTL_SHFT                             0x18
#define DWC_GUSB2I2CCTL_I2CEN_BMSK                              0x800000
#define DWC_GUSB2I2CCTL_I2CEN_SHFT                                  0x17
#define DWC_GUSB2I2CCTL_ADDR_BMSK                               0x7f0000
#define DWC_GUSB2I2CCTL_ADDR_SHFT                                   0x10
#define DWC_GUSB2I2CCTL_REGADDR_BMSK                              0xff00
#define DWC_GUSB2I2CCTL_REGADDR_SHFT                                 0x8
#define DWC_GUSB2I2CCTL_RWDATA_BMSK                                 0xff
#define DWC_GUSB2I2CCTL_RWDATA_SHFT                                  0x0

#define DWC_GUSB2PHYACC_ULPI_ADDR(base,p)                     ((base) + 0x0000c280 + 0x4 * (p))
#define DWC_GUSB2PHYACC_ULPI_OFFS(p)                          (0x0000c280 + 0x4 * (p))
#define DWC_GUSB2PHYACC_ULPI_RMSK                             0xffff1fff
#define DWC_GUSB2PHYACC_ULPI_MAXp                                      0
#define DWC_GUSB2PHYACC_ULPI_POR                              0x00000000
#define DWC_GUSB2PHYACC_ULPI_RSVD42_BMSK                      0xf8000000
#define DWC_GUSB2PHYACC_ULPI_RSVD42_SHFT                            0x1b
#define DWC_GUSB2PHYACC_ULPI_DISUIPIDRVR_BMSK                  0x4000000
#define DWC_GUSB2PHYACC_ULPI_DISUIPIDRVR_SHFT                       0x1a
#define DWC_GUSB2PHYACC_ULPI_NEWREGREQ_BMSK                    0x2000000
#define DWC_GUSB2PHYACC_ULPI_NEWREGREQ_SHFT                         0x19
#define DWC_GUSB2PHYACC_ULPI_VSTSDONE_BMSK                     0x1000000
#define DWC_GUSB2PHYACC_ULPI_VSTSDONE_SHFT                          0x18
#define DWC_GUSB2PHYACC_ULPI_VSTSBSY_BMSK                       0x800000
#define DWC_GUSB2PHYACC_ULPI_VSTSBSY_SHFT                           0x17
#define DWC_GUSB2PHYACC_ULPI_REGWR_BMSK                         0x400000
#define DWC_GUSB2PHYACC_ULPI_REGWR_SHFT                             0x16
#define DWC_GUSB2PHYACC_ULPI_REGADDR_BMSK                       0x3f0000
#define DWC_GUSB2PHYACC_ULPI_REGADDR_SHFT                           0x10
#define DWC_GUSB2PHYACC_ULPI_EXTREGADDR_BMSK                      0x1f00
#define DWC_GUSB2PHYACC_ULPI_EXTREGADDR_SHFT                         0x8
#define DWC_GUSB2PHYACC_ULPI_REGDATA_BMSK                           0xff
#define DWC_GUSB2PHYACC_ULPI_REGDATA_SHFT                            0x0

#define DWC_GUSB3PIPECTL_ADDR(base,p)                        ((base) + 0x0000c2c0 + 0x4 * (p))
#define DWC_GUSB3PIPECTL_OFFS(p)                        (0x0000c2c0 + 0x4 * (p))
#define DWC_GUSB3PIPECTL_RMSK                                0xffffffff
#define DWC_GUSB3PIPECTL_MAXp                                         0
#define DWC_GUSB3PIPECTL_POR                                 0x034c0003
#define DWC_GUSB3PIPECTL_PHYSOFTRST_BMSK                     0x80000000
#define DWC_GUSB3PIPECTL_PHYSOFTRST_SHFT                           0x1f
#define DWC_GUSB3PIPECTL_RSVD8_BMSK                          0x40000000
#define DWC_GUSB3PIPECTL_RSVD8_SHFT                                0x1e
#define DWC_GUSB3PIPECTL_U2SSINACTP3OK_BMSK                  0x20000000
#define DWC_GUSB3PIPECTL_U2SSINACTP3OK_SHFT                        0x1d
#define DWC_GUSB3PIPECTL_DISRXDETP3_BMSK                     0x10000000
#define DWC_GUSB3PIPECTL_DISRXDETP3_SHFT                           0x1c
#define DWC_GUSB3PIPECTL_UX_EXIT_IN_PX_BMSK                   0x8000000
#define DWC_GUSB3PIPECTL_UX_EXIT_IN_PX_SHFT                        0x1b
#define DWC_GUSB3PIPECTL_PING_ENHANCEMENT_EN_BMSK             0x4000000
#define DWC_GUSB3PIPECTL_PING_ENHANCEMENT_EN_SHFT                  0x1a
#define DWC_GUSB3PIPECTL_U1U2EXITFAIL_TO_RECOV_BMSK           0x2000000
#define DWC_GUSB3PIPECTL_U1U2EXITFAIL_TO_RECOV_SHFT                0x19
#define DWC_GUSB3PIPECTL_REQUEST_P1P2P3_BMSK                  0x1000000
#define DWC_GUSB3PIPECTL_REQUEST_P1P2P3_SHFT                       0x18
#define DWC_GUSB3PIPECTL_STARTRXDETU3RXDET_BMSK                0x800000
#define DWC_GUSB3PIPECTL_STARTRXDETU3RXDET_SHFT                    0x17
#define DWC_GUSB3PIPECTL_DISRXDETU3RXDET_BMSK                  0x400000
#define DWC_GUSB3PIPECTL_DISRXDETU3RXDET_SHFT                      0x16
#define DWC_GUSB3PIPECTL_GUSBPIPECTL_BUS_BMSK                  0x200000
#define DWC_GUSB3PIPECTL_GUSBPIPECTL_BUS_SHFT                      0x15
#define DWC_GUSB3PIPECTL_DELAYP1P2P3_BMSK                      0x180000
#define DWC_GUSB3PIPECTL_DELAYP1P2P3_SHFT                          0x13
#define DWC_GUSB3PIPECTL_DELAYP1TRANS_BMSK                      0x40000
#define DWC_GUSB3PIPECTL_DELAYP1TRANS_SHFT                         0x12
#define DWC_GUSB3PIPECTL_SUSPENDENABLE_BMSK                     0x20000
#define DWC_GUSB3PIPECTL_SUSPENDENABLE_SHFT                        0x11
#define DWC_GUSB3PIPECTL_DATWIDTH_BMSK                          0x18000
#define DWC_GUSB3PIPECTL_DATWIDTH_SHFT                              0xf
#define DWC_GUSB3PIPECTL_ABORTRXDETINU2_BMSK                     0x4000
#define DWC_GUSB3PIPECTL_ABORTRXDETINU2_SHFT                        0xe
#define DWC_GUSB3PIPECTL_SKIPRXDET_BMSK                          0x2000
#define DWC_GUSB3PIPECTL_SKIPRXDET_SHFT                             0xd
#define DWC_GUSB3PIPECTL_LFPSP0ALGN_BMSK                         0x1000
#define DWC_GUSB3PIPECTL_LFPSP0ALGN_SHFT                            0xc
#define DWC_GUSB3PIPECTL_P3P2TRANOK_BMSK                          0x800
#define DWC_GUSB3PIPECTL_P3P2TRANOK_SHFT                            0xb
#define DWC_GUSB3PIPECTL_P3EXSIGP2_BMSK                           0x400
#define DWC_GUSB3PIPECTL_P3EXSIGP2_SHFT                             0xa
#define DWC_GUSB3PIPECTL_LFPSFILTER_BMSK                          0x200
#define DWC_GUSB3PIPECTL_LFPSFILTER_SHFT                            0x9
#define DWC_GUSB3PIPECTL_PRTOPDIR_BMSK                            0x180
#define DWC_GUSB3PIPECTL_PRTOPDIR_SHFT                              0x7
#define DWC_GUSB3PIPECTL_TX_SWING_BMSK                             0x40
#define DWC_GUSB3PIPECTL_TX_SWING_SHFT                              0x6
#define DWC_GUSB3PIPECTL_TX_MARGIN_BMSK                            0x38
#define DWC_GUSB3PIPECTL_TX_MARGIN_SHFT                             0x3
#define DWC_GUSB3PIPECTL_TX_DE_EPPHASIS_BMSK                        0x6
#define DWC_GUSB3PIPECTL_TX_DE_EPPHASIS_SHFT                        0x1
#define DWC_GUSB3PIPECTL_ELASTIC_BUFFER_MODE_BMSK                   0x1
#define DWC_GUSB3PIPECTL_ELASTIC_BUFFER_MODE_SHFT                   0x0

#define DWC_GTXFIFOSIZ_ADDR(base,p)                            ((base) + 0x0000c300 + 0x4 * (p))
#define DWC_GTXFIFOSIZ_OFFS(p)                                 (0x0000c300 + 0x4 * (p))
#define DWC_GTXFIFOSIZ_RMSK                                    0xffffffff
#define DWC_GTXFIFOSIZ_MAXp                                            15
#define DWC_GTXFIFOSIZ_POR                                     0x00000000
#define DWC_GTXFIFOSIZ_TXFSTADDR_N_BMSK                        0xffff0000
#define DWC_GTXFIFOSIZ_TXFSTADDR_N_SHFT                              0x10
#define DWC_GTXFIFOSIZ_TXFDEP_N_BMSK                               0xffff
#define DWC_GTXFIFOSIZ_TXFDEP_N_SHFT                                  0x0

#define DWC_GRXFIFOSIZ_ADDR(base,p)                            ((base) + 0x0000c380 + 0x4 * (p))
#define DWC_GRXFIFOSIZ_OFFS(p)                                 (0x0000c380 + 0x4 * (p))
#define DWC_GRXFIFOSIZ_RMSK                                    0xffffffff
#define DWC_GRXFIFOSIZ_MAXp                                             2
#define DWC_GRXFIFOSIZ_POR                                     0x00000000
#define DWC_GRXFIFOSIZ_RXFSTADDR_N_BMSK                        0xffff0000
#define DWC_GRXFIFOSIZ_RXFSTADDR_N_SHFT                              0x10
#define DWC_GRXFIFOSIZ_RXFDEP_N_BMSK                               0xffff
#define DWC_GRXFIFOSIZ_RXFDEP_N_SHFT                                  0x0

#define DWC_GEVNTADRLO_ADDR(base,p)                            ((base) + 0x0000c400 + 0x10 * (p))
#define DWC_GEVNTADRLO_OFFS(p)                                 (0x0000c400 + 0x10 * (p))
#define DWC_GEVNTADRLO_RMSK                                    0xffffffff
#define DWC_GEVNTADRLO_MAXp                                             1
#define DWC_GEVNTADRLO_POR                                     0x00000000
#define DWC_GEVNTADRLO_EVNTADRLO_BMSK                          0xffffffff
#define DWC_GEVNTADRLO_EVNTADRLO_SHFT                                 0x0

#define DWC_GEVNTADRHI_ADDR(base,p)                            ((base) + 0x0000c404 + 0x10 * (p))
#define DWC_GEVNTADRHI_OFFS(p)                                 (0x0000c404 + 0x10 * (p))
#define DWC_GEVNTADRHI_RMSK                                    0xffffffff
#define DWC_GEVNTADRHI_MAXp                                             1
#define DWC_GEVNTADRHI_POR                                     0x00000000
#define DWC_GEVNTADRHI_EVNTADRHI_BMSK                          0xffffffff
#define DWC_GEVNTADRHI_EVNTADRHI_SHFT                                 0x0

#define DWC_GEVNTSIZ_ADDR(base,p)                              ((base) + 0x0000c408 + 0x10 * (p))
#define DWC_GEVNTSIZ_OFFS(p)                                   (0x0000c408 + 0x10 * (p))
#define DWC_GEVNTSIZ_RMSK                                      0xffffffff
#define DWC_GEVNTSIZ_MAXp                                               1
#define DWC_GEVNTSIZ_POR                                       0x00000000
#define DWC_GEVNTSIZ_EVNTINTRPTMASK_BMSK                       0x80000000
#define DWC_GEVNTSIZ_EVNTINTRPTMASK_SHFT                             0x1f
#define DWC_GEVNTSIZ_RSVD74_BMSK                               0x7fff0000
#define DWC_GEVNTSIZ_RSVD74_SHFT                                     0x10
#define DWC_GEVNTSIZ_EVENTSIZ_BMSK                                 0xffff
#define DWC_GEVNTSIZ_EVENTSIZ_SHFT                                    0x0

#define DWC_GEVNTCOUNT_ADDR(base,p)                            ((base) + 0x0000c40c + 0x10 * (p))
#define DWC_GEVNTCOUNT_OFFS(p)                                 (0x0000c40c + 0x10 * (p))
#define DWC_GEVNTCOUNT_RMSK                                    0xffffffff
#define DWC_GEVNTCOUNT_MAXp                                             1
#define DWC_GEVNTCOUNT_POR                                     0x00000000
#define DWC_GEVNTCOUNT_RSVD75_BMSK                             0xffff0000
#define DWC_GEVNTCOUNT_RSVD75_SHFT                                   0x10
#define DWC_GEVNTCOUNT_EVNTCOUNT_BMSK                              0xffff
#define DWC_GEVNTCOUNT_EVNTCOUNT_SHFT                                 0x0

#define DWC_GSBUSCFG0_ADDR(x)                                                    ((x) + 0x0000c100)
#define DWC_GSBUSCFG0_OFFS                                                       (0x0000c100)
#define DWC_GSBUSCFG0_RMSK                                                       0xffffffff
#define DWC_GSBUSCFG0_POR                                                        0x0000000e
#define DWC_GSBUSCFG0_RSVD10_BMSK                                                0xffffff00
#define DWC_GSBUSCFG0_RSVD10_SHFT                                                       0x8
#define DWC_GSBUSCFG0_INCR256BRSTENA_BMSK                                              0x80
#define DWC_GSBUSCFG0_INCR256BRSTENA_SHFT                                               0x7
#define DWC_GSBUSCFG0_INCR128BRSTENA_BMSK                                              0x40
#define DWC_GSBUSCFG0_INCR128BRSTENA_SHFT                                               0x6
#define DWC_GSBUSCFG0_INCR64BRSTENA_BMSK                                               0x20
#define DWC_GSBUSCFG0_INCR64BRSTENA_SHFT                                                0x5
#define DWC_GSBUSCFG0_INCR32BRSTENA_BMSK                                               0x10
#define DWC_GSBUSCFG0_INCR32BRSTENA_SHFT                                                0x4
#define DWC_GSBUSCFG0_INCR16BRSTENA_BMSK                                                0x8
#define DWC_GSBUSCFG0_INCR16BRSTENA_SHFT                                                0x3
#define DWC_GSBUSCFG0_INCR8BRSTENA_BMSK                                                 0x4
#define DWC_GSBUSCFG0_INCR8BRSTENA_SHFT                                                 0x2
#define DWC_GSBUSCFG0_INCR4BRSTENA_BMSK                                                 0x2
#define DWC_GSBUSCFG0_INCR4BRSTENA_SHFT                                                 0x1
#define DWC_GSBUSCFG0_INCRBRSTENA_BMSK                                                  0x1
#define DWC_GSBUSCFG0_INCRBRSTENA_SHFT                                                  0x0

#define DWC_GSBUSCFG1_ADDR(x)                                                    ((x) + 0x0000c104)
#define DWC_GSBUSCFG1_OFFS                                                       (0x0000c104)
#define DWC_GSBUSCFG1_RMSK                                                       0xffffffff
#define DWC_GSBUSCFG1_POR                                                        0x00001700
#define DWC_GSBUSCFG1_RSVD0_BMSK                                                 0xffffe000
#define DWC_GSBUSCFG1_RSVD0_SHFT                                                        0xd
#define DWC_GSBUSCFG1_EN1KPAGE_BMSK                                                  0x1000
#define DWC_GSBUSCFG1_EN1KPAGE_SHFT                                                     0xc
#define DWC_GSBUSCFG1_PIPETRANSLIMIT_BMSK                                             0xf00
#define DWC_GSBUSCFG1_PIPETRANSLIMIT_SHFT                                               0x8
#define DWC_GSBUSCFG1_RSVD1_BMSK                                                       0xff
#define DWC_GSBUSCFG1_RSVD1_SHFT                                                        0x0

#define DWC_GTXTHRCFG_ADDR(x)                                                    ((x) + 0x0000c108)
#define DWC_GTXTHRCFG_OFFS                                                       (0x0000c108)
#define DWC_GTXTHRCFG_RMSK                                                       0xffffffff
#define DWC_GTXTHRCFG_POR                                                        0x00000000
#define DWC_GTXTHRCFG_USBISOTHREN_BMSK                                           0x80000000
#define DWC_GTXTHRCFG_USBISOTHREN_SHFT                                                 0x1f
#define DWC_GTXTHRCFG_USBNONISOTHREN_BMSK                                        0x40000000
#define DWC_GTXTHRCFG_USBNONISOTHREN_SHFT                                              0x1e
#define DWC_GTXTHRCFG_USBTXPKTCNTSEL_BMSK                                        0x20000000
#define DWC_GTXTHRCFG_USBTXPKTCNTSEL_SHFT                                              0x1d
#define DWC_GTXTHRCFG_RSVD3_BMSK                                                 0x10000000
#define DWC_GTXTHRCFG_RSVD3_SHFT                                                       0x1c
#define DWC_GTXTHRCFG_USBTXPKTCNT_BMSK                                            0xf000000
#define DWC_GTXTHRCFG_USBTXPKTCNT_SHFT                                                 0x18
#define DWC_GTXTHRCFG_USBMAXTXBURSTSIZE_BMSK                                       0xff0000
#define DWC_GTXTHRCFG_USBMAXTXBURSTSIZE_SHFT                                           0x10
#define DWC_GTXTHRCFG_SBUSISOTHREN_BMSK                                              0x8000
#define DWC_GTXTHRCFG_SBUSISOTHREN_SHFT                                                 0xf
#define DWC_GTXTHRCFG_SBUSNONISOTHREN_BMSK                                           0x4000
#define DWC_GTXTHRCFG_SBUSNONISOTHREN_SHFT                                              0xe
#define DWC_GTXTHRCFG_RSVD2_BMSK                                                     0x3800
#define DWC_GTXTHRCFG_RSVD2_SHFT                                                        0xb
#define DWC_GTXTHRCFG_RSVD1_BMSK                                                      0x7ff
#define DWC_GTXTHRCFG_RSVD1_SHFT                                                        0x0

#define DWC_GRXTHRCFG_ADDR(x)                                                    ((x) + 0x0000c10c)
#define DWC_GRXTHRCFG_OFFS                                                       (0x0000c10c)
#define DWC_GRXTHRCFG_RMSK                                                       0xffffffff
#define DWC_GRXTHRCFG_POR                                                        0x00000000
#define DWC_GRXTHRCFG_RSVD5_BMSK                                                 0xc0000000
#define DWC_GRXTHRCFG_RSVD5_SHFT                                                       0x1e
#define DWC_GRXTHRCFG_USBRXPKTCNTSEL_BMSK                                        0x20000000
#define DWC_GRXTHRCFG_USBRXPKTCNTSEL_SHFT                                              0x1d
#define DWC_GRXTHRCFG_RSVD4_BMSK                                                 0x10000000
#define DWC_GRXTHRCFG_RSVD4_SHFT                                                       0x1c
#define DWC_GRXTHRCFG_USBRXPKTCNT_BMSK                                            0xf000000
#define DWC_GRXTHRCFG_USBRXPKTCNT_SHFT                                                 0x18
#define DWC_GRXTHRCFG_USBMAXRXBURSTSIZE_BMSK                                       0xf80000
#define DWC_GRXTHRCFG_USBMAXRXBURSTSIZE_SHFT                                           0x13
#define DWC_GRXTHRCFG_RSVD3_BMSK                                                    0x70000
#define DWC_GRXTHRCFG_RSVD3_SHFT                                                       0x10
#define DWC_GRXTHRCFG_RXTHREN_BMSK                                                   0x8000
#define DWC_GRXTHRCFG_RXTHREN_SHFT                                                      0xf
#define DWC_GRXTHRCFG_RSVD2_BMSK                                                     0x7800
#define DWC_GRXTHRCFG_RSVD2_SHFT                                                        0xb
#define DWC_GRXTHRCFG_RSVD1_BMSK                                                      0x7ff
#define DWC_GRXTHRCFG_RSVD1_SHFT                                                        0x0

#define DWC_GCTL_ADDR(x)                                                         ((x) + 0x0000c110)
#define DWC_GCTL_OFFS                                                            (0x0000c110)
#define DWC_GCTL_RMSK                                                            0xffffffff
#define DWC_GCTL_POR                                                             0x30c02000
#define DWC_GCTL_PWRDNSCALE_BMSK                                                 0xfff80000
#define DWC_GCTL_PWRDNSCALE_SHFT                                                       0x13
#define DWC_GCTL_MASTERFILTBYPASS_BMSK                                              0x40000
#define DWC_GCTL_MASTERFILTBYPASS_SHFT                                                 0x12
#define DWC_GCTL_BYPSSETADDR_BMSK                                                   0x20000
#define DWC_GCTL_BYPSSETADDR_SHFT                                                      0x11
#define DWC_GCTL_U2RSTECN_BMSK                                                      0x10000
#define DWC_GCTL_U2RSTECN_SHFT                                                         0x10
#define DWC_GCTL_FRMSCLDWN_BMSK                                                      0xc000
#define DWC_GCTL_FRMSCLDWN_SHFT                                                         0xe
#define DWC_GCTL_PRTCAPDIR_BMSK                                                      0x3000
#define DWC_GCTL_PRTCAPDIR_SHFT                                                         0xc
#define DWC_GCTL_CORESOFTRESET_BMSK                                                   0x800
#define DWC_GCTL_CORESOFTRESET_SHFT                                                     0xb
#define DWC_GCTL_SOFITPSYNC_BMSK                                                      0x400
#define DWC_GCTL_SOFITPSYNC_SHFT                                                        0xa
#define DWC_GCTL_U1U2TIMERSCALE_BMSK                                                  0x200
#define DWC_GCTL_U1U2TIMERSCALE_SHFT                                                    0x9
#define DWC_GCTL_DEBUGATTACH_BMSK                                                     0x100
#define DWC_GCTL_DEBUGATTACH_SHFT                                                       0x8
#define DWC_GCTL_RAMCLKSEL_BMSK                                                        0xc0
#define DWC_GCTL_RAMCLKSEL_SHFT                                                         0x6
#define DWC_GCTL_SCALEDOWN_BMSK                                                        0x30
#define DWC_GCTL_SCALEDOWN_SHFT                                                         0x4
#define DWC_GCTL_DISSCRAMBLE_BMSK                                                       0x8
#define DWC_GCTL_DISSCRAMBLE_SHFT                                                       0x3
#define DWC_GCTL_RSVD6_BMSK                                                             0x4
#define DWC_GCTL_RSVD6_SHFT                                                             0x2
#define DWC_GCTL_GBLHIBERNATIONEN_BMSK                                                  0x2
#define DWC_GCTL_GBLHIBERNATIONEN_SHFT                                                  0x1
#define DWC_GCTL_DSBLCLKGTNG_BMSK                                                       0x1
#define DWC_GCTL_DSBLCLKGTNG_SHFT                                                       0x0

#define DWC_GSTS_ADDR(x)                                                         ((x) + 0x0000c118)
#define DWC_GSTS_OFFS                                                            (0x0000c118)
#define DWC_GSTS_RMSK                                                            0xffffffff
#define DWC_GSTS_POR                                                             0x3e800002
#define DWC_GSTS_CBELT_BMSK                                                      0xfff00000
#define DWC_GSTS_CBELT_SHFT                                                            0x14
#define DWC_GSTS_RSVD7_BMSK                                                         0xff800
#define DWC_GSTS_RSVD7_SHFT                                                             0xb
#define DWC_GSTS_OTG_IP_BMSK                                                          0x400
#define DWC_GSTS_OTG_IP_SHFT                                                            0xa
#define DWC_GSTS_BC_IP_BMSK                                                           0x200
#define DWC_GSTS_BC_IP_SHFT                                                             0x9
#define DWC_GSTS_ADP_IP_BMSK                                                          0x100
#define DWC_GSTS_ADP_IP_SHFT                                                            0x8
#define DWC_GSTS_HOST_IP_BMSK                                                          0x80
#define DWC_GSTS_HOST_IP_SHFT                                                           0x7
#define DWC_GSTS_DEVICE_IP_BMSK                                                        0x40
#define DWC_GSTS_DEVICE_IP_SHFT                                                         0x6
#define DWC_GSTS_CSRTIMEOUT_BMSK                                                       0x20
#define DWC_GSTS_CSRTIMEOUT_SHFT                                                        0x5
#define DWC_GSTS_BUSERRADDRVLD_BMSK                                                    0x10
#define DWC_GSTS_BUSERRADDRVLD_SHFT                                                     0x4
#define DWC_GSTS_R6_BMSK                                                                0xc
#define DWC_GSTS_R6_SHFT                                                                0x2
#define DWC_GSTS_CURMOD_BMSK                                                            0x3
#define DWC_GSTS_CURMOD_SHFT                                                            0x0

#define DWC_GSNPSID_ADDR(x)                                                      ((x) + 0x0000c120)
#define DWC_GSNPSID_OFFS                                                         (0x0000c120)
#define DWC_GSNPSID_RMSK                                                         0xffffffff
#define DWC_GSNPSID_POR                                                          0x5533203a
#define DWC_GSNPSID_SYNOPSYSID_BMSK                                              0xffffffff
#define DWC_GSNPSID_SYNOPSYSID_SHFT                                                     0x0

#define DWC_GGPIO_ADDR(x)                                                        ((x) + 0x0000c124)
#define DWC_GGPIO_OFFS                                                           (0x0000c124)
#define DWC_GGPIO_RMSK                                                           0xffffffff
#define DWC_GGPIO_POR                                                            0x00000000
#define DWC_GGPIO_GPO_BMSK                                                       0xffff0000
#define DWC_GGPIO_GPO_SHFT                                                             0x10
#define DWC_GGPIO_GPI_BMSK                                                           0xffff
#define DWC_GGPIO_GPI_SHFT                                                              0x0

#define DWC_GUID_ADDR(x)                                                         ((x) + 0x0000c128)
#define DWC_GUID_OFFS                                                            (0x0000c128)
#define DWC_GUID_RMSK                                                            0xffffffff
#define DWC_GUID_POR                                                             0x11203a27
#define DWC_GUID_USERID_BMSK                                                     0xffffffff
#define DWC_GUID_USERID_SHFT                                                            0x0

#define DWC_GUCTL_ADDR(x)                                                        ((x) + 0x0000c12c)
#define DWC_GUCTL_OFFS                                                           (0x0000c12c)
#define DWC_GUCTL_RMSK                                                           0xffdfffff
#define DWC_GUCTL_POR                                                            0x00008010
#define DWC_GUCTL_REFCLKPER_BMSK                                                 0xffc00000
#define DWC_GUCTL_REFCLKPER_SHFT                                                       0x16
#define DWC_GUCTL_RSVD_BMSK                                                        0x1c0000
#define DWC_GUCTL_RSVD_SHFT                                                            0x12
#define DWC_GUCTL_SPRSCTRLTRANSEN_BMSK                                              0x20000
#define DWC_GUCTL_SPRSCTRLTRANSEN_SHFT                                                 0x11
#define DWC_GUCTL_RESBWHSEPS_BMSK                                                   0x10000
#define DWC_GUCTL_RESBWHSEPS_SHFT                                                      0x10
#define DWC_GUCTL_CMDEVADDR_BMSK                                                     0x8000
#define DWC_GUCTL_CMDEVADDR_SHFT                                                        0xf
#define DWC_GUCTL_USBHSTINAUTORETRYEN_BMSK                                           0x4000
#define DWC_GUCTL_USBHSTINAUTORETRYEN_SHFT                                              0xe
#define DWC_GUCTL_RSVD7_BMSK                                                         0x3000
#define DWC_GUCTL_RSVD7_SHFT                                                            0xc
#define DWC_GUCTL_INSRTEXTRFSBODI_BMSK                                                0x800
#define DWC_GUCTL_INSRTEXTRFSBODI_SHFT                                                  0xb
#define DWC_GUCTL_DTCT_BMSK                                                           0x600
#define DWC_GUCTL_DTCT_SHFT                                                             0x9
#define DWC_GUCTL_DTFT_BMSK                                                           0x1ff
#define DWC_GUCTL_DTFT_SHFT                                                             0x0

#define DWC_GBUSERRADDRLO_ADDR(x)                                                ((x) + 0x0000c130)
#define DWC_GBUSERRADDRLO_OFFS                                                   (0x0000c130)
#define DWC_GBUSERRADDRLO_RMSK                                                   0xffffffff
#define DWC_GBUSERRADDRLO_POR                                                    0x00000000
#define DWC_GBUSERRADDRLO_BUSERRADDR_BMSK                                        0xffffffff
#define DWC_GBUSERRADDRLO_BUSERRADDR_SHFT                                               0x0

#define DWC_GBUSERRADDRHI_ADDR(x)                                                ((x) + 0x0000c134)
#define DWC_GBUSERRADDRHI_OFFS                                                   (0x0000c134)
#define DWC_GBUSERRADDRHI_RMSK                                                   0xffffffff
#define DWC_GBUSERRADDRHI_POR                                                    0x00000000
#define DWC_GBUSERRADDRHI_BUSERRADDR_BMSK                                        0xffffffff
#define DWC_GBUSERRADDRHI_BUSERRADDR_SHFT                                               0x0

#define DWC_GPRTBIMAPLO_ADDR(x)                                                  ((x) + 0x0000c138)
#define DWC_GPRTBIMAPLO_OFFS                                                     (0x0000c138)
#define DWC_GPRTBIMAPLO_RMSK                                                     0xffffffff
#define DWC_GPRTBIMAPLO_POR                                                      0x00000000
#define DWC_GPRTBIMAPLO_BINUM8_BMSK                                              0xf0000000
#define DWC_GPRTBIMAPLO_BINUM8_SHFT                                                    0x1c
#define DWC_GPRTBIMAPLO_BINUM7_BMSK                                               0xf000000
#define DWC_GPRTBIMAPLO_BINUM7_SHFT                                                    0x18
#define DWC_GPRTBIMAPLO_BINUM6_BMSK                                                0xf00000
#define DWC_GPRTBIMAPLO_BINUM6_SHFT                                                    0x14
#define DWC_GPRTBIMAPLO_BINUM5_BMSK                                                 0xf0000
#define DWC_GPRTBIMAPLO_BINUM5_SHFT                                                    0x10
#define DWC_GPRTBIMAPLO_BINUM4_BMSK                                                  0xf000
#define DWC_GPRTBIMAPLO_BINUM4_SHFT                                                     0xc
#define DWC_GPRTBIMAPLO_BINUM3_BMSK                                                   0xf00
#define DWC_GPRTBIMAPLO_BINUM3_SHFT                                                     0x8
#define DWC_GPRTBIMAPLO_BINUM2_BMSK                                                    0xf0
#define DWC_GPRTBIMAPLO_BINUM2_SHFT                                                     0x4
#define DWC_GPRTBIMAPLO_BINUM1_BMSK                                                     0xf
#define DWC_GPRTBIMAPLO_BINUM1_SHFT                                                     0x0

#define DWC_GPRTBIMAPHI_ADDR(x)                                                  ((x) + 0x0000c13c)
#define DWC_GPRTBIMAPHI_OFFS                                                     (0x0000c13c)
#define DWC_GPRTBIMAPHI_RMSK                                                     0xffffffff
#define DWC_GPRTBIMAPHI_POR                                                      0x00000000
#define DWC_GPRTBIMAPHI_RSVD16_BMSK                                              0xf0000000
#define DWC_GPRTBIMAPHI_RSVD16_SHFT                                                    0x1c
#define DWC_GPRTBIMAPHI_BINUM15_BMSK                                              0xf000000
#define DWC_GPRTBIMAPHI_BINUM15_SHFT                                                   0x18
#define DWC_GPRTBIMAPHI_BINUM14_BMSK                                               0xf00000
#define DWC_GPRTBIMAPHI_BINUM14_SHFT                                                   0x14
#define DWC_GPRTBIMAPHI_BINUM13_BMSK                                                0xf0000
#define DWC_GPRTBIMAPHI_BINUM13_SHFT                                                   0x10
#define DWC_GPRTBIMAPHI_BINUM12_BMSK                                                 0xf000
#define DWC_GPRTBIMAPHI_BINUM12_SHFT                                                    0xc
#define DWC_GPRTBIMAPHI_BINUM11_BMSK                                                  0xf00
#define DWC_GPRTBIMAPHI_BINUM11_SHFT                                                    0x8
#define DWC_GPRTBIMAPHI_BINUM10_BMSK                                                   0xf0
#define DWC_GPRTBIMAPHI_BINUM10_SHFT                                                    0x4
#define DWC_GPRTBIMAPHI_BINUM9_BMSK                                                     0xf
#define DWC_GPRTBIMAPHI_BINUM9_SHFT                                                     0x0

#define DWC_GHWPARAMS0_ADDR(x)                                                   ((x) + 0x0000c140)
#define DWC_GHWPARAMS0_OFFS                                                      (0x0000c140)
#define DWC_GHWPARAMS0_RMSK                                                      0xfffffffb
#define DWC_GHWPARAMS0_POR                                                       0x00000000
#define DWC_GHWPARAMS0_GHWPARAMS0_31_24_BMSK                                     0xff000000
#define DWC_GHWPARAMS0_GHWPARAMS0_31_24_SHFT                                           0x18
#define DWC_GHWPARAMS0_GHWPARAMS0_23_16_BMSK                                       0xff0000
#define DWC_GHWPARAMS0_GHWPARAMS0_23_16_SHFT                                           0x10
#define DWC_GHWPARAMS0_GHWPARAMS0_15_8_BMSK                                          0xff00
#define DWC_GHWPARAMS0_GHWPARAMS0_15_8_SHFT                                             0x8
#define DWC_GHWPARAMS0_GHWPARAMS0_7_6_BMSK                                             0xc0
#define DWC_GHWPARAMS0_GHWPARAMS0_7_6_SHFT                                              0x6
#define DWC_GHWPARAMS0_GHWPARAMS0_5_3_BMSK                                             0x38
#define DWC_GHWPARAMS0_GHWPARAMS0_5_3_SHFT                                              0x3
#define DWC_GHWPARAMS0_GHWPARAMS0_2_0_BMSK                                              0x3
#define DWC_GHWPARAMS0_GHWPARAMS0_2_0_SHFT                                              0x0

#define DWC_GHWPARAMS1_ADDR(x)                                                   ((x) + 0x0000c144)
#define DWC_GHWPARAMS1_OFFS                                                      (0x0000c144)
#define DWC_GHWPARAMS1_RMSK                                                      0xffffffff
#define DWC_GHWPARAMS1_POR                                                       0x01614938
#define DWC_GHWPARAMS1_GHWPARAMS1_31_BMSK                                        0x80000000
#define DWC_GHWPARAMS1_GHWPARAMS1_31_SHFT                                              0x1f
#define DWC_GHWPARAMS1_GHWPARAMS1_30_BMSK                                        0x40000000
#define DWC_GHWPARAMS1_GHWPARAMS1_30_SHFT                                              0x1e
#define DWC_GHWPARAMS1_GHWPARAMS1_29_BMSK                                        0x20000000
#define DWC_GHWPARAMS1_GHWPARAMS1_29_SHFT                                              0x1d
#define DWC_GHWPARAMS1_GHWPARAMS1_28_BMSK                                        0x10000000
#define DWC_GHWPARAMS1_GHWPARAMS1_28_SHFT                                              0x1c
#define DWC_GHWPARAMS1_GHWPARAMS1_27_BMSK                                         0x8000000
#define DWC_GHWPARAMS1_GHWPARAMS1_27_SHFT                                              0x1b
#define DWC_GHWPARAMS1_GHWPARAMS1_26_BMSK                                         0x4000000
#define DWC_GHWPARAMS1_GHWPARAMS1_26_SHFT                                              0x1a
#define DWC_GHWPARAMS1_GHWPARAMS1_25_24_BMSK                                      0x3000000
#define DWC_GHWPARAMS1_GHWPARAMS1_25_24_SHFT                                           0x18
#define DWC_GHWPARAMS1_GHWPARAMS1_23_BMSK                                          0x800000
#define DWC_GHWPARAMS1_GHWPARAMS1_23_SHFT                                              0x17
#define DWC_GHWPARAMS1_GHWPARAMS1_22_21_BMSK                                       0x600000
#define DWC_GHWPARAMS1_GHWPARAMS1_22_21_SHFT                                           0x15
#define DWC_GHWPARAMS1_GHWPARAMS1_20_15_BMSK                                       0x1f8000
#define DWC_GHWPARAMS1_GHWPARAMS1_20_15_SHFT                                            0xf
#define DWC_GHWPARAMS1_GHWPARAMS1_14_12_BMSK                                         0x7000
#define DWC_GHWPARAMS1_GHWPARAMS1_14_12_SHFT                                            0xc
#define DWC_GHWPARAMS1_GHWPARAMS1_11_9_BMSK                                           0xe00
#define DWC_GHWPARAMS1_GHWPARAMS1_11_9_SHFT                                             0x9
#define DWC_GHWPARAMS1_GHWPARAMS1_8_6_BMSK                                            0x1c0
#define DWC_GHWPARAMS1_GHWPARAMS1_8_6_SHFT                                              0x6
#define DWC_GHWPARAMS1_GHWPARAMS1_5_3_BMSK                                             0x38
#define DWC_GHWPARAMS1_GHWPARAMS1_5_3_SHFT                                              0x3
#define DWC_GHWPARAMS1_GHWPARAMS1_2_0_BMSK                                              0x7
#define DWC_GHWPARAMS1_GHWPARAMS1_2_0_SHFT                                              0x0

#define DWC_GHWPARAMS2_ADDR(x)                                                   ((x) + 0x0000c148)
#define DWC_GHWPARAMS2_OFFS                                                      (0x0000c148)
#define DWC_GHWPARAMS2_RMSK                                                      0xffffffff
#define DWC_GHWPARAMS2_POR                                                       0x11203a27
#define DWC_GHWPARAMS2_GHWPARAMS2_31_0_BMSK                                      0xffffffff
#define DWC_GHWPARAMS2_GHWPARAMS2_31_0_SHFT                                             0x0

#define DWC_GHWPARAMS3_ADDR(x)                                                   ((x) + 0x0000c14c)
#define DWC_GHWPARAMS3_OFFS                                                      (0x0000c14c)
#define DWC_GHWPARAMS3_RMSK                                                      0xffffffff
#define DWC_GHWPARAMS3_POR                                                       0x10420085
#define DWC_GHWPARAMS3_GHWPARAMS3_31_BMSK                                        0x80000000
#define DWC_GHWPARAMS3_GHWPARAMS3_31_SHFT                                              0x1f
#define DWC_GHWPARAMS3_GHWPARAMS3_30_23_BMSK                                     0x7f800000
#define DWC_GHWPARAMS3_GHWPARAMS3_30_23_SHFT                                           0x17
#define DWC_GHWPARAMS3_GHWPARAMS3_22_18_BMSK                                       0x7c0000
#define DWC_GHWPARAMS3_GHWPARAMS3_22_18_SHFT                                           0x12
#define DWC_GHWPARAMS3_GHWPARAMS3_17_12_BMSK                                        0x3f000
#define DWC_GHWPARAMS3_GHWPARAMS3_17_12_SHFT                                            0xc
#define DWC_GHWPARAMS3_GHWPARAMS3_11_BMSK                                             0x800
#define DWC_GHWPARAMS3_GHWPARAMS3_11_SHFT                                               0xb
#define DWC_GHWPARAMS3_GHWPARAMS3_10_BMSK                                             0x400
#define DWC_GHWPARAMS3_GHWPARAMS3_10_SHFT                                               0xa
#define DWC_GHWPARAMS3_GHWPARAMS3_9_8_BMSK                                            0x300
#define DWC_GHWPARAMS3_GHWPARAMS3_9_8_SHFT                                              0x8
#define DWC_GHWPARAMS3_GHWPARAMS3_7_6_BMSK                                             0xc0
#define DWC_GHWPARAMS3_GHWPARAMS3_7_6_SHFT                                              0x6
#define DWC_GHWPARAMS3_GHWPARAMS3_5_4_BMSK                                             0x30
#define DWC_GHWPARAMS3_GHWPARAMS3_5_4_SHFT                                              0x4
#define DWC_GHWPARAMS3_GHWPARAMS3_3_2_BMSK                                              0xc
#define DWC_GHWPARAMS3_GHWPARAMS3_3_2_SHFT                                              0x2
#define DWC_GHWPARAMS3_GHWPARAMS3_1_0_BMSK                                              0x3
#define DWC_GHWPARAMS3_GHWPARAMS3_1_0_SHFT                                              0x0

#define DWC_GHWPARAMS4_ADDR(x)                                                   ((x) + 0x0000c150)
#define DWC_GHWPARAMS4_OFFS                                                      (0x0000c150)
#define DWC_GHWPARAMS4_RMSK                                                      0xffffffff
#define DWC_GHWPARAMS4_POR                                                       0x48822005
#define DWC_GHWPARAMS4_GHWPARAMS4_31_28_BMSK                                     0xf0000000
#define DWC_GHWPARAMS4_GHWPARAMS4_31_28_SHFT                                           0x1c
#define DWC_GHWPARAMS4_GHWPARAMS4_27_24_BMSK                                      0xf000000
#define DWC_GHWPARAMS4_GHWPARAMS4_27_24_SHFT                                           0x18
#define DWC_GHWPARAMS4_GHWPARAMS4_23_BMSK                                          0x800000
#define DWC_GHWPARAMS4_GHWPARAMS4_23_SHFT                                              0x17
#define DWC_GHWPARAMS4_GHWPARAMS4_22_BMSK                                          0x400000
#define DWC_GHWPARAMS4_GHWPARAMS4_22_SHFT                                              0x16
#define DWC_GHWPARAMS4_GHWPARAMS4_21_BMSK                                          0x200000
#define DWC_GHWPARAMS4_GHWPARAMS4_21_SHFT                                              0x15
#define DWC_GHWPARAMS4_GHWPARAMS4_20_17_BMSK                                       0x1e0000
#define DWC_GHWPARAMS4_GHWPARAMS4_20_17_SHFT                                           0x11
#define DWC_GHWPARAMS4_GHWPARAMS4_16_13_BMSK                                        0x1e000
#define DWC_GHWPARAMS4_GHWPARAMS4_16_13_SHFT                                            0xd
#define DWC_GHWPARAMS4_GHWPARAMS4_12_6_BMSK                                          0x1fc0
#define DWC_GHWPARAMS4_GHWPARAMS4_12_6_SHFT                                             0x6
#define DWC_GHWPARAMS4_GHWPARAMS4_5_0_BMSK                                             0x3f
#define DWC_GHWPARAMS4_GHWPARAMS4_5_0_SHFT                                              0x0

#define DWC_GHWPARAMS5_ADDR(x)                                                   ((x) + 0x0000c154)
#define DWC_GHWPARAMS5_OFFS                                                      (0x0000c154)
#define DWC_GHWPARAMS5_RMSK                                                      0xffffffff
#define DWC_GHWPARAMS5_POR                                                       0x04202088
#define DWC_GHWPARAMS5_GHWPARAMS5_31_28_BMSK                                     0xf0000000
#define DWC_GHWPARAMS5_GHWPARAMS5_31_28_SHFT                                           0x1c
#define DWC_GHWPARAMS5_GHWPARAMS5_27_22_BMSK                                      0xfc00000
#define DWC_GHWPARAMS5_GHWPARAMS5_27_22_SHFT                                           0x16
#define DWC_GHWPARAMS5_GHWPARAMS5_21_16_BMSK                                       0x3f0000
#define DWC_GHWPARAMS5_GHWPARAMS5_21_16_SHFT                                           0x10
#define DWC_GHWPARAMS5_GHWPARAMS5_15_10_BMSK                                         0xfc00
#define DWC_GHWPARAMS5_GHWPARAMS5_15_10_SHFT                                            0xa
#define DWC_GHWPARAMS5_GHWPARAMS5_9_4_BMSK                                            0x3f0
#define DWC_GHWPARAMS5_GHWPARAMS5_9_4_SHFT                                              0x4
#define DWC_GHWPARAMS5_GHWPARAMS5_3_0_BMSK                                              0xf
#define DWC_GHWPARAMS5_GHWPARAMS5_3_0_SHFT                                              0x0

#define DWC_GHWPARAMS6_ADDR(x)                                                   ((x) + 0x0000c158)
#define DWC_GHWPARAMS6_OFFS                                                      (0x0000c158)
#define DWC_GHWPARAMS6_RMSK                                                      0xffffffff
#define DWC_GHWPARAMS6_POR                                                       0x065c8c20
#define DWC_GHWPARAMS6_GHWPARAMS6_31_16_BMSK                                     0xffff0000
#define DWC_GHWPARAMS6_GHWPARAMS6_31_16_SHFT                                           0x10
#define DWC_GHWPARAMS6_BUSFLTRSSUPPORT_BMSK                                          0x8000
#define DWC_GHWPARAMS6_BUSFLTRSSUPPORT_SHFT                                             0xf
#define DWC_GHWPARAMS6_BCSUPPORT_BMSK                                                0x4000
#define DWC_GHWPARAMS6_BCSUPPORT_SHFT                                                   0xe
#define DWC_GHWPARAMS6_OTG_SS_SUPPORT_BMSK                                           0x2000
#define DWC_GHWPARAMS6_OTG_SS_SUPPORT_SHFT                                              0xd
#define DWC_GHWPARAMS6_ADPSUPPORT_BMSK                                               0x1000
#define DWC_GHWPARAMS6_ADPSUPPORT_SHFT                                                  0xc
#define DWC_GHWPARAMS6_HNPSUPPORT_BMSK                                                0x800
#define DWC_GHWPARAMS6_HNPSUPPORT_SHFT                                                  0xb
#define DWC_GHWPARAMS6_SRPSUPPORT_BMSK                                                0x400
#define DWC_GHWPARAMS6_SRPSUPPORT_SHFT                                                  0xa
#define DWC_GHWPARAMS6_GHWPARAMS6_9_8_BMSK                                            0x300
#define DWC_GHWPARAMS6_GHWPARAMS6_9_8_SHFT                                              0x8
#define DWC_GHWPARAMS6_GHWPARAMS6_7_BMSK                                               0x80
#define DWC_GHWPARAMS6_GHWPARAMS6_7_SHFT                                                0x7
#define DWC_GHWPARAMS6_GHWPARAMS6_6_BMSK                                               0x40
#define DWC_GHWPARAMS6_GHWPARAMS6_6_SHFT                                                0x6
#define DWC_GHWPARAMS6_GHWPARAMS6_5_0_BMSK                                             0x3f
#define DWC_GHWPARAMS6_GHWPARAMS6_5_0_SHFT                                              0x0

#define DWC_GHWPARAMS7_ADDR(x)                                                   ((x) + 0x0000c15c)
#define DWC_GHWPARAMS7_OFFS                                                      (0x0000c15c)
#define DWC_GHWPARAMS7_RMSK                                                      0xffffffff
#define DWC_GHWPARAMS7_POR                                                       0x03080cea
#define DWC_GHWPARAMS7_GHWPARAMS7_31_16_BMSK                                     0xffff0000
#define DWC_GHWPARAMS7_GHWPARAMS7_31_16_SHFT                                           0x10
#define DWC_GHWPARAMS7_GHWPARAMS7_15_0_BMSK                                          0xffff
#define DWC_GHWPARAMS7_GHWPARAMS7_15_0_SHFT                                             0x0

#define DWC_GDBGFIFOSPACE_ADDR(x)                                                ((x) + 0x0000c160)
#define DWC_GDBGFIFOSPACE_OFFS                                                   (0x0000c160)
#define DWC_GDBGFIFOSPACE_RMSK                                                   0xffffffff
#define DWC_GDBGFIFOSPACE_POR                                                    0x00820000
#define DWC_GDBGFIFOSPACE_SPACE_AVAILABLE_BMSK                                   0xffff0000
#define DWC_GDBGFIFOSPACE_SPACE_AVAILABLE_SHFT                                         0x10
#define DWC_GDBGFIFOSPACE_RSVD8_BMSK                                                 0xff00
#define DWC_GDBGFIFOSPACE_RSVD8_SHFT                                                    0x8
#define DWC_GDBGFIFOSPACE_FIFO_QUEUE_SELECT_BMSK                                       0xff
#define DWC_GDBGFIFOSPACE_FIFO_QUEUE_SELECT_SHFT                                        0x0

#define DWC_GDBGLTSSM_ADDR(x)                                                    ((x) + 0x0000c164)
#define DWC_GDBGLTSSM_OFFS                                                       (0x0000c164)
#define DWC_GDBGLTSSM_RMSK                                                       0xfddfffdf
#define DWC_GDBGLTSSM_POR                                                        0x00000000
#define DWC_GDBGLTSSM_RSVD14_BMSK                                                0xc0000000
#define DWC_GDBGLTSSM_RSVD14_SHFT                                                      0x1e
#define DWC_GDBGLTSSM_X3_XS_SWAPPING_BMSK                                        0x20000000
#define DWC_GDBGLTSSM_X3_XS_SWAPPING_SHFT                                              0x1d
#define DWC_GDBGLTSSM_X3_DS_HOST_SHUTDOWN_BMSK                                   0x10000000
#define DWC_GDBGLTSSM_X3_DS_HOST_SHUTDOWN_SHFT                                         0x1c
#define DWC_GDBGLTSSM_PRTDIRECTION_BMSK                                           0x8000000
#define DWC_GDBGLTSSM_PRTDIRECTION_SHFT                                                0x1b
#define DWC_GDBGLTSSM_LTDBTIMEOUT_BMSK                                            0x4000000
#define DWC_GDBGLTSSM_LTDBTIMEOUT_SHFT                                                 0x1a
#define DWC_GDBGLTSSM_LTDBLINKSTATE_BMSK                                          0x1c00000
#define DWC_GDBGLTSSM_LTDBLINKSTATE_SHFT                                               0x16
#define DWC_GDBGLTSSM_LTDBSUBSTATE_BMSK                                            0x1c0000
#define DWC_GDBGLTSSM_LTDBSUBSTATE_SHFT                                                0x12
#define DWC_GDBGLTSSM_ELASTICBUFFERMODE_BMSK                                        0x20000
#define DWC_GDBGLTSSM_ELASTICBUFFERMODE_SHFT                                           0x11
#define DWC_GDBGLTSSM_TXELECLDLE_BMSK                                               0x10000
#define DWC_GDBGLTSSM_TXELECLDLE_SHFT                                                  0x10
#define DWC_GDBGLTSSM_RXPOLARITY_BMSK                                                0x8000
#define DWC_GDBGLTSSM_RXPOLARITY_SHFT                                                   0xf
#define DWC_GDBGLTSSM_TXDETRXLOOPBACK_BMSK                                           0x4000
#define DWC_GDBGLTSSM_TXDETRXLOOPBACK_SHFT                                              0xe
#define DWC_GDBGLTSSM_RSVD12_BMSK                                                    0x2000
#define DWC_GDBGLTSSM_RSVD12_SHFT                                                       0xd
#define DWC_GDBGLTSSM_LTDBPHYCMDSTATE_BMSK                                           0x1800
#define DWC_GDBGLTSSM_LTDBPHYCMDSTATE_SHFT                                              0xb
#define DWC_GDBGLTSSM_POWERDOWN_BMSK                                                  0x600
#define DWC_GDBGLTSSM_POWERDOWN_SHFT                                                    0x9
#define DWC_GDBGLTSSM_RXEQTRAIN_BMSK                                                  0x100
#define DWC_GDBGLTSSM_RXEQTRAIN_SHFT                                                    0x8
#define DWC_GDBGLTSSM_TXDEEMPHASIS_BMSK                                                0xc0
#define DWC_GDBGLTSSM_TXDEEMPHASIS_SHFT                                                 0x6
#define DWC_GDBGLTSSM_LTDBCLKSTATE_BMSK                                                0x18
#define DWC_GDBGLTSSM_LTDBCLKSTATE_SHFT                                                 0x3
#define DWC_GDBGLTSSM_TXSWING_BMSK                                                      0x4
#define DWC_GDBGLTSSM_TXSWING_SHFT                                                      0x2
#define DWC_GDBGLTSSM_RXTERMINATION_BMSK                                                0x2
#define DWC_GDBGLTSSM_RXTERMINATION_SHFT                                                0x1
#define DWC_GDBGLTSSM_TXONESZEROS_BMSK                                                  0x1
#define DWC_GDBGLTSSM_TXONESZEROS_SHFT                                                  0x0

#define DWC_GDBGLNMCC_ADDR(x)                                                    ((x) + 0x0000c168)
#define DWC_GDBGLNMCC_OFFS                                                       (0x0000c168)
#define DWC_GDBGLNMCC_RMSK                                                       0x7ffffeff
#define DWC_GDBGLNMCC_POR                                                        0x00000000
#define DWC_GDBGLNMCC_RSVD2_BMSK                                                 0x7ffffe00
#define DWC_GDBGLNMCC_RSVD2_SHFT                                                        0x9
#define DWC_GDBGLNMCC_LNMCC_BERC_BMSK                                                  0xff
#define DWC_GDBGLNMCC_LNMCC_BERC_SHFT                                                   0x0

#define DWC_GDBGBMU_ADDR(x)                                                      ((x) + 0x0000c16c)
#define DWC_GDBGBMU_OFFS                                                         (0x0000c16c)
#define DWC_GDBGBMU_RMSK                                                         0xffffffff
#define DWC_GDBGBMU_POR                                                          0x00000000
#define DWC_GDBGBMU_BMU_BCU_BMSK                                                 0xffffff00
#define DWC_GDBGBMU_BMU_BCU_SHFT                                                        0x8
#define DWC_GDBGBMU_BMU_DCU_BMSK                                                       0xf0
#define DWC_GDBGBMU_BMU_DCU_SHFT                                                        0x4
#define DWC_GDBGBMU_BMU_CCU_BMSK                                                        0xf
#define DWC_GDBGBMU_BMU_CCU_SHFT                                                        0x0

#define DWC_GDBGLSPMUX_HST_ADDR(x)                                               ((x) + 0x0000c170)
#define DWC_GDBGLSPMUX_HST_OFFS                                                  (0x0000c170)
#define DWC_GDBGLSPMUX_HST_RMSK                                                  0xffffffff
#define DWC_GDBGLSPMUX_HST_POR                                                   0x00000000
#define DWC_GDBGLSPMUX_HST_RSVD3_BMSK                                            0xffffc000
#define DWC_GDBGLSPMUX_HST_RSVD3_SHFT                                                   0xe
#define DWC_GDBGLSPMUX_HST_HOSTSELECT_BMSK                                           0x3fff
#define DWC_GDBGLSPMUX_HST_HOSTSELECT_SHFT                                              0x0

#define DWC_GDBGLSP_ADDR(x)                                                      ((x) + 0x0000c174)
#define DWC_GDBGLSP_OFFS                                                         (0x0000c174)
#define DWC_GDBGLSP_RMSK                                                         0xffffffff
#define DWC_GDBGLSP_POR                                                          0x00000000
#define DWC_GDBGLSP_LSPDEBUG_BMSK                                                0xffffffff
#define DWC_GDBGLSP_LSPDEBUG_SHFT                                                       0x0

#define DWC_GDBGEPINFO0_ADDR(x)                                                  ((x) + 0x0000c178)
#define DWC_GDBGEPINFO0_OFFS                                                     (0x0000c178)
#define DWC_GDBGEPINFO0_RMSK                                                     0xffffffff
#define DWC_GDBGEPINFO0_POR                                                      0x00000000
#define DWC_GDBGEPINFO0_EPDEBUG_BMSK                                             0xffffffff
#define DWC_GDBGEPINFO0_EPDEBUG_SHFT                                                    0x0

#define DWC_GDBGEPINFO1_ADDR(x)                                                  ((x) + 0x0000c17c)
#define DWC_GDBGEPINFO1_OFFS                                                     (0x0000c17c)
#define DWC_GDBGEPINFO1_RMSK                                                     0xffffffff
#define DWC_GDBGEPINFO1_POR                                                      0x00000000
#define DWC_GDBGEPINFO1_EPDEBUG_BMSK                                             0xffffffff
#define DWC_GDBGEPINFO1_EPDEBUG_SHFT                                                    0x0

#define DWC_GPRTBIMAP_HSLO_ADDR(x)                                               ((x) + 0x0000c180)
#define DWC_GPRTBIMAP_HSLO_OFFS                                                  (0x0000c180)
#define DWC_GPRTBIMAP_HSLO_RMSK                                                  0xffffffff
#define DWC_GPRTBIMAP_HSLO_POR                                                   0x00000000
#define DWC_GPRTBIMAP_HSLO_BINUM8_BMSK                                           0xf0000000
#define DWC_GPRTBIMAP_HSLO_BINUM8_SHFT                                                 0x1c
#define DWC_GPRTBIMAP_HSLO_BINUM7_BMSK                                            0xf000000
#define DWC_GPRTBIMAP_HSLO_BINUM7_SHFT                                                 0x18
#define DWC_GPRTBIMAP_HSLO_BINUM6_BMSK                                             0xf00000
#define DWC_GPRTBIMAP_HSLO_BINUM6_SHFT                                                 0x14
#define DWC_GPRTBIMAP_HSLO_BINUM5_BMSK                                              0xf0000
#define DWC_GPRTBIMAP_HSLO_BINUM5_SHFT                                                 0x10
#define DWC_GPRTBIMAP_HSLO_BINUM4_BMSK                                               0xf000
#define DWC_GPRTBIMAP_HSLO_BINUM4_SHFT                                                  0xc
#define DWC_GPRTBIMAP_HSLO_BINUM3_BMSK                                                0xf00
#define DWC_GPRTBIMAP_HSLO_BINUM3_SHFT                                                  0x8
#define DWC_GPRTBIMAP_HSLO_BINUM2_BMSK                                                 0xf0
#define DWC_GPRTBIMAP_HSLO_BINUM2_SHFT                                                  0x4
#define DWC_GPRTBIMAP_HSLO_BINUM1_BMSK                                                  0xf
#define DWC_GPRTBIMAP_HSLO_BINUM1_SHFT                                                  0x0

#define DWC_GPRTBIMAP_HSHI_ADDR(x)                                               ((x) + 0x0000c184)
#define DWC_GPRTBIMAP_HSHI_OFFS                                                  (0x0000c184)
#define DWC_GPRTBIMAP_HSHI_RMSK                                                  0xffffffff
#define DWC_GPRTBIMAP_HSHI_POR                                                   0x00000000
#define DWC_GPRTBIMAP_HSHI_RSVD16_BMSK                                           0xf0000000
#define DWC_GPRTBIMAP_HSHI_RSVD16_SHFT                                                 0x1c
#define DWC_GPRTBIMAP_HSHI_BINUM15_BMSK                                           0xf000000
#define DWC_GPRTBIMAP_HSHI_BINUM15_SHFT                                                0x18
#define DWC_GPRTBIMAP_HSHI_BINUM14_BMSK                                            0xf00000
#define DWC_GPRTBIMAP_HSHI_BINUM14_SHFT                                                0x14
#define DWC_GPRTBIMAP_HSHI_BINUM13_BMSK                                             0xf0000
#define DWC_GPRTBIMAP_HSHI_BINUM13_SHFT                                                0x10
#define DWC_GPRTBIMAP_HSHI_BINUM12_BMSK                                              0xf000
#define DWC_GPRTBIMAP_HSHI_BINUM12_SHFT                                                 0xc
#define DWC_GPRTBIMAP_HSHI_BINUM11_BMSK                                               0xf00
#define DWC_GPRTBIMAP_HSHI_BINUM11_SHFT                                                 0x8
#define DWC_GPRTBIMAP_HSHI_BINUM10_BMSK                                                0xf0
#define DWC_GPRTBIMAP_HSHI_BINUM10_SHFT                                                 0x4
#define DWC_GPRTBIMAP_HSHI_BINUM9_BMSK                                                  0xf
#define DWC_GPRTBIMAP_HSHI_BINUM9_SHFT                                                  0x0

#define DWC_GPRTBIMAP_FSLO_ADDR(x)                                               ((x) + 0x0000c188)
#define DWC_GPRTBIMAP_FSLO_OFFS                                                  (0x0000c188)
#define DWC_GPRTBIMAP_FSLO_RMSK                                                  0xffffffff
#define DWC_GPRTBIMAP_FSLO_POR                                                   0x00000000
#define DWC_GPRTBIMAP_FSLO_BINUM8_BMSK                                           0xf0000000
#define DWC_GPRTBIMAP_FSLO_BINUM8_SHFT                                                 0x1c
#define DWC_GPRTBIMAP_FSLO_BINUM7_BMSK                                            0xf000000
#define DWC_GPRTBIMAP_FSLO_BINUM7_SHFT                                                 0x18
#define DWC_GPRTBIMAP_FSLO_BINUM6_BMSK                                             0xf00000
#define DWC_GPRTBIMAP_FSLO_BINUM6_SHFT                                                 0x14
#define DWC_GPRTBIMAP_FSLO_BINUM5_BMSK                                              0xf0000
#define DWC_GPRTBIMAP_FSLO_BINUM5_SHFT                                                 0x10
#define DWC_GPRTBIMAP_FSLO_BINUM4_BMSK                                               0xf000
#define DWC_GPRTBIMAP_FSLO_BINUM4_SHFT                                                  0xc
#define DWC_GPRTBIMAP_FSLO_BINUM3_BMSK                                                0xf00
#define DWC_GPRTBIMAP_FSLO_BINUM3_SHFT                                                  0x8
#define DWC_GPRTBIMAP_FSLO_BINUM2_BMSK                                                 0xf0
#define DWC_GPRTBIMAP_FSLO_BINUM2_SHFT                                                  0x4
#define DWC_GPRTBIMAP_FSLO_BINUM1_BMSK                                                  0xf
#define DWC_GPRTBIMAP_FSLO_BINUM1_SHFT                                                  0x0

#define DWC_GPRTBIMAP_FSHI_ADDR(x)                                               ((x) + 0x0000c18c)
#define DWC_GPRTBIMAP_FSHI_OFFS                                                  (0x0000c18c)
#define DWC_GPRTBIMAP_FSHI_RMSK                                                  0xffffffff
#define DWC_GPRTBIMAP_FSHI_POR                                                   0x00000000
#define DWC_GPRTBIMAP_FSHI_RSVD16_BMSK                                           0xf0000000
#define DWC_GPRTBIMAP_FSHI_RSVD16_SHFT                                                 0x1c
#define DWC_GPRTBIMAP_FSHI_BINUM15_BMSK                                           0xf000000
#define DWC_GPRTBIMAP_FSHI_BINUM15_SHFT                                                0x18
#define DWC_GPRTBIMAP_FSHI_BINUM14_BMSK                                            0xf00000
#define DWC_GPRTBIMAP_FSHI_BINUM14_SHFT                                                0x14
#define DWC_GPRTBIMAP_FSHI_BINUM13_BMSK                                             0xf0000
#define DWC_GPRTBIMAP_FSHI_BINUM13_SHFT                                                0x10
#define DWC_GPRTBIMAP_FSHI_BINUM12_BMSK                                              0xf000
#define DWC_GPRTBIMAP_FSHI_BINUM12_SHFT                                                 0xc
#define DWC_GPRTBIMAP_FSHI_BINUM11_BMSK                                               0xf00
#define DWC_GPRTBIMAP_FSHI_BINUM11_SHFT                                                 0x8
#define DWC_GPRTBIMAP_FSHI_BINUM10_BMSK                                                0xf0
#define DWC_GPRTBIMAP_FSHI_BINUM10_SHFT                                                 0x4
#define DWC_GPRTBIMAP_FSHI_BINUM9_BMSK                                                  0xf
#define DWC_GPRTBIMAP_FSHI_BINUM9_SHFT                                                  0x0

#define DWC_GHWPARAMS8_ADDR(x)                                                   ((x) + 0x0000c600)
#define DWC_GHWPARAMS8_OFFS                                                      (0x0000c600)
#define DWC_GHWPARAMS8_RMSK                                                      0xffffffff
#define DWC_GHWPARAMS8_POR                                                       0x0000065c
#define DWC_GHWPARAMS8_GHWPARAMS8_32_0_BMSK                                      0xffffffff
#define DWC_GHWPARAMS8_GHWPARAMS8_32_0_SHFT                                             0x0

#define DWC_DEPCMDPAR2_ADDR(base,p)                            ((base) + 0x0000c800 + 0x10 * (p))
#define DWC_DEPCMDPAR2_OFFS(p)                                 (0x0000c800 + 0x10 * (p))
#define DWC_DEPCMDPAR2_RMSK                                    0xffffffff
#define DWC_DEPCMDPAR2_MAXp                                            31
#define DWC_DEPCMDPAR2_POR                                     0x00000000
#define DWC_DEPCMDPAR2_PARAMETER_BMSK                          0xffffffff
#define DWC_DEPCMDPAR2_PARAMETER_SHFT                                 0x0

#define DWC_DEPCMDPAR1_ADDR(base,p)                            ((base) + 0x0000c804 + 0x10 * (p))
#define DWC_DEPCMDPAR1_OFFS(p)                                 (0x0000c804 + 0x10 * (p))
#define DWC_DEPCMDPAR1_RMSK                                    0xffffffff
#define DWC_DEPCMDPAR1_MAXp                                            31
#define DWC_DEPCMDPAR1_POR                                     0x00000000
#define DWC_DEPCMDPAR1_PARAMETER_BMSK                          0xffffffff
#define DWC_DEPCMDPAR1_PARAMETER_SHFT                                 0x0

#define DWC_DEPCMDPAR0_ADDR(base,p)                            ((base) + 0x0000c808 + 0x10 * (p))
#define DWC_DEPCMDPAR0_OFFS(p)                                 (0x0000c808 + 0x10 * (p))
#define DWC_DEPCMDPAR0_RMSK                                    0xffffffff
#define DWC_DEPCMDPAR0_MAXp                                            31
#define DWC_DEPCMDPAR0_POR                                     0x00000000
#define DWC_DEPCMDPAR0_PARAMETER_BMSK                          0xffffffff
#define DWC_DEPCMDPAR0_PARAMETER_SHFT                                 0x0

#define DWC_DEPCMD_ADDR(base,p)                                ((base) + 0x0000c80c + 0x10 * (p))
#define DWC_DEPCMD_OFFS(p)                                     (0x0000c80c + 0x10 * (p))
#define DWC_DEPCMD_RMSK                                        0xffffffff
#define DWC_DEPCMD_MAXp                                                31
#define DWC_DEPCMD_POR                                         0x00000000
#define DWC_DEPCMD_COMMANDPARAM_BMSK                           0xffff0000
#define DWC_DEPCMD_COMMANDPARAM_SHFT                                 0x10
#define DWC_DEPCMD_CMDSTATUS_BMSK                                  0xf000
#define DWC_DEPCMD_CMDSTATUS_SHFT                                     0xc
#define DWC_DEPCMD_HIPRI_FORCERM_BMSK                               0x800
#define DWC_DEPCMD_HIPRI_FORCERM_SHFT                                 0xb
#define DWC_DEPCMD_CMDACT_BMSK                                      0x400
#define DWC_DEPCMD_CMDACT_SHFT                                        0xa
#define DWC_DEPCMD_R39_BMSK                                         0x200
#define DWC_DEPCMD_R39_SHFT                                           0x9
#define DWC_DEPCMD_CMDIOC_BMSK                                      0x100
#define DWC_DEPCMD_CMDIOC_SHFT                                        0x8
#define DWC_DEPCMD_RSVD90_BMSK                                       0xf0
#define DWC_DEPCMD_RSVD90_SHFT                                        0x4
#define DWC_DEPCMD_CMDTYP_BMSK                                        0xf
#define DWC_DEPCMD_CMDTYP_SHFT                                        0x0

#define DWC_DCFG_ADDR(x)                                                         ((x) + 0x0000c700)
#define DWC_DCFG_OFFS                                                            (0x0000c700)
#define DWC_DCFG_RMSK                                                            0xffffffff
#define DWC_DCFG_POR                                                             0x00080804
#define DWC_DCFG_RSVD9_BMSK                                                      0xff000000
#define DWC_DCFG_RSVD9_SHFT                                                            0x18
#define DWC_DCFG_IGNSTRMPP_BMSK                                                    0x800000
#define DWC_DCFG_IGNSTRMPP_SHFT                                                        0x17
#define DWC_DCFG_LPMCAP_BMSK                                                       0x400000
#define DWC_DCFG_LPMCAP_SHFT                                                           0x16
#define DWC_DCFG_NUMP_BMSK                                                         0x3e0000
#define DWC_DCFG_NUMP_SHFT                                                             0x11
#define DWC_DCFG_INTRNUM_BMSK                                                       0x1f000
#define DWC_DCFG_INTRNUM_SHFT                                                           0xc
#define DWC_DCFG_PERFRINT_BMSK                                                        0xc00
#define DWC_DCFG_PERFRINT_SHFT                                                          0xa
#define DWC_DCFG_DEVADDR_BMSK                                                         0x3f8
#define DWC_DCFG_DEVADDR_SHFT                                                           0x3
#define DWC_DCFG_DEVSPD_BMSK                                                            0x7
#define DWC_DCFG_DEVSPD_SHFT                                                            0x0

#define DWC_DCTL_ADDR(x)                                                         ((x) + 0x0000c704)
#define DWC_DCTL_OFFS                                                            (0x0000c704)
#define DWC_DCTL_RMSK                                                            0xffffffff
#define DWC_DCTL_POR                                                             0x00000000
#define DWC_DCTL_RUN_STOP_BMSK                                                   0x80000000
#define DWC_DCTL_RUN_STOP_SHFT                                                         0x1f
#define DWC_DCTL_CSFTRST_BMSK                                                    0x40000000
#define DWC_DCTL_CSFTRST_SHFT                                                          0x1e
#define DWC_DCTL_LSFTRST_BMSK                                                    0x20000000
#define DWC_DCTL_LSFTRST_SHFT                                                          0x1d
#define DWC_DCTL_HIRDTHRES_BMSK                                                  0x1f000000
#define DWC_DCTL_HIRDTHRES_SHFT                                                        0x18
#define DWC_DCTL_APPL1RES_BMSK                                                     0x800000
#define DWC_DCTL_APPL1RES_SHFT                                                         0x17
#define DWC_DCTL_RSVD40_BMSK                                                       0x700000
#define DWC_DCTL_RSVD40_SHFT                                                           0x14
#define DWC_DCTL_HIBERNATIONEN_BMSK                                                 0x80000
#define DWC_DCTL_HIBERNATIONEN_SHFT                                                    0x13
#define DWC_DCTL_L1HIBERNATIONEN_BMSK                                               0x40000
#define DWC_DCTL_L1HIBERNATIONEN_SHFT                                                  0x12
#define DWC_DCTL_CRS_BMSK                                                           0x20000
#define DWC_DCTL_CRS_SHFT                                                              0x11
#define DWC_DCTL_CSS_BMSK                                                           0x10000
#define DWC_DCTL_CSS_SHFT                                                              0x10
#define DWC_DCTL_RSVD41_BMSK                                                         0xe000
#define DWC_DCTL_RSVD41_SHFT                                                            0xd
#define DWC_DCTL_INITU2ENA_BMSK                                                      0x1000
#define DWC_DCTL_INITU2ENA_SHFT                                                         0xc
#define DWC_DCTL_ACCEPTU2ENA_BMSK                                                     0x800
#define DWC_DCTL_ACCEPTU2ENA_SHFT                                                       0xb
#define DWC_DCTL_INITU1ENA_BMSK                                                       0x400
#define DWC_DCTL_INITU1ENA_SHFT                                                         0xa
#define DWC_DCTL_ACCEPTU1ENA_BMSK                                                     0x200
#define DWC_DCTL_ACCEPTU1ENA_SHFT                                                       0x9
#define DWC_DCTL_ULSTCHNGREQ_BMSK                                                     0x1e0
#define DWC_DCTL_ULSTCHNGREQ_SHFT                                                       0x5
#define DWC_DCTL_TSTCTL_BMSK                                                           0x1e
#define DWC_DCTL_TSTCTL_SHFT                                                            0x1
#define DWC_DCTL_RSVD0_BMSK                                                             0x1
#define DWC_DCTL_RSVD0_SHFT                                                             0x0

#define DWC_DEVTEN_ADDR(x)                                                       ((x) + 0x0000c708)
#define DWC_DEVTEN_OFFS                                                          (0x0000c708)
#define DWC_DEVTEN_RMSK                                                          0xffffffff
#define DWC_DEVTEN_POR                                                           0x00000000
#define DWC_DEVTEN_RSVD2_BMSK                                                    0xffffc000
#define DWC_DEVTEN_RSVD2_SHFT                                                           0xe
#define DWC_DEVTEN_U2INACTTIMOUTRCVDEN_BMSK                                          0x2000
#define DWC_DEVTEN_U2INACTTIMOUTRCVDEN_SHFT                                             0xd
#define DWC_DEVTEN_VENDEVTSTRCVDEN_BMSK                                              0x1000
#define DWC_DEVTEN_VENDEVTSTRCVDEN_SHFT                                                 0xc
#define DWC_DEVTEN_EVNTOVERFLOWEN_BMSK                                                0x800
#define DWC_DEVTEN_EVNTOVERFLOWEN_SHFT                                                  0xb
#define DWC_DEVTEN_CMDCMPLTEN_BMSK                                                    0x400
#define DWC_DEVTEN_CMDCMPLTEN_SHFT                                                      0xa
#define DWC_DEVTEN_ERRTICERREVTEN_BMSK                                                0x200
#define DWC_DEVTEN_ERRTICERREVTEN_SHFT                                                  0x9
#define DWC_DEVTEN_RSVD39_BMSK                                                        0x100
#define DWC_DEVTEN_RSVD39_SHFT                                                          0x8
#define DWC_DEVTEN_SOFTEVTEN_BMSK                                                      0x80
#define DWC_DEVTEN_SOFTEVTEN_SHFT                                                       0x7
#define DWC_DEVTEN_EOPFEVTEN_BMSK                                                      0x40
#define DWC_DEVTEN_EOPFEVTEN_SHFT                                                       0x6
#define DWC_DEVTEN_HIBERNATIONREQEVTEN_BMSK                                            0x20
#define DWC_DEVTEN_HIBERNATIONREQEVTEN_SHFT                                             0x5
#define DWC_DEVTEN_WKUPEVTEN_BMSK                                                      0x10
#define DWC_DEVTEN_WKUPEVTEN_SHFT                                                       0x4
#define DWC_DEVTEN_ULSTCNGEN_BMSK                                                       0x8
#define DWC_DEVTEN_ULSTCNGEN_SHFT                                                       0x3
#define DWC_DEVTEN_CONNECTDONEEVTEN_BMSK                                                0x4
#define DWC_DEVTEN_CONNECTDONEEVTEN_SHFT                                                0x2
#define DWC_DEVTEN_USBRSTEVTEN_BMSK                                                     0x2
#define DWC_DEVTEN_USBRSTEVTEN_SHFT                                                     0x1
#define DWC_DEVTEN_DISSCONNEVTEN_BMSK                                                   0x1
#define DWC_DEVTEN_DISSCONNEVTEN_SHFT                                                   0x0

#define DWC_DSTS_ADDR(x)                                                         ((x) + 0x0000c70c)
#define DWC_DSTS_OFFS                                                            (0x0000c70c)
#define DWC_DSTS_RMSK                                                            0xffffffff
#define DWC_DSTS_POR                                                             0x20820004
#define DWC_DSTS_RSVD4_BMSK                                                      0xc0000000
#define DWC_DSTS_RSVD4_SHFT                                                            0x1e
#define DWC_DSTS_DCNRD_BMSK                                                      0x20000000
#define DWC_DSTS_DCNRD_SHFT                                                            0x1d
#define DWC_DSTS_RSVD3_BMSK                                                      0x10000000
#define DWC_DSTS_RSVD3_SHFT                                                            0x1c
#define DWC_DSTS_PLC_BMSK                                                         0x8000000
#define DWC_DSTS_PLC_SHFT                                                              0x1b
#define DWC_DSTS_CSC_BMSK                                                         0x4000000
#define DWC_DSTS_CSC_SHFT                                                              0x1a
#define DWC_DSTS_RSS_BMSK                                                         0x2000000
#define DWC_DSTS_RSS_SHFT                                                              0x19
#define DWC_DSTS_SSS_BMSK                                                         0x1000000
#define DWC_DSTS_SSS_SHFT                                                              0x18
#define DWC_DSTS_COREIDLE_BMSK                                                     0x800000
#define DWC_DSTS_COREIDLE_SHFT                                                         0x17
#define DWC_DSTS_DEVCTRLHLT_BMSK                                                   0x400000
#define DWC_DSTS_DEVCTRLHLT_SHFT                                                       0x16
#define DWC_DSTS_USBLNKST_BMSK                                                     0x3c0000
#define DWC_DSTS_USBLNKST_SHFT                                                         0x12
#define DWC_DSTS_RXFIFOEMPTY_BMSK                                                   0x20000
#define DWC_DSTS_RXFIFOEMPTY_SHFT                                                      0x11
#define DWC_DSTS_SOFFN_BMSK                                                         0x1fff8
#define DWC_DSTS_SOFFN_SHFT                                                             0x3
#define DWC_DSTS_CONNECTSPD_BMSK                                                        0x7
#define DWC_DSTS_CONNECTSPD_SHFT                                                        0x0

#define DWC_DGCMDPAR_ADDR(x)                                                     ((x) + 0x0000c710)
#define DWC_DGCMDPAR_OFFS                                                        (0x0000c710)
#define DWC_DGCMDPAR_RMSK                                                        0xffffffff
#define DWC_DGCMDPAR_POR                                                         0x00000000
#define DWC_DGCMDPAR_PARAMETER_BMSK                                              0xffffffff
#define DWC_DGCMDPAR_PARAMETER_SHFT                                                     0x0

#define DWC_DGCMD_ADDR(x)                                                        ((x) + 0x0000c714)
#define DWC_DGCMD_OFFS                                                           (0x0000c714)
#define DWC_DGCMD_RMSK                                                           0xffffffff
#define DWC_DGCMD_POR                                                            0x00000000
#define DWC_DGCMD_RSVD6_BMSK                                                     0xffff0000
#define DWC_DGCMD_RSVD6_SHFT                                                           0x10
#define DWC_DGCMD_CMDSTATUS_BMSK                                                     0xf000
#define DWC_DGCMD_CMDSTATUS_SHFT                                                        0xc
#define DWC_DGCMD_RSVD5_BMSK                                                          0x800
#define DWC_DGCMD_RSVD5_SHFT                                                            0xb
#define DWC_DGCMD_CMDACT_BMSK                                                         0x400
#define DWC_DGCMD_CMDACT_SHFT                                                           0xa
#define DWC_DGCMD_RSVD4_BMSK                                                          0x200
#define DWC_DGCMD_RSVD4_SHFT                                                            0x9
#define DWC_DGCMD_CMDIOC_BMSK                                                         0x100
#define DWC_DGCMD_CMDIOC_SHFT                                                           0x8
#define DWC_DGCMD_CMDTYP_BMSK                                                          0xff
#define DWC_DGCMD_CMDTYP_SHFT                                                           0x0

#define DWC_DALEPENA_ADDR(x)                                                     ((x) + 0x0000c720)
#define DWC_DALEPENA_OFFS                                                        (0x0000c720)
#define DWC_DALEPENA_RMSK                                                        0xffffffff
#define DWC_DALEPENA_POR                                                         0x00000000
#define DWC_DALEPENA_USBACTEP_BMSK                                               0xffffffff
#define DWC_DALEPENA_USBACTEP_SHFT                                                      0x0

#define DWC_OCFG_ADDR(x)                                                         ((x) + 0x0000cc00)
#define DWC_OCFG_OFFS                                                            (0x0000cc00)
#define DWC_OCFG_RMSK                                                            0xffffffff
#define DWC_OCFG_POR                                                             0x00000000
#define DWC_OCFG_RSVD0_BMSK                                                      0xffffffc0
#define DWC_OCFG_RSVD0_SHFT                                                             0x6
#define DWC_OCFG_DISPRTPWRCUTOFF_BMSK                                                  0x20
#define DWC_OCFG_DISPRTPWRCUTOFF_SHFT                                                   0x5
#define DWC_OCFG_OTGHIBDISMASK_BMSK                                                    0x10
#define DWC_OCFG_OTGHIBDISMASK_SHFT                                                     0x4
#define DWC_OCFG_OTGSFTRSTMSK_BMSK                                                      0x8
#define DWC_OCFG_OTGSFTRSTMSK_SHFT                                                      0x3
#define DWC_OCFG_OTG_VERSION_BMSK                                                       0x4
#define DWC_OCFG_OTG_VERSION_SHFT                                                       0x2
#define DWC_OCFG_HNPCAP_BMSK                                                            0x2
#define DWC_OCFG_HNPCAP_SHFT                                                            0x1
#define DWC_OCFG_SRPCAP_BMSK                                                            0x1
#define DWC_OCFG_SRPCAP_SHFT                                                            0x0

#define DWC_OCTL_ADDR(x)                                                         ((x) + 0x0000cc04)
#define DWC_OCTL_OFFS                                                            (0x0000cc04)
#define DWC_OCTL_RMSK                                                            0xffffffff
#define DWC_OCTL_POR                                                             0x00000040
#define DWC_OCTL_RSVD0_BMSK                                                      0xffffff00
#define DWC_OCTL_RSVD0_SHFT                                                             0x8
#define DWC_OCTL_OTG3_GOERR_BMSK                                                       0x80
#define DWC_OCTL_OTG3_GOERR_SHFT                                                        0x7
#define DWC_OCTL_PERIMODE_BMSK                                                         0x40
#define DWC_OCTL_PERIMODE_SHFT                                                          0x6
#define DWC_OCTL_PRTPWRCTL_BMSK                                                        0x20
#define DWC_OCTL_PRTPWRCTL_SHFT                                                         0x5
#define DWC_OCTL_HNPREQ_BMSK                                                           0x10
#define DWC_OCTL_HNPREQ_SHFT                                                            0x4
#define DWC_OCTL_SESREQ_BMSK                                                            0x8
#define DWC_OCTL_SESREQ_SHFT                                                            0x3
#define DWC_OCTL_TERMSELDLPULSE_BMSK                                                    0x4
#define DWC_OCTL_TERMSELDLPULSE_SHFT                                                    0x2
#define DWC_OCTL_DEVSETHNPEN_BMSK                                                       0x2
#define DWC_OCTL_DEVSETHNPEN_SHFT                                                       0x1
#define DWC_OCTL_HSTSETHNPEN_BMSK                                                       0x1
#define DWC_OCTL_HSTSETHNPEN_SHFT                                                       0x0

#define DWC_OEVT_ADDR(x)                                                         ((x) + 0x0000cc08)
#define DWC_OEVT_OFFS                                                            (0x0000cc08)
#define DWC_OEVT_RMSK                                                            0xffffffff
#define DWC_OEVT_POR                                                             0x00000000
#define DWC_OEVT_DEVICEMODE_BMSK                                                 0x80000000
#define DWC_OEVT_DEVICEMODE_SHFT                                                       0x1f
#define DWC_OEVT_RSVD0_BMSK                                                      0x70000000
#define DWC_OEVT_RSVD0_SHFT                                                            0x1c
#define DWC_OEVT_OTGXHCIRUNSTPSETEVNT_BMSK                                        0x8000000
#define DWC_OEVT_OTGXHCIRUNSTPSETEVNT_SHFT                                             0x1b
#define DWC_OEVT_OTGDEVRUNSTPSETEVNT_BMSK                                         0x4000000
#define DWC_OEVT_OTGDEVRUNSTPSETEVNT_SHFT                                              0x1a
#define DWC_OEVT_OTGHIBENTRYEVNT_BMSK                                             0x2000000
#define DWC_OEVT_OTGHIBENTRYEVNT_SHFT                                                  0x19
#define DWC_OEVT_OTGCONIDSTSCHNGEVNT_BMSK                                         0x1000000
#define DWC_OEVT_OTGCONIDSTSCHNGEVNT_SHFT                                              0x18
#define DWC_OEVT_HRRCONFNOTIFEVNT_BMSK                                             0x800000
#define DWC_OEVT_HRRCONFNOTIFEVNT_SHFT                                                 0x17
#define DWC_OEVT_HRRINITNOTIFEVNT_BMSK                                             0x400000
#define DWC_OEVT_HRRINITNOTIFEVNT_SHFT                                                 0x16
#define DWC_OEVT_OTGADEVIDLEEVNT_BMSK                                              0x200000
#define DWC_OEVT_OTGADEVIDLEEVNT_SHFT                                                  0x15
#define DWC_OEVT_OTGADEVBHOSTENDEVNT_BMSK                                          0x100000
#define DWC_OEVT_OTGADEVBHOSTENDEVNT_SHFT                                              0x14
#define DWC_OEVT_OTGADEVHOSTEVNT_BMSK                                               0x80000
#define DWC_OEVT_OTGADEVHOSTEVNT_SHFT                                                  0x13
#define DWC_OEVT_OTGADEVHNPCHNGEVNT_BMSK                                            0x40000
#define DWC_OEVT_OTGADEVHNPCHNGEVNT_SHFT                                               0x12
#define DWC_OEVT_OTGADEVSRPDETEVNT_BMSK                                             0x20000
#define DWC_OEVT_OTGADEVSRPDETEVNT_SHFT                                                0x11
#define DWC_OEVT_OTGADEVSESSENDDETEVNT_BMSK                                         0x10000
#define DWC_OEVT_OTGADEVSESSENDDETEVNT_SHFT                                            0x10
#define DWC_OEVT_RSVD2_BMSK                                                          0xf000
#define DWC_OEVT_RSVD2_SHFT                                                             0xc
#define DWC_OEVT_OTGBDEVBHOSTENDEVNT_BMSK                                             0x800
#define DWC_OEVT_OTGBDEVBHOSTENDEVNT_SHFT                                               0xb
#define DWC_OEVT_OTGBDEVHNPCHNGEVNT_BMSK                                              0x400
#define DWC_OEVT_OTGBDEVHNPCHNGEVNT_SHFT                                                0xa
#define DWC_OEVT_OTGBDEVSESSVLDDETEVNT_BMSK                                           0x200
#define DWC_OEVT_OTGBDEVSESSVLDDETEVNT_SHFT                                             0x9
#define DWC_OEVT_OTGBDEVVBUSCHNGEVNT_BMSK                                             0x100
#define DWC_OEVT_OTGBDEVVBUSCHNGEVNT_SHFT                                               0x8
#define DWC_OEVT_RSVD3_BMSK                                                            0xf0
#define DWC_OEVT_RSVD3_SHFT                                                             0x4
#define DWC_OEVT_BSESVLD_BMSK                                                           0x8
#define DWC_OEVT_BSESVLD_SHFT                                                           0x3
#define DWC_OEVT_HSTNEGSTS_BMSK                                                         0x4
#define DWC_OEVT_HSTNEGSTS_SHFT                                                         0x2
#define DWC_OEVT_SESREQSTS_BMSK                                                         0x2
#define DWC_OEVT_SESREQSTS_SHFT                                                         0x1
#define DWC_OEVT_OEVTERROR_BMSK                                                         0x1
#define DWC_OEVT_OEVTERROR_SHFT                                                         0x0

#define DWC_OEVTEN_ADDR(x)                                                       ((x) + 0x0000cc0c)
#define DWC_OEVTEN_OFFS                                                          (0x0000cc0c)
#define DWC_OEVTEN_RMSK                                                          0xffffffff
#define DWC_OEVTEN_POR                                                           0x00000000
#define DWC_OEVTEN_RSVD0_BMSK                                                    0xf0000000
#define DWC_OEVTEN_RSVD0_SHFT                                                          0x1c
#define DWC_OEVTEN_OTGXHCIRUNSTPSETEVNTEN_BMSK                                    0x8000000
#define DWC_OEVTEN_OTGXHCIRUNSTPSETEVNTEN_SHFT                                         0x1b
#define DWC_OEVTEN_OTGDEVRUNSTPSETEVNTEN_BMSK                                     0x4000000
#define DWC_OEVTEN_OTGDEVRUNSTPSETEVNTEN_SHFT                                          0x1a
#define DWC_OEVTEN_OTGHIBENTRYEVNTEN_BMSK                                         0x2000000
#define DWC_OEVTEN_OTGHIBENTRYEVNTEN_SHFT                                              0x19
#define DWC_OEVTEN_OTGCONIDSTSCHNGEVNTEN_BMSK                                     0x1000000
#define DWC_OEVTEN_OTGCONIDSTSCHNGEVNTEN_SHFT                                          0x18
#define DWC_OEVTEN_HRRCONFNOTIFEVNTEN_BMSK                                         0x800000
#define DWC_OEVTEN_HRRCONFNOTIFEVNTEN_SHFT                                             0x17
#define DWC_OEVTEN_HRRINITNOTIFEVNTEN_BMSK                                         0x400000
#define DWC_OEVTEN_HRRINITNOTIFEVNTEN_SHFT                                             0x16
#define DWC_OEVTEN_OTGADEVIDLEEVNTEN_BMSK                                          0x200000
#define DWC_OEVTEN_OTGADEVIDLEEVNTEN_SHFT                                              0x15
#define DWC_OEVTEN_OTGADEVBHOSTENDEVNTEN_BMSK                                      0x100000
#define DWC_OEVTEN_OTGADEVBHOSTENDEVNTEN_SHFT                                          0x14
#define DWC_OEVTEN_OTGADEVHOSTEVNTEN_BMSK                                           0x80000
#define DWC_OEVTEN_OTGADEVHOSTEVNTEN_SHFT                                              0x13
#define DWC_OEVTEN_OTGADEVHNPCHNGEVNTEN_BMSK                                        0x40000
#define DWC_OEVTEN_OTGADEVHNPCHNGEVNTEN_SHFT                                           0x12
#define DWC_OEVTEN_OTGADEVSRPDETEVNTEN_BMSK                                         0x20000
#define DWC_OEVTEN_OTGADEVSRPDETEVNTEN_SHFT                                            0x11
#define DWC_OEVTEN_OTGADEVSESSENDDETEVNTEN_BMSK                                     0x10000
#define DWC_OEVTEN_OTGADEVSESSENDDETEVNTEN_SHFT                                        0x10
#define DWC_OEVTEN_RSVD2_BMSK                                                        0xf000
#define DWC_OEVTEN_RSVD2_SHFT                                                           0xc
#define DWC_OEVTEN_OTGBDEVBHOSTENDEVNTEN_BMSK                                         0x800
#define DWC_OEVTEN_OTGBDEVBHOSTENDEVNTEN_SHFT                                           0xb
#define DWC_OEVTEN_OTGBDEVHNPCHNGEVNTEN_BMSK                                          0x400
#define DWC_OEVTEN_OTGBDEVHNPCHNGEVNTEN_SHFT                                            0xa
#define DWC_OEVTEN_OTGBDEVSESSVLDDETEVNTEN_BMSK                                       0x200
#define DWC_OEVTEN_OTGBDEVSESSVLDDETEVNTEN_SHFT                                         0x9
#define DWC_OEVTEN_OTGBDEVVBUSCHNGEVNTEN_BMSK                                         0x100
#define DWC_OEVTEN_OTGBDEVVBUSCHNGEVNTEN_SHFT                                           0x8
#define DWC_OEVTEN_RSVD3_BMSK                                                          0xff
#define DWC_OEVTEN_RSVD3_SHFT                                                           0x0

#define DWC_OSTS_ADDR(x)                                                         ((x) + 0x0000cc10)
#define DWC_OSTS_OFFS                                                            (0x0000cc10)
#define DWC_OSTS_RMSK                                                            0xffffffff
#define DWC_OSTS_POR                                                             0x00000819
#define DWC_OSTS_RSVD0_BMSK                                                      0xffffc000
#define DWC_OSTS_RSVD0_SHFT                                                             0xe
#define DWC_OSTS_DEVRUNSTP_BMSK                                                      0x2000
#define DWC_OSTS_DEVRUNSTP_SHFT                                                         0xd
#define DWC_OSTS_XHCIRUNSTP_BMSK                                                     0x1000
#define DWC_OSTS_XHCIRUNSTP_SHFT                                                        0xc
#define DWC_OSTS_OTGSTATE_BMSK                                                        0xf00
#define DWC_OSTS_OTGSTATE_SHFT                                                          0x8
#define DWC_OSTS_RSVD1_BMSK                                                            0xe0
#define DWC_OSTS_RSVD1_SHFT                                                             0x5
#define DWC_OSTS_PERIPHERALSTATE_BMSK                                                  0x10
#define DWC_OSTS_PERIPHERALSTATE_SHFT                                                   0x4
#define DWC_OSTS_XHCIPRTPOWER_BMSK                                                      0x8
#define DWC_OSTS_XHCIPRTPOWER_SHFT                                                      0x3
#define DWC_OSTS_BSESVLD_BMSK                                                           0x4
#define DWC_OSTS_BSESVLD_SHFT                                                           0x2
#define DWC_OSTS_ASESVLD_BMSK                                                           0x2
#define DWC_OSTS_ASESVLD_SHFT                                                           0x1
#define DWC_OSTS_CONIDSTS_BMSK                                                          0x1
#define DWC_OSTS_CONIDSTS_SHFT                                                          0x0

#define DWC_ADPCFG_ADDR(x)                                                       ((x) + 0x0000cc20)
#define DWC_ADPCFG_OFFS                                                          (0x0000cc20)
#define DWC_ADPCFG_RMSK                                                          0xffffffff
#define DWC_ADPCFG_POR                                                           0x00000000
#define DWC_ADPCFG_PRBPER_BMSK                                                   0xc0000000
#define DWC_ADPCFG_PRBPER_SHFT                                                         0x1e
#define DWC_ADPCFG_PRBDELTA_BMSK                                                 0x30000000
#define DWC_ADPCFG_PRBDELTA_SHFT                                                       0x1c
#define DWC_ADPCFG_PRBDSCHG_BMSK                                                  0xc000000
#define DWC_ADPCFG_PRBDSCHG_SHFT                                                       0x1a
#define DWC_ADPCFG_RSVD0_BMSK                                                     0x3ffffff
#define DWC_ADPCFG_RSVD0_SHFT                                                           0x0

#define DWC_ADPCTL_ADDR(x)                                                       ((x) + 0x0000cc24)
#define DWC_ADPCTL_OFFS                                                          (0x0000cc24)
#define DWC_ADPCTL_RMSK                                                          0xffffffff
#define DWC_ADPCTL_POR                                                           0x00000000
#define DWC_ADPCTL_RSVD0_BMSK                                                    0xe0000000
#define DWC_ADPCTL_RSVD0_SHFT                                                          0x1d
#define DWC_ADPCTL_ENAPRB_BMSK                                                   0x10000000
#define DWC_ADPCTL_ENAPRB_SHFT                                                         0x1c
#define DWC_ADPCTL_ENASNS_BMSK                                                    0x8000000
#define DWC_ADPCTL_ENASNS_SHFT                                                         0x1b
#define DWC_ADPCTL_ADPEN_BMSK                                                     0x4000000
#define DWC_ADPCTL_ADPEN_SHFT                                                          0x1a
#define DWC_ADPCTL_ADPRES_BMSK                                                    0x2000000
#define DWC_ADPCTL_ADPRES_SHFT                                                         0x19
#define DWC_ADPCTL_WB_BMSK                                                        0x1000000
#define DWC_ADPCTL_WB_SHFT                                                             0x18
#define DWC_ADPCTL_RSVD1_BMSK                                                      0xffffff
#define DWC_ADPCTL_RSVD1_SHFT                                                           0x0

#define DWC_ADPEVT_ADDR(x)                                                       ((x) + 0x0000cc28)
#define DWC_ADPEVT_OFFS                                                          (0x0000cc28)
#define DWC_ADPEVT_RMSK                                                          0xffffffff
#define DWC_ADPEVT_POR                                                           0x00000000
#define DWC_ADPEVT_RSVD0_BMSK                                                    0xe0000000
#define DWC_ADPEVT_RSVD0_SHFT                                                          0x1d
#define DWC_ADPEVT_ADPPRBEVNT_BMSK                                               0x10000000
#define DWC_ADPEVT_ADPPRBEVNT_SHFT                                                     0x1c
#define DWC_ADPEVT_ADPSNSEVNT_BMSK                                                0x8000000
#define DWC_ADPEVT_ADPSNSEVNT_SHFT                                                     0x1b
#define DWC_ADPEVT_ADPTMOUTEVNT_BMSK                                              0x4000000
#define DWC_ADPEVT_ADPTMOUTEVNT_SHFT                                                   0x1a
#define DWC_ADPEVT_ADPRSTCMPLTEVNT_BMSK                                           0x2000000
#define DWC_ADPEVT_ADPRSTCMPLTEVNT_SHFT                                                0x19
#define DWC_ADPEVT_RSVD1_BMSK                                                     0x1fff800
#define DWC_ADPEVT_RSVD1_SHFT                                                           0xb
#define DWC_ADPEVT_RTIM_BMSK                                                          0x7ff
#define DWC_ADPEVT_RTIM_SHFT                                                            0x0

#define DWC_ADPEVTEN_ADDR(x)                                                     ((x) + 0x0000cc2c)
#define DWC_ADPEVTEN_OFFS                                                        (0x0000cc2c)
#define DWC_ADPEVTEN_RMSK                                                        0xffffffff
#define DWC_ADPEVTEN_POR                                                         0x00000000
#define DWC_ADPEVTEN_RSVD0_BMSK                                                  0xe0000000
#define DWC_ADPEVTEN_RSVD0_SHFT                                                        0x1d
#define DWC_ADPEVTEN_ADPPRBEVNTEN_BMSK                                           0x10000000
#define DWC_ADPEVTEN_ADPPRBEVNTEN_SHFT                                                 0x1c
#define DWC_ADPEVTEN_ADPSNSEVNTEN_BMSK                                            0x8000000
#define DWC_ADPEVTEN_ADPSNSEVNTEN_SHFT                                                 0x1b
#define DWC_ADPEVTEN_ADPTMOUTEVNTEN_BMSK                                          0x4000000
#define DWC_ADPEVTEN_ADPTMOUTEVNTEN_SHFT                                               0x1a
#define DWC_ADPEVTEN_ADPRSTCMPLTEVNTEN_BMSK                                       0x2000000
#define DWC_ADPEVTEN_ADPRSTCMPLTEVNTEN_SHFT                                            0x19
#define DWC_ADPEVTEN_RSVD1_BMSK                                                   0x1ffffff
#define DWC_ADPEVTEN_RSVD1_SHFT                                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: USB30_QSCRATCH
 *--------------------------------------------------------------------------*/
#define USB30_QSCRATCH_REG_BASE_OFFS                                      0x000f8800
#define DWC_HS_PHY_CTRL_ADDR(x)                                          ((x) + 0x00000010)
#define DWC_HS_PHY_CTRL_OFFS                                             (0x00000010)
#define DWC_HS_PHY_CTRL_RMSK                                              0x7ffffff
#define DWC_HS_PHY_CTRL_POR                                              0x072203b2
#define DWC_HS_PHY_CTRL_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK                     0x4000000
#define DWC_HS_PHY_CTRL_CLAMP_MPM_DPSE_DMSE_EN_N_SHFT                          0x1a
#define DWC_HS_PHY_CTRL_FREECLK_SEL_BMSK                                  0x2000000
#define DWC_HS_PHY_CTRL_FREECLK_SEL_SHFT                                       0x19
#define DWC_HS_PHY_CTRL_DMSEHV_CLAMP_EN_N_BMSK                            0x1000000
#define DWC_HS_PHY_CTRL_DMSEHV_CLAMP_EN_N_SHFT                                 0x18
#define DWC_HS_PHY_CTRL_USB2_SUSPEND_N_SEL_BMSK                            0x800000
#define DWC_HS_PHY_CTRL_USB2_SUSPEND_N_SEL_SHFT                                0x17
#define DWC_HS_PHY_CTRL_USB2_SUSPEND_N_BMSK                                0x400000
#define DWC_HS_PHY_CTRL_USB2_SUSPEND_N_SHFT                                    0x16
#define DWC_HS_PHY_CTRL_USB2_UTMI_CLK_EN_BMSK                              0x200000
#define DWC_HS_PHY_CTRL_USB2_UTMI_CLK_EN_SHFT                                  0x15
#define DWC_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK                           0x100000
#define DWC_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT                               0x14
#define DWC_HS_PHY_CTRL_AUTORESUME_BMSK                                     0x80000
#define DWC_HS_PHY_CTRL_AUTORESUME_SHFT                                        0x13
#define DWC_HS_PHY_CTRL_USE_CLKCORE_BMSK                                    0x40000
#define DWC_HS_PHY_CTRL_USE_CLKCORE_SHFT                                       0x12
#define DWC_HS_PHY_CTRL_DPSEHV_CLAMP_EN_N_BMSK                              0x20000
#define DWC_HS_PHY_CTRL_DPSEHV_CLAMP_EN_N_SHFT                                 0x11
#define DWC_HS_PHY_CTRL_IDHV_INTEN_BMSK                                     0x10000
#define DWC_HS_PHY_CTRL_IDHV_INTEN_SHFT                                        0x10
#define DWC_HS_PHY_CTRL_OTGSESSVLDHV_INTEN_BMSK                              0x8000
#define DWC_HS_PHY_CTRL_OTGSESSVLDHV_INTEN_SHFT                                 0xf
#define DWC_HS_PHY_CTRL_VBUSVLDEXTSEL0_BMSK                                  0x4000
#define DWC_HS_PHY_CTRL_VBUSVLDEXTSEL0_SHFT                                     0xe
#define DWC_HS_PHY_CTRL_VBUSVLDEXT0_BMSK                                     0x2000
#define DWC_HS_PHY_CTRL_VBUSVLDEXT0_SHFT                                        0xd
#define DWC_HS_PHY_CTRL_OTGDISABLE0_BMSK                                     0x1000
#define DWC_HS_PHY_CTRL_OTGDISABLE0_SHFT                                        0xc
#define DWC_HS_PHY_CTRL_COMMONONN_BMSK                                        0x800
#define DWC_HS_PHY_CTRL_COMMONONN_SHFT                                          0xb
#define DWC_HS_PHY_CTRL_ULPIPOR_BMSK                                          0x400
#define DWC_HS_PHY_CTRL_ULPIPOR_SHFT                                            0xa
#define DWC_HS_PHY_CTRL_ID_HV_CLAMP_EN_N_BMSK                                 0x200
#define DWC_HS_PHY_CTRL_ID_HV_CLAMP_EN_N_SHFT                                   0x9
#define DWC_HS_PHY_CTRL_OTGSESSVLD_HV_CLAMP_EN_N_BMSK                         0x100
#define DWC_HS_PHY_CTRL_OTGSESSVLD_HV_CLAMP_EN_N_SHFT                           0x8
#define DWC_HS_PHY_CTRL_CLAMP_EN_N_BMSK                                        0x80
#define DWC_HS_PHY_CTRL_CLAMP_EN_N_SHFT                                         0x7
#define DWC_HS_PHY_CTRL_FSEL_BMSK                                              0x70
#define DWC_HS_PHY_CTRL_FSEL_SHFT                                               0x4
#define DWC_HS_PHY_CTRL_REFCLKOUT_EN_BMSK                                       0x8
#define DWC_HS_PHY_CTRL_REFCLKOUT_EN_SHFT                                       0x3
#define DWC_HS_PHY_CTRL_SIDDQ_BMSK                                              0x4
#define DWC_HS_PHY_CTRL_SIDDQ_SHFT                                              0x2
#define DWC_HS_PHY_CTRL_RETENABLEN_BMSK                                         0x2
#define DWC_HS_PHY_CTRL_RETENABLEN_SHFT                                         0x1
#define DWC_HS_PHY_CTRL_POR_BMSK                                                0x1
#define DWC_HS_PHY_CTRL_POR_SHFT                                                0x0

#define DWC_CHARGING_DET_CTRL_ADDR(x)                                    ((x) + 0x00000018)
#define DWC_CHARGING_DET_CTRL_PHYS(x)                                    ((x) + 0x00000018)
#define DWC_CHARGING_DET_CTRL_OFFS                                       (0x00000018)
#define DWC_CHARGING_DET_CTRL_RMSK                                             0x3f
#define DWC_CHARGING_DET_CTRL_POR                                        0x00000000
#define DWC_CHARGING_DET_CTRL_VDATDETENB0_BMSK                                 0x20
#define DWC_CHARGING_DET_CTRL_VDATDETENB0_SHFT                                  0x5
#define DWC_CHARGING_DET_CTRL_VDATSRCENB0_BMSK                                 0x10
#define DWC_CHARGING_DET_CTRL_VDATSRCENB0_SHFT                                  0x4
#define DWC_CHARGING_DET_CTRL_VDMSRCAUTO_BMSK                                   0x8
#define DWC_CHARGING_DET_CTRL_VDMSRCAUTO_SHFT                                   0x3
#define DWC_CHARGING_DET_CTRL_CHRGSEL0_BMSK                                     0x4
#define DWC_CHARGING_DET_CTRL_CHRGSEL0_SHFT                                     0x2
#define DWC_CHARGING_DET_CTRL_DCDENB0_BMSK                                      0x2
#define DWC_CHARGING_DET_CTRL_DCDENB0_SHFT                                      0x1
#define DWC_CHARGING_DET_CTRL_ACAENB0_BMSK                                      0x1
#define DWC_CHARGING_DET_CTRL_ACAENB0_SHFT                                      0x0

#define DWC_CHARGING_DET_OUTPUT_ADDR(x)                                  ((x) + 0x0000001c)
#define DWC_CHARGING_DET_OUTPUT_PHYS(x)                                  ((x) + 0x0000001c)
#define DWC_CHARGING_DET_OUTPUT_OFFS                                     (0x0000001c)
#define DWC_CHARGING_DET_OUTPUT_RMSK                                          0xfff
#define DWC_CHARGING_DET_OUTPUT_POR                                      0x00000000
#define DWC_CHARGING_DET_OUTPUT_DMSEHV_BMSK                                   0x800
#define DWC_CHARGING_DET_OUTPUT_DMSEHV_SHFT                                     0xb
#define DWC_CHARGING_DET_OUTPUT_DPSEHV_BMSK                                   0x400
#define DWC_CHARGING_DET_OUTPUT_DPSEHV_SHFT                                     0xa
#define DWC_CHARGING_DET_OUTPUT_LINESTATE_BMSK                                0x300
#define DWC_CHARGING_DET_OUTPUT_LINESTATE_SHFT                                  0x8
#define DWC_CHARGING_DET_OUTPUT_RIDFLOAT_N_BMSK                                0x80
#define DWC_CHARGING_DET_OUTPUT_RIDFLOAT_N_SHFT                                 0x7
#define DWC_CHARGING_DET_OUTPUT_RIDFLOAT_BMSK                                  0x40
#define DWC_CHARGING_DET_OUTPUT_RIDFLOAT_SHFT                                   0x6
#define DWC_CHARGING_DET_OUTPUT_RIDGND_BMSK                                    0x20
#define DWC_CHARGING_DET_OUTPUT_RIDGND_SHFT                                     0x5
#define DWC_CHARGING_DET_OUTPUT_RIDC_BMSK                                      0x10
#define DWC_CHARGING_DET_OUTPUT_RIDC_SHFT                                       0x4
#define DWC_CHARGING_DET_OUTPUT_RIDB_BMSK                                       0x8
#define DWC_CHARGING_DET_OUTPUT_RIDB_SHFT                                       0x3
#define DWC_CHARGING_DET_OUTPUT_RIDA_BMSK                                       0x4
#define DWC_CHARGING_DET_OUTPUT_RIDA_SHFT                                       0x2
#define DWC_CHARGING_DET_OUTPUT_DCDOUT_BMSK                                     0x2
#define DWC_CHARGING_DET_OUTPUT_DCDOUT_SHFT                                     0x1
#define DWC_CHARGING_DET_OUTPUT_CHGDET_BMSK                                     0x1
#define DWC_CHARGING_DET_OUTPUT_CHGDET_SHFT                                     0x0

#define DWC_ALT_INTERRUPT_EN_ADDR(x)                                     ((x) + 0x00000020)
#define DWC_ALT_INTERRUPT_EN_PHYS(x)                                     ((x) + 0x00000020)
#define DWC_ALT_INTERRUPT_EN_OFFS                                        (0x00000020)
#define DWC_ALT_INTERRUPT_EN_RMSK                                             0xfff
#define DWC_ALT_INTERRUPT_EN_POR                                         0x00000000
#define DWC_ALT_INTERRUPT_EN_DMSEHV_LO_INTEN_BMSK                             0x800
#define DWC_ALT_INTERRUPT_EN_DMSEHV_LO_INTEN_SHFT                               0xb
#define DWC_ALT_INTERRUPT_EN_DMSEHV_HI_INTEN_BMSK                             0x400
#define DWC_ALT_INTERRUPT_EN_DMSEHV_HI_INTEN_SHFT                               0xa
#define DWC_ALT_INTERRUPT_EN_DPSEHV_LO_INTEN_BMSK                             0x200
#define DWC_ALT_INTERRUPT_EN_DPSEHV_LO_INTEN_SHFT                               0x9
#define DWC_ALT_INTERRUPT_EN_DPSEHV_HI_INTEN_BMSK                             0x100
#define DWC_ALT_INTERRUPT_EN_DPSEHV_HI_INTEN_SHFT                               0x8
#define DWC_ALT_INTERRUPT_EN_DMSEHV_INTEN_BMSK                                 0x80
#define DWC_ALT_INTERRUPT_EN_DMSEHV_INTEN_SHFT                                  0x7
#define DWC_ALT_INTERRUPT_EN_DPSEHV_INTEN_BMSK                                 0x40
#define DWC_ALT_INTERRUPT_EN_DPSEHV_INTEN_SHFT                                  0x6
#define DWC_ALT_INTERRUPT_EN_RIDFLOATNINTEN_BMSK                               0x20
#define DWC_ALT_INTERRUPT_EN_RIDFLOATNINTEN_SHFT                                0x5
#define DWC_ALT_INTERRUPT_EN_CHGDETINTEN_BMSK                                  0x10
#define DWC_ALT_INTERRUPT_EN_CHGDETINTEN_SHFT                                   0x4
#define DWC_ALT_INTERRUPT_EN_DPINTEN_BMSK                                       0x8
#define DWC_ALT_INTERRUPT_EN_DPINTEN_SHFT                                       0x3
#define DWC_ALT_INTERRUPT_EN_DCDINTEN_BMSK                                      0x4
#define DWC_ALT_INTERRUPT_EN_DCDINTEN_SHFT                                      0x2
#define DWC_ALT_INTERRUPT_EN_DMINTEN_BMSK                                       0x2
#define DWC_ALT_INTERRUPT_EN_DMINTEN_SHFT                                       0x1
#define DWC_ALT_INTERRUPT_EN_ACAINTEN_BMSK                                      0x1
#define DWC_ALT_INTERRUPT_EN_ACAINTEN_SHFT                                      0x0

#define DWC_HS_PHY_IRQ_STAT_ADDR(x)                                      ((x) + 0x00000024)
#define DWC_HS_PHY_IRQ_STAT_PHYS(x)                                      ((x) + 0x00000024)
#define DWC_HS_PHY_IRQ_STAT_OFFS                                         (0x00000024)
#define DWC_HS_PHY_IRQ_STAT_RMSK                                              0xfff
#define DWC_HS_PHY_IRQ_STAT_POR                                          0x00000000
#define DWC_HS_PHY_IRQ_STAT_DMSEHV_LO_INTLCH_BMSK                             0x800
#define DWC_HS_PHY_IRQ_STAT_DMSEHV_LO_INTLCH_SHFT                               0xb
#define DWC_HS_PHY_IRQ_STAT_DMSEHV_HI_INTLCH_BMSK                             0x400
#define DWC_HS_PHY_IRQ_STAT_DMSEHV_HI_INTLCH_SHFT                               0xa
#define DWC_HS_PHY_IRQ_STAT_DPSEHV_LO_INTLCH_BMSK                             0x200
#define DWC_HS_PHY_IRQ_STAT_DPSEHV_LO_INTLCH_SHFT                               0x9
#define DWC_HS_PHY_IRQ_STAT_DPSEHV_HI_INTLCH_BMSK                             0x100
#define DWC_HS_PHY_IRQ_STAT_DPSEHV_HI_INTLCH_SHFT                               0x8
#define DWC_HS_PHY_IRQ_STAT_DMSEHV_INTLCH_BMSK                                 0x80
#define DWC_HS_PHY_IRQ_STAT_DMSEHV_INTLCH_SHFT                                  0x7
#define DWC_HS_PHY_IRQ_STAT_DPSEHV_INTLCH_BMSK                                 0x40
#define DWC_HS_PHY_IRQ_STAT_DPSEHV_INTLCH_SHFT                                  0x6
#define DWC_HS_PHY_IRQ_STAT_RIDFLOATNINTLCH_BMSK                               0x20
#define DWC_HS_PHY_IRQ_STAT_RIDFLOATNINTLCH_SHFT                                0x5
#define DWC_HS_PHY_IRQ_STAT_CHGDETINTLCH_BMSK                                  0x10
#define DWC_HS_PHY_IRQ_STAT_CHGDETINTLCH_SHFT                                   0x4
#define DWC_HS_PHY_IRQ_STAT_DPINTLCH_BMSK                                       0x8
#define DWC_HS_PHY_IRQ_STAT_DPINTLCH_SHFT                                       0x3
#define DWC_HS_PHY_IRQ_STAT_DCDINTLCH_BMSK                                      0x4
#define DWC_HS_PHY_IRQ_STAT_DCDINTLCH_SHFT                                      0x2
#define DWC_HS_PHY_IRQ_STAT_DMINTLCH_BMSK                                       0x2
#define DWC_HS_PHY_IRQ_STAT_DMINTLCH_SHFT                                       0x1
#define DWC_HS_PHY_IRQ_STAT_ACAINTLCH_BMSK                                      0x1
#define DWC_HS_PHY_IRQ_STAT_ACAINTLCH_SHFT                                      0x0


#define DWC_SS_PHY_CTRL_ADDR(x)                                          ((x) + 0x00000030)
#define DWC_SS_PHY_CTRL_OFFS                                             (0x00000030)
#define DWC_SS_PHY_CTRL_RMSK                                             0x1fffffff
#define DWC_SS_PHY_CTRL_POR                                              0x10108002
#define DWC_SS_PHY_CTRL_REF_USE_PAD_BMSK                                 0x10000000
#define DWC_SS_PHY_CTRL_REF_USE_PAD_SHFT                                       0x1c
#define DWC_SS_PHY_CTRL_TEST_BURNIN_BMSK                                  0x8000000
#define DWC_SS_PHY_CTRL_TEST_BURNIN_SHFT                                       0x1b
#define DWC_SS_PHY_CTRL_TEST_POWERDOWN_BMSK                               0x4000000
#define DWC_SS_PHY_CTRL_TEST_POWERDOWN_SHFT                                    0x1a
#define DWC_SS_PHY_CTRL_RTUNE_REQ_BMSK                                    0x2000000
#define DWC_SS_PHY_CTRL_RTUNE_REQ_SHFT                                         0x19
#define DWC_SS_PHY_CTRL_LANE0_PWR_PRESENT_BMSK                            0x1000000
#define DWC_SS_PHY_CTRL_LANE0_PWR_PRESENT_SHFT                                 0x18
#define DWC_SS_PHY_CTRL_USB2_REF_CLK_EN_BMSK                               0x800000
#define DWC_SS_PHY_CTRL_USB2_REF_CLK_EN_SHFT                                   0x17
#define DWC_SS_PHY_CTRL_USB2_REF_CLK_SEL_BMSK                              0x400000
#define DWC_SS_PHY_CTRL_USB2_REF_CLK_SEL_SHFT                                  0x16
#define DWC_SS_PHY_CTRL_SSC_REF_CLK_SEL_BMSK                               0x3fe000
#define DWC_SS_PHY_CTRL_SSC_REF_CLK_SEL_SHFT                                    0xd
#define DWC_SS_PHY_CTRL_SSC_RANGE_BMSK                                       0x1c00
#define DWC_SS_PHY_CTRL_SSC_RANGE_SHFT                                          0xa
#define DWC_SS_PHY_CTRL_REF_USB2_EN_BMSK                                      0x200
#define DWC_SS_PHY_CTRL_REF_USB2_EN_SHFT                                        0x9
#define DWC_SS_PHY_CTRL_REF_SS_PHY_EN_BMSK                                    0x100
#define DWC_SS_PHY_CTRL_REF_SS_PHY_EN_SHFT                                      0x8
#define DWC_SS_PHY_CTRL_SS_PHY_RESET_BMSK                                      0x80
#define DWC_SS_PHY_CTRL_SS_PHY_RESET_SHFT                                       0x7
#define DWC_SS_PHY_CTRL_MPLL_MULTI_BMSK                                        0x7f
#define DWC_SS_PHY_CTRL_MPLL_MULTI_SHFT                                         0x0


#endif /* _DWC_DWC_H_ */
