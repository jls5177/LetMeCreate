/**
 * This example shows how to use the Temp&Hum Click wrapper of the LetMeCreate
 * library.
 *
 * It reads the temperature and humidity from the sensor and exits.
 *
 * The Temp&Hum Click must be inserted in Mikrobus 1 before running this program.
 */

#include <stdio.h>
#include <letmecreate/letmecreate.h>

#include "examples/common.h"

int main(void)
{
    float temperature = 0.f;
    float humidity = 0.f;

    i2c_init();
    i2c_select_bus(MIKROBUS_2);

    temphum_click_enable();
    sleep_ms(500);
    temphum_click_get_temperature(&temperature, &humidity);
    printf("temperature: %.3f degrees celsius\n", temperature);
    printf("humidity: %.3f %% humidity\n", humidity);
    temphum_click_disable();

    i2c_release();

    return 0;
}
