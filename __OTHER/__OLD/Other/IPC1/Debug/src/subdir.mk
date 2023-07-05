################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/IPC1.cpp \
../src/MessageQueue.cpp 

CPP_DEPS += \
./src/IPC1.d \
./src/MessageQueue.d 

OBJS += \
./src/IPC1.o \
./src/MessageQueue.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/IPC1.d ./src/IPC1.o ./src/MessageQueue.d ./src/MessageQueue.o

.PHONY: clean-src

