/**
 * @file FileSys.c
 * @brief File System 
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
#include "FileSys_cfg.h"
/******* L O C A L - D E F I N E S *******************************************/

/******* L O C A L - T Y P E S ***********************************************/

/******* L O C A L - D A T A *************************************************/
uint8_t fileCache_Arr[FILESYS_CACHE_SIZE)];
uint8_t fileName_Arr[FILENAMEW_MAX_SIZE];
/******* L O C A L - F U N C T I O N - P R O T O T Y P E S *******************/

/******** G L O B A L - F U N C T I O N - P R O T O T Y P E S *****************/

/******* L O C A L - F U N C T I O N - D E F I N I T I O N S *****************/
FILESYS_ERRCODE_e FileSystem_ReadFileGeneral(const uint8_t *pName,uint8_t name_Size,uint8_t* pBuffer, uint32_t bufferSize,uint8_t continue_File)
{//MARK bug:how to save filesys_var if the buffer is small???
    uint16_t ret;
    uint32_t offset = 0;
    uint16_t blockSize;
    FILESYS_ERRCODE_e ret;

    ret = FileSys_ReadFileHeader(pName, (uint16_t)name_Size, FILESYS_CONFIG_DEVICE.filePassword, bufferSize,pBuffer);

    u32PacketNo = 1;
    u32FileOffset = ret;
    u32LastFileOffset = 0;

    while (offset < bufSize)
    {
        blockSize = (bufSize - offset) > FOE_PACKET_MAX_SIZE ? FOE_PACKET_MAX_SIZE : (bufSize - offset);       
        ret = FileSystem_ReadData(offset, blockSize, pBuf + offset);
        if (ret != FILESYS_ERR_NONE)
        {
            return ret;
        }
        offset += blockSize;
    }
    return 0;
}
FILESYS_ERRCODE_e FileSystem_WriteFileGeneral(const uint8_t *pName,uint8_t name_Size,const uint8_t *pBuf, uint32_t dataLen)
{//MARK bug:the method for handling bDataFollowing is lacking 
    uint32_t offset = 0;
    uint16_t blockSize;
    FILESYS_ERRCODE_e ret;

    while (offset < dataLen)
    {
        blockSize = (dataLen - offset) > FOE_PACKET_MAX_SIZE ? FOE_PACKET_MAX_SIZE : (dataLen - offset);
        
        ret = FileSys_WriteData((uint8_t*)(pBuf + offset), blockSize,1);
        if (ret != FILESYS_ERR_NONE)
        {
            return ret;
        }
        if (ret != FILESYS_ERR_NONE)
        {
            return ret;
        }

        offset += blockSize;
    }

    return FILESYS_ERR_NONE;
}
FILESYS_ERRCODE_e FileSys_ReadDirPage(void){
    FILESYS_ERRCODE_e errCode = fileSysErrCode_EOK;
    uint16_t curReadLen = 0;
    uint32_t curOffset = File_Cfg_Expand.file_Config->flashStartAddr;
    uint32_t fileLen = File_Cfg_Expand.fileCacheSize;
    uint32_t curOffset0 = File_Cfg_Expand.file_Config->flashStartAddr;
    DirVar.fileSys_DirFileSize = 0;
    FileSys_CurrentStatus.fileSys_FlashWriteStatus = 0;
    FileSys_CurrentStatus.fileSys_FlashReadStatus = 0;
    FileSys_CurrentStatus.fileSys_IsFileExit = 0;

    memset(File_Cfg_Expand.fileCache,0,File_Cfg_Expand.fileCacheSize);

    //Read page by page
      while (fileLen > 0)
  {
    //num bytes to read for this iteration
        if (fileLen > File_Cfg_Expand.fileSysPageSize)
    {
      curReadLen = File_Cfg_Expand.fileSysPageSize;
    }
    else
    {
      curReadLen = fileLen;
    }

    //read one page
        File_Cfg_Expand.File_Cfg_InnerFlash->Flash_Read(curOffset, curReadLen, (uint8_t *)File_Cfg_Expand.fileCache)+ (curOffset - curOffset0);
    fileLen -= curReadLen;
    curOffset += curReadLen;
        if(File_Cfg_Expand.File_Cfg_InnerFlash->Feed_Dog() != NULL){
            File_Cfg_Expand.File_Cfg_InnerFlash->Feed_Dog();
        }
  }
    //Calc u32FileCRC
    uint32_t dirFileCRC = File_Cfg_Expand.File_Cfg_InnerFlash->CRC32_Calc((uint8_t *)File_Cfg_Expand.fileCache, File_Cfg_Expand.fileCacheSize-sizeof(uint32_t));
    //Extract the dir struct
    FILESYS_SECTOR_DIRECTORY_t *pDir = (FILESYS_SECTOR_DIRECTORY_t *)File_Cfg_Expand.fileCache;
    if((pDir->directoryCRC32 == dirFileCRC) && ((pDir->totlaFileNums > 0) && (pDir->totlaFileNums < File_Cfg_Expand.fileSysMaxFileNumb))
      && ((pDir->totlaOcuppyPages < File_Cfg_Expand.fileSysPageNumb) && (pDir->totlaOcuppyPages > 0)))
  {
    DirVar.fileSys_FileNum = pDir->totlaFileNums;
    DirVar.fileSys_SumPages = pDir->totlaOcuppyPages;
  }
  else
  {
    DirVar.fileSys_FileNum = 0;
    DirVar.fileSys_SumPages = 0;
    memset(File_Cfg_Expand.fileCache,0 ,File_Cfg_Expand.fileCacheSize);
        errCode = fileSysErrCode_notFound;
  }
    return  errCode;
};
FILESYS_ERRCODE_e FileSys_ClcDir(void){
    File_Cfg_Expand.File_Cfg_InnerFlash.Flash_Eraser(File_Cfg_Expand.file_Config->flashStartAddr, File_Cfg_Expand.fileSysDirSize);
    memset(&DirVar, 0, sizeof(DirVar));
    memset(&FileSys_CurrentStatus, 0, sizeof(FileSys_CurrentStatus));
  memset(File_Cfg_Expand.fileCache,0,File_Cfg_Expand.fileCacheSize);
}

