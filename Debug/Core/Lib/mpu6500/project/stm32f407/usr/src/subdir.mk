################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/mpu6500/project/stm32f407/usr/src/getopt.c \
../Core/Lib/mpu6500/project/stm32f407/usr/src/main.c \
../Core/Lib/mpu6500/project/stm32f407/usr/src/mutex.c \
../Core/Lib/mpu6500/project/stm32f407/usr/src/shell.c \
../Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_hal_msp.c \
../Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_it.c \
../Core/Lib/mpu6500/project/stm32f407/usr/src/system_stm32f4xx.c 

C_DEPS += \
./Core/Lib/mpu6500/project/stm32f407/usr/src/getopt.d \
./Core/Lib/mpu6500/project/stm32f407/usr/src/main.d \
./Core/Lib/mpu6500/project/stm32f407/usr/src/mutex.d \
./Core/Lib/mpu6500/project/stm32f407/usr/src/shell.d \
./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_hal_msp.d \
./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_it.d \
./Core/Lib/mpu6500/project/stm32f407/usr/src/system_stm32f4xx.d 

OBJS += \
./Core/Lib/mpu6500/project/stm32f407/usr/src/getopt.o \
./Core/Lib/mpu6500/project/stm32f407/usr/src/main.o \
./Core/Lib/mpu6500/project/stm32f407/usr/src/mutex.o \
./Core/Lib/mpu6500/project/stm32f407/usr/src/shell.o \
./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_hal_msp.o \
./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_it.o \
./Core/Lib/mpu6500/project/stm32f407/usr/src/system_stm32f4xx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/mpu6500/project/stm32f407/usr/src/%.o Core/Lib/mpu6500/project/stm32f407/usr/src/%.su Core/Lib/mpu6500/project/stm32f407/usr/src/%.cyclo: ../Core/Lib/mpu6500/project/stm32f407/usr/src/%.c Core/Lib/mpu6500/project/stm32f407/usr/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Programming/STM32F405RGT6_test/Core/Lib/ringbuffer" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Programming/STM32F405RGT6_test/Core/Lib/flight_controller_libs" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-mpu6500-2f-project-2f-stm32f407-2f-usr-2f-src

clean-Core-2f-Lib-2f-mpu6500-2f-project-2f-stm32f407-2f-usr-2f-src:
	-$(RM) ./Core/Lib/mpu6500/project/stm32f407/usr/src/getopt.cyclo ./Core/Lib/mpu6500/project/stm32f407/usr/src/getopt.d ./Core/Lib/mpu6500/project/stm32f407/usr/src/getopt.o ./Core/Lib/mpu6500/project/stm32f407/usr/src/getopt.su ./Core/Lib/mpu6500/project/stm32f407/usr/src/main.cyclo ./Core/Lib/mpu6500/project/stm32f407/usr/src/main.d ./Core/Lib/mpu6500/project/stm32f407/usr/src/main.o ./Core/Lib/mpu6500/project/stm32f407/usr/src/main.su ./Core/Lib/mpu6500/project/stm32f407/usr/src/mutex.cyclo ./Core/Lib/mpu6500/project/stm32f407/usr/src/mutex.d ./Core/Lib/mpu6500/project/stm32f407/usr/src/mutex.o ./Core/Lib/mpu6500/project/stm32f407/usr/src/mutex.su ./Core/Lib/mpu6500/project/stm32f407/usr/src/shell.cyclo ./Core/Lib/mpu6500/project/stm32f407/usr/src/shell.d ./Core/Lib/mpu6500/project/stm32f407/usr/src/shell.o ./Core/Lib/mpu6500/project/stm32f407/usr/src/shell.su ./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_hal_msp.cyclo ./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_hal_msp.d ./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_hal_msp.o ./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_hal_msp.su ./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_it.cyclo ./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_it.d ./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_it.o ./Core/Lib/mpu6500/project/stm32f407/usr/src/stm32f4xx_it.su ./Core/Lib/mpu6500/project/stm32f407/usr/src/system_stm32f4xx.cyclo ./Core/Lib/mpu6500/project/stm32f407/usr/src/system_stm32f4xx.d ./Core/Lib/mpu6500/project/stm32f407/usr/src/system_stm32f4xx.o ./Core/Lib/mpu6500/project/stm32f407/usr/src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Lib-2f-mpu6500-2f-project-2f-stm32f407-2f-usr-2f-src

