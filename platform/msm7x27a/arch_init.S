/*
 * Copyright (c) 2009-2011, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *	may be used to endorse or promote products derived from this
 *	software without specific prior written permission.
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
.globl SET_SA
SET_SA:
		//; routine complete
		B	   _cpu_early_init_complete
.ltorg

.globl __cpu_early_init
__cpu_early_init:
		//; Zero out r0 for use throughout this code. All other GPRs
		//; (r1-r3) are set throughout this code to help establish
		//; a consistent startup state for any code that follows.
		//; Users should add code at the end of this routine to establish
		//; their own stack address (r13), add translation page tables, enable
		//; the caches, etc.
		MOV	r0,  #0x0


		//; Remove hardcoded cache settings. appsbl_handler.s calls Set_SA
		//;   API to dynamically configure cache for slow/nominal/fast parts


		//; Initialize ASID to zero
		MCR	p15, 0, r0, c13, c0, 1   //; WCP15_CONTEXTIDR r0

		//; ICIALL to invalidate entire I-Cache
		MCR	p15, 0, r0, c7, c5, 0	//; ICIALLU

		//; Initialize ADFSR to zero
		MCR	p15, 0, r0, c5, c1, 0	//; ADFSR   r0

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
		MCR	p15, 0, r0, c3, c0, 0	//; WCP15_DACR  r0

		//;Make sure TLBLKCR is complete before continuing
		ISB

		//; Invalidate the UTLB
		MCR	p15, 0, r0, c8, c7, 0	  //; UTLBIALL

		//; Make sure UTLB request has been presented to macro before continuing
		ISB

SYSI2:
		//; Enable Z bit to enable branch prediction (default is off)
		MRC	p15, 0, r2, c1, c0, 0	  //; RCP15_SCTLR  r2
		ORR	r2, r2, #0x00000800
		MCR	p15, 0, r2, c1, c0, 0	  //; WCP15_SCTLR  r2

		//; Make sure Link stack is initialized with branch and links to sequential addresses
		//; This aids in creating a predictable startup environment
		BL	  SEQ1
SEQ1:   BL	  SEQ2
SEQ2:   BL	  SEQ3
SEQ3:   BL	  SEQ4
SEQ4:   BL	  SEQ5
SEQ5:   BL	  SEQ6
SEQ6:   BL	  SEQ7
SEQ7:   BL	  SEQ8
SEQ8:

		ISB

		//; Initialize the Watchpoint Control Registers to zero (optional)
		//;;; MCR	p14, 0, r0, c0, c0, 7	   ; WCP14_DBGWCR0  r0
		//;;; MCR	p14, 0, r0, c0, c1, 7	   ; WCP14_DBGWCR1  r0


		//;----------------------------------------------------------------------
		//; The saved Program Status Registers (SPSRs) should be setup
		//; prior to any automatic mode switches. The following
		//; code sets these registers up to a known state. Users will need to
		//; customize these settings to meet their needs.
		//;----------------------------------------------------------------------
		MOV	r2,  #0x1f
		MOV	r1,  #0xd7				 //;ABT mode
		msr	cpsr_c, r1				 //;ABT mode
		msr	spsr_cxfs, r2			  //;clear the spsr
		MOV	r1,  #0xdb				 //;UND mode
		msr	cpsr_c, r1				 //;UND mode
		msr	spsr_cxfs, r2			  //;clear the spsr
		MOV	r1,  #0xd1				 //;FIQ mode
		msr	cpsr_c, r1				 //;FIQ mode
		msr	spsr_cxfs, r2			  //;clear the spsr
		MOV	r1,  #0xd2				 //;IRQ mode
		msr	cpsr_c, r1				 //;IRQ mode
		msr	spsr_cxfs, r2			  //;clear the spsr
		MOV	r1,  #0xd6				 //;Monitor mode
		msr	cpsr_c, r1				 //;Monitor mode
		msr	spsr_cxfs, r2			  //;clear the spsr
		MOV	r1,  #0xd3				 //;SVC mode
		msr	cpsr_c, r1				 //;SVC mode
		msr	spsr_cxfs, r2			  //;clear the spsr


		//;----------------------------------------------------------------------
		//; Enabling Error reporting is something users may want to do at
		//; some other point in time. We have chosen some default settings
		//; that should be reviewed. Most of these registers come up in an
		//; unpredictable state after reset.
		//;----------------------------------------------------------------------
//;Start of error and control setting


		//; Set ACTLR (reset unpredictable)
		//; Set AVIVT control, error reporting, etc.
		//; MOV   r3, #0x07
		//; Enable I and D cache parity
		//;ACTLR[2:0] = 3'h7 - enable parity error reporting from L2/I$/D$)
		//;ACTLR[5:4] = 2'h3 - enable parity
		//;ACTLR[19:18] =2'h3 - always generate and check parity(when MMU disabled).
		//;Value to be written #0xC0037
		// MOVW   r3, #0x0037
//; .word 0xe3003037  // hardcoded MOVW instruction due to lack of compiler support
		// MOVT   r3, #0x000C
//; .word 0xe340300c  // hardcoded MOVW instruction due to lack of compiler support
		//; MCR   p15, 0, r3, c1, c0, 1	  //; WCP15_ACTLR  r3

//;End of error and control setting

/*
#ifdef APPSBL_ETM_ENABLE
		;----------------------------------------------------------------------
		; Optionally Enable the ETM (Embedded Trace Macro) which is used for debug
		;----------------------------------------------------------------------

		; enable ETM clock if disabled
		MRC   p15, 7, r1, c15, c0, 5	   ; RCP15_CPMR		   r1
		ORR   r1, r1, #0x00000008
		MCR   p15, 7, r1, c15, c0, 5	   ; WCP15_CPMR		   r1
		ISB

		; set trigger event to counter1 being zero
		MOV   r3, #0x00000040
		MCR   p14, 1, r3, c0, c2, 0		; WCP14_ETMTRIGGER	 r3

		; clear ETMSR
		MOV   r2, #0x00000000
		MCR   p14, 1, r2, c0, c4, 0		; WCP14_ETMSR		  r2

		; clear trace enable single address comparator usage
		MCR   p14, 1, r2, c0, c7, 0		; WCP14_ETMTECR2	   r2

		; set trace enable to always
		MOV   r2, #0x0000006F
		MCR   p14, 1, r2, c0, c8, 0		; WCP14_ETMTEEVR	   r2

		; clear trace enable address range comparator usage and exclude nothing
		MOV   r2, #0x01000000
		MCR   p14, 1, r2, c0, c9, 0		; WCP14_ETMTECR1	   r2

		; set view data to always
		MOV   r2, #0x0000006F
		MCR   p14, 1, r2, c0, c12, 0	   ; WCP14_ETMVDEVR	   r2

		; clear view data single address comparator usage
		MOV   r2, #0x00000000
		MCR   p14, 1, r2, c0, c13, 0	   ;  WCP14_ETMVDCR1	   r2

		; clear view data address range comparator usage and exclude nothing
		MOV   r2, #0x00010000
		MCR   p14, 1, r2, c0, c15, 0	   ;  WCP14_ETMVDCR3	   r2

		; set counter1 to 194
		MOV   r2, #0x000000C2
		MCR   p14, 1, r2, c0, c0, 5		;  WCP14_ETMCNTRLDVR1   r2

		; set counter1 to never reload
		MOV   r2, #0x0000406F
		MCR   p14, 1, r2, c0, c8, 5		;  WCP14_ETMCNTRLDEVR1  r2

		; set counter1 to decrement every cycle
		MOV   r2, #0x0000006F
		MCR   p14, 1, r2, c0, c4, 5		; WCP14_ETMCNTENR1	 r2

		; Set trace synchronization frequency 1024 bytes
		MOV   r2, #0x00000400
		MCR   p14, 1, r2, c0, c8, 7		; WCP14_ETMSYNCFR	  r2

		; Program etm control register
		;  - Set the CPU to ETM clock ratio to 1:1
		;  - Set the ETM to perform data address tracing
		MOV   r2, #0x00002008
		MCR   p14, 1, r2, c0, c0, 0		; WCP14_ETMCR		  r2
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
		MRC   p15, 0, r1, c1, c0, 2		; Read CP Access Control Register
		ORR   r1, r1, #0x00F00000		  ; enable full access for p10,11
		MCR   p15, 0, r1, c1, c0, 2		; Write CPACR

		;make sure the CPACR is complete before continuing
		ISB

	   ; Enable VFP itself (certain OSes may want to dynamically set/clear
	   ; the enable bit based on the application being executed
		MOV   r1, #0x40000000
		FMXR  FPEXC, r1
#endif *//* APPSBL_VFP_ENABLE */

		/* we have no stack, so just tail-call into the SET_SA routine... */

		B	   SET_SA
.ltorg
