//#############################################################################
//
// FILE:   i2c_ex4_eeprom_polling.c
//
// TITLE:  I2C EEPROM Write / Read using polling
//
//! \addtogroup driver_example_list
//! <h1>I2C EEPROM</h1>
//!
//! This program will shows how to perform different EEPROM write and read
//! commands using I2C polling method
//! EEPROM used for this example is AT24C256
//!
//! \b External \b Connections \n
//!  - Connect external I2C EEPROM at address 0x50
//!  --------------------------------
//!    Signal   |  I2CA   |  EEPROM
//!  --------------------------------
//!     SCL     | GPIO105 |  SCL
//!     SDA     | GPIO104 |  SDA
//!     Make sure to connect GND pins if EEPROM and C2000 device are in different board.
//!  --------------------------------

//! //Example 1: EEPROM Byte Write
//! //Example 2: EEPROM Byte Read
//! //Example 3: EEPROM word (16-bit) write
//! //Example 4: EEPROM word (16-bit) read
//! //Example 5: EEPROM Page write
//! //Example 6: EEPROM word Paged read
//!
//! \b Watch \b Variables \n
//!  - \b TX_MsgBuffer - Message buffer which stores the data to be transmitted
//!  - \b RX_MsgBuffer - Message buffer which stores the data to be received
//!
//!
//#############################################################################
//
// 
// $Copyright:
// Copyright (C) 2013-2024 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "i2cLib_FIFO_polling.h"
#include "Hal_Eep.h"


//
// Globals
//
               // Used in interrupt

//!  --------------------------------
//!    Signal   |  I2CA   |  EEPROM
//!  --------------------------------
//!     SCL     | GPIO105 |  SCL
//!     SDA     | GPIO104 |  SDA
//!  --------------------------------







// Main
//
void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pullups.
    //
    Device_initGPIO();

    //
    // Initialize I2C pins
    //
    //I2C_GPIO_init();

    //
    // Initialize PIE and clear PIE registers. Disable CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //I2Cinit();
    Board_init();

    EEprom_test_demo();
    //I2Cs connected to I2CA will be found in AvailableI2C_slaves buffer
    //after you run I2CBusScan function.
//    uint16_t *pAvailableI2C_slaves = AvailableI2C_slaves;
//    status = I2CBusScan(I2CA_BASE, pAvailableI2C_slaves);
//
//    uint16_t i;
//
//    for(i=0;i<MAX_BUFFER_SIZE;i++)
//    {
//        TX_MsgBuffer[i] = 0;
//        RX_MsgBuffer[i] = 0;
//    }
//
//    EEPROM.SlaveAddr      = 0x50;
//    EEPROM.base           = I2CA_BASE;
//    EEPROM.pControlAddr   = &ControlAddr;
//    EEPROM.NumOfAddrBytes = 2;
//    EEPROM.pTX_MsgBuffer  = TX_MsgBuffer;
//    EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
//    EEPROM.NumOfAttempts  = 5;
//    EEPROM.Delay_us       = 10;
//    EEPROM.WriteCycleTime_in_us = 6000;    //10ms for EEPROM this code was tested
//
//    //Example 1: EEPROM Byte Write
//    //Write 11 to EEPROM address 0x0
//    ControlAddr = 0;
//    EEPROM.NumOfDataBytes = 1;
//    TX_MsgBuffer[0]       = 11;
//    status = I2C_MasterTransmitter(&EEPROM);
//
//    //Wait for EEPROM write cycle time
//    //This delay is not mandatory. User can run their application code instead.
//    //It is however important to wait for EEPROM write cycle time before you initiate
//    //another read / write transaction
//    DEVICE_DELAY_US(EEPROM.WriteCycleTime_in_us);
//
//    //Example 2: EEPROM Byte Read
//    //Make sure 11 is written to EEPROM address 0x0
//    ControlAddr = 0;
//    EEPROM.pControlAddr   = &ControlAddr;
//    EEPROM.NumOfDataBytes = 1;
//    status = I2C_MasterReceiver(&EEPROM);
//
//    while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);
//
//    verifyEEPROMRead();
//
//    //Example 3: EEPROM word (16-bit) write
//    //EEPROM address 0x1 = 22 &  0x2 = 33
//    ControlAddr = 0;   //EEPROM address to write
//    EEPROM.NumOfDataBytes  = 2;
//    TX_MsgBuffer[0]        = 0x11;
//    TX_MsgBuffer[1]        = 0x22;
//    EEPROM.pTX_MsgBuffer   = TX_MsgBuffer;
//    status = I2C_MasterTransmitter(&EEPROM);
//
//    //Wait for EEPROM write cycle time
//    //This delay is not mandatory. User can run their application code instead.
//    //It is however important to wait for EEPROM write cycle time before you initiate
//    //another read / write transaction
//    DEVICE_DELAY_US(EEPROM.WriteCycleTime_in_us);
//
//    //Example 4: EEPROM word (16-bit) read
//     //Make sure EEPROM address 1 has 0x11 and 2 has 0x22
//     ControlAddr = 0;
//     EEPROM.pControlAddr   = &ControlAddr;
//     EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
//     EEPROM.NumOfDataBytes = 2;
//
//     status = I2C_MasterReceiver(&EEPROM);
//
//     verifyEEPROMRead();
//
//
//    //Example 5: EEPROM Page write
//    //Program address = data pattern from address 64
//
//    for(i=0;i<MAX_BUFFER_SIZE;i++)
//    {
//        TX_MsgBuffer[i] = i+64;
//    }
//
//    ControlAddr = 64;   //EEPROM address to write
//    EEPROM.NumOfDataBytes  = MAX_BUFFER_SIZE;
//    EEPROM.pTX_MsgBuffer   = TX_MsgBuffer;
//    status = I2C_MasterTransmitter(&EEPROM);
//
//    //Wait for EEPROM write cycle time
//    //This delay is not mandatory. User can run their application code instead.
//    //It is however important to wait for EEPROM write cycle time before you initiate
//    //another read / write transaction
//    DEVICE_DELAY_US(EEPROM.WriteCycleTime_in_us);
//
//    //Example 6: EEPROM word Paged read
//    ControlAddr = 64;
//    EEPROM.pControlAddr   = &ControlAddr;
//    EEPROM.pRX_MsgBuffer  = RX_MsgBuffer;
//    EEPROM.NumOfDataBytes = MAX_BUFFER_SIZE;
//
//    status = I2C_MasterReceiver(&EEPROM);
//
//    verifyEEPROMRead();
//
//    if(status)
//    {
//        fail();
//    }
//    else
//    {
//        pass();
//    }
//
//
//
//    if(status)
//    {
//        fail();
//    }
//    else
//    {
//        pass();
//    }

}

//
// pass - Function to be called if data written matches data read
//

//
// fail - Function to be called if data written does NOT match data read
//
