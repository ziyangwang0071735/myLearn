/**
 * @file FileSys_cfg.c
 * @brief File System config
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

/******* I N C L U D E - F I L E S *******************************************/
#include "FileSys.h"
/******* L O C A L - D E F I N E S *******************************************/

/******* L O C A L - T Y P E S ***********************************************/

/******* L O C A L - D A T A *************************************************/

/******* L O C A L - F U N C T I O N - P R O T O T Y P E S *******************/

/******** G L O B A L - F U N C T I O N - P R O T O T Y P E S *****************/
/********USER CONFIG START***********/
/**
 * Users configure according to the hardware and file system parameter requirements.
 */
FILESYS_CFG_t File_Cfg_28P65 = {
    .flashSectorSize = 1024,
    .flashMinAvailVol = 131072,//128 * 1024,
    .fileSysPageFactor = 1,
    .flashStartAddr = 0xC0000,
    .updateStartAddr = 0xDE000,
    .filePassword = 0x12345678,
};

FILESYS_FLASH_CFG_t File_Cfg_28P65_InnerFlash = {
    .StartAddress = 0xC0000,
    .EndAddress = 0xC0000 + 256 * 1024,
    .Flash_Write = Hal_Flash_Write,
    .Flash_Read = Hal_Flash_Read,
    .Flash_Init = Hal_Flash_Init,
    .Flash_Eraser = Hal_Flash_Eraser,
    .Flash_Status = NULL,
    .Feed_Dog = NULL,
    .CRC32_Calc =CRC32_Calc,
};
const char* fileSys_illegal_Name_Arr[FILESYS_ILLEDGE_FILENAME_NUM] = {"dir", ".","..","\n","/","\\","\t","?","|", NULL};
/********USER CONFIG END***********/

/******* L O C A L - F U N C T I O N - D E F I N I T I O N S *****************/

/******* G L O B A L - F U N C T I O N - D E F I N I T I O N S ***************/

/******* E N D ***** (FileSys_config.c) *********************************************/