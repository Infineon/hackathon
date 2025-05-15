/** Project CPP includes. */
#include "TLx493D_inc.hpp"


using namespace ifx::tlx493d;
// Define the number of calibration samples
const int CALIBRATION_SAMPLES = 20;
// Offsets for calibration
double xOffset = 0, yOffset = 0, zOffset = 0;

/* Definition of the power pin and sensor objects for Kit2Go XMC1100 boards. */
//const uint8_t POWER_PIN = 15; // XMC1100 : LED2

// TLx493D_A1B6 dut(Wire, TLx493D_IIC_ADDR_A0_e);

// TLx493D_A2B6 dut(Wire, TLx493D_IIC_ADDR_A0_e);
// TLx493D_P2B6 dut(Wire, TLx493D_IIC_ADDR_A0_e);
// TLx493D_W2B6 dut(Wire, TLx493D_IIC_ADDR_A0_e);


/** Definition of the power pin and sensor objects for S2Go with XMC4700 Relax Lite board. */
// const uint8_t POWER_PIN = 8; // XMC : P1.10

// TLx493D_A1B6 dut(Wire, TLx493D_IIC_ADDR_A0_e);

// TLx493D_A2BW dut(Wire, TLx493D_IIC_ADDR_A0_e);
// TLx493D_W2B6 dut(Wire, TLx493D_IIC_ADDR_A0_e);
// TLx493D_W2BW dut(Wire, TLx493D_IIC_ADDR_A0_e);


/** P3XX evaluation board */
// const uint8_t POWER_PIN = 8;
TLx493D_A2B6 dut(Wire1, TLx493D_IIC_ADDR_A0_e);


/** Definition of the power pin and sensor objects for Arduino Uno boards */
/** Care must be taken to level shift down to 3.3V as the sensor boards expect only 3.3V ! */
/** Therefore disabled here. */
// const uint8_t POWER_PIN = 7;

// TLx493D_W2B6 dut(Wire, TLx493D_IIC_ADDR_A0_e);
// TLx493D_W2BW dut(Wire, TLx493D_IIC_ADDR_A0_e);


/** Definition of a counter variable. */



void setup() {
    Serial.begin(115200);
    delay(3000);

    /** Definition of the power pin to power up the sensor. */
    /** Set delay after power-on to 50 for A1B6 Kit2Go sensor. */
    /** All other Kit2Go boards */


    /** P3XX evaluation board */
    // dut.setPowerPin(POWER_PIN, OUTPUT, INPUT, LOW, HIGH, 1000, 250000);

    dut.begin();

    Serial.print("setup done.\n");
    // Perform calibration
    calibrateSensor();
    Serial.println("Calibration completed.");
}


/** In the loop we continuously reading the temperature value as well as the
 *  magnetic values in X, Y, Z-direction of the sensor and printing them to
 *  the serial monitor
 */
void loop() {
    double  x, y, z;

    dut.setSensitivity(TLx493D_FULL_RANGE_e);
    dut.getMagneticField(&x, &y, &z);
    x-= xOffset;
    y-= yOffset;
    z-= zOffset;




    Serial.print(x);
    Serial.print(",");

    Serial.print(y);
    Serial.print(",");

    Serial.print(z);
    Serial.println("");

    

    delay(500);

}

void calibrateSensor() {
    double sumX = 0, sumY = 0, sumZ = 0;

    for (int i = 0; i < CALIBRATION_SAMPLES; ++i) {
        double temp;
        double valX, valY, valZ;

        dut.getMagneticFieldAndTemperature(&valX, &valY, &valZ, &temp);
        sumX += valX;
        sumY += valY;
        sumZ += valZ;

        delay(10); // Adjust delay as needed
    }

    // Calculate average offsets
    xOffset = sumX / CALIBRATION_SAMPLES;
    yOffset = sumY / CALIBRATION_SAMPLES;
    zOffset = sumZ / CALIBRATION_SAMPLES;
}
