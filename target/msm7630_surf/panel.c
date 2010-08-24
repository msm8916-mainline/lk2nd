/*
 * Copyright (c) 2007, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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
 *  * Neither the name of Code Aurora nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
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

#include <debug.h>
#include <dev/gpio.h>
#include <kernel/thread.h>
#include <gpio_hw.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#if DISPLAY_TYPE_MDDI
#include <platform/mddi.h>

#define MDDI_CLIENT_CORE_BASE  0x108000
#define LCD_CONTROL_BLOCK_BASE 0x110000
#define SPI_BLOCK_BASE         0x120000
#define I2C_BLOCK_BASE         0x130000
#define PWM_BLOCK_BASE         0x140000
#define GPIO_BLOCK_BASE        0x150000
#define SYSTEM_BLOCK1_BASE     0x160000
#define SYSTEM_BLOCK2_BASE     0x170000


#define	MDDICAP0    (MDDI_CLIENT_CORE_BASE|0x00)
#define	MDDICAP1    (MDDI_CLIENT_CORE_BASE|0x04)
#define	MDDICAP2    (MDDI_CLIENT_CORE_BASE|0x08)
#define	MDDICAP3    (MDDI_CLIENT_CORE_BASE|0x0C)
#define	MDCAPCHG    (MDDI_CLIENT_CORE_BASE|0x10)
#define	MDCRCERC    (MDDI_CLIENT_CORE_BASE|0x14)
#define	TTBUSSEL    (MDDI_CLIENT_CORE_BASE|0x18)
#define	DPSET0      (MDDI_CLIENT_CORE_BASE|0x1C)
#define	DPSET1      (MDDI_CLIENT_CORE_BASE|0x20)
#define	DPSUS       (MDDI_CLIENT_CORE_BASE|0x24)
#define	DPRUN       (MDDI_CLIENT_CORE_BASE|0x28)
#define	SYSCKENA    (MDDI_CLIENT_CORE_BASE|0x2C)
#define	TESTMODE    (MDDI_CLIENT_CORE_BASE|0x30)
#define	FIFOMONI    (MDDI_CLIENT_CORE_BASE|0x34)
#define	INTMONI     (MDDI_CLIENT_CORE_BASE|0x38)
#define	MDIOBIST    (MDDI_CLIENT_CORE_BASE|0x3C)
#define	MDIOPSET    (MDDI_CLIENT_CORE_BASE|0x40)
#define	BITMAP0     (MDDI_CLIENT_CORE_BASE|0x44)
#define	BITMAP1     (MDDI_CLIENT_CORE_BASE|0x48)
#define	BITMAP2     (MDDI_CLIENT_CORE_BASE|0x4C)
#define	BITMAP3     (MDDI_CLIENT_CORE_BASE|0x50)
#define	BITMAP4     (MDDI_CLIENT_CORE_BASE|0x54)


#define	SRST        (LCD_CONTROL_BLOCK_BASE|0x00)
#define	PORT_ENB    (LCD_CONTROL_BLOCK_BASE|0x04)
#define	START       (LCD_CONTROL_BLOCK_BASE|0x08)
#define	PORT        (LCD_CONTROL_BLOCK_BASE|0x0C)
#define	CMN         (LCD_CONTROL_BLOCK_BASE|0x10)
#define	GAMMA       (LCD_CONTROL_BLOCK_BASE|0x14)
#define	INTFLG      (LCD_CONTROL_BLOCK_BASE|0x18)
#define	INTMSK      (LCD_CONTROL_BLOCK_BASE|0x1C)
#define	MPLFBUF     (LCD_CONTROL_BLOCK_BASE|0x20)
#define	HDE_LEFT    (LCD_CONTROL_BLOCK_BASE|0x24)
#define	VDE_TOP     (LCD_CONTROL_BLOCK_BASE|0x28)

#define	PXL         (LCD_CONTROL_BLOCK_BASE|0x30)
#define	HCYCLE      (LCD_CONTROL_BLOCK_BASE|0x34)
#define	HSW         (LCD_CONTROL_BLOCK_BASE|0x38)
#define	HDE_START   (LCD_CONTROL_BLOCK_BASE|0x3C)
#define	HDE_SIZE    (LCD_CONTROL_BLOCK_BASE|0x40)
#define	VCYCLE      (LCD_CONTROL_BLOCK_BASE|0x44)
#define	VSW         (LCD_CONTROL_BLOCK_BASE|0x48)
#define	VDE_START   (LCD_CONTROL_BLOCK_BASE|0x4C)
#define	VDE_SIZE    (LCD_CONTROL_BLOCK_BASE|0x50)
#define	WAKEUP      (LCD_CONTROL_BLOCK_BASE|0x54)
#define	WSYN_DLY    (LCD_CONTROL_BLOCK_BASE|0x58)
#define	REGENB      (LCD_CONTROL_BLOCK_BASE|0x5C)
#define	VSYNIF      (LCD_CONTROL_BLOCK_BASE|0x60)
#define	WRSTB       (LCD_CONTROL_BLOCK_BASE|0x64)
#define	RDSTB       (LCD_CONTROL_BLOCK_BASE|0x68)
#define	ASY_DATA    (LCD_CONTROL_BLOCK_BASE|0x6C)
#define	ASY_DATB    (LCD_CONTROL_BLOCK_BASE|0x70)
#define	ASY_DATC    (LCD_CONTROL_BLOCK_BASE|0x74)
#define	ASY_DATD    (LCD_CONTROL_BLOCK_BASE|0x78)
#define	ASY_DATE    (LCD_CONTROL_BLOCK_BASE|0x7C)
#define	ASY_DATF    (LCD_CONTROL_BLOCK_BASE|0x80)
#define	ASY_DATG    (LCD_CONTROL_BLOCK_BASE|0x84)
#define	ASY_DATH    (LCD_CONTROL_BLOCK_BASE|0x88)
#define	ASY_CMDSET  (LCD_CONTROL_BLOCK_BASE|0x8C)

#define	MONI        (LCD_CONTROL_BLOCK_BASE|0xB0)

#define	Current     (LCD_CONTROL_BLOCK_BASE|0xC0)
#define	LCD         (LCD_CONTROL_BLOCK_BASE|0xC4)
#define	COMMAND     (LCD_CONTROL_BLOCK_BASE|0xC8)


#define	SSICTL      (SPI_BLOCK_BASE|0x00)
#define	SSITIME     (SPI_BLOCK_BASE|0x04)
#define	SSITX       (SPI_BLOCK_BASE|0x08)
#define	SSIRX       (SPI_BLOCK_BASE|0x0C)
#define	SSIINTC     (SPI_BLOCK_BASE|0x10)
#define	SSIINTS     (SPI_BLOCK_BASE|0x14)
#define	SSIDBG1     (SPI_BLOCK_BASE|0x18)
#define	SSIDBG2     (SPI_BLOCK_BASE|0x1C)
#define	SSIID       (SPI_BLOCK_BASE|0x20)


#define	I2CSETUP	(I2C_BLOCK_BASE|0x00)
#define	I2CCTRL	    (I2C_BLOCK_BASE|0x04)


#define	TIMER0LOAD	  (PWM_BLOCK_BASE|0x00)
#define	TIMER0VALUE	  (PWM_BLOCK_BASE|0x04)
#define	TIMER0CONTROL (PWM_BLOCK_BASE|0x08)
#define	TIMER0INTCLR  (PWM_BLOCK_BASE|0x0C)
#define	TIMER0RIS	  (PWM_BLOCK_BASE|0x10)
#define	TIMER0MIS	  (PWM_BLOCK_BASE|0x14)
#define	TIMER0BGLOAD  (PWM_BLOCK_BASE|0x18)
#define	PWM0OFF	      (PWM_BLOCK_BASE|0x1C)
#define	TIMER1LOAD	  (PWM_BLOCK_BASE|0x20)
#define	TIMER1VALUE	  (PWM_BLOCK_BASE|0x24)
#define	TIMER1CONTROL (PWM_BLOCK_BASE|0x28)
#define	TIMER1INTCLR  (PWM_BLOCK_BASE|0x2C)
#define	TIMER1RIS	  (PWM_BLOCK_BASE|0x30)
#define	TIMER1MIS	  (PWM_BLOCK_BASE|0x34)
#define	TIMER1BGLOAD  (PWM_BLOCK_BASE|0x38)
#define	PWM1OFF	      (PWM_BLOCK_BASE|0x3C)
#define	TIMERITCR	  (PWM_BLOCK_BASE|0x60)
#define	TIMERITOP	  (PWM_BLOCK_BASE|0x64)
#define	PWMCR	      (PWM_BLOCK_BASE|0x68)
#define	PWMID	      (PWM_BLOCK_BASE|0x6C)
#define	PWMMON	      (PWM_BLOCK_BASE|0x70)


#define	GPIODATA    (GPIO_BLOCK_BASE|0x00)
#define	GPIODIR     (GPIO_BLOCK_BASE|0x04)
#define	GPIOIS      (GPIO_BLOCK_BASE|0x08)
#define	GPIOIBE     (GPIO_BLOCK_BASE|0x0C)
#define	GPIOIEV     (GPIO_BLOCK_BASE|0x10)
#define	GPIOIE      (GPIO_BLOCK_BASE|0x14)
#define	GPIORIS     (GPIO_BLOCK_BASE|0x18)
#define	GPIOMIS     (GPIO_BLOCK_BASE|0x1C)
#define	GPIOIC      (GPIO_BLOCK_BASE|0x20)
#define	GPIOOMS     (GPIO_BLOCK_BASE|0x24)
#define	GPIOPC      (GPIO_BLOCK_BASE|0x28)

#define	GPIOID      (GPIO_BLOCK_BASE|0x30)


#define	WKREQ       (SYSTEM_BLOCK1_BASE|0x00)
#define	CLKENB      (SYSTEM_BLOCK1_BASE|0x04)
#define	DRAMPWR     (SYSTEM_BLOCK1_BASE|0x08)
#define	INTMASK     (SYSTEM_BLOCK1_BASE|0x0C)
#define	GPIOSEL     (SYSTEM_BLOCK2_BASE|0x00)

struct init_table {
    unsigned int reg;
    unsigned int val;
};

static struct init_table toshiba_480x640_init_table[] = {
	{ DPSET0,               0x4BEC0066 },  // # MDC.DPSET0  # Setup DPLL parameters
	{ DPSET1,               0x00000113 },  //   # MDC.DPSET1  
	{ DPSUS,                0x00000000 },  //   # MDC.DPSUS  # Set DPLL oscillation enable
	{ DPRUN,                0x00000001 },  //   # MDC.DPRUN  # Release reset signal for DPLL
	{ 0,                    14         },  // wait_ms(14);
	{ SYSCKENA,             0x00000001 },  //   # MDC.SYSCKENA  # Enable system clock output
	{ CLKENB,               0x000000EF },  //   # SYS.CLKENB  # Enable clocks for each module (without DCLK , i2cCLK)
	{ GPIO_BLOCK_BASE,      0x03FF0000 },  //   # GPI .GPIODATA  # GPIO2(RESET_LCD_N) set to 0 , GPIO3(eDRAM_Power) set to 0
	{ GPIODIR,              0x0000024D },  //   # GPI .GPIODIR  # Select direction of GPIO port (0,2,3,6,9 output)
	{ SYSTEM_BLOCK2_BASE,   0x00000173 },  //   # SYS.GPIOSEL  # GPIO port multiplexing control
	{ GPIOPC,               0x03C300C0 },  //   # GPI .GPIOPC  # GPIO2,3 PD cut
	{ SYSTEM_BLOCK1_BASE,   0x00000000 },  //   # SYS.WKREQ  # Wake-up request event is VSYNC alignment
	{ GPIOIS,               0x00000000 },  //   # GPI .GPIOIS  # Set interrupt sense of GPIO
	{ GPIOIEV,              0x00000001 },  //   # GPI .GPIOIEV  # Set interrupt event of GPIO
	{ GPIOIC,               0x000003FF },  //   # GPI .GPIOIC  # GPIO interrupt clear
	{ GPIO_BLOCK_BASE,      0x00060006 },  //   # GPI .GPIODATA  # Release LCDD reset
	{ GPIO_BLOCK_BASE,      0x00080008 },  //   # GPI .GPIODATA  # eDRAM VD supply
	{ GPIO_BLOCK_BASE,      0x02000200 },  //   # GPI .GPIODATA  # TEST LED ON
	{ DRAMPWR,              0x00000001 },  //   # SYS.DRAMPWR  # eDRAM power up
	{ TIMER0CONTROL,        0x00000060 },  //   # PWM.Timer0Control  # PWM0 output stop
	{ PWM_BLOCK_BASE,       0x00001388 },  //   # PWM.Timer0Load  # PWM0 10kHz , Duty 99 (BackLight OFF)
	//{PWM0OFF,               0x00000001 },  //   # PWM.PWM0OFF  
#if 0
	{ PWM0OFF,              0x00001387 }, // SURF 100% backlight
	{ PWM0OFF,              0x00000000 }, // FFA 100% backlight
#endif
	{ PWM0OFF,              0x000009C3 }, // 50% BL
	{ TIMER1CONTROL,        0x00000060 },  //   # PWM.Timer1Control  # PWM1 output stop
	{ TIMER1LOAD,           0x00001388 },  //   # PWM.Timer1Load  # PWM1 10kHz , Duty 99 (BackLight OFF)
	//{PWM1OFF,               0x00000001 },  //   # PWM.PWM1OFF  
	{ PWM1OFF,              0x00001387 },
	{ TIMER0CONTROL,        0x000000E0 },  //   # PWM.Timer0Control  # PWM0 output start
	{ TIMER1CONTROL,        0x000000E0 },  //   # PWM.Timer1Control  # PWM1 output start
	{ PWMCR,                0x00000003 },  //   # PWM.PWMCR  # PWM output enable
	{ 0,                    1          },  //  wait_ms(1);        
	{ SPI_BLOCK_BASE,       0x00000799 },  //   # SPI .SSICTL  # SPI operation mode setting
	{ SSITIME,              0x00000100 },  //   # SPI .SSITIME  # SPI serial interface timing setting
	{ SPI_BLOCK_BASE,       0x0000079b },  //   # SPI .SSICTL  # Set SPI active mode

	{ SSITX,                0x00000000 },  //   # SPI.SSITX  # Release from Deep Stanby mode
	{ 0,                    1          },  //  wait_ms(1);
	{ SSITX,                0x00000000 },  //   # SPI.SSITX  
	{ 0,                    1          },  //  wait_ms(1);
	{ SSITX,                0x00000000 },  //   # SPI.SSITX  
	{ 0,                    1          },  //  wait_ms(1);
	{ SSITX,                0x000800BA },  //   # SPI.SSITX          *NOTE 1  # Command setting of SPI block
	{ SSITX,                0x00000111 },  //     # Display mode setup(1) : Normaly Black
	{ SSITX,                0x00080036 },  //     # Command setting of SPI block
	{ SSITX,                0x00000100 },  //     # Memory access control
	{ 0,                    2          },  //  wait_ms(2);    //      #  Wait SPI fifo empty
	{ SSITX,                0x000800BB },  //   # Command setting of SPI block
	{ SSITX,                0x00000100 },  //   # Display mode setup(2)
	{ SSITX,                0x0008003A },  //   # Command setting of SPI block
	{ SSITX,                0x00000160 },  //   # RGB Interface data format
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800BF },  //   # Command setting of SPI block
	{ SSITX,                0x00000100 },  //   # Drivnig method
	{ SSITX,                0x000800B1 },  //   # Command setting of SPI block
	{ SSITX,                0x0000015D },  //   # Booster operation setup
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800B2 },  //   # Command setting of SPI block
	{ SSITX,                0x00000133 },  //   # Booster mode setup
	{ SSITX,                0x000800B3 },  //   # Command setting of SPI block
	{ SSITX,                0x00000122 },  //     # Booster frequencies setup
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800B4 },  //     # Command setting of SPI block
	{ SSITX,                0x00000102 },  //     # OP-amp capability/System clock freq. division setup
	{ SSITX,                0x000800B5 },  //     # Command setting of SPI block
	{ SSITX,                0x0000011F },  //     # VCS Voltage adjustment  (1C->1F for Rev 2)
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800B6 },  //     # Command setting of SPI block
	{ SSITX,                0x00000128 },  //     # VCOM Voltage adjustment
	{ SSITX,                0x000800B7 },  //     # Command setting of SPI block
	{ SSITX,                0x00000103 },  //     # Configure an external display signal
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800B9 },  //     # Command setting of SPI block
	{ SSITX,                0x00000120 },  //     # DCCK/DCEV timing setup
	{ SSITX,                0x000800BD },  //     # Command setting of SPI block
	{ SSITX,                0x00000102 },  //     # ASW signal control
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800BE },  //     # Command setting of SPI block
	{ SSITX,                0x00000100 },  //     # Dummy display (white/black) count setup for QUAD Data operation
	{ SSITX,                0x000800C0 },  //     # Command setting of SPI block
	{ SSITX,                0x00000111 },  //     # wait_ms(-out FR count setup (A)
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800C1 },  //     # Command setting of SPI block
	{ SSITX,                0x00000111 },  //     # wait_ms(-out FR count setup (B)
	{ SSITX,                0x000800C2 },  //     # Command setting of SPI block
	{ SSITX,                0x00000111 },  //     # wait_ms(-out FR count setup (C)
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800C3 },  //     # Command setting of SPI block
	{ SSITX,                0x0008010A },  //     # wait_ms(-in line clock count setup (D)
	{ SSITX,                0x0000010A },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800C4 },  //     # Command setting of SPI block
	{ SSITX,                0x00080160 },  //     # Seep-in line clock count setup (E)
	{ SSITX,                0x00000160 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800C5 },  //     # Command setting of SPI block
	{ SSITX,                0x00080160 },  //     # wait_ms(-in line clock count setup (F)
	{ SSITX,                0x00000160 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800C6 },  //     # Command setting of SPI block
	{ SSITX,                0x00080160 },  //     # wait_ms(-in line clock setup (G)
	{ SSITX,                0x00000160 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800C7 },  //     # Command setting of SPI block
	{ SSITX,                0x00080133 },  //     # Gamma 1 fine tuning (1)
	{ SSITX,                0x00000143 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800C8 },  //     # Command setting of SPI block
	{ SSITX,                0x00000144 },  //     # Gamma 1 fine tuning (2)
	{ SSITX,                0x000800C9 },  //     # Command setting of SPI block
	{ SSITX,                0x00000133 },  //     # Gamma 1 inclination adjustment
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800CA },  //     # Command setting of SPI block
	{ SSITX,                0x00000100 },  //     # Gamma 1 blue offset adjustment
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800EC },  //     # Command setting of SPI block
	{ SSITX,                0x00080102 },  //     # Total number of horizontal clock cycles (1) [PCLK Sync. VGA setting]
	{ SSITX,                0x00000118 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800CF },  //     # Command setting of SPI block
	{ SSITX,                0x00000101 },  //     # Blanking period control (1) [PCLK Sync. Table1 for VGA]
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D0 },  //     # Command setting of SPI block
	{ SSITX,                0x00080110 },  //     # Blanking period control (2) [PCLK Sync. Table1 for VGA]
	{ SSITX,                0x00000104 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D1 },  //     # Command setting of SPI block
	{ SSITX,                0x00000101 },  //     # CKV timing control on/off [PCLK Sync. Table1 for VGA]
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D2 },  //     # Command setting of SPI block
	{ SSITX,                0x00080100 },  //     # CKV1,2 timing control [PCLK Sync. Table1 for VGA]
	{ SSITX,                0x0000013A },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D3 },  //     # Command setting of SPI block
	{ SSITX,                0x00080100 },  //     # OEV timing control [PCLK Sync. Table1 for VGA]
	{ SSITX,                0x0000013A },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D4 },  //     # Command setting of SPI block
	{ SSITX,                0x00080124 },  //     # ASW timing control (1) [PCLK Sync. Table1 for VGA]
	{ SSITX,                0x0000016E },  //     
	{ 0,                    1          },  //  wait_ms(1);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D5 },  //     # Command setting of SPI block
	{ SSITX,                0x00000124 },  //     # ASW timing control (2) [PCLK Sync. Table1 for VGA]
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800ED },  //     # Command setting of SPI block
	{ SSITX,                0x00080101 },  //     # Total number of horizontal clock cycles (2) [PCLK Sync. Table1 for QVGA ]
	{ SSITX,                0x0000010A },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D6 },  //     # Command setting of SPI block
	{ SSITX,                0x00000101 },  //     # Blanking period control (1) [PCLK Sync. Table2 for QVGA]
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D7 },  //     # Command setting of SPI block
	{ SSITX,                0x00080110 },  //     # Blanking period control (2) [PCLK Sync. Table2 for QVGA]
	{ SSITX,                0x0000010A },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D8 },  //     # Command setting of SPI block
	{ SSITX,                0x00000101 },  //     # CKV timing control on/off [PCLK Sync. Table2 for QVGA]
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800D9 },  //     # Command setting of SPI block
	{ SSITX,                0x00080100 },  //     # CKV1,2 timing control [PCLK Sync. Table2 for QVGA]
	{ SSITX,                0x00000114 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800DE },  //     # Command setting of SPI block
	{ SSITX,                0x00080100 },  //     # OEV timing control [PCLK Sync. Table2 for QVGA]
	{ SSITX,                0x00000114 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800DF },  //     # Command setting of SPI block
	{ SSITX,                0x00080112 },  //     # ASW timing control (1) [PCLK Sync. Table2 for QVGA]
	{ SSITX,                0x0000013F },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800E0 },  //     # Command setting of SPI block
	{ SSITX,                0x0000010B },  //     # ASW timing control (2) [PCLK Sync. Table2 for QVGA]
	{ SSITX,                0x000800E2 },  //     # Command setting of SPI block
	{ SSITX,                0x00000101 },  //     # Built-in oscillator frequency division setup [Frequency division ratio : 2 (60Hq)
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800E3 },  //     # Command setting of SPI block
	{ SSITX,                0x00000136 },  //     # Built-in oscillator clock count setup
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800E4 },  //     # Command setting of SPI block
	{ SSITX,                0x00080100 },  //     # CKV timing control for using build-in osc
	{ SSITX,                0x00000103 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800E5 },  //     # Command setting of SPI block
	{ SSITX,                0x00080102 },  //     # OEV timing control for using build-in osc
	{ SSITX,                0x00000104 },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800E6 },  //     # Command setting of SPI block
	{ SSITX,                0x00000103 },  //     # DCEV timing control for using build-in osc
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800E7 },  //     # Command setting of SPI block
	{ SSITX,                0x00080104 },  //     # ASW timing setup for using build-in osc(1)
	{ SSITX,                0x0000010A },  //     
	{ 0,                    2          },  //  wait_ms(2);      //    #  Wait SPI fifo empty
	{ SSITX,                0x000800E8 },  //     # Command setting of SPI block
	{ SSITX,                0x00000104 },  //     # ASW timing setup for using build-in osc(2)
            
            
	{ CLKENB,               0x000001EF },  //   # SYS.CLKENB  # DCLK enable
	{ START,                0x00000000 },  //   # LCD.START  # LCDC wait_ms( mode
	{ WRSTB,                0x0000003F },  //   # LCD.WRSTB  # write_client_reg( strobe
	{ RDSTB,                0x00000432 },  //   # LCD.RDSTB  # Read strobe
	{ PORT_ENB,             0x00000002 },  //   # LCD.PORT_ENB  # Asynchronous port enable
	{ VSYNIF,               0x00000000 },  //   # LCD.VSYNCIF  # VSYNC I/F mode set
	{ ASY_DATA,             0x80000000 },  //   # LCD.ASY_DATx  # Index setting of SUB LCDD
	{ ASY_DATB,             0x00000001 },  //     # Oscillator start
	{ ASY_CMDSET,           0x00000005 },  //   # LCD.ASY_CMDSET  # Direct command transfer enable
	{ ASY_CMDSET,           0x00000004 },  //   # LCD.ASY_CMDSET  # Direct command transfer disable
	{ 0,                    10         },  //  wait_ms(10);
	{ ASY_DATA,             0x80000000 },  //   # LCD.ASY_DATx  # DUMMY write_client_reg(@*NOTE2
	{ ASY_DATB,             0x80000000 },  //     
	{ ASY_DATC,             0x80000000 },  //     
	{ ASY_DATD,             0x80000000 },  //     
	{ ASY_CMDSET,           0x00000009 },  //   # LCD.ASY_CMDSET  
	{ ASY_CMDSET,           0x00000008 },  //   # LCD.ASY_CMDSET  
	{ ASY_DATA,             0x80000007 },  //   # LCD.ASY_DATx  # Index setting of SUB LCDD
	{ ASY_DATB,             0x00004005 },  //     # LCD driver control
	{ ASY_CMDSET,           0x00000005 },  //   # LCD.ASY_CMDSET  # Direct command transfer enable
	{ ASY_CMDSET,           0x00000004 },  //   # LCD.ASY_CMDSET  # Direct command transfer disable
	{ 0,                    20         },  //  wait_ms(20);
	{ ASY_DATA,             0x80000059 },  //   # LCD.ASY_DATx  # Index setting of SUB LCDD
	{ ASY_DATB,             0x00000000 },  //     # LTPS I/F control
	{ ASY_CMDSET,           0x00000005 },  //   # LCD.ASY_CMDSET  # Direct command transfer enable
	{ ASY_CMDSET,           0x00000004 },  //   # LCD.ASY_CMDSET  # Direct command transfer disable

	{ VSYNIF,               0x00000001 },  //   # LCD.VSYNCIF  # VSYNC I/F mode OFF
	{ PORT_ENB,             0x00000001 },  //   # LCD.PORT_ENB  # SYNC I/F  output select

	/******************************/

	{ VSYNIF,               0x00000001 },  // VSYNC I/F mode OFF
	{ PORT_ENB,             0x00000001 },  // SYNC I/F mode ON

	{ BITMAP1,              0x01E000F0 },  // MDC.BITMAP2  ); // Setup of PITCH size to Frame buffer1
	{ BITMAP2,              0x01E000F0 },  // MDC.BITMAP3  ); // Setup of PITCH size to Frame buffer2
	{ BITMAP3,              0x01E000F0 },  // MDC.BITMAP4  ); // Setup of PITCH size to Frame buffer3
	{ BITMAP4,              0x00DC00B0 },  // MDC.BITMAP5  ); // Setup of PITCH size to Frame buffer4
	{ CLKENB,               0x000001EF },  // SYS.CLKENB  ); // DCLK supply
	{ PORT_ENB,             0x00000001 },  // LCD.PORT_ENB  ); // Synchronous port enable
	{ PORT,                 0x00000004 },  // LCD.PORT  ); // Polarity of DE is set to high active
	{ PXL,                  0x00000002 },  // LCD.PXL  ); // ACTMODE 2 set (1st frame black data output)
	{ MPLFBUF,              0x00000000 },  // LCD.MPLFBUF  ); // Select the reading buffer
	{ HCYCLE,               0x0000010b },  // LCD.HCYCLE  ); // Setup to VGA size
	{ HSW,                  0x00000003 },  // LCD.HSW  
	{ HDE_START,            0x00000007 },  // LCD.HDE_START  
	{ HDE_SIZE,             0x000000EF },  // LCD.HDE_SIZE  
	{ VCYCLE,               0x00000285 },  // LCD.VCYCLE  
	{ VSW,                  0x00000001 },  // LCD.VSW  
	{ VDE_START,            0x00000003 },  // LCD.VDE_START  
	{ VDE_SIZE,             0x0000027F },  // LCD.VDE_SIZE  

	{ START,                0x00000001 },  // LCD.START  ); // LCDC - Pixel data transfer start

	{ 0,                    10         },  //  wait_ms( 10  );
	{ SSITX,                0x000800BC },  // SPI.SSITX  ); // Command setting of SPI block
	{ SSITX,                0x00000180 },  // Display data setup
	{ SSITX,                0x0008003B },  // Command setting of SPI block
	{ SSITX,                0x00000100 },  // Quad Data configuration - VGA
	{ 0,                    1          },  //  wait_ms( 1          ); //  Wait SPI fifo empty
	{ SSITX,                0x000800B0 },  // Command setting of SPI block
	{ SSITX,                0x00000116 },  // Power supply ON/OFF control
	{ 0,                    1          },  //  wait_ms( 1          ); //  Wait SPI fifo empty
	{ SSITX,                0x000800B8 },  // Command setting of SPI block
	{ SSITX,                0x000801FF },  // Output control
	{ SSITX,                0x000001F5 },   
	{ 0,                    1          },  //  wait_ms( 1);         //  Wait SPI fifo empty
	{ SSITX,                0x00000011 },  // wait_ms(-out (Command only)
	{ SSITX,                0x00000029 },  // Display on (Command only)

	{ SYSTEM_BLOCK1_BASE,   0x00000002 },  //    # wakeREQ -> GPIO

	{ 0, 0 }
};

