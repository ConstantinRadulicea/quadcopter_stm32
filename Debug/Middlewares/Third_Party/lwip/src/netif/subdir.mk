################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/lwip/src/netif/bridgeif.c \
../Middlewares/Third_Party/lwip/src/netif/bridgeif_fdb.c \
../Middlewares/Third_Party/lwip/src/netif/ethernet.c \
../Middlewares/Third_Party/lwip/src/netif/lowpan6.c \
../Middlewares/Third_Party/lwip/src/netif/lowpan6_ble.c \
../Middlewares/Third_Party/lwip/src/netif/lowpan6_common.c \
../Middlewares/Third_Party/lwip/src/netif/slipif.c \
../Middlewares/Third_Party/lwip/src/netif/zepif.c 

C_DEPS += \
./Middlewares/Third_Party/lwip/src/netif/bridgeif.d \
./Middlewares/Third_Party/lwip/src/netif/bridgeif_fdb.d \
./Middlewares/Third_Party/lwip/src/netif/ethernet.d \
./Middlewares/Third_Party/lwip/src/netif/lowpan6.d \
./Middlewares/Third_Party/lwip/src/netif/lowpan6_ble.d \
./Middlewares/Third_Party/lwip/src/netif/lowpan6_common.d \
./Middlewares/Third_Party/lwip/src/netif/slipif.d \
./Middlewares/Third_Party/lwip/src/netif/zepif.d 

OBJS += \
./Middlewares/Third_Party/lwip/src/netif/bridgeif.o \
./Middlewares/Third_Party/lwip/src/netif/bridgeif_fdb.o \
./Middlewares/Third_Party/lwip/src/netif/ethernet.o \
./Middlewares/Third_Party/lwip/src/netif/lowpan6.o \
./Middlewares/Third_Party/lwip/src/netif/lowpan6_ble.o \
./Middlewares/Third_Party/lwip/src/netif/lowpan6_common.o \
./Middlewares/Third_Party/lwip/src/netif/slipif.o \
./Middlewares/Third_Party/lwip/src/netif/zepif.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/lwip/src/netif/%.o Middlewares/Third_Party/lwip/src/netif/%.su Middlewares/Third_Party/lwip/src/netif/%.cyclo: ../Middlewares/Third_Party/lwip/src/netif/%.c Middlewares/Third_Party/lwip/src/netif/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Programming/STM32F405RGT6_test/Core/Lib/ringbuffer" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Programming/STM32F405RGT6_test/Core/Lib/flight_controller_libs" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/interface" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src/include" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src/lwshell" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/uart_driver" -I"C:/Programming/STM32F405RGT6_test/Middlewares/Third_Party/lwip/src/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-lwip-2f-src-2f-netif

clean-Middlewares-2f-Third_Party-2f-lwip-2f-src-2f-netif:
	-$(RM) ./Middlewares/Third_Party/lwip/src/netif/bridgeif.cyclo ./Middlewares/Third_Party/lwip/src/netif/bridgeif.d ./Middlewares/Third_Party/lwip/src/netif/bridgeif.o ./Middlewares/Third_Party/lwip/src/netif/bridgeif.su ./Middlewares/Third_Party/lwip/src/netif/bridgeif_fdb.cyclo ./Middlewares/Third_Party/lwip/src/netif/bridgeif_fdb.d ./Middlewares/Third_Party/lwip/src/netif/bridgeif_fdb.o ./Middlewares/Third_Party/lwip/src/netif/bridgeif_fdb.su ./Middlewares/Third_Party/lwip/src/netif/ethernet.cyclo ./Middlewares/Third_Party/lwip/src/netif/ethernet.d ./Middlewares/Third_Party/lwip/src/netif/ethernet.o ./Middlewares/Third_Party/lwip/src/netif/ethernet.su ./Middlewares/Third_Party/lwip/src/netif/lowpan6.cyclo ./Middlewares/Third_Party/lwip/src/netif/lowpan6.d ./Middlewares/Third_Party/lwip/src/netif/lowpan6.o ./Middlewares/Third_Party/lwip/src/netif/lowpan6.su ./Middlewares/Third_Party/lwip/src/netif/lowpan6_ble.cyclo ./Middlewares/Third_Party/lwip/src/netif/lowpan6_ble.d ./Middlewares/Third_Party/lwip/src/netif/lowpan6_ble.o ./Middlewares/Third_Party/lwip/src/netif/lowpan6_ble.su ./Middlewares/Third_Party/lwip/src/netif/lowpan6_common.cyclo ./Middlewares/Third_Party/lwip/src/netif/lowpan6_common.d ./Middlewares/Third_Party/lwip/src/netif/lowpan6_common.o ./Middlewares/Third_Party/lwip/src/netif/lowpan6_common.su ./Middlewares/Third_Party/lwip/src/netif/slipif.cyclo ./Middlewares/Third_Party/lwip/src/netif/slipif.d ./Middlewares/Third_Party/lwip/src/netif/slipif.o ./Middlewares/Third_Party/lwip/src/netif/slipif.su ./Middlewares/Third_Party/lwip/src/netif/zepif.cyclo ./Middlewares/Third_Party/lwip/src/netif/zepif.d ./Middlewares/Third_Party/lwip/src/netif/zepif.o ./Middlewares/Third_Party/lwip/src/netif/zepif.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-lwip-2f-src-2f-netif

