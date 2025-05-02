#include <tle94112-ino.hpp>
#include "Arduino_BMI270_BMM150.h"

// PID parameters
float Kp = 1.5;     // Proportional factor
float Ki = 0.81;    // Integral factor
float Kd = 0.03;    // Derivative factor
float alpha = 0.91; // Filter coefficient
float setpoint = 0.0;
float previous_error = 0.0;  // Previous error for derivative calculation
float integral = 0.0;        // Integral term for PID

float accAngle, gyroRate, angle;
float dt = 0.01;

float errorSum = 0;
float lastError = 0;
unsigned long lastTime = 0;

bool debugEnabled = true;  // Debug mode toggle

//! Tle94112 registers for motor 1 and 2
uint8_t motorReg[2][2] = {
  {REG_ACT_1, REG_PWM_DC_1},
  {REG_ACT_3, REG_PWM_DC_3}
};

//! Tle94112 register for motor direction
volatile uint8_t oldDirection [] = { LL_HH, HH_LL };

// Tle94112 object for motor controller
Tle94112Ino controller = Tle94112Ino(3,4);

/**
 * @brief Changes the motor direction and speed
 * @param motorNum uint8_t motor number (0 or 1)
 * @param dir uint8_t direction (HH_LL or LL_HH for forward/backward of one motor on four half bridges)
 * @param speed uint8_t speed (0-255)
 * @param errorCheck bool if true, the error register is cleared after setting the speed
 */
void motorSet(uint8_t motorNum, uint8_t dir, uint8_t speed, bool errorCheck = false)
{
    if (dir != oldDirection[motorNum])
    {
        controller.directWriteReg(motorReg[motorNum][0], dir);
        oldDirection[motorNum] = dir;
    }
    controller.directWriteReg(motorReg[motorNum][1], speed);
    if (errorCheck)
    {
        controller.clearErrors();
    }
}

/**
 * @brief 
 * @param motor 
 * @param speed 
 */
void motor_pwm(int motor, int speed)
{
    if (speed > 0){
        motorSet(motor, LL_HH, abs(speed));
    } else {
        motorSet(motor, HH_LL, abs(speed));
    }
}

/**
 * @brief Turns the Multi-Halfbridge on or off and reinitializes it if turned on
 * @param state bool true to turn on, false to turn off
 */
void setMultiHalfbridge(bool state) {
    if (state) {
        digitalWrite(4, HIGH); // Turn on the Multi-Halfbridge
        Serial.println("Multi-Halfbridge turned ON. Reinitializing...");
        
        // Reinitialize the motor controller
        controller.begin();

        // Reconfigure half bridges and PWM channels for motors
        controller.configHB(controller.TLE_HB1, controller.TLE_HIGH, controller.TLE_PWM1);
        controller.configHB(controller.TLE_HB2, controller.TLE_HIGH, controller.TLE_PWM1);
        controller.configHB(controller.TLE_HB3, controller.TLE_LOW,  controller.TLE_NOPWM);
        controller.configHB(controller.TLE_HB4, controller.TLE_LOW,  controller.TLE_NOPWM);

        controller.configHB(controller.TLE_HB9,  controller.TLE_HIGH, controller.TLE_PWM3);
        controller.configHB(controller.TLE_HB10, controller.TLE_HIGH, controller.TLE_PWM3);
        controller.configHB(controller.TLE_HB11, controller.TLE_LOW,  controller.TLE_NOPWM);
        controller.configHB(controller.TLE_HB12, controller.TLE_LOW,  controller.TLE_NOPWM);

        // Ensure motors are stopped
        controller.configPWM(controller.TLE_PWM1, controller.TLE_FREQ200HZ, 0);
        controller.configPWM(controller.TLE_PWM3, controller.TLE_FREQ200HZ, 0);
        controller.clearErrors();

        Serial.println("Multi-Halfbridge reinitialized.");
    } else {
        digitalWrite(4, LOW); // Turn off the Multi-Halfbridge
        Serial.println("Multi-Halfbridge turned OFF.");
    }
}

void setup() {
    pinMode(4, OUTPUT); // Configure pin 4 as output
    digitalWrite(4, HIGH); // Default: Multi-Halfbridge is ON
    delay(100);

    // LED setup
    pinMode(LED2, OUTPUT);

    // Initialize the motor controller
    controller.begin();

    // Configure half bridges and PWM channels for motors
    controller.configHB(controller.TLE_HB1, controller.TLE_HIGH, controller.TLE_PWM1);
    controller.configHB(controller.TLE_HB2, controller.TLE_HIGH, controller.TLE_PWM1);
    controller.configHB(controller.TLE_HB3, controller.TLE_LOW,  controller.TLE_NOPWM);
    controller.configHB(controller.TLE_HB4, controller.TLE_LOW,  controller.TLE_NOPWM);

    controller.configHB(controller.TLE_HB9,  controller.TLE_HIGH, controller.TLE_PWM3);
    controller.configHB(controller.TLE_HB10, controller.TLE_HIGH, controller.TLE_PWM3);
    controller.configHB(controller.TLE_HB11, controller.TLE_LOW,  controller.TLE_NOPWM);
    controller.configHB(controller.TLE_HB12, controller.TLE_LOW,  controller.TLE_NOPWM);

    // All stop
    controller.configPWM(controller.TLE_PWM1, controller.TLE_FREQ200HZ, 0);
    controller.configPWM(controller.TLE_PWM3, controller.TLE_FREQ200HZ, 0);
    controller.clearErrors();

    // Initialize serial communication
    Serial.begin(9600);
    while (!Serial) ;  // Wait for serial connection
    Serial.println("Started");

    // Initialize the IMU sensor
    if (!IMU.begin()) {
        Serial.println("Failed to initialize IMU!");
        while (1) ;  // Halt execution if IMU initialization fails
    }

    Serial.print("Accelerometer sample rate = ");
    Serial.print(IMU.accelerationSampleRate());
    Serial.println(" Hz");

    Serial.println();
    Serial.println("Acceleration in G's");
    Serial.println("X\tY\tZ");
}

