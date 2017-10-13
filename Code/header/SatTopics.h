#include <rodos.h>
#include "main_sensors.h"
//#include "../wifi/topics.h"

struct sat_attitude2
{
	float raw_pitch; // rad
	float raw_roll; // rad
	float raw_yaw; // rad
	float raw_pitch_velocity; // rad/sec
	float raw_roll_velocity; // rad/sec
	float raw_yaw_velocity; // rad/sec
	float pitch; // rad
	float roll; // rad
	float yaw; // rad
	float pitch_velocity; // rad/sec
	float roll_velocity; // rad/sec
	float yaw_velocity; // rad/sec
};

struct float_3_struct{
	float value1;
	float value2;
	float value3;
};

struct pid_stuff{
	uint8_t controller;
	float p;
	float i;
	float d;
};
struct docking_go_internal{
	uint8_t go;
};
struct velocity_rotation_internal{
	float value;
};
struct velocity_angle_internal{
	float value;
};
extern Topic<long> duty_cycle;
extern Topic<state> mission_mode;
extern Topic<sat_attitude2> sat_attitude;
extern Topic<float_3_struct> acc_topic;
extern Topic<float_3_struct> gyr_topic;
extern Topic<float_3_struct> mag_topic;
extern Topic<float> angular_velocity_topic;
extern Topic<float> sat_heading_topic;
extern Topic<float> light_sensor_topic;
extern Topic<bool> sun_found_topic;
extern Topic<float> panel_voltage_topic;
extern Topic<float> panel_current_topic;
extern Topic<float> battery_voltage_topic;
extern Topic<float> battery_current_topic;
extern Topic<pid_stuff> pid_values_1;
extern Topic<pid_stuff> pid_values_2;
//TC stuff
extern Topic<float> joystick_topic;
extern Topic<docking_go_internal> docking_topic;
extern Topic<state> mission_mode_telecommand;
extern Topic<velocity_rotation_internal> velocity_mode_topic;
extern Topic<velocity_angle_internal> angle_mode_topic;
