/******************************************************************************
* @F_NAME :          trace.h
* @F_PURPOSE :       Purpose
* @F_CREATED_BY :    Shengping Wang
* @F_CREATION_DATE : 2015-08-26
* @F_LANGUAGE :      C
* @F_MPROC_TYPE :   Both endian
*************************************** (C) Copyright 2015 Midea Group   *****/

#ifndef TRACE_H
#define TRACE_H

/*______ I N C L U D E - F I L E S ___________________________________________*/
#include "Hal_Type.h"

/*______ G L O B A L - D E F I N E S _________________________________________*/

/*______ G L O B A L - T Y P E S _____________________________________________*/
typedef enum
{
    TRACE_Idle,
    TRACE_Running,
    TRACE_Trigged,
    TRACE_Done,
    TRACE_Error
} TRACE_Status_t;

typedef enum
{
    TRACE_Higher,
    TRACE_HigherEqual,
    TRACE_Lower,
    TRACE_LowerEqual,
    TRACE_NotEqual,
    TRACE_Equal,
    TRACE_Immidiately,
    TRACE_f32_Higher,
    TRACE_f32_HigherEqual,
    TRACE_f32_Lower,
    TRACE_f32_LowerEqual,
    TRACE_f32_NotEqual,
    TRACE_f32_Equal,
} TRACE_Op_t;

typedef enum
{
    TRACE_Clock_CurrentLoop,
    TRACE_Clock_VelocityLoop,
    TRACE_Clock_PostionLoop,
    TRACE_Clock_Power,
    TRACE_Clock_None = 0xff,
} TRACE_Clock_t;
/*______ G L O B A L - D A T A _______________________________________________*/

/*______ G L O B A L - M A C R O S ___________________________________________*/

/*______ G L O B A L - F U N C T I O N S - P R O T O T Y P E S _______________*/
extern void TRACE_Run(void);
extern void TRACE_SetChannel(uint8_t* channelAddr, uint8_t channelLength);
extern void TRACE_SetTrigger(int16_t* triggerAddr, uint16_t triggerMask, int16_t triggerLevel, TRACE_Op_t triggerOp);
extern void TRACE_SetTraceCount(uint16_t preCount, uint16_t postCount);
extern void TRACE_SetSampleRate(uint8_t rate);
extern void TRACE_SetTimeout(uint16_t timeout_ms);
extern void TRACE_Clearall(void);
extern uint8_t TRACE_GetStatus(void);
extern void TRACE_Start(void);
extern void TRACE_GetAllBuff(void);
extern uint16_t TRACE_GetBuff(uint8_t* buff, uint16_t len);

extern void TRACE_AutoPolling(void);

extern void TRACE_GetDataSector(uint8_t** sector1, uint16_t* length1, uint8_t** sector2, uint16_t* length2);

extern uint8_t TRACE_GetAutoPollingFlag(void);

void TRACE_InitFifo(void);

uint8_t TRACE_SendTraceData(void);
#endif /* TRACE_H */

/* _____ E N D _____ (trace.h) ____________________________________________*/