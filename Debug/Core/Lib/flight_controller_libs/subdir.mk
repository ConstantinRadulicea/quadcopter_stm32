################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/flight_controller_libs/attitude_controller.c \
../Core/Lib/flight_controller_libs/crc.c \
../Core/Lib/flight_controller_libs/filters.c \
../Core/Lib/flight_controller_libs/flight_control_loop.c \
../Core/Lib/flight_controller_libs/geometry2D.c \
../Core/Lib/flight_controller_libs/geometry3D.c \
../Core/Lib/flight_controller_libs/imu.c \
../Core/Lib/flight_controller_libs/madgwick_filter.c \
../Core/Lib/flight_controller_libs/motor_mixer.c \
../Core/Lib/flight_controller_libs/pid.c \
../Core/Lib/flight_controller_libs/rate_controller.c \
../Core/Lib/flight_controller_libs/rc_control.c 

C_DEPS += \
./Core/Lib/flight_controller_libs/attitude_controller.d \
./Core/Lib/flight_controller_libs/crc.d \
./Core/Lib/flight_controller_libs/filters.d \
./Core/Lib/flight_controller_libs/flight_control_loop.d \
./Core/Lib/flight_controller_libs/geometry2D.d \
./Core/Lib/flight_controller_libs/geometry3D.d \
./Core/Lib/flight_controller_libs/imu.d \
./Core/Lib/flight_controller_libs/madgwick_filter.d \
./Core/Lib/flight_controller_libs/motor_mixer.d \
./Core/Lib/flight_controller_libs/pid.d \
./Core/Lib/flight_controller_libs/rate_controller.d \
./Core/Lib/flight_controller_libs/rc_control.d 

OBJS += \
./Core/Lib/flight_controller_libs/attitude_controller.o \
./Core/Lib/flight_controller_libs/crc.o \
./Core/Lib/flight_controller_libs/filters.o \
./Core/Lib/flight_controller_libs/flight_control_loop.o \
./Core/Lib/flight_controller_libs/geometry2D.o \
./Core/Lib/flight_controller_libs/geometry3D.o \
./Core/Lib/flight_controller_libs/imu.o \
./Core/Lib/flight_controller_libs/madgwick_filter.o \
./Core/Lib/flight_controller_libs/motor_mixer.o \
./Core/Lib/flight_controller_libs/pid.o \
./Core/Lib/flight_controller_libs/rate_controller.o \
./Core/Lib/flight_controller_libs/rc_control.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/flight_controller_libs/%.o Core/Lib/flight_controller_libs/%.su Core/Lib/flight_controller_libs/%.cyclo: ../Core/Lib/flight_controller_libs/%.c Core/Lib/flight_controller_libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Programming/STM32F405RGT6_test/Core/Lib/ringbuffer" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Programming/STM32F405RGT6_test/Core/Lib/flight_controller_libs" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/interface" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-flight_controller_libs

clean-Core-2f-Lib-2f-flight_controller_libs:
	-$(RM) ./Core/Lib/flight_controller_libs/attitude_controller.cyclo ./Core/Lib/flight_controller_libs/attitude_controller.d ./Core/Lib/flight_controller_libs/attitude_controller.o ./Core/Lib/flight_controller_libs/attitude_controller.su ./Core/Lib/flight_controller_libs/crc.cyclo ./Core/Lib/flight_controller_libs/crc.d ./Core/Lib/flight_controller_libs/crc.o ./Core/Lib/flight_controller_libs/crc.su ./Core/Lib/flight_controller_libs/filters.cyclo ./Core/Lib/flight_controller_libs/filters.d ./Core/Lib/flight_controller_libs/filters.o ./Core/Lib/flight_controller_libs/filters.su ./Core/Lib/flight_controller_libs/flight_control_loop.cyclo ./Core/Lib/flight_controller_libs/flight_control_loop.d ./Core/Lib/flight_controller_libs/flight_control_loop.o ./Core/Lib/flight_controller_libs/flight_control_loop.su ./Core/Lib/flight_controller_libs/geometry2D.cyclo ./Core/Lib/flight_controller_libs/geometry2D.d ./Core/Lib/flight_controller_libs/geometry2D.o ./Core/Lib/flight_controller_libs/geometry2D.su ./Core/Lib/flight_controller_libs/geometry3D.cyclo ./Core/Lib/flight_controller_libs/geometry3D.d ./Core/Lib/flight_controller_libs/geometry3D.o ./Core/Lib/flight_controller_libs/geometry3D.su ./Core/Lib/flight_controller_libs/imu.cyclo ./Core/Lib/flight_controller_libs/imu.d ./Core/Lib/flight_controller_libs/imu.o ./Core/Lib/flight_controller_libs/imu.su ./Core/Lib/flight_controller_libs/madgwick_filter.cyclo ./Core/Lib/flight_controller_libs/madgwick_filter.d ./Core/Lib/flight_controller_libs/madgwick_filter.o ./Core/Lib/flight_controller_libs/madgwick_filter.su ./Core/Lib/flight_controller_libs/motor_mixer.cyclo ./Core/Lib/flight_controller_libs/motor_mixer.d ./Core/Lib/flight_controller_libs/motor_mixer.o ./Core/Lib/flight_controller_libs/motor_mixer.su ./Core/Lib/flight_controller_libs/pid.cyclo ./Core/Lib/flight_controller_libs/pid.d ./Core/Lib/flight_controller_libs/pid.o ./Core/Lib/flight_controller_libs/pid.su ./Core/Lib/flight_controller_libs/rate_controller.cyclo ./Core/Lib/flight_controller_libs/rate_controller.d ./Core/Lib/flight_controller_libs/rate_controller.o ./Core/Lib/flight_controller_libs/rate_controller.su ./Core/Lib/flight_controller_libs/rc_control.cyclo ./Core/Lib/flight_controller_libs/rc_control.d ./Core/Lib/flight_controller_libs/rc_control.o ./Core/Lib/flight_controller_libs/rc_control.su

.PHONY: clean-Core-2f-Lib-2f-flight_controller_libs

