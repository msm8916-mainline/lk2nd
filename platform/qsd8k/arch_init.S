/*
 * Copyright (c) 2008-2009, The Linux Foundation. All rights reserved.
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


/* TODO:
 * - style cleanup
 * - do we need to do *all* of this at boot?
 */

.text		
.code 32

#define DSB .byte 0x4f, 0xf0, 0x7f, 0xf5
#define ISB .byte 0x6f, 0xf0, 0x7f, 0xf5

/*
 ; LVT Ring Osc counter
 ; used to determine sense amp settings
 ; Clobbers registers r0, r4, r5, r6, r7, r9, r10, r11
*/
.equ CLK_CTL_BASE,     0xA8600000
.equ A_GLBL_CLK_ENA,   0x0000
.equ A_PRPH_WEB_NS_REG,0x0080
.equ A_MSM_CLK_RINGOSC,0x00D0
.equ A_TCXO_CNT,       0x00D4
.equ A_TCXO_CNT_DONE,  0x00D8
.equ A_RINGOSC_CNT,    0x00DC
.equ A_MISC_CLK_CTL,   0x0108
.equ CLK_TEST,         0xA8600114       
.equ SPSS_CSR_BASE,    0xAC100000 
.equ A_SCRINGOSC,      0x0510

//;; Number of TCXO cycles to count ring oscillations
.equ TCXO_CNT_VAL,     0x100

//; Raptor addresses
.equ TCSR_SPARE2,      0xA8700060


.globl SET_SA
SET_SA:
        //; no stack at this point and any registers we use will be 0'd
        //; after we return
        LDR     r0, =TCSR_SPARE2
        LDR     r1, [r0]
        LDR     r0, = 0x010F
        AND     r2, r1, r0   //; concerned with bits [8, 3:0]

        //;--------------------------------------------------------------------
        //; Fuse bits used to determine sense amp settings
        //;--------------------------------------------------------------------

        LDR     r0, = 0x0105
        AND     r4, r2, r0    //;  mask off all but L1 ACC2, L1 ACC1 and L1 ACC0
        //;set to default of FC00
        LDR     r5, =PVR0F0_6bits    //; point to PVR0F0
        LDR     r3, =PVR2F0_6bits    //; point to PVR2F0

ck_0:
        //; if L1_[2:0] == 000 then ACC setting = FC00
        LDR     r1, = 0x0
        CMP     r4, r1
        BNE     ck_1
        B       WRITE_L1_SA_SETTINGS

ck_1:
        //; if L1_[2:0] == 001 then ACC setting = FC00
        LDR     r1, = 0x01
        CMP     r4, r1
        BNE     ck_2
        B       WRITE_L1_SA_SETTINGS

ck_2:
        //; if L1_[2:0] == 010 then ACC setting = 7C00
        LDR     r1, = 0x04
        CMP     r4, r1
        BNE     ck_3
        LDR     r5, =PVR0F0_5bits    //; point to PVR0F0
        LDR     r3, =PVR2F0_5bits    //; point to PVR2F0
        B       WRITE_L1_SA_SETTINGS

ck_3:
        //; if L1_[2:0] == 011 then ACC setting = FC00
        LDR     r1, = 0x05
        CMP     r4, r1
        BNE     ck_4
        LDR     r5, =PVR0F0_6bits    //; point to PVR0F0
        LDR     r3, =PVR2F0_6bits    //; point to PVR2F0
        B       WRITE_L1_SA_SETTINGS

ck_4:
        //; if L1_[2:0] == 0100 then ACC setting = 3C00
        LDR     r1, = 0x0100
        CMP     r4, r1
        BNE     ck_5
        LDR     r5, =PVR0F0_4bits    //; point to PVR0F0
        LDR     r3, =PVR2F0_4bits    //; point to PVR2F0
        B       WRITE_L1_SA_SETTINGS

ck_5:
        //; if L1_[2:0] == 0101 then ACC setting = 0400
        LDR     r1, = 0x0101
        CMP     r4, r1
        BNE     ck_6
        LDR     r5, =PVR0F0_1bits    //; point to PVR0F0
        LDR     r3, =PVR2F0_1bits    //; point to PVR2F0
        B       WRITE_L1_SA_SETTINGS

