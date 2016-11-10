#include <stdio.h>
#include <letmecreate/letmecreate.h>

/*
 * Example how to write a text to the OLED display.
 */
int main(void)
{
    i2c_init();
    oled_click_init();
    oled_click_write_text("Hello   Creator!");

    return 0;
}