static void _panel_init(struct init_table *init_table)
{
	unsigned n;

	dprintf(INFO, "panel_init()\n");

	n = 0;
	while (init_table[n].reg != 0 || init_table[n].val != 0) {
		if (init_table[n].reg != 0)
			mddi_remote_write(init_table[n].val, init_table[n].reg);
		else
			thread_sleep(init_table[n].val);//mdelay(init_table[n].val);
		n++;
	}

	dprintf(INFO, "panel_init() done\n");
}

void panel_init(struct mddi_client_caps *client_caps)
{
	switch(client_caps->manufacturer_name) {
	case 0xd263: // Toshiba
		dprintf(INFO, "Found Toshiba panel\n");
		_panel_init(toshiba_480x640_init_table);
		break;
	case 0x4474: //??
		if (client_caps->product_code == 0xc065)
			dprintf(INFO, "Found WVGA panel\n");
		break;
	}
}
#endif //mddi

void panel_poweron(void)
{
#if DISPLAY_TYPE_MDDI
	gpio_set(88, 0);
	gpio_config(88, GPIO_OUTPUT);
	thread_sleep(1); //udelay(10);
	gpio_set(88, 1);
	thread_sleep(10); //mdelay(10);

	//mdelay(1000); // uncomment for second stage boot
#elif DISPLAY_TYPE_LCDC
	panel_backlight(1);
	lcdc_on();
#endif
}

