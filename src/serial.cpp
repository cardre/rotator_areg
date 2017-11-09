// Functions related to handling incoming serial data
// rototor_areg
// VK5CD

#include <Arduino.h>
#include "serial.h"
#include "rotator.h"
#include "config.h"

// Serial data buffer handling
const int serial_buffer_size = 30;
const char eol = byte('\n');
char serial_buffer[serial_buffer_size];
byte next_serial_index = serial_buffer_size ; // so inital clear zeros buffer


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

  switch (serial_buffer[0])
  {
    // All the chars for our simple CLI serial interface
    case 't': // set Target
    case 'T':
    case 'g': // Get current orientation
    case 'G':
    case 's': // Stop motors (ramp down)
    case 'S':
    case 'e': // Emergency stop motors immediately
    case 'E':
    case 'h': // Move to home positoin
    case 'H':
      if ( strchr(serial_buffer, eol) )
      {
        // Got a complete cmd, so process it
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
            // Move to home position 0,0
            serial_cli_cmd_home_orientation();
            break;
        }
        // Cmd has been handled, clear out buffer
        serial_data_clear();
      }
      else if ( next_serial_index >= serial_buffer_size )
      {
        // Buffer Overflowed, so clear all data and start again
        serial_data_clear();
      }
      else
      {
        // Still waiting for more data to get 'end of line', so return now
        return;
      }
      break;

    default:
      // No one handled the serial data byte, so throw it away
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

// Process the CLI cmd to set target
// format is [s|S][\-,0..9]*()\,[\-,0..9]*).
// e.g. east & 45 degree elevation = 'S90,45'
// e.g. west and no change to elevation = 's-90'
void serial_cli_cmd_set_target()
{
  int azimuth, elevation = 0 ;

  // See if we have 1 (az) or 2 (az,el) numbers
  char * comma_ptr = strchr(serial_buffer, ',');
  if ( comma_ptr )
  {
    // Should be 2 values comma separated
    * comma_ptr = 0 ; // terminate az string
    azimuth = String(serial_buffer + 1).toInt(); // +1 to jump over 't'
    elevation = String(comma_ptr + 1).toInt(); // +1 to jump over ','
  }
  else
  {
    // Single value, only set azimuth and elevation to 0
    azimuth = String(serial_buffer + 1).toInt(); // +1 to jump over 't'
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
