/**
 * @file NVM.c
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
#include "CRC32.h"
#include "FLTMON_MOTOR_Extern.h"
#include "AxisGlobal.h"
#include "SupplierGlobal.h"
#include "xil_mem.h"
#include "FCT_Extern.h"

/******* L O C A L - D E F I N E S *******************************************/
#define EEPROM_BLOCK_VERSION 0x0001

#define NVM_WRITE_ERR_MAX_CNT 2U

#define NVM_SAVE_BLOCK_INTERVAL_TIME 1800U // 30 minutes

/******* L O C A L - T Y P E S ***********************************************/
static UserMemoryBlock_t MemoryBlockBuffer;
static uint16_t NVM_OrignalMemoryBlockNum[NVM_EEPROM_NUM] = {SSDCB_EEPROM_BLOCK_NUM / 2, SSDB_EEPROM_BLOCK_NUM / 2, EADB_EEPROM_BLOCK_NUM / 2};

static uint8_t DefaultProductionData[NVM_EEPROM_NUM][MEMORY_BLOCK_SIZE] = {
    {
        0x4B, 0x55, 0x4B, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xFF, 0xFF, // 0~15
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x32, 0x36, // 16~31
        0x30, 0x30, 0x34, 0x33, 0x37, 0x31, 0x34, 0x32, 0x30, 0x31, 0x32, 0x36, 0x36, 0x30, 0x31, 0x30, // 32~47
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x38, 0x34, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 48~63
        0xFF, 0xFF, 0x00, 0x60, 0xC8, 0xFE, 0x3C, 0xB0, 0x00, 0x60, 0xC8, 0xFE, 0x3C, 0xB1, 0x00, 0x60, // 64~79
        0xC8, 0xFE, 0x3C, 0xB2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 80~95
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 96~111
        0xFF, 0xFF, 0x30, 0x31, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 112~127
    },
    {
        0x4B, 0x55, 0x4B, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0xFF, 0xFF, // 0~15
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x32, 0x36, // 16~31
        0x30, 0x30, 0x34, 0x33, 0x37, 0x31, 0x34, 0x32, 0x30, 0x31, 0x32, 0x36, 0x36, 0x30, 0x31, 0x30, // 32~47
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x38, 0x34, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 48~63
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 64~79
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 80~95
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 96~111
        0xFF, 0xFF, 0x30, 0x31, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 112~127
    },
    {
        0x4B, 0x55, 0x4B, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x35, 0x30, 0x30, 0x30, 0x30, 0xFF, 0xFF, // 0~15
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x32, 0x36, // 16~31
        0x30, 0x30, 0x34, 0x33, 0x37, 0x31, 0x34, 0x32, 0x30, 0x31, 0x32, 0x36, 0x36, 0x30, 0x31, 0x30, // 32~47
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x38, 0x34, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 48~63
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 64~79
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 80~95
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 96~111
        0xFF, 0xFF, 0x30, 0x31, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 112~127
    }};

/******* L O C A L - D A T A *************************************************/
static UserMemoryBlock_t SSDCB_OriginalMemoryBlock[SSDCB_EEPROM_BLOCK_NUM / 2];
static UserMemoryBlock_t SSDB_OriginalMemoryBlock[SSDB_EEPROM_BLOCK_NUM / 2];

static UserMemoryBlock_t* pOriginalMemoryBlock[NVM_EEPROM_NUM] = {&SSDCB_OriginalMemoryBlock[0], &SSDB_OriginalMemoryBlock[0], &EADB_OriginalMemoryBlock[0]};
static uint16_t MemoryBlockInUseNum[NVM_EEPROM_NUM] = {NVM_SSDCB_BLOCK_MEMORY_INUSE_MAX, NVM_SSDB_BLOCK_MEMORY_INUSE_MAX, NVM_EADB_BLOCK_MEMORY_INUSE_MAX};

/******* L O C A L - F U N C T I O N S ***************************************/
static int32_t NVM_ReadRemoteMemorytoLocal(uint8_t nvm_id,uint16_t block_id);
static int32_t NVM_WriteRemoteMemoryfromLocal(uint8_t nvm_id, uint16_t block_id);
static void NVM_ReadLocalMemorytoApplication(uint8_t nvm_id, uint16_t block_id);
static void NVM_WriteLocalMemoryfromApplication(uint8_t nvm_id, uint16_t block_id);
static void NVM_WriteSSDCBDatafromApplication(uint16_t block_id);
static void NVM_WriteSSDBDatafromApplication(uint16_t block_id);
static void NVM_WriteEADBDatafromApplication(uint16_t block_id);

/******* G L O B A L - F U N C T I O N S *************************************/
extern void FLTMON_EepromWarning(SUPPLIER_T* pSupplier, uint8_t eepromId);

/**
 * @brief initial NVM module
 *
 */
