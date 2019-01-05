// Functions related to handling incoming serial data
// rototor_areg
// VK5CD

#include "serial.h"
#include "rotator.h"
#include "config.h"

// Serial data buffer handling
const int serial_buffer_size = 30;
byte serial_buffer[serial_buffer_size];
byte next_serial_index = serial_buffer_size ; // so inital clear zeros buffer
bool serial_help_sent = false;

// CLI constants
const char cli_eol = byte('\n');

// SPID rot2 constants
const char spid_eol = 0x20;                 //space
const char spid_pulse_resolution = 0x01;    // report one pulse per degree resolution

// Simple serial data handler
//
// MUST NOT BLOCK AS WILL INTERFERE WITH MOTOR CONTROL!
//
void serial_data_handler()
{
  // Keep reading all serial data into our buffer
  if ( next_serial_index < serial_buffer_size )
  {
    serial_buffer[next_serial_index++] = Serial.read() ;
    // DEBUG check -> Serial.println(serial_buffer);
  }
  else
  {
    // Buffer overflowed, so clear all data and start again
    serial_data_clear();
    return;
  }

  switch (serial_buffer[0]) // check first byte of buffer
  {
    // All the chars for our simple CLI serial interface
    // (Make sure 1st byte does not conflict with other protocols below)
    //
    case 't': // Set target
    case 'T':
    case 'g': // Get current orientation
    case 'G':
    case 's': // Stop motors (ramp down)
    case 'S':
    case 'e': // Emergency stop motors immediately
    case 'E':
    case 'h': // Move to home orientation
    case 'H':
    case '?': // Display help
    case cli_eol:
      // Do we have a complete line to process?
      if ( strchr( (char *)serial_buffer, cli_eol) )
      {
        // Got a complete cli cmd, so process it
        switch (serial_buffer[0])
        {
          case 't':
          case 'T':
            // Set target orientation, e.g. 't123,45' (az,el)
            serial_cli_cmd_set_target();
            break;
          case 'g':
          case 'G':
            // Get current orientation
            serial_cli_cmd_get_orientation();
            break;
          case 's':
          case 'S':
            // Stop motors (ramp down)
            serial_cli_cmd_stop_motors();
            break;
          case 'e':
          case 'E':
            // Emergency stop motors immediately
            serial_cli_cmd_emergency_stop_motors();
            break;
          case 'h':
          case 'H':
            // Move to home orientation 0,0
            serial_cli_cmd_home_orientation();
            break;
          case '?':
          case cli_eol:
            // print help screen
            serial_cli_print_help();
            break;
        }
        // Cmd has been handled, clear out buffer
        serial_data_clear();
      }
      break;

    // Alphasid Rot2 protocol packet (always begins with 'W')
    //
    case 'W': // 0x57
      // Check have a 13 byte long command packet that terminates in spid_eol
      if ( next_serial_index > 12 )
      {
        // Check have a complete spid cmd, so process it
        if ( serial_buffer[12] == spid_eol )
        {
          serial_spid_rot2_parse_command();
        }
        // Cmd has been handled/ignored, clear out buffer
        serial_data_clear();
      }
      break;

    // No one handled the 1st serial data byte, so throw it away
    //
    default:
      serial_data_clear();
  }
}

// Clear out serial buffer
//
void serial_data_clear()
{
  memset(serial_buffer, 0, next_serial_index);
  next_serial_index = 0 ;
}

// ------------- CLI protocol ----------------

// Process the CLI cmd to set target
// format is [t|T][\-,0..9]*()\,[\-,0..9]*).
// e.g. east & 45 degree elevation = 'T90,45'
// e.g. west and no change to elevation = 't-90'
void serial_cli_cmd_set_target()
{
  int azimuth, elevation = 0 ;

  // See if we have 1 (az) or 2 (az,el) numbers
  char * comma_ptr = strchr((char *)serial_buffer, ',');
  if ( comma_ptr )
  {
    // Should be 2 values comma separated
    * comma_ptr = 0 ; // terminate az string
    azimuth = String((char *)serial_buffer + 1).toInt(); // +1 to jump over 't'
    elevation = String(comma_ptr + 1).toInt(); // +1 to jump over ','
  }
  else
  {
    // Single value, only set azimuth and elevation to 0
    azimuth = String((char *)serial_buffer + 1).toInt(); // +1 to jump over 's'
  }

  // Now set the target
  rotator_target_orientation(azimuth, elevation);

  // Confirm setting back to serial CLI
  Serial.print(F("set_target: "));
  Serial.print(azimuth);
  Serial.print(F(" "));
  Serial.print(elevation);
  Serial.println();
}

// Outputs to serial the current orientation of the rotator
//
void serial_cli_cmd_get_orientation()
{
  rotator_values cur_orientation ;
  rotator_current_orientation(&cur_orientation);

  Serial.print(F("current_orientation: "));
  Serial.print(cur_orientation.azimuth);
  Serial.print(F(" "));
  Serial.print(cur_orientation.elevation);
  Serial.println();
}

