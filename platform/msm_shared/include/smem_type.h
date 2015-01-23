/* Copyright (c) 2015, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SMEM_TYPE_H
#define SMEM_TYPE_H

/**
 * @file smem_type.h
 *
 * Public data types used by SMEM
 */

/** @addtogroup smem
@{ */

/*===========================================================================
                               TYPE DEFINITIONS
===========================================================================*/
/******************************************************************************
   The most significant two bytes of this number are the smem major version and
 the least significant two bytes are the smem minor version.  The major version
 number should be updated whenever a change which causes an incompatibility is
 introduced.
   The minor version number can track API changes and deprecations that will
 not affect remote processors.  This may include changes to the smem_mem_type
 enum, if dependencies have already been satisfied on the relevant processors.
   Inconsistencies in minor version, between processors, will not prevent smem
 from booting, but major version inconsistencies will.
   0x2XXX in the minor version indicates that it is the SMEM minor version, and
 unrelated to proc comm, which no longer exists.
******************************************************************************/
#define SMEM_VERSION_ID                     0x000B2002

#define SMEM_MAJOR_VERSION_MASK             0xFFFF0000
#define SMEM_MINOR_VERSION_MASK             0x0000FFFF
#define SMEM_FULL_VERSION_MASK              0xFFFFFFFF

#define SMEM_NUM_SMD_CHANNELS               64
#define SMEM_NUM_SMP2P_EDGES                8

