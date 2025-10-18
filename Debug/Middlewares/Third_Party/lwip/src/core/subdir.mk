################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/lwip/src/core/altcp.c \
../Middlewares/Third_Party/lwip/src/core/altcp_alloc.c \
../Middlewares/Third_Party/lwip/src/core/altcp_tcp.c \
../Middlewares/Third_Party/lwip/src/core/def.c \
../Middlewares/Third_Party/lwip/src/core/dns.c \
../Middlewares/Third_Party/lwip/src/core/inet_chksum.c \
../Middlewares/Third_Party/lwip/src/core/init.c \
../Middlewares/Third_Party/lwip/src/core/ip.c \
../Middlewares/Third_Party/lwip/src/core/mem.c \
../Middlewares/Third_Party/lwip/src/core/memp.c \
../Middlewares/Third_Party/lwip/src/core/netif.c \
../Middlewares/Third_Party/lwip/src/core/pbuf.c \
../Middlewares/Third_Party/lwip/src/core/raw.c \
../Middlewares/Third_Party/lwip/src/core/stats.c \
../Middlewares/Third_Party/lwip/src/core/sys.c \
../Middlewares/Third_Party/lwip/src/core/tcp.c \
../Middlewares/Third_Party/lwip/src/core/tcp_in.c \
../Middlewares/Third_Party/lwip/src/core/tcp_out.c \
../Middlewares/Third_Party/lwip/src/core/timeouts.c \
../Middlewares/Third_Party/lwip/src/core/udp.c 

C_DEPS += \
./Middlewares/Third_Party/lwip/src/core/altcp.d \
./Middlewares/Third_Party/lwip/src/core/altcp_alloc.d \
./Middlewares/Third_Party/lwip/src/core/altcp_tcp.d \
./Middlewares/Third_Party/lwip/src/core/def.d \
./Middlewares/Third_Party/lwip/src/core/dns.d \
./Middlewares/Third_Party/lwip/src/core/inet_chksum.d \
./Middlewares/Third_Party/lwip/src/core/init.d \
./Middlewares/Third_Party/lwip/src/core/ip.d \
./Middlewares/Third_Party/lwip/src/core/mem.d \
./Middlewares/Third_Party/lwip/src/core/memp.d \
./Middlewares/Third_Party/lwip/src/core/netif.d \
./Middlewares/Third_Party/lwip/src/core/pbuf.d \
./Middlewares/Third_Party/lwip/src/core/raw.d \
./Middlewares/Third_Party/lwip/src/core/stats.d \
./Middlewares/Third_Party/lwip/src/core/sys.d \
./Middlewares/Third_Party/lwip/src/core/tcp.d \
./Middlewares/Third_Party/lwip/src/core/tcp_in.d \
./Middlewares/Third_Party/lwip/src/core/tcp_out.d \
./Middlewares/Third_Party/lwip/src/core/timeouts.d \
./Middlewares/Third_Party/lwip/src/core/udp.d 

OBJS += \
./Middlewares/Third_Party/lwip/src/core/altcp.o \
./Middlewares/Third_Party/lwip/src/core/altcp_alloc.o \
./Middlewares/Third_Party/lwip/src/core/altcp_tcp.o \
./Middlewares/Third_Party/lwip/src/core/def.o \
./Middlewares/Third_Party/lwip/src/core/dns.o \
./Middlewares/Third_Party/lwip/src/core/inet_chksum.o \
./Middlewares/Third_Party/lwip/src/core/init.o \
./Middlewares/Third_Party/lwip/src/core/ip.o \
./Middlewares/Third_Party/lwip/src/core/mem.o \
./Middlewares/Third_Party/lwip/src/core/memp.o \
./Middlewares/Third_Party/lwip/src/core/netif.o \
./Middlewares/Third_Party/lwip/src/core/pbuf.o \
./Middlewares/Third_Party/lwip/src/core/raw.o \
./Middlewares/Third_Party/lwip/src/core/stats.o \
./Middlewares/Third_Party/lwip/src/core/sys.o \
./Middlewares/Third_Party/lwip/src/core/tcp.o \
./Middlewares/Third_Party/lwip/src/core/tcp_in.o \
./Middlewares/Third_Party/lwip/src/core/tcp_out.o \
./Middlewares/Third_Party/lwip/src/core/timeouts.o \
./Middlewares/Third_Party/lwip/src/core/udp.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/lwip/src/core/%.o Middlewares/Third_Party/lwip/src/core/%.su Middlewares/Third_Party/lwip/src/core/%.cyclo: ../Middlewares/Third_Party/lwip/src/core/%.c Middlewares/Third_Party/lwip/src/core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Core/Lib/mpu9250/src -I../Core/Lib/mpu9250/interface -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Programming/STM32F405RGT6_test/Core/Lib/ringbuffer" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Programming/STM32F405RGT6_test/Core/Lib/flight_controller_libs" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/mpu6500/interface" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src/include" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/lwshell/lwshell/src/lwshell" -I"C:/Programming/STM32F405RGT6_test/Core/Lib/uart_driver" -I"C:/Programming/STM32F405RGT6_test/Middlewares/Third_Party/lwip/src/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-lwip-2f-src-2f-core

