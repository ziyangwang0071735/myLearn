/**
 * @file        NVM.h
 * @brief       Non-Volatile Memory application
 * @author      Rockshaw.Luo
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
 * V0.0.0|Rockshaw.Luo |01/22/2024      |Module Initial
 */

#ifndef NVM_H
#define NVM_H

/******* I N C L U D E - F I L E S *******************************************/
#include "NVM_Extern.h"
#include "NVM_COnfig.h"
#include "AxisGlobal.h"
/******* G L O B A L - D E F I N E S *****************************************/
#define NVM_SUCCESS 0U
#define NVM_FAILURE 1U
#define NVM_MAIN_BLOCK_CORRUPTED 2U
#define NVM_BLOCK_NOT_WRITEN 3U

/******* G L O B A L - T Y P E S *********************************************/
typedef struct
{
    uint32_t SSDCBAccumulatedRunningTime;
} SSDCB_RT_Payload_t;

typedef struct
{
    TOBJ3027 InverterErrorMessagesHistory;
} SSDCB_IEMH_Payload_t;

typedef struct
{
    TOBJ5027 SupplierErrorMessagesHistory;
} SSDCB_SEMH_Payload_t;

typedef struct
{
    uint32_t SSDBAccumulatedRunningTime;
    uint32_t Axis0_SSDB_RunTime;
    uint32_t Axis1_SSDB_RunTime;
    uint32_t Axis2_SSDB_RunTime;
    uint32_t Axis3_SSDB_RunTime;
    uint32_t Axis4_SSDB_RunTime;
} SSDB_RT_Payload_t;


/******* G L O B A L - D A T A ***********************************************/

/******* G L O B A L - F U N C T I O N S - P R O T O T Y P E S ***************/

#endif /* NVM_H      */
/******* E N D  (NVM.h     )****************************************************/