void NVM_Init(void)
{
    uint8_t nvm_num = 0;
    int32_t Status = NVM_SUCCESS;

    for (uint8_t nvm_id = 0; nvm_id < nvm_num; nvm_id++)
    {
        uint8_t block_end_id = MemoryBlockInUseNum[nvm_id];
        for (uint8_t block_id = 0; block_id < block_end_id; block_id++)
        {
            // Read Origin
            Status = NVM_ReadBlockData(nvm_id, block_id);
            /* TODO Add EEPROM MONITOR */
            // if (Status != NVM_SUCCESS)
            // {
            //     FLTMON_EepromWarning(&Supplier, nvm_id);
            // }
        }
    }

    return;
}

/**
 * @brief Read NVM data to local memory
 *
 */
static int32_t NVM_ReadRemoteMemorytoLocal(uint8_t nvm_id, uint16_t block_id)
{
    ASSERT(nvm_id < NVM_EEPROM_NUM);
    ASSERT(block_id < NVM_OrignalMemoryBlockNum[nvm_id]);

    int32_t Status = NVM_SUCCESS;
    uint16_t main_addr = 0;
    uint16_t backup_addr = 0;
    uint32_t CRC32 = 0;
    UserMemoryBlock_t* pMBBuf = (UserMemoryBlock_t*) &MemoryBlockBuffer;
    UserMemoryBlock_t* pMBEep = NULL;

    main_addr = block_id * MEMORY_BLOCK_SIZE;
    pMBEep = (UserMemoryBlock_t*) pOriginalMemoryBlock[nvm_id] + block_id;
    Status = NVM_Read_Eeprom(nvm_id, main_addr, (uint8_t*) pMBBuf);
    if (Status != NVM_SUCCESS)
    {
        return NVM_FAILURE;
    }

    CRC32 = CRC32_Calc((uint8_t*) pMBBuf, MEMORY_BLOCK_SIZE - 4);
    if (CRC32 == pMBBuf->CRC32)
    {
        memcpy((uint8_t*) pMBEep, (uint8_t*) pMBBuf, sizeof(UserMemoryBlock_t));
        Status = NVM_SUCCESS;

        backup_addr = (block_id + NVM_OrignalMemoryBlockNum[nvm_id]) * MEMORY_BLOCK_SIZE;
        Status = NVM_Read_Eeprom(nvm_id, backup_addr, (uint8_t*) pMBBuf);
        if (Status == NVM_SUCCESS)
        {
            CRC32 = CRC32_Calc((uint8_t*) pMBBuf, MEMORY_BLOCK_SIZE - 4);
            if (CRC32 != pMBBuf->CRC32)
            {
                NVM_Write_Eeprom(nvm_id, backup_addr, (uint8_t*) pMBEep);
            }
        }
    }
    else
    {
        backup_addr = (block_id + NVM_OrignalMemoryBlockNum[nvm_id]) * MEMORY_BLOCK_SIZE;
        Status = NVM_Read_Eeprom(nvm_id, backup_addr, (uint8_t*) pMBBuf);
        if (Status != NVM_SUCCESS)
        {
            return NVM_FAILURE;
        }
        CRC32 = CRC32_Calc((uint8_t*) pMBBuf, MEMORY_BLOCK_SIZE - 4);
        if (CRC32 == pMBBuf->CRC32)
        {
            memcpy((uint8_t*) pMBEep, (uint8_t*) pMBBuf, sizeof(UserMemoryBlock_t));
            Status = NVM_SUCCESS;

            NVM_Write_Eeprom(nvm_id, main_addr, (uint8_t*) pMBEep);
        }
        else
        {
            Status = NVM_FAILURE;
        }
    }

    return Status;
}


