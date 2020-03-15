################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../components/support/fifo.c \
../components/support/mem_mang4.c \
../components/support/mf_crc.c \
../components/support/package.c 

OBJS += \
./components/support/fifo.o \
./components/support/mem_mang4.o \
./components/support/mf_crc.o \
./components/support/package.o 

C_DEPS += \
./components/support/fifo.d \
./components/support/mem_mang4.d \
./components/support/mf_crc.d \
./components/support/package.d 


# Each subdirectory must supply rules for building sources it contributes
components/support/fifo.o: ../components/support/fifo.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"components/support/fifo.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
components/support/mem_mang4.o: ../components/support/mem_mang4.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"components/support/mem_mang4.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
components/support/mf_crc.o: ../components/support/mf_crc.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"components/support/mf_crc.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
components/support/package.o: ../components/support/package.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"components/support/package.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

