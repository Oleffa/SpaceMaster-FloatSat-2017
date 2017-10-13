/*
 * main_imu.cpp
 *
 *  Created on: 06.02.2017
 *      Author: Oliver Struckmeier
 *
 * reads the lightsensor and publishes the values und ther topic
 */
#include "rodos.h"
#include <stdio.h>
#include "hal.h"
#include "../header/lightSensor_test.h"

float f_imu = 2;
#define TSL_ADDRESS1 0x39
#define TSL_ADDRESS2 0x49
#define TSL_CH0_LOW 0xAC

bool initcalled = false;

float prevLux = 0.0;

#include "../header/SatTopics.h"

HAL_I2C i2c2(I2C_IDX3);  //(IDX3 SCL = PA8, SDA = PC9)

class main_light: public Thread {

public:

	main_light(const char* name) : Thread(name) {
	}

	void init() {
		 myInit();
	}

	void run() {
//		suspendCallerUntil(NOW()+(1000*MILLISECONDS)*10);
		while (1) {
			if(!initcalled){
				myInit();
			}
		//initialized_lightSensor();
		uint8_t i2cAddress;
		uint8_t cmd[2];
		uint8_t rx[4];
		int32_t retVal;
		cmd[0] = 0x9C;

		i2cAddress = LIGHT_I2C_ADRESS;
		//read values from the buffer
		 retVal = i2c2.writeRead(i2cAddress, &cmd[0], 1, rx, 4);
		//PRINTF("The retval is =%d ",retVal);
		//catch error and reset imu then
		 if(retVal<0){
			 i2c2.reset();
			 i2c2.init();
		 }
		 //Bitshift
		 uint16_t channel0 = (rx[1] << 8) | rx[0];
		 uint16_t channel1 = (rx[3] << 8) | rx[2];

		 //Calculate the lux value
			float lux = -1;
			float ratio = (float)channel1 / channel0;
			if (ratio <= 0.5) {
				// lux = (0.0304 - 0.062 * pow(ratio, 1.4)) * channel0;
				// or:
				if (ratio <= 0.125) {
					lux = (0.0304 - 0.0272 * ratio) * channel0;
				}
				else if (ratio <= 0.250) {
					lux = (0.0325 - 0.0440 * ratio) * channel0;
				}
				else if (ratio <= 0.375) {
					lux = (0.0351 - 0.0544 * ratio) * channel0;
				}
				else {
					lux = (0.0381 - 0.0624 * ratio) * channel0;
				}
			}
			else if (ratio <= 0.61) {
				lux = (0.0224 - 0.031 * ratio) * channel0;
			}
			else if (ratio <= 0.80) {
				lux = (0.0128 - 0.0153 * ratio) * channel0;
			}
			else if (ratio <= 1.3){
				lux = (0.00146 - 0.00112 * ratio) * channel0;
			}
			else {
				lux = 0;
			}

			//catch weird huge values of the light sensor

			/*if(lux > prevLux*10 && prevLux!=0.0 && lux > 0){
				lux = prevLux;
			}else{
				prevLux = lux;
			}*/
			//PRINTF("lux: %f\n", lux);
			light_sensor_topic.publish(lux);
			int x = channel0;
			int y = channel1;
			int x1 = rx[1];
			int x2 = rx[2];
			int x3 = rx[3];
			int x4 = rx[4];
			//PRINTF("lux: %f, channel0: %d, channel1: %d, rx1: %d, rx2: %d, rx3: %d, rx4: %d\n", lux,x,y,x1,x2,x3,x4);


			//pause the Thread for a certain time
            suspendCallerUntil(NOW()+(1000*MILLISECONDS)/f_imu);
		}

	}
	void myInit(){
		int a1,a2;

		i2c2.init(400000);

		uint8_t cmd[2];
		uint8_t readback[1];

		cmd[0] = LIGHT_REG_CONTROL;

		//cmd[0] = 0x80;
		cmd[1] = 0x03;
		a1=i2c2.write(LIGHT_I2C_ADRESS, cmd, 2);

		PRINTF("Initialization Status %d\n",a1);





		cmd[0] = LIGHT_COMMAND_BIT | LIGHT_REG_TIMING;
		cmd[1] = LIGHT_INTEGRATIONTIME_13MS ;
		a2=i2c2.write(LIGHT_I2C_ADRESS, cmd, 2);
		 PRINTF("Set Timing Status %d\n",a2);


		 //  HAL_I2C::writeRead(const uint8_t addr, const uint8_t* txBuf, uint32_t txBufSize, uint8_t* rxBuf, uint32_t rxBufSize) {


		i2c2.writeRead(LIGHT_I2C_ADRESS,&cmd[0],1,&readback[0],1);
		initcalled = true;
	}
};main_light main_light("light");



