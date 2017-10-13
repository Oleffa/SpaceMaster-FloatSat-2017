/*
 * sendingTelemetry.cpp
 *
 *  Created on: Nov 15, 2016
 *      Author: root
 */
#include <stdio.h>
#include "rodos.h"
#include "topics.h"
#include "math.h"
#include "../header/SatTopics.h"

attitude wifi_attitude_conversion;
light_sensor lightData_conversion;
/*=============================random variables===========================*/
float tm_tick = 100;
tm_struct_connection asdfasdjkl;
attitude temp_attitude;
float_3_struct sat_att_tm;
float_3_struct gyr_tm;
float_3_struct mag_tm;
/*==============================Subscriber stuff===========================*/
HAL_GPIO debug_led_orange(GPIO_061);
//attitude=============================================
static CommBuffer<attitude> attitude_buffer;
static CommBuffer<float_3_struct> acc_buffer;
static CommBuffer<float_3_struct> gyr_buffer;
static CommBuffer<float_3_struct> mag_buffer;
static CommBuffer<float> heading_buffer;
static Subscriber sub_attitude(sat_attitude, attitude_buffer, "satellite attitude");
static Subscriber sub_acc_topic(acc_topic, acc_buffer, "acc values");
static Subscriber sub_gyr_topic(gyr_topic, gyr_buffer, "gyr values");
static Subscriber sub_mag_topic(mag_topic, mag_buffer, "mag values");
static Subscriber sub_heading_topic(sat_heading_topic, heading_buffer, "sat heading angle");
//lightsensor=============================================
static CommBuffer<light_sensor> light_sensor_buffer;
static Subscriber sub_light_topic(light_sensor_topic, light_sensor_buffer, "satelite state");
//battery voltage/current
static CommBuffer<float> battery_c_buffer;
static CommBuffer<float> battery_v_buffer;
static Subscriber sub_battery_current(panel_current_topic, battery_c_buffer, "bat current");
static Subscriber sub_batter_voltage(panel_voltage_topic, battery_v_buffer, "bat voltage");
battery_power battery_power_conversion;
float battery_v_conversion;
float battery_c_conversion;
//panel voltage/current=============================================
static CommBuffer<float> panel_c_buffer;
static CommBuffer<float> panel_v_buffer;
static Subscriber sub_panel_current(panel_current_topic, panel_c_buffer, "panel current");
static Subscriber sub_panel_voltage(panel_voltage_topic, panel_v_buffer, "panel voltage");
solar_panel_power panel_power_conversion;
float panel_v_conversion;
float panel_c_conversion;
//image=========================================================================
image image_conversion;
//thermal knife=================================================================
static CommBuffer<state> tm_state_buffer;
static Subscriber sub_mission_mode(mission_mode, tm_state_buffer, "mission mode");
state tm_state;
state thermal_knife_conversion;
tm_struct_thermal_knife tm_heatblade;
//pid values=============================================
static CommBuffer<tm_struct_pid_values> tm_pid_buffer;
static Subscriber sub_tm_pid(pid_values_1, tm_pid_buffer, "pid values");
tm_struct_pid_values pid_conversion;
//motor===================================================
static CommBuffer<long> dc_buffer_sender;
static Subscriber sub_dc_sender(duty_cycle, dc_buffer_sender, "motor duty cycle");
long dc_conversion;
tm_struct_motor dc_conversion_struct;
//mission mode
tm_mode_struct tm_mode_conversion;
/*=====================================Joystick=========================================*/
static CommBuffer<float> joystick_buffer_tm;
static Subscriber sub_joystick(joystick_topic, joystick_buffer_tm, "motor duty cycle");
float tempJoystickDc;
/*==============================================================================*/

