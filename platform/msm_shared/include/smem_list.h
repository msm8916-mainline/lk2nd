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

#ifndef SMEM_LIST_H
#define SMEM_LIST_H

/**
 * @file smem_list.h
 *
 * Public data types and functions to implement a generic
 * list.
 */

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup smem
@{ */


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/** Defines the structure to link to the SMEM list */
typedef struct smem_list_link
{
  struct smem_list_link *next;
} smem_list_link_type;


/** Defines the generic SMEM list type */
typedef struct smem_list
{
  smem_list_link_type *next;
  long count;
} smem_list_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smem_list_init
===========================================================================*/
/**
  Creates a generic list.

  @param[in/out] list   Pointer to the user list.

  @return
  N/A.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_init( smem_list_type *list );

/*===========================================================================
FUNCTION      smem_list_append
===========================================================================*/
/**
  Appends an element to the end of a generic list.

  The element must contain a smem_list_link_type at its base address.
  The element should not be already present in any list.

  @param[in/out] list   Pointer to the user list.
  @param[in] item       Element to be inserted.

  @return
  N/A.

  @dependencies
  The element must contain a smem_list_link_type at its base address.
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_append( smem_list_type *list, void *item );

/*===========================================================================
FUNCTION      smem_list_delete
===========================================================================*/
/**
  Removes the specified element from a generic list.

  The element may be present at any point in the list.

  @param[in] list       Pointer to the user list.
  @param[in] item       Element to be removed.

  @return
  0 if the element is deleted, or a negative error code if the element
  is not found.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
long smem_list_delete( smem_list_type *list, void *item );

/*===========================================================================
FUNCTION      smem_list_first
===========================================================================*/
/**
  Retrieves the first element in the list without removing it.

  @param[in] list       Pointer to the user list.

  @return
  Pointer to the first element in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_list_first( smem_list_type *list );

/*===========================================================================
FUNCTION      smem_list_next
===========================================================================*/
/**
  Retrieves the next element in the list.

  @param[in] list       Pointer to the user list.

  @return
  Pointer to the next element in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_list_next( void * item );

/*===========================================================================
FUNCTION      smem_list_count
===========================================================================*/
/**
  Gets the number of elements in the list.

  @param[in] list       Pointer to the user list.

  @return
  Count of elements in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
long smem_list_count( smem_list_type *list );

#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup smem */

#endif /* SMEM_LIST_H */
