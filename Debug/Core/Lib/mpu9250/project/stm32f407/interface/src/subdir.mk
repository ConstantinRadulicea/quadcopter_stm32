################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/mpu9250/project/stm32f407/interface/src/clock.c \
../Core/Lib/mpu9250/project/stm32f407/interface/src/delay.c \
../Core/Lib/mpu9250/project/stm32f407/interface/src/gpio.c \
../Core/Lib/mpu9250/project/stm32f407/interface/src/iic.c \
../Core/Lib/mpu9250/project/stm32f407/interface/src/spi.c \
../Core/Lib/mpu9250/project/stm32f407/interface/src/uart.c 

C_DEPS += \
./Core/Lib/mpu9250/project/stm32f407/interface/src/clock.d \
./Core/Lib/mpu9250/project/stm32f407/interface/src/delay.d \
./Core/Lib/mpu9250/project/stm32f407/interface/src/gpio.d \
./Core/Lib/mpu9250/project/stm32f407/interface/src/iic.d \
./Core/Lib/mpu9250/project/stm32f407/interface/src/spi.d \
./Core/Lib/mpu9250/project/stm32f407/interface/src/uart.d 

OBJS += \
./Core/Lib/mpu9250/project/stm32f407/interface/src/clock.o \
./Core/Lib/mpu9250/project/stm32f407/interface/src/delay.o \
./Core/Lib/mpu9250/project/stm32f407/interface/src/gpio.o \
./Core/Lib/mpu9250/project/stm32f407/interface/src/iic.o \
./Core/Lib/mpu9250/project/stm32f407/interface/src/spi.o \
./Core/Lib/mpu9250/project/stm32f407/interface/src/uart.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/mpu9250/project/stm32f407/interface/src/%.o Core/Lib/mpu9250/project/stm32f407/interface/src/%.su Core/Lib/mpu9250/project/stm32f407/interface/src/%.cyclo: ../Core/Lib/mpu9250/project/stm32f407/interface/src/%.c Core/Lib/mpu9250/project/stm32f407/interface/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-mpu9250-2f-project-2f-stm32f407-2f-interface-2f-src

clean-Core-2f-Lib-2f-mpu9250-2f-project-2f-stm32f407-2f-interface-2f-src:
	-$(RM) ./Core/Lib/mpu9250/project/stm32f407/interface/src/clock.cyclo ./Core/Lib/mpu9250/project/stm32f407/interface/src/clock.d ./Core/Lib/mpu9250/project/stm32f407/interface/src/clock.o ./Core/Lib/mpu9250/project/stm32f407/interface/src/clock.su ./Core/Lib/mpu9250/project/stm32f407/interface/src/delay.cyclo ./Core/Lib/mpu9250/project/stm32f407/interface/src/delay.d ./Core/Lib/mpu9250/project/stm32f407/interface/src/delay.o ./Core/Lib/mpu9250/project/stm32f407/interface/src/delay.su ./Core/Lib/mpu9250/project/stm32f407/interface/src/gpio.cyclo ./Core/Lib/mpu9250/project/stm32f407/interface/src/gpio.d ./Core/Lib/mpu9250/project/stm32f407/interface/src/gpio.o ./Core/Lib/mpu9250/project/stm32f407/interface/src/gpio.su ./Core/Lib/mpu9250/project/stm32f407/interface/src/iic.cyclo ./Core/Lib/mpu9250/project/stm32f407/interface/src/iic.d ./Core/Lib/mpu9250/project/stm32f407/interface/src/iic.o ./Core/Lib/mpu9250/project/stm32f407/interface/src/iic.su ./Core/Lib/mpu9250/project/stm32f407/interface/src/spi.cyclo ./Core/Lib/mpu9250/project/stm32f407/interface/src/spi.d ./Core/Lib/mpu9250/project/stm32f407/interface/src/spi.o ./Core/Lib/mpu9250/project/stm32f407/interface/src/spi.su ./Core/Lib/mpu9250/project/stm32f407/interface/src/uart.cyclo ./Core/Lib/mpu9250/project/stm32f407/interface/src/uart.d ./Core/Lib/mpu9250/project/stm32f407/interface/src/uart.o ./Core/Lib/mpu9250/project/stm32f407/interface/src/uart.su

.PHONY: clean-Core-2f-Lib-2f-mpu9250-2f-project-2f-stm32f407-2f-interface-2f-src

