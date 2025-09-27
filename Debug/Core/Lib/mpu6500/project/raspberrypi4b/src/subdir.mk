################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/mpu6500/project/raspberrypi4b/src/main.c 

C_DEPS += \
./Core/Lib/mpu6500/project/raspberrypi4b/src/main.d 

OBJS += \
./Core/Lib/mpu6500/project/raspberrypi4b/src/main.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/mpu6500/project/raspberrypi4b/src/%.o Core/Lib/mpu6500/project/raspberrypi4b/src/%.su Core/Lib/mpu6500/project/raspberrypi4b/src/%.cyclo: ../Core/Lib/mpu6500/project/raspberrypi4b/src/%.c Core/Lib/mpu6500/project/raspberrypi4b/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Programming/STM32F405RGT6_test/Core/Lib/ringbuffer" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Programming/STM32F405RGT6_test/Core/Lib/flight_controller_libs" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-mpu6500-2f-project-2f-raspberrypi4b-2f-src

clean-Core-2f-Lib-2f-mpu6500-2f-project-2f-raspberrypi4b-2f-src:
	-$(RM) ./Core/Lib/mpu6500/project/raspberrypi4b/src/main.cyclo ./Core/Lib/mpu6500/project/raspberrypi4b/src/main.d ./Core/Lib/mpu6500/project/raspberrypi4b/src/main.o ./Core/Lib/mpu6500/project/raspberrypi4b/src/main.su

.PHONY: clean-Core-2f-Lib-2f-mpu6500-2f-project-2f-raspberrypi4b-2f-src

