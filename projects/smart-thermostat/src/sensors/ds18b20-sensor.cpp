#include "sensors/ds18b20-sensor.h"

#include <DallasTemperature.h>

#include "errors.h"
#include "settings.h"

namespace tstat {

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in ds18b20-sensor.h.
Ds18b20Sensor ds18b20Sensor;

OneWire _oneWire(settings::DS18B20_SENSOR_PIN);
DallasTemperature _probes(&_oneWire);

void Ds18b20Sensor::setup() {
  _probes.begin();
  // The resolution of the temperature sensor is user-configurable to 9, 10, 11,
  //  or 12 bits, corresponding to increments of 0.5°C, 0.25°C, 0.125°C, and
  //  0.0625°C, respectively.
  //  The default resolution at power-up is 12-bit.
  //  Src: https//
  //  www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf
  //  However the default resolution used by the DallasTemperature is 9 bit.
  _probes.setResolution(12);
}

/**
 * Read data from the DS18B20 sensor.
 *
 * The data is cached for settings.DS18B20_DATA_CACHE_PERIOD msec to avoid
 * hammering the sensor.
 *
 * Example:
 *    Ds18b20SensorException exc;
 *    float sensorTemp = ds18b20Sensor.getData(exc);
 *    if (exc == Ds18b20SensorException::SensorError) ...
 */
float Ds18b20Sensor::getData(enum Ds18b20SensorException &exc) {
  // Cache the read data for DS18B20_DATA_CACHE_PERIOD msec.
  // So we avoid hammering sensors.
  auto nowTs = millis();
  if ((_lastDataTs == 0) ||
      ((nowTs - _lastDataTs) > settings::DS18B20_DATA_CACHE_PERIOD)) {
    // The cache has expired.

    //_probes.requestTemperatures();
    // By using requestTemperaturesByAddress(ADDRESS) we avoid a delay when
    //  no sensor is detected (as the library blocks for ~1sec waiting for
    //  any sensor).
    _probes.requestTemperaturesByAddress(settings::DS18B20_SENSOR_ADDRESS);
    _cachedData = _probes.getTempC(settings::DS18B20_SENSOR_ADDRESS);
    _lastDataTs = nowTs;
  }

  if (_cachedData == DEVICE_DISCONNECTED_C) {
#if IS_DEBUG == true
    Serial.println((String) "Ds18b20Sensor - error reading data");
#endif
    exc = Ds18b20SensorException::SensorError;
    errorManager.addDs18b20SensorError();
    return SENSOR_ERROR;
  }

  exc = Ds18b20SensorException::Success;
  errorManager.removeDs18b20SensorError();

  return _cachedData;
}

}  // namespace tstat
