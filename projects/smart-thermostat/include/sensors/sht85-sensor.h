#ifndef _SHT85_SENSOR_H
#define _SHT85_SENSOR_H

namespace tstat {

struct Sht85Data {
  float temperature;
  float humidity;
};

enum struct Sht85SensorException {
  Success,
  SensorError,
};

class Sht85Sensor {
 private:
  unsigned long _lastDataTs = 0;
  float _cachedTemperature;
  float _cachedHumidity;

 public:
  void setup();
  struct Sht85Data getData(enum Sht85SensorException& exc);
};

// "Soft" singleton global object defined as extern and initialized here,
//  but also defined in sht85-sensor.cpp.
extern Sht85Sensor sht85Sensor;

}  // namespace tstat
#endif
