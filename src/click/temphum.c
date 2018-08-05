#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <letmecreate/click/temphum.h>
#include <letmecreate/core/common.h>
#include <letmecreate/core/i2c.h>

#define HTS221_ADDRESS                  (0x5F)

#define TEMPHUM_STRING  "temphum"

// #define DEBUG_BUILD

#ifdef DEBUG_BUILD
#define DEBUG(__STR, ...)  fprintf(stdout, "%s[%s:%u] "__STR, TEMPHUM_STRING, __func__,__LINE__,##__VA_ARGS__)
#else
#define DEBUG(__STR, ...)
#endif

#define ERROR(__STR, ...)  fprintf(stdout, "%s: "__STR, TEMPHUM_STRING,##__VA_ARGS__)

#define MaxTemp       120
#define MinTemp       -40
#define MaxHumi       100
#define MinHumi       0

#define CONVERT_NATIVE_2COMPLEMENT(_VAR) do { \
    if (_VAR&0x8000) { \
        _VAR = -(0x8000-(0x7fff&_VAR)); \
    } \
} while(0)

#define CONVERT_16B(_LSB,_MSB) ((_MSB << 8) + _LSB)

enum HTS221_REGISTERS {
    WHOAMI           = 0x0F ,
    AV_CONF          = 0x10 ,
    CTRL_REG1        = 0x20 ,
    CTRL_REG2        = 0x21 ,
    CTRL_REG3        = 0x22 ,
    STATUS_REG       = 0x27 ,
    HUMIDITY_OUT_L   = 0x28 ,
    HUMIDITY_OUT_H   = 0x29 ,
    TEMP_OUT_L       = 0x2A ,
    TEMP_OUT_H       = 0x2B ,
    CALIB_START      = 0x30 ,
    CALIB_END        = 0x3F
};


static int write_register(uint8_t reg, uint8_t value)
{
    DEBUG("reg=0x%X, value=0x%X\n", reg, value);

    if (i2c_write_register(HTS221_ADDRESS, reg, value) < 0) {
        return -1;
    }
    return 0;
}


static int read_register(uint8_t reg, uint8_t *data, uint8_t length)
{
    size_t ii;

    if (!data || length == 0)
    {
        ERROR("Cannot read into an invalid buffer");
        return -1;
    }

    DEBUG("reg=%X\n", reg);
    for (ii=0; ii < length; ii++)
    {
       if (i2c_read_register(HTS221_ADDRESS, reg+ii, &data[ii]) < 0) {
          return -1;
       }
       DEBUG("*data[%u]=0x%X\n", ii, data[ii]);
    }

    return 0;
}


/*******************************************************************************
* Function linear_interpolation(int x0, float y0, int x1, float y1, float mes)
* ------------------------------------------------------------------------------
* Overview: Function performs linear interpolation on sensors data
* Input: Nothing
* Output: Temperature data
*******************************************************************************/
float linear_interpolation(int x0, float y0, int x1, float y1, float mes)
{
   float a = (float) ((y1 - y0) / (x1 - x0));
   float b = (float) (-a*x0 + y0);
   float cal = (float) (a * mes + b);
   return cal;
}


static int set_AV_CONF_value(uint8_t avgt, uint8_t avgh)
{
    uint8_t value = (avgt & 0x7) << 3 | (avgh & 0x7);
    return write_register(AV_CONF, value);
}

static int set_CTRL_value(uint8_t ctrl1, uint8_t ctrl2, uint8_t ctrl3)
{
    if (write_register(CTRL_REG1, ctrl1) < 0) {
        return -1;
    }
    if (write_register(CTRL_REG2, ctrl2) < 0) {
        return -1;
    }
    if (write_register(CTRL_REG3, ctrl3) < 0) {
        return -1;
    }

    return 0;
}

#define WHO_AM_I    (0xBC)

