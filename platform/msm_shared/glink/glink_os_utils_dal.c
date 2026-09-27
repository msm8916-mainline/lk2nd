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


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "glink_os_utils.h"
#include <arch/defines.h>
#include <malloc.h>
#include <kernel/thread.h>
#include <string.h>
#include <kernel/event.h>
#include <platform/irqs.h>
#include <platform/interrupts.h>

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

#define OS_LOG_BUFFER_SIZE  ( 16384 )

/*===========================================================================
                           TYPE DEFINITIONS
===========================================================================*/
typedef void (*glink_os_thread_fn_type)(void* param);

typedef struct _glink_os_thread_info_type {
  glink_os_thread_fn_type thread_fn;
  void                    *param;
}glink_os_thread_info_type;

static uint32 cs_variable = 1;
/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      glink_os_cs_init
===========================================================================*/
/**
  Initializes a Critical Section

  @param[in]  cs   pointer to critical section object allocated by caller.

  @return
  TRUE if critical section was initialized, FALSE otherwise
*/
/*=========================================================================*/
boolean glink_os_cs_init( os_cs_type *cs )
{
  boolean return_value = TRUE;

  /* Create the new critical section */

  return ( return_value );
}

/*===========================================================================
  FUNCTION  glink_os_cs_deinit
===========================================================================*/
/**
  This function de-initializes a critical section.

  @param[in]  cs         Pointer to the critical section to be de-initialized.

  @return
  TRUE if critical section was initialized, FALSE otherwise

*/
/*=========================================================================*/
boolean glink_os_cs_deinit( os_cs_type *cs )
{
  /* Deinitialize the critical section */

  return TRUE;
}

/*===========================================================================
FUNCTION      glink_os_cs_create
===========================================================================*/
/**
  Create and initializesa Critical Section

  @return     The critical section.
*/
/*=========================================================================*/
os_cs_type *glink_os_cs_create( void )
{
  /* Create the new critical section */

  return ( os_cs_type * )cs_variable;
}


/*===========================================================================
FUNCTION      glink_os_cs_acquire
===========================================================================*/
/**
  Lock a critical section

  @param[in]  cs   Pointer the the critical section

  @return     None.
*/
/*=========================================================================*/
void glink_os_cs_acquire( os_cs_type *cs )
{
  enter_critical_section();
}

/*===========================================================================
FUNCTION      glink_os_cs_release
===========================================================================*/
/**
  Unlock a critical section

  @param[in]  cs   Pointer the the critical section

  @return     None.
*/
/*=========================================================================*/
void glink_os_cs_release( os_cs_type *cs )
{
  exit_critical_section();
}

/*===========================================================================
FUNCTION      glink_os_cs_destroy
===========================================================================*/
/**
  Destroys a Critical Section

  @return     none.
*/
/*=========================================================================*/
void glink_os_cs_destroy( os_cs_type *cs )
{
  /* Initialize the critical section */
  return;
}

/*===========================================================================
  FUNCTION  glink_os_malloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap.  The region should be
  freed using \c glink_os_free when no longer used.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/

void *glink_os_malloc( size_t size )
{
  void *pMem;

  pMem = malloc(size);
  if (pMem == NULL)
      return NULL;
  return pMem;
}

/*===========================================================================
  FUNCTION  glink_os_calloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap and initialize with
  the zeroes.  The region should be freed using \c glink_os_free
  when no longer used.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void *glink_os_calloc( size_t size )
{
  void *pMem;
  pMem = malloc(size);
  if( pMem == NULL )
  {
    return NULL;
  }
  else
  {
    memset( pMem, 0, size );
    return pMem;
  }
}

/*===========================================================================
  FUNCTION  glink_os_free
===========================================================================*/
/**
  Free a region of memory that was allocated by \c glink_os_malloc.

  @param[in] pMem    The reference to the region of memory to be freed.

  @return    NA
*/
/*=========================================================================*/
void glink_os_free( void *pMem )
{
  free( pMem );
}

/*===========================================================================
  FUNCTION  glink_os_string_copy
===========================================================================*/
/**
  Copies the source string into the destination buffer until
  size is reached, or until a '\0' is encountered.  If valid,
  the destination string will always be NULL deliminated.

  @param[in] dst    The destination string, contents will be updated.
  @param[in] src    The source string
  @param[in] size   The maximum copy size (size of dst)

  @return
  The destination string pointer, dst.
*/
/*==========================================================================*/
char *glink_os_string_copy( char *dst, const char *src, uint32 size )
{
  ( void )strlcpy( dst, src, size );

  return dst;
}

/*===========================================================================
  FUNCTION  glink_os_string_compare
===========================================================================*/
/**
  Compares two strings delimited by size or NULL character.

  @param[in] s1     String 1
  @param[in] s2     String 2

  @return
  0 if strings are identical (up to size characters), non-zero otherwise
*/
/*==========================================================================*/
long glink_os_string_compare( const char *s1, const char *s2 )
{
  return strcmp( s1, s2 );
}

/*===========================================================================
  FUNCTION  glink_os_copy_mem
===========================================================================*/
/**
  Copies the source buffer into the destination buffer.

  @param[in] dst    The destination, contents will be updated.
  @param[in] src    The source
  @param[in] size   The maximum copy size (size of dst)

  @return
  The destination string pointer, dst.
*/
/*==========================================================================*/
void glink_os_copy_mem( void *dst, const void *src, uint32 size )
{
  memcpy( dst, src, size );
}

/*===========================================================================
  FUNCTION      glink_os_register_isr
===========================================================================*/
/**
  Registers ISR with the interrupt controller

  @param[in]  irq_in    Interrupt to register for
  @param[in]  isr       Callback to be invoked when interrupt fires
  @param[in]  cb_data   Data to be provided to the callback

  @return     TRUE if registration was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_register_isr( uint32 irq_in, os_isr_cb_fn isr, void* cb_data )
{
  boolean return_value = TRUE;

  /* Register the interrupt */
  dprintf(SPEW, "Register interrupt: %u\n", irq_in);
  register_int_handler(irq_in, (int_handler)(isr), cb_data);

  return ( return_value );
}

/*===========================================================================
  FUNCTION      glink_os_deregister_isr
===========================================================================*/
/**
  De-registers ISR with the interrupt controller

  @param[in]  irq_in    Interrupt to deregister for

  @return     TRUE if de-registration was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_deregister_isr( uint32 irq_in )
{
  boolean return_value = TRUE;
  mask_interrupt(irq_in);
  return ( return_value );
}

/*===========================================================================
  FUNCTION      glink_os_enable_interrupt
===========================================================================*/
/**
  Enables the interrupt in the interrupt controller

  @param[in]  irq_in    Interrupt to enable

  @return     TRUE if operation was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_enable_interrupt( uint32 irq_in )
{
  boolean return_value = TRUE;
  unmask_interrupt(irq_in);
  return ( return_value );
}

/*===========================================================================
  FUNCTION      glink_os_disable_interrupt
===========================================================================*/
/**
  Disables the interrupt in the interrupt controller

  @param[in]  irq_in    Interrupt to disable

  @return     TRUE if operation was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_disable_interrupt( uint32 irq_in )
{
  boolean return_value = TRUE;
  dprintf(INFO, "Disable IPC Interrupt\n");
  mask_interrupt(irq_in);
  return ( return_value );
}