static void NVM_ReadSSDCBDatatoApplication(uint16_t block_id)
{
    Xil_AssertVoid(block_id < NVM_OrignalMemoryBlockNum[NVM_SSDCB_EEPROM]);

    UserMemoryBlock_t* pMBEep = NULL;
    uint8_t* pPayload = NULL;
    uint8_t AxisId = 0;

    pMBEep = (UserMemoryBlock_t*) pOriginalMemoryBlock[NVM_SSDCB_EEPROM] + block_id;
    pPayload = (uint8_t*) &pMBEep->Payload[0];
    /* TODO update based on New OBJ */
    switch (block_id)
    {
    case 0:
        // KUKA information
        memcpy(&ModuleInformationSSDCB0x2FD0.KUKA[0], &((ProductionData_t*) pMBEep)->KUKA_Info.KUKA[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.KUKA));
        memcpy(&ModuleInformationSSDCB0x2FD0.DataLayoutVersion[0], &((ProductionData_t*) pMBEep)->KUKA_Info.DataLayoutVersion[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.DataLayoutVersion));
        memcpy(&ModuleInformationSSDCB0x2FD0.BoardVariant[0], &((ProductionData_t*) pMBEep)->KUKA_Info.BoardVariant[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.BoardVariant));
        memcpy(&ModuleInformationSSDCB0x2FD0.HWVersion[0], &((ProductionData_t*) pMBEep)->KUKA_Info.HWVersion[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.HWVersion));
        // SN Board ID
        memcpy(&ModuleInformationSSDCB0x2FD0.BarCode[0], &((ProductionData_t*) pMBEep)->SN_BoardID.BarCode[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.BarCode));
        memcpy(&ModuleInformationSSDCB0x2FD0.MaterialNumber[0], &((ProductionData_t*) pMBEep)->SN_BoardID.MaterialNumber[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.MaterialNumber));
        memcpy(&ModuleInformationSSDCB0x2FD0.Res[0], &((ProductionData_t*) pMBEep)->SN_BoardID.Res[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.Res));
        memcpy(&ModuleInformationSSDCB0x2FD0.SupplierCode[0], &((ProductionData_t*) pMBEep)->SN_BoardID.SupplierCode[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.SupplierCode));
        memcpy(&ModuleInformationSSDCB0x2FD0.Revision[0], &((ProductionData_t*) pMBEep)->SN_BoardID.Revision[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.Revision));
        memcpy(&ModuleInformationSSDCB0x2FD0.SeriesNumber[0], &((ProductionData_t*) pMBEep)->SN_BoardID.SeriesNumber[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.SeriesNumber));
        memcpy(&ModuleInformationSSDCB0x2FD0.CheckDigit[0], &((ProductionData_t*) pMBEep)->SN_BoardID.CheckDigit[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.CheckDigit));
        // MAC Address
        memcpy(&ModuleInformationSSDCB0x2FD0.XF1KSI[0], &((ProductionData_t*) pMBEep)->MAC_Adrress.XF1KSI[0], sizeof(((ProductionData_t*) pMBEep)->MAC_Adrress.XF1KSI));
        memcpy(&ModuleInformationSSDCB0x2FD0.XF2KONI[0], &((ProductionData_t*) pMBEep)->MAC_Adrress.XF2KONI[0], sizeof(((ProductionData_t*) pMBEep)->MAC_Adrress.XF2KONI));
        memcpy(&ModuleInformationSSDCB0x2FD0.XF4KLIOT[0], &((ProductionData_t*) pMBEep)->MAC_Adrress.XF4KLIOT[0], sizeof(((ProductionData_t*) pMBEep)->MAC_Adrress.XF4KLIOT));
        memcpy(&ModuleInformationSSDCB0x2FD0.PROFINET1[0], &((ProductionData_t*) pMBEep)->MAC_Adrress.PROFINET1[0], sizeof(((ProductionData_t*) pMBEep)->MAC_Adrress.PROFINET1));
        memcpy(&ModuleInformationSSDCB0x2FD0.PROFINET2[0], &((ProductionData_t*) pMBEep)->MAC_Adrress.PROFINET2[0], sizeof(((ProductionData_t*) pMBEep)->MAC_Adrress.PROFINET2));
        // Tester ID
        memcpy(&ModuleInformationSSDCB0x2FD0.TesterID[0], &((ProductionData_t*) pMBEep)->TesterID.TesterID[0], sizeof(((ProductionData_t*) pMBEep)->TesterID.TesterID));
        // All Data Checksum
        memcpy(&ModuleInformationSSDCB0x2FD0.CRC32, &((ProductionData_t*) pMBEep)->AllCheckSum.CRC32, sizeof(((ProductionData_t*) pMBEep)->AllCheckSum.CRC32));
        break;
    case 1:
        OperatingTimeCounter0x2006.SSDCBAccumulatedRunningTime = ((SSDCB_RT_Payload_t*) pPayload)->SSDCBAccumulatedRunningTime;
        break;
    case 10:
        memcpy(&SupplierErrorMessagesHistory0x5027, &((SSDCB_SEMH_Payload_t*) pPayload)->SupplierErrorMessagesHistory, sizeof(TOBJ5027));
        break;
    default: // 2~9
        AxisId = block_id - 2;
        Xil_MemCpy((TOBJ3027*) &KspConvGroupObjects[AxisId].InverterErrorMessagesHistory, &((SSDCB_IEMH_Payload_t*) pPayload)->InverterErrorMessagesHistory, sizeof(TOBJ3027));
        break;
    }
}

