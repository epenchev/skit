################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Blitz.cpp \
../Config.cpp \
../Daemon.cpp \
../DataPacket.cpp \
../DataSource.cpp \
../HttpClient.cpp \
../HttpSink.cpp \
../HttpSource.cpp \
../MediaHttpClient.cpp \
../MediaHttpServer.cpp \
../Observer.cpp \
../TcpServer.cpp \
../Url.cpp 

OBJS += \
./Blitz.o \
./Config.o \
./Daemon.o \
./DataPacket.o \
./DataSource.o \
./HttpClient.o \
./HttpSink.o \
./HttpSource.o \
./MediaHttpClient.o \
./MediaHttpServer.o \
./Observer.o \
./TcpServer.o \
./Url.o 

CPP_DEPS += \
./Blitz.d \
./Config.d \
./Daemon.d \
./DataPacket.d \
./DataSource.d \
./HttpClient.d \
./HttpSink.d \
./HttpSource.d \
./MediaHttpClient.d \
./MediaHttpServer.d \
./Observer.d \
./TcpServer.d \
./Url.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/boost/include -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


