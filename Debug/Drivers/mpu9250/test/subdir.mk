################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/mpu9250/test/driver_mpu9250_dmp_pedometer_test.c \
../Drivers/mpu9250/test/driver_mpu9250_dmp_read_test.c \
../Drivers/mpu9250/test/driver_mpu9250_dmp_tap_orient_motion_test.c \
../Drivers/mpu9250/test/driver_mpu9250_fifo_test.c \
../Drivers/mpu9250/test/driver_mpu9250_read_test.c \
../Drivers/mpu9250/test/driver_mpu9250_register_test.c 

C_DEPS += \
./Drivers/mpu9250/test/driver_mpu9250_dmp_pedometer_test.d \
./Drivers/mpu9250/test/driver_mpu9250_dmp_read_test.d \
./Drivers/mpu9250/test/driver_mpu9250_dmp_tap_orient_motion_test.d \
./Drivers/mpu9250/test/driver_mpu9250_fifo_test.d \
./Drivers/mpu9250/test/driver_mpu9250_read_test.d \
./Drivers/mpu9250/test/driver_mpu9250_register_test.d 

OBJS += \
./Drivers/mpu9250/test/driver_mpu9250_dmp_pedometer_test.o \
./Drivers/mpu9250/test/driver_mpu9250_dmp_read_test.o \
./Drivers/mpu9250/test/driver_mpu9250_dmp_tap_orient_motion_test.o \
./Drivers/mpu9250/test/driver_mpu9250_fifo_test.o \
./Drivers/mpu9250/test/driver_mpu9250_read_test.o \
./Drivers/mpu9250/test/driver_mpu9250_register_test.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/mpu9250/test/%.o Drivers/mpu9250/test/%.su Drivers/mpu9250/test/%.cyclo: ../Drivers/mpu9250/test/%.c Drivers/mpu9250/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-mpu9250-2f-test

clean-Drivers-2f-mpu9250-2f-test:
	-$(RM) ./Drivers/mpu9250/test/driver_mpu9250_dmp_pedometer_test.cyclo ./Drivers/mpu9250/test/driver_mpu9250_dmp_pedometer_test.d ./Drivers/mpu9250/test/driver_mpu9250_dmp_pedometer_test.o ./Drivers/mpu9250/test/driver_mpu9250_dmp_pedometer_test.su ./Drivers/mpu9250/test/driver_mpu9250_dmp_read_test.cyclo ./Drivers/mpu9250/test/driver_mpu9250_dmp_read_test.d ./Drivers/mpu9250/test/driver_mpu9250_dmp_read_test.o ./Drivers/mpu9250/test/driver_mpu9250_dmp_read_test.su ./Drivers/mpu9250/test/driver_mpu9250_dmp_tap_orient_motion_test.cyclo ./Drivers/mpu9250/test/driver_mpu9250_dmp_tap_orient_motion_test.d ./Drivers/mpu9250/test/driver_mpu9250_dmp_tap_orient_motion_test.o ./Drivers/mpu9250/test/driver_mpu9250_dmp_tap_orient_motion_test.su ./Drivers/mpu9250/test/driver_mpu9250_fifo_test.cyclo ./Drivers/mpu9250/test/driver_mpu9250_fifo_test.d ./Drivers/mpu9250/test/driver_mpu9250_fifo_test.o ./Drivers/mpu9250/test/driver_mpu9250_fifo_test.su ./Drivers/mpu9250/test/driver_mpu9250_read_test.cyclo ./Drivers/mpu9250/test/driver_mpu9250_read_test.d ./Drivers/mpu9250/test/driver_mpu9250_read_test.o ./Drivers/mpu9250/test/driver_mpu9250_read_test.su ./Drivers/mpu9250/test/driver_mpu9250_register_test.cyclo ./Drivers/mpu9250/test/driver_mpu9250_register_test.d ./Drivers/mpu9250/test/driver_mpu9250_register_test.o ./Drivers/mpu9250/test/driver_mpu9250_register_test.su

.PHONY: clean-Drivers-2f-mpu9250-2f-test

