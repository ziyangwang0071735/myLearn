/******* I N C L U D E - F I L E S ********************************************/
#include "FileMan.h"
#include "FileMan_config.h"
#include "Flash_Extern.h"
#include "foeappl.h"
#include"bootloaderappl.h"
#include "CRC32.h"
/******* L O C A L - D E F I N E S ********************************************/




/******* L O C A L - T Y P E S ************************************************/

/******* G L O B A L - D A T A ************************************************/
uint16_t FileMan_flashWriteStatus = 0;
uint16_t FileMan_flashReadStatus = 0;

FILEMAN_VAR_t FileMan_var;

/******* P R I V A T E - D A T A **********************************************/

/******* L O C A L - D A T A **************************************************/

/******* L O C A L - M A C R O S **********************************************/

/******* I M P O R T - F U N C T I O N S - P R O T O T Y P E S ****************/


/******* L O C A L - F U N C T I O N S - P R O T O T Y P E S ******************/

// Initial dir file
void FileMan_initDirectory(void)
{
  FileMan_clrFile();

  //init FileMan_fileSize
  FileMan_fileNum = 0;
  FileMan_sumSectors = 0;
  FileMan_flashWriteStatus = 0;
  FileMan_flashReadStatus = 0;
  memset(FileMan_curSector,0,FILEMAN_MAX_EDCDIRCTORY_SIZE);
}



uint16_t FileMan_GetNextSector(uint16_t StartSector,uint16_t serchNo)
{
  FILEMAN_SECTOR_DIRECTORY_t *dir = (FILEMAN_SECTOR_DIRECTORY_t *)FileMan_curSector;
  for(uint16_t i = StartSector; i < FILEMAN_MAX_NUM; i++)
  {
    if(dir->SectorTable[i] == serchNo)
    {
       return i;
    }
  }

  return -1;

}

uint32_t FileMan_GetNextWriteAddr(uint16_t FileIndex, uint32_t CurWriteAddr)
{
  FILEMAN_SECTOR_DIRECTORY_t *dir = (FILEMAN_SECTOR_DIRECTORY_t *)FileMan_curSector;
  uint16_t StartSector = (CurWriteAddr - FILEMAN_DIRECTORY_END_ADDRESS) / FILEMAN_FLASH_SECTOR_SIZE;

  uint16_t sectorIndex = 65535;

  if(FileMan_FileExist_Flag == 1)
  {
    sectorIndex = FileMan_GetNextSector(StartSector+1,FileIndex+1);
    if(sectorIndex >= FILEMAN_MAX_NUM)
    {
      sectorIndex = FileMan_GetNextSector(StartSector,0);
    }
  }
  else
  {
    sectorIndex = FileMan_GetNextSector(StartSector,0);
  }

  if(sectorIndex < FILEMAN_MAX_NUM)
  {
    dir->SectorTable[sectorIndex] = FileIndex + 1;
    return (FILEMAN_DIRECTORY_END_ADDRESS + sectorIndex * FILEMAN_FLASH_SECTOR_SIZE);
  }
  else
  {
    return 0;
  }

}


uint32_t FileMan_GetFileSize(uint16_t FileIndex)
{
  FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];

  return fileInfo[FileIndex].fileSize;
}

uint32_t FileMan_GetFileCRC(uint16_t FileIndex)
{
  FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];

  return fileInfo[FileIndex].fileCRC;
}

uint32_t FileMan_GetAddress(uint16_t FileIndex)
{
  FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];

  return fileInfo[FileIndex].fileAddress;
}


uint32_t FileMan_ReadFileWithSector(uint32_t readAddress,uint32_t readSize, uint8_t *OutBuf)
{
    uint16_t curReadLen = 0;
    uint32_t curOffset = 0,fileLen = 0,curOffset0 = 0;

  fileLen = readSize;

  curOffset = readAddress;
  curOffset0 = curOffset;

  while (fileLen > 0)
  {
    /* Calculate num bytes to write for this iteration */
    if (fileLen > FILEMAN_FLASH_PAGE_SIZE)
    {
      curReadLen = FILEMAN_FLASH_PAGE_SIZE;
    }
    else
    {
      curReadLen = fileLen;
    }

    //read one page
    Flash_ReadData(curOffset, curReadLen, OutBuf + (curOffset - curOffset0));

    fileLen -= curReadLen;
    curOffset += curReadLen;
  }

  return readSize;
}

