/*
 * voltagesensor.cpp
 *
 *  Created on: 28.01.2017
 *      Author: Oliver Struckmeier
 */

#include <rodos.h>
#include <inttypes.h>
#include "hal.h"
#include "../header/voltagesensor.h"
#include "../header/SatTopics.h"

bool init_voltagesensor = false;

//registers
#define CONFIG_R		0x00	// configuration register
#define V_SHUNT_R		0x01	// differential shunt voltage
#define V_BUS_R			0x02	// bus voltage (wrt to system/chip GND)
#define P_BUS_R			0x03	// system power draw (= V_BUS * I_SHUNT)
#define I_SHUNT_R		0x04	// shunt current
#define CAL_R			0x05	// calibration register

#define INA219_REG_SHUNTVOLTAGE                (0x01)
#define INA219_REG_BUSVOLTAGE                  (0x02)
#define INA219_REG_POWER                       (0x03)
#define INA219_REG_CURRENT                     (0x04)
#define INA219_REG_CALIBRATION					(0x05)

#define INA219_CONFIG_GAIN_8_320MV             (0x1800)
#define INA219_CONFIG_BVOLTAGERANGE_32V        (0x2000)
#define INA219_CONFIG_BADCRES_12BIT            (0x0400)
#define INA219_CONFIG_SADCRES_12BIT_1S_532US   (0x0018)
#define INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS (0x0007)

//=======Solar panel=======
#define SolarVoltageADC ADC_CH_001 //PA1
#define SolarCurrentADC ADC_CH_002 //PA2
HAL_ADC SolarV(ADC_IDX1);
HAL_ADC SolarC(ADC_IDX2);
float current = 0.0;
float voltage = 0.0;

//=======Battery stuff======
#define ADDRESS 0b01000000
HAL_I2C HAL_Voltage(I2C_IDX2);


voltagesensor::voltagesensor() : Thread(name){
}

void voltagesensor::init(){
	suspendCallerUntil(NOW()+(1000*MILLISECONDS)/100);
}

void voltagesensor::run(){
	if(!init_voltagesensor){
		myInit();
		init_voltagesensor = true;
	}
	while(1){
		//=========================Solar panel stuff=========================
		//Current and voltage
		voltage = (SolarV.read(SolarVoltageADC)/4095.0)*3300*1.67; ///4095)*3000)/0.242;
		current = ((SolarC.read(SolarCurrentADC)/4095.0)*3300)/10.0;

		//Publish voltage and current of the solar panel
		panel_current_topic.publish(current);
		panel_voltage_topic.publish(voltage);


		//=========================Battery stuff===========================
		//Voltage
		uint8_t read[2];
		uint8_t txBuf[1];
		txBuf[0] = V_BUS_R;
		HAL_Voltage.writeRead(0x40, txBuf, 1, read, 2);
		uint16_t batteryVoltage = (uint16_t) ((read[0] << 8) | read[1]);
		batteryVoltage = (int16_t)((batteryVoltage >> 3) * 4);
		float IN_Voltage = batteryVoltage;
		//Publish voltage
		battery_voltage_topic.publish(IN_Voltage);
		//Current
		uint16_t value = 0x1000;// 1A
		uint8_t tx[3];
		tx[0] = INA219_REG_CALIBRATION;
		tx[1] = (value >> 8) & 0xFF;
		tx[2] = value & 0xFF;
		HAL_Voltage.write(0x40, tx, 3);
		memset(txBuf, 0, sizeof(txBuf));
		txBuf[0] = I_SHUNT_R;
		HAL_Voltage.writeRead(0x40,txBuf,1,read,2);
		uint16_t batteryCurrent = (uint16_t) ((read[1] << 8) | read[0]);
		float batteryCurrent_ = batteryCurrent / 100.0;
		//Publish the value
		battery_current_topic.publish(batteryCurrent_);

		suspendCallerUntil(NOW()+100*MILLISECONDS);
	}
}

void voltagesensor::myInit(){
	//Init
	SolarV.init(SolarVoltageADC);
	SolarV.config(ADC_PARAMETER_RESOLUTION, 12);
	SolarC.init(SolarCurrentADC);
	SolarC.config(ADC_PARAMETER_RESOLUTION, 12);
	//Battery init
	HAL_Voltage.init(400000);
	uint16_t config = 0;
	config = INA219_CONFIG_BVOLTAGERANGE_32V |
	INA219_CONFIG_GAIN_8_320MV |
	INA219_CONFIG_BADCRES_12BIT |
	INA219_CONFIG_SADCRES_12BIT_1S_532US |
	INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
	uint8_t txBuf[3];
	txBuf[0] = CONFIG_R;
	txBuf[1] = (config >> 8) & 0xFF;
	txBuf[2] = config & 0xFF;
	HAL_Voltage.write(0x40, txBuf, 3);
	init_voltagesensor = true;
}



