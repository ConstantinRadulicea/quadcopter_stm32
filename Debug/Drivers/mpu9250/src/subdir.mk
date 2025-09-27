################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/mpu9250/src/driver_mpu9250.c 

C_DEPS += \
./Drivers/mpu9250/src/driver_mpu9250.d 

OBJS += \
./Drivers/mpu9250/src/driver_mpu9250.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/mpu9250/src/%.o Drivers/mpu9250/src/%.su Drivers/mpu9250/src/%.cyclo: ../Drivers/mpu9250/src/%.c Drivers/mpu9250/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-mpu9250-2f-src

clean-Drivers-2f-mpu9250-2f-src:
	-$(RM) ./Drivers/mpu9250/src/driver_mpu9250.cyclo ./Drivers/mpu9250/src/driver_mpu9250.d ./Drivers/mpu9250/src/driver_mpu9250.o ./Drivers/mpu9250/src/driver_mpu9250.su

.PHONY: clean-Drivers-2f-mpu9250-2f-src

