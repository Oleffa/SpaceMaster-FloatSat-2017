/*
 * imu.cpp
 *
 *  Created on: 30.11.2016
 *      Author: Oliver Struckmeier
 */

#include "../header/imu.h"
#include <Math.h>
#include <rodos.h>
HAL_GPIO* cs;
HAL_GPIO* en;
HAL_I2C* i2c;

//accelerometer register
imu_registers acc_reg[] = {
		{0x20, 0b01110111}, /* (0111) 200Hz (0) continuous (111) xyz axis enable */
		{0x21, 0b00000000}, /* (00) 773 Hz Filter frequency (000) 2g mode (00) normal mode (0) spi enable */
};
//gyro register
imu_registers gyr_reg[] = {
		{0x20,0b11111111},//{0x20,0b10111111}, /* (10) 380Hz (11) Bandwidth 100 (1) normal mode (111) axis enable */
		{0x23,0b00000000}, /* (0) Block data update (0) Big Endian (00) 245 dps (0) empty (00) normal mode (0) spi */
		{0xFF,0b00000000}  /* Signal end of register array */
};
//magnetometer registers
imu_registers mag_reg[] = {
		{0x24, 0b11110000},//{0x24, 0b1111000}, /* (1) temperature enable (11) high resolution (101) 100Hz (00) interupt latched */
		{0x25, 0b00000000}, /* (0) empty (01) 2 gauss (00000) empty */
		{0x26, 0b00000000}, /* (00) HP filter normal mode (0) Filter bypass (00) empty (0) normal (00) normal mode */
};
//imu adresses
#define gyr_adr 0x6B //i2c adress of gyro
#define acc_adr	0x1D //i2c adress of accelerometer
#define mag_adr 0x1D //i2c adress of magnetometer

//data registers
imu_data_reg imu_dat_reg[] = {
		0xA8, //gyro data register
		0xA8, //acc data register
		0x88, //mag data register
		0x85  //temp data register
};

//Scale factor for the accelerometer and gyro
float acc_scale[] ={
		0.00006015,
		0.00006077,
		0.00006185
};
//scale values for gyro
float gyr_scale[]={
		0.00015272,
		0.00015272,
		0.00015272
		/*0.0175,
		0.0175,
		0.0175*/
};

//calibration
int samples = 1000;
int done_samples_acc = 0;
int done_samples_gyr = 0;
float acc_buffer[3];
float gyr_buffer[3];
float acc_offset[]={
		0,
		0,
		0
};
//Gyro Offset
float gyr_offset[]={
		0,
		0,
		0
};
//Magnetometer Offset
float mag_offset[]={
		-476.8980,
		-296.5852,
		-468.3243

};

float sum[3] = {0, 0, 0}; //summ variable for accelerometer
float sum2[3] = {0, 0, 0}; //summ variable for gyro


IMU::IMU(HAL_I2C *imu_i2c, HAL_GPIO *imu_en, HAL_GPIO *imu_chipSelect){
	i2c = imu_i2c;
	cs = imu_chipSelect;
	en = imu_en;
}

