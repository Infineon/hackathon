<img src="./img/infineon_logo.png" alt="Infineon Logo" height="50"/>

# Improving the PID controller for a Self-Balancing Robot

## Hackathon Material
* [Topic Introduction Slides](./topic_introduction.pdf)
* [Challenge Introduction Slides](./challenge_introduction.pdf)

## Running the Example

### Overview
The example is meant to help you understand how the robot can be programmed and how you can communicate with the gyroscope and motor controller.
It's not mandatory to run the example or to follow the same approach. Feel free to refer to the documention pages linked below and follow a different approach.

### Preparation
Please start by installing PSOC™ 6 for Arduino and the required libraries.

#### Installation of PSOC™ 6 for Arduino
Follow the instructions from the [slides](./topic_introduction.pdf) and make sure to use the alternative installation link provided there.

#### Installation of the Multi-Half-Bridge library for Arduino
The [Infineon TLE94112ES](https://www.infineon.com/cms/de/product/power/motor-control-ics/brushed-dc-motor-control-ics/multi-half-bridge-ics/tle94112el/) multi-half-bridge motor driver board is used to drive the robot's motors. You can find it in the Arduino library manager by looking for "*multi-half-bridge*".

#### Installation of BMI270 library for Arduino
The [Bosch BMI270](https://www.bosch-sensortec.com/products/motion-sensors/imus/bmi270/) Inertial Measurement Unit is an acceleration sensor and gyroscope, providing the input for the balancing algorithm.
For this challenge we use a modified version of the Arduino library for this sensor. Please download it from [here](https://github.com/Infineon/hackathon/releases/download/v1.0.0/Arduino_BMI270_BMM150-wire-patch.zip) and add it to your Arduino IDE by navigating to *Sketch > Include Library > Add ZIP library...*.

#### Cloning this Repository
Afterwards you can clone this git repo:
```
git clone https://github.com/Infineon/hackathon
```

#### Compile & Run the Example
Now that you have the example on your computer you can open it in the Arduino IDE (it's located in `examples/balancingRobot/`)
Afterwards, make sure you have connected the robot to your computer and selected the right board (*CY8CKIT-062S2-AI*) and serial port.
Now, you can compile & upload the example and the robot should start moving.

Of course this is only an example to undestand how everything works - doing it better and making the robot balance properly is your task :)

## Useful Links
* [PSOC™ 6 for Arduino on GitHub](https://github.com/Infineon/arduino-core-psoc6)
* [PSOC™ 6 for Arduino docs](https://github.com/Infineon/arduino-core-psoc6)
* [Multi-Half-Bridge Arduino library on GitHub](https://github.com/Infineon/arduino-multi-half-bridge)
* [Multi-Half-Bridge Arduino library docs](https://github.com/Infineon/multi-half-bridge/wiki/Ino-Getting-Started)
* [BMI270 Arduino Library](https://github.com/Infineon/hackathon/releases/download/v1.0.0/Arduino_BMI270_BMM150-wire-patch.zip)

## Infineon Team

**Eric** (Embedded Systems Engineer)

<img src="./img/eric.png" alt="Eric" height="150"/>

**Olaf** (Principal Engineer)

<img src="./img/olaf.png" alt="Julian" height="150"/>

### How to reach us?
Please [open an issue](https://github.com/Infineon/hackathon/issues) in this repository or just talk to us at the venue.