static bool temphum_init(void)
{
    uint8_t whoami;

    // Ensure we are talking to the correct device
    if (read_register(WHOAMI, &whoami, 1) < 0) {
        ERROR("Failed to read WHOAMI register");
        return false;
    }
    if (whoami != WHO_AM_I) {
       ERROR("Invalid WHOAMI ID: 0x%X", whoami);
       return false;
    }

    // Set the average rates
    if (set_AV_CONF_value(0b011, 0b011) < 0) {
        ERROR("failed to set AV CONF register");
        return false;
    }

    // Set the control register values
    if (set_CTRL_value(0x87, 0x00, 0x00) < 0) {
        ERROR("failed to set control register");
        return false;
    }

    return true;
}

int H0_T0_OUT;
int H1_T0_OUT;
int T0_OUT;
int T1_OUT;
float T0_DegC_cal;
float T1_DegC_cal;
float H0_RH_cal;
float H1_RH_cal;

static bool temphum_calibrate(void)
{
    uint8_t data[16] = {0};

    if (read_register(CALIB_START, data, sizeof (data)) < 0) {
        ERROR("Failed to read calibration register");
        return false;
    }

    T0_DegC_cal = (float) (((data[5] & 0x03) << 8) + data[2])/8.0;
    T1_DegC_cal = (float) (((data[5] & 0x0C) << 6) + data[3])/8.0;
    H0_RH_cal   = (float) (data[0])/2.0;
    H1_RH_cal   = (float) (data[1])/2.0;

    H0_T0_OUT = CONVERT_16B(data[6],  data[7]);
    H1_T0_OUT = CONVERT_16B(data[10], data[11]);
    T0_OUT    = CONVERT_16B(data[12], data[13]);
    T1_OUT    = CONVERT_16B(data[14], data[15]);

    // convert negative 2's complement values to native negative value
    CONVERT_NATIVE_2COMPLEMENT(H0_T0_OUT);
    CONVERT_NATIVE_2COMPLEMENT(H1_T0_OUT);
    CONVERT_NATIVE_2COMPLEMENT(T0_OUT);
    CONVERT_NATIVE_2COMPLEMENT(T1_OUT);

    return true;
}


int temphum_click_enable(void)
{
    if (!temphum_init() || !temphum_calibrate()) {
        return -1;
    }
    return 0;
}


int temphum_click_disable(void)
{
    if (write_register(CTRL_REG1, 0x07) < 0) {
        ERROR("Failed to disable the sensor");
        return -1;
    }
    return 0;
}


int temphum_click_get_temperature(float *temperature, float *humidity)
{
    float temp_humidity;
    float temp_temperature;
    int H_OUT;
    int T_OUT;
    uint8_t status;
    uint8_t data[4];

    if (temperature == NULL || humidity == NULL) {
        ERROR("Cannot store temperature or humidity using null pointer.");
        return -1;
    }

    if (read_register(STATUS_REG, &status, 1) < 0) {
       ERROR("Failed to read status register");
       return -1;
    }
    if ((status&0x03) != 0x03) {
       ERROR("Status is not ready yet, 0x%X", status);
       return -1;
    }

    if (read_register(HUMIDITY_OUT_L, data, sizeof (data)) < 0) {
        ERROR("Failed to read the temp and humidity values");
        return -1;
    }

    H_OUT = CONVERT_16B(data[0], data[1]);
    T_OUT = CONVERT_16B(data[2], data[3]);

    CONVERT_NATIVE_2COMPLEMENT(H_OUT);
    CONVERT_NATIVE_2COMPLEMENT(T_OUT);

    temp_temperature = linear_interpolation(T0_OUT, T0_DegC_cal, T1_OUT, T1_DegC_cal, T_OUT);
    temp_humidity    = linear_interpolation(H0_T0_OUT, H0_RH_cal, H1_T0_OUT, H1_RH_cal, H_OUT);

    // Constraint for measurement after calibration
    if ((int)temp_humidity>MaxHumi-1 || (int)temp_humidity==-72) temp_humidity = MaxHumi;
    if ((int)temp_humidity<MinHumi ) temp_humidity = MinHumi;
    if ((int)temp_temperature>MaxTemp-1) temp_temperature = MaxTemp;
    if ((int)temp_temperature<MinTemp ) temp_temperature = MinTemp;

    *temperature  = temp_temperature;
    *humidity     = temp_humidity;
    return 0;
}