ck_6:
        //; if L1_[2:0] == 0110 then ACC setting = 0C00
        LDR     r1, = 0x0104
        CMP     r4, r1
        BNE     ck_7
        LDR     r5, =PVR0F0_2bits    //; point to PVR0F0
        LDR     r3, =PVR2F0_2bits    //; point to PVR2F0
        B       WRITE_L1_SA_SETTINGS

ck_7:
        //; if L1_[2:0] == 0111 then ACC setting = 1C00
        LDR     r1, = 0x0105
        CMP     r4, r1
        LDREQ   r5, =PVR0F0_3bits    //; point to PVR0F0
        LDREQ   r3, =PVR2F0_3bits    //; point to PVR2F0

WRITE_L1_SA_SETTINGS:

        LDR     r5, [r5]
        LDR     r3, [r3]

        //;WCP15_PVR0F0   r5
        MCR     p15,0x0,r5,c15,c15,0   //; write R5 to PVR0F0

        //;WCP15_PVR2F0   r3
        MCR     p15,0x2,r3,c15,c15,0   //; write R3 to PVR2F0

        AND     r4, r2, #0x000A    //;  mask off all but L2 array SA settings
        LDR     r5, =HVT_010102    //; point to L2VR3F1 setting
                       //;it gets ovewritten if its one of the other two cases
        //; if L2_1 and L2_0 == 0     ACC setting = 010102
        LDR     r1, = 0x0000
        CMP     r4, r1
        BEQ     WRITE_L2_SA_SETTINGS

        //; if L2_1 = 0 & L2_0 = 1    ACC setting = 010102
        LDR     R1, = 0x0002
        CMP     r4, r1
        BEQ     WRITE_L2_SA_SETTINGS

        //; if L2_1 = 1 & L2_0 = 0    ACC setting = 010101
        LDR     r5, =HVT_010101
        LDR     R1, = 0x0008
        CMP     r4, r1
        BEQ     WRITE_L2_SA_SETTINGS

        //; else L2_1 = 1 & L2_0 = 1  ACC setting = 212102
        LDR     r5, =HVT_212102


WRITE_L2_SA_SETTINGS:
        //;WCP15_L2VR3F1  r4
        LDR     r5, [r5]
        MCR     p15,0x3,r5,c15,c15,1     //;write r4 to L2VR3F1

        LDR     r0, =0                   //;make sure the registers we touched
        LDR     r1, =0                   //;are cleared when we return
        LDR     r2, =0
        LDR     r3, =0
        LDR     r4, =0
        LDR     r5, =0

        //; routine complete
        B       _cpu_early_init_complete

//; L1 SA settings according to LVT speed
PVR0F0_0bits:
.word 0x38000000 //; PVR0F0
PVR2F0_0bits:
.word 0x00000000 //; PVR2F0  0 bits set

PVR0F0_1bits:
.word 0x38000400 //; PVR0F0
PVR2F0_1bits:
.word 0x04000000 //; PVR2F0  1 bits set

PVR0F0_2bits:
.word 0x38000C00 //; PVR0F0
PVR2F0_2bits:
.word 0x0C000000 //; PVR2F0  2 bits set

PVR0F0_3bits:
.word 0x38001C00 //; PVR0F0
PVR2F0_3bits:
.word 0x1C000000 //; PVR2F0  3 bits set

PVR0F0_4bits:
.word 0x38003C00 //; PVR0F0
PVR2F0_4bits:
.word 0x3C000000 //; PVR2F0  4 bits set

PVR0F0_5bits:
.word 0x38007C00 //; PVR0F0
PVR2F0_5bits:
.word 0x7C000000 //; PVR2F0  5 bits set

PVR0F0_6bits:
.word 0x3800FC00 //; PVR0F0
PVR2F0_6bits:
.word 0xFC000000 //; PVR2F0  6 bits set

//; L2 SA settings according to HVT speed
HVT_212102:
.word 0x00212102 //; L2VR3F1

HVT_010102:
.word 0x00010102 //; L2VR3F1

HVT_010101:
.word 0x00010101 //; L2VR3F1


.ltorg


