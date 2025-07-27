/**
 * @file NVM_Config.c
 * @brief Non-Volatile Memory application
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

/******* I N C L U D E - F I L E S *******************************************/
#include "NVM.h"
#include "GT24C_Extern.h"
#include "GT25C_Extern.h"

/******* L O C A L - D E F I N E S *******************************************/

/******* L O C A L - T Y P E S ***********************************************/

/******* L O C A L - D A T A *************************************************/

/******* L O C A L - F U N C T I O N S ***************************************/

/******* G L O B A L - F U N C T I O N S *************************************/
int32_t NVM_Write_Eeprom(uint8_t nvm_id, uint16_t offset, uint8_t* buf)
{
    int32_t Status = NVM_SUCCESS;

    switch (nvm_id)
    {
    case NVM_SSDCB_EEPROM:
        Status = Hal_Eep_Write();
        break;
    case NVM_SSDB_EEPROM:
        Status = Hal_Eep_Write();
        break;
    default:
        break;
    }

    return Status;
}

int32_t NVM_Read_Eeprom(uint8_t nvm_id, uint16_t offset, uint8_t* buf)
{
    int32_t Status = NVM_SUCCESS;

    switch (nvm_id)
    {
    case NVM_SSDCB_EEPROM:
        //Status = GT24C_Read(offset, (uint8_t*) buf, MEMORY_BLOCK_SIZE);
        pEep->pEepIntf->EepHandle.NumOfDataBytes = EEP_IIC_BUF_SIZE;
        Hal_Eep_SendReadCmdWithAddr(pEep->pEepIntf, pEep->SpiBase);
        DEVICE_DELAY_US(500);
        Hal_Eep_SendRestart(pEep->pEepIntf, pEep->SpiBase);
        DEVICE_DELAY_US(500);
        Hal_Eep_GetData(pEep->pEepIntf, pEep->SpiBase);

        DEVICE_DELAY_US(6000);
        break;
    case NVM_SSDB_EEPROM:
        //Status = GT25C_Read(&GT25C_Obj[GT25C_EEPROM_SSDB], offset, (uint8_t*) buf, MEMORY_BLOCK_SIZE);
        pEep->pEepIntf->EepHandle.NumOfDataBytes = EEP_IIC_BUF_SIZE;
        Hal_Eep_SendReadCmdWithAddr(pEep->pEepIntf, pEep->SpiBase);
        DEVICE_DELAY_US(500);
        Hal_Eep_SendRestart(pEep->pEepIntf, pEep->SpiBase);
        DEVICE_DELAY_US(500);
        Hal_Eep_GetData(pEep->pEepIntf, pEep->SpiBase);

        DEVICE_DELAY_US(6000);
        break;
    default:
        break;
    }

    return Status;
}

/******* E N D ***** (NVM_Config.c) *********************************************/