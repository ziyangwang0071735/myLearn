/**
 * @file        FileSys.h
 * @brief       File System 
 * @author      Wang.Ziyang  
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
 * V0.0.0|Wang.Ziyang  |05/19/2025      |Module Initial                
 */

#ifndef FILESYS_H
#define FILESYS_H

/******* I N C L U D E - F I L E S *******************************************/
#include "stdint.h"
#include "FileSys_cfg.h"
/******* G L O B A L - D E F I N E S *****************************************/
#if  DEVICE_16bit == 1 
#define  SIZE_FACTOR   1
#define  ODD_CURRENT   1//eg.(nameSize + ODD_CURRENT)>>SIZE_FACTOR
#elif DEVICE_32bit == 1
#define  SIZE_FACTOR = 0
#define  ODD_CURRENT = 0
#endif

#define FILENAMEW_MAX_SIZE 32
#define PAYLOAD_MAX_LENGTH  (120*sizeof(FILESYS_FILEINFO_t)) //FILESYS_CONFIG_DEVICE.FILESYS_PAGE_NUMB*sizeof(FILESYS_FILEINFO_t)
#define FILEPAGE_MAPPING_MAX_SIZE 4//max_file_num = 120 < 4*32
#define FILESYS_MAX_FILENAME_SIZE 32// Maximum length of a filename
#define FILESYS_ILLEDGE_FILENAME_NUM 10 


typedef struct FILESYS_FILEINFO FILESYS_FILEINFO_t;
typedef struct FILESYS_SECTOR_DIRECTORY FILESYS_SECTOR_DIRECTORY_t;
typedef struct FILESYS_CFG_EXPAND FILESYS_CFG_EXPAND_t;
typedef struct FILESYS_CFG_USER FILESYS_CFG_t;
typedef struct FILESYS_DIR_VAR FILESYS_DIR_VAR_t;
typedef struct FILESYS_CURRENT_STATUS FILESYS_CURRENT_STATUS_t;
typedef struct FILESYS_FLASH_CFG_USER FILESYS_FLASH_CFG_t;

extern FILESYS_CFG_EXPAND_t File_Cfg_Expand;
extern FILESYS_CFG_t FILESYS_CONFIG_DEVICE;
extern FILESYS_FLASH_CFG_t FILESYS_CONFIG_FLASH;
extern FILESYS_DIR_VAR_t  DirVar;
extern FILESYS_CURRENT_STATUS_t  FileSys_CurrentStatus;
extern FILESYS_VAR_t FileSys_Var;
extern const char* fileSys_illegal_Name_Arr[FILESYS_ILLEDGE_FILENAME_NUM]; 


/******* G L O B A L - T Y P E S *********************************************/




typedef struct FILESYS_CFG_USER {
    uint32_t flashSectorSize;       // Size of a flash sector
    uint32_t flashMinAvailVol;      // Minimum available volume in flash
    uint32_t fileSysPageFactor;     // FILESYS_PAGE_SIZE = fileSysPageFactor * flashSectorSize
    uint32_t flashStartAddr;        // Start address of the flash
    uint32_t updateStartAddr;
  uint32_t filePassword;      // File password    
} FILESYS_CFG_t;

typedef struct FILESYS_FLASH_CFG_USER{
    uint32_t StartAddress;//FLASH_START_ADDRESS,shall cover FILESYS_CFG_t.flashStartAddr
    uint32_t EndAddress;//FLASH_START_ADDRESS,shall cover FILESYS_CFG_EXPAND_t.flashEndAddr
    uint32_t  (*Flash_Write)(uint32_t addr, const uint8_t *data, uint32_t len);
    uint32_t  (*Flash_Read)(uint32_t addr, uint8_t *buf, uint32_t len);
    uint32_t  (*Flash_Init)(void);
    uint32_t  (*Flash_Eraser)(uint32_t addr, uint32_t len);
  uint32_t  (*Feed_Dog)(void);
  uint32_t (*CRC32_Calc)(const uint8_t* buf, uint32_t len);
}FILESYS_FLASH_CFG_t;
typedef struct
{
  uint32_t readSize;
  uint32_t readFileRemainderSize;
  uint32_t readFileAddress;
  uint16_t readFileIndex;
  uint32_t prevReadOffset;
  uint32_t fileCRC;
  uint16_t writeFileIndex;//special function:-1 isbootloader
  uint32_t writeFileOffset;
  uint32_t writeFileSize;
  uint16_t writeFileSector;
    uint32_t writeFlashAddress;
} FILESYS_VAR_t;

typedef struct FILESYS_CFG_EXPAND{
    FILESYS_CFG_t* file_Config;
    FILESYS_FLASH_CFG_t*File_Cfg_InnerFlash;
    uint32_t fileCacheSize;            // Directory cache size
    uint32_t fileSysDirSize;          // Size of the file system directory
    uint32_t fileSysPageNumb;         // Number of pages in the file system 
    uint32_t fileSysFilenameSize;     // Maximum length of a filename
    uint32_t fileSysPageSize;         // Size of each page，while is the smallest unit of reading
                    // (fileSysPageSize = fileSysPageFactor * flashSectorSize)
    uint32_t fileSysMaxFileVol;       // Maximum file volume size
    uint32_t fileSysMaxFileNumb;      // Maximum number of files
    uint32_t fileSysMaxFileSize;      // Maximum size of a file
    uint32_t flashEndAddr;            // End address of the flash
    uint32_t *fileSys_IllegalName;     // the size of the directory being written/read
  uint32_t *fileCache;        // Pointer to the directory cache
    uint32_t *fileName;        // Pointer to the file name
}FILESYS_CFG_EXPAND_t;


