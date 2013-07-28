################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../task/TaskThread.cpp \
../task/TaskThreadPool.cpp 

OBJS += \
./task/TaskThread.o \
./task/TaskThreadPool.o 

CPP_DEPS += \
./task/TaskThread.d \
./task/TaskThreadPool.d 


# Each subdirectory must supply rules for building sources it contributes
task/%.o: ../task/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/boost/include -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