void IMU::init(){
	//init the pins
	cs->init(true, 1, 1); //chip select
	en->init(true, 1, 1); //power enable pin
	i2c->init(400000); //init i2c
	//set registers acc
	for(int i = 0; i < sizeof(*acc_reg);i++){
		i2c->write(acc_adr, (uint8_t*) (acc_reg[i]), 2); //1 for acc
	}
	//set register gyro
	for(int i = 0; i < sizeof(*gyr_reg);i++){
		i2c->write(gyr_adr, (uint8_t*) (gyr_reg[i]), 2); //1 for acc
	}
	//set register mag
	uint8_t a1[2] = {0x24, 0b00000000};
	uint8_t a2[2] = {0x25, 0b11110000};
	uint8_t a3[2] = {0x26, 0b00000000};
	i2c->write(mag_adr, a1, 2);
	i2c->write(mag_adr, a2, 2);
	i2c->write(mag_adr, a3, 2);
}
//read the accelerometer (raw and scaled)
void IMU::readAccelerometer(float* data, bool readRaw){
	//read raw values
	//buffer for raw values
	uint8_t buffer[6];
	//read raw data
	i2c->writeRead(acc_adr, imu_dat_reg[1],1,buffer,6);
	//bitshift
	data[0] = (int16_t) ((buffer[1] << 8) | buffer[0]);
	data[1] = (int16_t) ((buffer[3] << 8) | buffer[2]);
	data[2] = (int16_t) ((buffer[5] << 8) | buffer[4]);

	//Scaling
	if(!readRaw){
		data[0] = (data[0] - acc_offset[0]) * acc_scale[0];
		data[1] = (data[1] - acc_offset[1]) * acc_scale[1];
		data[2] = (data[2] - acc_offset[2]) * acc_scale[2];
	}
}
//read the gyr (raw and scaled)
void IMU::readGyro(float* data, bool readRaw){
	//read raw values
	//buffer for raw values
	uint8_t buffer[6];
	//read raw data
	i2c->writeRead(gyr_adr, imu_dat_reg[0],1,buffer,6);
	//bitshift
	data[0] = (int16_t) ((buffer[1] << 8) | buffer[0]);
	data[1] = (int16_t) ((buffer[3] << 8) | buffer[2]);
	data[2] = (int16_t) ((buffer[5] << 8) | buffer[4]);
	//Scaling
	if(!readRaw){
		data[0] = (data[0] - gyr_offset[0]) * gyr_scale[0];
		data[1] = (data[1] - gyr_offset[1]) * gyr_scale[1];
		data[2] = (data[2] - gyr_offset[2]) * gyr_scale[2];
	}
	data[0] = (data[0]* 180.0)/M_PI;
	data[1] = (data[1]* 180.0)/M_PI;
	data[2] = (data[2]* 180.0)/M_PI;
}
//calculate the euer angles by integration (unused)
void IMU::readEuler(float* data, float refreshRate){
	float angularVelocity[3];
	readGyro(angularVelocity, false);	//read angular velocity

	//add the angular velocity * time to the old angle
	data[0] = data[0] + (angularVelocity[0] * (1/refreshRate));
	data[1] = data[1] + (angularVelocity[1] * (1/refreshRate));
	data[2] = data[2] + (angularVelocity[2] * (1/refreshRate));

}
//read magnetometer values (raw and scaled)
void IMU::readMagnetometer(float* data, bool readRaw){
	uint8_t buffer[6];
	if(i2c->writeRead(mag_adr, imu_dat_reg[2], 1, buffer, 6) != 6){
		data[0] = data[1] = data[2] = 0.0;
		//PRINTF("ERROR: imu.cpp/readMagnetometer\n");
	}
	//bitshift
	data[0] = ((int16_t) ((buffer[1] << 8) | buffer[0])) * 0.08; //0.08 = scaling factor
	data[1] = ((int16_t) ((buffer[3] << 8) | buffer[2])) * 0.08;
	data[2] = ((int16_t) ((buffer[5] << 8) | buffer[4])) * 0.08;

	if(!readRaw){
		//von hand abgemessene min und max werte um die {x, y, z achse}
		float MxMin[3] = {-82.0, -30.0, -45.0};
		float MxMax[3] = {30.0, 57.0, 64.0};

		float mv[3];
		//scaling about the max and min values from pdf
		mv[0] = ((data[0] - MxMin[0])/(MxMax[0]-MxMin[0])*2)-1;
		mv[1] = ((data[1] - MxMin[1])/(MxMax[1]-MxMin[1])*2)-1;
		mv[2] = ((data[2] - MxMin[2])/(MxMax[2]-MxMin[2])*2)-1;
		data[0] = mv[0];
		data[1] = mv[1];
		data[2] = mv[2];
	}
}

