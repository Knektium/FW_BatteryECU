ifndef $(TOOLCHAIN_PATH)
  TOOLCHAIN_PATH := /usr/bin
endif
ifndef $(NODE_ID)
  NODE_ID := 0x31
endif
CC := $(TOOLCHAIN_PATH)/arm-none-eabi-gcc
OBJCOPY := $(TOOLCHAIN_PATH)/arm-none-eabi-objcopy
OBJDUMP := $(TOOLCHAIN_PATH)/arm-none-eabi-objdump
RM := rm -f
MEDDELA := python3 -m meddela

APP_NAME := BatteryECU

MAP_FILE := $(APP_NAME).map
LINKER_SCRIPT := linker_script.ld
TARGET_MCU := XMC1404_F064x0200
TARGET_MCU_FAMILY := XMC1400
LIBS := -lm

CFLAGS := \
	-c \
	-O0 \
	-ffunction-sections \
	-fdata-sections \
	-Wall \
	-std=gnu99 \
	-pipe \
	-c \
	-fmessage-length=0 \
	-mcpu=cortex-m0 \
	-mthumb \
	-g3 \
	-gdwarf-2 \
	-D"$(TARGET_MCU)"

OBJS :=

DAVE_PATH := Dave/Generated
LIB_PATH := Libraries
STARTUP_PATH := Startup
MEDDELA_CONFIG_PATH := ..
MEDDELA_TEMPLATE_PATH := $(MEDDELA_CONFIG_PATH)/templates

INCLUDE_PATHS := \
	-I. \
	-I"Libraries/XMCLib/inc" \
	-I"Libraries/CMSIS/Include" \
	-I"Libraries/CMSIS/Infineon/$(TARGET_MCU_FAMILY)_series/Include" \
	-I"Libraries" \
	-I"$(DAVE_PATH)"

DAVE_MODULES := \
	. \
	INTERRUPT \
	ADC_MEASUREMENT \
	CLOCK_XMC1 \
	CPU_CTRL_XMC1 \
	GLOBAL_SCU_XMC1 \
	GLOBAL_ADC \
	DIGITAL_IO \
	CAN_NODE \
	GLOBAL_CAN \
	FREERTOS \
	FREERTOS/portable/GCC \
	FREERTOS/portable/IAR \
	FREERTOS/portable/Keil

LIB_MODULES := \
	XMCLib/src \
	Newlib

MEDDELA_SOURCE_FILES := \
	CAN_Config_XMC1400.c \
	Tasks/MessageManager.c

MEDDELA_HEADER_FILES := \
	CAN_Config_XMC1400.h \
	CAN_Config.h \
	Tasks/MessageManager.h

.PHONY: all clean cleanall

all: $(APP_NAME).elf $(APP_NAME).hex

define include_module =
  OBJS += $(patsubst %.c,%.o,$(wildcard $(2)/*.c))
endef

# Include Dave modules
$(foreach module,$(DAVE_MODULES),$(eval $(call include_module,$(module),$(DAVE_PATH)/$(module))))

# Include Library modules
$(foreach module,$(LIB_MODULES),$(eval $(call include_module,$(module),$(LIB_PATH)/$(module))))

# Include main
OBJS += $(patsubst %.c,%.o,main.c)

# Include Tasks
OBJS += $(patsubst %.c,%.o,$(wildcard Tasks/*.c))

# Include Meddela files
OBJS += $(patsubst %.c,%.o,$(MEDDELA_SOURCE_FILES))

# Include Startup
OBJS += $(patsubst %.c,%.o,$(wildcard $(STARTUP_PATH)/*.c))
OBJS += $(patsubst %.S,%.o,$(wildcard $(STARTUP_PATH)/*.S))

$(APP_NAME).elf: $(OBJS) $(LINKER_SCRIPT)
	@echo "Linking $@..."
	@$(CC) -T"$(LINKER_SCRIPT)" -nostartfiles -Xlinker --gc-sections -specs=nano.specs -specs=nosys.specs -Wl,-Map,$(MAP_FILE) -mcpu=cortex-m0 -mthumb -g3 -gdwarf-2 -o "$(APP_NAME).elf" $(OBJS) $(LIBS)

$(APP_NAME).hex: $(APP_NAME).elf
	@$(OBJCOPY) -O ihex $< $@

$(MEDDELA_HEADER_FILES):
	$(MEDDELA) render \
	--config=$(MEDDELA_CONFIG_PATH)/wolley.json \
	--id=$(NODE_ID) \
	--template=$(MEDDELA_TEMPLATE_PATH)/$@.template \
	> $@

$(MEDDELA_SOURCE_FILES):
	$(MEDDELA) render \
	--config=$(MEDDELA_CONFIG_PATH)/wolley.json \
	--id=$(NODE_ID) \
	--template=$(MEDDELA_TEMPLATE_PATH)/$@.template \
	> $@

%.o: %.c $(MEDDELA_HEADER_FILES)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(INCLUDE_PATHS) -Wa,-adhlns="$@.lst" -o $@ $<

Startup/startup_$(TARGET_MCU_FAMILY).o: Startup/startup_$(TARGET_MCU_FAMILY).S
	@echo 'Assembling $<'
	@$(CC) -x assembler-with-cpp -D$(TARGET_MCU) $(INCLUDE_PATHS) -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mcpu=cortex-m0 -mthumb -g3 -gdwarf-2 -o "$@" "$<" 

clean:
	-$(RM) $(OBJS)
	-$(RM) $(addsuffix .lst,$(OBJS))

cleanall: clean
	-$(RM) $(APP_NAME).elf
	-$(RM) $(APP_NAME).hex
	-$(RM) $(MEDDELA_SOURCE_FILES) $(MEDDELA_HEADER_FILES)

patch:
	@git apply can_node_conf.patch
