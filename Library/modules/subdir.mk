################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../modules/mod_source_example.o 

CPP_SRCS += \
../modules/http_module.cpp \
../modules/mod_sink_http.cpp \
../modules/mod_source_example.cpp 

OBJS += \
./modules/http_module.o \
./modules/mod_sink_http.o \
./modules/mod_source_example.o 

CPP_DEPS += \
./modules/http_module.d \
./modules/mod_sink_http.d \
./modules/mod_source_example.d 


# Each subdirectory must supply rules for building sources it contributes
modules/%.o: ../modules/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


