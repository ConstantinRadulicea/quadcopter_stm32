################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/ringbuffer/ringbuffer.c 

C_DEPS += \
./Core/Lib/ringbuffer/ringbuffer.d 

OBJS += \
./Core/Lib/ringbuffer/ringbuffer.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/ringbuffer/%.o Core/Lib/ringbuffer/%.su Core/Lib/ringbuffer/%.cyclo: ../Core/Lib/ringbuffer/%.c Core/Lib/ringbuffer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Programming/STM32F405RGT6_test/Core/Lib/Dshot" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/ringbuffer" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu9250/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu9250/interface" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-ringbuffer

clean-Core-2f-Lib-2f-ringbuffer:
	-$(RM) ./Core/Lib/ringbuffer/ringbuffer.cyclo ./Core/Lib/ringbuffer/ringbuffer.d ./Core/Lib/ringbuffer/ringbuffer.o ./Core/Lib/ringbuffer/ringbuffer.su

.PHONY: clean-Core-2f-Lib-2f-ringbuffer

