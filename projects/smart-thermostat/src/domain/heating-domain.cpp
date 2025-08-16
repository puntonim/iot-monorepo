#include "domain/heating-domain.h"

#include <TaskManagerIO.h>

#include "devices/led-device.h"
#include "domain/scheduler-domain.h"
#include "sensors/ds18b20-sensor.h"
#include "sensors/sht85-sensor.h"
#include "settings.h"
#include "utils/pubsub-utils.h"
#include "utils/task-manager-utils.h"
#include "utils/time-utils.h"

namespace tstat {

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in heating-domain.h.
HeatingDomain heatingDomain;

void HeatingDomain::setup() {
  // SUBSCRIPTION SchedulerStartEvent ------------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::SchedulerStartEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "HeatingDomain - received event: " +
                       pEvent->topic);
#endif
        this->_onSchedulerStartEvent();
      });

  // SUBSCRIPTION SchedulerTimerUpdateEvent ------------------------------------
  // When the timer has changed (because the timer rotary encoder
  //  was rotated) then we want the runCheck() to run asap so the relay
  //  can turn ON/OFF instantly.
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::SchedulerTimerUpdateEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "HeatingDomain - received event: " +
                       pEvent->topic);
#endif
        this->runCheck();
      });

  // SUBSCRIPTION SchedulerTargetTUpdateEvent ----------------------------------
  // When the target T has changed (because the target T rotary encoder
  //  was rotated) then we want the runCheck() to run asap so the relay
  //  can turn ON/OFF instantly.
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::SchedulerTargetTUpdateEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "HeatingDomain - received event: " +
                       pEvent->topic);
#endif
        this->runCheck();
      });

  // SUBSCRIPTION SchedulerEndEvent --------------------------------------------
  pubsub_utils::pubSub.subscribe([this](
                                     pubsub_utils::SchedulerEndEvent* pEvent) {
#if IS_DEBUG == true
    Serial.println((String) "HeatingDomain - received event: " + pEvent->topic);
#endif
    this->_end();
  });
}

void HeatingDomain::runCheck() {
#if IS_DEBUG == true
  Serial.println("HeatingDomain - run check");
#endif
  domainLedDevice.switchOn(true);

  // When the timer is over the event SchedulerEndEvent is published, so we
  //  we don't even have to check the timer, but only the sensor T.
  schedulerDomain.tick();
  bool isOnForTemp = _checkForTemperature();
  bool isOnForTime = _checkForTimer();

  // -------------- DECISION TABLE -------------------
  // isOnForTime   isOnForTemp   status     action to take
  // -----------   -----------   ------     --------------
  //     F           T or F        F     -> noop
  //                               T     -> switch OFF
  //     T             T           T     -> noop
  //                               F     -> switch ON
  //     T             F           F     -> noop
  //                               T     -> switch OFF
  //
  // Notes:
  //  - `isOnForTime` is true when the timer is not over yet;
  //  - `isOnForTemp` is true when the sensor T is < target T
  //  - `status` is true when the heating is ON;

  // When the timer is over the event SchedulerEndEvent is published, so it
  //  might seem that we don't even have to check the timer. However this
  //  can happen: rotate timer rotary to 0 > SchedulerTimerUpdateEvent >
  //  runCheck() > tick() > SchedulerEndEvent > switchOff and then this code
  //  runs and if we don;t check for time we switch on again for temp.
  if (!isOnForTime)
    _end();
  else {
    if (isOnForTemp && (!_isOn))
      _switchOn();
    else if ((!isOnForTemp) && _isOn)
      _switchOff();
  }

  domainLedDevice.switchOff(true);
}

bool HeatingDomain::_checkForTemperature() {
  Ds18b20SensorException exc;
  // Reading only DS18B20 temp for now (and ignoring SHT85 sensor).
  float sensorTemp = ds18b20Sensor.getData(exc);
  // There is no need to handle the exception because, in case of sensor error,
  //  a message is published and the subscribed callbacks are invoked.
  // if (exc == Ds18b20SensorException::SensorError)

  // TODO replace this with the PID algo:
  // https://playground.arduino.cc/Code/PIDLibrary/ if ((sensorTemp <
  // settings.TARGET_T) && !_isOn) _switchOn(); else if
  // ((sensorTemp >= settings.TARGET_T) && _isOn) _switchOff();
  return sensorTemp < schedulerDomain.targetTemperature;
}

bool HeatingDomain::_checkForTimer() {
  schedulerDomain.timer.tick();
  return !schedulerDomain.timer.isOver();
}

void HeatingDomain::_switchOn() {
  if (!_isOn) {
    _isOn = true;
    pubsub_utils::pubSub.publish(
        new pubsub_utils::HeatingStatusUpdateEvent(true));
  }
}

void HeatingDomain::_switchOff() {
  if (_isOn) {
    _isOn = false;
    pubsub_utils::pubSub.publish(
        new pubsub_utils::HeatingStatusUpdateEvent(false));
  }
}

void HeatingDomain::_onSchedulerStartEvent() {
  // Execute `runCheck` so it reacts asap. Without this
  //  it would run after the period set in scheduleFixedRate, so 1 sec.
  runCheck();
#if IS_DEBUG == true
  Serial.println("HeatingDomain - starting a new run task");
#endif
  if (runTaskId != TASKMGR_INVALIDID) {
    // BUG if this happens then we run one more runTaskId every time we press
    //  the timer or temperature button. This will result in a freeze as
    //  the memory and cpu gets full of tasks.
#if IS_DEBUG == true
    Serial.println(
        "!!BUG!! HeatingDomain - starting a new run task but there is"
        " already at least 1 existing");
#endif
  } else
    runTaskId = taskManager.scheduleFixedRate(
        settings::HEATING_DOMAIN_RUN_PERIOD, [] { heatingDomain.runCheck(); });
}

/**
 * Stop: switch off the heating and cancel the ongoing schedule.
 */
void HeatingDomain::_end() {
  if (_isOn) _switchOff();
  task_manager_utils::cancelTask(runTaskId);
}

}  // namespace tstat