.globl __cpu_early_init
__cpu_early_init:
        //; Zero out r0 for use throughout this code. All other GPRs
        //; (r1-r3) are set throughout this code to help establish
        //; a consistent startup state for any code that follows.
        //; Users should add code at the end of this routine to establish
        //; their own stack address (r13), add translation page tables, enable
        //; the caches, etc.
        MOV    r0,  #0x0


        //; Remove hardcoded cache settings. appsbl_handler.s calls Set_SA
        //;   API to dynamically configure cache for slow/nominal/fast parts

        //; DCIALL to invalidate L2 cache bank (needs to be run 4 times, once per bank)
        //; This must be done early in code (prior to enabling the caches)
        MOV    r1, #0x2
        MCR    p15, 0, r1, c9, c0, 6   //; DCIALL bank D ([15:14] == 2'b00)
        ORR    r1, r1, #0x00004000
        MCR    p15, 0, r1, c9, c0, 6   //; DCIALL bank C ([15:14] == 2'b01)
        ADD    r1, r1, #0x00004000
        MCR    p15, 0, r1, c9, c0, 6   //; DCIALL bank B ([15:14] == 2'b10)
        ADD    r1, r1, #0x00004000
        MCR    p15, 0, r1, c9, c0, 6   //; DCIALL bank A ([15:14] == 2'b11)

        //; Initialize the BPCR - setup Global History Mask (GHRM) to all 1's
        //; and have all address bits (AM) participate.
        //; Different settings can be used to improve performance
        // MOVW   r1, #0x01FF
.word 0xe30011ff  // hardcoded MOVW instruction due to lack of compiler support
        // MOVT   r1, #0x01FF
