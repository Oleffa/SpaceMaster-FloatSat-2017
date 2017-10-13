/*
 * lightSensor.h
 *
 *  Created on: Jan 7, 2015
 *      Author: faisal
 *      Reworked by Oliver Struckmeier to match the floatsat project 4.2.2017
 */

#ifndef LIGHTSENSOR_H_
#define LIGHTSENSOR_H_







#include "hal/hal_i2c.h"
#include "rodos.h"
#include "math.h"
#include <stdio.h>





/* The ADDR pin can be used if you have an i2c address conflict, to change the address.

 Connect it to ground to set the address to 0x29,

 connect it to 3.3V (vcc) to se t the address to 0x49 or leave it floating (unconnected) to use address 0x39 */


//HAL_I2C i2c_interface1(I2C_IDX1);



/*******************************************************************************************************
 ********************************** Light Sensor *******************************************************
 ******************************************************************************************************/
//#define LS_ADR_0 0b0101001 //ADR Selection PIN on Ground
//#define LS_ADR_1 0b1001001 //ADR Selection PIN on VDD
#define LS_ADR_0 0x39   //0x39

#define LS_TIMING_ADR 0x01

#define LS_TIMING_VAL 0x4

//#define LS_TIMING_VAL 0b00000000 //Integration Time 13ms, low gain

#define LS_CONTROL_ADR 0x00
#define LS_CONTROL_VAL 0x03

#define LS_OUT_L1 0x8C
#define LS_OUT_H1 0X8D

#define LS_OUT_L2 0x8E
#define LS_OUT_H2 0X8F

#define LIGHT_REG_DATA0LOW 		0x0C

////////////////////////////////////////////////////
#define LIGHT_I2C_ADRESS 		0x39
#define LIGHT_REG_CONTROL		0x0
#define LIGHT_REG_TIMING		0x01

#define LIGHT_REG_DATA0LOW 		0x0C

#define LIGHT_SENSITIVITY_13MS		5
#define LIGHT_SENSITIVITY_101MS 	36
#define LIGHT_SENSITIVITY_402MS		144
#define LIGHT_INTEGRATIONTIME_13MS      0x00    // 13.7ms
#define LIGHT_INTEGRATIONTIME_101MS     0x01    // 101ms
#define LIGHT_INTEGRATIONTIME_402MS     0x02     // 402ms

#define LIGHT_COMMAND_BIT 		0x80

class LIGHT{
public:
	LIGHT();
  void readLight(float* data);
  void init();
};

#endif /* LIGHTSENSOR_H_ */
