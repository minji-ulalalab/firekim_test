################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/lib/cube_f051/Drivers/STM32F0xx_HAL_Driver/Src/subdir.mk
-include src/hw/driver/modbusSlave/subdir.mk
-include src/hw/driver/subdir.mk
-include src/hw/subdir.mk
-include src/common/core/subdir.mk
-include src/bsp/startup/subdir.mk
-include src/bsp/subdir.mk
-include src/ap/subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := stm32k6u7_fw_v1.100
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
stm32k6u7_fw_v1.100.elf \

MAP_FILES += \
stm32k6u7_fw_v1.100.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
stm32k6u7_fw_v1.100.list \


# All Target
all: main-build

# Main-build Target
main-build: stm32k6u7_fw_v1.100.elf secondary-outputs

# Tool invocations
stm32k6u7_fw_v1.100.elf stm32k6u7_fw_v1.100.map: $(OBJS) $(USER_OBJS) D:\001.Device\07.FireKIm_CC_Board\stm32k6u7_fw_v1.100\stm32k6u7_fw_v1.100\src\bsp\ldscript\STM32F051K6UX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "stm32k6u7_fw_v1.100.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"D:\001.Device\07.FireKIm_CC_Board\stm32k6u7_fw_v1.100\stm32k6u7_fw_v1.100\src\bsp\ldscript\STM32F051K6UX_FLASH.ld" --specs=nosys.specs -Wl,-Map="stm32k6u7_fw_v1.100.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

stm32k6u7_fw_v1.100.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "stm32k6u7_fw_v1.100.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout stm32k6u7_fw_v1.100.elf stm32k6u7_fw_v1.100.list stm32k6u7_fw_v1.100.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
