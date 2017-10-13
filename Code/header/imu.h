/*
 * imu.h
 *
 *  Created on: 30.11.2016
 *      Author: Oliver Struckmeier
 */

#ifndef SRC_IMU_H_
#define SRC_IMU_H_

#include <rodos.h>
#include "stdint.h"
#include <string>

typedef uint8_t imu_registers[2];
typedef uint8_t imu_adresses[1];
typedef uint8_t imu_data_reg[1];

class IMU {
public:
	/*
	 * Public constructor
	 *
	 * @param - imu_i2c: Pointer to i2c object
	 * @param - imu_en: Pointer imu enable pin
	 * @param - imu_chipPin: Pointer to imu chip select pin
	 */
	IMU(HAL_I2C *imu_i2c, HAL_GPIO *imu_en, HAL_GPIO *imu_chipPin);
	/*
	 * Initialize the IMU
	 */
	void init();

	void readAccelerometer(float* data, bool readRaw);
	void readGyro(float* data, bool readRaw);
	void readEuler(float* data, float refreshRate);
	void readMagnetometer(float* data, bool readRaw);

	bool calibrateAccelerometer();
	bool calibrateGyro();

	void getAccOffset(float* data);
	void getGyrOffset(float* data);

	void output(float* data_a, float* data_g);
	//void read();
	float getHeading();
};

#endif /* SRC_IMU_H_ */
