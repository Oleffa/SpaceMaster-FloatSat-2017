/*****************************************************************
topics.h

Original Created by: Atheel Redah @ University of Wrzburg
Original Creation Date: March 8, 2015

Development environment specifics:
	Software Platform: Rodos (Realtime Onboard Dependable Operating System).
	Hardware Platform: STM32F4 + Wrzburg Uni Informatik 8 Discovery AddOn Board Version 2.0.
*****************************************************************/

#ifndef __topics_h__
#define __topics_h_


/* Includes ------------------------------------------------------------------*/
#include "wifi_Interface/wf121.h"
#include "wifi_Interface/linkinterfacewf121.h"
#define PACK_SIZE	50


//OWN CODE
struct attitude
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
struct light_sensor
{
	float value; //no unit
	float ignore1; //no unit
	float ignore2; //no unit
	float ignore3; //no unit
};
struct battery_power
{
	float voltage; //mV
	float current; //mA
};

struct solar_panel_power
{
	float voltage; //mV
	float current; //mA
};

struct tm_struct_motor{
	float value;
};

struct image{
	uint8_t format; // 0 => bmp, ...
	static const uint32_t num_bytes = 50688;
	uint8_t bytes[num_bytes];
};

struct tm_struct_thermal_knife
{
	uint8_t value; // 0 => off, 1 => on (automatically turned off by board after Xsec)
};
struct tm_struct_pid_values
{
	uint8_t controller; // 0 => reaction wheel, 1 => satellite velocity, 2 => satellite attitude
	float p;
	float i;
	float d;
};

struct tm_struct_connection{
	uint8_t connection;
};

struct tm_mode_struct{
	uint8_t state;
};

/*=====Telecommand Topics=====*/
//Command to turn on knife
struct tc_struct_thermal_knife {
	uint8_t value; // 0 => off, 1 => on (automatically turned off by board after Xsec)
};
//PID Gains (for up to the 3 controllers: 2 actuators and 1 complete rotation control) (9 gains)

struct tc_struct_pid_values {
	uint8_t controller; // 0 => reaction wheel, 1 => satellite velocity, 2 => satellite attitude
	float p;
	float i;
	float d;
};
//motor
struct reaction_wheel
{
	float value; //rpm, +:counter-clockwise, -:clockwise
};
//joystick stuff
struct joystick_values{
	float value;
};
//docking command
struct docking_go{
	uint8_t go;
};
//angle rotation activation
struct angle_rotation{
	float value;
};
//vel rotation
struct velocity_rotation{
	float value;
};
//take pic
struct tc_camera_struct{
	uint8_t snap;
};
//whatever
/*struct tc_imu_calibration_struct{

};
struct tc_mode_type_struct{

};
*/

//TM
extern Topic<attitude> tm_wifi_attitude;
extern Topic<light_sensor> tm_light_sensor;
extern Topic<battery_power> tm_battery_power;
extern Topic<solar_panel_power> tm_solar_panel_power;
extern Topic<tm_struct_motor> tm_motor;
extern Topic<image> tm_image;
extern Topic<tm_struct_thermal_knife> tm_thermal_knife;
extern Topic<tm_struct_pid_values> tm_pid_values;
extern Topic<tm_struct_connection> tm_connection;
extern Topic<tm_mode_struct> tm_mode;
//TC
extern Topic<tc_struct_thermal_knife> tc_thermal_knife;
extern Topic<tc_struct_pid_values> tc_pid_values;
extern Topic<reaction_wheel> tc_reaction_wheel;
extern Topic<joystick_values> tc_joystick;
extern Topic<docking_go> tc_docking;
extern Topic<angle_rotation> tc_angle_rotation;
extern Topic<velocity_rotation> tc_velocity_rotation;
extern Topic<tc_camera_struct> tc_camera;
extern Topic<tc_camera_struct> tc_;
//extern Topic<tc_mode_type_struct> tc_mode_type;
//extern Topic<tc_imu_calibration_struct> tc_imu_calibration




extern HAL_UART gatewayWifi; // USB-UART
extern WF121 wf121;
extern LinkinterfaceWF121 linkwf121;
extern Gateway gw;

#endif













