<img src="./img/infineon_logo.png" alt="Infineon Logo" height="50"/>

# Smart Grippers with Infineon Magnetic Sensing and Motor Control

## Hackathon Material
* [Topic Introduction Slides](./topic_introduction.pdf)
* [Challenge Introduction Slides](./challenge_introduction.pdf)

## Running the Example

### Overview
The example is meant to help you to understand  how the gripper can be programmed and how you can communicate, with the motor controller (IFX007T Shield 2Go), the angle sensor (TLE5012B E1000) and the 3D Magnetic Sensor.
It's not mandatory to run the example or to follow the same approach, but it is recommended that you can understand the whole system and also FOC.
Feel free to refer to the documention pages linked below and follow a different approach.


### Preparation
Please start by installing XMC for Arduino and all the requiered libraries.


#### Installation of XMC for Arduino
Follow the instructions from this [website](https://xmc-arduino.readthedocs.io/en/latest/installation-instructions.html) and make sure to use the **alternative installation link** provided here:
https://github.com/LinjingZhang/XMC-for-Arduino/releases/download/V3.5.3-beta/package_infineon_index.json


#### Installation of the XENSIV™ Angle Sensor TLx5012B library for Arduino
The [Infineon TLE5012B E1000](https://www.infineon.com/cms/de/product/sensor/magnetic-sensors/magnetic-position-sensors/angle-sensors/tle5012b-e1000/) angle sensor is used to get the postion from the BLDC motor. A diametrically magnet is attached to the motor, so the we get the postion from the motor.
You can find the Arduino library manager by looking for "*XENSIV™ Angle Sensor TLx5012B*".

#### Installation of 3D Magnetic Sensor library for Arduino
The [Infineon 3D Magnetic Sensor](https://www.infineon.com/cms/en/product/sensor/magnetic-sensors/magnetic-position-sensors/3d-magnetics/tle493d-a2b6/) messures the magnetic field in 3 directions (X, Y and Z).
You can find it in the Arduino library manager by looking for "*XENSIV™ 3D Magnetic Sensor TLx493D*".


#### Installation of Simple FOC library for Arduino
The [Simple FOC library] (https://docs.simplefoc.com)(Simple Field Oriented Control)  is an open-source Arduino-compatible library designed to make it easy to control BLDC and PMSM motors using Field Oriented Control (FOC).

You can find it in the Arduino library manager by looking for "*XENSIV™ 3D Magnetic Sensor TLx493D*".

#### Cloning this Repository
Afterwards you can clone this git repo:
```
git clone https://github.com/Infineon/hackathon
```
#### Compile & Run the Example
Now that you have the example on your computer you can open it in the Arduino IDE (it's located in `examples/adaptiveGripper/torque_control/`).
Afterwards, install the following files in you SimpleFOC library folder:
"TLE5012Sensor.h, TLE5012Sensor.cpp".
Then you can connected the gripper to your computer and selected the right board (*XMC4700 Relax Kit*) and serial port.
Now, you can compile & upload the example.



Of course this is only an example to undestand how everything works - So now it is your task to grasp every object :)

And if you have questions or a problem just come to us, we may help you or just follow this quote:
"If there is no path before you, create your own." (Quote: Star Wars the Clone Wars: Season 7, Episode 5 – Gone with a Trace)

## Useful Links
* [XMC for Arduino](https://github.com/Infineon/XMC-for-Arduino?tab=readme-ov-file)
* [XMC for Arduino docs](https://xmc-arduino.readthedocs.io/en/latest/index.html)
* [3D Magnetic Arduino library on GitHub](https://github.com/Infineon/arduino-xensiv-3d-magnetic-sensor-tlx493d)
* [3D Magnetic Arduino library docs](https://www.infineon.com/cms/en/product/sensor/magnetic-sensors/magnetic-position-sensors/3d-magnetics/)
* [XENSIV™ TLx5012B Angle Sensor Arduino Library](https://github.com/Infineon/xensiv-angle-sensor-tlx5012)
* [TLE5012B E1000](https://www.infineon.com/cms/de/product/sensor/magnetic-sensors/magnetic-position-sensors/angle-sensors/tle5012b-e1000/)
* [Simple FOC Arduino library on Github](https://github.com/simplefoc/Arduino-FOC)
* [Simple FOC Arduino library docs](https://docs.simplefoc.com)

## Infineon Team

**Eric** (Embedded Systems Engineer)

<img src="./img/eric.png" alt="Eric" height="150"/>

**Linjing** (Embedded Software Engineer)

<img src="./img/linjing.jpg" alt="Linjing" height="150"/>

### How to reach us?
Please [open an issue](https://github.com/Infineon/hackathon/issues) in this repository or just talk to us at the venue.
