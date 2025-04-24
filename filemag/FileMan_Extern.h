#ifndef FILEMAN_EXTERN_H
#define FILEMAN_EXTERN_H

/******* I N C L U D E - F I L E S *******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileMan.h"
#include "FileMan_Extern.h"

/******* G L O B A L - D E F I N E S ******************************************/
#define FILEMAN_MAX_NUM 62
#define FILEMAN_NAME_MAX_LEN 32


#define  FILEMAN_DIRECTORY_START_ADDRESS  0xC01000
#define  FILEMAN_DIRECTORY_END_ADDRESS  0xC03000
#define  FILEMAN_OCCUPY_END_ADDRESS  (0xC03000+0x3E000)//62 sectors

#define  FILEMAN_FLASH_SECTOR_SIZE     4096
#define  FILEMAN_FLASH_PAGE_SIZE     256
#define  FILEMAN_MAX_FILE_SIZE         (2*FILEMAN_FLASH_SECTOR_SIZE) /*8KB*/
#define  FILEMAN_MAX_FOE_FILE_SIZE     (FILEMAN_FLASH_SECTOR_SIZE) /*4KB*/
#define FILEMAN_MAX_EDCDIRCTORY_SIZE (2802U)//FILEMAN_MAX_NUM*sizeof(FILEMAN_FILEINFO_t)+62+4+4+4
#define FILEMAN_READ_FILEINFO_OFFSET (70U)//FILEMAN_MAX_NUM*sizeof(FILEMAN_FILEINFO_t)
#define  FILEMAN_FILE_BUFF_SIZE     (FILEMAN_MAX_FOE_FILE_SIZE + 192)
/******* G L O B A L - T Y P E S **********************************************/
#pragma pack(1)

typedef struct
{
  uint32_t TotlaFileNums;//4
  uint32_t TotlaOcuppySectors;//4
  uint8_t SectorTable[FILEMAN_MAX_NUM];//62
    uint8_t Payload[FILEMAN_MAX_EDCDIRCTORY_SIZE-12-FILEMAN_MAX_NUM];//2802-4-4-4-62
    uint32_t DirectoryCRC32;//4
} FILEMAN_SECTOR_DIRECTORY_t;

typedef struct
{
    char fileName[FILEMAN_NAME_MAX_LEN];
    uint32_t fileAddress;
    uint32_t fileSize;
    uint32_t fileCRC;
} FILEMAN_FILEINFO_t;//44

#pragma pack()

typedef struct
{
  uint32_t readSize;
  uint32_t readFileRemainderSize;
  uint32_t readFileAddress;
  uint16_t readFileIndex;
  uint32_t prevReadOffset;
  uint32_t fileCRC;
  uint16_t writeFileIndex;
  uint32_t writeFileOffset;
  uint32_t writeFileSize;
  uint16_t writeFileSector;
} FILEMAN_VAR_t;


/******* G L O B A L - D A T A ************************************************/
extern uint16_t FileMan_flashWriteStatus;//write file to flash failure, 0--no error,1-- error.
extern uint16_t FileMan_flashReadStatus;//read file from flash failure, 0--no error,1-- error.
extern uint16_t FileMan_formatFlag;

extern FILEMAN_VAR_t FileMan_var;
/******* G L O B A L - M A C R O S ********************************************/

/******* G L O B A L - F U N C T I O N S - P R O T O T Y P E S ****************/
extern void FileMan_initDirectory(void);
extern void FileMan_initDisk(void);
extern uint16_t FileMan_AddFileToDirectory(char  * pFileName,uint16_t nameSize);
extern uint16_t FileMan_ReadFirstSectorFile(char *pFileName,uint16_t nameSize, uint8_t *OutBuf);
extern void FileMan_ReadRemainderSectorFile(uint32_t hadReadSize, uint8_t *OutBuf);
extern uint16_t FileMan_WtireFileData(uint16_t WriteBlock,uint16_t Size,char *FileName,uint8_t *WriteBuf);
extern uint8_t FileMan_DelFileData(uint16_t FileIndex);
extern uint32_t FileMan_GetFileCRC(uint16_t FileIndex);
extern uint32_t FileMan_GetFileSize(uint16_t FileIndex);
#endif /*FILEMAN_EXTERN_H */
/******* E N D  (FILEMAN_EXTERN_H.h)****************************************************/