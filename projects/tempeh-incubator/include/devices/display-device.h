
#ifndef _DISPLAY_DEVICE_H
#define _DISPLAY_DEVICE_H

#include <TaskManagerIO.h>

#include "settings.h"

namespace incub {

class DisplayDevice {
 private:
  bool _isOn = false;
  unsigned short _counterForDisplayDataExecutions = 0;
  short _indexForCurrentlyDisplayedErrorMsg = -1;
  void _printData();
  void _printFirstRow();
  void _printSecondRow();
  void _refreshFirstRow();

  // Data on display.
  bool _isHeatingOn = false;

 public:
  void setup();
  void toogle();
  bool isOn();
  void switchOff(bool doResetSwitchOffDisplayTaskId = true);
  void switchOn(bool doCancelExistingSwitchOffDisplayTask = true);
  taskid_t switchOffTaskId = TASKMGR_INVALIDID;
  taskid_t displayDataTaskId = TASKMGR_INVALIDID;
};

// "Soft" singleton global object defined as extern and initialized here,
//  but also defined in led-device.cpp.
extern DisplayDevice displayDevice;

class RowPrinter {
 private:
  const unsigned short _MAX_SIZE = 16;
  unsigned short _currentSize = 0;

 public:
  RowPrinter(unsigned short i);
  unsigned short i;
  template <typename T>
  void print(T value);
  template <typename T1, typename T2>
  void print(T1 value, T2 extra);
  void printFillingBlanks();
  void printRightAlign(char* string);
};

}  // namespace incub
#endif
