# SpaceMaster-FloatSat-2017
Code repository for the FloatSat project during the first semester of the SpaceMaster program (winter 2016/2017) in Würzburg.

![Floatsat](https://github.com/Oleffa/SpaceMaster-FloatSat-2017/blob/master/Pictures/P1030937.JPG)

## Description
The task was to design, build and program a so called FloatSat from scratch. A FloatSat is based on a hemispherical air suspension that simulates an environment similar to space, without friction that influences the spacecraft. The mission of the satellite is to deploy by spinning the satellite and finding the sun using a light sensor, then using a heatblade and cutting nylon wires in order to open the spring powered solar panel mechanism and charge the batteries. Furthermore the satellite is able to connect to a groundstation using WiFi and sending/receiving telemetry/telecomands using UART or SPI. In order to observe the satellite it also sends a videostream to the ground station using WiFi. Aditionally to these basic tasks the satellites mission was to perform a docking maneuver with satellites developped by other groups. For this we had to collaborate and agree on a common docking interface and telemetry/telecommand format to achieve compatibility between the different satellites.

## The Structure
The structure of the satelite was designed as 3D-model to fit inside of the hemisphere while having a stable and low center of gravity to keep the spacecraft in balance. Pictures of the structure can be found in the Pictures folder.
The spacecraft included:
  - Structure
  - STM32F4DISCOVERY Development Board with an extension board developed by the University of Würzburg (including IMU, Gyroscope and Magnetometer)
  - Solar Panels
  - Heatblade
  - Rechargeable Lithium Iron Phosphate Battery
  - Power Sensor (I2C)
  - Luminosity Sensor (I2C)
  - Motors (PWM + GPIO)
  - Camera (I2C + DCMI)
  - WiFi (UART/SPI)
  - Raspberry Pi 3 + Camera

## The Software
The software was written for the STM32F4DISCOVERY Development Board using C++ and RODOS.

### RODOS
The RODOS (Realtime Onboard Dependable Operating System, http://www.montenegros.de/sergio/rodos/index.html) is a real-time kernel and middleware providing an integrated object-oriented framework interface to multitaskin resources management and communication infrastructure. It provides high fault tolerance and simple hardware abstraction. The middleware carries out transparent communication between applications and computing nodes. The message exchange is asynchronous using the publisher/subscriber protocol. Around this middleware the user can implement applications that can communicate with external units through gateways to the network.

### The Attitude control

/Code/src/imu.cpp
/Code/src/MadgwickFilter.cpp
/Code/src/main_control.cpp

The satellite uses data from the Gyroscope, Accelerometer and Magnetometer in order to determine its orientation in 3D-space. The first step when deploying the satellite is therefore a calibration of the IMU.
The raw values read form the registers of the IMU then are filtered using an implementation of the madgwick filter.
The control system of the satellite is a PID controller using the filtered IMU data. It provides methods for moving with a set angular velocity or turn for a certain angle around its vertical axis by controlling the motor speed of the PWM motor. Turning is achieved by creating momentum using a flywheel attached to the motor. If the satellite is idling, it tries to hold its current position.

### Telecommands/Telemetry

/Code/wifi/*

The communication is using WiFi and UART. The data transfer is using the publisher/subscriber method. The topics used are declared in /Code/wifi/toppics.h and used to send/receive data by the recievingTelecommands.cpp and sendingTelemetry.cpp.
WiFi and communication settings can be set using the wifi_interface/wf121.cpp file.
