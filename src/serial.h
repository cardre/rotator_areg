// Functions related to handling incoming serial data
// rototor_areg
// VK5CD
//

#include <Arduino.h>

// Our functions
void serial_data_clear();
void serial_data_handler();

// Protocol implementations

// Simple CLI commands
void serial_cli_cmd_set_target();
void serial_cli_cmd_get_orientation();
void serial_cli_cmd_stop_motors();
void serial_cli_cmd_emergency_stop_motors();
void serial_cli_cmd_home_orientation();
void serial_cli_print_help();

// SPID ROT2 prototocl
void serial_spid_rot2_parse_command();
void serial_spid_rot2_send_response();
int serial_spid_rot2_parse_direction( byte *buf, byte len,  bool *err );
