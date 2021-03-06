#include <stdbool.h>
#include <stdio.h>
#include <letmecreate/click/opto.h>
#include <letmecreate/core/common.h>
#include <letmecreate/core/gpio.h>
#include <letmecreate/core/gpio_monitor.h>


static const uint8_t channel_pins[OPTO_CLICK_CHANNEL_COUNT] = {
    TYPE_INT,
    0,              /* Channel 2 is on CS pin, so ignore it */
    TYPE_RST,
    TYPE_AN
};

static bool check_channel_index(uint8_t channel_index)
{
    if (channel_index == OPTO_CLICK_CHANNEL_2) {
        fprintf(stderr, "opto: Channel 2 is not supported on Ci40.\n");
        return false;
    }

    if (channel_index >= OPTO_CLICK_CHANNEL_COUNT) {
        fprintf(stderr, "opto: Invalid channel index.\n");
        return false;
    }

    return true;
}

int opto_click_attach_callback(uint8_t mikrobus_index, uint8_t channel_index, void (*callback)(uint8_t))
{
    uint8_t gpio_pin = 0;

    if (check_valid_mikrobus(mikrobus_index) < 0
    ||  check_channel_index(channel_index) == false)
        return -1;

    if (callback == NULL) {
        fprintf(stderr, "opto: Cannot attach null callback.\n");
        return -1;
    }

    if (gpio_get_pin(mikrobus_index, channel_pins[channel_index], &gpio_pin) < 0
    ||  gpio_init(gpio_pin) < 0
    ||  gpio_monitor_init() < 0
    ||  gpio_monitor_add_callback(gpio_pin, GPIO_EDGE, callback) < 0)
        return -1;

    return 0;
}

int opto_click_read_channel(uint8_t mikrobus_index, uint8_t channel_index, uint8_t *state)
{
    uint8_t gpio_pin = 0;

    if (check_valid_mikrobus(mikrobus_index) < 0
    ||  check_channel_index(channel_index) == false)
        return -1;

    if (state == NULL) {
        fprintf(stderr, "opto: Cannot store state using null pointer.\n");
        return -1;
    }

    if (gpio_get_pin(mikrobus_index, channel_pins[channel_index], &gpio_pin) < 0
    ||  gpio_init(gpio_pin) < 0
    ||  gpio_get_value(gpio_pin, state) < 0)
        return -1;

    return 0;
}
