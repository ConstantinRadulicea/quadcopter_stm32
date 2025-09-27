################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/mpu9250/project/raspberrypi4b/interface/src/gpio.c \
../Drivers/mpu9250/project/raspberrypi4b/interface/src/iic.c \
../Drivers/mpu9250/project/raspberrypi4b/interface/src/mutex.c \
../Drivers/mpu9250/project/raspberrypi4b/interface/src/spi.c 

C_DEPS += \
./Drivers/mpu9250/project/raspberrypi4b/interface/src/gpio.d \
./Drivers/mpu9250/project/raspberrypi4b/interface/src/iic.d \
./Drivers/mpu9250/project/raspberrypi4b/interface/src/mutex.d \
./Drivers/mpu9250/project/raspberrypi4b/interface/src/spi.d 

OBJS += \
./Drivers/mpu9250/project/raspberrypi4b/interface/src/gpio.o \
./Drivers/mpu9250/project/raspberrypi4b/interface/src/iic.o \
./Drivers/mpu9250/project/raspberrypi4b/interface/src/mutex.o \
./Drivers/mpu9250/project/raspberrypi4b/interface/src/spi.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/mpu9250/project/raspberrypi4b/interface/src/%.o Drivers/mpu9250/project/raspberrypi4b/interface/src/%.su Drivers/mpu9250/project/raspberrypi4b/interface/src/%.cyclo: ../Drivers/mpu9250/project/raspberrypi4b/interface/src/%.c Drivers/mpu9250/project/raspberrypi4b/interface/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-mpu9250-2f-project-2f-raspberrypi4b-2f-interface-2f-src

clean-Drivers-2f-mpu9250-2f-project-2f-raspberrypi4b-2f-interface-2f-src:
	-$(RM) ./Drivers/mpu9250/project/raspberrypi4b/interface/src/gpio.cyclo ./Drivers/mpu9250/project/raspberrypi4b/interface/src/gpio.d ./Drivers/mpu9250/project/raspberrypi4b/interface/src/gpio.o ./Drivers/mpu9250/project/raspberrypi4b/interface/src/gpio.su ./Drivers/mpu9250/project/raspberrypi4b/interface/src/iic.cyclo ./Drivers/mpu9250/project/raspberrypi4b/interface/src/iic.d ./Drivers/mpu9250/project/raspberrypi4b/interface/src/iic.o ./Drivers/mpu9250/project/raspberrypi4b/interface/src/iic.su ./Drivers/mpu9250/project/raspberrypi4b/interface/src/mutex.cyclo ./Drivers/mpu9250/project/raspberrypi4b/interface/src/mutex.d ./Drivers/mpu9250/project/raspberrypi4b/interface/src/mutex.o ./Drivers/mpu9250/project/raspberrypi4b/interface/src/mutex.su ./Drivers/mpu9250/project/raspberrypi4b/interface/src/spi.cyclo ./Drivers/mpu9250/project/raspberrypi4b/interface/src/spi.d ./Drivers/mpu9250/project/raspberrypi4b/interface/src/spi.o ./Drivers/mpu9250/project/raspberrypi4b/interface/src/spi.su

.PHONY: clean-Drivers-2f-mpu9250-2f-project-2f-raspberrypi4b-2f-interface-2f-src

