/******************************************************************************
* @F_NAME :          trace_config.c
* @F_PURPOSE :       Purpose
* @F_CREATED_BY :    Shengping Wang
* @F_CREATION_DATE : 2015-08-26
* @F_LANGUAGE :      C
* @F_MPROC_TYPE :   Both endian
*************************************** (C) Copyright 2015 Midea Group   *****/
/*______ I N C L U D E - F I L E S ___________________________________________*/
#include "fifoa.h"
#include "trace_config.h"
#include "DebugP_Extern.h"

/*______ L O C A L - D E F I N E S ___________________________________________*/
#define TRACE_BUFF_LENGTH ((uint16_t) (0x4000 - 1))
/*______ L O C A L - T Y P E S _______________________________________________*/

/*______ G L O B A L - D A T A _______________________________________________*/

/*______ P R I V A T E - D A T A _____________________________________________*/

/*______ L O C A L - D A T A _________________________________________________*/
static uint8_t Trace_buff[TRACE_BUFF_LENGTH + 1];

static uint16_t Trace_TimeoutCount;

static FIFOA_Buff_t Trace_FIFO;
/*______ L O C A L - M A C R O S _____________________________________________*/

/*______ I M P O R T - F U N C T I O N S - P R O T O T Y P E S _______________*/

/*______ L O C A L - F U N C T I O N S - P R O T O T Y P E S _________________*/

/*______ G L O B A L - F U N C T I O N S _____________________________________*/
void TRACE_StartMSTimer(uint16_t value)
{
    Trace_TimeoutCount = value;
}
uint8_t TRACE_GetTimeoutEvent(void)
{
    static uint16_t localCount = TRACE_TIMEOUT - 1;
    uint8_t flag = 0;
    if (Trace_TimeoutCount > 0)
    {
        if (localCount == 0)
        {
            localCount = TRACE_TIMEOUT - 1;
            Trace_TimeoutCount--;
        }
        else
        {
            localCount--;
        }
    }
    if (Trace_TimeoutCount == 0)
    {
        flag = 1;
    }
    return flag;
}

uint8_t TRACE_Initial(uint16_t unitlen)
{
    return FIFOA_Initial(&Trace_FIFO, Trace_buff, (TRACE_BUFF_LENGTH / unitlen * unitlen) + 1);
}
uint16_t TRACE_Read(uint8_t* dstBuff, uint16_t len)
{
    return FIFOA_Read(&Trace_FIFO, dstBuff, len);
}
uint16_t TRACE_Write(uint8_t* srcBuff, uint16_t len)
{
    return FIFOA_ForceWrite(&Trace_FIFO, srcBuff, len);
}
uint16_t TRACE_ForceWrite(uint8_t* srcBuff, uint16_t len)
{
    return FIFOA_ForceWrite(&Trace_FIFO, srcBuff, len);
}
uint16_t TRACE_GetCurLength(void)
{
    return FIFOA_GetCurLength(&Trace_FIFO);
}
uint8_t* TRACE_GetHead(void)
{
    return FIFOA_GetHead(&Trace_FIFO);
}


uint8_t* p1_buff;
uint8_t* p2_buff;
uint16_t l1_len;
uint16_t l2_len;

uint16_t p1_left;
uint16_t p2_left;

void TRACE_ClrSendStatus(void)
{
    p1_left = 0;
    p2_left = 0;
    l1_len = l2_len = 0;
    p1_buff = p2_buff = 0;
}

uint8_t TRACE_SendData(uint8_t* buff, uint16_t len, uint8_t type)
{
    if (type == 1)
    {
        p1_buff = buff;
        l1_len = len;
        p1_left = len;
    }
    else if (type == 2)
    {
        p2_buff = buff;
        l2_len = len;
        p2_left = len;
    }

    return 0;
    //  return (WIFIP_SyncSend(buff,len));
}

void TRACE_GetDataFIFOSector(uint8_t** sector1, uint16_t* length1, uint8_t** sector2, uint16_t* length2)
{
    FIFOA_GetDataSector(&Trace_FIFO, sector1, length1, sector2, length2);
}

uint8_t TRACE_ReadFrameFromFIFO(FIFOA_Buff_t* fifo, uint8_t* buff)
{
    //  uint8_t totalLen=FIFOA_GetCurLength(fifo);
    //  uint8_t curFrameLen=*(FIFOA_GetHead(fifo));
    //  if(totalLen<curFrameLen)
    //    return 0;
    //  else{
    //    FIFOA_Read(fifo, buff, curFrameLen);
    //    return curFrameLen;
    //  }
    return 0;
}

uint8_t TRACE_SendFrameInFIFO(void)
{
    return 0;
}

int8_t TRACE_WritePollingHead(uint8_t len)
{
    TRACE_ForceWrite(&len, 1);
    return 0;
}


int8_t TRACE_SendPollingData(void)
{
    // uint8_t len;
    // uint16_t workLen;
    // int8_t status=0;
    // while(1)
    // {
    //   if(status==0) /* read data from fifo */
    //   {
    //     workLen=TRACE_Read(&len, 1);
    //         if(workLen==0)
    //         {
    //           status=0; /* there is no data */
    //           break;
    //         }else
    //         {
    //           TRACE_Read(Trace_PollingCANBuff,len);
    //           status=DBUGP_SendTracePollingFrame(Trace_PollingCANBuff,len);
    //         }
    //   }
    //   else /* send the previous data */
    //   {
    //     status=DBUGP_SendTracePollingFrame(Trace_PollingCANBuff, len);
    //   }
    // }
    // return status;
    return 0;
}

/********* test code ************/


uint8_t TRACE_SendTraceData(void)
{
    uint8_t ret = 0;
    if (TRACE_GetAutoPollingFlag() == 0)
    {
        if (p1_left > 0)
        {
            DebugP_SendTraceTriggerBuff(p1_buff, l1_len, &p1_left);
            ret = 1;
        }
        else if (p2_left == 1)
        {
            DebugP_SendTraceTriggerBuff(p2_buff, l2_len, &p2_left);
            ret = 1;
        }
    }
    else
    {
        TRACE_SendPollingData();ret = 2;
    }
    return ret;
}
/*______ P R I V A T E - F U N C T I O N S ___________________________________*/

/*______ L O C A L - F U N C T I O N S _______________________________________*/

/*______ E N D _____ ( trace_config.c) ____________________________________________*/