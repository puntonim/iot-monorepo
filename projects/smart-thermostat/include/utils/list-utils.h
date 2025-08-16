#ifndef _LIST_UTILS_H
#define _LIST_UTILS_H

#include <list>

namespace tstat {
namespace list_utils {

// Note: this function is implemented here and not in a proper .cpp file
//  because in that case it generates a compile error:
//  .pio/build/uno_r4_wifi/src/devices/display-device.cpp.o: In function
//  `tstat::DisplayDevice::_printSecondRow()':
//  display-device.cpp:(.text._ZN5tstat13DisplayDevice15_printSecondRowEv+0x9e):
//  undefined reference to `char*
//  tstat::list_utils::getByIndex<char*>(std::__cxx11::list<char*,
//  std::allocator<char*> >, short)'
template <typename T>
static T getByIndex(std::list<T> l, short i) {
  // TODO maybe we should use the same exc-like mechanism as in
  // ds18b20-sensor.ino.
  if (i >= l.size()) return nullptr;
  auto it = l.begin();
  std::advance(it, i);
  return *it;

  // TODO
  // This function is useless as this would be enough:
  // char* msg = (i < l.size()) ? std::next(mylist.begin(), i) : nullptr;
}

}  // namespace list_utils
}  // namespace tstat
#endif
