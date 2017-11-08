// Functions/values/constants related to config/setup of rotator
// rototor_areg
// VK5CD

// Define if you want debug serial messages printed
#undef DEBUG_SERIAL

// Initial values for configuration parameters
// Should eventually end up in flash/eeprom so can be configured later
const int az_tolerance_degrees = 14 ;
const int el_tolerance_degrees = 6 ;
const int az_ramp_time_msecs = 1000 ;
const int el_ramp_time_msecs = 1000 ;
const int az_min_degrees = -270 ;
const int az_max_degrees = 270 ;
const int el_min_degrees = -20 ; // 20 degrees down below horizon
const int el_max_degrees = 85 ; // 90 is pointing straight up
const int az_motor_max_pwm = 255 ; // 255 is maximum PWM can use with analogWrite
const int el_motor_max_pwm = 255 ;
// ---- future config values could add.. ----
// const int mag_decl_degrees = 10 ; // added to magnetic heading to get true north
const long serial_port_speed = 115200 ;

// As we adjust the PWM frequency, this multiplier is used to correct millis/delay times
const int millis_correction = 8 ;

// Magnetometer sometimes returns strange values, this sets limit to how many we'll accept
const int max_heading_degrees_change_allowed = 20 ; // If we exceed previous value by this much, ignore/error
const int max_heading_errors_allowed = 30 ; // Start accepting values after this many times

// How long to lockout movement for after E stop if still receiving targets
const long movement_disabled_lockout_millis = 10000 ;
