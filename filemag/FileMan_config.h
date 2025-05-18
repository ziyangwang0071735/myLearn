/******* I N C L U D E - F I L E S ********************************************/
#include "syst.h"
#include "FileMan_Extern.h"

/******* G L O B A L - D E F I N E S ******************************************/

/******* G L O B A L - T Y P E S **********************************************/


/******* G L O B A L - D A T A ************************************************/
extern uint16_t FileMan_fileNum;
extern uint32_t FileMan_sumSectors;
extern uint16_t FileMan_FileExist_Flag;
extern uint16_t FileMan_dirFileSize;
extern char FileMan_curSector[FILEMAN_MAX_EDCDIRCTORY_SIZE];
/******* G L O B A L - M A C R O S ********************************************/

/******* G L O B A L - F U N C T I O N S - P R O T O T Y P E S ****************/
uint16_t FileMan_FileNameCheck(char  *pFileName,uint16_t nameSize);
uint16_t FileMan_SearchMinSector(FILEMAN_SECTOR_DIRECTORY_t *dir,uint16_t serchNo);
uint16_t FileMan_GetSumSectors(FILEMAN_SECTOR_DIRECTORY_t *dir);
void FileMan_clrFile(void);
void FileMan_AddaFileINDirEnd(char  *pFileName, uint16_t FileIndex, uint32_t FileSize,char *OutBuf);
void FileMan_UpdateTotalFreeINDirectory(char *OutBuf);
void FileMan_listFiles(char *OutBuf);
#endif  // FILEMAN_CONFIG_H
/* ***** E N D ***** (IFileMan_config.h) *********************************************/
/** @}
 */