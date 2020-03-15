################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bsp/Src/drv_adc.c \
../bsp/Src/drv_flash.c \
../bsp/Src/drv_uart.c 

OBJS += \
./bsp/Src/drv_adc.o \
./bsp/Src/drv_flash.o \
./bsp/Src/drv_uart.o 

C_DEPS += \
./bsp/Src/drv_adc.d \
./bsp/Src/drv_flash.d \
./bsp/Src/drv_uart.d 


# Each subdirectory must supply rules for building sources it contributes
bsp/Src/drv_adc.o: ../bsp/Src/drv_adc.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"bsp/Src/drv_adc.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
bsp/Src/drv_flash.o: ../bsp/Src/drv_flash.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"bsp/Src/drv_flash.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
bsp/Src/drv_uart.o: ../bsp/Src/drv_uart.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"bsp/Src/drv_uart.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

