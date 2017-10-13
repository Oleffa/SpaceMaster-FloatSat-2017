/*
 * MadgwickFilter.cpp
 *
 *  Created on: 09.01.2017
 *      Author: Unkown
 *      Reworked by Oliver Struckmeier 1.1.2017
 */

#include "rodos.h"
#include "stdint.h"
#include "../header/madgwickFilter.h"

/*=========================================Help Methods===============================================*/
/*
 * Found on: https://stackoverflow.com/questions/1349542/john-carmacks-unusual-fast-inverse-square-root-quake-iii
 * used for faster inverse square root
 */
float FastInvSqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int*)&x;              // get bits for floating value
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float*)&i;                // convert bits back to float
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    return x;
}

/**
 * Constructor
 *
 * @param float filter_interval:	Filter update interval
 * @param float filter_gain:		Filter gain
 */
MadgwickFilter::MadgwickFilter(float filter_interval, float filter_gain){
	this->beta = filter_gain;
	this->interval = filter_interval;
    q0 = 1;
    q1 = q2 = q3 = 0;
}

//Calculates the filtered values
void MadgwickFilter::filterUpdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
	float scale; // Used to scale xyz values to length 1

	/* Rate of change of quaternion from gyroscope */
	float qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	float qDot2 = 0.5f * ( q0 * gx + q2 * gz - q3 * gy);
	float qDot3 = 0.5f * ( q0 * gy - q1 * gz + q3 * gx);
	float qDot4 = 0.5f * ( q0 * gz + q1 * gy - q2 * gx);

	/* Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation) */
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		/* Normalise accelerometer measurement */
		scale = FastInvSqrt(ax * ax + ay * ay + az * az);
		ax *= scale;
		ay *= scale;
		az *= scale;

		/* Normalise magnetometer measurement */
		scale = FastInvSqrt(mx * mx + my * my + mz * mz);
		mx *= scale;
		my *= scale;
		mz *= scale;

		// Auxiliary variables to avoid repeated arithmetic
		float _2q0mx = 2.0f * q0 * mx;
		float _2q0my = 2.0f * q0 * my;
		float _2q0mz = 2.0f * q0 * mz;
		float _2q1mx = 2.0f * q1 * mx;
		float _2q0 = 2.0f * q0;
		float _2q1 = 2.0f * q1;
		float _2q2 = 2.0f * q2;
		float _2q3 = 2.0f * q3;
		float _2q0q2 = 2.0f * q0 * q2;
		float _2q2q3 = 2.0f * q2 * q3;
		float q0q0 = q0 * q0;
		float q0q1 = q0 * q1;
		float q0q2 = q0 * q2;
		float q0q3 = q0 * q3;
		float q1q1 = q1 * q1;
		float q1q2 = q1 * q2;
		float q1q3 = q1 * q3;
		float q2q2 = q2 * q2;
		float q2q3 = q2 * q3;
		float q3q3 = q3 * q3;

		// Reference direction of Earth's magnetic field
		float hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2 + _2q1 * mz * q3 - mx * q2q2 - mx * q3q3;
		float hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q3 - my * q3q3;
		float _2bx = sqrt(hx * hx + hy * hy);
		float _2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1 + _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
		float _4bx = 2.0f * _2bx;
		float _4bz = 2.0f * _2bz;
		float _8bx = 2.0f * _4bx;
		float _8bz = 2.0f * _4bz;

		// Gradient decent algorithm corrective step
		float t0 = 0.5 - q1q1 - q2q2;
		float t1 = (2.0f*(q1q3 - q0q2) - ax);
		float t2 = (2.0f*(q0q1 + q2q3) - ay);
		float t3 = 4.0f * (2.0f*t0 - az);
		float t4 = (_4bx*(0.5 - q2q2 - q3q3) + _4bz*(q1q3 - q0q2) - mx);
		float t5 = _4bx*(q0q2 + q1q3) + _4bz*t0 - mz;
		float t6 = _4bx*(q1q2 - q0q3) + _4bz*(q0q1 + q2q3) - my;

		float s0 = -_2q2 * t1 + _2q1 * t2                          - _4bz * q2  * t4 + ( -_4bx*q3 + _4bz*q1) * t6 +  _4bx * q2              * t5;
		float s1 =  _2q3 * t1 + _2q0 * t2 - q1 * t3                + _4bz * q3  * t4 + (  _4bx*q2 + _4bz*q0) * t6 + (_4bx * q3 - _8bz * q1) * t5;
		float s2 = -_2q0 * t1 + _2q3 * t2 - q2 * t3  + (-_8bx * q2 - _4bz * q0) * t4 + (  _4bx*q1 + _4bz*q3) * t6 + (_4bx * q0 - _8bz * q2) * t5;
		float s3 =  _2q1 * t1 + _2q2 * t2            + (-_8bx * q3 + _4bz * q1) * t4 + ( -_4bx*q0 + _4bz*q2) * t6 +  _4bx * q1              * t5;

		// normalise step magnitude
		scale = FastInvSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
		s0 *= scale;
		s1 *= scale;
		s2 *= scale;
		s3 *= scale;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * interval;
	q1 += qDot2 * interval;
	q2 += qDot3 * interval;
	q3 += qDot4 * interval;

	// Normalise quaternion
	scale = FastInvSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= scale;
	q1 *= scale;
	q2 *= scale;
	q3 *= scale;
}
/**
 * Calculates Euler angles (roll,pitch,yaw) from the quaternion q.
 * The resulting angles range from -180 to 180 degrees.
 *
 * The pitch angle is positive if the nose of the spacecraft is pointing up
 *
 * @param float* data: A pointer to a float[3] array. The values are
 * 						stored as:	data[0] = roll
 * 									data[1] = pitch
 * 									data[2] = yaw
 */
