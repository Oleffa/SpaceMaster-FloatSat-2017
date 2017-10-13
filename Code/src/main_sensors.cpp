/*
 * main_attitude.cpp
 *
 * Created on: 30.11.2016
 * Author: Oliver Struckmeier
 *
 * this class is responsible for all the sensors and reads+publishes them to the topics
 * also the main mission mode control runs here. the mission modes are switched and then published to all the other threadds which then act accordingly
 *
 */

#include "rodos.h"
#include "hal.h"
#include "../header/imu.h"
#include "../header/main_sensors.h"
#include "../header/main_actuators.h"
#include "../header/global_variables.h"
//#include "../header/lightsensor.h"
#include "../header/lightSensor_test.h"
#include "../header/SatTopics.h"
#include "../wifi/topics.h"
#include "../header/Camera.h"
#include "../header/encoder.h"

//TEST
#include <inttypes.h> //used to output uint_32
HAL_GPIO led_blue(GPIO_063);
int x = 0;
int t = 0;

/*========================Misc. variables===============================*/
int f_attitude = 100; //refresh rate of the thread in 1 second
long time_per_tick_sensors = (1000*MILLISECONDS)/f_attitude; //time passing between one cycle of the run loop
float temp_light;
bool initialized_sensors=false;
/*========================IMU===========================================*/
HAL_GPIO imu_cs(GPIO_018);				//chip select pin
HAL_GPIO imu_en(GPIO_055);				//power enable pin
HAL_I2C imu_i2c(I2C_IDX2);				//imu i2c
IMU imu(&imu_i2c, &imu_en, &imu_cs);	// create new instance of imu
float acc_val[3];
float gyr_val[3];//imu data buffers
float mag_val[3];
float heading; //heading angle of the satellite
float lightsensor_val; //lightsensor value
/*========================Publisher subscriber stuff====================*/
static CommBuffer<state> mission_mode_tc_buffer;
static CommBuffer<bool> sun_found_buffer;
static Subscriber sub_sun_found(sun_found_topic, sun_found_buffer, "sun found topic subscriber");
static Subscriber sub_mission_mode2(mission_mode_telecommand, mission_mode_tc_buffer, "mission mode");
/*========================State machine=================================*/
state satelliteState;
state oldSatelliteState;
state tcInputState;
/*========================Calibration Variables=========================*/
bool calibrating_acc;
bool calibrating_gyr;
/*========================Open solar panels=============================*/
float solar_panel_timer;
/*========================Light Sensor==================================*/
//LIGHT light;
bool sun_found= false;
light_sensor tempL;
/*========================Servo Stuff===================================*/
float servo_timer;
/*========================cam===========================================*/
Camera c("cam");
/*========================encoder===========================================*/
encoder e;
float encoder_val;

velocity_rotation_internal xx;

class main_sensors: public Thread {
public:

	main_sensors(const char* name) : Thread(name) {
	}

	void init() {
		//init see below (own init method to make sure its called)
	}

