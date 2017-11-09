rotator_areg
============

## Introduction

Antenna rotator for www.AREG.org.au

An (initially) simple antenna rotator based on an Arduino Uno, Adafruit 9DOF board and a DFRobot Motor Controller.

Developed with Platform IO.

VK5CD

## Parts

The code is currently assuming the following components:

- [Arduino Uno](https://store.arduino.cc/usa/arduino-uno-rev3) or compatible
- [Adafruit 9DOF breakout board](https://www.adafruit.com/product/1714) or compatible
- [DFRobot Arduino Motor Shield (L298N)](https://www.dfrobot.com/wiki/index.php/Arduino_Motor_Shield_%28L298N%29_%28SKU:DRI0009%29) or compatible with jumpers set in PWM mode and PWRIN, but the later is dependent on your motors and their power requirements

## Wiring

- Adafruit 9DOF board is wired to A4, A5, 5V & Ground
- DFRobot Motor Shield uses D4, D5, D6 & D7 + 5V & Ground