uint16_t FileMan_findFileIndex(char  * pFileName,uint16_t nameSize)
{
  FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];
  if(FileMan_FileNameCheck(pFileName,nameSize) == 0)
  {
    return -1;
  }

    for (uint16_t i = 0; i < FileMan_fileNum; i++)
    {
      if (nameSize == strlen(fileInfo[i].fileName) && (strncmp(fileInfo[i].fileName, pFileName,nameSize) == 0))
        {
            return i;
        }
    }

    return -1;
}

void FileMan_initDisk(void)
{
    uint16_t curReadLen = 0;
    uint32_t curOffset = FILEMAN_DIRECTORY_START_ADDRESS,fileLen = FILEMAN_MAX_EDCDIRCTORY_SIZE,curOffset0 = FILEMAN_DIRECTORY_START_ADDRESS;


  FileMan_dirFileSize = 0;
  FileMan_flashWriteStatus = 0;
  FileMan_flashReadStatus = 0;
  FileMan_FileExist_Flag = 0;
  //init
  memset(FileMan_curSector,0,FILEMAN_MAX_EDCDIRCTORY_SIZE);

  //从flash读取dir信息，保存到FileMan_curSector，并计算出共有多少个文件。
  while (fileLen > 0)
  {
    /* Calculate num bytes to write for this iteration */
    if (fileLen > FILEMAN_FLASH_PAGE_SIZE)
    {
      curReadLen = FILEMAN_FLASH_PAGE_SIZE;
    }
    else
    {
      curReadLen = fileLen;
    }

    //read one page
    Flash_ReadData(curOffset, curReadLen, (uint8_t *)FileMan_curSector + (curOffset - curOffset0));

    fileLen -= curReadLen;
    curOffset += curReadLen;
  }
  //Calc u32FileCRC
  uint32_t dirFileCRC = CRC32_Calc((uint8_t *)FileMan_curSector, FILEMAN_MAX_EDCDIRCTORY_SIZE-4);

  FILEMAN_SECTOR_DIRECTORY_t *dir = (FILEMAN_SECTOR_DIRECTORY_t *)FileMan_curSector;
  //todo:Implement file data uniqueness verification using the SHA-256 algorithm.
  if((dir->DirectoryCRC32 == dirFileCRC) && ((dir->TotlaFileNums > 0) && (dir->TotlaFileNums < FILEMAN_MAX_NUM))
      && ((dir->TotlaOcuppySectors < FILEMAN_MAX_NUM) && (dir->TotlaOcuppySectors > 0)))
  {
    FileMan_fileNum = dir->TotlaFileNums;
    FileMan_sumSectors = dir->TotlaOcuppySectors;
  }
  else
  {
    FileMan_fileNum = 0;
    FileMan_sumSectors = 0;
    memset(FileMan_curSector,0,FILEMAN_MAX_EDCDIRCTORY_SIZE);
  }


}



//uint16_t FileMan_saveFile(char  *pFileName,uint16_t nameSize)
uint16_t FileMan_AddFileToDirectory(char  *pFileName,uint16_t nameSize)
{
  uint16_t FileIndex = -1;

    if (FileMan_fileNum >= FILEMAN_MAX_NUM)
    {
        return FileIndex; // No space for new files
    }

  if(FileMan_FileNameCheck(pFileName,nameSize) == 0)
  {
    return FileIndex;
  }

  /* dir*/
  if ( ( ((*pFileName == 'd') && (*(pFileName + 1) == 'i') && (*(pFileName + 2) == 'r')) && (nameSize == 3))
      || ((strncmp(pFileName, "EdcDirectoryContents", 20) == 0) && (nameSize == 20)) )
  {
    return FileIndex;
  }

    FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];
    FILEMAN_SECTOR_DIRECTORY_t *dir = (FILEMAN_SECTOR_DIRECTORY_t *)FileMan_curSector;

    FileIndex = FileMan_findFileIndex(pFileName,nameSize);
    if (FileIndex < FileMan_fileNum)
    {
      FileMan_FileExist_Flag = 1;
        return FileIndex;
    }

    //seach min free sector no, as the first address of new file.
    uint16_t sectorIndex = FileMan_SearchMinSector(dir,0);//serach min free sector
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




