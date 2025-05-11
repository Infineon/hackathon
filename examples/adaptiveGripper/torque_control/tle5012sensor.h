#ifndef TLE5012SENSOR_H
#define TLE5012SENSOR_H

#include <SimpleFOC.h>
#include <tlx5012-arduino.hpp>

using namespace tle5012;

class TLE5012Sensor : public Sensor {
public:
    // Constructor
    TLE5012Sensor(SPIClass3W* spi, int csPin, int misoPin, int mosiPin, int sckPin);

    // Initialize the sensor hardware
    void init();

    // Get the current shaft angle from the sensor hardware
    float getSensorAngle();

private:
    Tle5012Ino _sensor; // Instance of the TLE5012 sensor
    errorTypes _checkError; // Variable to store sensor initialization errors
};

#endif