################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../http/HTTPRequest.cpp \
../http/HTTPResponse.cpp \
../http/HTTPUtils.cpp 

OBJS += \
./http/HTTPRequest.o \
./http/HTTPResponse.o \
./http/HTTPUtils.o 

CPP_DEPS += \
./http/HTTPRequest.d \
./http/HTTPResponse.d \
./http/HTTPUtils.d 


# Each subdirectory must supply rules for building sources it contributes
http/%.o: ../http/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/boost/include -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


