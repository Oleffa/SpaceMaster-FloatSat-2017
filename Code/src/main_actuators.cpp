/*
 * main_actuators.cpp
 *
 * Created on: 30.11.2016
 * Author: Oliver Struckmeier
 *
 */

#include "rodos.h"
#include <stdio.h>
#include <stdlib.h>
#include "hal.h"
#include "math.h"
#include "../header/global_variables.h"
#include "../header/SatTopics.h"
#include "../header/servo.h"

/*================Misc. variables=============================================*/
int f_actuators = 200; 				//refresh rate of the thread in 1/second
long time_per_tick = (1000*MILLISECONDS)/f_actuators; //time passing between one cycle of the run loop
state mission_mode_actuators;
state last_mission_mode;
/*================PWM STUFF====================================================*/
HAL_GPIO HBRIDGE_EN(GPIO_066);		//declare HAL_GPIO for GPIO_066= PE2 (HBRIDGE Power Enable pin)
/*=============================HBridge A - motor============================*/
bool clockwise;
bool initialized = false;

GPIO_PIN HBRIDGE_A_INA_CLOCKWISE = GPIO_036;
GPIO_PIN HBRIDGE_A_INB_CLOCKWISE = GPIO_017;

HAL_GPIO HBRIDGE_A_INA(HBRIDGE_A_INA_CLOCKWISE);	//declare HAL_GPIO for GPIO_017 = PB1 (HBRIDGE-B INA pin)
HAL_GPIO HBRIDGE_A_INB(HBRIDGE_A_INB_CLOCKWISE);	//declare HAL_GPIO for GPIO_017 = PB1 (HBRIDGE-B INA pin)
HAL_PWM Motor(PWM_IDX12);			//declare HAL_PWM for PWM_IDX12 = TIM4-CH1 (HBRIDGE-A), please refer to hal_pwm.h for correct PWM mapping

long dc = 0; 							//duty cycle of the motor (0-1000)
float temp_dc1;

/*============================ HBridge B - Heatblade==================================*/
HAL_GPIO HBRIDGE_B_INA(GPIO_016);
HAL_GPIO HBRIDGE_B_INB(GPIO_071);
HAL_PWM Heatblade(PWM_IDX13);
long hb_activation_time = 0;		//time the heatblade was active
long hb_intensity = 950;			//power of the heatblde (0-1000)
bool hb_active = false;
bool heating_done = false;
/*===========================HBridge C - Docking Electro Magnets=======================*/
HAL_GPIO HBRIDGE_C_INA(GPIO_072);
HAL_GPIO HBRIDGE_C_INB(GPIO_074);
HAL_PWM DockingMagnets(PWM_IDX14);
/*===========================HBridge D - DC Motor===========================================*/
HAL_GPIO HBRIDGE_D_INA(GPIO_076);
HAL_GPIO HBRIDGE_D_INB(GPIO_079);
HAL_PWM DC_Motor(PWM_IDX15);
/*================================Subscriber stuff======================================*/
//subscriber to duty cycle
static CommBuffer<long> dc_buffer;
static CommBuffer<float> joystick_buffer;
static CommBuffer<state> mission_mode_buffer;
static Subscriber sub_dc(duty_cycle, dc_buffer, "motor duty cycle");
static Subscriber sub_joystick(joystick_topic, joystick_buffer, "motor duty cycle");
static Subscriber sub_mission_mode(mission_mode, mission_mode_buffer, "mission mode");

/*=====================================Servo Stuff================================*/
SERVO svo(&HBRIDGE_D_INA, &HBRIDGE_D_INB, &DC_Motor);
int servoPower;
long timer;

class main_actuators: public Thread {

public:

	main_actuators(const char* name) : Thread(name) {
	}

	void init() {

	}

