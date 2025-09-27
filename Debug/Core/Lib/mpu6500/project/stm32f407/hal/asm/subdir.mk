################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_ew.s \
../Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.s 

S_DEPS += \
./Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_ew.d \
./Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.d 

OBJS += \
./Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_ew.o \
./Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/mpu6500/project/stm32f407/hal/asm/%.o: ../Core/Lib/mpu6500/project/stm32f407/hal/asm/%.s Core/Lib/mpu6500/project/stm32f407/hal/asm/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"C:/Programming/STM32F405RGT6_test/Core/Lib/ringbuffer" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu9250/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu9250/interface" -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Programming/STM32F405RGT6_test/Core/Lib/flight_controller_libs" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Lib-2f-mpu6500-2f-project-2f-stm32f407-2f-hal-2f-asm

clean-Core-2f-Lib-2f-mpu6500-2f-project-2f-stm32f407-2f-hal-2f-asm:
	-$(RM) ./Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_ew.d ./Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_ew.o ./Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.d ./Core/Lib/mpu6500/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.o

.PHONY: clean-Core-2f-Lib-2f-mpu6500-2f-project-2f-stm32f407-2f-hal-2f-asm

