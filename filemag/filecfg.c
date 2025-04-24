/******* I N C L U D E - F I L E S ********************************************/
#include "FileMan_config.h"
#include "Flash_extern.h"
#include"bootloaderappl.h"
/******* L O C A L - D E F I N E S ********************************************/
#define FILEMAN_LINE_BYTES  64
#define FILEMAN_SECTOR_POS_IN_LINE 53
#define FILEMAN_BYTES_POS_IN_LINE 34
#define FILEMAN_TOTAL_FILES_IN_LINE 12
#define FILEMAN_DIRHEADER_BYTES (98)
/******* L O C A L - T Y P E S ************************************************/

/******* G L O B A L - D A T A ************************************************/
uint16_t FileMan_dirFileSize = 0;
char FileMan_curSector[FILEMAN_MAX_EDCDIRCTORY_SIZE];
/******* P R I V A T E - D A T A **********************************************/

/******* L O C A L - D A T A **************************************************/
uint16_t FileMan_fileNum = 0;
uint32_t FileMan_sumSectors = 0;
uint16_t FileMan_FileExist_Flag = 0;
uint16_t FileMan_formatFlag = 0;
const char *FileMan_DirHeader = "Kuka-EDC-FS V1.0.2-0(MAX FILE NAME LEN: 30)\n"
                             "Card: KUKA ID-Card (Flash: W25Q128,Sector: 4K bytes)\n\n";

const char *FileMan_FileLine  = "                                  4.096 bytes in      1 sectors\n";

const char *FileMan_DirFooter = "_______________________________________________________________\n"
                "Total        1 file(s)            4.096 bytes in      1 sectors\n"
                "Free        30 file(s)          122.880 bytes in     30 sectors\n";

/******* L O C A L - M A C R O S **********************************************/

/******* I M P O R T - F U N C T I O N S - P R O T O T Y P E S ****************/

/******* L O C A L - F U N C T I O N S - P R O T O T Y P E S ******************/

/******* G L O B A L - F U N C T I O N S **************************************/

/******* P R I V A T E - F U N C T I O N S ************************************/

/******* L O C A L - F U N C T I O N S ****************************************/

uint16_t FileMan_FileNameCheck(char  *pFileName,uint16_t nameSize)
{

  if(nameSize > FILEMAN_NAME_MAX_LEN)
  {
    return FALSE;
  }

  if(strstr(pFileName, " "))
  {
    return FALSE;
  }

  if(strstr(pFileName, "\n"))
  {
    return FALSE;
  }

  if(strstr(pFileName, "\t"))
  {
    return FALSE;
  }

  return TRUE;
}

uint16_t FileMan_SearchMinSector(FILEMAN_SECTOR_DIRECTORY_t *dir,uint16_t serchNo)
{
  for(uint16_t i = 0; i < FILEMAN_MAX_NUM; i++)
  {
    if(dir->SectorTable[i] == serchNo)
    {
       return i;
    }
  }

  return -1;

}


uint16_t FileMan_GetSumSectors(FILEMAN_SECTOR_DIRECTORY_t *dir)
{

  uint16_t sumSectors = 0;
  for(uint16_t i = 0; i < FILEMAN_MAX_NUM; i++)
  {
    if(dir->SectorTable[i])
    {
      sumSectors += 1;
    }
  }

  return sumSectors;

}

void FileMan_clrFile(void)
{
  //Flash_Erase(FILEMAN_DIRECTORY_START_ADDRESS,CMD_SECTOR_ERASE_INDEX);
  uint16_t i;
  i = writeData_Eep(msg_clr);
}


