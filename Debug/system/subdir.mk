################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../system/SynchronisedQueue.cpp \
../system/SystemThread.cpp \
../system/SystemTimer.cpp \
../system/TCPClientSocket.cpp \
../system/TCPServerSocket.cpp \
../system/TaskThread.cpp 

OBJS += \
./system/SynchronisedQueue.o \
./system/SystemThread.o \
./system/SystemTimer.o \
./system/TCPClientSocket.o \
./system/TCPServerSocket.o \
./system/TaskThread.o 

CPP_DEPS += \
./system/SynchronisedQueue.d \
./system/SystemThread.d \
./system/SystemTimer.d \
./system/TCPClientSocket.d \
./system/TCPServerSocket.d \
./system/TaskThread.d 


# Each subdirectory must supply rules for building sources it contributes
system/%.o: ../system/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/boost/include -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


