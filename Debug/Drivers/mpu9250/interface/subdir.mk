################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/mpu9250/interface/driver_mpu9250_interface_template.c 

C_DEPS += \
./Drivers/mpu9250/interface/driver_mpu9250_interface_template.d 

OBJS += \
./Drivers/mpu9250/interface/driver_mpu9250_interface_template.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/mpu9250/interface/%.o Drivers/mpu9250/interface/%.su Drivers/mpu9250/interface/%.cyclo: ../Drivers/mpu9250/interface/%.c Drivers/mpu9250/interface/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-mpu9250-2f-interface

clean-Drivers-2f-mpu9250-2f-interface:
	-$(RM) ./Drivers/mpu9250/interface/driver_mpu9250_interface_template.cyclo ./Drivers/mpu9250/interface/driver_mpu9250_interface_template.d ./Drivers/mpu9250/interface/driver_mpu9250_interface_template.o ./Drivers/mpu9250/interface/driver_mpu9250_interface_template.su

.PHONY: clean-Drivers-2f-mpu9250-2f-interface

