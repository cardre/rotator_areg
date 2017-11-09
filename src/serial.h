// Functions related to handling incoming serial data
// rototor_areg
// VK5CD
//

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

// SPID protocol - TODO