void panel_backlight(int on)
{
    unsigned char reg_data = 0xA0;
    if(on)
        pmic_write(0x132, reg_data);
    else
        pmic_write(0x132, 0);
}

static unsigned wega_reset_gpio =
GPIO_CFG(180, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA);

#define LDO12_CNTRL            0x015
#define LDO15_CNTRL            0x089
#define LDO16_CNTRL            0x08A
#define LDO20_CNTRL            0x11F  // PM8058 only
#define LDO_LOCAL_EN_BMSK      0x80

static int display_common_power(int on)
{
    int rc = 0, flag_on = !!on;
    static int display_common_power_save_on;
    unsigned int vreg_ldo12, vreg_ldo15, vreg_ldo20, vreg_ldo16, vreg_ldo8;
    if (display_common_power_save_on == flag_on)
        return 0;

    display_common_power_save_on = flag_on;

    if (on) {
        /* reset Toshiba WeGA chip -- toggle reset pin -- gpio_180 */
        rc = gpio_tlmm_config(wega_reset_gpio, GPIO_ENABLE);
        if (rc) {
            return rc;
        }

        gpio_set(180, 0);   /* bring reset line low to hold reset*/
    }

    // Set power for WEGA chip.
    // Set LD020 to 1.5V
    pmic_write(LDO20_CNTRL, 0x00 | LDO_LOCAL_EN_BMSK);
    mdelay(5);

    // Set LD012 to 1.8V
    pmic_write(LDO12_CNTRL, 0x06 | LDO_LOCAL_EN_BMSK);
    mdelay(5);

    // Set LD016 to 2.6V
    pmic_write(LDO16_CNTRL, 0x16 | LDO_LOCAL_EN_BMSK);
    mdelay(5);

    // Set LD015 to 3.0V
    pmic_write(LDO15_CNTRL, 0x1E | LDO_LOCAL_EN_BMSK);
    mdelay(5);

    gpio_set(180, 1);   /* bring reset line high */
    mdelay(10); /* 10 msec before IO can be accessed */
    if (rc) {
        return rc;
    }

    return rc;
}