void FileMan_AddaFileINDirEnd(char  *pFileName, uint16_t FileIndex, uint32_t FileSize,char *OutBuf)
{
  uint16_t NumOfBit[6] = {0,0,0,0,0,0};
  int16_t i = 0,j = 0,InsertPos = 0;
  uint16_t nameSize = 0;


  //1.add new line in the directory file for write file
  InsertPos = FileMan_dirFileSize;
  memcpy(&(OutBuf[InsertPos]), FileMan_FileLine, FILEMAN_LINE_BYTES);

  nameSize = strlen(pFileName);
  for(i = 0; i < nameSize; i++)
  {
    OutBuf[InsertPos + i] = *(pFileName + i);
  }
  FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];
  //update fileSize,file occupy sectors
  NumOfBit[0] = fileInfo[FileIndex].fileSize / 100000;
  NumOfBit[1] = fileInfo[FileIndex].fileSize / 10000 % 10;
  NumOfBit[2] = fileInfo[FileIndex].fileSize / 1000 % 10;
  NumOfBit[3] = fileInfo[FileIndex].fileSize / 100 % 10;
  NumOfBit[4] = fileInfo[FileIndex].fileSize / 10 % 10;
  NumOfBit[5] = fileInfo[FileIndex].fileSize % 10;
  j=0;
  for (i = (InsertPos + FILEMAN_BYTES_POS_IN_LINE - 2); i < (InsertPos + FILEMAN_BYTES_POS_IN_LINE + 5); i++)
  {
    if(j < 6)
    {
      if(i != (InsertPos + FILEMAN_BYTES_POS_IN_LINE + 1))
      {
        if((NumOfBit[j]) || (j == 1 && NumOfBit[0]) || (j == 2 && (NumOfBit[0] || NumOfBit[1]))
            || (j == 3 && (NumOfBit[0] || NumOfBit[1] || NumOfBit[2]))
            || (j == 4 && (NumOfBit[0] || NumOfBit[1] || NumOfBit[2] || NumOfBit[3])) || (j == 5))
        {
          OutBuf[i] = NumOfBit[j] + 0x30;
        }
        else
        {
          OutBuf[i] = ' ';
        }

        j++;
      }
      else
      {
        if(NumOfBit[0] == 0 && NumOfBit[1] == 0 && NumOfBit[2] == 0)
        {
          OutBuf[i] = ' ';
        }
        else
        {
          OutBuf[i] = '.';
        }
      }
    }
  }

  //update file occupy sectors
  uint16_t occupySector = ((fileInfo[FileIndex].fileSize + (FILEMAN_FLASH_SECTOR_SIZE - 1)) / FILEMAN_FLASH_SECTOR_SIZE);
  NumOfBit[0] = (occupySector) / 10 % 10;
  NumOfBit[1] = (occupySector) % 10;
  j = 0;
  for (i = (InsertPos + FILEMAN_SECTOR_POS_IN_LINE); i < (InsertPos + FILEMAN_SECTOR_POS_IN_LINE + 2); i++)
  {
    if(j < 2)
    {
      if((NumOfBit[j]) || (j == 1))
      {
        OutBuf[i] = NumOfBit[j] + 0x30;
      }
      else
      {
        OutBuf[i] = ' ';
      }

      j++;
    }
  }

  FileMan_dirFileSize += FILEMAN_LINE_BYTES;
}


