/*
 * topics.cpp
 *
 *  Created on: 30.11.2016
 *  Author: Oliver Struckmeier
 *
 *  this Class contains the satellite internal topics
 */

#include <rodos.h>
#include "../header/main_sensors.h"
#include "../header/SatTopics.h"

Topic<long> duty_cycle(1, "motor duty cycle");
Topic<state> mission_mode(2, "mission mode");
Topic<sat_attitude2> sat_attitude(3, "attitude");
Topic<float_3_struct> acc_topic(4, "acc values");
Topic<float_3_struct> gyr_topic(5, "gyr values");
Topic<float_3_struct> mag_topic(6, "mag values");
Topic<float> angular_velocity_topic(7, "angular velocity");
Topic<float> sat_heading_topic(8, "satellite heading angle");
Topic<float> light_sensor_topic(9, "light intensity");
Topic<float> panel_voltage_topic(11, "Panel Voltage");
Topic<float> panel_current_topic(12, "Panel Current");
Topic<float> battery_voltage_topic(13, "Battery Voltage");
Topic<float> battery_current_topic(14, "Battery Current");
Topic<bool> sun_found_topic(15, "sun found topic");

Topic<pid_stuff> pid_values_1(16, "pid values");
Topic<pid_stuff> pid_values_2(17, "pid values");

//TC stuff
Topic<float> joystick_topic(18, "joystick input");
Topic<docking_go_internal> docking_topic(19, "joystick input");
Topic<state> mission_mode_telecommand(20, "mission mode from tc input");
Topic<velocity_rotation_internal> velocity_mode_topic(21,"vel mode");
Topic<velocity_angle_internal> angle_mode_topic(22,"vel mode");
