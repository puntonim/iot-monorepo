#include "domain/heating-domain.h"

#include <TaskManagerIO.h>

#include "devices/led-device.h"
#include "errors.h"
#include "sensors/ds18b20-sensor.h"
#include "sensors/sht85-sensor.h"
#include "settings.h"
#include "utils/pubsub-utils.h"
#include "utils/task-manager-utils.h"
#include "utils/time-utils.h"

namespace incub {

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in heating-domain.h.
HeatingDomain heatingDomain;

void HeatingDomain::setup() {
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::ErrorStatusUpdateEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println(
            (String) "HeatingDomain - received event: " + pEvent->topic +
            " isError=" + (pEvent->isError ? "ON" : "OFF"));
#endif
        if (pEvent->isError) this->_onError();
      });

  // Execute `runCheck()` straight away, then schedule a periodic execution.
  runCheck();
#if IS_DEBUG == true
  Serial.println("HeatingDomain - starting a new run task");
#endif
  runTaskId = taskManager.scheduleFixedRate(settings::HEATING_DOMAIN_RUN_PERIOD,
                                            [] { heatingDomain.runCheck(); });
}

void HeatingDomain::runCheck() {
#if IS_DEBUG == true
  Serial.println("HeatingDomain - run check");
#endif
  domainLedDevice.switchOn(true);

  Ds18b20SensorException exc;
  // Reading only DS18B20 temp for now because that is the tempeh cake T.
  // And ignoring SHT85 sensor for now because that is the air T and humidity.
  float sensorTemp = ds18b20Sensor.getData(exc);
  if (exc != Ds18b20SensorException::Success) {
#if IS_DEBUG == true
    Serial.println("HeatingDomain - ERROR on reading Ds18b20 temperature");
#endif
    return;
  }
  auto nowTs = millis();

#if IS_DEBUG == true
  Serial.println((String) "HeatingDomain - Ds18b20 T=" + sensorTemp);
#endif

  // -------------- DECISION LOGIC -------------------
  // Note: tempeh cake temperature: 32°C, air humidity: 50-75%
  // - If heating is OFF:
  //     - AND T <= 30.5C
  //       AND ALSO it's been OFF for at least 5 mins: switch ON
  // - If heating is ON:
  //     - AND T >=32C: switch OFF
  //     - OR it's been ON for at least 10 min: switch OFF

  if (!_isOn) {
    // Heating is OFF.
    if ((sensorTemp <= settings::LOW_TARGET_T) &&
        ((_lastSwitchTs == 0) ||  // Arduino has just been powered ON.
         ((nowTs - _lastSwitchTs) > settings::HEATING_ON_DEBOUNCE_PERIOD))) {
      _switchOn();
    }
  } else {
    // Heating is ON.
    if ((sensorTemp >= settings::HIGH_TARGET_T) ||
        ((nowTs - _lastSwitchTs) > settings::HEATING_ON_MAX_PERIOD)) {
      _switchOff();
    }
  }

  domainLedDevice.switchOff(true);
}

void HeatingDomain::_switchOn() {
  if (!_isOn) {
#if IS_DEBUG == true
    Serial.println((String) "HeatingDomain - switching ON");
#endif
    _isOn = true;
    _lastSwitchTs = millis();
    pubsub_utils::pubSub.publish(
        new pubsub_utils::HeatingStatusUpdateEvent(true));
  }
}

void HeatingDomain::_switchOff() {
  if (_isOn) {
#if IS_DEBUG == true
    Serial.println((String) "HeatingDomain - switching OFF");
#endif
    _isOn = false;
    _lastSwitchTs = millis();
    pubsub_utils::pubSub.publish(
        new pubsub_utils::HeatingStatusUpdateEvent(false));
  }
}

/**
 * In case of error in Ds18b20 temoperature sensor (the one used in the
 *  DECISION LOGIC), then switch OFF the heating.
 */
void HeatingDomain::_onError() {
  if (errorManager.isDs18b20SensorError()) _switchOff();
}

/**
 * Stop: switch off the heating and cancel the ongoing schedule.
 */
void HeatingDomain::_end() {
  if (_isOn) _switchOff();
  task_manager_utils::cancelTask(runTaskId);
}

}  // namespace incub