static void NVM_ReadSSDBDatatoApplication(uint16_t block_id)
{
    Xil_AssertVoid(block_id < NVM_OrignalMemoryBlockNum[NVM_SSDB_EEPROM]);

    UserMemoryBlock_t* pMBEep = NULL;
    uint8_t* pPayload = NULL;

    pMBEep = (UserMemoryBlock_t*) pOriginalMemoryBlock[NVM_SSDB_EEPROM] + block_id;
    pPayload = (uint8_t*) &pMBEep->Payload[0];

    switch (block_id)
    {
    case 0:
        // KUKA information
        memcpy(&ModuleInformationSSDB0x2FD1.KUKA[0], &((ProductionData_t*) pMBEep)->KUKA_Info.KUKA[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.KUKA));
        memcpy(&ModuleInformationSSDB0x2FD1.DataLayoutVersion[0], &((ProductionData_t*) pMBEep)->KUKA_Info.DataLayoutVersion[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.DataLayoutVersion));
        memcpy(&ModuleInformationSSDB0x2FD1.BoardVariant[0], &((ProductionData_t*) pMBEep)->KUKA_Info.BoardVariant[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.BoardVariant));
        memcpy(&ModuleInformationSSDB0x2FD1.HWVersion[0], &((ProductionData_t*) pMBEep)->KUKA_Info.HWVersion[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.HWVersion));
        // SN Board ID
        memcpy(&ModuleInformationSSDB0x2FD1.BarCode[0], &((ProductionData_t*) pMBEep)->SN_BoardID.BarCode[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.BarCode));
        memcpy(&ModuleInformationSSDB0x2FD1.MaterialNumber[0], &((ProductionData_t*) pMBEep)->SN_BoardID.MaterialNumber[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.MaterialNumber));
        memcpy(&ModuleInformationSSDB0x2FD1.Res[0], &((ProductionData_t*) pMBEep)->SN_BoardID.Res[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.Res));
        memcpy(&ModuleInformationSSDB0x2FD1.SupplierCode[0], &((ProductionData_t*) pMBEep)->SN_BoardID.SupplierCode[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.SupplierCode));
        memcpy(&ModuleInformationSSDB0x2FD1.Revision[0], &((ProductionData_t*) pMBEep)->SN_BoardID.Revision[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.Revision));
        memcpy(&ModuleInformationSSDB0x2FD1.SeriesNumber[0], &((ProductionData_t*) pMBEep)->SN_BoardID.SeriesNumber[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.SeriesNumber));
        memcpy(&ModuleInformationSSDB0x2FD1.CheckDigit[0], &((ProductionData_t*) pMBEep)->SN_BoardID.CheckDigit[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.CheckDigit));
        //Tester ID
        memcpy(&ModuleInformationSSDB0x2FD1.TesterID[0], &((ProductionData_t*) pMBEep)->TesterID.TesterID[0], sizeof(((ProductionData_t*) pMBEep)->TesterID.TesterID));
        // All Data Checksum
        memcpy(&ModuleInformationSSDB0x2FD1.CRC32, &((ProductionData_t*) pMBEep)->AllCheckSum.CRC32, sizeof(((ProductionData_t*) pMBEep)->AllCheckSum.CRC32));
        break;
    case 1:
        pPayload = (uint8_t*) &pMBEep->Payload[0];
        OperatingTimeCounter0x2006.SSDBAccumulatedRunningTime = ((SSDB_RT_Payload_t*) pPayload)->SSDBAccumulatedRunningTime;

        KspConvGroupObjects[0].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime = ((SSDB_RT_Payload_t*) pPayload)->Axis0_IPM_RunTime;
        KspConvGroupObjects[1].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime = ((SSDB_RT_Payload_t*) pPayload)->Axis1_IPM_RunTime;
        KspConvGroupObjects[2].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime = ((SSDB_RT_Payload_t*) pPayload)->Axis2_IPM_RunTime;
        KspConvGroupObjects[3].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime = ((SSDB_RT_Payload_t*) pPayload)->Axis3_IPM_RunTime;
        KspConvGroupObjects[4].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime = ((SSDB_RT_Payload_t*) pPayload)->Axis4_IPM_RunTime;
        KspConvGroupObjects[5].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime = ((SSDB_RT_Payload_t*) pPayload)->Axis5_IPM_RunTime;
        break;
    default:
        break;
    }
}

