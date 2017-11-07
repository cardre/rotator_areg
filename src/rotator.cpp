// Functions related to orientation and running motors for the rotator
// rototor_areg
// VK5CD

#include "config.h"
#include "rotator.h"
#include "ahrs.h"
#include "motors.h"

// Our current and target orientations and values
sensors_vec_t  cur_orientation, target_orientation;
float az_motor_pwm_speed, el_motor_pwm_speed; // last pwm speed set for motors

// How much to change each motor speed when ramping p/ms
const float az_ramp_per_msec = float(az_motor_max_pwm) / float(az_ramp_time_msecs) ;
const float el_ramp_per_msec = float(el_motor_max_pwm) / float(el_ramp_time_msecs) ;

// To disable any new movement from motors
bool movement_disabled ;

// Internal routine to set desired orientation
void set_target(int azimuth, int elevation)
{
  // Limit azimuth to +/- 180 degrees where 0 = north, 90 = east etc
  while (azimuth > 180 ) azimuth -= 360 ;
  while (azimuth < -180 ) azimuth += 360 ;

  // Limit elevation to our min/max values
  if (elevation > el_max_degrees) elevation = el_max_degrees ;
  if (elevation < el_min_degrees) elevation = el_min_degrees ;

  // Now set our desired orientation
  target_orientation.heading = azimuth;
  target_orientation.pitch = elevation;

  // We've now had a target set, so allow motors to move
  movement_disabled = false ;
}

// Initial config and setup
void rotator_setup()
{
  ahrs_setup();
  motors_setup();

  // Default to our current orientation and stopped
  get_orientation(&cur_orientation, true); // true = force initial value, ignoring errors
  target_orientation = cur_orientation;
  az_motor_pwm_speed = 0 ;
  el_motor_pwm_speed = 0 ;
  movement_disabled = false ; // Don't start moving until we've been given a target
}

