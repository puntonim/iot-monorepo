#ifndef _HEATING_DOMAIN_H
#define _HEATING_DOMAIN_H

#include <TaskManagerIO.h>

namespace incub {

class HeatingDomain {
 private:
  bool _isOn = false;
  void _switchOn();
  void _switchOff();
  void _onError();
  void _end();
  unsigned long _lastSwitchTs = 0;

 public:
  void setup();
  void runCheck();
  taskid_t runTaskId = TASKMGR_INVALIDID;
};

// "Soft" singleton global object defined as extern and initialized here,
//  but also defined in heating-domain.cpp.
extern HeatingDomain heatingDomain;

}  // namespace incub
#endif