#pragma pack(1)
typedef struct FILESYS_FILEINFO{
    char fileName[FILENAMEW_MAX_SIZE>>SIZE_FACTOR];//size:1*32
    uint32_t filePage_Mapping[FILEPAGE_MAPPING_MAX_SIZE];//size:4*4
    uint32_t fileSize;//size:4
    uint32_t fileCRC;//size:4
} FILESYS_FILEINFO_t;//size:56

typedef struct FILESYS_SECTOR_DIRECTORY{
    uint32_t totlaFileNums;
    uint32_t totlaOcuppyPages;
    uint32_t filePage_Mapping[FILEPAGE_MAPPING_MAX_SIZE];
    uint8_t payload[PAYLOAD_MAX_LENGTH ];
    uint32_t directoryCRC32;
} FILESYS_SECTOR_DIRECTORY_t;
#pragma pack()

typedef struct FILESYS_DIR_VAR{
  uint16_t fileSys_DirFileSize;// the size of the directory being written/read
  uint16_t fileSys_FileNum;// the number of files in current directory.
  uint32_t fileSys_SumPages;// the sum of pages in current directory.
}FILESYS_DIR_VAR_t;

typedef struct FILESYS_CURRENT_STATUS
{
  uint16_t fileSys_FlashWriteStatus;//write err status
  uint16_t fileSys_FlashReadStatus;//read err status
  uint16_t fileSys_IsWriteDir;
  uint16_t fileSys_IsReadDir;
  uint16_t fileSys_IsFileExit;
    uint16_t fileSys_WriteFirstIn;
    uint16_t fileSys_FormatFlag;
    uint16_t fileSys_IsBootMode;
}FILESYS_CURRENT_STATUS_t;

/******* G L O B A L - D A T A ***********************************************/
typedef enum FILESYS_ERRCODE{
  fileSysErrCode_EOK = 0x0,             /**< \brief No error */
    fileSysErrCode_notDefined = 0x8000,          /**< \brief Not defined */
    fileSysErrCode_notFound = 0x8001,            /**< \brief The file requested  could not be found on the server */
    fileSysErrCode_access = 0x8002,              /**< \brief Read or write access to this file not allowed (e.g. due to local control). */
    fileSysErrCode_diskFull = 0x8003,            /**< \brief Disk to store file is full or memory allocation exceeded */
    fileSysErrCode_illegal = 0x8004,             /**< \brief Illegal operation, e.g. service identifier invalid */
    fileSysErrCode_packeNo = 0x8005,             /**< \brief <Compatible with FOE> FoE packet number invalid */
    fileSysErrCode_exists = 0x8006,              /**< \brief The file which is requested to be downloaded does already exist */
    fileSysErrCode_noUser = 0x8007,              /**< \brief No User */
    fileSysErrCode_bootstrapOnly = 0x8008,       /**< \brief <Compatible with FOE>FoE only supported in Bootstrap */
    fileSysErrCode_notInBootstrap = 0x8009,      /**< \brief This file may not be accessed in BOOTSTRAP state */
    fileSysErrCode_noRights = 0x800A,            /**< \brief Password invalid */
    fileSysErrCode_progError = 0x800B,           /**< \brief Generic programming error. Should only be returned if error reason cannot be distinguished */
    fileSysErrCode_invalidChecksum = 0x800C,     /**< \brief checksum included in the file is invalid */
    fileSysErrCode_invalidFirmware = 0x800D,     /**< \brief The hardware does not support the downloaded firmware */
    fileSysErrCode_noFile = 0x800F,              /**< \brief Do not use (identical with 0x8001) */
    fileSysErrCode_noFileHeader = 0x8010,        /**< \brief Missing file header of error in file header */
    fileSysErrCode_flashError = 0x8011           /**< \brief Flash cannot be accessed */
} FILESYS_ERRCODE_e;

/******* G L O B A L - F U N C T I O N - P R O T O T Y P E S *****************/
void FileSys_Cfg_Init(void);
void FileSys_Init(void);
FILESYS_ERRCODE_e FileSys_ReadDirPage(void);
FILESYS_ERRCODE_e FileSys_ReadFileHeader(uint8_t * pName, uint16_t nameSize, uint32_t password, 
                    uint16_t maxBlockSize, uint8_t *pData);
FILESYS_ERRCODE_e FileSystem_ReadData(uint32_t offset, uint16_t maxBlockSize, uint8_t *pData);
FILESYS_ERRCODE_e FileSys_PrepareWrite(uint8_t * pName, uint16_t nameSize, uint32_t password);
FILESYS_ERRCODE_e FileSys_WriteData(uint8_t * pData, uint16_t Size, BOOL bDataFollowing);
FILESYS_ERRCODE_e FileSys_isBootMode(void);
FILESYS_ERRCODE_e FileSys_ClcDir(void);//Clear the directory in the flash.
FILESYS_ERRCODE_e FileSys_IsFileNameIllegal(void)
#endif /* FILESYS_H */
/******* E N D  (FileSys.h)****************************************************/