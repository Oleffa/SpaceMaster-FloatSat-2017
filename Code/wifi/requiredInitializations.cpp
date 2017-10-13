/*
 * requiredInitializations.cpp
 *
 *  Created on: Nov 15, 2016
 *      Author: root
 */


#include "rodos.h"
#include "hal.h"
#include "stdio.h"
#include "math.h"
#include "topics.h"
#include "../header/SatTopics.h"
HAL_GPIO led_red(GPIO_062);

class GatewayInitiator : public Initiator {


	void init(){

		gw.resetTopicsToForward();
	   //own code
	   gw.addTopicsToForward(&tm_wifi_attitude);
	   gw.addTopicsToForward(&tm_light_sensor);
	   gw.addTopicsToForward(&tm_battery_power);
	   gw.addTopicsToForward(&tm_solar_panel_power);
	   gw.addTopicsToForward(&tm_image);
	   gw.addTopicsToForward(&tm_thermal_knife);
	   gw.addTopicsToForward(&tm_pid_values);
	   gw.addTopicsToForward(&tm_connection);
	   gw.addTopicsToForward(&tm_mode);

	   //Telemetry here
	   gw.addTopicsToForward(&tc_thermal_knife);
	   gw.addTopicsToForward(&tc_pid_values);
	   gw.addTopicsToForward(&tc_reaction_wheel);
	   gw.addTopicsToForward(&tc_joystick);
	   gw.addTopicsToForward(&tc_docking);
	   gw.addTopicsToForward(&tc_angle_rotation);
	   gw.addTopicsToForward(&tc_velocity_rotation);
	   gw.addTopicsToForward(&tc_camera);
	   //gw.addTopicsToForward(&tc_mode_type);
	   //gw.addTopicsToForward(&tc_imu_calibration);
	}
}gatewayInitiator;

class SystemInitialization :public Thread
{
public:

	void init ()
	{
		led_red.init(true, 1, 0);
		led_red.setPins(1);
		//PRINTF("WIFI INIT - hi\n");
	}

	void run() {
		//PRINTF("WIFI RUN-hi\n");
		init();
		/*** Gateway Wifi ********/
		led_red.setPins(1);
		  wf121.init("YETENet","yeteyete");
		  PRINTF("init");
			  // Target IP Address: 192.168.0.102 = 0xC0A80066 (reverse and hex)
			  // Target Port: 2000
		  //192.168.0.1 = 0xC0A80001
		  //192.168.1.221 = 0xC0A801DD
		  //192.168.1.138 = 0xC0A8018A
		 wf121.enableUDPConnection(0xDD01A8C0,2001);
		 led_red.setPins(0);
		/**************************/
		PRINTF("Hi Guys From FloatSat Program");
	}
}systemInitialization;
