/*
 * main_telemetry.h
 *
 *  Created on: 08.01.2017
 *      Author: Oliver Struckmeier
 */

#ifndef HEADER_MAIN_TELEMETRY_H_
#define HEADER_MAIN_TELEMETRY_H_


struct sat_attitude
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


#endif /* HEADER_MAIN_TELEMETRY_H_ */
