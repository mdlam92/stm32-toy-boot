# STM32F103 Secure Boot Challenge
# Toolchain: arm-none-eabi-gcc

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size

# Target MCU
MCU = -mcpu=cortex-m3 -mthumb

# Compiler flags
CFLAGS = $(MCU) -Wall -Wextra -Os -g
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -ffreestanding -nostdlib
CFLAGS += -Iinclude -Isrc
CFLAGS += -Ilib/micro-ecc -Ilib/sha256
CFLAGS += -include src/uECC_config.h

# Linker flags
LDFLAGS = $(MCU) -T linker.ld -nostdlib
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=build/bootloader.map

# Source files
BOOT_SRCS = src/startup.c src/main.c src/bootloader.c src/crypto.c \
            src/flash_protect.c src/hw_init.c src/libc_stubs.c

# Library sources
LIB_SRCS = lib/micro-ecc/uECC.c lib/sha256/sha256.c

BOOT_OBJS = $(BOOT_SRCS:src/%.c=build/%.o)
LIB_OBJS = $(LIB_SRCS:lib/%.c=build/lib_%.o)

# Test app sources
APP_SRCS = test_app/blinky/main.c test_app/blinky/startup.c
APP_OBJS = $(APP_SRCS:test_app/blinky/%.c=build/app_%.o)

.PHONY: all bootloader test_app clean flash sign qemu genkey

all: bootloader test_app

# Create build directory
build:
	mkdir -p build

# Bootloader build
bootloader: build build/bootloader.bin build/bootloader.elf
	$(SIZE) build/bootloader.elf

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

# Library object files (flatten the path)
build/lib_micro-ecc_uECC.o: lib/micro-ecc/uECC.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/lib_sha256_sha256.o: lib/sha256/sha256.c | build
	$(CC) $(CFLAGS) -c $< -o $@

LIB_OBJS = build/lib_micro-ecc_uECC.o build/lib_sha256_sha256.o

build/bootloader.elf: $(BOOT_OBJS) $(LIB_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ -lgcc

build/bootloader.bin: build/bootloader.elf
	$(OBJCOPY) -O binary $< $@

build/bootloader.hex: build/bootloader.elf
	$(OBJCOPY) -O ihex $< $@

# Test application build
test_app: build build/blinky.bin
	@echo "Test app built and signed."

build/app_%.o: test_app/blinky/%.c | build
	$(CC) $(CFLAGS) -DAPP_BUILD -c $< -o $@

build/blinky_raw.elf: $(APP_OBJS)
	$(CC) $(MCU) -T test_app/blinky/app_linker.ld -nostdlib \
		-Wl,--gc-sections $(APP_OBJS) -o $@ -lgcc

build/blinky_raw.bin: build/blinky_raw.elf
	$(OBJCOPY) -O binary $< $@

build/blinky.bin: build/blinky_raw.bin keys/private_key.pem
	python3 tools/sign_image.py $< $@ --key keys/private_key.pem

# Sign the test app (manual invocation)
sign: build/blinky_raw.bin
	python3 tools/sign_image.py build/blinky_raw.bin build/blinky.bin --key keys/private_key.pem

# Generate new keypair
genkey:
	mkdir -p keys
	python3 tools/genkey.py

# Flash to device via OpenOCD
flash: build/bootloader.bin build/blinky.bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program build/bootloader.bin 0x08000000 verify" \
		-c "program build/blinky.bin 0x08004000 verify reset exit"

# Flash bootloader only
flash-boot: build/bootloader.bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program build/bootloader.bin 0x08000000 verify reset exit"

# Run in QEMU (STM32 support is limited, but works for basic testing)
qemu: build/bootloader.bin build/blinky.bin
	@echo "Creating combined image for QEMU..."
	dd if=/dev/zero of=build/flash.bin bs=1K count=64 2>/dev/null
	dd if=build/bootloader.bin of=build/flash.bin bs=1 conv=notrunc 2>/dev/null
	dd if=build/blinky.bin of=build/flash.bin bs=1 seek=16384 conv=notrunc 2>/dev/null
	qemu-system-arm -M stm32vldiscovery -nographic \
		-kernel build/flash.bin \
		-serial mon:stdio

# Alternative: Use xPack QEMU which has better STM32 support
qemu-xpack: build/bootloader.bin build/blinky.bin
	@echo "Creating combined image..."
	dd if=/dev/zero of=build/flash.bin bs=1K count=64 2>/dev/null
	dd if=build/bootloader.bin of=build/flash.bin bs=1 conv=notrunc 2>/dev/null
	dd if=build/blinky.bin of=build/flash.bin bs=1 seek=16384 conv=notrunc 2>/dev/null
	qemu-system-gnuarmeclipse -M STM32F103C8 -nographic \
		-image build/flash.bin

# Disassemble for inspection
disasm: build/bootloader.elf
	$(OBJDUMP) -d -S $< > build/bootloader.lst

clean:
	rm -rf build/

# Debug: show memory usage
size: build/bootloader.elf
	$(SIZE) -A $<

# Generate compile_commands.json for IDE support
compile_commands.json: Makefile
	@echo "Generating compile_commands.json..."
	@echo '[' > $@
	@for src in $(BOOT_SRCS); do \
		echo '  {"directory": "$(CURDIR)", "file": "'$$src'", "command": "$(CC) $(CFLAGS) -c '$$src'"},' >> $@; \
	done
	@echo '  {"directory": "$(CURDIR)", "file": "lib/micro-ecc/uECC.c", "command": "$(CC) $(CFLAGS) -c lib/micro-ecc/uECC.c"},' >> $@
	@echo '  {"directory": "$(CURDIR)", "file": "lib/sha256/sha256.c", "command": "$(CC) $(CFLAGS) -c lib/sha256/sha256.c"}' >> $@
	@echo ']' >> $@
	@echo "Done. Reload window for IntelliSense to pick it up."

# Ensure keys exist before building test app
keys/private_key.pem:
	@echo "No keypair found. Generating new keys..."
	@mkdir -p keys
	@python3 tools/genkey.py
