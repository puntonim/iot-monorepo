#ifndef _ERRORS_H
#define _ERRORS_H

#include <list>

#define SENSOR_ERROR -127

namespace tstat {

class ErrorManager {
 private:
  bool _isDs18b20SensorError = false;
  // TODO is this correct or should this be: char _DS18B20_SENSOR_ERROR_MSG[] =
  // "ERRORE T ds18b20";???????
  char* _DS18B20_SENSOR_ERROR_MSG = "ERRORE T ds18b20";
  bool _isSht85SensorError = false;
  char* _SHT85_SENSOR_ERROR_MSG = "ERRORE T sht85";
  // Using a `list` (doubly-linked list) and not a `forward_list` (singly-linked
  //  list) because it needs to operate like a queue (with `push_back()`). And
  //  not using a `queue` because we need to be able to `remove` any element.
  std::list<char*> _errorMessageListForDisplay;
  void _handleErrors();

 public:
  void addDs18b20SensorError();
  void removeDs18b20SensorError();
  bool isDs18b20SensorError();
  void addSht85SensorError();
  void removeSht85SensorError();
  bool isSht85SensorError();
  bool areThereErrors();
  std::list<char*> getErrorMessageListForDisplay();
};

// "Soft" singleton global object defined as extern and initialized here,
//  but also defined in errors.cpp.
extern ErrorManager errorManager;

}  // namespace tstat
#endif
