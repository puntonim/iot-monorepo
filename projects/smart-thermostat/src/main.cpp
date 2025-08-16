/* THERMOSTAT, LED, DISPLAY
 * ========================
 * Display:
 *   1 row: ON|OFF desired-temp (starts at 18 degrees).
 *   2 row: temp e humidity from sensors.
 *   The display switches off automatically after 20 seconds.
 *   The data shown on the display is updated every 1 sec.
 * Display button: switch on the display. Switch off the heating when hold.
 * Temperature button: increase the target temperature.
 * Timer button: increase the timer.
 * LED yellow: blinks when heating is ON, so when sensor T < desired T.
 * LED red: blinks when there is an error. Read the error on the display.
 * LED white: ON while the heating domain is running.
 *
 * Devices:
 *  1xSHT-85: thermometer and hygrometer sensor
 *  1xDS18B20: thermometer sensor
 *  3x push button
 *  1x display I2C 16x2
 *  3x LEDs
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
#include "domain/scheduler-domain.h"
#include "sensors/ds18b20-sensor.h"
#include "sensors/sht85-sensor.h"
#include "utils/list-utils.h"
#include "utils/task-manager-utils.h"

using namespace tstat;

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
  schedulerDomain.setup();
  heatingDomain.setup();
  relayDevice.setup();
  displayDevice.setup();
}

void loop() {
  // To debug TaskManagerIO's tasks.
  // tstat::task_manager_utils::printDebugTasks(false);

  taskManager.runLoop();
}
