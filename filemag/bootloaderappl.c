/**
 * @file        bootloaderappl.c
 * @brief       Update firmware
 * @author      zhangnf
 * @version     0.0.0
 * @attention
 *  This material is the exclusive property of KUKA Robot Group
 *  Except as expressly permitted by separate agreement, this material may only
 *  be used by members of the development department of KUKA Robot Group for
 *  internal development purposes of KUKA Robot Group. * Copyright (C) 2024 KUKA Robot Group. All Rights Reserved.
 * Version|Author|Date|Describe
 * -------|-------|-------|--------
 * V0.0.0|zhangnf|04/23/2024|Create File,For EDC EtherCAT FOE update firmware.
 */

 #include <stdbool.h>
 #include "ecat_def.h"
 #include "applInterface.h"
 #include "bootloaderappl.h"
 #include"bootmode.h"
 #include "Flash_Extern.h"
 #include "FileMan_Extern.h"
 #include "Wdt_Ctrl.h"
 #include "CRC32.h"
 
 #define MAX_FIRMWARE_ADDRESS  ((6*1024*1024) - (4*1024))
 
 CHAR  aFileName[MAX_FILE_NAME_SIZE];
 u8    aFileData[FILEMAN_FILE_BUFF_SIZE];//4256
 u32   WriteFlashAddress;
 
 
 static BOOL firstIn;
 
 
 
 
 
 UINT16 FoE_Read(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password, UINT16 maxBlockSize, UINT16 *pData)
 {
     UINT16 readSize = 0;
     CHAR aReadFileName[MAX_FILE_NAME_SIZE];
 
     FileMan_var.readFileRemainderSize = 0;
     FileMan_var.readFileAddress = 0;
     FileMan_var.prevReadOffset = 0;
 
     if ((nameSize + 1) > MAX_FILE_NAME_SIZE - 2)
     {
         return 0;
     }
 
     /*Read requested file name to endianess conversion if required*/
     MBXSTRCPY(aReadFileName, pName, nameSize);
     aReadFileName[nameSize] = '\0';
 
     if(FileMan_ReadFirstSectorFile(aReadFileName,nameSize,aFileData) == 0)
   {
     return 0;
   }
 
   readSize = maxBlockSize;
 
     if (FileMan_var.readSize < readSize)
     {
       readSize = (UINT16)FileMan_var.readSize;
 
     }
 
     FileMan_var.fileCRC = 0;
     //Calc u32FileCRC.
     CRC32_Calc_Cycle(aFileData, readSize, &FileMan_var.fileCRC);
     /*copy the first foe data block*/
     MEMCPY(pData, aFileData, readSize);
 //    FileMan_var.readFileIndex = 0;
     return readSize;
 }
 
 
 UINT16 FoE_ReadData(UINT32 offset, UINT16 maxBlockSize, UINT16 *pData)
 {
   UINT32 sizeError = 0;
 
     if (FileMan_var.readSize <= offset)
     {
         return 0;
     }
 
     /*get file length to send*/
     sizeError = FileMan_var.readSize - offset;
 
     if (sizeError > maxBlockSize)
     {
         /*transmit max block size if the file data to be send is greater than the max data block*/
         sizeError = maxBlockSize;
 
     }
     else
     {
       if(FileMan_var.readFileRemainderSize > 0)
       {
           MEMCPY(&(((UINT8 *)aFileData)[0]), &(((UINT8 *)aFileData)[offset - FileMan_var.prevReadOffset]), sizeError);
           memset(&(aFileData[sizeError]), 0, FILEMAN_FILE_BUFF_SIZE - sizeError);
 
           FileMan_ReadRemainderSectorFile(sizeError,aFileData);
           FileMan_var.prevReadOffset = offset;
           sizeError = FileMan_var.readSize - offset;
             if (sizeError > maxBlockSize)
             {
                 /*transmit max block size if the file data to be send is greater than the max data block*/
                 sizeError = maxBlockSize;
             }
       }
 
     }
 
     //Calc u32FileCRC.
     CRC32_Calc_Cycle(&(((UINT8 *)aFileData)[offset - FileMan_var.prevReadOffset]), sizeError, &FileMan_var.fileCRC);
     /*copy the foe data block 2 .. n*/
     MEMCPY(pData, &(((UINT8 *)aFileData)[offset - FileMan_var.prevReadOffset]), sizeError);
 
     if(FileMan_var.readFileRemainderSize == 0 && (sizeError == (FileMan_var.readSize - offset)) && (sizeError == 0))//finish to read file.
     {
       if(FileMan_var.fileCRC != FileMan_GetFileCRC(FileMan_var.readFileIndex) ||
           (FileMan_var.readSize != FileMan_GetFileSize(FileMan_var.readFileIndex)) )//check fileCRC and fileSize
       {
         FileMan_flashReadStatus = 1;
         return 0;
       }
     }
 
     return sizeError;
 }
 
 UINT16 FoE_WriteData(UINT16 MBXMEM * pData, UINT16 Size, BOOL bDataFollowing)
 {
   u32 EraseFlashAddress = 0;
   u16 FileBlockflag = 0;
 
   if (FileMan_formatFlag == 1)
   {
     FileMan_formatFlag = 0;
     return 0;
   }
 
   if((!bBootMode) && (Size == 0) && (FileMan_var.writeFileIndex < FILEMAN_MAX_NUM))
   {
     if(0 == FileMan_DelFileData(FileMan_var.writeFileIndex))
     {
       return ECAT_FOE_ERRCODE_ILLEGAL;
     }
     return 0;
   }
 
     if (Size < 0 || (Size == 0 && bBootMode) || ((FileMan_var.writeFileIndex < FILEMAN_MAX_NUM) && bBootMode))
     {
       return ECAT_FOE_ERRCODE_PACKENO;
     }
 
     if (Size)
     {
       MBXMEMCPY(&aFileData, (UINT8 *)pData, Size);
     }
 
     //update write address offset.
     FileMan_var.writeFileOffset += Size;
 
     //First execute function process
   if (firstIn)
   {
     if (FileMan_var.writeFileIndex < FILEMAN_MAX_NUM)
     {
       if (bDataFollowing)//The file size is more than the data written in a single operation by FOE.
       {
         FileBlockflag = 1;//The first part of the file.
       }
       else//The file size is smaller than the data written in a single operation by FOE.
       {FileBlockflag = 2;// The first part of the file, which is also the final part.
        FileMan_var.writeFileSize = FileMan_var.writeFileOffset;
  }

}
else //FWupdate
  {
  if((aFileData[0x30] != 0xAA) || (aFileData[0x31] != 0x99)||
     (aFileData[0x32] != 0x55) || (aFileData[0x33] != 0x66) )
  {
    return ECAT_FOE_ERRCODE_INVALID_FIRMWARE;
  }

  if((aFileData[0x63] != 0) )
  {
    return ECAT_FOE_ERRCODE_INVALID_FIRMWARE;
  }

  for (int PageEar = 0; PageEar < 96; PageEar++) //one earse 64KB, total earse 6MB
  {
    EraseFlashAddress = 64 * 1024 * PageEar + UPDATE_START_ADDRESS;
    Flash_Erase(EraseFlashAddress,CMD_64K_BLK_ERASE_INDEX);
  }

  WriteFlashAddress = UPDATE_START_ADDRESS;
  }

firstIn = FALSE;
}
else
{
  if (FileMan_var.writeFileIndex < FILEMAN_MAX_NUM)//write file data
  {
      if (bDataFollowing == 0)//last part of the file is written
      {
        FileBlockflag = 3;
        FileMan_var.writeFileSize = FileMan_var.writeFileOffset;
      }
      else//2nd,3rd... part of the file is written
      {
        FileBlockflag = 4;
      }
  }
}



if (FileMan_var.writeFileIndex < FILEMAN_MAX_NUM)//write file data
{
  if(FileMan_WtireFileData(FileBlockflag,Size,aFileName,aFileData) == 0)
  {
    return ECAT_FOE_ERRCODE_ILLEGAL;
  }
}
else//FWupdate
{
  //check write address offset before write flash
if(FileMan_var.writeFileOffset >= MAX_FIRMWARE_ADDRESS)
{
  return ECAT_FOE_ERRCODE_INVALID_FIRMWARE;
}

if(0 == Flash_PageWrite(WriteFlashAddress, ((UINT8 *)pData), Size))
{
  return ECAT_FOE_ERRCODE_PROGERROR;
}

WriteFlashAddress = WriteFlashAddress + Size;
}

return 0;
}

