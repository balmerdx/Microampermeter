#USE_CLANG = 1

RM := rm -rf

TC=${ARM_NONE_EABI}/arm-none-eabi
CC=$(TC)-gcc
CC_ASM=$(TC)-gcc
CC_LINK=$(TC)-gcc
LD=$(TC)-ld -v
OBJCOPY=$(TC)-objcopy
AR=$(TC)-ar
GDB=$(TC)-gdb

ifdef USE_CLANG
CLANG_PATH = /home/balmer/radio/clang/clang+llvm-11.0.1-x86_64-linux-gnu-ubuntu-16.04/bin
CC=${CLANG_PATH}/clang
endif


STM_LIB_DIR = "Drivers"
C_INCLIDES= \
-I$(STM_LIB_DIR)/STM32H7xx_HAL_Driver/Inc \
-I$(STM_LIB_DIR)/STM32H7xx_HAL_Driver/Inc/Legacy \
-I$(STM_LIB_DIR)/CMSIS/Include \
-I$(STM_LIB_DIR)/CMSIS/Device/ST/STM32H7xx/Include \
-ICore/Src \
-ICore/Inc \
-ICore/display \
-IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc \
-IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
-IUSB_DEVICE/App \
-IUSB_DEVICE/Target

C_INCLIDES+= -includeDrivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal.h

M_CPU = -mthumb -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard
LDFLAGS = $(M_CPU) -specs=nano.specs

M_CPU_ADDITIONAL = 
ifdef USE_CLANG
	#M_CPU_ADDITIONAL += --target=arm-none-eabi -fno-short-enums
	M_CPU_ADDITIONAL += --target=armv7em-none-eabi

	C_INCLIDES += -isystem${ARM_NONE_EABI}/../arm-none-eabi/include
endif

ifeq ($(MAKECMDGOALS),debug)
C_DEFINES= -DDEBUG
C_FLAGS = -O0 -g -gdwarf-2
endif

ifeq ($(MAKECMDGOALS),release)
C_DEFINES= -DNDEBUG
	ifdef USE_CLANG
		C_FLAGS = -Oz -g -gdwarf-2
	else
		C_FLAGS = -Os -g -gdwarf-2
	endif
endif

#C_FLAGS += -S #asm listing

C_FLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,--gc-sections 
LDFLAGS += -Wl,--print-memory-usage

C_FLAGS += -ffast-math

#C_FLAGS += -std=gnu11
#C_FLAGS += -funsigned-char
C_DEFINES+= \
-DUSE_HAL_DRIVER \
-DSTM32H750xx \
-DHSE_VALUE=8000000 \
-DARM_MATH_CM7 \
-DDISPLAY_ILI9481

C_FLAGS+= -Wall -c -MMD $(C_INCLIDES) $(C_DEFINES) $(M_CPU) ${M_CPU_ADDITIONAL}

LIBS = -lc -lm -lnosys
LDFLAGS += -TSTM32H750VBTx_FLASH.ld  $(LIBS)  -Wl,-Map=output/linker.map,--cref
ASM_FLAGS = -x assembler-with-cpp -c $(M_CPU)

TARGET=output/program

# All of the sources participating in the build are defined here
include sources.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

# All Target
#all: executable

release: executable

debug: executable

# Tool invocations
executable: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Linker'
	@$(CC_LINK) $(OBJS) $(USER_OBJS) $(LDFLAGS) -o "$(TARGET)_$(MAKECMDGOALS).elf"
	@echo 'Finished building target: $@'
	@echo 'Create Binary'
	@$(OBJCOPY) -S  -O binary  "$(TARGET)_$(MAKECMDGOALS).elf" "$(TARGET)_$(MAKECMDGOALS).bin"
	@echo ' '

# Other Targets
clean:
	-@echo 'Clean files'	
	-@$(RM) $(OBJS)$(DEPS) $(TARGET)
	-@echo ' '
	-@$(RM) linker.map

flashd:
	python ~/radio/stm32/tools/stm32_flash_0.py "${CURDIR}/$(TARGET)_debug.bin"

flashr:
	python ~/radio/stm32/tools/stm32_flash_0.py "${CURDIR}/$(TARGET)_release.bin"

.PHONY: all clean dependents
