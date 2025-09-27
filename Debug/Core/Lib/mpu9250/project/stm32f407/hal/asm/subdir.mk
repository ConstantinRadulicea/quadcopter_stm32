################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.s \
../Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.s 

S_DEPS += \
./Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.d \
./Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.d 

OBJS += \
./Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.o \
./Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/mpu9250/project/stm32f407/hal/asm/%.o: ../Core/Lib/mpu9250/project/stm32f407/hal/asm/%.s Core/Lib/mpu9250/project/stm32f407/hal/asm/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Lib-2f-mpu9250-2f-project-2f-stm32f407-2f-hal-2f-asm

clean-Core-2f-Lib-2f-mpu9250-2f-project-2f-stm32f407-2f-hal-2f-asm:
	-$(RM) ./Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.d ./Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.o ./Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.d ./Core/Lib/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.o

.PHONY: clean-Core-2f-Lib-2f-mpu9250-2f-project-2f-stm32f407-2f-hal-2f-asm

