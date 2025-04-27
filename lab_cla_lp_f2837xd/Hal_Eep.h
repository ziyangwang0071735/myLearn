/**
 * @file        Hal_Eep.h 
 * @brief       Hardware eeprom basic function implementation
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

 #ifndef HAL_EEP_H 
 #define HAL_EEP_H 
 
  /******* I N C L U D E - F I L E S *******************************************/
 
  /******* G L O B A L - D E F I N E S *****************************************/
 #define IIC_EEPROM I2CA_BASE
 #define MAX_BUFFER_SIZE             64
 
 //
 // Error messages for read and write functions
 //
 #define ERROR_BUS_BUSY              0x1000
 #define ERROR_NACK_RECEIVED         0x2000
 #define ERROR_ARBITRATION_LOST      0x3000
 #define ERROR_STOP_NOT_READY        0x5555
 #define SUCCESS                     0x0000
  /******* G L O B A L - T Y P E S *********************************************/
 typedef struct {
     uint16_t address;
     uint16_t DataWr[MAX_BUFFER_SIZE];
     uint16_t DataRd[MAX_BUFFER_SIZE];
     uint16_t Status;
     uint16_t Data_Len;
     uint32_t base;
 } EEPROMDriver;
  /******* G L O B A L - D A T A ***********************************************/
 extern EEPROMDriver eeprom_handle;
  /******* G L O B A L - F U N C T I O N - P R O T O T Y P E S *****************/
 extern uint16_t EEPROM_Init(EEPROMDriver *dev);
 extern uint16_t EEPROM_Write(EEPROMDriver *dev);
 extern uint16_t EEPROM_Read(EEPROMDriver *dev);
 extern uint16_t EEPROM_Buffer_Clear(EEPROMDriver *dev);
 extern void EEprom_test_demo();
 #endif /* HAL_EEP_H  */
 /******* E N D  (Hal_Eep.h )****************************************************/