class attitudeData:public Thread{
public:
	void init(){
		asdfasdjkl.connection = 1;
		debug_led_orange.init(true, 1, 0);
	}
	void run(void){
		init();
		while(1){
			//get mission mode
			tm_state_buffer.get(tm_state);
			debug_led_orange.setPins(~debug_led_orange.readPins());
			//==========901 attitude
			attitude_buffer.get(temp_attitude);
			tm_wifi_attitude.publish(temp_attitude);

			//==========902 light sensor
			light_sensor_buffer.get(lightData_conversion);
				lightData_conversion.value = 10;
			tm_light_sensor.publish(lightData_conversion);

			//==========903 battery voltage/current
			battery_c_buffer.get(battery_c_conversion);
			battery_v_buffer.get(battery_v_conversion);
			battery_power_conversion.voltage = battery_v_conversion;
			battery_power_conversion.current = battery_c_conversion;
				battery_power_conversion.current = 2;
				battery_power_conversion.voltage = 2;
			tm_battery_power.publish(battery_power_conversion);
			//==========04 panel voltage/current
			panel_c_buffer.get(panel_c_conversion);
			panel_v_buffer.get(panel_v_conversion);
			panel_power_conversion.voltage = panel_v_conversion;
			panel_power_conversion.current = panel_c_conversion;
				panel_power_conversion.current = 2;
				panel_power_conversion.voltage = 2;
			tm_solar_panel_power.publish(panel_power_conversion);
			//==========905 motor
			if(tm_state == JOYSTICKMODE){
				joystick_buffer_tm.get(tempJoystickDc);
				dc_conversion_struct.value = (float)tempJoystickDc;
			}else{
				dc_buffer_sender.get(dc_conversion);
				dc_conversion_struct.value = (float)dc_conversion;
			}
			dc_conversion_struct.value = 250;
			tm_motor.publish(dc_conversion_struct);


			//==========906 image
			//TODO
			//image_conversion.format = 0;
			//static const uint32_t ashasdff = 0;
			//image_conversion.num_bytes = [ashasdff];
			//==========907 thermal knife
			tm_state_buffer.get(thermal_knife_conversion);
			if(thermal_knife_conversion==OPENPANEL){
				tm_heatblade.value = 1;
			}else{
				tm_heatblade.value = 0;
			}

			tm_thermal_knife.publish(tm_heatblade);
			//908 pid values
			tm_pid_buffer.get(pid_conversion);
			pid_conversion.controller = 1;
			pid_conversion.p = 2;
			pid_conversion.i = 3;
			pid_conversion.d = 4;
			tm_pid_values.publish(pid_conversion);
			//==========909 connection led
			tm_connection.publish(asdfasdjkl);
			int aa = asdfasdjkl.connection;

			//==========910 mission mode
			switch(thermal_knife_conversion){
			case 0: //IDLE
				tm_mode_conversion.state = 1;
				break;
			case 1://CAlibrating
				tm_mode_conversion.state = 2;
				break;
			case 2://find sun
				tm_mode_conversion.state = 2;
				break;
			case 3://openpanel
				tm_mode_conversion.state = 2;
				break;
			case 4://emergency
				tm_mode_conversion.state = 1;
				break;
			case 5://turn_with
				tm_mode_conversion.state = 3;
				break;
			case 6://turnfor
				tm_mode_conversion.state = 3;
				break;
			case 7://extend
				tm_mode_conversion.state = 4;
				break;
			case 8://retract
				tm_mode_conversion.state = 4;
				break;
			}
			tm_mode.publish(tm_mode_conversion);
			/*
			PRINTF("roll: %f, pitch: %f, heading: %f, w: %f\n", temp_attitude.roll, temp_attitude.pitch, temp_attitude.yaw);
			PRINTF("raw_pitch: %f raw_roll: %f	raw_yaw: %f\n", temp_attitude.raw_pitch, temp_attitude.raw_roll, temp_attitude.raw_yaw);
			PRINTF("raw_pitch_velocity: %f raw_roll_velocity: %f	raw_yaw_velocity: %f\n", temp_attitude.raw_pitch_velocity, temp_attitude.raw_roll_velocity, temp_attitude.raw_yaw_velocity);
			PRINTF("pitch: %f roll: %f	yaw: %f\n", temp_attitude.pitch, temp_attitude.roll, temp_attitude.yaw);
			PRINTF("pitch_velocity: %f roll_velocity: %f yaw_velocity: %f\n\n", temp_attitude.pitch_velocity, temp_attitude.roll_velocity, temp_attitude.yaw_velocity);
			PRINTF("light: %f     ", lightData_conversion.value);
			PRINTF("joystick duty cycle: %f     ", dc_conversion_struct.value);
			PRINTF("heatblade: %d     ", tm_heatblade.value);
			PRINTF("P: %f, I: %f, D: %f     ",pid_conversion.p, pid_conversion.i, pid_conversion.d);
			PRINTF("connection: %d     ", aa);
			PRINTF("tm_mode: %d\n", tm_mode_conversion);
			*/
			//pause thread
			suspendCallerUntil(NOW()+tm_tick*MILLISECONDS);
		}
	}
}attitudeData;
