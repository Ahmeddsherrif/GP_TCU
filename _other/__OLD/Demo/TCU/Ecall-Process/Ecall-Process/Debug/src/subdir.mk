################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BitStreamEncoder.cpp \
../src/MSDEncoder.cpp \
../src/MessageQueue.cpp \
../src/Mobile.cpp \
../src/Serial.cpp \
../src/Utilities.cpp \
../src/main.cpp 

CPP_DEPS += \
./src/BitStreamEncoder.d \
./src/MSDEncoder.d \
./src/MessageQueue.d \
./src/Mobile.d \
./src/Serial.d \
./src/Utilities.d \
./src/main.d 

OBJS += \
./src/BitStreamEncoder.o \
./src/MSDEncoder.o \
./src/MessageQueue.o \
./src/Mobile.o \
./src/Serial.o \
./src/Utilities.o \
./src/main.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/BitStreamEncoder.d ./src/BitStreamEncoder.o ./src/MSDEncoder.d ./src/MSDEncoder.o ./src/MessageQueue.d ./src/MessageQueue.o ./src/Mobile.d ./src/Mobile.o ./src/Serial.d ./src/Serial.o ./src/Utilities.d ./src/Utilities.o ./src/main.d ./src/main.o

.PHONY: clean-src