//Calibrations: called until the amount of samples is reached then main_sensors switches to the next mission mode
//accelerometer calibration
bool IMU::calibrateAccelerometer(){

	if(done_samples_acc < samples){
		readAccelerometer(acc_buffer, false);
		//output(acc_buffer);
		sum[0] += acc_buffer[0];
		sum[1] += acc_buffer[1];
		sum[2] += acc_buffer[2];
		done_samples_acc++;
		return false;
	}else{
		done_samples_acc = 0;
		acc_offset[0] = (sum[0] / samples);
		acc_offset[1] = (sum[1] / samples);
		acc_offset[2] = (sum[2] / samples);
		return true;
	}
}
//gyro calibration
bool IMU::calibrateGyro(){
	if(done_samples_gyr < samples){
		readGyro(gyr_buffer, false);
		//output(acc_buffer);
		sum2[0] += gyr_buffer[0];
		sum2[1] += gyr_buffer[1];
		sum2[2] += gyr_buffer[2];
		done_samples_gyr++;
		//PRINTF("GYRO: 1: %f, 2: %f, 3: %f\n", gyr_buffer[0], gyr_buffer[1], gyr_buffer[2]);
		return false;
	}else{
		done_samples_gyr = 0;
		gyr_offset[0] = (sum2[0] / samples);
		gyr_offset[1] = (sum2[1] / samples);
		gyr_offset[2] = (sum2[2] / samples);
		//PRINTF("GYRO: 1: %f, 2: %f, 3: %f\n", gyr_offset[0], gyr_offset[1], gyr_offset[2]);
		return true;
	}
}

//get imu offsets
void IMU::getAccOffset(float* data){
	data[0] = acc_offset[0];
	data[1] = acc_offset[1];
	data[2] = acc_offset[2];
}
void IMU::getGyrOffset(float* data){
	data[0] = gyr_offset[0];
	data[1] = gyr_offset[1];
	data[2] = gyr_offset[2];
}

void IMU::output(float* data_a, float* data_g){
	//output function for imu data
	char xd3[100];
	PRINTF("Acc[0] = %f : Acc[1] = %f : Acc[2] = %f ||| Gyr[0] = %f, Gyr[1] = %f, Gyr[2] = %f\n", data_a[0], data_a[1], data_a[2],data_g[0], data_g[1], data_g[2]);
}

//calculates the heading angle according to the pdf on wuecampus
float IMU::getHeading(){
	float a1[3] = {0.0, 0.0 ,0.0};
	float m1[3] = {0.0, 0.0, 0.0};
	readAccelerometer(a1, true);
	readMagnetometer(m1, false);

	//scale the accelerometer values
	a1[0] = a1[0] - acc_offset[0];
	a1[1] = a1[1] - acc_offset[1];
	a1[2] = a1[2] - acc_offset[2] - 1000;

	//get roll and pitch angle from the acc vectors
	float rollAngleAcc = atan(a1[1] / sqrt((a1[0] * a1[0])+(a1[2]*a1[2])));
	float pitchAngleAcc = atan(a1[0] / sqrt((a1[1] * a1[1])+(a1[2]*a1[2])));

	//tilt compensation
	float mxh = m1[0] * cosf(pitchAngleAcc) + m1[2] * sinf(pitchAngleAcc);
	float myh = m1[0] * sinf(rollAngleAcc) * sinf(pitchAngleAcc) + m1[1] * cosf(rollAngleAcc)
				- m1[2] * sinf(rollAngleAcc) * cosf(pitchAngleAcc);

	//calculate heading
	float heading = atan(myh / mxh);

	//compensation
	if (mxh < 0) {
		heading = 3.1415 - heading;
	} else if (mxh > 0 && myh < 0) {
		heading = -heading;
	} else if (mxh > 0 && myh > 0) {
		heading = (2 * 3.1415) - heading;
	} else if (mxh == 0 && myh < 0) {
		heading = (3.1415 / 2);
	} else if (mxh == 0 && myh > 0) {
		heading = (3 / 2) * 3.1415;
	}
	//rad-> deg
	heading = heading * 57.2958 - 90; //90 degree offset to match the actual north pole

	//if turned around more than 1 full rotation reset the heading angle
	if(heading > 360){
		heading = heading - 360;
	}
	if(heading < 0){
		heading = heading + 360;
	}

	return heading;
}