FILESYS_ERRCODE_e FileSys_IsBootMode(void){
    return fileSysErrCode_EOK;
}
uint16_t FileSys_availablePage(FILESYS_SECTOR_DIRECTORY_t *dir,uint16_t serchNo){}
FILESYS_ERRCODE_e FileSys_IsFileNameIllegal(void) {
    for (int i = 0; i < FILESYS_ILLEDGE_FILENAME_NUM; i++) {
        if (File_Cfg_Expand.fileSys_IllegalName[i] == NULL) {
            break; 
        }
        if (strstr(File_Cfg_Expand.fileName, fileSys_illegal_Name_Arr[i]) != NULL) {
            return fileSysErrCode_illegal; 
        }
    }
    return fileSysErrCode_EOK; 
}
uint16_t FileSys_findFileIndex(char  * pFileName,uint16_t nameSize){}
uint16_t FileSys_AddFileToDirectory(char  *pFileName,uint16_t nameSize) {
  uint16_t FileIndex = -1;

    if (DirVar.fileSys_FileNum >= File_Cfg_Expand.fileSysMaxFileNumb)
    {
        return FileIndex; // No space for new files
    }
    MEMCPY(File_Cfg_Expand.fileName, pFileName, nameSize + ODD_CURRENT);
    MEMCPY((File_Cfg_Expand.fileName + ((nameSize + ODD_CURRENT)>>SIZE_FACTOR)), "\0", 1); 
  if(FileSys_IsFileNameIllegal() != fileSysErrCode_EOK)
  {
    return FileIndex;
  }

    FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILESYS_READ_FILEINFO_OFFSET];
    FILESYS_SECTOR_DIRECTORY_t *dir = (FILESYS_SECTOR_DIRECTORY_t *)File_Cfg_Expand.fileCache; = (FILESYS_SECTOR_DIRECTORY_t *)File_Cfg_Expand.fileCache;
    FileIndex = FileSys_findFileIndex(pFileName,nameSize);
    if (FileIndex < File_Cfg_Expand.fileSysMaxFileNumb)
    {
      DirVar.fileExist_Flag = 1;
        return FileIndex;
    }

    //seach min free sector no, as the first address of new file.
    uint16_t sectorIndex = FileSys_availablePage(dir,0);//serach min free sector
    if(sectorIndex > FILEMAN_MAX_NUM)
    {
       return FileIndex;
    }

    FileIndex = dir->TotlaFileNums;
    uint32_t newFileAddress = FILEMAN_DIRECTORY_END_ADDRESS + sectorIndex * FILEMAN_FLASH_SECTOR_SIZE;
  if(newFileAddress < FILEMAN_DIRECTORY_END_ADDRESS || newFileAddress > FILEMAN_OCCUPY_END_ADDRESS)
  {
    return FileIndex;
  }

  dir->SectorTable[sectorIndex] = FileIndex + 1;
    strncpy(fileInfo[FileIndex].fileName, pFileName, nameSize);
    fileInfo[FileIndex].fileName[nameSize] = 0x00;
    fileInfo[FileIndex].fileAddress = newFileAddress;
    fileInfo[FileIndex].fileSize = 0; // Initial size


    FileMan_FileExist_Flag = 0;

    return (FileIndex);
}                
uint16_t FileSys_WriteFileData(uint16_t WriteBlock,uint16_t Size,char *FileName,uint8_t *WriteBuf){}
uint8_t FileSys_DelFileData(uint16_t FileIndex){}
uint16_t FileSys_ReadFileFirstBlock(char *pFileName,uint16_t nameSize, uint8_t *OutBuf){}
void FileSys_ReadFileRemainderPage(uint32_t hadReadSize, uint8_t *OutBuf){}
/******* G L O B A L - F U N C T I O N - D E F I N I T I O N S ***************/
FILESYS_ERRCODE_e FileSys_ReadFileHeader(uint8_t * pName, uint16_t nameSize, uint32_t password, 
                    uint16_t maxBlockSize, uint8_t *pData){
  uint16_t readSize = 0;
 
     FileSys_Var.readFileRemainderSize = 0;
     FileSys_Var.readFileAddress = 0;
     FileSys_Var.prevReadOffset = 0;
 
     if ((nameSize + 1) > FILENAMEW_MAX_SIZE - 2)
     {
         return readSize;
     }//MARK
 
     /*Read requested file name to endianess conversion if required*/
      MEMCPY(File_Cfg_Expand.fileName, pName, nameSize + ODD_CURRENT);
      MEMCPY((File_Cfg_Expand.fileName + ((nameSize + ODD_CURRENT)>>SIZE_FACTOR)), "\0", 1); //string termination
     if(FileSys_ReadFileFirstBlock(File_Cfg_Expand.fileName,nameSize,File_Cfg_Expand.fileCache) != fileSysErrCode_EOK)
   {
     return readSize;
   }
 
   readSize = maxBlockSize;
 
     if (FileSys_Var.readSize < readSize)
     {
       readSize = (uint16_t)FileSys_Var.readSize;
 
     }
 
     FileSys_Var.fileCRC = 0;
     //Calc u32FileCRC.
     File_Cfg_Expand.File_Cfg_InnerFlash->CRC32_Calc(File_Cfg_Expand.fileCache, readSize, &FileSys_Var.fileCRC);
     MEMCPY(pData, File_Cfg_Expand.fileCache, readSize);
 //    FileSys_Var.readFileIndex = 0;
     return readSize;
                                        }
