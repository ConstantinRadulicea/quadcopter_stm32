################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/mpu9250/example/driver_mpu9250_basic.c \
../Drivers/mpu9250/example/driver_mpu9250_dmp.c \
../Drivers/mpu9250/example/driver_mpu9250_fifo.c 

C_DEPS += \
./Drivers/mpu9250/example/driver_mpu9250_basic.d \
./Drivers/mpu9250/example/driver_mpu9250_dmp.d \
./Drivers/mpu9250/example/driver_mpu9250_fifo.d 

OBJS += \
./Drivers/mpu9250/example/driver_mpu9250_basic.o \
./Drivers/mpu9250/example/driver_mpu9250_dmp.o \
./Drivers/mpu9250/example/driver_mpu9250_fifo.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/mpu9250/example/%.o Drivers/mpu9250/example/%.su Drivers/mpu9250/example/%.cyclo: ../Drivers/mpu9250/example/%.c Drivers/mpu9250/example/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-mpu9250-2f-example

clean-Drivers-2f-mpu9250-2f-example:
	-$(RM) ./Drivers/mpu9250/example/driver_mpu9250_basic.cyclo ./Drivers/mpu9250/example/driver_mpu9250_basic.d ./Drivers/mpu9250/example/driver_mpu9250_basic.o ./Drivers/mpu9250/example/driver_mpu9250_basic.su ./Drivers/mpu9250/example/driver_mpu9250_dmp.cyclo ./Drivers/mpu9250/example/driver_mpu9250_dmp.d ./Drivers/mpu9250/example/driver_mpu9250_dmp.o ./Drivers/mpu9250/example/driver_mpu9250_dmp.su ./Drivers/mpu9250/example/driver_mpu9250_fifo.cyclo ./Drivers/mpu9250/example/driver_mpu9250_fifo.d ./Drivers/mpu9250/example/driver_mpu9250_fifo.o ./Drivers/mpu9250/example/driver_mpu9250_fifo.su

.PHONY: clean-Drivers-2f-mpu9250-2f-example

