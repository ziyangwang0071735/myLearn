/******************************************************************************
* @F_NAME :          trace_config.h
* @F_PURPOSE :       Purpose
* @F_CREATED_BY :    Shengping Wang
* @F_CREATION_DATE : 2015-08-26
* @F_LANGUAGE :      C
* @F_MPROC_TYPE :   Both endian
*************************************** (C) Copyright 2015 Midea Group   *****/

#ifndef TRACE_CONFIG_H
#define TRACE_CONFIG_H

/*______ I N C L U D E - F I L E S ___________________________________________*/
#include "Hal_Type.h"
#include "fifoa.h"

/*______ G L O B A L - D E F I N E S _________________________________________*/

#define TRACE_TIMEOUT 10U
/*______ G L O B A L - T Y P E S _____________________________________________*/

/*______ G L O B A L - D A T A _______________________________________________*/

/*______ G L O B A L - M A C R O S ___________________________________________*/

/*______ G L O B A L - F U N C T I O N S - P R O T O T Y P E S _______________*/
extern void TRACE_StartMSTimer(uint16_t value);
extern uint8_t TRACE_GetTimeoutEvent(void);

extern uint8_t TRACE_Initial(uint16_t unitlen);
extern uint16_t TRACE_Read(uint8_t* dstBuff, uint16_t len);
extern uint16_t TRACE_Write(uint8_t* srcBuff, uint16_t len);
extern uint16_t TRACE_GetCurLength(void);
extern uint8_t* TRACE_GetHead(void);

extern uint8_t TRACE_SendData(uint8_t* buff, uint16_t len, uint8_t type);
extern void TRACE_GetDataFIFOSector(uint8_t** sector1, uint16_t* length1, uint8_t** sector2, uint16_t* length2);

uint8_t TRACE_GetP1Status(void);
uint8_t TRACE_GetP2Status(void);
void TRACE_SendP1Data(void);
void TRACE_SendP2Data(void);
extern uint8_t TRACE_GetDataGroup(void);

extern uint16_t TRACE_ForceWrite(uint8_t* srcBuff, uint16_t len);

extern uint8_t TRACE_SendFrameInFIFO(void);
extern uint8_t TRACE_GetAutoPollingFlag(void);
extern int8_t TRACE_WritePollingHead(uint8_t len);

#endif /* TRACE_CONFIG_H */

/* _____ E N D _____ (trace_config.h) ____________________________________________*/