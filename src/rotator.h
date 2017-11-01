// Functions related to orientation and running motors for the rotator
// rototor_areg
// VK5CD
//
// This file provides the logic to run the rotator independent of any serial protocol
// Rather it provides generic rotator functions that a protocol implementation can call

// Our rotator return values for serial protocol interfaces
struct rotator_values
{
  int azimuth;
  int elevation;
};

// Our functions
void rotator_setup();
void rotator_update();
void rotator_target_orientation(int azimuth, int elevation);
void rotator_target_orientation(rotator_values target);
void rotator_current_orientation(rotator_values * return_values);
void rotator_stop_motors();
void rotator_emergency_stop_motors();
