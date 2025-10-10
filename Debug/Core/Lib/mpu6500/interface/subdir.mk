################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/mpu6500/interface/driver_mpu6500_basic.c \
../Core/Lib/mpu6500/interface/driver_mpu6500_interface.c 

C_DEPS += \
./Core/Lib/mpu6500/interface/driver_mpu6500_basic.d \
./Core/Lib/mpu6500/interface/driver_mpu6500_interface.d 

OBJS += \
./Core/Lib/mpu6500/interface/driver_mpu6500_basic.o \
./Core/Lib/mpu6500/interface/driver_mpu6500_interface.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/mpu6500/interface/%.o Core/Lib/mpu6500/interface/%.su Core/Lib/mpu6500/interface/%.cyclo: ../Core/Lib/mpu6500/interface/%.c Core/Lib/mpu6500/interface/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Programming/STM32F405RGT6_test/Core/Lib/ringbuffer" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Programming/STM32F405RGT6_test/Core/Lib/flight_controller_libs" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/interface" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src/include" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src/lwshell" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-mpu6500-2f-interface

clean-Core-2f-Lib-2f-mpu6500-2f-interface:
	-$(RM) ./Core/Lib/mpu6500/interface/driver_mpu6500_basic.cyclo ./Core/Lib/mpu6500/interface/driver_mpu6500_basic.d ./Core/Lib/mpu6500/interface/driver_mpu6500_basic.o ./Core/Lib/mpu6500/interface/driver_mpu6500_basic.su ./Core/Lib/mpu6500/interface/driver_mpu6500_interface.cyclo ./Core/Lib/mpu6500/interface/driver_mpu6500_interface.d ./Core/Lib/mpu6500/interface/driver_mpu6500_interface.o ./Core/Lib/mpu6500/interface/driver_mpu6500_interface.su

.PHONY: clean-Core-2f-Lib-2f-mpu6500-2f-interface

