/**
 * @file FileSys_cfg.h
 * @brief File System config file
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
#ifndef FILESYS_CFG_H
#define FILESYS_CFG_H

/******* I N C L U D E - F I L E S *******************************************/
#include"stdint.h"

/*******U S E R - C O N F I G - S T A R T*************************************/

#define FILESYS_CONFIG_DEVICE File_Cfg_28P65
#define FILESYS_CONFIG_FLASH File_Cfg_28P65_InnerFlash
#define FILESYS_CACHE_SIZE sizeof(FILESYS_SECTOR_DIRECTORY_t)//if the maxBlockSize > 6,736Byte,the User have to modify the configuration
#define DEVICE_16bit 1 //Chip Type eg. TI F28P65
#define DEVICE_32bit 0 //Chip Type eg. ST H743
/*******U S E R - C O N F I G - E N D*************************************/
 int Hal_Flash_Write(uint32_t addr, const uint8_t *data, uint32_t len);
 int Hal_Flash_Read(uint32_t addr, uint8_t *buf, uint32_t len);
 int Hal_Flash_Init(void);
 int Hal_Flash_Eraser(uint32_t addr, uint32_t len);
 int Hal_Feed_Dog(void);
 uint32_t CRC32_Calc(const uint8_t* buf, uint32_t len);
 uint32_t Flash_Status(void);


/******* G L O B A L - T Y P E S *********************************************/



/******* G L O B A L - D A T A ***********************************************/

/******* G L O B A L - F U N C T I O N - P R O T O T Y P E S *****************/

#endif /* FILESYS_CFG_H */
/******* E N D ***** (FileSys_cfg.h) *********************************************/