// Stop motors (ramp down)
//
void serial_cli_cmd_stop_motors()
{
  rotator_stop_motors();
  Serial.print(F("Stopping motors\n"));
}

// Emergency stop motors immediately
//
void serial_cli_cmd_emergency_stop_motors()
{
  rotator_emergency_stop_motors();
  Serial.print(F("EMERGENCY Stop motors\n"));
}

// Move to home orientation
//
void serial_cli_cmd_home_orientation()
{
  rotator_home_orientation();
  Serial.print(F("Move to Home orientation (0,0)\n"));
}

// Help/banner info
//
void serial_cli_print_help(void)
{
  Serial.println(F("Az/El Rotator - www.areg.org.au"));
  Serial.println();
  Serial.println(F("Simple CLI serial commands:"));
  Serial.println(F("  t|T<azimuth>,<elevation> = set target, e.g. 't90,30' is East with 30 degrees elevation"));
  Serial.println(F("  g|G - get current orientation, returns azimuth elevation, e.g. 'current_orientation: 145 0'"));
  Serial.println(F("  h|H - move to Home orientation (0,0)"));
  Serial.println(F("  s|S - stop motors (nicely) by ramping down"));
  Serial.println(F("  e|E - EMERGENCY stop motors immediately"));
  Serial.println(F("   ?  - Help"));
  Serial.println();
}

// ------------- Spid Rot2 protocol ----------------

// Spid Rot2 protocol parsing
//
void serial_spid_rot2_parse_command()
{
  int azimuth, elevation = 0;
  bool error = false;

  switch (serial_buffer[11])    // command byte
  {
    case 0x0f:    // stop
      rotator_stop_motors();              //use slow down mechanisms
      delay(0.5);                         //give motors time to stop <- TODO can't have delay! blocks motor control
      serial_spid_rot2_send_response();   //send current position
      break;
    case 0x1f:    // status
      serial_spid_rot2_send_response();   //send current position
      break;
    case 0x2f:    // set
      // parse value from serial buffer
      azimuth = serial_spid_rot2_parse_direction( &serial_buffer[1], 4, &error );
      elevation = serial_spid_rot2_parse_direction( &serial_buffer[6], 4, &error );

      // range check and limit the azimuth
      // TODO - need to account for wind up here
      if( azimuth < az_min_degrees )
        azimuth = az_min_degrees;
      else if( azimuth > az_max_degrees )
        azimuth = az_max_degrees;

      // range check and limit the elevation
      if( elevation < el_min_degrees )
        elevation = el_min_degrees;
      else if( elevation > el_max_degrees )
        elevation = el_max_degrees;

      //move rotator if no errors in parsing values
      if( !error )
        rotator_target_orientation( azimuth, elevation );
      break;
    default:
      break;
  }
}

void serial_spid_rot2_send_response()
{
  // fetch current position
  rotator_values cur_orientation;
  rotator_current_orientation(&cur_orientation);

  // prepare az/el values
  // TODO - check wind up values?
  uint16_t az = cur_orientation.azimuth + 360;     //no negative numbers
  uint16_t el = cur_orientation.elevation + 360;

  // fill buffer old school method
  char buf[12];
  buf[0] = 0x57;
  buf[1] = (az % 1000) / 100;
  buf[2] = (az % 100) / 10;
  buf[3] = (az % 10) / 1;
  buf[4] = 0x00;                      // ignore the 1/10th value
  buf[5] = spid_pulse_resolution;
  buf[6] = (el % 1000) / 100;
  buf[7] = (el % 100) / 10;
  buf[8] = (el % 10) / 1;
  buf[9] = 0x00;                      // ignore the 1/10th value
  buf[10] = spid_pulse_resolution;
  buf[11] = 0x20;

  // send entire buffer
  Serial.write(buf,12);
}

int serial_spid_rot2_parse_direction( byte *buf, byte len, bool *err )
{
  // don't do something stupid, make sure buf and len are valid
  if ((len != 4) || (buf == NULL))
  {
    *err = true;
    return 0;
  }

  // rebuild unsigned value stored in buf
  uint16_t u_dir = ((buf[0] - 0x30) * 1000);
  u_dir += ((buf[1] - 0x30) * 100);
  u_dir += ((buf[2] - 0x30) * 10);
  u_dir += ((buf[3] - 0x30) * 1);

  // application should have read spid pulse resolution from status packet,
  // ignore anything invalid
  if( buf[4] != spid_pulse_resolution )
  {
    *err = true;
    return 0;
  }

  // make sure that unsigned direction is sane
  if (u_dir > (720*spid_pulse_resolution))     // three full rotations !
  {
    *err = true;
    return 0;
  }

  *err = false;

  //now calculate and return the result
  return (int)( u_dir / spid_pulse_resolution ) - 360;   //yes negative numbers are allowed
}
