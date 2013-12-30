################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../HTTP/HTTPRequest.cpp \
../HTTP/HTTPResponse.cpp \
../HTTP/HTTPUtils.cpp 

OBJS += \
./HTTP/HTTPRequest.o \
./HTTP/HTTPResponse.o \
./HTTP/HTTPUtils.o 

CPP_DEPS += \
./HTTP/HTTPRequest.d \
./HTTP/HTTPResponse.d \
./HTTP/HTTPUtils.d 


# Each subdirectory must supply rules for building sources it contributes
HTTP/%.o: ../HTTP/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/boost/include -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


