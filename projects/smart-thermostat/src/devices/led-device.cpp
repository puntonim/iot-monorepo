#include "devices/led-device.h"

#include "settings.h"
#include "utils/pubsub-utils.h"
#include "utils/task-manager-utils.h"

namespace tstat {

//********** CLASS ErrorLedDevice **********************************************

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in led-device.h.
ErrorLedDevice errorLedDevice;

// Note: at the moment the sensor SHT85 is only read by the display, so its
// error status is updated
//  only when the display is ON. This means that if the errorLed has been
//  triggered by an error on
//   the SHT85 sensor, then the errorLed status can be delayed until the display
//   is ON again.

void ErrorLedDevice::setup() {
  pinMode(_PIN, OUTPUT);

  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::ErrorStatusUpdateEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println(
            (String) "ErrorLedDevice - received event: " + pEvent->topic +
            " isError=" + (pEvent->isError ? "ON" : "OFF"));
#endif
        pEvent->isError ? this->startBlinking() : this->stopBlinking();
      });
}

void ErrorLedDevice::switchOn() {
  if (!_isOn) {
    analogWrite(_PIN, _BRIGHTNESS_VALUE);
    _isOn = true;
  }
}

void ErrorLedDevice::switchOff() {
  if (_isOn) {
    analogWrite(_PIN, 0);
    _isOn = false;
  }
}

void ErrorLedDevice::toggle() { _isOn ? switchOff() : switchOn(); }

void ErrorLedDevice::startBlinking() {
  // Start the blinking task only if it's not already running.
  if (blinkTaskId == TASKMGR_INVALIDID) {
#if IS_DEBUG == true
    Serial.println((String) "ErrorLedDevice - starting a new blinking task");
#endif
    blinkTaskId = taskManager.scheduleFixedRate(
        settings::ERROR_LED_BLINKING_PERIOD, [] { errorLedDevice.toggle(); });
  }
}

void ErrorLedDevice::stopBlinking() {
#if IS_DEBUG == true
  Serial.println((String) "ErrorLedDevice - stopping the blinking task");
#endif
  task_manager_utils::cancelTask(blinkTaskId);
  // Make sure the led is off as the task might have left it on.
  switchOff();
}

//********** CLASS DomainLedDevice *********************************************

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in led-device.h.
DomainLedDevice domainLedDevice;

void DomainLedDevice::setup() { pinMode(_PIN, OUTPUT); }

/**
 * Switch ON the led.
 *
 * Args:
 *  const bool doUseDelay: when switching ON/OFF the led too fast, then the
 *   led does NOT even blink. To avoid this, use doUseDelay=true so that the
 *   led waits 200msec if the prev state change was too soon.
 */
void DomainLedDevice::switchOn(const bool doUseDelay /* = false */) {
  unsigned long nowTs;
  if (doUseDelay) {
    nowTs = millis();
    if ((!_isOn) && (_lastStatusChangeTs != 0) &&
        (nowTs - _lastStatusChangeTs < 200)) {
      taskManager.yieldForMicros(200000);  // microsec.
    }
  }
  if (!_isOn) {
    analogWrite(_PIN, _BRIGHTNESS_VALUE);
    _isOn = true;
    if (doUseDelay) _lastStatusChangeTs = nowTs;
  }
}

/**
 * Switch OFF the led.
 *
 * Args:
 *  const bool doUseDelay: when switching ON/OFF the led too fast, then the
 *   led does NOT even blink. To avoid this, use doUseDelay=true so that the
 *   led waits 200msec if the prev state change was too soon.
 */
void DomainLedDevice::switchOff(const bool doUseDelay /* = false */) {
  unsigned long nowTs;
  if (doUseDelay) {
    nowTs = millis();
    if ((_isOn) && (_lastStatusChangeTs != 0) &&
        (nowTs - _lastStatusChangeTs < 200)) {
      taskManager.yieldForMicros(200000);  // microsec.
    }
  }
  if (_isOn) {
    analogWrite(_PIN, 0);
    _isOn = false;
    if (doUseDelay) _lastStatusChangeTs = nowTs;
  }
}

}  // namespace tstat