/**
 * @brief Smoothly ramps the motor input
 */
int16_t motor_ramp(int16_t motor_in)
{
  if (abs(motor_in) < 1) {
    return 0;
  }
  if (motor_in > 0) {
    return motor_in + 20;
  } else {
    return motor_in - 20;
  }
}

uint32_t loop_counter = 1;

/**
 * @brief Checks for PID updates via serial and adjusts parameters
 */
void checkSerialInput() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n'); // Read input until newline
        input.trim(); // Remove any leading/trailing whitespace

        if (input.startsWith("kp ") || input.startsWith("ki ") || input.startsWith("kd ") || input.startsWith("alpha ")) {
            resetPID();  // Reset the PID state when parameters are updated
        }

        if (input.startsWith("kp ")) {
            float value = input.substring(3).toFloat();
            if (value >= 0.1 && value <= 2.0) { // Validate range
                Kp = value;
                Serial.print("Updated Kp to: "); Serial.println(Kp);
            } else {
                Serial.println("Error: Kp out of range (0.1 - 2.0)");
            }
        } else if (input.startsWith("ki ")) {
            float value = input.substring(3).toFloat();
            if (value >= 0.0 && value <= 1.0) { // Validate range
                Ki = value;
                Serial.print("Updated Ki to: "); Serial.println(Ki);
            } else {
                Serial.println("Error: Ki out of range (0.0 - 1.0)");
            }
        } else if (input.startsWith("kd ")) {
            float value = input.substring(3).toFloat();
            if (value >= 0.0 && value <= 1.0) { // Validate range
                Kd = value;
                Serial.print("Updated Kd to: "); Serial.println(Kd);
            } else {
                Serial.println("Error: Kd out of range (0.0 - 1.0)");
            }
        } else if (input.startsWith("alpha ")) {
            float value = input.substring(6).toFloat();
            if (value >= 0.5 && value <= 1.0) { // Validate range
                alpha = value;
                Serial.print("Updated alpha to: "); Serial.println(alpha);
            } else {
                Serial.println("Error: Alpha out of range (0.5 - 1.0)");
            }
        } else if (input.startsWith("d ")) {
            int debugValue = input.substring(2).toInt();
            debugEnabled = (debugValue == 1);
            Serial.print("Debug mode: "); Serial.println(debugEnabled ? "Enabled" : "Disabled");
        } else if (input == "PING") {
            Serial.println("PONG"); // Respond to initialization check
        } else if (input == "bridge on") {
            setMultiHalfbridge(true); // Turn on the Multi-Halfbridge
        } else if (input == "bridge off") {
            setMultiHalfbridge(false); // Turn off the Multi-Halfbridge
        } else {
            Serial.println("Error: Invalid command. Use 'kp <value>', 'ki <value>', 'kd <value>', 'alpha <value>', 'd <0/1>', 'bridge on', or 'bridge off'.");
        }
    }
}

/**
 * @brief Resets the PID controller state
 */
void resetPID() {
    errorSum = 0.0;
    lastError = 0.0;
    Serial.println("PID state reset.");
}

void loop() {
  checkSerialInput(); // Check for user input to update PID parameters

  // Check if acceleration data is available
  if (IMU.accelerationAvailable()) {
    float ax, ay, az;
    IMU.readAcceleration(ax, ay, az);  // Read acceleration values
    float gx, gy, gz;
    IMU.readGyroscope(gx, gy, gz);

    unsigned long now = millis();
    dt = (now - lastTime) / 1000.0;
    lastTime = now;

    // Calculate the current angle (e.g., pitch) based on acceleration values
    angle = ax / az;
    angle = atan(angle) * 180 / PI;  // Convert to degrees
    gyroRate = gy;

    angle = alpha * (angle + gyroRate * dt) + (1 - alpha) * accAngle;

    // PID control
    float error = setpoint - angle;
    errorSum += error * dt;
    float dError = (error - lastError) / dt;

    float out = Kp * error + Ki * errorSum + Kd * dError;
    lastError = error;

    // Constrain PID output to the range [-255, 255]
    int16_t output = constrain(round(motor_ramp(out)), -100, 100);

    // Adjust motor speed based on PID output
    output = output * -1;

    motor_pwm(0, output);  // Control motor 1
    motor_pwm(1, output);  // Control motor 2

    // Print debug information if debug mode is enabled
    if (debugEnabled) {
      Serial.print("Angle: ");
      Serial.println(angle);
      /*Serial.print(" | Output: ");
      Serial.println(output);*/
    }

    loop_counter++;

    // Optional delay to slow down data rate
    delay(1);
  }
}
