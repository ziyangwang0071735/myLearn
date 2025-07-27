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
#include "driverlib.h"
#include "board.h"
#include "Hal_I2C.h"

 /******* G L O B A L - D E F I N E S *****************************************/
#define EEP_IIC_DEVICE_ADDR     0x50

/* Max is currently 14 because of 2 address bytes and the 16-byte FIFO */
#define EEP_IIC_BUF_SIZE        14

/******* G L O B A L - T Y P E S *********************************************/
/*
 * Enum representing the status codes for I2C communication
 */
typedef enum
{
    EEP_ACCESS_OK               = 0x0000,
    EEP_BUS_BUSY                = 0x0001,
    EEP_NACK_RECEIVED           = 0x0002,
    EEP_ARBITRATION_LOST        = 0x0004,
    EEP_RXFIFO_NOT_READY        = 0x0008,
    EEP_STOP_NOT_READY          = 0x0010,
    EEP_WRITE_ERROR             = 0x0020,
    EEP_READ_SEND_CMD_ERROR     = 0x0040,
    EEP_READ_RESTART_ERROR      = 0x0080,
    EEP_READ_READ_DATA_ERROR    = 0x0100,

} Hal_Eep_Status_t;

typedef struct {

    /*
     * I2C communication
     */
    uint8_t DataWr[EEP_IIC_BUF_SIZE];
    uint8_t DataRd[EEP_IIC_BUF_SIZE];
    uint32_t EepAddr;
    uint16_t Status;
    struct I2CHandle EepHandle;

} Hal_Eep_t;

/******* G L O B A L - D A T A ***********************************************/
extern Hal_Eep_t EepI2C0Obj;
extern Hal_Eep_t EepI2C1Obj;

/******* G L O B A L - F U N C T I O N - P R O T O T Y P E S *****************/
/**
 * @brief   Initialize the EEPROM handler with default configuration.
 *
 * @param   pEep  Pointer to the Hal_Eep_t object to be initialized.
 * @param   base  I2C controller base address.
 *
 * @note    This function resets the read and write buffers, sets the default
 *          I2C target address, and configures the I2C handle with parameters
 *          such as base address, control address, buffer pointers, number of
 *          address bytes, retry attempts, delay, and write cycle time.
 */
extern void Hal_Eep_Init(Hal_Eep_t *pEep, uint32_t base);


/**
 * @brief   Write data to the EEPROM using the I2C controller.
 *
 * @param   pEep  Pointer to the Hal_Eep_t object containing the data to write.
 * @param   base  I2C controller base address.
 *
 * @return  None.
 *
 * @note    Before calling this function, ensure that all relevant members of
 *          the Hal_Eep_t structure (e.g., address, data buffer, and byte counts)
 *          are properly initialized. Failure to do so may result in undefined
 *          behavior or communication errors.
 */
extern void Hal_Eep_Write(Hal_Eep_t *pEep, uint32_t base);


/**
 * @brief   Send a read command with EEPROM address to initiate a read operation.
 *
 * @param   pEep  Pointer to the EEPROM handle structure.
 * @param   base  I2C peripheral base address.
 *
 * @note    Configures target address, I2C base, address bytes, and register
 *          address before sending the read command. Sets the
 *          EEP_READ_SEND_CMD_ERROR flag if the operation fails.
 */
extern void Hal_Eep_SendReadCmdWithAddr(Hal_Eep_t *pEep, uint32_t base);


/**
 * @brief   Send a restart condition during the EEPROM read sequence.
 *
 * @param   pEep  Pointer to the EEPROM handle structure.
 * @param   base  I2C peripheral base address.
 *
 * @note    Send a restart signal after the device address and EEPROM address
 *          have been transmitted to EEPROM.
 *          Sets the EEP_READ_RESTART_ERROR flag if the operation fails.
 */
extern void Hal_Eep_SendRestart(Hal_Eep_t *pEep, uint32_t base);


/**
 * @brief   Retrieve data from EEPROM after a successful read operation.
 *
 * @param   pEep  Pointer to the EEPROM handle structure.
 * @param   base  I2C peripheral base address.
 *
 * @note    Read data from the I2C receive buffer. Set the
 *          EEP_READ_READ_DATA_ERROR flag if data retrieval fails. Requires
 *          successful command and restart operations first.
 */
extern void Hal_Eep_GetData(Hal_Eep_t *pEep, uint32_t base);


/**
 * @brief   Enable the Write Protection (WP) for the EEPROM.
 *
 * @param   pin  The GPIO pin connected to the WP (Write Protection) control.
 *
 * @note    This function sets the specified pin to high (logic 1) to enable
 *          the write protection feature of the EEPROM.
 */
extern void Hal_Eep_EnableWP(uint16_t pin);


/**
 * @brief   Disable the Write Protection (WP) for the EEPROM.
 *
 * @param   pin  The GPIO pin connected to the WP (Write Protection) control.
 *
 * @note    This function sets the specified pin to low (logic 0) to disable
 *          the write protection feature of the EEPROM.
 */
extern void Hal_Eep_DisableWP(uint16_t pin);


#endif /* HAL_EEP_H  */
/******* E N D  (Hal_Eep.h )****************************************************/