static void NVM_ReadEADBDatatoApplication(uint16_t block_id)
{
    Xil_AssertVoid(block_id < NVM_OrignalMemoryBlockNum[NVM_EADB_EEPROM]);

    UserMemoryBlock_t* pMBEep = NULL;
    uint8_t* pPayload = NULL;

    pMBEep = (UserMemoryBlock_t*) pOriginalMemoryBlock[NVM_EADB_EEPROM] + block_id;
    pPayload = (uint8_t*) &pMBEep->Payload[0];

    switch (block_id)
    {
    case 0:
        // KUKA information
        memcpy(&ModuleInformationEADB0x2FD2.KUKA[0], &((ProductionData_t*) pMBEep)->KUKA_Info.KUKA[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.KUKA));
        memcpy(&ModuleInformationEADB0x2FD2.DataLayoutVersion[0], &((ProductionData_t*) pMBEep)->KUKA_Info.DataLayoutVersion[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.DataLayoutVersion));
        memcpy(&ModuleInformationEADB0x2FD2.BoardVariant[0], &((ProductionData_t*) pMBEep)->KUKA_Info.BoardVariant[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.BoardVariant));
        memcpy(&ModuleInformationEADB0x2FD2.HWVersion[0], &((ProductionData_t*) pMBEep)->KUKA_Info.HWVersion[0], sizeof(((ProductionData_t*) pMBEep)->KUKA_Info.HWVersion));
        // SN Board ID
        memcpy(&ModuleInformationEADB0x2FD2.BarCode[0], &((ProductionData_t*) pMBEep)->SN_BoardID.BarCode[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.BarCode));
        memcpy(&ModuleInformationEADB0x2FD2.MaterialNumber[0], &((ProductionData_t*) pMBEep)->SN_BoardID.MaterialNumber[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.MaterialNumber));
        memcpy(&ModuleInformationEADB0x2FD2.Res[0], &((ProductionData_t*) pMBEep)->SN_BoardID.Res[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.Res));
        memcpy(&ModuleInformationEADB0x2FD2.SupplierCode[0], &((ProductionData_t*) pMBEep)->SN_BoardID.SupplierCode[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.SupplierCode));
        memcpy(&ModuleInformationEADB0x2FD2.Revision[0], &((ProductionData_t*) pMBEep)->SN_BoardID.Revision[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.Revision));
        memcpy(&ModuleInformationEADB0x2FD2.SeriesNumber[0], &((ProductionData_t*) pMBEep)->SN_BoardID.SeriesNumber[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.SeriesNumber));
        memcpy(&ModuleInformationEADB0x2FD2.CheckDigit[0], &((ProductionData_t*) pMBEep)->SN_BoardID.CheckDigit[0], sizeof(((ProductionData_t*) pMBEep)->SN_BoardID.CheckDigit));
        // Tester ID
        memcpy(&ModuleInformationEADB0x2FD2.TesterID[0], &((ProductionData_t*) pMBEep)->TesterID.TesterID[0], sizeof(((ProductionData_t*) pMBEep)->TesterID.TesterID));
        // All Data Checksum
        memcpy(&ModuleInformationEADB0x2FD2.CRC32, &((ProductionData_t*) pMBEep)->AllCheckSum.CRC32, sizeof(((ProductionData_t*) pMBEep)->AllCheckSum.CRC32));
        break;
    case 1:
        pPayload = (uint8_t*) &pMBEep->Payload[0];
        KspConvGroupObjects[6].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime = ((EADB_RT_Payload_t*) pPayload)->Axis6_IPM_Runtime;
        KspConvGroupObjects[7].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime = ((EADB_RT_Payload_t*) pPayload)->Axis7_IPM_Runtime;
        break;
    default:
        break;
    }

    return;
}/**
 * @brief Read local memory which loaded from eeprom when initialization
 *
 */
static void NVM_ReadLocalMemorytoApplication(uint8_t nvm_id, uint16_t block_id)
{
    Xil_AssertVoid(nvm_id < NVM_EEPROM_NUM);
    Xil_AssertVoid(block_id < NVM_OrignalMemoryBlockNum[nvm_id]);

    switch (nvm_id)
    {
    case NVM_SSDCB_EEPROM:
        NVM_ReadSSDCBDatatoApplication(block_id);
        break;
    case NVM_SSDB_EEPROM:
        NVM_ReadSSDBDatatoApplication(block_id);
        break;
    case NVM_EADB_EEPROM:
        NVM_ReadEADBDatatoApplication(block_id);
    }

    return;
}


static void NVM_WriteSSDCBDatafromApplication(uint16_t block_id)
{
    Xil_AssertVoid(block_id < NVM_OrignalMemoryBlockNum[NVM_SSDCB_EEPROM]);

    UserMemoryBlock_t* pMBEep = NULL;
    uint8_t* pPayload = NULL;
    uint8_t AxisId = 0;

    pMBEep = (UserMemoryBlock_t*) pOriginalMemoryBlock[NVM_SSDCB_EEPROM] + block_id;
    pPayload = (uint8_t*) &pMBEep->Payload[0];

    switch (block_id)
    {
    case 0:
        memcpy(pMBEep, &DefaultProductionData[NVM_SSDCB_EEPROM], sizeof(UserMemoryBlock_t));
        break;
    case 1:
        pMBEep->BlockId = block_id;
        pMBEep->BlockVersion = EEPROM_BLOCK_VERSION;
        ((SSDCB_RT_Payload_t*) pPayload)->SSDCBAccumulatedRunningTime = OperatingTimeCounter0x2006.SSDCBAccumulatedRunningTime;
        break;
    case 10:
        pMBEep->BlockId = block_id;
        pMBEep->BlockVersion = EEPROM_BLOCK_VERSION;
        memcpy(&((SSDCB_SEMH_Payload_t*) pPayload)->SupplierErrorMessagesHistory, &SupplierErrorMessagesHistory0x5027, sizeof(TOBJ5027));
        break;
    default: // 2~9
        pMBEep->BlockId = block_id;
        pMBEep->BlockVersion = EEPROM_BLOCK_VERSION;
        AxisId = block_id - 2;
        Xil_MemCpy(&((SSDCB_IEMH_Payload_t*) pPayload)->InverterErrorMessagesHistory, (TOBJ3027*) &KspConvGroupObjects[AxisId].InverterErrorMessagesHistory, sizeof(TOBJ3027));
        break;
    }

    return;
}

