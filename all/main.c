/*
 *  ======== main.c ========
 *  Simple example that uses the Sensors APIs
 */
#include "TMP102.h"

#include "mcu.h"  /* for mcu_msWait() */

#include "config.h" /* for sensor handle names */

int32_t tmp1020Temp;
int32_t tmp1020TempAsIntCelsius;
int32_t tmp1020TempAsMilliCelsius;
float tmp1020TempAsFloatCelsius;

/*
 *  ======== main ========
 */
int main(void)
{
    TMP102_config(TMP102_0);

    for (;;) {
        /* Note: in continuous modes (CC), polling rate should
         *       not exceed sensor conversion rate
         */
        mcu_msWait(1000); /* delay 1 sec */

        /* Read TMP102_0 temp */
        tmp1020Temp = TMP102_read(TMP102_0);
        tmp1020TempAsIntCelsius = TMP102_toIntCelsius(TMP102_0, tmp1020Temp);
        tmp1020TempAsMilliCelsius = TMP102_toMilliCelsius(TMP102_0, tmp1020Temp);
        tmp1020TempAsFloatCelsius = TMP102_toFloatCelsius(TMP102_0, tmp1020Temp);

    }
}
