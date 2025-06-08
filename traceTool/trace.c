/******************************************************************************
* @F_NAME :          trace.c
* @F_PURPOSE :       Purpose
* @F_CREATED_BY :    Shengping Wang
* @F_CREATION_DATE : 2015-08-26
* @F_LANGUAGE :      C
* @F_MPROC_TYPE :   Both endian
*************************************** (C) Copyright 2015 Midea Group   *****/
/*______ I N C L U D E - F I L E S ___________________________________________*/
#include "Hal_Type.h"
#include "trace.h"
#include "trace_config.h"

/*______ L O C A L - D E F I N E S ___________________________________________*/

#define TRACE_Max_Channel 6
/*______ L O C A L - T Y P E S _______________________________________________*/
typedef struct
{
    uint8_t* channelAddr;
    uint8_t channelLength;
    uint8_t active;
} Trace_Channel_t;
/*______ G L O B A L - D A T A _______________________________________________*/

/*______ P R I V A T E - D A T A _____________________________________________*/

/*______ L O C A L - D A T A _________________________________________________*/

static TRACE_Status_t Trace_Status;
static Trace_Channel_t Trace_Channel[TRACE_Max_Channel];

static int16_t* Trace_Trigger;
static uint16_t Trace_TriggerMask;
static TRACE_Op_t Trace_Operation;
static int16_t Trace_i16TriggerRight;
static float32_t Trace_f32TriggerRight;

static int16_t* Trace_Trigger1;
static uint16_t Trace_TriggerMask1;
static TRACE_Op_t Trace_Operation1;
static int16_t Trace_i16TriggerRight1;
static float32_t Trace_f32TriggerRight1;

static uint8_t Trace_TriggerSetIndex;
static uint8_t Trace_TriggerStatus;

static uint16_t Trace_PreCount;
static uint16_t Trace_PostCount;

static uint16_t Trace_PreCount_work;
static uint16_t Trace_PostCount_work;

static uint8_t Trace_SampleRate;
static uint8_t Trace_SampleRateCount;

static uint16_t Trace_TimeoutMS = 0;
static uint8_t Trace_ChannelId = 0;

static uint8_t Trace_AutoPollingFlag = 0;

static uint8_t Trace_PollingLength = 4;
static uint8_t Trace_PreSendFlag;

static uint8_t Trace_DataSector = 0;

/*______ L O C A L - M A C R O S _____________________________________________*/

/*______ I M P O R T - F U N C T I O N S - P R O T O T Y P E S _______________*/

/*______ L O C A L - F U N C T I O N S - P R O T O T Y P E S _________________*/
static void Trace_DataCollect(void);
/*______ G L O B A L - F U N C T I O N S _____________________________________*/
uint8_t TRACE_GetStatus(void)
{
    return Trace_Status;
}

uint8_t TRACE_GetAutoPollingFlag(void)
{
    return Trace_AutoPollingFlag;
}

void TRACE_SetChannel(uint8_t* channelAddr, uint8_t channelLength)
{
    if (Trace_ChannelId < TRACE_Max_Channel)
    {
        if (channelAddr != 0 && channelLength != 0)
        {
            Trace_Channel[Trace_ChannelId].channelAddr = channelAddr;
            Trace_Channel[Trace_ChannelId].channelLength = channelLength;
            Trace_Channel[Trace_ChannelId].active = 1;
            Trace_ChannelId++;
            Trace_PollingLength += channelLength;
        }
        else
        {
            Trace_Channel[Trace_ChannelId].active = 0;
        }
    }
}

void TRACE_SetTrigger(int16_t* triggerAddr, uint16_t triggerMask, int16_t triggerLevel, TRACE_Op_t triggerOp)
{
    if (Trace_TriggerSetIndex == 0)
    {
        Trace_Trigger = triggerAddr;
        Trace_TriggerMask = triggerMask;
        Trace_i16TriggerRight = triggerLevel;
        Trace_Operation = triggerOp;
        Trace_TriggerSetIndex++;
        *((uint32_t*) &Trace_f32TriggerRight) = ((uint32_t) triggerLevel) << 16;
    }
    else
    {
        Trace_Trigger1 = triggerAddr;
        Trace_TriggerMask1 = triggerMask;
        Trace_i16TriggerRight1 = triggerLevel;
        Trace_Operation1 = triggerOp;
        Trace_TriggerSetIndex = 0;
        *((uint32_t*) &Trace_f32TriggerRight1) = ((uint32_t) triggerLevel) << 16;
    }
}

void TRACE_SetTraceCount(uint16_t preCount, uint16_t postCount)
{
    Trace_PreCount = preCount;
    Trace_PostCount = postCount;

    Trace_PreCount_work = Trace_PreCount;
    Trace_PostCount_work = Trace_PostCount;
}

