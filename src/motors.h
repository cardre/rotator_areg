// Functions related to motor control
// rototor_areg
// VK5CD

// Arduino PWM Speed Control using LM298 board
const int E1 = 5; // Direction on El motor, HIGH = pitch up
const int M1 = 4; // PWM motor speed for El motor
const int E2 = 6; // Direction on Az motor, HIGH = clockwise
const int M2 = 7; // PWM motor speed for Az motor

void motors_setup();
void set_el_motor_pwm_speed(int pwm_speed);
void set_az_motor_pwm_speed(int pwm_speed);
void test_motors_movement();
void set_az_motor_dir_pitch_up(bool pitch_up);
void set_el_motor_dir_clockwise(bool clockwise);
