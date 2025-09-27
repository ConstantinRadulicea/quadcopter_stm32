################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.s \
../Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.s 

S_DEPS += \
./Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.d \
./Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.d 

OBJS += \
./Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.o \
./Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/mpu9250/project/stm32f407/hal/asm/%.o: ../Drivers/mpu9250/project/stm32f407/hal/asm/%.s Drivers/mpu9250/project/stm32f407/hal/asm/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Drivers-2f-mpu9250-2f-project-2f-stm32f407-2f-hal-2f-asm

clean-Drivers-2f-mpu9250-2f-project-2f-stm32f407-2f-hal-2f-asm:
	-$(RM) ./Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.d ./Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_ew.o ./Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.d ./Drivers/mpu9250/project/stm32f407/hal/asm/startup_stm32f407xx_mdk.o

.PHONY: clean-Drivers-2f-mpu9250-2f-project-2f-stm32f407-2f-hal-2f-asm

