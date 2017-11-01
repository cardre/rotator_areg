// Functions related to Altitude & Heading Reference System (AHRS) from 9DOF
// rototor_areg
// VK5CD

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_Simple_AHRS.h>

// Our Functions
void ahrs_setup();
bool get_orientation(sensors_vec_t * orientation, bool initial_setting = false);
