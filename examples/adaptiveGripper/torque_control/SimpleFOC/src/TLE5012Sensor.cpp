#include "TLE5012Sensor.h"

// Constructor
TLE5012Sensor::TLE5012Sensor(SPIClass3W* spi, int csPin, int misoPin, int mosiPin, int sckPin)
    : _sensor(spi, csPin, misoPin, mosiPin, sckPin) {}

// Initialize the sensor hardware
void TLE5012Sensor::init() {
    _checkError = _sensor.begin();
    if (_checkError != NO_ERROR) {
        Serial.print("TLE5012 sensor initialization error: ");
        Serial.println(_checkError, HEX);
    } else {
        Serial.println("TLE5012 sensor initialized successfully!");
    }
}

// Get the current shaft angle from the sensor hardware
float TLE5012Sensor::getSensorAngle() {
    double angle = 0.0;
    _sensor.getAngleValue(angle); // Read the angle from the sensor

    // Convert the angle to radians and adjust it to the range [0, 2]
    float angleRad = (angle + 180.0) * (M_PI / 180.0); // Convert degrees to radians
    if (angleRad > 2 * M_PI) {
        angleRad -= 2 * M_PI; // Ensure the angle is within 0 to 2
    }
    return angleRad;
}