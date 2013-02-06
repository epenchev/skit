################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Blitz.cpp \
../DataPacket.cpp \
../DataSource.cpp \
../HttpClient.cpp \
../HttpServer.cpp \
../HttpSink.cpp \
../HttpSource.cpp \
../MediaHttpClient.cpp \
../MediaUrl.cpp \
../Observer.cpp \
../TcpServer.cpp 

OBJS += \
./Blitz.o \
./DataPacket.o \
./DataSource.o \
./HttpClient.o \
./HttpServer.o \
./HttpSink.o \
./HttpSource.o \
./MediaHttpClient.o \
./MediaUrl.o \
./Observer.o \
./TcpServer.o 

CPP_DEPS += \
./Blitz.d \
./DataPacket.d \
./DataSource.d \
./HttpClient.d \
./HttpServer.d \
./HttpSink.d \
./HttpSource.d \
./MediaHttpClient.d \
./MediaUrl.d \
./Observer.d \
./TcpServer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/boost/include -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


