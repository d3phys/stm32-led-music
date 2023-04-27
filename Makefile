#-----------------------
# Compiler/linker flags
#-----------------------

CC = arm-none-eabi-g++

CFLAGS = \
	-Idrivers/cmsis\
	-Idrivers/ll\
	-DSTM32F051x8 \
	-std=c++11 \
	-Wall \
	-Wextra \
	-march=armv6-m \
	-mcpu=cortex-m0

LDFLAGS = \
	-Idrivers\
	-DSTM32F051x8 \
	-Wall \
	-Wextra \
	-Wl,--start-group -lgcc -lc -lg -Wl,--end-group -Wl,--gc-sections \
	-march=armv6-m \
	-mcpu=cortex-m0 \
	-Wl,--warn-common \
	-Wl,--fatal-warnings \
	-Wl,-z,max-page-size=8 \
	-Wl,-T,entry.lds

ifeq ($(DEBUG),1)
	CFLAGS += -g
endif
	CFLAGS += -g3
	LDFLAGS += -g3

#-------
# Files
#-------

SOURCES = \
	entry.S \
	blinkled.cc

OBJECTS_HALFWAY_DONE = $(SOURCES:%.c=build/%.o)
OBJECTS              = $(OBJECTS_HALFWAY_DONE:%.S=build/%.o)

EXECUTABLE_FLASH = build/reaction.elf
BINARY_FLASH     = build/reaction.bin

#---------------
# Build scripts
#---------------

all: $(EXECUTABLE_FLASH) $(BINARY_FLASH) $(SOURCES)

$(EXECUTABLE_FLASH): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BINARY_FLASH): $(EXECUTABLE_FLASH)
	arm-none-eabi-objcopy -O binary $< $@

build/%.o: %.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c $<

build/%.o: %.cc
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c $<

build/%.o: %.S
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf build

#----------------------
# Hardware interaction
#----------------------

flash: $(BINARY_FLASH)
	st-flash write $(BINARY_FLASH) 0x08000000

reset: $(BINARY_FLASH)
	st-flash --connect-under-reset --reset write $(BINARY_FLASH) 0x08000000

GDB_FLAGS = \
	--eval-command="set architecture arm" \
	--eval-command="set gnutarget elf32-littlearm" \
	--eval-command="target remote localhost:1234" \
	--eval-command="file $(EXECUTABLE_FLASH)"

hardware: $(EXECUTABLE_FLASH)
	st-util -p 1234

gdb: $(BINARY_FLASH)
	arm-none-eabi-gdb $(GDB_FLAGS)

.PHONY: all clean flash hardware gdb