// Main loop of rotator to run the motors, check orientation, target etc
//
// MUST NOT BLOCK AS WILL INTERFERE WITH SERIAL COMMANDS!
//
long prev_msecs = millis() / millis_correction ;
//
void rotator_update()
{
  float az_motor_pwm_speed_wanted = 0 ; // 0 = stopped, >0 clockwise, <0 anti-clockwise, max = abs(255)
  float el_motor_pwm_speed_wanted = 0 ; // 0 = stopped, >0 clockwise, <0 anti-clockwise, max = abs(255)
  float az_pwm_change ; // How much to change for this iteration
  float el_pwm_change ; // How much to change for this iteration
  long cur_msecs = millis() / millis_correction ;

  // Get our current orientation to work out what to do
  get_orientation(&cur_orientation);
  #ifdef DEBUG_SERIAL
    // Serial.println(cur_orientation.heading);
  #endif

  // ----------------------------------
  // Elevation calculations
  if ( ! movement_disabled )
  {
    if (cur_orientation.pitch - (el_tolerance_degrees/2) > target_orientation.pitch)
    {
      el_motor_pwm_speed_wanted = - el_motor_max_pwm ; // pitch down
    }
    else if (cur_orientation.pitch + (el_tolerance_degrees/2) < target_orientation.pitch)
    {
      el_motor_pwm_speed_wanted = el_motor_max_pwm ; // pitch up
    }
    // else otherwise we want to stop (default is already 0)
  }

  // Adjust elevation motors if required
  if ( el_motor_pwm_speed_wanted != el_motor_pwm_speed )
  {
    // Calculate how much to change pwm speed by based on ramp times
    if ( el_motor_pwm_speed_wanted > el_motor_pwm_speed )
      el_pwm_change = ( cur_msecs - prev_msecs ) * az_ramp_per_msec ;
    else
      el_pwm_change = - ( cur_msecs - prev_msecs ) * az_ramp_per_msec ;
    el_motor_pwm_speed += el_pwm_change ;

    // If close enough to desired speed, then set it
    if ( abs(el_motor_pwm_speed_wanted - el_motor_pwm_speed) < 5)
    {
      el_motor_pwm_speed = el_motor_pwm_speed_wanted ;
      #ifdef DEBUG_SERIAL
        Serial.println(F("EL RAMP: TARGET REACHED"));
      #endif
    }
    #ifdef DEBUG_SERIAL
      Serial.print(F("EL RAMP: "));
      Serial.print(el_motor_pwm_speed_wanted);
      Serial.print(F(" "));
      Serial.print(el_motor_pwm_speed);
      Serial.print(F(" "));
      Serial.print(el_pwm_change);
      Serial.println();
    #endif

    set_el_motor_pwm_speed(el_motor_pwm_speed);
  }

  // ----------------------------------
  // Azimuth calculations
  if ( ! movement_disabled )
  {
    if (cur_orientation.heading - (az_tolerance_degrees/2) > target_orientation.heading)
    {
      az_motor_pwm_speed_wanted = - az_motor_max_pwm ; // anti-clockwise
    }
    else if (cur_orientation.heading + (az_tolerance_degrees/2) < target_orientation.heading)
    {
      az_motor_pwm_speed_wanted = az_motor_max_pwm ; // clockwise
    }
    // else otherwise we want to stop (default is already 0)
  }

  // Adjust azimuth motors if required
  if ( az_motor_pwm_speed_wanted != az_motor_pwm_speed )
  {
    // Calculate how much to change pwm speed by based on ramp times
    if ( az_motor_pwm_speed_wanted > az_motor_pwm_speed )
      az_pwm_change = ( cur_msecs - prev_msecs ) * az_ramp_per_msec ;
    else
      az_pwm_change = - ( cur_msecs - prev_msecs ) * az_ramp_per_msec ;
    az_motor_pwm_speed += az_pwm_change ;

    // If close enough to desired speed, then set it
    if ( abs(az_motor_pwm_speed_wanted - az_motor_pwm_speed) < 5)
    {
      az_motor_pwm_speed = az_motor_pwm_speed_wanted ;
      #ifdef DEBUG_SERIAL
        Serial.println(F("AZ RAMP: TARGET REACHED"));
      #endif
    }
    #ifdef DEBUG_SERIAL
      Serial.print(F("AZ RAMP: "));
      Serial.print(cur_orientation.heading);
      Serial.print(F(" "));
      Serial.print(target_orientation.heading);
      Serial.print(F(" "));
      Serial.print(az_motor_pwm_speed_wanted);
      Serial.print(F(" "));
      Serial.print(az_motor_pwm_speed);
      Serial.print(F(" "));
      Serial.print(az_pwm_change);
      Serial.println();
    #endif

    set_az_motor_pwm_speed(az_motor_pwm_speed);
  }

  // Now update our prev_msecs for next iteration
  prev_msecs = cur_msecs ;
}

// Used to set what we want the rotator to point to
void rotator_target_orientation(int azimuth, int elevation)
{
  set_target(azimuth, elevation);
}

// Used to set what we want the rotator to point to
void rotator_target_orientation(rotator_values target)
{
  set_target(target.azimuth, target.elevation);
}

// Return our current orientation
void rotator_current_orientation(rotator_values * return_values)
{
  // Current orientation is updated in main rotator_update function
  // so just return our current values
  return_values->azimuth = cur_orientation.heading;
  return_values->elevation = cur_orientation.pitch;
}

// Tell rotator to stop moving and ramp down motors as usual
void rotator_stop_motors()
{
  // Just set the target to our current orientation
  get_orientation(&cur_orientation);
  target_orientation = cur_orientation;
  // movement_disabled = true ; // Still allow targetting of current orientation, so don't disable
}

// Tell rototar to immediately stop motors and disable further movement
void rotator_emergency_stop_motors()
{
  set_el_motor_pwm_speed(0);
  set_az_motor_pwm_speed(0);
  el_motor_pwm_speed = 0 ;
  az_motor_pwm_speed = 0 ;
  movement_disabled = true ;
  get_orientation(&cur_orientation);
  target_orientation = cur_orientation;
}
