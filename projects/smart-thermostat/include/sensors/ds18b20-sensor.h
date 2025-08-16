#ifndef _DS18B20_SENSOR_H
#define _DS18B20_SENSOR_H

namespace tstat {

enum struct Ds18b20SensorException {
  Success,
  SensorError,
};

class Ds18b20Sensor {
 private:
  unsigned long _lastDataTs = 0;
  float _cachedData;

 public:
  void setup();
  float getData(enum Ds18b20SensorException &exc);
};

// "Soft" singleton global object defined as extern and initialized here,
//  but also defined in ds18b20-sensor.cpp.
extern Ds18b20Sensor ds18b20Sensor;

}  // namespace tstat
#endif
