################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bsp/bsp.c \
../src/bsp/stm32f0xx_hal_msp.c \
../src/bsp/stm32f0xx_it.c \
../src/bsp/syscalls.c \
../src/bsp/sysmem.c \
../src/bsp/system_stm32f0xx.c 

OBJS += \
./src/bsp/bsp.o \
./src/bsp/stm32f0xx_hal_msp.o \
./src/bsp/stm32f0xx_it.o \
./src/bsp/syscalls.o \
./src/bsp/sysmem.o \
./src/bsp/system_stm32f0xx.o 

C_DEPS += \
./src/bsp/bsp.d \
./src/bsp/stm32f0xx_hal_msp.d \
./src/bsp/stm32f0xx_it.d \
./src/bsp/syscalls.d \
./src/bsp/sysmem.d \
./src/bsp/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/bsp/%.o src/bsp/%.su src/bsp/%.cyclo: ../src/bsp/%.c src/bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32F051x8 -c -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/ap" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/bsp" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/common" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/hw" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/lib" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/common/hw/include" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/lib/cube_f051/Drivers/CMSIS/Include" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/lib/cube_f051/Drivers/STM32F0xx_HAL_Driver/Inc" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/lib/cube_f051/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/common/core" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-bsp

clean-src-2f-bsp:
	-$(RM) ./src/bsp/bsp.cyclo ./src/bsp/bsp.d ./src/bsp/bsp.o ./src/bsp/bsp.su ./src/bsp/stm32f0xx_hal_msp.cyclo ./src/bsp/stm32f0xx_hal_msp.d ./src/bsp/stm32f0xx_hal_msp.o ./src/bsp/stm32f0xx_hal_msp.su ./src/bsp/stm32f0xx_it.cyclo ./src/bsp/stm32f0xx_it.d ./src/bsp/stm32f0xx_it.o ./src/bsp/stm32f0xx_it.su ./src/bsp/syscalls.cyclo ./src/bsp/syscalls.d ./src/bsp/syscalls.o ./src/bsp/syscalls.su ./src/bsp/sysmem.cyclo ./src/bsp/sysmem.d ./src/bsp/sysmem.o ./src/bsp/sysmem.su ./src/bsp/system_stm32f0xx.cyclo ./src/bsp/system_stm32f0xx.d ./src/bsp/system_stm32f0xx.o ./src/bsp/system_stm32f0xx.su

.PHONY: clean-src-2f-bsp

