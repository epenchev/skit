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
../FsSource.cpp \
../HttpClient.cpp \
../HttpConnSink.cpp \
../HttpSink.cpp \
../HttpSource.cpp \
../IOServicePool.cpp \
../MediaHttpClient.cpp \
../MediaHttpServer.cpp \
../Observer.cpp \
../TcpServer.cpp \
../Url.cpp \
../VODMediaServer.cpp 

OBJS += \
./Blitz.o \
./Config.o \
./Daemon.o \
./DataPacket.o \
./DataSource.o \
./FsSource.o \
./HttpClient.o \
./HttpConnSink.o \
./HttpSink.o \
./HttpSource.o \
./IOServicePool.o \
./MediaHttpClient.o \
./MediaHttpServer.o \
./Observer.o \
./TcpServer.o \
./Url.o \
./VODMediaServer.o 

CPP_DEPS += \
./Blitz.d \
./Config.d \
./Daemon.d \
./DataPacket.d \
./DataSource.d \
./FsSource.d \
./HttpClient.d \
./HttpConnSink.d \
./HttpSink.d \
./HttpSource.d \
./IOServicePool.d \
./MediaHttpClient.d \
./MediaHttpServer.d \
./Observer.d \
./TcpServer.d \
./Url.d \
./VODMediaServer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/boost/include -I../include -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


