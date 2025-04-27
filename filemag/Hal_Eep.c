/**
 * @file Hal_Eep.c 
 * @brief Hardware eeprom basic function implementation
 * @author      Ziyang.Wang  
 * @version     V0.0.0
 * @attention
 *  This material is the exclusive property of KUKA Robot Group
 *  Except as expressly permitted by separate agreement, this material may only
 *  be used by members of the development department of KUKA Robot Group for
 *  internal development purposes of KUKA Robot Group.
 *  Copyright (C) 2025
 *  KUKA Robot Group. All Rights Reserved.
 * @htmlonly
 * <span style="font-weight: bold">History</span>
 * @endhtmlonly
 * Version|Author|Date|Describe
 * -------|-------|-------|--------
 * V0.0.0|Ziyang.Wang  |04/27/2025      |Module Initial                
 */

 /******* I N C L U D E - F I L E S *******************************************/
 #include "driverlib.h"
 #include "device.h"
 #include "i2cLib_FIFO_polling.h"
 #include "Hal_Eep.h"
 #include "i2c.h"
  /******* L O C A L - D E F I N E S *******************************************/
 
  /******* L O C A L - T Y P E S ***********************************************/
 
  /******* L O C A L - D A T A *************************************************/
 EEPROMDriver eeprom_handle = {
     .address = 0x0000,
     .Status = 0x0000,
     .Data_Len = 0x0000,
     .base = IIC_EEPROM,
 };
 
 struct I2CHandle EEPROM;
 struct I2CHandle *currentMsgPtr;  // Used in interrupt
 
 uint16_t passCount = 0;
 uint16_t failCount = 0;
 
 uint16_t AvailableI2C_targets[20];
 //uint16_t TX_MsgBuffer[MAX_BUFFER_SIZE];
 //uint16_t RX_MsgBuffer[MAX_BUFFER_SIZE];
 uint32_t ControlAddr;
 uint16_t status;
 
  /******* L O C A L - F U N C T I O N - P R O T O T Y P E S *******************/
 void fail(void);
 void pass(void);
 void verifyEEPROMRead(void);
 
 
 
  /******* L O C A L - F U N C T I O N - D E F I N I T I O N S *****************/
 void EEprom_test_demo(){
 
     I2C_putData(I2CA_BASE, 49);
     EEPROM_Init(&eeprom_handle);
     uint16_t *pAvailableI2C_targets = AvailableI2C_targets;
     status = I2CBusScan(IIC_EEPROM, pAvailableI2C_targets);
 
     eeprom_handle.address = 0x0000;
     int i;
     for(i=0;i<MAX_BUFFER_SIZE;i++)
     {
         eeprom_handle.DataWr[i] = i+1;
     }
      
     eeprom_handle.Data_Len = MAX_BUFFER_SIZE;
     eeprom_handle.Status = EEPROM_Write(&eeprom_handle);
 
     DEVICE_DELAY_US(EEPROM.WriteCycleTime_in_us);
 
     eeprom_handle.Status = EEPROM_Read(&eeprom_handle);
 
 
     
 
 }
  /******* G L O B A L - F U N C T I O N - D E F I N I T I O N S ***************/
 uint16_t EEPROM_Init(EEPROMDriver *dev){
     EEPROM_Buffer_Clear(&eeprom_handle);
 
     EEPROM.TargetAddr      = 0x50;
     EEPROM.base           = dev->base;
     EEPROM.pControlAddr   = &dev->address;
     EEPROM.NumOfAddrBytes = 2;
     EEPROM.pTX_MsgBuffer  = dev->DataWr;
     EEPROM.pRX_MsgBuffer  = dev->DataRd;
     EEPROM.NumOfAttempts  = 5;
     EEPROM.Delay_us       = 10;
     EEPROM.WriteCycleTime_in_us = 6000;    //10ms for EEPROM this code was tested
 
     return 0;
     
 }
 uint16_t EEPROM_Buffer_Clear(EEPROMDriver *dev){
     uint16_t i;
     for(i=0;i<MAX_BUFFER_SIZE;i++)
     {
         dev->DataRd[i] = 0;
         dev->DataWr[i] = 0;
     }
 }
 uint16_t EEPROM_Write(EEPROMDriver *dev){
     uint16_t status;
     EEPROM.NumOfDataBytes  = dev->Data_Len;
     status = I2C_ControllerTransmitter(&EEPROM);
     return status;
 }
 uint16_t EEPROM_Read(EEPROMDriver *dev){
     uint16_t status;
     EEPROM.NumOfDataBytes  = dev->Data_Len;
     status = I2C_ControllerReceiver(&EEPROM);
     return status;
 }
 
 void verifyEEPROMRead(void)
 {
 //    uint16_t i;
 //    while(I2C_getStatus(EEPROM.base) & I2C_STS_BUS_BUSY);
 //
 //    for(i=0;i<EEPROM.NumOfDataBytes;i++)
 //    {
 //        if(EEPROM.pRX_MsgBuffer[i] != EEPROM.pTX_MsgBuffer[i])
 //        {
 //            //Transmitted data doesn't match received data
 //            //Fail condition. PC shouldn't reach here
 //            ESTOP0;
 //            fail();
 //        }
 //    }
 }
  /******* E N D ***** (Hal_Eep.c ) *********************************************/