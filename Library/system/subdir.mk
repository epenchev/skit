################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../system/Buffer.cpp \
../system/Daemon.cpp \
../system/SynchronisedQueue.cpp \
../system/SystemLib.cpp \
../system/SystemThread.cpp \
../system/SystemTimer.cpp \
../system/TCPAcceptor.cpp \
../system/TCPSocket.cpp \
../system/TaskThread.cpp 

OBJS += \
./system/Buffer.o \
./system/Daemon.o \
./system/SynchronisedQueue.o \
./system/SystemLib.o \
./system/SystemThread.o \
./system/SystemTimer.o \
./system/TCPAcceptor.o \
./system/TCPSocket.o \
./system/TaskThread.o 

CPP_DEPS += \
./system/Buffer.d \
./system/Daemon.d \
./system/SynchronisedQueue.d \
./system/SystemLib.d \
./system/SystemThread.d \
./system/SystemTimer.d \
./system/TCPAcceptor.d \
./system/TCPSocket.d \
./system/TaskThread.d 


# Each subdirectory must supply rules for building sources it contributes
system/%.o: ../system/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


