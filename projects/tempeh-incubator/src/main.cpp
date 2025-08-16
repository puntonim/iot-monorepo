/* TEMPEH INCUBATOR: RELAY, BULB, DISPLAY
 * ======================================
 * A *basic* tempeh incubator:
 *  a. it has a relay to switch an incandescent bulb (60W AC), used as heating
 *      element
 *  b. it keeps the bulb ON for a period of LESS THAN 10 mins consecutive
 *  c. it keeps the bulb OFF for a period of AT LEAST 5 mins consecutive
 * Note: b. and c. are needed only because I am still testing the relay: it
 *  supports a big load (30A at 220V AC), however I want to be extra careful
 *  and make sure it doesn't heat up too much. Once I am confident, I will
 *  remove b. and c..
 *
 * The *basic* tempeh incubator is built with:
 *  - inside a hoven, with the door ajar, but partially closed by a kitchen
 *     towel such that the open space (where air is coming out) is
 *     about 3x1 cm:
 *    - feet blanket heater AC, always on at max level 6/6, connected to a wall
 *       socket;
 *    - incandescent bulb 60W AC connected to a wall socket and to the relay
 *       controlled by Arduino;
 *    - kitchen towel soaked in water, to keep himidity high;
 *  - outside the hove:
 *    - arduino with all the electronic components described here;
 *  - it's winter and the room T is ~10-13°C and humidity 60%.
 *
 * Display: show the temperatures and humidity detected by sensors.
 * Rotary encoders buttons: switch on the display. Note: we don't really need
 *  rotary encoders, a regular button would work too, but that's what I have
 *  wired ATM so I am reusing it.
 *
 * Devices:
 *  1xSHT-85: thermometer and hygrometer sensor.
 *  1xDS18B20: thermometer sensor.
 *  2x rotary encoder: to switch on the display. Note: we don't really need
 *   rotary encoders, a regular button would work too, but that's what I have
 *   wired ATM so I am reusing it.
 *  1x display I2C 16x2.
 *  1x relay Seeed Studio Grove SPDT 30A 250V AC: to control an incandescent
 *   bulb (60W AC).
 *
 * Libs:
 *  - arduino-sht by Johannes Winkelmann, Andreas Brauchli
 *    https://github.com/Sensirion/arduino-sht
 *  - DallasTemperature by Miles Burton
 *    https://docs.arduino.cc/libraries/dallastemperature/
 *  - LiquidCrystalIO by Arduino, Adafruit, theCodersCorner
 *    https://docs.arduino.cc/libraries/liquidcrystalio/
 *  - TaskManagerIO by davetcc
 *    https://github.com/TcMenu/TaskManagerIO
 *  - IoAbstraction by tcmenu
 *    https://docs.arduino.cc/libraries/ioabstraction/
 */

#include <Arduino.h>

#include <list>

#include "devices/display-device.h"
#include "devices/led-device.h"
#include "devices/relay-device.h"
#include "devices/rotary-encoder-device.h"
#include "domain/heating-domain.h"
#include "sensors/ds18b20-sensor.h"
#include "sensors/sht85-sensor.h"
#include "utils/list-utils.h"
#include "utils/task-manager-utils.h"

using namespace incub;

void setup() {
#if IS_DEBUG == true
  Serial.begin(57600);
  unsigned short i = 30;
  while (!Serial && i--) delay(1);
  Serial.println("START");
#endif

  // Order matters as events subscribers' callbacks are invoked in this order.
  sht85Sensor.setup();
  ds18b20Sensor.setup();
  rotaryEncoderDevices.setup();
  errorLedDevice.setup();
  domainLedDevice.setup();
  relayDevice.setup();
  displayDevice.setup();
  heatingDomain.setup();
}

void loop() {
  // To debug TaskManagerIO's tasks.
  // tstat::task_manager_utils::printDebugTasks(false);

  taskManager.runLoop();
}