	void run() {
		//mission_mode.publish(satelliteState);

		while (1) {
			//debug_led2.setPins(1);
			//make sure it is initialized
			if(initialized_sensors==false){
				myinit();
				initialized_sensors = true;
			}

			if(satelliteState != CALIBRATING){
				//PRINTF("hi1 ");
				mission_mode_tc_buffer.get(tcInputState);
				//PRINTF("read state: %d, current state: %d\n", tcInputState, satelliteState);
				if(tcInputState != oldSatelliteState){
					//TODO manual_mode = true;
					//PRINTF("change mode\n");
					switch(tcInputState){
					case JOYSTICKMODE:
						PRINTF("joystick\n");
						satelliteState = JOYSTICKMODE;
						break;
					case RETRACT:
						PRINTF("Rretract\n");
						satelliteState = RETRACT;
					break;
					case EXTEND:
						PRINTF("extend\n");
						satelliteState = EXTEND;
					break;
					}
				}
			}
			/*
			 * states in which the satellite is
			 */
			sun_found_buffer.get(sun_found);
			//led_blue.setPins(~led_blue.readPins());
			switch(satelliteState){
			case CALIBRATING:
				PRINTF("Calibrating %d / 1000\n", t);
				if(calibrating_acc){
					if(imu.calibrateAccelerometer() == true){
						calibrating_acc = false;
						//if(outputPRINTFS_main_sensors == true)PRINTF("===Calibrating Acc Done==\n");
					}else{
						//if(outputPRINTFS_main_sensors == true)PRINTF("Calibrating Acc");
					}
				}

				if(calibrating_gyr){
					if(imu.calibrateGyro() == true){
						calibrating_gyr = false;
						//if(outputPRINTFS_main_sensors == true)PRINTF("===Calibrating Gyro Done==\n");
					}else{
						//if(outputPRINTFS_main_sensors == true)PRINTF("Calibrating Gyro %d / 1000\n", t);
					}
				}
				if(!calibrating_acc && !calibrating_gyr){
					satelliteState = TURN_WITH;//OPENPANEL;
					mission_mode.publish(satelliteState);
					PRINTF("Calibration Done, waiting 5 s\n");

				}
				t = t + 1;
				break;
			case FIND_SUN:
				readSensors(true, true, true, true);
				if(sun_found){
					satelliteState = OPENPANEL;
					PRINTF("foundsun\n");
				}else{
					//PRINTF("findsun\n");
				}
				break;
			case OPENPANEL:
				readSensors(true, true, true, true);
				if(solar_panel_timer < panel_opening_time){
					solar_panel_timer += time_per_tick_sensors/MILLISECONDS;
					PRINTF("Panel opening in progress\n");
				}else{
					satelliteState = EXTEND;
				}
				PRINTF("openpanel\n");
				break;
			case IDLE:
				//if calibration is done
				//standard operation mode
				//read all sensors
				//PRINTF("idle\n");
				readSensors(true, true, true, true);
				//satelliteState = EXTEND;
				//satelliteState = DOCKING;
				break;
			case EXTEND:
				readSensors(true, true, true, true);
				if(servo_timer < 1200){
					servo_timer += time_per_tick_sensors/MILLISECONDS;
					PRINTF("timer: %f / 1500\n", servo_timer);
				}else{
					PRINTF("Arm extended\n");
					servo_timer = 0;

					satelliteState = DOCKING;
				}
				break;
			case RETRACT:
				readSensors(true, true, true, true);
				if(servo_timer < 1200){
					servo_timer += time_per_tick_sensors/MILLISECONDS;
					PRINTF("timer: %f / 1500\n", servo_timer);
				}else{
					PRINTF("Arm retracted\n");
					servo_timer = 0;
					satelliteState = TURN_WITH;
				}
				break;
			case TURN_FOR:
				//read all sensors
				readSensors(true, true, true, true);

				break;
			case TURN_WITH:
				readSensors(true, true, true, true);
				//PRINTF("TURN_WITH\n");
				break;
			case TAKEPIC:
				readSensors(true, true, true, true);
				c.takePicture();
				suspendCallerUntil(NOW()+2*MILLISECONDS);
				c.sendImage();
				satelliteState = IDLE;
				break;
			case DOCKING:
				readSensors(true, true, true, true);
				//suspendCallerUntil(NOW() + 10*(1000*MILLISECONDS));
				//PRINTF("docking\n");
				//satelliteState = RETRACT;
				break;
			case JOYSTICKMODE:
				readSensors(true, true, true, true);
				led_blue.setPins(1);
				break;
			case EMERGENCY:
				readSensors(true, true, true, true);
				break;
			default:
				satelliteState = EMERGENCY;
				break;
			}
			//publish the mission state
			mission_mode.publish(satelliteState);
			oldSatelliteState = satelliteState;
			//pause thread
			switch(satelliteState){
			case CALIBRATING:
				suspendCallerUntil(NOW()+(1000*MILLISECONDS)/1000);
				break;
			default:
				suspendCallerUntil(NOW()+(1000*MILLISECONDS)/f_attitude);
				break;
			}
		}
	}

	/*
	 * read all sensors
	 * acc, gyr, mag: true=raw values, false = scaled values
	 */
	void readSensors(bool acc, bool gyr, bool mag, bool lig){
		imu.readAccelerometer(acc_val, false);
		float_3_struct bla;
		bla.value1 = acc_val[0];
		bla.value2 = acc_val[1];
		bla.value3 = acc_val[2];
		acc_topic.publish(bla);
		imu.readGyro(gyr_val, false);
		bla.value1 = gyr_val[0];
		bla.value2 = gyr_val[1];
		bla.value3 = gyr_val[2];
		gyr_topic.publish(bla);
		imu.readMagnetometer(mag_val, false);
		bla.value1 = mag_val[0];
		bla.value2 = mag_val[1];
		bla.value3 = mag_val[2];
		mag_topic.publish(bla);
		//imu.readEuler(euler, f_attitude);
		//light.readLight(&lightsensor_val);
		/*tempL.value = lightsensor_val;
		tempL.ignore1 = 0;
		tempL.ignore2 = 0;
		tempL.ignore3 = 0;
		if(lightsensor_val != 0){
			//debug_led2.setPins(1);
		}else{
			//debug_led2.setPins(0);
		}
		light_sensor_topic.publish(lightsensor_val);
		PRINTF("ligh %f\n", lightsensor_val);
	*/
		heading = imu.getHeading();
		sat_heading_topic.publish(heading);
		//read encoder
		encoder_val = e.encoder_read((1000*MILLISECONDS)/f_attitude);
		//PRINTF("encoder: %f\n", encoder_val);
	}
	void myinit(){
		//init the imu
		imu.init();
		satelliteState = CALIBRATING;
		oldSatelliteState = satelliteState;
		mission_mode.publish(satelliteState);
		//calibration
		calibrating_acc = true;
		calibrating_gyr = true;
		//init light sensor
		//light.init();
		initialized_sensors = true;
		c.init_camera();
		e.encoder_init();
		led_blue.init(true,1,0);
		xx.value = 00;
		//PRINTF("sensor init done");
	}
};
main_sensors main_sensors("Sensor Input");

/***********************************************************************/