#if DISPLAY_TYPE_LCDC
static struct msm_gpio lcd_panel_gpios[] = {
    { GPIO_CFG(45, 0, GPIO_OUTPUT,  GPIO_NO_PULL, GPIO_2MA), "spi_clk" },
    { GPIO_CFG(46, 0, GPIO_OUTPUT,  GPIO_NO_PULL, GPIO_2MA), "spi_cs0" },
    { GPIO_CFG(47, 0, GPIO_OUTPUT,  GPIO_NO_PULL, GPIO_2MA), "spi_mosi" },
    { GPIO_CFG(48, 0, GPIO_INPUT,  GPIO_NO_PULL, GPIO_2MA), "spi_miso" }
};

int lcdc_toshiba_panel_power(int on)
{
    int rc, i;
    struct msm_gpio *gp;

    rc = display_common_power(on);
    if (rc < 0) {
        return rc;
    }

    if (on) {
        rc = platform_gpios_enable(lcd_panel_gpios,
                ARRAY_SIZE(lcd_panel_gpios));
        if(rc)
        {
            return rc;
        }
    } else {    /* off */
        gp = lcd_panel_gpios;
        for (i = 0; i < ARRAY_SIZE(lcd_panel_gpios); i++) {
            /* ouput low */
            gpio_set(GPIO_PIN(gp->gpio_cfg), 0);
            gp++;
        }
    }

    return rc;
}

