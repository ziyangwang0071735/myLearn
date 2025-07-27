/**
 * @file        Hal_IICDevice_Unittest.c
 * @brief       IIC device unit test
 * @author      Jane.Xie
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
 * Version|Author  |Date      |Describe
 * -------|--------|----------|--------------
 * V0.0.0 |Jane.Xie|06/18/2025|Module Initial
 */

/******* I N C L U D E - F I L E S *******************************************/
#include "Hal_Eep.h"
#include "Hal_PcbTmpr.h"
#include "board.h"

/******* L O C A L - D E F I N E S *******************************************/
#define IICDEVICE_TEST_ENABLE
#ifdef IICDEVICE_TEST_ENABLE
/******* L O C A L - T Y P E S ***********************************************/
typedef struct {

    Hal_Eep_t *pEepIntf;
    uint32_t SpiBase;
    uint16_t WpPin;

} Hal_IICDev_Eep_Object_t;

typedef struct {

    Hal_PcbTmpr_t *pTmprIntf;
    uint32_t SpiBase;

} Hal_IICDev_PcbTmpr_Object_t;

/******* L O C A L - D A T A *************************************************/
Hal_IICDev_PcbTmpr_Object_t IICDEV_PcbTmpr_SSDCB_Obj =
{
    .pTmprIntf  = &PcbTmprI2C0Obj,
    .SpiBase    = SSDCB_IIC_BASE,
};

Hal_IICDev_PcbTmpr_Object_t IICDEV_PcbTmpr_SSDB_Obj =
{
    .pTmprIntf  = &PcbTmprI2C1Obj,
    .SpiBase    = SSDB_IIC_BASE,
};

Hal_IICDev_Eep_Object_t IICDEV_Eep_SSDCB_Obj =
{
    .pEepIntf   = &EepI2C0Obj,
    .SpiBase    = SSDCB_IIC_BASE,
    .WpPin      = BID_EROM_WP_OUT,
};

Hal_IICDev_Eep_Object_t IICDEV_Eep_SSDB_Obj =
{
    .pEepIntf   = &EepI2C1Obj,
    .SpiBase    = SSDB_IIC_BASE,
    .WpPin      = DRIVE_EEPROM_WP_OUT,
};


float32_t TmprCelsiusRes;
/******* L O C A L - F U N C T I O N - P R O T O T Y P E S *******************/

/******* L O C A L - F U N C T I O N - D E F I N I T I O N S *****************/
static void Hal_PcbTmpr_Unittest_Init(Hal_IICDev_PcbTmpr_Object_t *pPcbTmpr)
{
    Hal_PcbTmpr_Init(pPcbTmpr->pTmprIntf, pPcbTmpr->SpiBase);
}

static void Hal_PcbTmpr_Unittest_Run(Hal_IICDev_PcbTmpr_Object_t *pPcbTmpr)
{
    Hal_PcbTmpr_SendReadTmprCmd(pPcbTmpr->pTmprIntf, pPcbTmpr->SpiBase);
    DEVICE_DELAY_US(200);
    TmprCelsiusRes = Hal_PcbTmpr_GetTmpr(pPcbTmpr->pTmprIntf, pPcbTmpr->SpiBase);
}

static void Hal_Eep_Unittest_Init(Hal_IICDev_Eep_Object_t *pEep)
{
    Hal_Eep_Init(pEep->pEepIntf, pEep->SpiBase);
    Hal_Eep_DisableWP(pEep->WpPin);
}

static void Hal_Eep_Unittest_Run(Hal_IICDev_Eep_Object_t *pEep)
{
    static uint16_t TestCnt = 0;

    DEVICE_DELAY_US(6000);

    pEep->pEepIntf->EepAddr = TestCnt * EEP_IIC_BUF_SIZE;

    for(uint16_t i = 0; i < EEP_IIC_BUF_SIZE; i++)
    {
        pEep->pEepIntf->DataWr[i] = (TestCnt * EEP_IIC_BUF_SIZE + i) * 5;
    }

    pEep->pEepIntf->EepHandle.NumOfDataBytes = EEP_IIC_BUF_SIZE;
    Hal_Eep_Write(pEep->pEepIntf, pEep->SpiBase);

    DEVICE_DELAY_US(6000);

    pEep->pEepIntf->EepHandle.NumOfDataBytes = EEP_IIC_BUF_SIZE;
    Hal_Eep_SendReadCmdWithAddr(pEep->pEepIntf, pEep->SpiBase);
    DEVICE_DELAY_US(500);
    Hal_Eep_SendRestart(pEep->pEepIntf, pEep->SpiBase);
    DEVICE_DELAY_US(500);
    Hal_Eep_GetData(pEep->pEepIntf, pEep->SpiBase);

    DEVICE_DELAY_US(6000);

    TestCnt++;

    for (uint16_t i = 0; i < EEP_IIC_BUF_SIZE; i++)
        {
            if ((pEep->pEepIntf->DataWr[i] & 0xff) != pEep->pEepIntf->DataRd[i])
            {
                ESTOP0;
            }
        }

}

/******* G L O B A L - F U N C T I O N - D E F I N I T I O N S ***************/
void Hal_IICDevice_Unittest_Init()
{
//    Hal_PcbTmpr_Unittest_Init(&IICDEV_PcbTmpr_SSDCB_Obj);
    Hal_Eep_Unittest_Init(&IICDEV_Eep_SSDCB_Obj);

//    Hal_PcbTmpr_Unittest_Init(&IICDEV_PcbTmpr_SSDB_Obj);
//    Hal_Eep_Unittest_Init(&IICDEV_Eep_SSDB_Obj);
}


void Hal_IICDevice_Unittest_Run()
{
//    Hal_PcbTmpr_Unittest_Run(&IICDEV_PcbTmpr_SSDCB_Obj);
    Hal_Eep_Unittest_Run(&IICDEV_Eep_SSDCB_Obj);

//    Hal_PcbTmpr_Unittest_Run(&IICDEV_PcbTmpr_SSDB_Obj);
//    Hal_Eep_Unittest_Run(&IICDEV_Eep_SSDB_Obj);
}
#endif
/******* E N D ***** (Hal_Eep_Unittest.c) ************************************/