.word 0xe34011ff  // hardcoded MOVT instruction due to lack of compiler support
        MCR    p15, 7, r1, c15, c0, 2   //; WCP15_BPCR


        //; Initialize all I$ Victim Registers to 0 for startup
        MCR    p15, 0, r0, c9, c1, 0    //; WCP15_ICVIC0    r0
        MCR    p15, 0, r0, c9, c1, 1    //; WCP15_ICVIC1    r0
        MCR    p15, 0, r0, c9, c1, 2    //; WCP15_ICVIC2    r0
        MCR    p15, 0, r0, c9, c1, 3    //; WCP15_ICVIC3    r0
        MCR    p15, 0, r0, c9, c1, 4    //; WCP15_ICVIC4    r0
        MCR    p15, 0, r0, c9, c1, 5    //; WCP15_ICVIC5    r0
        MCR    p15, 0, r0, c9, c1, 6    //; WCP15_ICVIC5    r0
        MCR    p15, 0, r0, c9, c1, 7    //; WCP15_ICVIC7    r0

        //; Initialize all I$ Locked Victim Registers (Unlocked Floors) to 0
        MCR    p15, 1, r0, c9, c1, 0    //; WCP15_ICFLOOR0  r0
        MCR    p15, 1, r0, c9, c1, 1    //; WCP15_ICFLOOR1  r0
        MCR    p15, 1, r0, c9, c1, 2    //; WCP15_ICFLOOR2  r0
        MCR    p15, 1, r0, c9, c1, 3    //; WCP15_ICFLOOR3  r0
        MCR    p15, 1, r0, c9, c1, 4    //; WCP15_ICFLOOR4  r0
        MCR    p15, 1, r0, c9, c1, 5    //; WCP15_ICFLOOR5  r0
        MCR    p15, 1, r0, c9, c1, 6    //; WCP15_ICFLOOR6  r0
        MCR    p15, 1, r0, c9, c1, 7    //; WCP15_ICFLOOR7  r0

        //; Initialize all D$ Victim Registers to 0
        MCR    p15, 2, r0, c9, c1, 0    //; WP15_DCVIC0    r0
        MCR    p15, 2, r0, c9, c1, 1    //; WP15_DCVIC1    r0
        MCR    p15, 2, r0, c9, c1, 2    //; WP15_DCVIC2    r0
        MCR    p15, 2, r0, c9, c1, 3    //; WP15_DCVIC3    r0
        MCR    p15, 2, r0, c9, c1, 4    //; WP15_DCVIC4    r0
        MCR    p15, 2, r0, c9, c1, 5    //; WP15_DCVIC5    r0
        MCR    p15, 2, r0, c9, c1, 6    //; WP15_DCVIC6    r0
        MCR    p15, 2, r0, c9, c1, 7    //; WP15_DCVIC7    r0

        //; Initialize all D$ Locked VDCtim Registers (Unlocked Floors) to 0
        MCR    p15, 3, r0, c9, c1, 0    //; WCP15_DCFLOOR0  r0
        MCR    p15, 3, r0, c9, c1, 1    //; WCP15_DCFLOOR1  r0
        MCR    p15, 3, r0, c9, c1, 2    //; WCP15_DCFLOOR2  r0
        MCR    p15, 3, r0, c9, c1, 3    //; WCP15_DCFLOOR3  r0
        MCR    p15, 3, r0, c9, c1, 4    //; WCP15_DCFLOOR4  r0
        MCR    p15, 3, r0, c9, c1, 5    //; WCP15_DCFLOOR5  r0
        MCR    p15, 3, r0, c9, c1, 6    //; WCP15_DCFLOOR6  r0
        MCR    p15, 3, r0, c9, c1, 7    //; WCP15_DCFLOOR7  r0

        //; Initialize ASID to zero
        MCR    p15, 0, r0, c13, c0, 1   //; WCP15_CONTEXTIDR r0

        //; ICIALL to invalidate entire I-Cache
        MCR    p15, 0, r0, c7, c5, 0    //; ICIALLU

        //; DCIALL to invalidate entire D-Cache
        MCR    p15, 0, r0, c9, c0, 6    //; DCIALL  r0

	//; Initialize ADFSR to zero
        MCR    p15, 0, r0, c5, c1, 0    //; ADFSR   r0

	//; Initialize EFSR to zero
        MCR    p15, 7, r0, c15, c0, 1   //; EFSR    r0

        //; The VBAR (Vector Base Address Register) should be initialized
        //; early in your code. We are setting it to zero
        MCR    p15, 0, r0, c12, c0, 0   //; WCP15_VBAR  r0

        //; Ensure the MCR's above have completed their operation before continuing
        DSB
        ISB

        //;-------------------------------------------------------------------
        //; There are a number of registers that must be set prior to enabling
        //; the MMU. The DCAR is one of these registers. We are setting
        //; it to zero (no access) to easily detect improper setup in subsequent
        //; code sequences
        //;-------------------------------------------------------------------
        //; Setup DACR (Domain Access Control Register) to zero
        MCR    p15, 0, r0, c3, c0, 0    //; WCP15_DACR  r0

        //; Setup DCLKCR to allow normal D-Cache line fills
        MCR    p15, 1, r0, c9, c0, 7    //; WCP15_DCLKCR r0

        //; Setup the TLBLKCR
        //; Victim = 6'b000000; Floor = 6'b000000;
        //; IASIDCFG = 2'b00 (State-Machine); IALLCFG = 2'b01 (Flash); BNA = 1'b0;
        MOV    r1, #0x02
        MCR    p15, 0, r1, c10, c1, 3     //; WCP15_TLBLKCR  r1

        //;Make sure TLBLKCR is complete before continuing
        ISB

        //; Invalidate the UTLB
        MCR    p15, 0, r0, c8, c7, 0      //; UTLBIALL

        //; Make sure UTLB request has been presented to macro before continuing
        ISB

        // Disable predecode repair cache on certain Scorpion revisions
        // (Raptor V2 and earlier, or Halcyon V1)
        MRC    p15, 0, r1, c0, c0, 0      //; MIDR
        BIC    r2, r1, #0xf7              //; check for Raptor2 or below
        LDR    r3, =0x510f0000
        CMP    r2, r3
        BEQ    DPRC
        BIC    r2, r1, #0xf0              //; check for Halcyon V1
        LDR    r3, =0x511f0000
        CMP    r2, r3
        BNE    SYSI2

DPRC:
        MRC    p15, 0, r1, c15, c15, 2    //; PVR0F2
        ORR    r1, r1, #0x10              //; enable bit 4
        MCR    p15, 0, r1, c15, c15, 2    //; disable predecode repair cache

