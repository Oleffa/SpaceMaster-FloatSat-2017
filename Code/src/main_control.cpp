/*
 * main_control.cpp
 *
 * Created on: 30.11.2016
 * Author: Oliver Struckmeier
 *
 * Class that contains the controller and the modes to search for the sun, turn with a speed or to an angle
 *
 */

#include "rodos.h"
#include <stdio.h>
#include "hal.h"
#include "math.h"
#include <stdlib.h>
#include "../header/global_variables.h"
#include "../header/MadgwickFilter.h"
#include "../header/SatTopics.h"
#include "../header/main_sensors.h"
/*
 * for ptrinfts use: if(outputPRINTFS_main_control == true)PRINTF("text\n"); to easily toggle printf outpu
 */

/*==============================Misc. variables============================*/
int f_control = 10; 		//how often the run loop is exectued per second
long dc_control = 0;
float angular_velocity;
state mission_mode_control = CALIBRATING;
float heading_control = 0.0;
int dre = 0;
bool sunFound = false;
/*==============================Publisher stuff===========================*/
static Application duty_cycle_module("motor duty cycle", 1000);
static Application mission_mode_module("satelite state", 1000);
/*==============================Subscriber stuff===========================*/
static CommBuffer<float_3_struct> acc_buffer;
static CommBuffer<float_3_struct> gyr_buffer;
static CommBuffer<float_3_struct> mag_buffer;
static CommBuffer<float> heading_buffer;
static CommBuffer<float> light_buffer;
static CommBuffer<state> mission_mode_buffer;
static CommBuffer<velocity_rotation_internal> velocity_mode_buffer;
static CommBuffer<velocity_angle_internal> angle_mode_buffer;

static Subscriber sub_acc_topic(acc_topic, acc_buffer, "acc values");
static Subscriber sub_gyr_topic(gyr_topic, gyr_buffer, "gyr values");
static Subscriber sub_mag_topic(mag_topic, mag_buffer, "mag values");
static Subscriber sub_heading_topic(sat_heading_topic, heading_buffer, "sat heading angle");
static Subscriber sub_light_topic(light_sensor_topic, light_buffer, "sun intensity");
static Subscriber sub_mission_mode(mission_mode, mission_mode_buffer, "mission mode");
static Subscriber sub_velocity_rotation(velocity_mode_topic, velocity_mode_buffer, "mission mode");
static Subscriber sub_angle_rotation(angle_mode_topic, angle_mode_buffer, "mission mode");
/*==============================Filter Stuff===================================*/
MadgwickFilter filter(0.1,5);
float_3_struct acc_buffer_temp;
float_3_struct gyr_buffer_temp;
float_3_struct mag_buffer_temp;
float attitude_angles_rad[3];
sat_attitude2 att;
/*============================Values TODO=====================================*/
float motor_rpm = 0.0;
//angle control
float desired_a = 0.0;
float kP_a = 14.0;
float kI_a = 1.0;
float kD_a = 0.025;
float PID_a = 0.0;
float integral_a = 0.0;
float E_a = 0.0;
float E_prev_a = 0.0;
float derivative_a = 0.0;
//spc, satellite speed control
float kP_spc = 40.0;
float kI_spc = 2.0;
float kD_spc = 0.05;
float PID_spc = 0.0;
float integral_spc = 0.0;
float E_spc = 0.0;
float E_prev_spc = 0.0;
float derivative_spc = 0.0;
//sun controler
float kP_sun = 12;
float PID_sun = 0.0;
float E_sun = 0.0;
float E_prev_sun = 0.0;
float max_sun_intensity = 8.1;
float cur_sun_intensity = 0.0;

pid_stuff asdfhasdjklfh;
velocity_rotation_internal velocity_buffer;
velocity_angle_internal angle_buffer;

class main_control: public Thread {

public:

	main_control(const char* name) : Thread(name) {
	}

	void init() {
		suspendCallerUntil(NOW()+(1000*MILLISECONDS)/100);
	}

