#ifndef _RELAY_DEVICE_H
#define _RELAY_DEVICE_H

#include "settings.h"

namespace tstat {

class RelayDevice {
 private:
  bool _isOn = false;
  const pinid_t _PIN = settings::RELAY_S_PIN;
  void _switchOn();
  void _switchOff();

 public:
  void setup();
};

// "Soft" singleton global object defined as extern and initialized here,
//  but also defined in relay-device.cpp.
extern RelayDevice relayDevice;

}  // namespace tstat
#endif