SYSI2:
        //; setup L2CR1 to some default Instruction and data prefetching values
        //; Users may want specific settings for various performance enhancements
        MOV r2, #0x33
        MCR    p15, 3, r2, c15, c0, 3     //; WCP15_L2CR1  r0


        //; Enable Z bit to enable branch prediction (default is off)
        MRC    p15, 0, r2, c1, c0, 0      //; RCP15_SCTLR  r2
        ORR    r2, r2, #0x00000800
        MCR    p15, 0, r2, c1, c0, 0      //; WCP15_SCTLR  r2

        //; Make sure Link stack is initialized with branch and links to sequential addresses
        //; This aids in creating a predictable startup environment
        BL      SEQ1
SEQ1:   BL      SEQ2
SEQ2:   BL      SEQ3
SEQ3:   BL      SEQ4
SEQ4:   BL      SEQ5
SEQ5:   BL      SEQ6
SEQ6:   BL      SEQ7
SEQ7:   BL      SEQ8
SEQ8:

        //; REMOVE FOLLOWING THREE INSTRUCTIONS WHEN POWER COLLAPSE IS ENA
        //;Make sure the DBGOSLSR[LOCK] bit is cleared to allow access to the debug registers
        //; Writing anything but the "secret code" to the DBGOSLAR clears the DBGOSLSR[LOCK] bit
        MCR    p14, 0, r0, c1, c0, 4       //; WCP14_DBGOSLAR r0


        //; Read the DBGPRSR to clear the DBGPRSR[STICKYPD]
        //; Any read to DBGPRSR clear the STICKYPD bit
        //; ISB guarantees the read completes before attempting to
        //; execute a CP14 instruction.
        MRC    p14, 0, r3, c1, c5, 4       //; RCP14_DBGPRSR r3
        ISB

        //; Initialize the Watchpoint Control Registers to zero (optional)
        //;;; MCR    p14, 0, r0, c0, c0, 7       ; WCP14_DBGWCR0  r0
        //;;; MCR    p14, 0, r0, c0, c1, 7       ; WCP14_DBGWCR1  r0


        //;----------------------------------------------------------------------
        //; The saved Program Status Registers (SPSRs) should be setup
        //; prior to any automatic mode switches. The following
        //; code sets these registers up to a known state. Users will need to
        //; customize these settings to meet their needs.
        //;----------------------------------------------------------------------
        MOV    r2,  #0x1f
        MOV    r1,  #0xd7                 //;ABT mode
        msr    cpsr_c, r1                 //;ABT mode
        msr    spsr_cxfs, r2              //;clear the spsr
        MOV    r1,  #0xdb                 //;UND mode
        msr    cpsr_c, r1                 //;UND mode
        msr    spsr_cxfs, r2              //;clear the spsr
        MOV    r1,  #0xd1                 //;FIQ mode
        msr    cpsr_c, r1                 //;FIQ mode
        msr    spsr_cxfs, r2              //;clear the spsr
        MOV    r1,  #0xd2                 //;IRQ mode
        msr    cpsr_c, r1                 //;IRQ mode
        msr    spsr_cxfs, r2              //;clear the spsr
        MOV    r1,  #0xd6                 //;Monitor mode
        msr    cpsr_c, r1                 //;Monitor mode
        msr    spsr_cxfs, r2              //;clear the spsr
        MOV    r1,  #0xd3                 //;SVC mode
        msr    cpsr_c, r1                 //;SVC mode
        msr    spsr_cxfs, r2              //;clear the spsr


        //;----------------------------------------------------------------------
        //; Enabling Error reporting is something users may want to do at
        //; some other point in time. We have chosen some default settings
        //; that should be reviewed. Most of these registers come up in an
        //; unpredictable state after reset.
        //;----------------------------------------------------------------------
//;Start of error and control setting

        //; setup L2CR0 with various L2/TCM control settings
        //; enable out of order bus attributes and error reporting
        //; this register comes up unpredictable after reset
        // MOVW   r1, #0x0F0F
.word 0xe3001f0f  // hardcoded MOVW instruction due to lack of compiler support
        // MOVT   r1, #0xC005
