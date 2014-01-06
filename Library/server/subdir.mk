################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../server/HTTPServer.cpp \
../server/IOChannel.cpp \
../server/PluginManager.cpp \
../server/ServerController.cpp \
../server/TCPConnection.cpp 

OBJS += \
./server/HTTPServer.o \
./server/IOChannel.o \
./server/PluginManager.o \
./server/ServerController.o \
./server/TCPConnection.o 

CPP_DEPS += \
./server/HTTPServer.d \
./server/IOChannel.d \
./server/PluginManager.d \
./server/ServerController.d \
./server/TCPConnection.d 


# Each subdirectory must supply rules for building sources it contributes
server/%.o: ../server/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


