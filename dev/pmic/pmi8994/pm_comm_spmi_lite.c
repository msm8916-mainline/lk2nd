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

INCLUDE FILES FOR MODULE

===========================================================================*/

#include <pm8x41.h>
#include <debug.h>
#include <sys/types.h>
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_comm.h"
#include <spmi.h>

#define PM_MAX_REGS   0xFFFF

typedef uint32_t SpmiBus_ResultType;

enum spmi_result
{
    SPMI_BUS_SUCCESS,
};

pm_err_flag_type pm_comm_read_byte(uint32 slave_id, uint32 addr, uint8* data, uint8 priority)
{
    pm_err_flag_type     err = PM_ERR_FLAG__SUCCESS ;
    SpmiBus_ResultType   spmi_result = SPMI_BUS_SUCCESS;
    
    if(TRUE == spmi_initialized())
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG__SPMI_OPT_ERR ;
        }
        else
        {
            spmi_result = spmi_reg_read(slave_id, addr, data, priority);

            if ( spmi_result != SPMI_BUS_SUCCESS ) { err = PM_ERR_FLAG__SPMI_OPT_ERR ; }
        }
    }
    else
    {
        err = PM_ERR_FLAG__SPMI_OPT_ERR ;
    }

    return err ;
}

pm_err_flag_type pm_comm_read_byte_mask(uint32 slave_id, uint32 addr, uint8 mask, uint8* data, uint8 priority)
{
    pm_err_flag_type     err = PM_ERR_FLAG__SUCCESS ;
   
    err = spmi_reg_read(slave_id, addr, data, priority);
    if ( err != PM_ERR_FLAG__SUCCESS ) 
    { 
        err = PM_ERR_FLAG__SPMI_OPT_ERR ; 
    }
    else 
    { 
        *data = *data & mask; 
    }

    
    return err ;
}



pm_err_flag_type pm_comm_write_byte(uint32 slave_id, uint32 addr, uint8 data, uint8 priority)
{
    pm_err_flag_type     err = PM_ERR_FLAG__SUCCESS ;
    SpmiBus_ResultType   spmi_result = SPMI_BUS_SUCCESS;
    
    if(TRUE == spmi_initialized())
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG__SPMI_OPT_ERR ;
        }
        else
        {
            spmi_result = spmi_reg_write(slave_id, addr, &data, priority);

            if ( spmi_result != SPMI_BUS_SUCCESS ) { err = PM_ERR_FLAG__SPMI_OPT_ERR ; }
        }
    }
    else
    {
        err = PM_ERR_FLAG__SPMI_OPT_ERR ;
    }

    return err ;
}

pm_err_flag_type pm_comm_write_byte_mask(uint32 slave_id, uint32 addr, uint8 mask, uint8 value, uint8 priority)
{
    pm_err_flag_type         err = PM_ERR_FLAG__SUCCESS ;
    SpmiBus_ResultType       spmi_result = SPMI_BUS_SUCCESS;
    uint8                    data = 0 ;

    if(TRUE == spmi_initialized())
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG__SPMI_OPT_ERR ;
        }
        else
        {
             /* Read the data from the SPMI and leave all the bits other than the mask to the previous state */
            spmi_result = spmi_reg_read(slave_id, addr, &data, priority);

            if ( spmi_result == SPMI_BUS_SUCCESS )
            {
                data &= (~mask) ; 
                data |= (value & mask) ;

                spmi_result = spmi_reg_write(slave_id, addr, &data, priority);

                if ( spmi_result != SPMI_BUS_SUCCESS ) { err = PM_ERR_FLAG__SPMI_OPT_ERR ; }
            }
        }
    }
    else
    {
        err = PM_ERR_FLAG__SPMI_OPT_ERR ;
    }


    return err ;
}

#ifndef LK
pm_err_flag_type pm_comm_write_byte_array(uint32 slave_id, uint16 addr, uint32 num_bytes, uint8* data, uint8 priority) 
{
    pm_err_flag_type     err = PM_ERR_FLAG__SUCCESS ;
    SpmiBus_ResultType   spmi_result = SPMI_BUS_SUCCESS;

    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG__SPMI_OPT_ERR ;
        }
        else
        {
            spmi_result = SpmiBus_WriteLong(slave_id, (SpmiBus_AccessPriorityType)priority, addr, data, num_bytes);

            if ( spmi_result != SPMI_BUS_SUCCESS ) { err = PM_ERR_FLAG__SPMI_OPT_ERR ; }
        }
    }
    else
    {
        err = PM_ERR_FLAG__SPMI_OPT_ERR ;
    }

    return err ;
}

pm_err_flag_type pm_comm_read_byte_array (uint32 slave_id, uint16 addr, uint32 num_bytes, uint8* data, uint8 priority) 
{
    pm_err_flag_type     err = PM_ERR_FLAG__SUCCESS ;
    SpmiBus_ResultType   spmi_result = SPMI_BUS_SUCCESS;
    /* Read the data from the SPMI */
    uint32               dataReadLength = 0;

   
    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG__SPMI_OPT_ERR ;
        }
        else
        {
            spmi_result = SpmiBus_ReadLong(slave_id, (SpmiBus_AccessPriorityType)priority, addr, data, num_bytes, &dataReadLength);

            if ( spmi_result != SPMI_BUS_SUCCESS ) { err = PM_ERR_FLAG__SPMI_OPT_ERR ; }
        }
    }
    else
    {
        err = PM_ERR_FLAG__SPMI_OPT_ERR ;
    }

    return err ;
}
#endif