FILESYS_ERRCODE_e FileSystem_ReadData(uint32_t offset, uint16_t maxBlockSize, uint8_t *pData){
    uint32_t sizeError = 0;
     if (FileSys_Var.readSize <= offset)
     {
         return 0;
     }
     /*get file length to send*/
     sizeError = FileSys_Var.readSize - offset;
     if (sizeError > maxBlockSize)
     {
         /*transmit max block size if the file data to be send is greater than the max data block*/
         sizeError = maxBlockSize;
     }
     else
     {
       if(FileSys_Var.readFileRemainderSize > 0)
       {
           MEMCPY(&(((uint8_t *)File_Cfg_Expand.fileCache)[0]), 
                  &(((uint8_t *)File_Cfg_Expand.fileCache)[offset - FileSys_Var.prevReadOffset]), sizeError);
           memset(&(File_Cfg_Expand.fileCache[sizeError]), 0, sizeof() - sizeError);
 
           FileSys_ReadFileRemainderPage(sizeError,aFileData);
           FileSys_Var.prevReadOffset = offset;
           sizeError = FileSys_Var.readSize - offset;
             if (sizeError > maxBlockSize)
             {
                 /*transmit max block size if the file data to be send is greater than the max data block*/
                 sizeError = maxBlockSize;
             }
       }
 
}}
FILESYS_ERRCODE_e FileSys_PrepareWrite(uint8_t * pName, uint16_t nameSize, uint32_t password){
     if (nameSize < File_Cfg_Expand.fileSysFilenameSize)//mark /
    {
        /* for test every file name can be written */
        MEMCPY(File_Cfg_Expand.fileName, pName, nameSize + ODD_CURRENT);
        MEMCPY((File_Cfg_Expand.fileName + ((nameSize + ODD_CURRENT)>>SIZE_FACTOR)), "\0", 1); //string termination
        FileSys_Var.writeFileIndex = -1;
        FileSys_Var.writeFileOffset = 0;
        FileSys_Var.writeFileSector = 0;
        FileSys_Var.writeFileSize = 0;
        FileSys_Var.writeFlashAddress = 0;
        FileSys_Var.fileCRC = 0x00000000;
        FileSys_CurrentStatus.fileSys_WriteFirstIn = 1;

    if(fileSys_IsBootMode() != fileSysErrCode_EOK)
    {
    #if DEVICE_16bit == 1
        if ( strstr(File_Cfg_Expand.fileName, "format") && nameSize == 3)
   #else
            if(strstr(File_Cfg_Expand.fileName, "format") && nameSize == 6)
   #endif
      {
        FileSys_ClcDir();
                FileSys_CurrentStatus.fileSys_FormatFlag = 1;
      }
      else if (FileSys_IsFileNameIllegal() != fileSysErrCode_EOK)
      {
        return fileSysErrCode_illegal;
      }
      else
      {
        FileSys_Var.writeFileIndex = FileSys_AddFileToDirectory(File_Cfg_Expand.fileName,nameSize);
        if (FileSys_Var.writeFileIndex >File_Cfg_Expand.fileSysMaxFileNumb)
        {
          return fileSysErrCode_diskFull;
        }
      }
            if(File_Cfg_Expand.File_Cfg_InnerFlash->Feed_Dog() != NULL){
            File_Cfg_Expand.File_Cfg_InnerFlash->Feed_Dog();
        }
    }
    else// isBootMode
    {
      FileSys_Var.writeFileIndex = -1;//mark
            FileSys_CurrentStatus.fileSys_IsBootMode = 1;
          if (password != File_Cfg_Expand.file_Config->filePassword)
            {
                return fileSysErrCode_illegal;
            }
      if(File_Cfg_Expand.File_Cfg_InnerFlash->Feed_Dog() != NULL){
            File_Cfg_Expand.File_Cfg_InnerFlash->Feed_Dog();
        }
    }
        return fileSysErrCode_EOK;
    }
    else
    {
        return fileSysErrCode_diskFull;
    }
}

