/**
 * @file DebugP_Config.c
 * @brief For control servo, Debug protocol
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
 * Version|Auther|Date|Describe
 * -------|-------|-------|-------- 
 * V0.0.0|XiaoyuHe     |12/29/2021      |Module Initial                
 */

/******* I N C L U D E - F I L E S *******************************************/

#include "DebugP_Config.h"
#include "Hal_Type.h"
#include "Hal_Uart.h"

/******* L O C A L - F U N C T I O N S ***************************************/


uint32_t Uart_AsyncSend(uint8_t port, uint8_t* buff, uint8_t length)
{
    return Hal_Uart_Send(&Uart[UART_TRACE], buff, length);
}

uint32_t Uart_AsyncReceive(uint8_t port, uint8_t* buff, uint8_t length)
{
    return Hal_Uart_Recv(&Uart[UART_TRACE], buff, length);
}