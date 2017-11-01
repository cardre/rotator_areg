// Functions related to motor Control
// rototor_areg
// VK5CD

#include <Arduino.h>

#include "config.h"
#include "motors.h"

bool last_dir_pitch_up;
bool last_dir_clockwise;

// Setup motors and initial direction
void motors_setup()
{
  last_dir_pitch_up = true;
  last_dir_clockwise = true;

  pinMode(M1, OUTPUT);
  set_az_motor_dir_pitch_up(last_dir_pitch_up);
  set_az_motor_pwm_speed(0); // off
  pinMode(M2, OUTPUT);
  set_el_motor_dir_clockwise(last_dir_clockwise);
  set_el_motor_pwm_speed(0); // off

  // Change PWM frequency to reduce motor whine when accel/decel
  // NOTE: this changes 'delay()' etc. to run 8 times faster than normal
  //       See the 'millis_correction' const to correct and delay/millis values
  TCCR0B = TCCR0B & ( 0b11111000 | 02 ) ; // Normal divisor is 03
}

// Set speed of Elevation motor
// 0  stop
// 1..255 = pitch up speed
// -1..-255 = pitch down speed
void set_el_motor_pwm_speed(int pwm_speed)
{
  bool dir_pitch_up = pwm_speed > 0 ;
  if ( dir_pitch_up != last_dir_pitch_up )
  {
    set_az_motor_dir_pitch_up(dir_pitch_up) ;
    last_dir_pitch_up = dir_pitch_up ;
  }
  analogWrite( E1, abs(pwm_speed) ) ;
}

// Set speed of Azimuth motor
// 0  stop
// 1..255 = pitch up speed
// -1..-255 = pitch down speed
void set_az_motor_pwm_speed(int pwm_speed)
{
  bool dir_clockwise = pwm_speed > 0 ;
  if ( dir_clockwise != last_dir_clockwise )
  {
    set_el_motor_dir_clockwise(dir_clockwise) ;
    last_dir_clockwise = dir_clockwise ;
  }
  analogWrite( E2, abs(pwm_speed) ) ;
}

// Set motor direction pin settings
void set_az_motor_dir_pitch_up(bool dir_pitch_up)
{
  digitalWrite(M1, ! dir_pitch_up); // This is dependent on wiring and should be configurable
}
//
void set_el_motor_dir_clockwise(bool dir_clockwise)
{
  digitalWrite(M2, dir_clockwise); // This is dependent on wiring and should be configurable
}

#ifdef NOT_NEEDED
// Simple test routine that just moves the motors back and forward
//
// Following are * millis_correction due to change in PWM frequency affecting delay routing timer
const int stepDelay = 5 * millis_correction ;
const int waitDelay = 2000 * millis_correction ;
//
void test_motors_movement()
{
  int value;

  delay (waitDelay);

  for(value = 0 ; value <= 255; value+=1)
  {
    set_az_motor_pwm_speed(value);
    // set_el_motor_pwm_speed(value);
    delay(stepDelay);
  }

  delay (waitDelay);

  for(value = 255 ; value >= 0; value-=1)
  {
    set_az_motor_pwm_speed(value);
    // set_el_motor_pwm_speed(value);
    delay(stepDelay);
  }

  delay (waitDelay);

  for(value = 0 ; value >= -255; value-=1)
  {
    set_az_motor_pwm_speed(value);
    // set_el_motor_pwm_speed(value);
    delay(stepDelay);
  }

  delay (waitDelay);

  for(value = -255 ; value <= 0; value+=1)
  {
    set_az_motor_pwm_speed(value);
    // set_el_motor_pwm_speed(value);
    delay(stepDelay);
  }

  // -------------------------------------

  for(value = 0 ; value <= 255; value+=1)
  {
    set_el_motor_pwm_speed(value);
    delay(stepDelay);
  }

  delay (waitDelay);

  for(value = 255 ; value >= 0; value-=1)
  {
    set_el_motor_pwm_speed(value);
    delay(stepDelay);
  }

  delay (waitDelay);

  for(value = 0 ; value >= -255; value-=1)
  {
    set_el_motor_pwm_speed(value);
    delay(stepDelay);
  }

  delay (waitDelay);

  for(value = -255 ; value <= 0; value+=1)
  {
    set_el_motor_pwm_speed(value);
    delay(stepDelay);
  }
}
#endif // NOT_NEEDED