FILESYS_ERRCODE_e FileSys_WriteData(uint8_t * pData, uint16_t Size, BOOL bDataFollowing){

  uint32_t EraseFlashAddress = 0;
  uint32_t FileBlockflag = 0;

  if (FileSys_CurrentStatus.fileSys_FormatFlag == 1)
  {
    FileSys_CurrentStatus.fileSys_FormatFlag == 0;
    return fileSysErrCode_EOK;
  }

  if((!FileSys_CurrentStatus.fileSys_IsBootMode) && (Size == 0) && (FileSys_Var.writeFileIndex <File_Cfg_Expand.fileSysMaxFileNumb))
  {
    if(0 == FileSys_DelFileData(FileSys_Var.writeFileIndex))
    {
      return fileSysErrCode_illegal;
    }
    return fileSysErrCode_EOK;
  }

    if (Size < 0 || (Size == 0 && FileSys_CurrentStatus.fileSys_IsBootMode) || ((FileSys_Var.writeFileIndex < File_Cfg_Expand.fileSysMaxFileNumb) && FileSys_CurrentStatus.fileSys_IsBootMode))
    {
      return fileSysErrCode_packeNo;
    }

    if (Size)
    {
      MBXMEMSET(File_Cfg_Expand.fileCache,0,File_Cfg_Expand.fileCacheSize+ODD_CURRENT);
      MBXMEMCPY(File_Cfg_Expand.fileCache, (uint8_t *)pData, Size+ODD_CURRENT);
    }

    //update write address offset.
    FileSys_Var.writeFileOffset += Size;

    //First execute function process
  if (FileSys_CurrentStatus.fileSys_WriteFirstIn)
  {
    if (FileSys_Var.writeFileIndex < File_Cfg_Expand.fileSysMaxFileNumb)
    {
      if (bDataFollowing)//The file size is more than the data written in a single operation by FOE.
      {
        FileBlockflag = 1;//The first part of the file.
      }
      else//The file size is smaller than the data written in a single operation by FOE.
      {
        FileBlockflag = 2;// The first part of the file, which is also the final part.
            FileSys_Var.writeFileSize = FileSys_Var.writeFileOffset;
      }

    }
    else //FWupdate writeFileIndex = -1
      {
      // if((aFileData[0x30] != 0xAA) || (aFileData[0x31] != 0x99)||
      //    (aFileData[0x32] != 0x55) || (aFileData[0x33] != 0x66) )
      // {
      //   return ECAT_FOE_ERRCODE_INVALID_FIRMWARE;
      // }

      // if((aFileData[0x63] != 0) )
      // {
      //   return ECAT_FOE_ERRCODE_INVALID_FIRMWARE;
      // }//MARK

        File_Cfg_Expand.File_Cfg_InnerFlash->Flash_Eraser(
          File_Cfg_Expand.file_Config->updateStartAddr,
          File_Cfg_Expand.File_Cfg_InnerFlash->EndAddress - File_Cfg_Expand.file_Config->updateStartAddr);
      FileSys_Var.WriteFlashAddress = File_Cfg_Expand.file_Config->updateStartAddr;
      }

    FileSys_CurrentStatus.fileSys_WriteFirstIn = 0;
  }
  else
  {
      if (FileSys_Var.writeFileIndex < File_Cfg_Expand.fileSysMaxFileNumb)//write file data
      {
          if (bDataFollowing == 0)//last part of the file is written
          {
            FileBlockflag = 3;
            FileSys_Var.writeFileSize = FileSys_Var.writeFileOffset;
          }
          else//2nd,3rd... part of the file is written
          {
            FileBlockflag = 4;
          }
      }
  }

    if (FileSys_Var.writeFileIndex < File_Cfg_Expand.fileSysMaxFileNumb)//write file data
    {
      if(FileSys_WriteFileData(FileBlockflag,Size,File_Cfg_Expand.fileName,File_Cfg_Expand.fileCache) == 0)
      {
        return fileSysErrCode_illegal;}
    }
  else//FWupdate
    {
      //check write address offset before write flash
    if(FileSys_Var.writeFileOffset >= File_Cfg_Expand.File_Cfg_InnerFlash.EndAddress)
    {
      return fileSysErrCode_invalidFirmware;
    }
    if(0 != File_Cfg_Expand.File_Cfg_InnerFlash->Flash_Write((FileSys_Var.WriteFlashAddress),(uint8_t *)(File_Cfg_Expand.fileCache), Size))
    {
      return fileSysErrCode_progError;
    }
    FileSys_Var.writeFileOffset = FileSys_Var.writeFileOffset + Size;
  }

    return 0;
}

