#ifndef SERVO_H
#define SERVO_H

#include "rodos.h"
#include "stdint.h"
#include <string>


class SERVO{
public:
	SERVO(HAL_GPIO *INA, HAL_GPIO *INB, HAL_PWM *pwm);
	void init();
	void write(float dc_servo);
	void setDir(bool in);
	void resetPins(bool xa);
};
#endif