void MadgwickFilter::getEulerAnglesDeg(float* data) {
	float temp = 2*q0*q0 - 1;	// Avoid recalculating this
	data[0] = 57.295779513 * atan2f(2.0f * (q0*q1 + q2*q3), temp + 2*q3*q3);	// roll
	data[1] = 57.295779513 * asinf( 2.0f * (q0*q2 - q1*q3));					// pitch
	data[2] = 57.295779513 * atan2f(2.0f * (q1*q2 + q0*q3), temp + 2*q1*q1);	// yaw
}

/**
 * Calculates Euler angles (roll,pitch,yaw) from the quaternion q.
 * The resulting angles range from -pi to pi degrees.
 *
 * The pitch angle is positive if the nose of the spacecraft is pointing up
 *
 * @param float* data: A pointer to a float[3] array. The values are
 * 						stored as:	data[0] = roll
 * 									data[1] = pitch
 * 									data[2] = yaw
 */
void MadgwickFilter::getEulerAnglesRad(float* data) {
	float temp = 2*q0*q0 - 1;	// Avoid recalculating this
	data[0] = atan2f(2.0f * (q0*q1 + q2*q3), temp + 2*q3*q3);	// roll
	//data[0] = atan2f(2.0f * (q0*q1 + q2*q3), q0*q0 + q1*q1 - q2*q2 - q3*q3 );	// roll
	data[1] = asinf(-2.0f * (q0*q2 - q1*q3));					// pitch
	data[2] = atan2f(2.0f * (q1*q2 + q0*q3), temp + 2*q1*q1);	// yaw
	//data[2] = atan2f(2.0f * (q1*q2 + q0*q3),  q0*q0 - q1*q1 - q2*q2 + q3*q3);	// yaw
}

/**
 * Resets the quaternion to (1,0,0,0)
 */
void MadgwickFilter::resetFilter(){
	q0 = 1.0f;
	q1 = q2 = q3 = 0.0f;
}
void MadgwickFilter::setGain(float gain) {
	beta = gain;
}

float MadgwickFilter::getGain() {
	return beta;
}

void MadgwickFilter::getQuaternion(float* quat) {
	quat[0] = q0;
	quat[1] = q1;
	quat[2] = q2;
	quat[3] = q3;
}


