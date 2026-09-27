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

                         S H A R E D   M E M O R Y

===========================================================================*/



/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_list.h"
#include <sys/types.h>

/*=============================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*===========================================================================

                      PRIVATE FUNCTION DECLARATIONS

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
void smem_list_init( smem_list_type *list )
{
  list->next  = NULL;
  list->count = 0;
}

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
void smem_list_append( smem_list_type *list, void *item )
{
  smem_list_link_type *link = ( smem_list_link_type * )list;

  /* get next item until link->next points to null */
  while( link->next )
  {
    link = link->next;
  }

  /* link->next points to null here, but we can point it to the new item */
  link->next = item;

  /* point link to the head of the new item */
  link = item;

  /* terminate the list */
  link->next = NULL;

  list->count++;
}

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
long smem_list_delete( smem_list_type *list, void *item )
{
  smem_list_link_type *link = ( smem_list_link_type * )list;

  /* get next item until link points to item in question */
  while( link->next != item )
  {
    if( NULL == link->next )
    {
      return -1;
    }

    link = link->next;
  }

  link->next = ( ( smem_list_link_type * )item )->next;

  list->count--;

  return 0;
}

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
void *smem_list_first( smem_list_type *list )
{
  /* return the first item in the list */
  return list->next;
}

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
void *smem_list_next( void * item )
{
  /* just return the item that this item points to
   * (base address contains a pointer) */
  return ( ( smem_list_link_type * )item )->next;
}

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
long smem_list_count( smem_list_type *list )
{
  return list->count;
}

