################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/mpu9250/example/driver_mpu9250_basic.c \
../Core/Lib/mpu9250/example/driver_mpu9250_dmp.c \
../Core/Lib/mpu9250/example/driver_mpu9250_fifo.c 

C_DEPS += \
./Core/Lib/mpu9250/example/driver_mpu9250_basic.d \
./Core/Lib/mpu9250/example/driver_mpu9250_dmp.d \
./Core/Lib/mpu9250/example/driver_mpu9250_fifo.d 

OBJS += \
./Core/Lib/mpu9250/example/driver_mpu9250_basic.o \
./Core/Lib/mpu9250/example/driver_mpu9250_dmp.o \
./Core/Lib/mpu9250/example/driver_mpu9250_fifo.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/mpu9250/example/%.o Core/Lib/mpu9250/example/%.su Core/Lib/mpu9250/example/%.cyclo: ../Core/Lib/mpu9250/example/%.c Core/Lib/mpu9250/example/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-mpu9250-2f-example

clean-Core-2f-Lib-2f-mpu9250-2f-example:
	-$(RM) ./Core/Lib/mpu9250/example/driver_mpu9250_basic.cyclo ./Core/Lib/mpu9250/example/driver_mpu9250_basic.d ./Core/Lib/mpu9250/example/driver_mpu9250_basic.o ./Core/Lib/mpu9250/example/driver_mpu9250_basic.su ./Core/Lib/mpu9250/example/driver_mpu9250_dmp.cyclo ./Core/Lib/mpu9250/example/driver_mpu9250_dmp.d ./Core/Lib/mpu9250/example/driver_mpu9250_dmp.o ./Core/Lib/mpu9250/example/driver_mpu9250_dmp.su ./Core/Lib/mpu9250/example/driver_mpu9250_fifo.cyclo ./Core/Lib/mpu9250/example/driver_mpu9250_fifo.d ./Core/Lib/mpu9250/example/driver_mpu9250_fifo.o ./Core/Lib/mpu9250/example/driver_mpu9250_fifo.su

.PHONY: clean-Core-2f-Lib-2f-mpu9250-2f-example

