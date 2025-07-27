/**
 * @file        Hal_Eep.c
 * @brief       Hardware eeprom basic function implementation
 * @author      Ziyang.Wang  
 * @version     V0.0.0
 * @attention
 *  This material is the exclusive property of KUKA Robot Group
 *  Except as expressly permitted by separate agreement, this material may only
 *  be used by members of the development department of KUKA Robot Group for
 *  internal development purposes of KUKA Robot Group.
 *  Copyright (C) 2025
 *  KUKA Robot Group. All Rights Reserved.
 * @htmlonly
 * <span style="font-weight: bold">History</span>
 * @endhtmlonly
 * Version|Author|Date|Describe
 * -------|-------|-------|--------
 * V0.0.0|Ziyang.Wang  |04/27/2025      |Module Initial                
 */

/******* I N C L U D E - F I L E S *******************************************/
#include "Hal_Eep.h"
#include "board.h"

/******* L O C A L - D E F I N E S *******************************************/
#define Hal_Eep_WriteIO(pin, level) GPIO_writePin(pin, level)

/******* L O C A L - T Y P E S ***********************************************/


/******* L O C A L - D A T A *************************************************/
Hal_Eep_t EepI2C0Obj;
Hal_Eep_t EepI2C1Obj;

/******* L O C A L - F U N C T I O N - P R O T O T Y P E S *******************/
/**
 * @brief   Checks I2C error status and updates EEPROM status flags.
 *
 * @param   pEep    Pointer to the EEPROM handle structure.
 * @param   status  I2C operation status code to be checked.
 *
 * @return  uint16_t
 *          - TRUE:  I2C operation succeeded.
 *          - FALSE: I2C operation failed with an error.
 *
 * @note    Maps I2C status codes to EEPROM status bitfield. Handles common
 *          errors: BUS_BUSY, NACK_RECEIVED, ARBITRATION_LOST, RXFIFO_NOT_READY,
 *          and STOP_NOT_READY. Unknown errors are ignored. Returns FALSE for
 *          any error condition except SUCCESS. The EEP_ACCESS_OK flag is set
 *          only for successful operations.
 */
static uint16_t Hal_Eep_CheckI2CError(Hal_Eep_t *pEep, uint16_t status);


/******* L O C A L - F U N C T I O N - D E F I N I T I O N S *****************/
static uint16_t Hal_Eep_CheckI2CError(Hal_Eep_t *pEep, uint16_t status)
{
    ASSERT(pEep);

    switch (status)
    {
        case SUCCESS:
            pEep->Status |= EEP_ACCESS_OK;
            return TRUE;

        case ERROR_BUS_BUSY:
            pEep->Status |= EEP_BUS_BUSY;
            break;

        case ERROR_NACK_RECEIVED:
            pEep->Status |= EEP_NACK_RECEIVED;
            break;

        case ERROR_ARBITRATION_LOST:
            pEep->Status |= EEP_ARBITRATION_LOST;
            break;

        case ERROR_RXFIFO_NOT_READY:
            pEep->Status |= EEP_RXFIFO_NOT_READY;

        case ERROR_STOP_NOT_READY:
            pEep->Status |= EEP_STOP_NOT_READY;
            break;

        default:
            break;
    }

    return FALSE;
}


/******* G L O B A L - F U N C T I O N - D E F I N I T I O N S ***************/
void Hal_Eep_Init(Hal_Eep_t *pEep, uint32_t base)
{
    ASSERT(pEep);

    for(uint16_t i = 0; i < EEP_IIC_BUF_SIZE; i++)
    {
        pEep->DataRd[i] = 0;
        pEep->DataWr[i] = 0;
    }

    pEep->Status                   = EEP_ACCESS_OK;
    pEep->EepHandle.TargetAddr     = EEP_IIC_DEVICE_ADDR;
    pEep->EepHandle.I2Cbase        = base;
    pEep->EepHandle.NumOfAddrBytes = 2;
    pEep->EepHandle.pRegAddr       = &pEep->EepAddr;
    pEep->EepHandle.pTX_MsgBuffer  = &pEep->DataWr[0];
    pEep->EepHandle.pRX_MsgBuffer  = &pEep->DataRd[0];

}


void Hal_Eep_Write(Hal_Eep_t *pEep, uint32_t base)
{
    ASSERT(pEep);

    pEep->EepHandle.TargetAddr     = EEP_IIC_DEVICE_ADDR;
    pEep->EepHandle.I2Cbase        = base;
    pEep->EepHandle.NumOfAddrBytes = 2;
    pEep->EepHandle.pRegAddr       = &pEep->EepAddr;

    uint16_t Status = Hal_I2C_WriteData(&pEep->EepHandle);
    if(Hal_Eep_CheckI2CError(pEep, Status) == FALSE)
    {
        pEep->Status |= EEP_WRITE_ERROR;
    }
}


void Hal_Eep_SendReadCmdWithAddr(Hal_Eep_t *pEep, uint32_t base)
{
    ASSERT(pEep);

    pEep->EepHandle.TargetAddr     = EEP_IIC_DEVICE_ADDR;
    pEep->EepHandle.I2Cbase        = base;
    pEep->EepHandle.NumOfAddrBytes = 2;
    pEep->EepHandle.pRegAddr       = &pEep->EepAddr;

    uint16_t Status = Hal_I2C_SendReadCmdWithAddr(&pEep->EepHandle);
    if(Hal_Eep_CheckI2CError(pEep, Status) == FALSE)
    {
        pEep->Status |= EEP_READ_SEND_CMD_ERROR;
    }
}


void Hal_Eep_SendRestart(Hal_Eep_t *pEep, uint32_t base)
{
    ASSERT(pEep);

    pEep->EepHandle.I2Cbase        = base;
    uint16_t Status = Hal_I2C_SendRestart(&pEep->EepHandle);
    if(Hal_Eep_CheckI2CError(pEep, Status) == FALSE)
    {
        pEep->Status |= EEP_READ_RESTART_ERROR;
    }
}


void Hal_Eep_GetData(Hal_Eep_t *pEep, uint32_t base)
{
    ASSERT(pEep);

    pEep->EepHandle.I2Cbase        = base;
    uint16_t Status = Hal_I2C_GetData(&pEep->EepHandle);
    if(Hal_Eep_CheckI2CError(pEep, Status) == FALSE)
    {
        pEep->Status |=EEP_READ_READ_DATA_ERROR;
    }
}


void Hal_Eep_EnableWP(uint16_t pin)
{
    Hal_Eep_WriteIO(pin, 0);
}


void Hal_Eep_DisableWP(uint16_t pin)
{
    Hal_Eep_WriteIO(pin, 1);
}


/******* E N D ***** (Hal_Eep.c ) *********************************************/