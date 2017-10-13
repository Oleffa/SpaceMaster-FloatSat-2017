#include "rodos.h"
#include "topics.h"

 Topic<attitude> tm_wifi_attitude(901, "Satellite Attitude");
 Topic<light_sensor> tm_light_sensor(902, "lightsensor");
 Topic<battery_power> tm_battery_power(903, "battery power");
 Topic<solar_panel_power> tm_solar_panel_power(904, "panel power");
 Topic<image> tm_image(905, "camera image");
 Topic<tm_struct_thermal_knife> tm_thermal_knife(906, "thermal knife state");
 Topic<tm_struct_pid_values> tm_pid_values(907, "pid values");
 Topic<tm_struct_motor> tm_motor(908, "dc");
 Topic<tm_struct_connection> tm_connection(909, "connected state");
 Topic<tm_mode_struct> tm_mode(910, "mission state");
 //TC
 Topic<tc_struct_thermal_knife> tc_thermal_knife(1001, "Thermal Knife TC");
 Topic<tc_struct_pid_values> tc_pid_values(1002, "PID Gain Values TC");
 Topic<reaction_wheel> tc_reaction_wheel(1003, "Telecommand Reaction Wheel");
 Topic<joystick_values> tc_joystick(1004, "Telecommand Reaction Wheel");
 Topic<docking_go> tc_docking(1005, "Telecommand Reaction Wheel");
 Topic<angle_rotation> tc_angle_rotation(1006, "Telecommand Reaction Wheel");
 Topic<velocity_rotation> tc_velocity_rotation(1007, "Telecommand Reaction Wheel");
 Topic<tc_camera_struct> tc_camera(1008, "Telecommand Reaction Wheel");
 //Topic<tc_mode_type_struct> tc_mode_type(1009, "Telecommand Mode");
//Topic<tc_imu_calibration_struct> tc_imu_calibration(1020, "Imu Calibration");


HAL_UART gatewayWifi(UART_IDX3); // USB-UART
WF121 wf121(&gatewayWifi);
LinkinterfaceWF121 linkwf121(&wf121);
Gateway gw(&linkwf121,true);
