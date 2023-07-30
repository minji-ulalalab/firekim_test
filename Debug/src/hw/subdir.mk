################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hw/hw.c 

OBJS += \
./src/hw/hw.o 

C_DEPS += \
./src/hw/hw.d 


# Each subdirectory must supply rules for building sources it contributes
src/hw/%.o src/hw/%.su src/hw/%.cyclo: ../src/hw/%.c src/hw/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32F051x8 -c -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/ap" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/bsp" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/common" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/hw" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/lib" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/common/hw/include" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/lib/cube_f051/Drivers/CMSIS/Include" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/lib/cube_f051/Drivers/STM32F0xx_HAL_Driver/Inc" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/lib/cube_f051/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"D:/001.Device/07.FireKIm_CC_Board/stm32k6u7_fw_v1.100/stm32k6u7_fw_v1.100/src/common/core" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-hw

clean-src-2f-hw:
	-$(RM) ./src/hw/hw.cyclo ./src/hw/hw.d ./src/hw/hw.o ./src/hw/hw.su

.PHONY: clean-src-2f-hw

