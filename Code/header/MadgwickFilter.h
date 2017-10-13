/*
 * MadgwickFilter.h
 *
 *  Created on: 09.01.2017
 *      Author: Unkown
 *      Reworked by Oliver Struckmeier 1.1.2017
 */

#ifndef HEADER_MADGWICKFILTER_H_
#define HEADER_MADGWICKFILTER_H_
#include "stdint.h"
#include "math.h"

class MadgwickFilter {

private:

	volatile float q0, q1, q2, q3;

	volatile float beta;

	volatile float interval;

public:

	float getGain();

	MadgwickFilter(float filter_interval = 0.01f, float filter_gain = 0.1f);

	void filterUpdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);

	void getEulerAnglesDeg(float* data);

    void getEulerAnglesRad(float* data);

	void resetFilter();

	void setGain(float gain);

	void getQuaternion(float* quat);
};

#endif
