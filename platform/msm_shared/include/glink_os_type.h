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

#ifndef OS_TYPE_H
#define OS_TYPE_H

/*===========================================================================

                      G-Link OS Specific types/functions

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include <sys/types.h>
#include <string.h>
#include <assert.h>

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/

#define OS_LOG_INIT( ) glink_os_log_init()
#define GLINK_OS_SEC_TO_USEC (1000000ULL)
#define GLINK_OS_SEC_TO_TICK (19200000ULL)

#define GLINK_OS_TICK_TO_USEC(t) ((t)*10ULL/192ULL)
#define GLINK_OS_USEC_TO_TICK(u) ((u)*192ULL/10ULL)

#define GLINK_OS_UNREFERENCED_PARAM( param ) ( void )param

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
#define DALSYS_SYNC_OBJ_SIZE_BYTES           40
#define DALSYS_EVENT_OBJ_SIZE_BYTES          80

typedef struct DALSYSEventObj  DALSYSEventObj;

struct DALSYSEventObj
{
  unsigned long long _bSpace[DALSYS_EVENT_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
};


typedef struct DALSYSSyncObj  DALSYSSyncObj;
struct DALSYSSyncObj
{
  unsigned long long _bSpace[DALSYS_SYNC_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
};

typedef DALSYSSyncObj os_cs_type;

typedef void ( *os_timer_cb_fn )( void *cb_data );

typedef struct
{
  void * dal_event;
  DALSYSEventObj    dal_obj_memory;
}os_event_type;

typedef void ( *os_isr_cb_fn )( void *cb_data );

typedef struct os_ipc_intr_struct
{
  uint32_t processor;
  uint32_t irq_out;
} os_ipc_intr_type;

typedef unsigned long long os_timetick_type;

#endif /* OS_TYPE_H */