clean-Middlewares-2f-Third_Party-2f-lwip-2f-src-2f-core:
	-$(RM) ./Middlewares/Third_Party/lwip/src/core/altcp.cyclo ./Middlewares/Third_Party/lwip/src/core/altcp.d ./Middlewares/Third_Party/lwip/src/core/altcp.o ./Middlewares/Third_Party/lwip/src/core/altcp.su ./Middlewares/Third_Party/lwip/src/core/altcp_alloc.cyclo ./Middlewares/Third_Party/lwip/src/core/altcp_alloc.d ./Middlewares/Third_Party/lwip/src/core/altcp_alloc.o ./Middlewares/Third_Party/lwip/src/core/altcp_alloc.su ./Middlewares/Third_Party/lwip/src/core/altcp_tcp.cyclo ./Middlewares/Third_Party/lwip/src/core/altcp_tcp.d ./Middlewares/Third_Party/lwip/src/core/altcp_tcp.o ./Middlewares/Third_Party/lwip/src/core/altcp_tcp.su ./Middlewares/Third_Party/lwip/src/core/def.cyclo ./Middlewares/Third_Party/lwip/src/core/def.d ./Middlewares/Third_Party/lwip/src/core/def.o ./Middlewares/Third_Party/lwip/src/core/def.su ./Middlewares/Third_Party/lwip/src/core/dns.cyclo ./Middlewares/Third_Party/lwip/src/core/dns.d ./Middlewares/Third_Party/lwip/src/core/dns.o ./Middlewares/Third_Party/lwip/src/core/dns.su ./Middlewares/Third_Party/lwip/src/core/inet_chksum.cyclo ./Middlewares/Third_Party/lwip/src/core/inet_chksum.d ./Middlewares/Third_Party/lwip/src/core/inet_chksum.o ./Middlewares/Third_Party/lwip/src/core/inet_chksum.su ./Middlewares/Third_Party/lwip/src/core/init.cyclo ./Middlewares/Third_Party/lwip/src/core/init.d ./Middlewares/Third_Party/lwip/src/core/init.o ./Middlewares/Third_Party/lwip/src/core/init.su ./Middlewares/Third_Party/lwip/src/core/ip.cyclo ./Middlewares/Third_Party/lwip/src/core/ip.d ./Middlewares/Third_Party/lwip/src/core/ip.o ./Middlewares/Third_Party/lwip/src/core/ip.su ./Middlewares/Third_Party/lwip/src/core/mem.cyclo ./Middlewares/Third_Party/lwip/src/core/mem.d ./Middlewares/Third_Party/lwip/src/core/mem.o ./Middlewares/Third_Party/lwip/src/core/mem.su ./Middlewares/Third_Party/lwip/src/core/memp.cyclo ./Middlewares/Third_Party/lwip/src/core/memp.d ./Middlewares/Third_Party/lwip/src/core/memp.o ./Middlewares/Third_Party/lwip/src/core/memp.su ./Middlewares/Third_Party/lwip/src/core/netif.cyclo ./Middlewares/Third_Party/lwip/src/core/netif.d ./Middlewares/Third_Party/lwip/src/core/netif.o ./Middlewares/Third_Party/lwip/src/core/netif.su ./Middlewares/Third_Party/lwip/src/core/pbuf.cyclo ./Middlewares/Third_Party/lwip/src/core/pbuf.d ./Middlewares/Third_Party/lwip/src/core/pbuf.o ./Middlewares/Third_Party/lwip/src/core/pbuf.su ./Middlewares/Third_Party/lwip/src/core/raw.cyclo ./Middlewares/Third_Party/lwip/src/core/raw.d ./Middlewares/Third_Party/lwip/src/core/raw.o ./Middlewares/Third_Party/lwip/src/core/raw.su ./Middlewares/Third_Party/lwip/src/core/stats.cyclo ./Middlewares/Third_Party/lwip/src/core/stats.d ./Middlewares/Third_Party/lwip/src/core/stats.o ./Middlewares/Third_Party/lwip/src/core/stats.su ./Middlewares/Third_Party/lwip/src/core/sys.cyclo ./Middlewares/Third_Party/lwip/src/core/sys.d ./Middlewares/Third_Party/lwip/src/core/sys.o ./Middlewares/Third_Party/lwip/src/core/sys.su ./Middlewares/Third_Party/lwip/src/core/tcp.cyclo ./Middlewares/Third_Party/lwip/src/core/tcp.d ./Middlewares/Third_Party/lwip/src/core/tcp.o ./Middlewares/Third_Party/lwip/src/core/tcp.su ./Middlewares/Third_Party/lwip/src/core/tcp_in.cyclo ./Middlewares/Third_Party/lwip/src/core/tcp_in.d ./Middlewares/Third_Party/lwip/src/core/tcp_in.o ./Middlewares/Third_Party/lwip/src/core/tcp_in.su ./Middlewares/Third_Party/lwip/src/core/tcp_out.cyclo ./Middlewares/Third_Party/lwip/src/core/tcp_out.d ./Middlewares/Third_Party/lwip/src/core/tcp_out.o ./Middlewares/Third_Party/lwip/src/core/tcp_out.su ./Middlewares/Third_Party/lwip/src/core/timeouts.cyclo ./Middlewares/Third_Party/lwip/src/core/timeouts.d ./Middlewares/Third_Party/lwip/src/core/timeouts.o ./Middlewares/Third_Party/lwip/src/core/timeouts.su ./Middlewares/Third_Party/lwip/src/core/udp.cyclo ./Middlewares/Third_Party/lwip/src/core/udp.d ./Middlewares/Third_Party/lwip/src/core/udp.o ./Middlewares/Third_Party/lwip/src/core/udp.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-lwip-2f-src-2f-core

