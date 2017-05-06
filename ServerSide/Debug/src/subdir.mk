################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ChatRoom.cpp \
../src/Dispatcher.cpp \
../src/LoginAndSignUp.cpp \
../src/Server.cpp \
../src/User.cpp 

OBJS += \
./src/ChatRoom.o \
./src/Dispatcher.o \
./src/LoginAndSignUp.o \
./src/Server.o \
./src/User.o 

CPP_DEPS += \
./src/ChatRoom.d \
./src/Dispatcher.d \
./src/LoginAndSignUp.d \
./src/Server.d \
./src/User.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/user/Desktop/netow/msockets/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


