/*
 * servo.cpp
 *
 *  Created on: 24.01.2017
 *      Author: Oliver Struckmeier
 */

#include <rodos.h>
#include <inttypes.h>
#include "hal.h"
#include "../header/servo.h"

HAL_GPIO* HBRIDGE_D_INA2;
HAL_GPIO* HBRIDGE_D_INB2;
HAL_PWM* pwm;

bool out;

SERVO::SERVO(HAL_GPIO *INA, HAL_GPIO *INB, HAL_PWM *p){
	HBRIDGE_D_INA2 = INA;
	HBRIDGE_D_INB2 = INB;
	pwm = p;
}

void SERVO::init(){
	pwm->init(5000,1000);
	HBRIDGE_D_INA2->init(true,1,1);
	HBRIDGE_D_INB2->init(true,1,0);
	pwm->write(0);
}

void SERVO::write(float asdfsdfgasdfasdf){
	//inverse pins according to turn direction
	if(!out){
		HBRIDGE_D_INA2->setPins(0);
		HBRIDGE_D_INB2->setPins(1);
	}else{
		HBRIDGE_D_INA2->setPins(1);
		HBRIDGE_D_INB2->setPins(0);
	}
	pwm->write(asdfsdfgasdfasdf);
}

void SERVO::setDir(bool in){
	//set servo direction
	out = in;
}
void SERVO::resetPins(bool xa){
	//toggle pins
	if(xa){
		HBRIDGE_D_INA2->setPins(1);
		HBRIDGE_D_INB2->setPins(1);
	}else{
		HBRIDGE_D_INA2->setPins(0);
		HBRIDGE_D_INB2->setPins(0);
	}
}
