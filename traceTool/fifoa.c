/******************************************************************************
* @F_NAME :          fifoa.c
* @F_PURPOSE :       Purpose
* @F_CREATED_BY :    Shengping Wang
* @F_CREATION_DATE : 2015-08-26
* @F_LANGUAGE :      C
* @F_MPROC_TYPE :   Both endian
*************************************** (C) Copyright 2015 Midea Group   *****/
/*______ I N C L U D E - F I L E S ___________________________________________*/
#include "Hal_Type.h"
#include "fifoa.h"
/*______ L O C A L - D E F I N E S ___________________________________________*/

/*______ L O C A L - T Y P E S _______________________________________________*/

/*______ G L O B A L - D A T A _______________________________________________*/

/*______ P R I V A T E - D A T A _____________________________________________*/

/*______ L O C A L - D A T A _________________________________________________*/

/*______ L O C A L - M A C R O S _____________________________________________*/

/*______ I M P O R T - F U N C T I O N S - P R O T O T Y P E S _______________*/

/*______ L O C A L - F U N C T I O N S - P R O T O T Y P E S _________________*/

/*______ G L O B A L - F U N C T I O N S _____________________________________*/
/* when define buff, the buff size should be max element+1 */

uint8_t FIFOA_Initial(FIFOA_Buff_t* fifo, uint8_t* buff, uint16_t len)
{
    fifo->buff = buff;
    fifo->buffEnd = buff + len - 1;
    fifo->buffLength = len;
    fifo->nextReadIndex = fifo->buff;
    fifo->nextWriteIndex = fifo->buff;
    return 0;
}
uint16_t FIFOA_GetCurLength(FIFOA_Buff_t* fifo)
{
    return (fifo->nextWriteIndex + fifo->buffLength - fifo->nextReadIndex) % (fifo->buffLength);
}
uint16_t FIFOA_Read(FIFOA_Buff_t* srcFIFO, uint8_t* dstBuff, uint16_t len)
{
    uint16_t readLen = 0;
    while (srcFIFO->nextReadIndex != srcFIFO->nextWriteIndex && readLen < len)
    {
        *(dstBuff + readLen) = *(srcFIFO->nextReadIndex);
        readLen++;
        if (srcFIFO->nextReadIndex == srcFIFO->buffEnd)
            srcFIFO->nextReadIndex = srcFIFO->buff;
        else
            srcFIFO->nextReadIndex++;
    }
    return readLen;
}

uint8_t* FIFOA_GetHead(FIFOA_Buff_t* fifo)
{
    return fifo->nextReadIndex;
}

void FIFOA_GetDataSector(FIFOA_Buff_t* fifo, uint8_t** sector1, uint16_t* length1, uint8_t** sector2, uint16_t* length2)
{
    *sector1 = (fifo->nextReadIndex);
    if (fifo->nextWriteIndex >= fifo->nextReadIndex)
    {
        *length1 = fifo->nextWriteIndex - fifo->nextReadIndex;
        sector2 = 0;
        *length2 = 0;
    }
    else
    {
        *length1 = fifo->buffEnd - fifo->nextReadIndex + 1;
        *sector2 = (fifo->buff);
        *length2 = fifo->nextWriteIndex - fifo->buff;
    }
}

uint16_t FIFOA_ForceWrite(FIFOA_Buff_t* dstFIFO, uint8_t* srcBuff, uint16_t len)
{
    uint16_t writeLen = 0;
    uint8_t fullFlag = 0;
    while (writeLen < len)
    {
        if (dstFIFO->nextReadIndex == dstFIFO->buff)
        {
            if (dstFIFO->nextWriteIndex == dstFIFO->buffEnd)
            {
                fullFlag = 1;
            }
        }
        else
        {
            if (dstFIFO->nextWriteIndex == dstFIFO->nextReadIndex - 1)
            {
                fullFlag = 1;
            }
        }
        if (fullFlag)
        {
            if (dstFIFO->nextReadIndex == dstFIFO->buffEnd)
            {
                dstFIFO->nextReadIndex = dstFIFO->buff;
            }
            else
            {
                dstFIFO->nextReadIndex++;
            }
        }
        *(dstFIFO->nextWriteIndex) = __byte(srcBuff,  writeLen);
        writeLen++;
        if (dstFIFO->nextWriteIndex == dstFIFO->buffEnd)
            dstFIFO->nextWriteIndex = dstFIFO->buff;
        else
            dstFIFO->nextWriteIndex++;
    }
    return writeLen;
}
/*______ P R I V A T E - F U N C T I O N S ___________________________________*/

/*______ L O C A L - F U N C T I O N S _______________________________________*/

/*______ E N D _____ (fifoa.c) ____________________________________________*/