void FileSys_Cfg_Init(void){

    //INIT file_cfg_expand
    FILESYS_CFG_EXPAND_t File_Cfg_Expand = {
    .file_Config = &FILESYS_CONFIG_DEVICE,
    .File_Cfg_InnerFlash = &FILESYS_CONFIG_FLASH,
    .fileCacheSize = sizeof(FILESYS_SECTOR_DIRECTORY_t),
    .fileSysDirSize = 8*1024,
    .fileSysPageNumb = (FILESYS_CONFIG_DEVICE.flashMinAvailVol - 8*1024)/FILESYS_CONFIG_DEVICE.fileSysPageFactor,
    .fileSysFilenameSize = FILESYS_MAX_FILENAME_SIZE,
    .fileSysPageSize = FILESYS_CONFIG_DEVICE.flashSectorSize*FILESYS_CONFIG_DEVICE.fileSysPageFactor,
    .fileSysMaxFileVol = (FILESYS_CONFIG_DEVICE.flashMinAvailVol - 8*1024),
    .fileSysMaxFileNumb = 0,
    .fileSysMaxFileSize = 0,
    .flashEndAddr = FILESYS_CONFIG_DEVICE.flashStartAddr + FILESYS_CONFIG_DEVICE.flashMinAvailVol,
    .fileSys_IllegalName = fileSys_illegal_Name_Arr,
    .fileCache = fileCache_Arr,
    .fileName = fileName_Arr,
};
    File_Cfg_Expand.fileSysMaxFileNumb = File_Cfg_Expand.fileSysPageNumb - (File_Cfg_Expand.flashEndAddr - 
                                      File_Cfg_Expand.file_Config->updateStartAddr)/FILESYS_CONFIG_DEVICE.fileSysPageFactor;
    File_Cfg_Expand.fileSysMaxFileSize = File_Cfg_Expand.fileSysPageSize * File_Cfg_Expand.fileSysMaxFileNumb;

    FILESYS_DIR_VAR_t  DirVar = {0};
    FILESYS_CURRENT_STATUS_t  FileSys_CurrentStatus = {0};
    FILESYS_VAR_t FileSys_Var = {0};
}
void FileSys_Init(void){
    FILESYS_ERRCODE_e errCode = fileSysErrCode_EOK;
    //Init virtual cache
    FileSys_Cfg_Init();
    memset(File_Cfg_Expand.fileCache,0 ,File_Cfg_Expand.fileCacheSize);
   //Read the first page of the directory
    errCode = FileSys_ReadDirPage();
}
/******* E N D ***** (FileSys.c) *********************************************/