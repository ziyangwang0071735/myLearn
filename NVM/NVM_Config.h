/**
 * @file        NVM_Config.h
 * @brief       Non-Volatile Memory application
 * @author      Rockshaw.Luo
 * @version     V0.0.0
 * @attention
 *  This material is the exclusive property of KUKA Robot Group
 *  Except as expressly permitted by separate agreement, this material may only
 *  be used by members of the development department of KUKA Robot Group for
 *  internal development purposes of KUKA Robot Group.
 *  Copyright (C) 2021
 *  KUKA Robot Group. All Rights Reserved.
 * @htmlonly
 * <span style="font-weight: bold">History</span>
 * @endhtmlonly
 * Version|Author|Date|Describe
 * -------|-------|-------|--------
 * V0.0.0|Rockshaw.Luo |01/22/2024      |Module Initial
 */

#ifndef NVM_CONFIG_H
#define NVM_CONFIG_H

/******* I N C L U D E - F I L E S *******************************************/
#include "syst.h"

/******* G L O B A L - D E F I N E S *****************************************/
#define NVM_SSDCB_EEPROM 0U
#define NVM_SSDB_EEPROM 1U
#define NVM_EEPROM_NUM 2U

/******* G L O B A L - T Y P E S *********************************************/

/******* G L O B A L - D A T A ***********************************************/

/******* G L O B A L - F U N C T I O N S - P R O T O T Y P E S ***************/
int32_t NVM_Write_Eeprom(uint8_t nvm_id, uint16_t offset, uint8_t* buf);

int32_t NVM_Read_Eeprom(uint8_t nvm_id, uint16_t offset, uint8_t* buf);

#endif /* NVM_CONFIG_H */
/******* E N D  (NVM_Config.h)****************************************************/