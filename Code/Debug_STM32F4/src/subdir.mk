################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Camera.cpp \
../src/MadgwickFilter.cpp \
../src/SatTopics.cpp \
../src/encoder.cpp \
../src/imu.cpp \
../src/main_actuators.cpp \
../src/main_control.cpp \
../src/main_light.cpp \
../src/main_sensors.cpp \
../src/servo.cpp \
../src/voltagesensor.cpp 

OBJS += \
./src/Camera.o \
./src/MadgwickFilter.o \
./src/SatTopics.o \
./src/encoder.o \
./src/imu.o \
./src/main_actuators.o \
./src/main_control.o \
./src/main_light.o \
./src/main_sensors.o \
./src/servo.o \
./src/voltagesensor.o 

CPP_DEPS += \
./src/Camera.d \
./src/MadgwickFilter.d \
./src/SatTopics.d \
./src/encoder.d \
./src/imu.d \
./src/main_actuators.d \
./src/main_control.d \
./src/main_light.d \
./src/main_sensors.d \
./src/servo.d \
./src/voltagesensor.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DUSE_STM32_DISCOVERY -DSTM32F40_41xxx -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\src\bare-metal\stm32f4\STM32F4xx_StdPeriph_Driver\inc" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\src\bare-metal\stm32f4\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\src\bare-metal\stm32f4\CMSIS\Include" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\src\bare-metal\stm32f4\hal" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\src\bare-metal\stm32f4" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\src\bare-metal-generic" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\src\independent\gateway" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\src\independent" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\api" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\src\bare-metal\stm32f4\sdCard" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\api\hal" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\rodos\default_usr_configs" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\support_libs" -I"C:\Users\FloatSat02\Desktop\Eclipse\Eclipse Luna SR1 121 V1\Workspace\support_libs\flash\spiFlash_AT45DBxxx" -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


