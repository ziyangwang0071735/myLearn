/**
 * @file        DebugP_Extern.h                                   
 * @brief       For control servo, Debug protocol
 * @author      XiaoyuHe     
 * @version     V0.0.0
 * @attention
 *  This material is the exclusive property of KUKA Robot Group                
 *  Except as expressly permitted by separate agreement, this material may only
 *  be used by members of the development department of KUKA Robot Group for   
 *  internal development purposes of KUKA Robot Group.      
 *  Copyright (C) 2021                                                         
 *  KUKA Robot Group. All Rights Reserved.                                     
 * @htmlonly 
 * <span style="font-weight: bold">History</span> 
 * @endhtmlonly 
 * Version|Author|Date|Describe
 * -------|-------|-------|-------- 
 * V0.0.0|XiaoyuHe     |12/29/2021      |Module Initial                
 */

/******* I N C L U D E - F I L E S *******************************************/

#ifndef DEBUGP_EXTERN_H
#define DEBUGP_EXTERN_H

#include "Hal_Type.h"

/******* G L O B A L - D E F I N E S *****************************************/

#define CMD_BUFF_MAXLEN 128

/******* G L O B A L - T Y P E S *********************************************/

typedef struct
{
    uint16_t id;
    uint8_t len;
    uint8_t buff[CMD_BUFF_MAXLEN];
} DebugP_Frame_t;


/******* G L O B A L - F U N C T I O N S - P R O T O T Y P E S ***************/

extern void DebugP_Init(void);
extern void DebugP_Main(void);
extern int8_t DebugP_SendTraceTriggerBuff(uint8_t* buff, uint16_t len, uint16_t* pLeft);
#endif /* DEBUGP_EXTERN_H */
/******* E N D  (DebugP_Extern.h)****************************************************/