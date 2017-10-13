/*
 * recievingTelecommands.cpp
 *
 *  Created on: Nov 15, 2016
 *      Author: root
 */

#include<stdio.h>
#include"rodos.h"
#include "topics.h"
#include "math.h"
#include "../header/SatTopics.h"

/*===============Variables============================ */
state mission_mode_receiver;
//joystick
float joystick;
float tmp_joystick;
state tmp_docking;
#include "../header/main_telemetry.h"

velocity_rotation_internal asdfafs;
velocity_angle_internal asdfasdfasdfasdf;


class ThermalKnifeTC : public SubscriberReceiver<tc_struct_thermal_knife>{
public:
	ThermalKnifeTC () : SubscriberReceiver<tc_struct_thermal_knife>(tc_thermal_knife, "thermal knife command received"){}
	void init(){

	}
	void put(tc_struct_thermal_knife &data_thermal_knife){
		tc_struct_thermal_knife* command = (tc_struct_thermal_knife*)&data_thermal_knife;
		if(data_thermal_knife.value == 1){
			mission_mode_receiver = OPENPANEL;
			mission_mode_telecommand.publish(mission_mode_receiver);
		}
		mission_mode_telecommand.publish(mission_mode_receiver);
	}
}ThermalKnifeTC;

class JoystickTC : public SubscriberReceiver<joystick_values>{
public:
	JoystickTC () : SubscriberReceiver<joystick_values>(tc_joystick){}
	void init(){

	}
	void put(joystick_values &data_joystick){
		joystick_values* command_joystick = (joystick_values*)&data_joystick;
		tmp_joystick = command_joystick->value;
		joystick_topic.publish(tmp_joystick);
	}
}JoystickTC;

class DockingTC : public SubscriberReceiver<docking_go>{
public:
	DockingTC () : SubscriberReceiver<docking_go>(tc_docking){}
	void init(){

	}
	void put(docking_go &data_docking){
		docking_go* command_dock = (docking_go*)&data_docking;
		if(command_dock->go == 1){
			tmp_docking = EXTEND;
			mission_mode_telecommand.publish(tmp_docking);
		}else if(command_dock->go == 0){

		}
	}
}DockingTC;

class VelocityMode : public SubscriberReceiver<velocity_rotation>{
public:
	VelocityMode () : SubscriberReceiver<velocity_rotation>(tc_velocity_rotation, "bla"){}
	void init(){

	}
	void put(velocity_rotation &data_joystick){
		velocity_rotation* command_velocity = (velocity_rotation*)&data_joystick;
		if(command_velocity->value != 0.0){
			asdfafs.value = command_velocity->value;
			velocity_mode_topic.publish(asdfafs);
		}else{
			asdfafs.value = 0;
			velocity_mode_topic.publish(asdfafs);
		}
	}
}VelocityMode;

class PositionMode : public SubscriberReceiver<angle_rotation>{
public:
	PositionMode () : SubscriberReceiver<angle_rotation>(tc_angle_rotation, "bla"){}
	void init(){

	}
	void put(angle_rotation &data_angle){
		angle_rotation* command_angle = (angle_rotation*)&data_angle;
		if(command_angle->value != 0.0){
			asdfasdfasdfasdf.value = command_angle->value;
			angle_mode_topic.publish(asdfasdfasdfasdf);
		}else{
			asdfafs.value = 0;
			angle_mode_topic.publish(asdfasdfasdfasdf);
		}
	}
}PositionMode;