int16_t FileMan_writeDirectoryToDisk(char  *pFileName, uint16_t FileIndex, uint32_t FileSize, uint32_t FileCRC)
{

  if(FileIndex > FILEMAN_MAX_NUM)
  {
    return -1;
  }

  uint16_t sectorTemp1 = 0,sectorTemp2 = 0;
  FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];
  FILEMAN_SECTOR_DIRECTORY_t *dir = (FILEMAN_SECTOR_DIRECTORY_t *)FileMan_curSector;

  if(FileMan_FileExist_Flag == 1)
  {
    sectorTemp1 = (fileInfo[FileIndex].fileSize / FILEMAN_FLASH_SECTOR_SIZE);
    sectorTemp2 = (FileSize / FILEMAN_FLASH_SECTOR_SIZE);
    if(sectorTemp2 < sectorTemp1)
    {
      uint16_t StartSector = (fileInfo[FileIndex].fileAddress - FILEMAN_DIRECTORY_END_ADDRESS) / FILEMAN_FLASH_SECTOR_SIZE;
      for(uint16_t i = 0; i < (FILEMAN_MAX_NUM - StartSector); i++)
      {
        uint16_t SerchRSector = FileMan_GetNextSector(i + StartSector+1,FileIndex+1);
        if(SerchRSector < FILEMAN_MAX_NUM && i >= sectorTemp2)
        {
          dir->SectorTable[SerchRSector] = 0;
        }
      }
    }

    FileMan_FileExist_Flag = 0;
  }
  else
  {
    FileMan_fileNum=FileMan_fileNum+1;
  }

  fileInfo[FileIndex].fileSize = FileSize;
  fileInfo[FileIndex].fileCRC = FileCRC;
  FileMan_sumSectors = FileMan_GetSumSectors(dir);

  dir->TotlaOcuppySectors = FileMan_sumSectors;
  dir->TotlaFileNums = FileMan_fileNum;

    //Calc u32FileCRC.
  uint32_t dirFileCRC = CRC32_Calc((uint8_t *)FileMan_curSector, FILEMAN_MAX_EDCDIRCTORY_SIZE-4);

    if(dir->DirectoryCRC32 != dirFileCRC )
    {
      dir->DirectoryCRC32 = CRC32_Calc((uint8_t *)FileMan_curSector, FILEMAN_MAX_EDCDIRCTORY_SIZE-4);
    }

  Flash_Erase(FILEMAN_DIRECTORY_START_ADDRESS,CMD_SECTOR_ERASE_INDEX);
  if(0 == Flash_PageWrite(FILEMAN_DIRECTORY_START_ADDRESS, (uint8_t *)FileMan_curSector, FILEMAN_MAX_EDCDIRCTORY_SIZE))
  {
    FileMan_flashWriteStatus = 4;
    return -1;
  }


  return 0;
}