#define SPI_SCLK    45
#define SPI_CS      46
#define SPI_MOSI    47
#define SPI_MISO    48

static void toshiba_spi_write_byte(char dc, unsigned char data)
{
    unsigned bit;
    int bnum;

    gpio_set(SPI_SCLK, 0); /* clk low */
    /* dc: 0 for command, 1 for parameter */
    gpio_set(SPI_MOSI, dc);
    mdelay(1);  /* at least 20 ns */
    gpio_set(SPI_SCLK, 1); /* clk high */
    mdelay(1);  /* at least 20 ns */
    bnum = 8;   /* 8 data bits */
    bit = 0x80;
    while (bnum) {
        gpio_set(SPI_SCLK, 0); /* clk low */
        if (data & bit)
            gpio_set(SPI_MOSI, 1);
        else
            gpio_set(SPI_MOSI, 0);
        mdelay(1);
        gpio_set(SPI_SCLK, 1); /* clk high */
        mdelay(1);
        bit >>= 1;
        bnum--;
    }
}

static int toshiba_spi_write (char cmd, unsigned data, int num)
{
    char *bp;
    gpio_set(SPI_CS, 1);    /* cs high */

    /* command byte first */
    toshiba_spi_write_byte(0, cmd);

    /* followed by parameter bytes */
    if (num) {
        bp = (char *)&data;;
        bp += (num - 1);
        while (num) {
            toshiba_spi_write_byte(1, *bp);
            num--;
            bp--;
        }
    }
    gpio_set(SPI_CS, 0);    /* cs low */
    mdelay(1);
    return 0;
}


