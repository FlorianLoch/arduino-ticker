#include "Adafruit_Si7021.h"

class CachingTempReader
{
private:
  Adafruit_Si7021 sensor;
  long lastReadTS = 0;
  float cache;
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

  float readTemperature()
  {
    if (this->lastReadTS + this->interval < millis())
    {
      this->cache = this->sensor.readTemperature();
      this->lastReadTS = millis();

      Serial.printf("%.2f °C\n", this->cache);
    }

    return this->cache;
  }
};