.word 0xe34c1005  // hardcoded MOVW instruction due to lack of compiler support
        MCR    p15, 3, r1, c15, c0, 1    //; WCP15_L2CR0  r1

        //; setup L2CPUCR
        //; MOV    r2, #0xFF
        //; Enable I and D cache parity
        //;L2CPUCR[7:5] = 3~Rh7 ~V enable parity error reporting for modified,
        //;tag, and data parity errors
        MOV    r2, #0xe0
        MCR    p15, 3, r2, c15, c0, 2    //; WCP15_L2CPUCR  r2

        //; setup SPCR
        //; enable all error reporting (reset value is unpredicatble for most bits)
        MOV    r3, #0x0F
        MCR    p15, 0, r3, c9, c7, 0     //; WCP15_SPCR  r3

        //; setup DMACHCRs (reset value unpredictable)
        //; control setting and enable all error reporting
        MOV   r1, #0x0F

        //; DMACHCR0 = 0000000F
        MOV   r2, #0x00                  //; channel 0
        MCR   p15, 0, r2, c11, c0, 0     //; WCP15_DMASELR  r2
        MCR   p15, 0, r1, c11, c0, 2     //; WCP15_DMACHCR  r1

        //; DMACHCR1 = 0000000F
        MOV   r2, #0x01                  //; channel 1
        MCR   p15, 0, r2, c11, c0, 0     //; WCP15_DMASELR  r2
        MCR   p15, 0, r1, c11, c0, 2     //; WCP15_DMACHCR  r1

        //; DMACHCR2 = 0000000F
        MOV   r2, #0x02                  //; channel 2
        MCR   p15, 0, r2, c11, c0, 0     //; WCP15_DMASELR  r2
        MCR   p15, 0, r1, c11, c0, 2     //; WCP15_DMACHCR  r1

        //; DMACHCR3 = 0000000F
        MOV   r2, #0x03                  //; channel 3
        MCR   p15, 0, r2, c11, c0, 0     //; WCP15_DMASELR  r2
        MCR   p15, 0, r1, c11, c0, 2     //; WCP15_DMACHCR  r1

        //; Set ACTLR (reset unpredictable)
        //; Set AVIVT control, error reporting, etc.
        //; MOV   r3, #0x07
        //; Enable I and D cache parity
        //;ACTLR[2:0] = 3'h7 - enable parity error reporting from L2/I$/D$)
        //;ACTLR[5:4] = 2'h3 - enable parity
        //;ACTLR[19:18] =2'h3 - always generate and check parity(when MMU disabled).
        //;Value to be written #0xC0037
        // MOVW   r3, #0x0037
.word 0xe3003037  // hardcoded MOVW instruction due to lack of compiler support
        // MOVT   r3, #0x000C
.word 0xe340300c  // hardcoded MOVW instruction due to lack of compiler support
            //; read the version_id to determine if d-cache should be disabled
            LDR r2, = 0xa8e00270  //;Read HW_REVISION_NUMBER, HWIO_HW_REVISION_NUMBER_ADDR
            LDR r2,[r2]
            AND r2,r2,#0xf0000000 //;hw_revision mask off bits 28-31
                //;if HW_revision is 1.0 or older, (revision==0)
                CMP r2,#0
        //; Disable d-cache on older QSD8650 (Rev 1.0) silicon
        //;orreq   r3, r3, #0x4000          //;disable dcache
        //;MCR   p15, 0, r3, c1, c0, 1      //; WCP15_ACTLR  r3

//;End of error and control setting

        //;----------------------------------------------------------------------
        //; Unlock ETM and read StickyPD to halt the ETM clocks from running.
        //; This is required for power saving whether the ETM is used or not.
        //;----------------------------------------------------------------------

        //;Clear ETMOSLSR[LOCK] bit
        MOV   r1, #0x00000000
        MCR   p14, 1, r1, c1, c0, 4        //; WCP14_ETMOSLAR      r1

        //;Clear ETMPDSR[STICKYPD] bit
        MRC   p14, 1, r2, c1, c5, 4        //; RCP14_ETMPDSR       r2