static void NVM_WriteSSDBDatafromApplication(uint16_t block_id)
{
    Xil_AssertVoid(block_id < NVM_OrignalMemoryBlockNum[NVM_SSDB_EEPROM]);

    UserMemoryBlock_t* pMBEep = NULL;
    uint8_t* pPayload = NULL;

    pMBEep = (UserMemoryBlock_t*) pOriginalMemoryBlock[NVM_SSDB_EEPROM] + block_id;
    pPayload = (uint8_t*) &pMBEep->Payload[0];

    switch (block_id)
    {
    case 0:
        memcpy(pMBEep, &DefaultProductionData[NVM_SSDB_EEPROM], sizeof(UserMemoryBlock_t));
        break;
    case 1:
        pMBEep->BlockId = block_id;
        pMBEep->BlockVersion = EEPROM_BLOCK_VERSION;
        ((SSDB_RT_Payload_t*) pPayload)->SSDBAccumulatedRunningTime = OperatingTimeCounter0x2006.SSDBAccumulatedRunningTime;
        ((SSDB_RT_Payload_t*) pPayload)->Axis0_IPM_RunTime = KspConvGroupObjects[0].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime;
        ((SSDB_RT_Payload_t*) pPayload)->Axis1_IPM_RunTime = KspConvGroupObjects[1].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime;
        ((SSDB_RT_Payload_t*) pPayload)->Axis2_IPM_RunTime = KspConvGroupObjects[2].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime;
        ((SSDB_RT_Payload_t*) pPayload)->Axis3_IPM_RunTime = KspConvGroupObjects[3].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime;
        ((SSDB_RT_Payload_t*) pPayload)->Axis4_IPM_RunTime = KspConvGroupObjects[4].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime;
       // ((SSDB_RT_Payload_t*) pPayload)->Axis5_IPM_RunTime = KspConvGroupObjects[5].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime;
        break;
    default:
        break;
    }

    return;
}

static void NVM_WriteEADBDatafromApplication(uint16_t block_id)
{
    Xil_AssertVoid(block_id < NVM_OrignalMemoryBlockNum[NVM_EADB_EEPROM]);

    UserMemoryBlock_t* pMBEep = NULL;
    uint8_t* pPayload = NULL;

    pMBEep = (UserMemoryBlock_t*) pOriginalMemoryBlock[NVM_EADB_EEPROM] + block_id;
    pPayload = (uint8_t*) &pMBEep->Payload[0];

    switch (block_id)
    {
    case 0:
        memcpy(pMBEep, &DefaultProductionData[NVM_EADB_EEPROM], sizeof(UserMemoryBlock_t));
        break;
    case 1:
        pMBEep->BlockId = block_id;
        pMBEep->BlockVersion = EEPROM_BLOCK_VERSION;
        ((EADB_RT_Payload_t*) pPayload)->Axis6_IPM_Runtime = KspConvGroupObjects[6].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime;
        ((EADB_RT_Payload_t*) pPayload)->Axis7_IPM_Runtime = KspConvGroupObjects[7].OperatingTimeCounterOfInverter.InverterAccumulatedRunningTime;
        break;
    default:
        break;
    }

    return;
}


/**
 * @brief Write local memory cyclically in the background loop
 *
 */
static void NVM_WriteLocalMemoryfromApplication(uint8_t nvm_id, uint16_t block_id)
{
    Xil_AssertVoid(nvm_id < NVM_EEPROM_NUM);
    Xil_AssertVoid(block_id < NVM_OrignalMemoryBlockNum[nvm_id]);

    switch (nvm_id)
    {
    case NVM_SSDCB_EEPROM:
        NVM_WriteSSDCBDatafromApplication(block_id);
        break;
    case NVM_SSDB_EEPROM:
        NVM_WriteSSDBDatafromApplication(block_id);break;
        break;
    default:
        break;
    }

    return;
}

/**
 * @brief Write NVM data from local memory when detecting AC loss
 * TODO: have to verify EEPREM Read time
 */
