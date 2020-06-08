#include "Adafruit_Si7021.h"

class CachingTempReader
{
private:
  Adafruit_Si7021 sensor;
  long lastReadTS = 0;
  float tempCache;
  float humidCache;
  long interval;

public:
  CachingTempReader(long interval = 2000) : interval{interval}
  {
    this->lastReadTS = -interval;
  };

  bool begin()
  {
    // notice: the sensor used in this project is using 0x76 as I2C address, not the default one (0x77)
    return this->sensor.begin();
  };

  bool refresh() {
    if (this->lastReadTS + this->interval < millis())
    {
      this->tempCache = this->sensor.readTemperature();
      this->humidCache = this->sensor.readHumidity();

      this->lastReadTS = millis();

      return true;
    }

    return false;
  };

  float getTemperature()
  {
    return this->tempCache;
  };

  float getHumidity() {
    return this->humidCache;
  };
};