/*
#ifdef APPSBL_ETM_ENABLE
        ;----------------------------------------------------------------------
        ; Optionally Enable the ETM (Embedded Trace Macro) which is used for debug
        ;----------------------------------------------------------------------

        ; enable ETM clock if disabled
        MRC   p15, 7, r1, c15, c0, 5       ; RCP15_CPMR           r1
        ORR   r1, r1, #0x00000008
        MCR   p15, 7, r1, c15, c0, 5       ; WCP15_CPMR           r1
        ISB

        ; set trigger event to counter1 being zero
        MOV   r3, #0x00000040
        MCR   p14, 1, r3, c0, c2, 0        ; WCP14_ETMTRIGGER     r3

        ; clear ETMSR
        MOV   r2, #0x00000000
        MCR   p14, 1, r2, c0, c4, 0        ; WCP14_ETMSR          r2

        ; clear trace enable single address comparator usage
        MCR   p14, 1, r2, c0, c7, 0        ; WCP14_ETMTECR2       r2

        ; set trace enable to always
        MOV   r2, #0x0000006F
        MCR   p14, 1, r2, c0, c8, 0        ; WCP14_ETMTEEVR       r2

        ; clear trace enable address range comparator usage and exclude nothing
        MOV   r2, #0x01000000
        MCR   p14, 1, r2, c0, c9, 0        ; WCP14_ETMTECR1       r2

        ; set view data to always
        MOV   r2, #0x0000006F
        MCR   p14, 1, r2, c0, c12, 0       ; WCP14_ETMVDEVR       r2

        ; clear view data single address comparator usage
        MOV   r2, #0x00000000
        MCR   p14, 1, r2, c0, c13, 0       ;  WCP14_ETMVDCR1       r2

        ; clear view data address range comparator usage and exclude nothing
        MOV   r2, #0x00010000
        MCR   p14, 1, r2, c0, c15, 0       ;  WCP14_ETMVDCR3       r2

        ; set counter1 to 194
        MOV   r2, #0x000000C2
        MCR   p14, 1, r2, c0, c0, 5        ;  WCP14_ETMCNTRLDVR1   r2

        ; set counter1 to never reload
        MOV   r2, #0x0000406F
        MCR   p14, 1, r2, c0, c8, 5        ;  WCP14_ETMCNTRLDEVR1  r2

        ; set counter1 to decrement every cycle
        MOV   r2, #0x0000006F
        MCR   p14, 1, r2, c0, c4, 5        ; WCP14_ETMCNTENR1     r2

        ; Set trace synchronization frequency 1024 bytes
        MOV   r2, #0x00000400
        MCR   p14, 1, r2, c0, c8, 7        ; WCP14_ETMSYNCFR      r2

        ; Program etm control register
        ;  - Set the CPU to ETM clock ratio to 1:1
        ;  - Set the ETM to perform data address tracing
        MOV   r2, #0x00002008
        MCR   p14, 1, r2, c0, c0, 0        ; WCP14_ETMCR          r2
        ISB
#endif *//* APPSBL_ETM_ENABLE */

/*
#ifdef APPSBL_VFP_ENABLE
       ;----------------------------------------------------------------------
       ; Perform the following operations if you intend to make use of
       ; the VFP/Neon unit. Note that the FMXR instruction requires a CPU ID
       ; indicating the VFP unit is present (i.e.Cortex-A8). .
       ; Some tools will require full double precision floating point support
       ; which will become available in Scorpion pass 2
       ;----------------------------------------------------------------------
       ; allow full access to CP 10 and 11 space for VFP/NEON use
        MRC   p15, 0, r1, c1, c0, 2        ; Read CP Access Control Register
        ORR   r1, r1, #0x00F00000          ; enable full access for p10,11
        MCR   p15, 0, r1, c1, c0, 2        ; Write CPACR

        ;make sure the CPACR is complete before continuing
        ISB

       ; Enable VFP itself (certain OSes may want to dynamically set/clear
       ; the enable bit based on the application being executed
        MOV   r1, #0x40000000
        FMXR  FPEXC, r1
#endif *//* APPSBL_VFP_ENABLE */

        /* we have no stack, so just tail-call into the SET_SA routine... */
        b SET_SA


.ltorg
