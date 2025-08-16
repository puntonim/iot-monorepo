#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <DallasTemperature.h>
#include <IoAbstraction.h>

#include "utils/time-utils.h"

// Set IS_DEBUG to false on PRODUCTION.
#define IS_DEBUG true
// Set DO_ENABLE_FILLING_BLANKS to true to fill each row of the display with
//  blank spaces to overwrite old text, longer than the one we are displaying.
//  Howerver this can hide some bugs (concurrent prints) so we might disable it
//  on dev by setting DO_ENABLE_FILLING_BLANKS = false.
#define DO_ENABLE_FILLING_BLANKS true

namespace incub {
namespace settings {

// Low and high T.
const float LOW_TARGET_T = 31.8;
const float HIGH_TARGET_T = 32.7;

// Display.
// Seconds after which the display switches off.
const unsigned short DISPLAY_SWITCHOFF_TIMER = 20;  // sec.

// Relay.
const pinid_t RELAY_S_PIN = 11;

// Heating domain.
const unsigned int HEATING_DOMAIN_RUN_PERIOD = 30 * 1000;  // msec, 30 secs.
// The heating will NOT switch ON, if it was ON sooner than this period.
const unsigned int HEATING_ON_DEBOUNCE_PERIOD = 5 * 60 * 1000;  // msec, 5 min.
const unsigned int HEATING_ON_MAX_PERIOD = 10 * 60 * 1000;      // msec, 10 min.

// Rotary encoders.
const pinid_t TARGET_T_ROTARY_CLK_PIN = 2;
const pinid_t TARGET_T_ROTARY_DT_PIN = 3;
const pinid_t TARGET_T_ROTARY_SW_PIN = 4;
const pinid_t TIMER_ROTARY_CLK_PIN = 5;
const pinid_t TIMER_ROTARY_DT_PIN = 6;
const pinid_t TIMER_ROTARY_SW_PIN = 7;

// DS18B20 sensor.
const pinid_t DS18B20_SENSOR_PIN = 8;
const DeviceAddress DS18B20_SENSOR_ADDRESS = {40, 255, 100, 31,
                                              94, 120, 150, 103};
const unsigned short DS18B20_DATA_CACHE_PERIOD = 1000;  // msec.

// DS18B20 sensor.
const unsigned short SHT85_DATA_CACHE_PERIOD = 1000;  // msec.

// Leds.
const pinid_t ERROR_LED_PIN = 10;
const unsigned short ERROR_LED_BRIGHTNESS_VALUE = 100;
const unsigned short ERROR_LED_BLINKING_PERIOD = 100;  // msec.
const pinid_t DOMAIN_LED_PIN = 9;
const unsigned short DOMAIN_LED_BRIGHTNESS_VALUE = 20;

}  // namespace settings
}  // namespace incub
#endif
