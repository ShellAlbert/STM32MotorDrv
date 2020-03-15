################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../components/devices/device.c \
../components/devices/m62429.c \
../components/devices/motor.c \
../components/devices/stepper_motor.c 

OBJS += \
./components/devices/device.o \
./components/devices/m62429.o \
./components/devices/motor.o \
./components/devices/stepper_motor.o 

C_DEPS += \
./components/devices/device.d \
./components/devices/m62429.d \
./components/devices/motor.d \
./components/devices/stepper_motor.d 


# Each subdirectory must supply rules for building sources it contributes
components/devices/device.o: ../components/devices/device.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"components/devices/device.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
components/devices/m62429.o: ../components/devices/m62429.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"components/devices/m62429.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
components/devices/motor.o: ../components/devices/motor.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"components/devices/motor.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
components/devices/stepper_motor.o: ../components/devices/stepper_motor.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DNEW_001 -DDEBUG -c -I"D:/STM32MotorDrv/stm32cubeide-motordrv/bsp/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/algorithm" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/devices" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/modules" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/object" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/components/support" -I"D:/STM32MotorDrv/stm32cubeide-motordrv/Application/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"components/devices/stepper_motor.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

