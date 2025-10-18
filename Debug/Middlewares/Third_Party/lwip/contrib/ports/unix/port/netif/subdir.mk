################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/fifo.c \
../Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/list.c \
../Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/pcapif.c \
../Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/sio.c \
../Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/tapif.c \
../Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/vdeif.c 

C_DEPS += \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/fifo.d \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/list.d \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/pcapif.d \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/sio.d \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/tapif.d \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/vdeif.d 

OBJS += \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/fifo.o \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/list.o \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/pcapif.o \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/sio.o \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/tapif.o \
./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/vdeif.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/%.o Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/%.su Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/%.cyclo: ../Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/%.c Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Programming/STM32F405RGT6_test/Core/Lib/ringbuffer" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Programming/STM32F405RGT6_test/Core/Lib/flight_controller_libs" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/interface" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src/include" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src/lwshell" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/uart_driver" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-lwip-2f-contrib-2f-ports-2f-unix-2f-port-2f-netif

clean-Middlewares-2f-Third_Party-2f-lwip-2f-contrib-2f-ports-2f-unix-2f-port-2f-netif:
	-$(RM) ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/fifo.cyclo ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/fifo.d ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/fifo.o ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/fifo.su ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/list.cyclo ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/list.d ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/list.o ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/list.su ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/pcapif.cyclo ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/pcapif.d ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/pcapif.o ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/pcapif.su ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/sio.cyclo ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/sio.d ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/sio.o ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/sio.su ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/tapif.cyclo ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/tapif.d ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/tapif.o ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/tapif.su ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/vdeif.cyclo ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/vdeif.d ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/vdeif.o ./Middlewares/Third_Party/lwip/contrib/ports/unix/port/netif/vdeif.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-lwip-2f-contrib-2f-ports-2f-unix-2f-port-2f-netif