void TRACE_SetSampleRate(uint8_t rate)
{
    Trace_SampleRate = rate;
    Trace_SampleRateCount = 0;
}

void TRACE_SetTimeout(uint16_t timeout_ms)
{
    Trace_TimeoutMS = timeout_ms;
}
void TRACE_AutoPolling(void)
{
    Trace_AutoPollingFlag = 1;
    Trace_PreSendFlag = 1;
    TRACE_Initial(Trace_PollingLength + 1);
    Trace_Status = TRACE_Running;
}
void TRACE_Clearall(void)
{
    uint8_t index;
    Trace_Status = TRACE_Idle;
    for (index = 0; index < TRACE_Max_Channel; index++)
    {
        Trace_Channel[index].active = 0;
    }
    Trace_ChannelId = 0;
    Trace_AutoPollingFlag = 0;
    Trace_PollingLength = 0;
    Trace_DataSector = 0;

    Trace_TriggerSetIndex = 0;
}
void TRACE_Start(void)
{
    TRACE_Initial(Trace_PollingLength);
    TRACE_StartMSTimer(Trace_TimeoutMS);

    Trace_PreCount_work = Trace_PreCount;
    Trace_PostCount_work = Trace_PostCount;

    Trace_DataSector = 0;
    Trace_TriggerStatus = 0;

    Trace_Status =TRACE_Running;
}
void TRACE_InitFifo(void)
{
    TRACE_Initial(Trace_PollingLength);
}
void TRACE_GetDataSector(uint8_t** sector1, uint16_t* length1, uint8_t** sector2, uint16_t* length2)
{
    TRACE_GetDataFIFOSector(sector1, length1, sector2, length2);
    uint16_t totalLen = Trace_PollingLength * (Trace_PreCount + Trace_PostCount);
    if (*length2 >= totalLen)
    {
        *sector1 = *sector2 + (*length2 - totalLen);
        *length1 = totalLen;
        *sector2 = 0;
        *length2 = 0;
    }
    else if ((*length2 + *length1) > totalLen)
    {
        *sector1 = *sector1 + (*length2 + *length1 - totalLen);
        *length1 = totalLen - *length2;
    }
}
void TRACE_GetAllBuff(void)
{
    uint8_t* p1;
    uint8_t* p2;
    uint16_t l1, l2;
    TRACE_GetDataSector(&p1, &l1, &p2, &l2);
    if (Trace_DataSector == 0)
    {
        if (l1 > 0)
        {
            TRACE_SendData(p1, l1, 1);
        }
        Trace_DataSector = 1;
    }
    else
    {
        if (l2 > 0)
        {
            TRACE_SendData(p2, l2, 2);
        }
        Trace_DataSector = 0;
    }
}

uint16_t TRACE_GetBuff(uint8_t* buff, uint16_t len)
{
    return TRACE_Read(buff, len);
}

void TRACE_Run(void)
{
    //  if(Trace_SampleRateCount==0)
    //      Trace_DataCollect();
    //  Trace_SampleRateCount++;
    //  if(Trace_SampleRateCount>=Trace_SampleRate)
    //      Trace_SampleRateCount=0;

    if (Trace_SampleRateCount > 0)
        Trace_SampleRateCount--;

    if (Trace_SampleRateCount == 0)
    {
        Trace_DataCollect();
        Trace_SampleRateCount = Trace_SampleRate;
    }

    static uint8_t l_waitFlag = 0; //TRACE_SendTraceData();
    static uint16_t cnt_wait = 0;

    if (l_waitFlag == 1)
    {
        cnt_wait++;
        if (cnt_wait > 6)
        {
            l_waitFlag = 0;
            cnt_wait = 0;
        }
    }
    else
    {
        l_waitFlag = TRACE_SendTraceData();
    }
}
/*______ P R I V A T E - F U N C T I O N S ___________________________________*/

/*______ L O C A L - F U N C T I O N S _______________________________________*/
static inline uint8_t Trace_CalcTrigerI16Unit(int16_t* addr, uint16_t mask, int16_t i16_level, TRACE_Op_t op)
{
    int16_t value;
    uint8_t retValue = 0;
    value = (*(addr)) & (mask);
    switch (op)
    {
    case TRACE_Higher:
        if (value > i16_level)
            retValue = 1;
        break;

    case TRACE_HigherEqual:
        if (value >= i16_level)
            retValue = 1;
        break;

    case TRACE_Lower:
        if (value < i16_level)
            retValue = 1;
        break;

    case TRACE_LowerEqual:
        if (value <= i16_level)
            retValue = 1;
        break;

    case TRACE_NotEqual:
        if (value != i16_level)
            retValue = 1;
        break;

    case TRACE_Equal:
        if (value == i16_level)
            retValue = 1;
        break;

    case TRACE_Immidiately:
        if (Trace_PreCount_work == 0)
        {
            retValue = 1;
        }
        else
        {
            Trace_PreCount_work--;
        }
        break;
    default:
        break;
    }
    return retValue;
}

