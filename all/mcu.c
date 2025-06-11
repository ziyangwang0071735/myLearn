/*
 *  Include Generic Header Files Here
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "i2cLib_FIFO_polling.h"
#include "mcu.h"
// #define I2C_FIFO_LEVEL         (16U)
// #define SUCCESS                (0)
// #define ERROR_NACK             (-1)
// #define ERROR_STOP_TIMEOUT     (-2)
// #define ERROR_RX_TIMEOUT       (-3)
struct I2CHandle i2cParams;
/*
 *  Include MCU Specific Header Files Here
 */

/*
 *  Include MCU Specific Header Files Here
 */

/********* MCU SPECIFIC I2C CODE STARTS HERE **********/

void mcu_i2cInit(uint8_t busId)
{
    /* Add MCU specific init necessary for I2C to be used */
    if (busId != 0) return;
    i2cParams.NumOfDataBytes = 2;
    i2cParams.NumOfAttempts  = 5;
}

int8_t mcu_i2cTransfer( uint8_t busId, uint8_t i2cAddr,
                        uint8_t *dataToWrite, uint8_t writeLength,
                        uint8_t *dataToRead,  uint8_t readLength)
{
    /*
     *  Add MCU specific I2C read/write code here.
     */
    
    uint16_t status;
    i2cParams.base = I2CA_BASE;
    i2cParams.Delay_us = 10;
    i2cParams.SlaveAddr = i2cAddr;
    if (writeLength > 0) {
        // Write-only 或 write + read
        i2cParams.pTX_MsgBuffer = dataToWrite;
        i2cParams.NumOfAddrBytes = 0;  // 如果是寄存器地址先写，改为1或2
        i2cParams.NumOfDataBytes = writeLength;

        status = I2C_MasterTransmitter(&i2cParams);
        if (status != SUCCESS) return ERROR_NACK;
    }
    if (readLength > 0) {
        // Read-only 或 write + read（上一步已发了地址）
        i2cParams.pRX_MsgBuffer = dataToRead;
        i2cParams.NumOfDataBytes = readLength;

        status = I2C_MasterReceiver(&i2cParams);
        if (status != SUCCESS) return ERROR_RX_TIMEOUT;
    }

    /*
     *  Add MCU specific return code for error handling
     */

    return (0);
}
/********* MCU SPECIFIC I2C CODE ENDS HERE **********/




/********* MCU SPECIFIC DELAY CODE STARTS HERE ************/
void mcu_msWait(uint16_t msWait)
{
    /*
     *  Add MCU specific wait loop for msWait. The unit is in milli-seconds
     */
    DEVICE_DELAY_US(1000*msWait);
}

void mcu_usWait(uint16_t usWait)
{
    /*
     *  Add MCU specific wait loop for usWait. The unit is in micro-seconds
     */
    DEVICE_DELAY_US(usWait);
}

/********* MCU SPECIFIC DELAY CODE ENDS HERE ************/
