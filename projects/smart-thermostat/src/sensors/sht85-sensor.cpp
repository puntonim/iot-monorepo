#include "sensors/sht85-sensor.h"

#include <SHTSensor.h>

#include "errors.h"
#include "settings.h"

namespace tstat {

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in sht85-sensor.h.
Sht85Sensor sht85Sensor;

// Note: SHT85 I2C address: 0x44.
SHTSensor sht(SHTSensor::SHT85);
// To use autodetect: SHTSensor sht;

void Sht85Sensor::setup() {
  // Wire.begin();
  // It should return a bool, but I tested it disconnecting wires and it always
  //  returns 0;
  sht.init();
  // SHT_ACCURACY_HIGH: highest repeatability at the cost of slower measurement.
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH);
}

// Note: at the moment the sensor SHT85 is only read by the display, so its
// error status is updated
//  only when the display is ON. This means that if the errorLed has been
//  triggered by an error on
//   the SHT85 sensor, then the errorLed status can be delayed until the display
//   is ON again.

/**
 * Read data from the SHT85 sensor.
 *
 * The data is cached for settings.SHT85_DATA_CACHE_PERIOD msec to avoid
 * hammering the sensor.
 */
struct Sht85Data Sht85Sensor::getData(enum Sht85SensorException& exc) {
  // Cache the read data for DS18B20_DATA_CACHE_PERIOD msec.
  // So we avoid hammering sensors.
  auto nowTs = millis();
  if ((_lastDataTs == 0) ||
      ((nowTs - _lastDataTs) > settings::SHT85_DATA_CACHE_PERIOD)) {
    // The cache has expired.

    if (sht.readSample()) {
      _cachedTemperature = sht.getTemperature();
      _cachedHumidity = sht.getHumidity();
    } else {
#if IS_DEBUG == true
      Serial.println((String) "Sht85Sensor - error reading data");
#endif
      _cachedTemperature = SENSOR_ERROR;
      _cachedHumidity = SENSOR_ERROR;
    }
    _lastDataTs = nowTs;
  }

  if (_cachedTemperature == SENSOR_ERROR) {
    exc = Sht85SensorException::SensorError;
    errorManager.addSht85SensorError();
  } else {
    exc = Sht85SensorException::Success;
    errorManager.removeSht85SensorError();
  }

  struct Sht85Data data;
  data.temperature = _cachedTemperature;
  data.humidity = _cachedHumidity;
  return data;
}

}  // namespace tstat