################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/mpu9250/project/raspberrypi4b/driver/src/raspberrypi4b_driver_mpu9250_interface.c 

C_DEPS += \
./Core/Lib/mpu9250/project/raspberrypi4b/driver/src/raspberrypi4b_driver_mpu9250_interface.d 

OBJS += \
./Core/Lib/mpu9250/project/raspberrypi4b/driver/src/raspberrypi4b_driver_mpu9250_interface.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/mpu9250/project/raspberrypi4b/driver/src/%.o Core/Lib/mpu9250/project/raspberrypi4b/driver/src/%.su Core/Lib/mpu9250/project/raspberrypi4b/driver/src/%.cyclo: ../Core/Lib/mpu9250/project/raspberrypi4b/driver/src/%.c Core/Lib/mpu9250/project/raspberrypi4b/driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-mpu9250-2f-project-2f-raspberrypi4b-2f-driver-2f-src

clean-Core-2f-Lib-2f-mpu9250-2f-project-2f-raspberrypi4b-2f-driver-2f-src:
	-$(RM) ./Core/Lib/mpu9250/project/raspberrypi4b/driver/src/raspberrypi4b_driver_mpu9250_interface.cyclo ./Core/Lib/mpu9250/project/raspberrypi4b/driver/src/raspberrypi4b_driver_mpu9250_interface.d ./Core/Lib/mpu9250/project/raspberrypi4b/driver/src/raspberrypi4b_driver_mpu9250_interface.o ./Core/Lib/mpu9250/project/raspberrypi4b/driver/src/raspberrypi4b_driver_mpu9250_interface.su

.PHONY: clean-Core-2f-Lib-2f-mpu9250-2f-project-2f-raspberrypi4b-2f-driver-2f-src