static int32_t NVM_WriteRemoteMemoryfromLocal(uint8_t nvm_id, uint16_t block_id)
{
    ASSERT(nvm_id < NVM_EEPROM_NUM);
    ASSERT(block_id < NVM_OrignalMemoryBlockNum[nvm_id]);

    int32_t Status = NVM_SUCCESS;
    uint16_t main_addr = 0;
    uint16_t backup_addr = 0;
    UserMemoryBlock_t* pMBEep = NULL;
    UserMemoryBlock_t* pMBBuf = (UserMemoryBlock_t*) &MemoryBlockBuffer;
    uint8_t WriteMaxCnt = NVM_WRITE_ERR_MAX_CNT;
    uint8_t WriteSuccess = TRUE;

    pMBEep = (UserMemoryBlock_t*) pOriginalMemoryBlock[nvm_id] + block_id;
    pMBEep->CRC32 = CRC32_Calc((uint8_t*) pMBEep, MEMORY_BLOCK_SIZE - 4);
    // origin
    while (WriteMaxCnt--)
    {
        main_addr = block_id * MEMORY_BLOCK_SIZE;
        Status = NVM_Write_Eeprom(nvm_id, main_addr, (uint8_t*) pMBEep);
        if (Status != NVM_SUCCESS)
        {
            WriteSuccess = FALSE;
            continue;
        }
        else
        {
            WriteSuccess = TRUE;
        }

        Status = NVM_Read_Eeprom(nvm_id, main_addr, (uint8_t*) pMBBuf);
        if (Status != NVM_SUCCESS)
        {
            WriteSuccess = FALSE;
            continue;
        }
        else
        {
            if (!memcmp(pMBBuf, pMBEep, MEMORY_BLOCK_SIZE))
            {
                WriteSuccess = TRUE;
                break;
            }
            else
            {
                WriteSuccess = FALSE;
                continue;
            }
        }
    }

    if (WriteSuccess == TRUE)
    {
        // backup
        backup_addr = (block_id + (SSDCB_EEPROM_BLOCK_NUM / 2)) * MEMORY_BLOCK_SIZE;
        NVM_Write_Eeprom(nvm_id, backup_addr, (uint8_t*) pMBEep);

        Status = NVM_SUCCESS;
    }
    else
    {
        Status = NVM_FAILURE;
    }

    return Status;
}


/**
 * @brief Read NVM block data
 *
 */
int32_t NVM_ReadBlockData(uint8_t nvm_id, uint16_t block_id)
{
    int32_t Status = NVM_SUCCESS;

    Status = NVM_ReadRemoteMemorytoLocal(nvm_id, block_id);
    if (Status == NVM_SUCCESS)
    {
        NVM_ReadLocalMemorytoApplication(nvm_id, block_id);
    }

    return Status;
}

/**
 * @brief Save NVM block data
 *
 */
int32_t NVM_WriteBlockData(uint8_t nvm_id, uint16_t block_id)
{
    int32_t Status = NVM_SUCCESS;

    NVM_WriteLocalMemoryfromApplication(nvm_id, block_id);
    Status = NVM_WriteRemoteMemoryfromLocal(nvm_id, block_id);
    if (Status != NVM_SUCCESS)
    {
        FLTMON_EepromWarning(&Supplier, nvm_id);
    }

    return NVM_SUCCESS;
}

/**
 * @brief Save NVM data
 *
 */
int32_t NVM_SaveEepromData(void)
{
    uint8_t nvm_num = 0;
    int32_t Status = NVM_SUCCESS;
    uint8_t block_start_id = 1;

    //TODO check Get_FCT_WriteProductionData_Cmd
    if (Get_FCT_WriteProductionData_Cmd())
    {
        block_start_id = 0;
    }
    else
    {
        block_start_id = 1;
    }

    for (uint8_t nvm_id = 0; nvm_id < nvm_num; nvm_id++)
    {
        uint8_t block_end_id = MemoryBlockInUseNum[nvm_id];
        for (uint8_t block_id = block_start_id; block_id < block_end_id; block_id++)
        {
            Status = NVM_WriteBlockData(nvm_id, block_id);
        }
    }

    return Status;
}

static void NVM_SaveErrHist(void)
{
    for (uint8_t AxisId = 0; AxisId < ALL_AXIS; AxisId++)
    {
        if (MotorAxis[AxisId].ErrHistSaveEepFlag)
        {
            NVM_WriteBlockData(NVM_SSDCB_EEPROM, NVM_SSDCB_BLOCK_AXIS0_ERROR_HISTORY + AxisId);
            MotorAxis[AxisId].ErrHistSaveEepFlag = FALSE;
            break; // wait next loop to save other axis error history
        }
    }

    if (Supplier.ErrHistSaveEepFlag)
    {
        NVM_WriteBlockData(NVM_SSDCB_EEPROM, NVM_SSDCB_BLOCK_SUPPLIER_ERROR_HISTORY);
        Supplier.ErrHistSaveEepFlag = FALSE;
    }

    return;
}