	void run() {

		while (1) {
			mission_mode_buffer.get(mission_mode_control);
			if(mission_mode_control == CALIBRATING){
				PRINTF("");
			}else{
				/*=================Filter and Get Attitude of the Satellite===================*/
				acc_buffer.get(acc_buffer_temp);
				gyr_buffer.get(gyr_buffer_temp);
				mag_buffer.get(mag_buffer_temp);
				heading_buffer.get(heading_control);
				//PRINTF("%f, %f, %f\n", gyr_buffer_temp.value1, gyr_buffer_temp.value2, gyr_buffer_temp.value3);
				angular_velocity = gyr_buffer_temp.value3;
				filter.filterUpdate(acc_buffer_temp.value1, acc_buffer_temp.value2, acc_buffer_temp.value3,
									gyr_buffer_temp.value1, gyr_buffer_temp.value2, gyr_buffer_temp.value3,
									mag_buffer_temp.value1, mag_buffer_temp.value2, -1*mag_buffer_temp.value3);
				filter.getEulerAnglesRad(attitude_angles_rad);
				filter.getEulerAnglesDeg(attitude_angles_rad);
				att.roll = attitude_angles_rad[0];
				att.pitch = attitude_angles_rad[1];
				att.yaw = (heading_control-180)/57.2958;//attitude_angles_rad[2];

				att.raw_pitch = 0;
				att.raw_roll = 0;
				att.raw_yaw = 0;
				att.raw_pitch_velocity = gyr_buffer_temp.value1;
				att.raw_roll_velocity = gyr_buffer_temp.value2;
				att.raw_yaw_velocity = gyr_buffer_temp.value3;
				att.roll_velocity = 0;
				att.yaw_velocity = 0;
				att.pitch_velocity = 0;
				//PRINTF("ang vel: %f \n", angular_velocity);

				if(dre > 3){
					//PRINTF("roll: %f, pitch: %f, yaw: %f, w: %f, heading: %f\n", att.roll, att.pitch, att.yaw, angular_velocity, heading_control);
					PRINTF("raw_pitch: %f raw_roll: %f	raw_yaw: %f\n", att.raw_pitch, att.raw_roll, att.raw_yaw);
					PRINTF("raw_pitch_velocity: %f raw_roll_velocity: %f	raw_yaw_velocity: %f\n", att.raw_pitch_velocity, att.raw_roll_velocity, att.raw_yaw_velocity);
					PRINTF("pitch: %f roll: %f	yaw: %f\n", att.pitch, att.roll, att.yaw);
					PRINTF("pitch_velocity: %f roll_velocity: %f yaw_velocity: %f\n\n", att.pitch_velocity, att.roll_velocity, att.yaw_velocity);
					PRINTF("heading: %f\n",heading_control);
					dre = 0;
				}else{
					dre++;
				}

				/*=================Control stuff motor speed==================================*/

				//========PID Stuff==========================
				if(mission_mode_control == IDLE){
					velocity_mode_buffer.get(velocity_buffer);
					angle_mode_buffer.get(angle_buffer);
					if(velocity_buffer.value != 0.0){
						turnWith(velocity_buffer.value);
					}else if(angle_buffer.value != 0.0){
						turnFor(angle_buffer.value);
					}else{
						turnFor(0);
					}
					//turnFor(0);
				}
				if(mission_mode_control == OPENPANEL){
					turnWith(0);
				}
				if(mission_mode_control == TURN_FOR){
					turnFor(60);
				}
				if(mission_mode_control == TURN_WITH){
					//PRINTF("ang vel2: %f \n", angular_velocity);
					turnWith(25);
				}
				if(mission_mode_control == FIND_SUN){
					findSun();
					if(sunFound==true){
						sun_found_topic.publish(sunFound);
					}
				}
				if(mission_mode_control == DOCKING){
					turnFor(0);
				}
				if(mission_mode_control == EXTEND || mission_mode_control == RETRACT){
					turnFor(0);
				}
				//PRINTF("ang vel4: %f \n", angular_velocity);

				/*=================Publisher stuff============================================*/
				//Publish motor duty cycle
				duty_cycle.publish(dc_control);
				//Publish sat attitude
				sat_attitude.publish(att);
				//publish pid values turn with controler
				if(mission_mode_control == TURN_WITH){
					asdfhasdjklfh.controller = 2;
					asdfhasdjklfh.p = kP_spc;
					asdfhasdjklfh.i = kI_spc;
					asdfhasdjklfh.d = kD_spc;
					pid_values_2.publish(asdfhasdjklfh);
				}
				//publish pid values turn to controler
				if(mission_mode_control == TURN_FOR){
					asdfhasdjklfh.controller = 1;
					asdfhasdjklfh.p = kP_a;
					asdfhasdjklfh.i = kI_a;
					asdfhasdjklfh.d = kD_a;
					pid_values_1.publish(asdfhasdjklfh);
				}
				//pause the Thread for a certain time
				suspendCallerUntil(NOW()+(1000*MILLISECONDS)/f_control);
			}
		}
	}
	void turnWith(float input_speed){//Satellite Speed Control
		//PRINTF("ang vel3: %f\n", angular_velocity);

		//PRINTF("vel buffer: %f\n", velocity_buffer.value);
		if(velocity_buffer.value != 0.0){
			input_speed = velocity_buffer.value;
			//PRINTF("vel: %ld\n", dc);
		}
		//if(input_speed == 0.0){
			//	PID_spc = 0.0;
		//}else{
			//Error
			E_spc = input_speed - angular_velocity;
			//PRINTF("Deviation Satellite speed control input_speed %f - angular_velocity %f = E_spc %f\n", input_speed, angular_velocity, E_spc);
			//I
			integral_spc = integral_spc + E_spc * ((float)f_control/1000);
			//integral_spc = 0;
			//D
			derivative_spc = (E_spc - E_prev_spc)/((float)f_control/1000);
			//derivative_spc = 0;
			//output [duty_cycle]
			PID_spc = (kP_spc * E_spc) + kI_spc * integral_spc + kD_spc * derivative_spc;
			E_prev_spc = E_spc;

			//Saturation Block
			if(PID_spc > 900){
				PID_spc = 900;
			}else if(PID_spc < -900){
				PID_spc = -900;
			}

		//}
		dc_control = PID_spc;
		//PRINTF("input: %f, PID_spc: %f, E_spc: %f\n",input_speed, PID_spc, E_spc);
		//PRINTF("dc_control: %ld\n",dc_control);
	}

