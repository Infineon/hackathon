#include <SimpleFOC.h>
#include "TLE5012Sensor.h"

// Define SPI pins for TLE5012 sensor
#define PIN_SPI1_SS0   94
#define PIN_SPI1_MOSI  69
#define PIN_SPI1_MISO  95
#define PIN_SPI1_SCK   68

// Create an instance of SPIClass3W for 3-wire SPI communication
tle5012::SPIClass3W tle5012::SPI3W1(2);

// Create an instance of TLE5012Sensor
TLE5012Sensor tle5012Sensor(&SPI3W1, PIN_SPI1_SS0, PIN_SPI1_MISO, PIN_SPI1_MOSI, PIN_SPI1_SCK);

// BLDC motor instance: BLDCMotor(pole_pairs, phase_resistance)
BLDCMotor motor = BLDCMotor(7, 0.24); // 7 pole pairs, 0.24 Ω phase resistance

// Define driver pins
const int U = 11;
const int V = 10;
const int W = 9;
const int EN_U = 6;
const int EN_V = 5;
const int EN_W = 3;

// BLDCDriver3PWM driver = BLDCDriver3PWM(pwmA, pwmB, pwmC, EnableA, EnableB, EnableC)
BLDCDriver3PWM driver = BLDCDriver3PWM(U, V, W, EN_U, EN_V, EN_W);

// Velocity set point variable
float target_velocity = 0; // rad/s

// Commander instance
Commander command = Commander(Serial);

// Velocity PID tuning callback
void onPid(char* cmd) {
  command.pid(&motor.PID_velocity, cmd);
}

// Target velocity callback
void doTarget(char* cmd) {
  command.scalar(&target_velocity, cmd);
}

void setup() {
  Serial.begin(115200);
  SimpleFOCDebug::enable(&Serial);

  // Initialize sensor
  tle5012Sensor.init();
  motor.linkSensor(&tle5012Sensor);

  // Driver config
  driver.voltage_power_supply = 12;
  driver.voltage_limit = 6; // Start low for safety
  if (!driver.init()) {
    Serial.println("Driver init failed!");
    return;
  }
  motor.linkDriver(&driver);

  // Motor config
  motor.controller = MotionControlType::velocity;
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // PID tuning - start with reasonable defaults for 5010 360KV
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  motor.PID_velocity.D = 0.001;
  motor.PID_velocity.output_ramp = 1000; // rad/s^2

  // Filtering
  motor.LPF_velocity.Tf = 0.01; // 10 ms filter

  // Limits
  motor.velocity_limit = 400; // rad/s (~3820 RPM electrical, ~545 RPM mechanical)
  motor.voltage_limit = 6;    // V

  // Monitoring
  motor.useMonitoring(Serial);

  // Initialize motor and FOC
  motor.init();
  motor.initFOC();

  // Commander commands
  command.add('T', doTarget, "target velocity");
  command.add('C', onPid, "velocity PID");

  Serial.println(F("Motor ready."));
  Serial.println(F("Commands:"));
  Serial.println(F("  T<float>  - Set target velocity (rad/s)"));
  Serial.println(F("  CP<float> - Set P gain"));
  Serial.println(F("  CI<float> - Set I gain"));
  Serial.println(F("  CD<float> - Set D gain"));
}

void loop() {
  tle5012Sensor.update();
  motor.loopFOC();
  motor.move(target_velocity);
  command.run();
}