################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../utils/FileReader.cpp \
../utils/IDGenerator.cpp \
../utils/PropertyMap.cpp \
../utils/XMLReader.cpp 

OBJS += \
./utils/FileReader.o \
./utils/IDGenerator.o \
./utils/PropertyMap.o \
./utils/XMLReader.o 

CPP_DEPS += \
./utils/FileReader.d \
./utils/IDGenerator.d \
./utils/PropertyMap.d \
./utils/XMLReader.d 


# Each subdirectory must supply rules for building sources it contributes
utils/%.o: ../utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