	void turnFor(float angle){ //Angle Control
		if(desired_a == 0.0){
			desired_a = heading_control + angle;
			if(desired_a > 360){
				desired_a = desired_a - 360;
			}
		}
		E_a = desired_a - heading_control; //Deviation from wished motor speed

		if(abs(E_a) > 180){
			if(E_a < 0){
				E_a =  2*180 + E_a;
			}else{
				E_a = E_a - 2*180;
			}
		}

		//PRINTF("E_a: %f,desired_a: %f, heading_control: %f\n", E_a, desired_a, heading_control);
		//I
		integral_a = integral_a + E_a * ((float)f_control/1000);
		//PRINTF("integral_a: %f", integral_a);
		//integral_a = 0;
		//D
		derivative_a = (E_a - E_prev_a)/((float)f_control/1000);
		//derivative_a = 0;
		//PRINTF("derivative_a: %f,test: %f, stuff: %f\n", derivative_a, E_a- E_prev_a, ((float)f_control/1000));
		//output [duty_cycle]
		PID_a = (kP_a * E_a) + kI_a * integral_a + kD_a * derivative_a;
		E_prev_a = E_a;

		//Saturation Block
		if(PID_a > 900){
			PID_a = 900;
		}else if(PID_a < -900){
			PID_a = -900;
		}
		PRINTF("Turn For dc: %f, E_a: %f, heading: %f, desired angle: %f, integral: %f\n",PID_a, E_a, heading_control, desired_a, integral_a);
		dc_control = PID_a;
	}
	void findSun(){
		light_buffer.get(cur_sun_intensity);
		E_sun = max_sun_intensity - cur_sun_intensity;
		PID_sun = (kP_sun * E_sun);
		//PRINTF("PID_sun: %f, E_sun: %f\n", PID_sun, E_sun);
		if(E_sun < 0.05){
			sunFound = true;
			PRINTF("sun found\n");
		}
		if(PID_sun>40){
			PID_sun = 40;
		}
		if(PID_sun<-40){
			PID_sun = -40;
		}
		//PRINTF("E_sun: %f, turnWith: %f\n", E_sun, PID_sun);
		//PRINTF("max: %f / cur: %f\n", maxSun, cur_sun_intensity);
		turnWith(PID_sun);
	}

};main_control main_control("Control");

/***********************************************************************/
