#include "devices/relay-device.h"

#include "settings.h"
#include "utils/pubsub-utils.h"

namespace tstat {

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in relay-device.h.
RelayDevice relayDevice;

void RelayDevice::setup() {
  pinMode(_PIN, OUTPUT);

  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::HeatingStatusUpdateEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println(
            (String) "RelayDevice - received event: " + pEvent->topic +
            " isOn=" + (pEvent->isOn ? "ON" : "OFF"));
#endif
        pEvent->isOn ? this->_switchOn() : this->_switchOff();
      });

  pubsub_utils::pubSub.subscribe([this](
                                     pubsub_utils::SchedulerEndEvent* pEvent) {
#if IS_DEBUG == true
    Serial.println((String) "RelayDevice - received event: " + pEvent->topic);
#endif
    this->_switchOff();
  });
}

void RelayDevice::_switchOn() {
  if (!_isOn) {
    digitalWrite(_PIN, HIGH);
    _isOn = true;
  }
}

void RelayDevice::_switchOff() {
  if (_isOn) {
    digitalWrite(_PIN, LOW);
    _isOn = false;
  }
}

}  // namespace tstat