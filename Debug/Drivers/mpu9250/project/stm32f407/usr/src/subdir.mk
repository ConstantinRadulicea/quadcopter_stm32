################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/mpu9250/project/stm32f407/usr/src/getopt.c \
../Drivers/mpu9250/project/stm32f407/usr/src/main.c \
../Drivers/mpu9250/project/stm32f407/usr/src/mutex.c \
../Drivers/mpu9250/project/stm32f407/usr/src/shell.c \
../Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_hal_msp.c \
../Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_it.c \
../Drivers/mpu9250/project/stm32f407/usr/src/system_stm32f4xx.c 

C_DEPS += \
./Drivers/mpu9250/project/stm32f407/usr/src/getopt.d \
./Drivers/mpu9250/project/stm32f407/usr/src/main.d \
./Drivers/mpu9250/project/stm32f407/usr/src/mutex.d \
./Drivers/mpu9250/project/stm32f407/usr/src/shell.d \
./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_hal_msp.d \
./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_it.d \
./Drivers/mpu9250/project/stm32f407/usr/src/system_stm32f4xx.d 

OBJS += \
./Drivers/mpu9250/project/stm32f407/usr/src/getopt.o \
./Drivers/mpu9250/project/stm32f407/usr/src/main.o \
./Drivers/mpu9250/project/stm32f407/usr/src/mutex.o \
./Drivers/mpu9250/project/stm32f407/usr/src/shell.o \
./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_hal_msp.o \
./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_it.o \
./Drivers/mpu9250/project/stm32f407/usr/src/system_stm32f4xx.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/mpu9250/project/stm32f407/usr/src/%.o Drivers/mpu9250/project/stm32f407/usr/src/%.su Drivers/mpu9250/project/stm32f407/usr/src/%.cyclo: ../Drivers/mpu9250/project/stm32f407/usr/src/%.c Drivers/mpu9250/project/stm32f407/usr/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-mpu9250-2f-project-2f-stm32f407-2f-usr-2f-src

clean-Drivers-2f-mpu9250-2f-project-2f-stm32f407-2f-usr-2f-src:
	-$(RM) ./Drivers/mpu9250/project/stm32f407/usr/src/getopt.cyclo ./Drivers/mpu9250/project/stm32f407/usr/src/getopt.d ./Drivers/mpu9250/project/stm32f407/usr/src/getopt.o ./Drivers/mpu9250/project/stm32f407/usr/src/getopt.su ./Drivers/mpu9250/project/stm32f407/usr/src/main.cyclo ./Drivers/mpu9250/project/stm32f407/usr/src/main.d ./Drivers/mpu9250/project/stm32f407/usr/src/main.o ./Drivers/mpu9250/project/stm32f407/usr/src/main.su ./Drivers/mpu9250/project/stm32f407/usr/src/mutex.cyclo ./Drivers/mpu9250/project/stm32f407/usr/src/mutex.d ./Drivers/mpu9250/project/stm32f407/usr/src/mutex.o ./Drivers/mpu9250/project/stm32f407/usr/src/mutex.su ./Drivers/mpu9250/project/stm32f407/usr/src/shell.cyclo ./Drivers/mpu9250/project/stm32f407/usr/src/shell.d ./Drivers/mpu9250/project/stm32f407/usr/src/shell.o ./Drivers/mpu9250/project/stm32f407/usr/src/shell.su ./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_hal_msp.cyclo ./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_hal_msp.d ./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_hal_msp.o ./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_hal_msp.su ./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_it.cyclo ./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_it.d ./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_it.o ./Drivers/mpu9250/project/stm32f407/usr/src/stm32f4xx_it.su ./Drivers/mpu9250/project/stm32f407/usr/src/system_stm32f4xx.cyclo ./Drivers/mpu9250/project/stm32f407/usr/src/system_stm32f4xx.d ./Drivers/mpu9250/project/stm32f407/usr/src/system_stm32f4xx.o ./Drivers/mpu9250/project/stm32f407/usr/src/system_stm32f4xx.su

.PHONY: clean-Drivers-2f-mpu9250-2f-project-2f-stm32f407-2f-usr-2f-src

