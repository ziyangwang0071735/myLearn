/******************************************************************************
* @F_NAME :          fifoa.h
* @F_PURPOSE :       Purpose
* @F_CREATED_BY :    Shengping Wang
* @F_CREATION_DATE : 2015-08-26
* @F_LANGUAGE :      C
* @F_MPROC_TYPE :   Both endian
*************************************** (C) Copyright 2015 Midea Group   *****/

#ifndef FIFOA_H
#define FIFOA_H

/*______ I N C L U D E - F I L E S ___________________________________________*/
#include "Hal_Type.h"

/*______ G L O B A L - D E F I N E S _________________________________________*/

/*______ G L O B A L - T Y P E S _____________________________________________*/
typedef struct
{
    uint8_t* buff;
    uint8_t* buffEnd;
    uint16_t buffLength;
    uint8_t* nextWriteIndex;
    uint8_t* nextReadIndex;
} FIFOA_Buff_t;
/*______ G L O B A L - D A T A _______________________________________________*/

/*______ G L O B A L - M A C R O S ___________________________________________*/

/*______ G L O B A L - F U N C T I O N S - P R O T O T Y P E S _______________*/
extern uint8_t FIFOA_Initial(FIFOA_Buff_t* fifo, uint8_t* buff, uint16_t len);
extern uint16_t FIFOA_Read(FIFOA_Buff_t* srcFIFO, uint8_t* dstBuff, uint16_t len);
extern uint16_t FIFOA_Write(FIFOA_Buff_t* dstFIFO, uint8_t* srcBuff, uint16_t len);
extern uint16_t FIFOA_GetCurLength(FIFOA_Buff_t* fifo);
extern uint8_t* FIFOA_GetHead(FIFOA_Buff_t* fifo);
extern uint16_t FIFOA_ForceWrite(FIFOA_Buff_t* dstFIFO, uint8_t* srcBuff, uint16_t len);

extern void FIFOA_GetDataSector(FIFOA_Buff_t* fifo, uint8_t** sector1, uint16_t* length1, uint8_t** sector2, uint16_t* length2);

#endif /* FIFOA_H */

/* _____ E N D _____ (fifoa.h) ____________________________________________*/