uint16_t FileMan_ReadFirstSectorFile(char *pFileName,uint16_t nameSize, uint8_t *OutBuf)
{
  uint32_t readSizeTemp = 0;

  /* dir */
  if ( ( ((*pFileName == 'd') && (*(pFileName + 1) == 'i') && (*(pFileName + 2) == 'r')) && (nameSize == 3))
      || ((strncmp(pFileName, "EdcDirectoryContents", 20) == 0) && (nameSize == 20)))
  {
    FileMan_listFiles((char *)OutBuf);
    FileMan_var.readSize = FileMan_dirFileSize;
  }
  else
  {
    FileMan_var.readFileIndex = FileMan_findFileIndex(pFileName,nameSize);
    if(FileMan_var.readFileIndex > FILEMAN_MAX_NUM)//File directory read error.
    {
      return FALSE;
    }
    else//File directory read correct.
    {
        FileMan_var.readFileAddress = FileMan_GetAddress(FileMan_var.readFileIndex);
        if(FileMan_GetFileSize(FileMan_var.readFileIndex) > FILEMAN_FLASH_SECTOR_SIZE)
        {
          readSizeTemp = FILEMAN_FLASH_SECTOR_SIZE;
          FileMan_var.readFileRemainderSize = FileMan_GetFileSize(FileMan_var.readFileIndex) - FILEMAN_FLASH_SECTOR_SIZE;
        }
        else
        {
          readSizeTemp = FileMan_GetFileSize(FileMan_var.readFileIndex);
          FileMan_var.readFileRemainderSize = 0;
        }
        FileMan_var.fileCRC = 0;
        FileMan_var.readSize = FileMan_ReadFileWithSector(FileMan_var.readFileAddress,readSizeTemp,OutBuf);
        FileMan_var.readFileAddress += FileMan_var.readSize ;//update readFileAddress
    }
  }

  return TRUE;

}


void FileMan_ReadRemainderSectorFile(uint32_t hadReadSize, uint8_t *OutBuf)
{
  uint32_t readSize = 0;

  if((FileMan_var.readFileRemainderSize + hadReadSize) > FILEMAN_FILE_BUFF_SIZE)
  {
    readSize = FILEMAN_FLASH_SECTOR_SIZE;
    FileMan_var.readFileRemainderSize = FileMan_var.readFileRemainderSize - readSize;
  }
  else
  {
    readSize = FileMan_var.readFileRemainderSize;
    FileMan_var.readFileRemainderSize = 0;
  }

  FileMan_var.readSize += FileMan_ReadFileWithSector(FileMan_var.readFileAddress,readSize,&OutBuf[hadReadSize]);
  FileMan_var.readFileAddress += readSize;
}


uint16_t FileMan_WtireFileData(uint16_t WriteBlock,uint16_t Size,char *FileName,uint8_t *WriteBuf)
{
  static uint32_t WriteFileAddress = 0;

  if((FileMan_var.writeFileIndex >= FILEMAN_MAX_NUM) || (WriteBlock <= 0) || (WriteBlock > 4))
  {
    return FALSE;
  }

  if(Size == 0)
  {
      if(0 != FileMan_writeDirectoryToDisk(FileName, FileMan_var.writeFileIndex, Size, FileMan_var.fileCRC))
      {
        return FALSE;
      }

    return 0;
  }

  if(WriteBlock == 1 || WriteBlock == 2)//If it is writing the first part of the file, it need to find the initial address of the file.
  {
      FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];
      if(fileInfo[FileMan_var.writeFileIndex].fileAddress < FILEMAN_DIRECTORY_END_ADDRESS || fileInfo[FileMan_var.writeFileIndex].fileAddress > FILEMAN_OCCUPY_END_ADDRESS)
      {
        return FALSE;
      }

      WriteFileAddress = fileInfo[FileMan_var.writeFileIndex].fileAddress;
    Flash_Erase(WriteFileAddress,CMD_SECTOR_ERASE_INDEX);
  }

  //write file data to flash.
  {
    uint32_t sizeLimit = FILEMAN_FLASH_SECTOR_SIZE * (FileMan_var.writeFileSector + 1);
        if(FileMan_var.writeFileOffset > sizeLimit)//file size more than one sector
        {
        if(0 == Flash_PageWrite(WriteFileAddress, WriteBuf, (Size - (FileMan_var.writeFileOffset - sizeLimit)) ))
        {
          FileMan_flashWriteStatus = 1;
          return FALSE;
        }

        uint32_t addressOffset = FileMan_GetNextWriteAddr(FileMan_var.writeFileIndex , WriteFileAddress);

          if(addressOffset >= FILEMAN_DIRECTORY_END_ADDRESS && addressOffset <= FILEMAN_OCCUPY_END_ADDRESS)
          {
            WriteFileAddress = addressOffset;
            Flash_Erase(WriteFileAddress,CMD_SECTOR_ERASE_INDEX);

            if(0 == Flash_PageWrite(WriteFileAddress, &WriteBuf[(Size - (FileMan_var.writeFileOffset - sizeLimit))], (FileMan_var.writeFileOffset - sizeLimit) ))
            {
              FileMan_flashWriteStatus = 2;
              return FALSE;
            }
            WriteFileAddress = WriteFileAddress + (FileMan_var.writeFileOffset - sizeLimit);
            FileMan_var.writeFileSector++;
          }
          else
          {
            WriteFileAddress = WriteFileAddress + (Size - (FileMan_var.writeFileOffset - sizeLimit));
            return FALSE;
          }
        }
        else//file size less than one sector
        {
        if(0 == Flash_PageWrite(WriteFileAddress, WriteBuf, Size))
        {
          FileMan_flashWriteStatus = 3;
          return FALSE;
        }
        WriteFileAddress = WriteFileAddress + Size;
        }

        //Calc u32FileCRC.
        CRC32_Calc_Cycle(WriteBuf, Size, &FileMan_var.fileCRC);
  }

  if(WriteBlock == 2 || WriteBlock == 3)//last part of the file, update file directory content.
  {
      if(0 != FileMan_writeDirectoryToDisk(FileName, FileMan_var.writeFileIndex, FileMan_var.writeFileSize, FileMan_var.fileCRC))
      {
        return FALSE;
      }
  }


  return TRUE;
}


