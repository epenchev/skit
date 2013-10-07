################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../server/HTTPServer.cpp \
../server/PluginManager.cpp \
../server/TCPConnection.cpp 

OBJS += \
./server/HTTPServer.o \
./server/PluginManager.o \
./server/TCPConnection.o 

CPP_DEPS += \
./server/HTTPServer.d \
./server/PluginManager.d \
./server/TCPConnection.d 


# Each subdirectory must supply rules for building sources it contributes
server/%.o: ../server/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