static void NVM_SaveRunTime(void)
{
    // Save the device operating time at every 30 minutes
    if ((OperatingTimeCounter0x2006.SSDCBPoweredOnRunningTime + 3) % NVM_SAVE_BLOCK_INTERVAL_TIME == 0)
    {
        static uint32_t NVM_SSDCB_SaveRunTime = 0;
        if (NVM_SSDCB_SaveRunTime != OperatingTimeCounter0x2006.SSDCBPoweredOnRunningTime)
        {
            NVM_WriteBlockData(NVM_SSDCB_EEPROM, NVM_SSDCB_BLOCK_DEVICE_RUNTIME);
            NVM_SSDCB_SaveRunTime = OperatingTimeCounter0x2006.SSDCBPoweredOnRunningTime;
        }
    }

    // Save the based axis inverter operating time at every 30 minutes
    if ((KspConvGroupObjects[0].OperatingTimeCounterOfInverter.InverterPoweredOnRunningTime + 2) % NVM_SAVE_BLOCK_INTERVAL_TIME == 0)
    {
        static uint32_t NVM_SSDB_SaveRunTime = 0;

        if (NVM_SSDB_SaveRunTime != KspConvGroupObjects[0].OperatingTimeCounterOfInverter.InverterPoweredOnRunningTime)
        {NVM_WriteBlockData(NVM_SSDB_EEPROM, NVM_SSDB_BLOCK_INVERTER_RUNTIME);
            NVM_SSDB_SaveRunTime = KspConvGroupObjects[0].OperatingTimeCounterOfInverter.InverterPoweredOnRunningTime;
        }
    }

    // Save the extended axis inverter operating time at every 30 minutes
    if ((KspConvGroupObjects[6].OperatingTimeCounterOfInverter.InverterPoweredOnRunningTime + 1) % NVM_SAVE_BLOCK_INTERVAL_TIME == 0)
    {
        static uint32_t NVM_EADB_SaveRunTime = 0;

        if (NVM_EADB_SaveRunTime != KspConvGroupObjects[6].OperatingTimeCounterOfInverter.InverterPoweredOnRunningTime)
        {
            NVM_WriteBlockData(NVM_EADB_EEPROM, NVM_SSDB_BLOCK_INVERTER_RUNTIME);

            NVM_EADB_SaveRunTime = KspConvGroupObjects[6].OperatingTimeCounterOfInverter.InverterPoweredOnRunningTime;
        }
    }

    return;
}


void NVM_SaveUpdate(void)
{
    static uint8_t NVM_ACLoss_SaveDataFlag = TRUE;
    static uint32_t NVM_ACLoss_SaveDataTime = 0;

    // EEPROM data can only be saved after self checking is completed
    if ((SelDiag.Status == SELFDIAG_FINISH)
        || (FLTMON_IsADCPowerFault() == TRUE)) // for AC no power , the selfdiag will be not finished. But the EEPROM need to be saved when only 24V power on
    {
        if ((Supplier.WarnCfgData.warningFlags.bits.ACLoss && NVM_ACLoss_SaveDataFlag) || Get_FCT_WriteProductionData_Cmd())
        {
            if (Get_FCT_WriteProductionData_Cmd())
            {
                NVM_Modify_Default_HW_Version();
            }

            NVM_SaveEepromData();
            Clear_FCT_WriteProductionData_Cmd();

            FCT_Eeprom_FaultInjection(EEPROM_FAULT_INJECTION_BLOCK);

            NVM_ACLoss_SaveDataFlag = FALSE;
            NVM_ACLoss_SaveDataTime = OperatingTimeCounter0x2006.SSDCBPoweredOnRunningTime;
            return;
        }
        // After saving eeprom data triggered by AC loss, need wait for 30 minutes to enable next save flag
        if (((OperatingTimeCounter0x2006.SSDCBPoweredOnRunningTime - NVM_ACLoss_SaveDataTime) >= NVM_SAVE_BLOCK_INTERVAL_TIME)
            || (Supplier.FltCfgData.ACLossStatus == FALSE && Supplier.WarnCfgData.warningFlags.bits.ACLoss == 0)) // ACLoss warning is cleared
        {
            NVM_ACLoss_SaveDataFlag = TRUE;
        }

        NVM_SaveErrHist();
        NVM_SaveRunTime();
    }

    return;
}

void NVM_Modify_Default_HW_Version(void)
{
    ProductionData_t* pProductionData = NULL;

    if (TestMode0x2091.TestSSDCBHWVersion)
    {
        pProductionData = (ProductionData_t*) &DefaultProductionData[NVM_SSDCB_EEPROM];
        memcpy(pProductionData->KUKA_Info.HWVersion, &TestMode0x2091.TestSSDCBHWVersion, 4);
    }

    if (TestMode0x2091.TestSSDBHWVersion)
    {
        pProductionData = (ProductionData_t*) &DefaultProductionData[NVM_SSDB_EEPROM];
        memcpy(pProductionData->KUKA_Info.HWVersion, &TestMode0x2091.TestSSDBHWVersion, 4);
    }

    if (TestMode0x2091.TestEADBHWVersion)
    {
        pProductionData = (ProductionData_t*) &DefaultProductionData[NVM_EADB_EEPROM];
        memcpy(pProductionData->KUKA_Info.HWVersion, &TestMode0x2091.TestEADBHWVersion, 4);
    }

    return;
}

/******* E N D ***** (NVM.c     ) *********************************************/