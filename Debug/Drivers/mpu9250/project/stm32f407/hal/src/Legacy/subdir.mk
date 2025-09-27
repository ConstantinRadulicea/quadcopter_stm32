################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_can.c \
../Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_eth.c 

C_DEPS += \
./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_can.d \
./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_eth.d 

OBJS += \
./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_can.o \
./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_eth.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/mpu9250/project/stm32f407/hal/src/Legacy/%.o Drivers/mpu9250/project/stm32f407/hal/src/Legacy/%.su Drivers/mpu9250/project/stm32f407/hal/src/Legacy/%.cyclo: ../Drivers/mpu9250/project/stm32f407/hal/src/Legacy/%.c Drivers/mpu9250/project/stm32f407/hal/src/Legacy/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-mpu9250-2f-project-2f-stm32f407-2f-hal-2f-src-2f-Legacy

clean-Drivers-2f-mpu9250-2f-project-2f-stm32f407-2f-hal-2f-src-2f-Legacy:
	-$(RM) ./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_can.cyclo ./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_can.d ./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_can.o ./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_can.su ./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_eth.cyclo ./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_eth.d ./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_eth.o ./Drivers/mpu9250/project/stm32f407/hal/src/Legacy/stm32f4xx_hal_eth.su

.PHONY: clean-Drivers-2f-mpu9250-2f-project-2f-stm32f407-2f-hal-2f-src-2f-Legacy