void FileMan_UpdateTotalFreeINDirectory(char *OutBuf)
{
  uint16_t NumOfBit[6] = {0,0,0,0,0,0};
  int16_t i = 0,j = 0,InsertPos = 0;

  //1.update total file nums(max files is 2 bits),total file size, total file occupy sectors
  InsertPos = FileMan_dirFileSize;
  memcpy(&(OutBuf[InsertPos]), FileMan_DirFooter, 3*FILEMAN_LINE_BYTES);
  FileMan_dirFileSize = FileMan_dirFileSize + 3*FILEMAN_LINE_BYTES;

  //update total file nums
  InsertPos = InsertPos + FILEMAN_LINE_BYTES;
  //High bits:
  if(FileMan_fileNum / 10)
  {
    OutBuf[InsertPos + FILEMAN_TOTAL_FILES_IN_LINE] = ((FileMan_fileNum / 10)+0x30);
  }
  else
  {
    OutBuf[InsertPos + FILEMAN_TOTAL_FILES_IN_LINE] = ' ';
  }
  //Low bits:
  OutBuf[InsertPos + FILEMAN_TOTAL_FILES_IN_LINE + 1] = ((FileMan_fileNum % 10)+0x30);

  //update total fileSize
  NumOfBit[0] = (FileMan_sumSectors * FILEMAN_FLASH_SECTOR_SIZE) / 100000;
  NumOfBit[1] = (FileMan_sumSectors * FILEMAN_FLASH_SECTOR_SIZE) / 10000 % 10;
  NumOfBit[2] = (FileMan_sumSectors * FILEMAN_FLASH_SECTOR_SIZE) / 1000 % 10;
  NumOfBit[3] = (FileMan_sumSectors * FILEMAN_FLASH_SECTOR_SIZE) / 100 % 10;
  NumOfBit[4] = (FileMan_sumSectors * FILEMAN_FLASH_SECTOR_SIZE) / 10 % 10;
  NumOfBit[5] = (FileMan_sumSectors * FILEMAN_FLASH_SECTOR_SIZE) % 10;
  j = 0;
  for (i = (InsertPos + FILEMAN_BYTES_POS_IN_LINE - 2); i < (InsertPos + FILEMAN_BYTES_POS_IN_LINE + 5); i++)
  {
    if(j < 6)
    {
      if(i != (InsertPos + FILEMAN_BYTES_POS_IN_LINE + 1))
      {
        if((NumOfBit[j]) || (j == 1 && NumOfBit[0]) || (j == 2 && (NumOfBit[0] || NumOfBit[1]))
            || (j == 3 && (NumOfBit[0] || NumOfBit[1] || NumOfBit[2]))
            || (j == 4 && (NumOfBit[0] || NumOfBit[1] || NumOfBit[2] || NumOfBit[3])) || (j == 5))
        {
          OutBuf[i] = NumOfBit[j] + 0x30;
        }
        else
        {
          OutBuf[i] = ' ';
        }

        j++;
      }
      else
      {
        if(NumOfBit[0] == 0 && NumOfBit[1] == 0 && NumOfBit[2] == 0)
        {
          OutBuf[i] = ' ';
        }
        else
        {
          OutBuf[i] = '.';
        }
      }
    }
  }

  //update total sectors
  NumOfBit[0] = (FileMan_sumSectors) / 10 % 10;
  NumOfBit[1] = (FileMan_sumSectors) % 10;
  j = 0;
  for (i = (InsertPos + FILEMAN_SECTOR_POS_IN_LINE); i < (InsertPos + FILEMAN_SECTOR_POS_IN_LINE + 2); i++)
  {
    if(j < 2)
    {

      if((NumOfBit[j]) || (j == 1))
      {
        OutBuf[i] = NumOfBit[j] + 0x30;
      }
      else
      {
        OutBuf[i] = ' ';
      }

      j++;
    }
  }


  //2.update free file nums(max files is 2 bits), free flash size, free flash  sectors
  uint16_t freeSectors = FILEMAN_MAX_NUM - FileMan_sumSectors;
  InsertPos = InsertPos + FILEMAN_LINE_BYTES;

  //High bits:
  if(freeSectors / 10)
  {
    OutBuf[InsertPos + FILEMAN_TOTAL_FILES_IN_LINE] = ((freeSectors / 10)+0x30);
  }
  else
  {
    OutBuf[InsertPos + FILEMAN_TOTAL_FILES_IN_LINE] = ' ';
  }
  //Low bits:
  OutBuf[InsertPos + FILEMAN_TOTAL_FILES_IN_LINE + 1] = ((freeSectors % 10)+0x30);

  //update free file Size
  uint32_t freeSize = (freeSectors) * FILEMAN_FLASH_SECTOR_SIZE;
  NumOfBit[0] = (freeSize) / 100000;
  NumOfBit[1] = (freeSize) / 10000 % 10;
  NumOfBit[2] = (freeSize) / 1000 % 10;
  NumOfBit[3] = (freeSize) / 100 % 10;
  NumOfBit[4] = (freeSize) / 10 % 10;
  NumOfBit[5] = (freeSize) % 10;
  j = 0;
  for (i = (InsertPos + FILEMAN_BYTES_POS_IN_LINE - 2); i < (InsertPos + FILEMAN_BYTES_POS_IN_LINE + 5); i++)
  {
    if(j < 6)
    {
      if(i != (InsertPos + FILEMAN_BYTES_POS_IN_LINE + 1))
      {
        if((NumOfBit[j]) || (j == 1 && NumOfBit[0]) || (j == 2 && (NumOfBit[0] || NumOfBit[1]))
            || (j == 3 && (NumOfBit[0] || NumOfBit[1] || NumOfBit[2]))
            || (j == 4 && (NumOfBit[0] || NumOfBit[1] || NumOfBit[2] || NumOfBit[3])) || (j == 5))
        {
          OutBuf[i] = NumOfBit[j] + 0x30;
        }
        else
        {
          OutBuf[i] = ' ';
        }

        j++;
      }
      else
      {
        if(NumOfBit[0] == 0 && NumOfBit[1] == 0 && NumOfBit[2] == 0)
        {
          OutBuf[i] = ' ';
        }
        else
        {
          OutBuf[i] = '.';
        }
      }
    }
  }

  //update free sectors
  NumOfBit[0] = (freeSectors) / 10 % 10;
  NumOfBit[1] = (freeSectors) % 10;
  j = 0;
  for (i = (InsertPos + FILEMAN_SECTOR_POS_IN_LINE); i < (InsertPos + FILEMAN_SECTOR_POS_IN_LINE + 2); i++)
  {
    if(j < 2)
    {
      if((NumOfBit[j]) || (j == 1))
      {
        OutBuf[i] = NumOfBit[j] + 0x30;
      }
      else
      {
        OutBuf[i] = ' ';
      }
      j++;
    }
  }

}


void FileMan_listFiles(char *OutBuf)
{
  uint16_t i = 0;
  memcpy(OutBuf,FileMan_DirHeader,FILEMAN_DIRHEADER_BYTES);FileMan_dirFileSize = FILEMAN_DIRHEADER_BYTES;//44+54;
  FILEMAN_FILEINFO_t *fileInfo = (FILEMAN_FILEINFO_t *)&FileMan_curSector[FILEMAN_READ_FILEINFO_OFFSET];
  for(i=0;i<FileMan_fileNum;i++)
  {
    FileMan_AddaFileINDirEnd(fileInfo[i].fileName,i,fileInfo[i].fileSize,OutBuf);
  }

  FileMan_UpdateTotalFreeINDirectory(OutBuf);
}

/******* E N D ***** (FileMan_config.c) *********************************************/
/** @}
 */