	void run() {
		while (1) {
			if(initialized == false){
				init2();
				initialized = true;
			}
			//get the latest mission mode
			mission_mode_buffer.get(mission_mode_actuators);
			if(mission_mode_actuators!=last_mission_mode){
				HBRIDGE_EN.setPins(0);
				swtichHBRIDGE(1,false);
				swtichHBRIDGE(2,false);
				swtichHBRIDGE(3,false);
				swtichHBRIDGE(4,false);
			}else{
				last_mission_mode = mission_mode_actuators;
			}
			//PRINTF("mission state: %d\n", mission_mode_actuators);
			switch(mission_mode_actuators){
			case CALIBRATING:
				break;
			case IDLE:
				//do motor stuff
				motor();
				break;
			case FIND_SUN:
				motor();
				break;
			case TURN_FOR:
				motor();
				break;
			case TURN_WITH:
				motor();
				break;
			case OPENPANEL:
				if(!heating_done){ //if the heating was not started yet do it
					hb_active = true;	//turn on heatblade
				}
				//heat up heatblade
				motor();
				heatBlade();
				break;
			case EXTEND:
				motor();
				armStuff(true, servoPower);
				break;
			case RETRACT:
				motor();
				armStuff(false, servoPower);
				break;
			case DOCKING:
				//motor();
				dockingMagnets();
				break;
			case JOYSTICKMODE:
				motor();
				break;
			case EMERGENCY:
				swtichHBRIDGE(1, false);
				swtichHBRIDGE(2, false);
				swtichHBRIDGE(3, false);
				swtichHBRIDGE(4, false);
				break;
			default:
				break;
			}
			//pause the Thread for a certain time
            suspendCallerUntil(NOW()+time_per_tick);
		}
	}
	/*
	 * method to turn the hbridges on and off because of the problem that the hbridges always give full power. so we have to reset the pins after using them
	 */
	void swtichHBRIDGE(int x, bool on){
		int pinState1 = 0;
		int pinState2 = 0;
		if(on){
			pinState1 = 1;
			pinState2 = 0;
		}else{
			pinState1 = 0;
			pinState2 = 0;
		}
		switch(x){
		case 1:
			HBRIDGE_A_INA.setPins(pinState1);
			HBRIDGE_A_INB.setPins(pinState2);
			break;
		case 2:
			HBRIDGE_B_INA.setPins(pinState1);
			HBRIDGE_B_INB.setPins(pinState2);
			break;
		case 3:
			HBRIDGE_C_INA.setPins(pinState1);
			HBRIDGE_C_INB.setPins(pinState2);
			break;
		case 4:
			if(on){
				svo.resetPins(true);
			}else{
				svo.resetPins(false);
			}
			break;
		}
		HBRIDGE_EN.setPins(1);
	}
	/*
	 * docking magnet
	 */
	void dockingMagnets(){
		/*
		swtichHBRIDGE(1,false);
		swtichHBRIDGE(2,false);
		swtichHBRIDGE(3,true);
		swtichHBRIDGE(4,false);
		*/
		HBRIDGE_C_INA.setPins(1);
		HBRIDGE_C_INB.setPins(1);
		DockingMagnets.write(1);
	}
	/*
	 * docking arm
	 */
	void armStuff(bool dir, int dc_servo){
		swtichHBRIDGE(1,false);
		swtichHBRIDGE(2,false);
		swtichHBRIDGE(3,false);
		swtichHBRIDGE(4,true);
		//PRINTF("SERVO: %d\n", dc_servo);
		svo.setDir(dir);
		svo.write(dc_servo);
	}
	/*
	 * heatblade
	 */
	void heatBlade(){
			swtichHBRIDGE(1,false);
			swtichHBRIDGE(2,true);
			swtichHBRIDGE(3,false);
			swtichHBRIDGE(4,false);

		if(hb_active){
			if(hb_activation_time < heatblade_duration){
				Heatblade.write(hb_intensity);
				hb_activation_time += time_per_tick/MILLISECONDS;
				if(outputPRINTFS_main_actuators == true)PRINTF("hb_running_for: %ld\n",hb_activation_time);
			}else{
				hb_activation_time = 0;
				hb_active = false;
				heating_done = true;
				if(outputPRINTFS_main_actuators == true)PRINTF("Heatblade Done\n");
			}
		}
	}
	/*
	 * motor
	 */
	void motor(){
		if(mission_mode_actuators == DOCKING){
			swtichHBRIDGE(1,true);
			swtichHBRIDGE(2,false);
			swtichHBRIDGE(3,true);
			swtichHBRIDGE(4,false);
		}else if(mission_mode_actuators == EXTEND ||mission_mode_actuators == RETRACT){
			swtichHBRIDGE(1,true);
			swtichHBRIDGE(2,false);
			swtichHBRIDGE(3,false);
			swtichHBRIDGE(4,true);
		}else if(mission_mode_actuators == OPENPANEL){
			swtichHBRIDGE(1,true);
			swtichHBRIDGE(2,true);
			swtichHBRIDGE(3,false);
			swtichHBRIDGE(4,false);
		}else{
			swtichHBRIDGE(1,true);
			swtichHBRIDGE(2,false);
			swtichHBRIDGE(3,false);
			swtichHBRIDGE(4,false);
		}
		//see if the manual mode (joystick) is used

			if(mission_mode_actuators == JOYSTICKMODE){
				joystick_buffer.get(temp_dc1);
				dc = (long)temp_dc1;
			}else{
				dc_buffer.get(dc); //read duty cycle from topic
			}

		//PRINTF("dc: %ld \n", dc);
		//cap the duty cycle
		if(dc > 1000){
			dc = 1000;
		}
		if(dc < -1000){
			dc = -1000;
		}
		//check if movement is counter/-clockwise (depending if duty cycle is + or -)
		if(dc < 0 ){
			clockwise = true;
		}else{
			clockwise = false;
		}
		dc = abs(dc);
		//set pins depending on turn direction
		if(clockwise){
			HBRIDGE_A_INA.setPins(1);
			HBRIDGE_A_INB.setPins(0);
			//PRINTF("Clockwise %ld\n", dc);
		}else{
			HBRIDGE_A_INA.setPins(0);
			HBRIDGE_A_INB.setPins(1);
			//PRINTF("Counter Clockwise %ld\n", dc);
		}
		//write duty cycle to the motor
		int temp_dc = (int) dc;
		//PRINTF("MOTOR: %d\n", temp_dc);
		Motor.write(temp_dc);
	}

	void init2(){
		suspendCallerUntil(NOW()+(1000*MILLISECONDS)/100);
		//enable pin
		HBRIDGE_EN.init(true, 1, 1);
		//Init motor
		Motor.init(5000,1000); //init the motor with a max power of 1000
		HBRIDGE_A_INA.init(true, 1, 1);
		HBRIDGE_A_INB.init(true, 1, 0);
		//Init heatblade
		Heatblade.init(5000,1000);
		HBRIDGE_B_INA.init(true, 1, 0);
		HBRIDGE_B_INB.init(true, 1, 1);
		//Init Electro Magnets
		DockingMagnets.init(5000,1);
		HBRIDGE_C_INA.init(true, 1, 0);
		HBRIDGE_C_INB.init(true, 1, 0);
		//Write 0 in the beginning
		Motor.write(0);
		Heatblade.write(0);
		svo.write(0);

		//init servo
		svo.init();
		servoPower = 400;
		timer = 0;

		clockwise = true; //turn directino of dc motor

		//init mission mode
		mission_mode_buffer.get(mission_mode_actuators);
	}
};main_actuators main_actuators("Actuator Control");

/***********************************************************************/


