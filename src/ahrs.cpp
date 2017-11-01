// Functions related to Altitude & Heading Reference System (AHRS) from 9DOF
// rototor_areg
// VK5CD

#include "ahrs.h"
#include "config.h"

// Observations about Adafruit Simple AHRS library and returned values
//
// the get_orientation function (see below) adjusts these returned
// values to something more sensible
//
// Pitch
//  - min -90 degrees - pointing straigh up
//  - max +90 degrees - pointing straight down
//  - 0 degress for pointing at horizon
//  - values wrap and start going in opposite direction if max/min exceeded
// Heading
//  - 0 degress is magnetic north
//    - add magnetic declination for your areat to get true north
//  - +90 degrees is west
//  - -90 degrees is east
//  - +/- 180 degrees is south (wraps around)


// Create sensor instances.
Adafruit_LSM303_Accel_Unified accel(30301);
Adafruit_LSM303_Mag_Unified   mag(30302);

// Create simple AHRS algorithm using the above sensors.
Adafruit_Simple_AHRS          ahrs(&accel, &mag);


// Inital setup of the 9DOF board
void ahrs_setup()
{
  // Initialize the sensors.
  accel.begin();
  mag.begin();
}

// Return sensible values for orientation from simple AHRS library
// This takes into consideration the way the board is mounted on the rotator
// and will ultimately require a configuration setting to change in future
//
// Occasionally get random errors from heading (magnetometer) so following
// is to ignore values if too far different from last values
int heading_errors_count = 0 ;
//
bool get_orientation(sensors_vec_t * orientation, bool initial_setting)
{
  int prev_heading = orientation->heading ;

  bool ret_val = ahrs.getOrientation(orientation);

  // Adjust Simple AHRS values to make sense
  int adj_heading = - orientation->heading + 180 ;
  if ( adj_heading > 180 ) adj_heading -= 360 ;

  // Check (using wrap around, hence %360) that haven't exceeded maximum degrees allowed
  if ( abs( ((adj_heading+360)%360) - ((prev_heading+360)%360) ) > max_heading_degrees_change_allowed )
  {
    // Too great a difference, so ignore it
    heading_errors_count ++ ;

    #ifdef DEBUG_SERIAL
      Serial.print(F("HEADING ERROR: "));
      Serial.println(heading_errors_count);
    #endif

    if ( heading_errors_count > max_heading_errors_allowed )
    {
      // Too many errors have occurred, so we can't ignore it any more
      heading_errors_count = 0 ;
    }
  }
  else
  {
    // Within allowed change, accept it
    heading_errors_count = 0 ;
  }
  if ( heading_errors_count == 0 )
  {
    orientation->heading = adj_heading ; // 0 degrees north, then positive clockwise
  }

  // orientation->pitch = - orientation->pitch ; // 0 degrees level/horizon, then positive increases pitch

  return ret_val ;
}
