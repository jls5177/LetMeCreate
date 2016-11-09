#ifndef __LETMECREATE_CLICK_ALPHANUM_H__
#define __LETMECREATE_CLICK_ALPHANUM_H__

#include <stdint.h>
#include <letmecreate/click/export.h>

/* Switch interval in ms */
#define ALPHANUM_SWITCH_INTERVAL 8

/* Init the alphanum clicker */
int LETMECREATE_CLICK_EXPORT alphanum_click_init(uint8_t mikrobus_index);

/* Converts a char into a 14 segment display value */
int LETMECREATE_CLICK_EXPORT alphanum_click_get_char(char c, uint16_t *value);

/* Write the two chars to */
int LETMECREATE_CLICK_EXPORT alphanum_click_write(char a, char b);

/* Raw write  */
int LETMECREATE_CLICK_EXPORT alphanum_click_raw_write(uint16_t a, uint16_t b);

/* This peridically switch between segment a and b to keep the illusion of a
 * simultanous display of both values
 */
void LETMECREATE_CLICK_EXPORT alphanum_click_switch_cycles(int iteration_count);

#endif