static inline uint8_t Trace_CalcTrigerF32Unit(float32_t* addr, float32_t f32_level, TRACE_Op_t op)
{
    float32_t value;
    uint8_t retValue = 0;
    value = *(addr);
    switch (op)
    {
    case TRACE_f32_Higher:
        if (value > f32_level)
            retValue = 1;
        break;

    case TRACE_f32_HigherEqual:
        if (value >= f32_level)
            retValue = 1;
        break;

    case TRACE_f32_Lower:
        if (value < f32_level)
            retValue = 1;
        break;

    case TRACE_f32_LowerEqual:
        if (value <= f32_level)
            retValue = 1;
        break;

    case TRACE_f32_NotEqual:
        if (value != f32_level)
            retValue = 1;
        break;

    case TRACE_f32_Equal:
        if (value == f32_level)
            retValue = 1;
        break;
    default:
        break;
    }
    return retValue;
}
uint8_t Trace_CalcTrigger(void)
{
    uint8_t retValue = 0;
    if (Trace_TriggerStatus == 0) /* calc trigger 0 */
    {
        if (Trace_Operation <= TRACE_Immidiately)
        {
            if (Trace_CalcTrigerI16Unit(Trace_Trigger, Trace_TriggerMask, Trace_i16TriggerRight, Trace_Operation) == 1)
            {
                Trace_TriggerStatus = 1;
            }
        }
        else if (Trace_CalcTrigerF32Unit((float32_t*) Trace_Trigger, Trace_f32TriggerRight, Trace_Operation) == 1)
        {
            Trace_TriggerStatus = 1;
        }
    }
    if (Trace_TriggerStatus == 1) /* calc trigger 1 */
    {
        if (Trace_Operation1 <= TRACE_Immidiately)
        {
            if (Trace_CalcTrigerI16Unit(Trace_Trigger1, Trace_TriggerMask1, Trace_i16TriggerRight1, Trace_Operation1) == 1)
            {
                Trace_TriggerStatus = 2;
                retValue = 1;
            }
        }
        else if (Trace_CalcTrigerF32Unit((float32_t*) Trace_Trigger1, Trace_f32TriggerRight1,Trace_Operation1) == 1)
        {
            Trace_TriggerStatus = 2;
            retValue = 1;
        }
    }
    return retValue;
}

void Trace_DataCollect(void)
{
    uint8_t channelId;
    switch (Trace_Status)
    {
    case TRACE_Idle:
        break;

    case TRACE_Running:
        if (Trace_TimeoutMS > 0)
        {
            if (TRACE_GetTimeoutEvent() == 1)
                Trace_Status = TRACE_Error;
        }
        else if (Trace_AutoPollingFlag == 0)
        {
            if (Trace_CalcTrigger() == 1)
                Trace_Status = TRACE_Trigged;

            channelId = 0;
            while ((channelId < TRACE_Max_Channel) && (Trace_Channel[channelId].active == 1))
            {
                TRACE_ForceWrite(Trace_Channel[channelId].channelAddr, Trace_Channel[channelId].channelLength);
                channelId++;
            }
        }
        else
        {
            if ((Trace_AutoPollingFlag) && (Trace_Channel[0].active == 1))
            {
                TRACE_WritePollingHead(Trace_PollingLength);
                channelId = 0;
                while ((channelId < TRACE_Max_Channel) && (Trace_Channel[channelId].active == 1))
                {
                    //USBDC_WriteSendFIFO(Trace_Channel[channelId].channelAddr,Trace_Channel[channelId].channelLength);
                    TRACE_ForceWrite(Trace_Channel[channelId].channelAddr, Trace_Channel[channelId].channelLength);
                    channelId++;
                }
            }
        }
        break;

    case TRACE_Trigged:
        if (Trace_TimeoutMS > 0)
        {
            if (TRACE_GetTimeoutEvent() == 1)
                Trace_Status = TRACE_Error;
        }
        else if (Trace_PostCount_work == 0)
        {
            Trace_Status = TRACE_Done;
        }
        else
        {
            channelId = 0;
            while (channelId < TRACE_Max_Channel && Trace_Channel[channelId].active == 1)
            {
                if (TRACE_ForceWrite(Trace_Channel[channelId].channelAddr, Trace_Channel[channelId].channelLength)
                    != Trace_Channel[channelId].channelLength)
                {
                    Trace_Status = TRACE_Error;
                    break;
                }
                else
                {
                }
                channelId++;
            }
            Trace_PostCount_work--;
        }
        break;

    case TRACE_Done:
    case TRACE_Error:
        /* Not defined yet */
        break;
    }
}
/*______ E N D _____ (fanmp.c) ____________________________________________*/