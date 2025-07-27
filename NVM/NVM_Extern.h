/**
 * @file        NVM_Extern.h
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

#ifndef NVM_EXTERN_H
#define NVM_EXTERN_H

/******* I N C L U D E - F I L E S *******************************************/
#include "Hal_Eep.h"

/******* G L O B A L - D E F I N E S *****************************************/


typedef enum
{
    NVM_SSDCB_BLOCK_PRODUCTION_DATA = 0U,
    NVM_SSDCB_BLOCK_DEVICE_RUNTIME,
    NVM_SSDCB_BLOCK_AXIS0_ERROR_HISTORY,
    NVM_SSDCB_BLOCK_AXIS1_ERROR_HISTORY,
    NVM_SSDCB_BLOCK_AXIS2_ERROR_HISTORY,
    NVM_SSDCB_BLOCK_AXIS3_ERROR_HISTORY,
    NVM_SSDCB_BLOCK_AXIS4_ERROR_HISTORY,
    NVM_SSDCB_BLOCK_AXIS5_ERROR_HISTORY,
    NVM_SSDCB_BLOCK_AXIS6_ERROR_HISTORY,
    NVM_SSDCB_BLOCK_AXIS7_ERROR_HISTORY,
    NVM_SSDCB_BLOCK_SUPPLIER_ERROR_HISTORY,
    NVM_SSDCB_BLOCK_MEMORY_INUSE_MAX,
} NVM_SSDCB_BLOCK_t;

typedef enum
{
    NVM_SSDB_BLOCK_PRODUCTION_DATA = 0U,
    NVM_SSDB_BLOCK_INVERTER_RUNTIME,
    NVM_SSDB_BLOCK_MEMORY_INUSE_MAX,
} NVM_SSDB_BLOCK_t;

typedef enum
{
    NVM_EADB_BLOCK_PRODUCTION_DATA = 0U,
    NVM_EADB_BLOCK_INVERTER_RUNTIME,
    NVM_EADB_BLOCK_MEMORY_INUSE_MAX,
} NVM_EADB_BLOCK_t;

#define MEMORY_BLOCK_SIZE sizeof(UserMemoryBlock_t)
#define SSDCB_EEPROM_BLOCK_NUM (GT24C_TOTAL_SIZE / MEMORY_BLOCK_SIZE)
#define SSDB_EEPROM_BLOCK_NUM (GT25C_TOTAL_SIZE / MEMORY_BLOCK_SIZE)
#define EADB_EEPROM_BLOCK_NUM (GT25C_TOTAL_SIZE / MEMORY_BLOCK_SIZE)

/******* G L O B A L - T Y P E S *********************************************/
#pragma pack(1)
typedef struct
{
    char KUKA[4];
    char DataLayoutVersion[2];
    char BoardVariant[4];
    char HWVersion[4];
    char Rsvd[16];
} KUKA_INfo_t;

typedef struct
{
    char BarCode[2];
    char MaterialNumber[8];
    char Res[1];
    char SupplierCode[6];
    char Revision[2];
    char SeriesNumber[7];
    char CheckDigit[2];
    char Rsvd[8];
} SN_BoardID_t;

typedef struct
{
    char XF1KSI[6];
    char XF2KONI[6];
    char XF4KLIOT[6];
    char PROFINET1[6];
    char PROFINET2[6];
    char Rsvd[18];
} MAC_Adrress_t;

typedef struct
{
    char TesterID[2];
    char Rsvd[8];
} TesterID_t;

typedef struct
{
    uint32_t CRC32;
} AllCheckSum_t;

typedef struct
{
    KUKA_INfo_t KUKA_Info;     // 30
    SN_BoardID_t SN_BoardID;   // 36
    MAC_Adrress_t MAC_Adrress; // 48
    TesterID_t TesterID;       // 10
    AllCheckSum_t AllCheckSum; // 4
} ProductionData_t;

typedef struct
{
    uint16_t BlockId;
    uint32_t BlockVersion;
    uint8_t Payload[118];
    uint32_t CRC32;
} UserMemoryBlock_t;

#pragma pack()

/******* G L O B A L - D A T A ***********************************************/

/******* G L O B A L - F U N C T I O N S - P R O T O T Y P E S ***************/
extern void NVM_Init(void);
extern int32_t NVM_ReadBlockData(uint8_t nvm_id, uint16_t block_id);
extern int32_t NVM_WriteBlockData(uint8_t nvm_id, uint16_t block_id);
extern int32_t NVM_SaveEepromData(void);
extern void NVM_SaveUpdate(void);
extern void NVM_Modify_Default_HW_Version(void);

int32_t NVM_Unittest(void);
#endif /* NVM_EXTERN_H */
/******* E N D  (NVM_Extern.h)****************************************************/