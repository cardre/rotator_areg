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
  Serial.begin(serial_port_speed);
  Serial.println(F("Rotator - www.areg.org.au"));
  Serial.println();
  Serial.println(F("Simple CLI serial commands:"));
  Serial.println(F("  t|T<azimuth>,<elevation> = set target, e.g. 't90,30' is East with 30 degrees elevation"));
  Serial.println(F("  g|G - get current orientation, returns azimuth elevation, e.g. 'current_orientation: 145 0'"));
  Serial.println(F("  s|S - stop motors (nicely) by ramping down"));
  Serial.println(F("  e|E - EMERGENCY stop motors immediately"));
  Serial.println();

  rotator_setup();
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
