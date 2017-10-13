/*
 * main_sensors.h
 *
 *  Created on: 01.12.2016
 *      Author: Oliver Struckmeier
 */

#ifndef HEADER_MAIN_SENSORS_H_
#define HEADER_MAIN_SENSORS_H_

/*
 * Names of the modes
 */
typedef enum{
	IDLE = 0,
	CALIBRATING = 1,
	FIND_SUN = 2,
	OPENPANEL = 3,
	EMERGENCY= 4,
	TURN_WITH = 5,
	TURN_FOR = 6,
	EXTEND = 7,
	RETRACT = 8,
	TAKEPIC = 9,
	DOCKING = 10,
	JOYSTICKMODE = 11
}state;
#endif /* HEADER_MAIN_SENSORS_H_ */