/** Types of memory that can be requested via smem_alloc.

  All of these types of memory have corresponding buffers allocated in
  smem_data_decl. If a buffer is added to smem_data_decl, add a corresponding
  entry to this list.

  SMEM_VERSION_FIRST and SMEM_VERSION_LAST are the first and last
  boundaries for external version checking via the smem_version_set routine.
  To set up versioning for a shared item, add an entry between
  SMEM_VERSION_FIRST and SMEM_VERSION_LAST and update the SMEM version in
  smem_version.h.

  SMEM_VERSION_LAST need not be the last item in the enum.
*/
typedef enum
{
  SMEM_MEM_FIRST,
  SMEM_RESERVED_PROC_COMM = SMEM_MEM_FIRST,
  SMEM_FIRST_FIXED_BUFFER = SMEM_RESERVED_PROC_COMM,
  SMEM_HEAP_INFO,
  SMEM_ALLOCATION_TABLE,
  SMEM_VERSION_INFO,
  SMEM_HW_RESET_DETECT,
  SMEM_RESERVED_AARM_WARM_BOOT,
  SMEM_DIAG_ERR_MESSAGE,
  SMEM_SPINLOCK_ARRAY,
  SMEM_MEMORY_BARRIER_LOCATION,
  SMEM_LAST_FIXED_BUFFER = SMEM_MEMORY_BARRIER_LOCATION,
  SMEM_AARM_PARTITION_TABLE,
  SMEM_AARM_BAD_BLOCK_TABLE,
  SMEM_RESERVE_BAD_BLOCKS,
  SMEM_RESERVED_WM_UUID,
  SMEM_CHANNEL_ALLOC_TBL,
  SMEM_SMD_BASE_ID,
  SMEM_SMEM_LOG_IDX = SMEM_SMD_BASE_ID + SMEM_NUM_SMD_CHANNELS,
  SMEM_SMEM_LOG_EVENTS,
  SMEM_RESERVED_SMEM_STATIC_LOG_IDX,
  SMEM_RESERVED_SMEM_STATIC_LOG_EVENTS,
  SMEM_RESERVED_SMEM_SLOW_CLOCK_SYNC,
  SMEM_RESERVED_SMEM_SLOW_CLOCK_VALUE,
  SMEM_RESERVED_BIO_LED_BUF,
  SMEM_SMSM_SHARED_STATE,
  SMEM_RESERVED_SMSM_INT_INFO,
  SMEM_RESERVED_SMSM_SLEEP_DELAY,
  SMEM_RESERVED_SMSM_LIMIT_SLEEP,
  SMEM_RESERVED_SLEEP_POWER_COLLAPSE_DISABLED,
  SMEM_APPS_POWER_STATUS = SMEM_RESERVED_SLEEP_POWER_COLLAPSE_DISABLED, /*APSS power status*/
  SMEM_RESERVED_KEYPAD_KEYS_PRESSED,
  SMEM_RESERVED_KEYPAD_STATE_UPDATED,
  SMEM_RESERVED_KEYPAD_STATE_IDX,
  SMEM_RESERVED_GPIO_INT,
  SMEM_RESERVED_MDDI_LCD_IDX,
  SMEM_RESERVED_MDDI_HOST_DRIVER_STATE,
  SMEM_RESERVED_MDDI_LCD_DISP_STATE,
  SMEM_RESERVED_LCD_CUR_PANEL,
  SMEM_RESERVED_MARM_BOOT_SEGMENT_INFO,
  SMEM_RESERVED_AARM_BOOT_SEGMENT_INFO,
  SMEM_RESERVED_SLEEP_STATIC,
  SMEM_RESERVED_SCORPION_FREQUENCY,
  SMEM_RESERVED_SMD_PROFILES,
  SMEM_RESERVED_TSSC_BUSY,
  SMEM_RESERVED_HS_SUSPEND_FILTER_INFO,
  SMEM_RESERVED_BATT_INFO,
  SMEM_RESERVED_APPS_BOOT_MODE,
  SMEM_VERSION_FIRST,
  SMEM_VERSION_SMD = SMEM_VERSION_FIRST,
  SMEM_VERSION_SMD_BRIDGE,
  SMEM_VERSION_SMSM,
  SMEM_VERSION_SMD_NWAY_LOOP,
  SMEM_VERSION_LAST = SMEM_VERSION_FIRST + 24,
  SMEM_RESERVED_OSS_RRCASN1_BUF1,
  SMEM_RESERVED_OSS_RRCASN1_BUF2,
  SMEM_ID_VENDOR0,
  SMEM_ID_VENDOR1,
  SMEM_ID_VENDOR2,
  SMEM_HW_SW_BUILD_ID,
  SMEM_RESERVED_SMD_BLOCK_PORT_BASE_ID,
  SMEM_RESERVED_SMD_BLOCK_PORT_PROC0_HEAP =
                   SMEM_RESERVED_SMD_BLOCK_PORT_BASE_ID +
                   SMEM_NUM_SMD_CHANNELS,
  SMEM_RESERVED_SMD_BLOCK_PORT_PROC1_HEAP =
                   SMEM_RESERVED_SMD_BLOCK_PORT_PROC0_HEAP +
                   SMEM_NUM_SMD_CHANNELS,
  SMEM_I2C_MUTEX = SMEM_RESERVED_SMD_BLOCK_PORT_PROC1_HEAP +
                   SMEM_NUM_SMD_CHANNELS,
  SMEM_SCLK_CONVERSION,
  SMEM_RESERVED_SMD_SMSM_INTR_MUX,
  SMEM_SMSM_CPU_INTR_MASK,
  SMEM_RESERVED_APPS_DEM_SLAVE_DATA,
  SMEM_RESERVED_QDSP6_DEM_SLAVE_DATA,
  SMEM_RESERVED_CLKREGIM_BSP,
  SMEM_RESERVED_CLKREGIM_SOURCES,
  SMEM_SMD_FIFO_BASE_ID,
  SMEM_USABLE_RAM_PARTITION_TABLE = SMEM_SMD_FIFO_BASE_ID +
                                    SMEM_NUM_SMD_CHANNELS,
  SMEM_POWER_ON_STATUS_INFO,
  SMEM_DAL_AREA,
  SMEM_SMEM_LOG_POWER_IDX,
  SMEM_SMEM_LOG_POWER_WRAP,
  SMEM_SMEM_LOG_POWER_EVENTS,
  SMEM_ERR_CRASH_LOG,
  SMEM_ERR_F3_TRACE_LOG,
  SMEM_SMD_BRIDGE_ALLOC_TABLE,
  SMEM_SMD_FEATURE_SET,
  SMEM_RESERVED_SD_IMG_UPGRADE_STATUS,
  SMEM_SEFS_INFO,
  SMEM_RESERVED_RESET_LOG,
  SMEM_RESERVED_RESET_LOG_SYMBOLS,
  SMEM_MODEM_SW_BUILD_ID,
  SMEM_SMEM_LOG_MPROC_WRAP,
  SMEM_RESERVED_BOOT_INFO_FOR_APPS,
  SMEM_SMSM_SIZE_INFO,
  SMEM_SMD_LOOPBACK_REGISTER,
  SMEM_SSR_REASON_MSS0,
  SMEM_SSR_REASON_WCNSS0,
  SMEM_SSR_REASON_LPASS0,
  SMEM_SSR_REASON_DSPS0,
  SMEM_SSR_REASON_VCODEC0,
  SMEM_VOICE,
  SMEM_ID_SMP2P_BASE_APPS, /* = 427 */
  SMEM_ID_SMP2P_BASE_MODEM = SMEM_ID_SMP2P_BASE_APPS +
                              SMEM_NUM_SMP2P_EDGES, /* = 435 */
  SMEM_ID_SMP2P_BASE_ADSP = SMEM_ID_SMP2P_BASE_MODEM +
                              SMEM_NUM_SMP2P_EDGES, /* = 443 */
  SMEM_ID_SMP2P_BASE_WCN = SMEM_ID_SMP2P_BASE_ADSP +
                              SMEM_NUM_SMP2P_EDGES, /* = 451 */
  SMEM_ID_SMP2P_BASE_RPM = SMEM_ID_SMP2P_BASE_WCN +
                              SMEM_NUM_SMP2P_EDGES, /* = 459 */
  SMEM_FLASH_DEVICE_INFO = SMEM_ID_SMP2P_BASE_RPM +
                              SMEM_NUM_SMP2P_EDGES, /* = 467 */
  SMEM_BAM_PIPE_MEMORY, /* = 468 */
  SMEM_IMAGE_VERSION_TABLE, /* = 469 */
  SMEM_LC_DEBUGGER, /* = 470 */
  SMEM_FLASH_NAND_DEV_INFO, /* =471 */
  SMEM_A2_BAM_DESCRIPTOR_FIFO,          /* = 472 */
  SMEM_CPR_CONFIG,                      /* = 473 */
  SMEM_CLOCK_INFO,                      /* = 474 */
  SMEM_IPC_INFO,                        /* = 475 */
  SMEM_RF_EEPROM_DATA,                  /* = 476 */
  SMEM_COEX_MDM_WCN,                    /* = 477 */
  SMEM_GLINK_NATIVE_XPORT_DESCRIPTOR,   /* = 478 */
  SMEM_GLINK_NATIVE_XPORT_FIFO_0,       /* = 479 */
  SMEM_GLINK_NATIVE_XPORT_FIFO_1,       /* = 480 */

  SMEM_MEM_LAST = SMEM_GLINK_NATIVE_XPORT_FIFO_1,
  SMEM_INVALID
} smem_mem_type;

/**
 * A list of hosts supported in SMEM
*/
typedef enum
{
  SMEM_APPS      = 0,                     /**< Apps Processor */
  SMEM_MODEM     = 1,                     /**< Modem processor */
  SMEM_ADSP      = 2,                     /**< ADSP processor */
  SMEM_RSVD3     = 3,                     /**< Reserved */
  SMEM_WCN       = 4,                     /**< WCN processor */
  SMEM_RSVD5     = 5,                     /**< Reserved */
  SMEM_RPM       = 6,                     /**< RPM processor */
  SMEM_NUM_HOSTS = 7,                     /**< Max number of host in target */
  SMEM_INVALID_HOST,                      /**< Invalid processor */

  SMEM_Q6        = SMEM_ADSP,             /**< Kept for legacy purposes.
                                           **  New code should use SMEM_ADSP */
  SMEM_RIVA      = SMEM_WCN,              /**< Kept for legacy purposes.
                                           **  New code should use SMEM_WCN */
}smem_host_type;

/** @} */ /* end_addtogroup smem */

#endif /* SMEM_TYPE_H */
