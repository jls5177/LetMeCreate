/**
 * @file temphum.h
 * @author Justin Simon
 * @date 2018
 * @copyright 3-clause BSD
 *
 * @example temphum/main.c
 */


#ifndef __LETMECREATE_CLICK_TEMPHUM_H__
#define __LETMECREATE_CLICK_TEMPHUM_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <letmecreate/click/export.h>

/**
 * @brief Enable Temp&Hum click.
 *
 * @param add_bit temphum click has a jumper on its board to change its address (must be 0 or 1)
 * @return 0 if successful, otherwise it returns -1.
 */
int LETMECREATE_CLICK_EXPORT temphum_click_enable(void);

/**
 * @brief Get a temperature measure (in degrees Celsius) from Temp&Hum click.
 *
 * @param[out] temperature Pointer to a 16-bit variable for writing temperature read from Temp&Hum click
 * @return 0 if successful, otherwise it returns -1.
 */
int LETMECREATE_CLICK_EXPORT temphum_click_get_temperature(float *temperature, float *humidity);

/**
 * @brief Disable Temp&Hum click.
 *
 * @return 0 if successful, otherwise it returns -1.
 */
int LETMECREATE_CLICK_EXPORT temphum_click_disable(void);

#ifdef __cplusplus
}
#endif

#endif
