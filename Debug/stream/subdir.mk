################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../stream/Stream.cpp \
../stream/StreamClient.cpp \
../stream/StreamFactory.cpp 

OBJS += \
./stream/Stream.o \
./stream/StreamClient.o \
./stream/StreamFactory.o 

CPP_DEPS += \
./stream/Stream.d \
./stream/StreamClient.d \
./stream/StreamFactory.d 


# Each subdirectory must supply rules for building sources it contributes
stream/%.o: ../stream/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/boost/include -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