void lcdc_disp_on (void)
{
    gpio_set(SPI_CS, 0);    /* low */
    gpio_set(SPI_SCLK, 1);  /* high */
    gpio_set(SPI_MOSI, 0);
    gpio_set(SPI_MISO, 0);

    if (1) {
        toshiba_spi_write(0, 0, 0);
        mdelay(7);
        toshiba_spi_write(0, 0, 0);
        mdelay(7);
        toshiba_spi_write(0, 0, 0);
        mdelay(7);
        toshiba_spi_write(0xba, 0x11, 1);
        toshiba_spi_write(0x36, 0x00, 1);
        mdelay(1);
        toshiba_spi_write(0x3a, 0x60, 1);
        toshiba_spi_write(0xb1, 0x5d, 1);
        mdelay(1);
        toshiba_spi_write(0xb2, 0x33, 1);
        toshiba_spi_write(0xb3, 0x22, 1);
        mdelay(1);
        toshiba_spi_write(0xb4, 0x02, 1);
        toshiba_spi_write(0xb5, 0x1e, 1); /* vcs -- adjust brightness */
        mdelay(1);
        toshiba_spi_write(0xb6, 0x27, 1);
        toshiba_spi_write(0xb7, 0x03, 1);
        mdelay(1);
        toshiba_spi_write(0xb9, 0x24, 1);
        toshiba_spi_write(0xbd, 0xa1, 1);
        mdelay(1);
        toshiba_spi_write(0xbb, 0x00, 1);
        toshiba_spi_write(0xbf, 0x01, 1);
        mdelay(1);
        toshiba_spi_write(0xbe, 0x00, 1);
        toshiba_spi_write(0xc0, 0x11, 1);
        mdelay(1);
        toshiba_spi_write(0xc1, 0x11, 1);
        toshiba_spi_write(0xc2, 0x11, 1);
        mdelay(1);
        toshiba_spi_write(0xc3, 0x3232, 2);
        mdelay(1);
        toshiba_spi_write(0xc4, 0x3232, 2);
        mdelay(1);
        toshiba_spi_write(0xc5, 0x3232, 2);
        mdelay(1);
        toshiba_spi_write(0xc6, 0x3232, 2);
        mdelay(1);
        toshiba_spi_write(0xc7, 0x6445, 2);
        mdelay(1);
        toshiba_spi_write(0xc8, 0x44, 1);
        toshiba_spi_write(0xc9, 0x52, 1);
        mdelay(1);
        toshiba_spi_write(0xca, 0x00, 1);
        mdelay(1);
        toshiba_spi_write(0xec, 0x02a4, 2); /* 0x02a4 */
        mdelay(1);
        toshiba_spi_write(0xcf, 0x01, 1);
        mdelay(1);
        toshiba_spi_write(0xd0, 0xc003, 2); /* c003 */
        mdelay(1);
        toshiba_spi_write(0xd1, 0x01, 1);
        mdelay(1);
        toshiba_spi_write(0xd2, 0x0028, 2);
        mdelay(1);
        toshiba_spi_write(0xd3, 0x0028, 2);
        mdelay(1);
        toshiba_spi_write(0xd4, 0x26a4, 2);
        mdelay(1);
        toshiba_spi_write(0xd5, 0x20, 1);
        mdelay(1);
        toshiba_spi_write(0xef, 0x3200, 2);
        mdelay(32);
        toshiba_spi_write(0xbc, 0x80, 1);   /* wvga pass through */
        toshiba_spi_write(0x3b, 0x00, 1);
        mdelay(1);
        toshiba_spi_write(0xb0, 0x16, 1);
        mdelay(1);
        toshiba_spi_write(0xb8, 0xfff5, 2);
        mdelay(1);
        toshiba_spi_write(0x11, 0, 0);
        mdelay(5);
        toshiba_spi_write(0x29, 0, 0);
        mdelay(5);
    }
}

void lcdc_on(void)
{
    lcdc_clock_init(27648000);
    lcdc_toshiba_panel_power(1);
    lcdc_disp_on();
}

#endif
