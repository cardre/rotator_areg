// rotator_areg project
// An initially very simple antenna rotator project buitl by AREG members
// Please see www.areg.org.au
//
// Based on Arduino Uno, Adafruit 9DOF sensor and DFRobot (LM298) motor controller board
//
// Compiled using the Platform IO environment

#include <Arduino.h>
#include "config.h"
#include "rotator.h"
#include "serial.h"

void setup()
{
  // enable serial port
  Serial.begin(serial_port_speed);

  // setup motors and ahrs
  rotator_setup();

  // clear serial buffers
  serial_data_clear();
}


void loop(void)
{
  // Let the rotator code update motors/orientation as needed
  rotator_update();

  // Check if have serial data to process
  if (Serial.available() > 0)
  {
    serial_data_handler();
  }
}
