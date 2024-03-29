################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GPSSensor.cpp \
../src/GyroscopeSensor.cpp \
../src/SensorFile.cpp \
../src/SpeedometerSensor.cpp \
../src/StateHandlersProcessing.cpp \
../src/main.cpp \
../src/stateHandlers.cpp 

CPP_DEPS += \
./src/GPSSensor.d \
./src/GyroscopeSensor.d \
./src/SensorFile.d \
./src/SpeedometerSensor.d \
./src/StateHandlersProcessing.d \
./src/main.d \
./src/stateHandlers.d 

OBJS += \
./src/GPSSensor.o \
./src/GyroscopeSensor.o \
./src/SensorFile.o \
./src/SpeedometerSensor.o \
./src/StateHandlersProcessing.o \
./src/main.o \
./src/stateHandlers.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/GPSSensor.d ./src/GPSSensor.o ./src/GyroscopeSensor.d ./src/GyroscopeSensor.o ./src/SensorFile.d ./src/SensorFile.o ./src/SpeedometerSensor.d ./src/SpeedometerSensor.o ./src/StateHandlersProcessing.d ./src/StateHandlersProcessing.o ./src/main.d ./src/main.o ./src/stateHandlers.d ./src/stateHandlers.o

.PHONY: clean-src