UINT16 FoE_Write(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password)
{
if (nameSize < MAX_FILE_NAME_SIZE - 1)
{
    /* for test every file name can be written */
    MBXSTRCPY(aFileName, pName, nameSize);
    MBXSTRCPY(aFileName + nameSize, "\0", 1); //string termination

    FileMan_var.writeFileIndex = 65535;
    FileMan_var.writeFileOffset = 0;
    FileMan_var.writeFileSector = 0;
    FileMan_var.writeFileSize = 0;
    WriteFlashAddress = 0;
    FileMan_var.fileCRC = 0x00000000;
    firstIn = TRUE;

if(!bBootMode)
{
  if(strstr(aFileName, "format") && nameSize == 6)
  {
    FileMan_initDirectory();
    FileMan_formatFlag = 1;
  }
  else if(strstr(aFileName, "dir") && nameSize == 3)
  {
    return ECAT_FOE_ERRCODE_ILLEGAL;
  }
  else
  {
    FileMan_var.writeFileIndex = FileMan_AddFileToDirectory(aFileName,nameSize);

    if (FileMan_var.writeFileIndex > FILEMAN_MAX_NUM)
    {
      return ECAT_FOE_ERRCODE_DISKFULL;
    }
  }
}
else//FW update
{
  FileMan_var.writeFileIndex = 65535;
  if (strstr(aFileName, ".bin") == NULL)
  {
     return ECAT_FOE_ERRCODE_INVALID_FIRMWARE;
  }

      if (password != 0x12345678U)
      {
          return ECAT_FOE_ERRCODE_ILLEGAL;
      }
  WatchDogFeed();
}
    return 0;
}
else
{
    return ECAT_FOE_ERRCODE_DISKFULL;
}
}

void FoE_Init(void)
{
pAPPL_FoeRead = FoE_Read;
pAPPL_FoeWriteData = FoE_WriteData;
pAPPL_FoeReadData = FoE_ReadData;
pAPPL_FoeWrite = FoE_Write;
}