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

#ifndef PM_COMM_H
#define PM_COMM_H

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_resources_and_types.h"
#include "pm_err_flags.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
 typedef enum CommType
{
    FIRST_INVALID_COMM_INTERFACE = -1,
    SPMI_1_0Type,
    Transcomm_Type,
    SSBI_2_0TypeLite = 0 ,
    SSBI_2_0Type,
    INVALID_COMM_INTERFACE
}CommType;
 
 /** 
   @struct pm_comm_info_type
   @brief  This struct is used to store the PMIC Comm 
           information.
  */
typedef struct
{
    uint32 slave_id;
    uint32 pmic_index;
    CommType mCommType;
}pm_comm_info_type;



/*! \brief This function initializes the comm info structure
 *  \param[in] None
 *
 *  \return None.
 *
 *  <b>Dependencies</b>
 *  \li DAL framework.
 */
void pm_comm_info_init(void);


/*! \brief This function returns the comminfo struct(periph slave id, commtype) 
 *  \param[in] pmic_chip_index - pmic index from the total num of pmics
 *
 *\param[in] slave_id_index - primary / secondary slave 
 *
 *  \return pm_comm_info_type.
 *
 */

pm_comm_info_type* pm_comm_get_comm_info(uint32 pmic_chip_index, uint32 slave_id_index);


pm_err_flag_type  pm_comm_read_byte(uint32 slave_id , 
                                         uint32 addr,
                                         uint8* data, 
                                         uint8 priority);

pm_err_flag_type  pm_comm_read_byte_mask(uint32 slave_id , 
                                         uint32 addr,
                                         uint8 mask,  
                                         uint8* data, 
                                         uint8 priority);

pm_err_flag_type  pm_comm_write_byte(uint32 slave_id , 
                                          uint32 addr,
                                          uint8 data, 
                                          uint8 priority);

pm_err_flag_type  pm_comm_write_byte_mask(uint32 slave_id , 
                                               uint32 addr,
                                               uint8 mask, 
                                               uint8 value, 
                                               uint8 priority);

#ifndef LK
pm_err_flag_type  pm_comm_write_byte_array(uint32 slave_id,
                                                uint16 addr , 
                                                uint32 num_bytes, 
                                                uint8* data, 
                                                uint8 priority) ;

pm_err_flag_type  pm_comm_read_byte_array ( uint32 slave_id ,
                                                uint16 addr , 
                                                uint32 num_bytes, 
                                                uint8* data, 
                                                uint8 priority) ;
pm_err_flag_type pm_comm_channel_init_internal(void);
#endif

#endif /* PM_COMM_H */