uint8_t FileMan_DelFileData(uint16_t FileIndex)
{
  if(FileIndex >= FILEMAN_MAX_NUM)
  {
    return FALSE;
  }
  FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];
  FILEMAN_SECTOR_DIRECTORY_t *dir = (FILEMAN_SECTOR_DIRECTORY_t *)FileMan_curSector;
  uint16_t sectorIndex = 0;
  int16_t occupySector = ((fileInfo[FileIndex].fileSize + (FILEMAN_FLASH_SECTOR_SIZE - 1)) / FILEMAN_FLASH_SECTOR_SIZE);

  do
  {
    sectorIndex =FileMan_GetNextSector(sectorIndex,FileIndex+1);
    dir->SectorTable[sectorIndex] = 0;
    memcpy(&(dir->SectorTable[sectorIndex]),&(dir->SectorTable[sectorIndex + 1]),FILEMAN_MAX_NUM - sectorIndex - 1);
    occupySector--;
    dir->TotlaOcuppySectors--;
    FileMan_sumSectors--;
  }while(occupySector > 0);


  if(FileMan_fileNum == (FileIndex+1))
  {
    memset(&(fileInfo[FileIndex].fileName),0,sizeof(FILEMAN_FILEINFO_t));
  }
  else
  {
    for(uint16_t i = 0; i < (FileMan_fileNum - FileIndex - 1); i++)
    {
      memcpy(&(fileInfo[FileIndex + i].fileName),&(fileInfo[FileIndex + i + 1].fileName),sizeof(FILEMAN_FILEINFO_t));
    }
  }
  dir->TotlaFileNums--;
  FileMan_fileNum--;

    //Calc u32FileCRC.
  uint32_t dirFileCRC = CRC32_Calc((uint8_t *)FileMan_curSector, FILEMAN_MAX_EDCDIRCTORY_SIZE-4);

    if(dir->DirectoryCRC32 != dirFileCRC )
    {
      dir->DirectoryCRC32 = CRC32_Calc((uint8_t *)FileMan_curSector, FILEMAN_MAX_EDCDIRCTORY_SIZE-4);
    }
    FileMan_FileExist_Flag = 0;
  Flash_Erase(FILEMAN_DIRECTORY_START_ADDRESS,CMD_SECTOR_ERASE_INDEX);
  if(0 == Flash_PageWrite(FILEMAN_DIRECTORY_START_ADDRESS, (uint8_t *)FileMan_curSector, FILEMAN_MAX_EDCDIRCTORY_SIZE))
  {
    FileMan_flashWriteStatus = 5;
    return FALSE;
  }

  return TRUE;
}

/******* G L O B A L - F U N C T I O N S **************************************/