# STM32 'Secure Boot' w/ issues (for educational purposes)

A minimal first-stage bootloader for STM32F103 (Blue Pill) demonstrating secure boot concepts.

**(this has some flaws - you should never use this)**

## Overview

This bootloader validates and launches signed application images. It implements:

- **Application header validation** - Magic number, size bounds, address checks
- **ECDSA signature verification** - P-256 with SHA-256 (using micro-ecc)
- **Flash write protection** - Option byte configuration for bootloader region
- **Secure boot flow** - Validate → Verify → Jump

## Memory Layout

```
0x0800_0000 +------------------+
            |   Bootloader     |  16KB (write protected region)
            |   (this code)    |
0x0800_4000 +------------------+
            |   App Header     |  256 bytes (metadata + signature)
0x0800_4100 +------------------+
            |   Application    |  remaining flash (~48KB)
            |   Image          |
            +------------------+
```

## Building

### Prerequisites

```bash
# ARM toolchain
sudo apt install gcc-arm-none-eabi    # Debian/Ubuntu
# or
brew install arm-none-eabi-gcc        # macOS
# or
sudo pacman -S arm-none-eabi-gcc      # Arch Linux

# Python virtual environment (for signing tools)
python -m venv .venv
source .venv/bin/activate
pip install cryptography
```

### Build Commands

```bash
# Activate virtual environment (needed for signing)
source .venv/bin/activate

# Generate signing keypair (first time only)
make genkey

# Build bootloader
make bootloader

# Build and sign test application
make test_app

# Build everything
make all

# Show size info
make size

# Disassemble for inspection
make disasm
```

## Testing with QEMU

QEMU has limited STM32 support, but you can do basic testing:

### Option 1: Standard QEMU (basic)

```bash
# Install QEMU
sudo apt install qemu-system-arm

# Build and run
make qemu
```

Note: The `stm32vldiscovery` machine in QEMU is for STM32F100 (Value Line), not F103. 
Peripheral behavior may differ, but basic execution works.

### Option 2: xPack QEMU (better STM32 support)

```bash
# Install xPack QEMU
npm install --global xpm
xpm install --global @xpack-dev-tools/qemu-arm

# Run
make qemu-xpack
```

### Option 3: Renode (best for STM32)

[Renode](https://renode.io/) has excellent STM32F103 support:

```bash
# Install Renode
# See: https://renode.readthedocs.io/en/latest/introduction/installing.html

# Create a Renode script (renode.resc):
cat > renode.resc << 'EOF'
using sysbus
mach create
machine LoadPlatformDescription @platforms/boards/stm32f103.repl
sysbus LoadBinary @build/flash.bin 0x08000000
showAnalyzer sysbus.usart1
start
EOF

# Run
renode renode.resc
```

## Flashing Real Hardware

With an ST-Link programmer:

```bash
# Flash bootloader + app
make flash

# Flash bootloader only
make flash-boot
```

## Project Structure

```
├── src/
│   ├── main.c           # Entry point
│   ├── bootloader.c     # Core boot logic ← THE INTERESTING FILE
│   ├── bootloader.h     # Header structures and constants
│   ├── crypto.c         # ECDSA verification wrapper
│   ├── crypto.h
│   ├── flash_protect.c  # Flash/option byte utilities
│   ├── hw_init.c        # Clock and GPIO setup
│   └── startup.c        # Vector table, Reset_Handler
├── lib/
│   ├── micro-ecc/       # ECDSA library (submodule)
│   └── sha256/          # SHA256 implementation
├── tools/
│   ├── genkey.py        # Generate ECDSA keypair
│   └── sign_image.py    # Sign application images
├── test_app/
│   └── blinky/          # Test application
└── keys/                # Generated keys (gitignored)
```

## Application Header Format

```c
typedef struct {
    uint32_t magic;           // 0xDEADBEEF
    uint32_t version;         // Packed: 0xMMmmPPPP (major.minor.patch)
    uint32_t app_size;        // Size of app binary in bytes
    uint32_t entry_point;     // Entry address (from vector table)
    uint32_t load_addr;       // Load address (0x08004100)
    uint8_t  signature[64];   // ECDSA P-256 signature (r || s)
    uint8_t  reserved[172];   // Padding to 256 bytes
} app_header_t;
```

## Signing Your Own Application

1. Build your app to run from `0x08004100` (see `test_app/blinky/app_linker.ld`)
2. Sign it:

```bash
python3 tools/sign_image.py your_app.bin signed_app.bin --key keys/private_key.pem
```

3. Flash to `0x08004000`:

```bash
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
    -c "program signed_app.bin 0x08004000 verify reset exit"
```

## Security Notes

This is a **toy implementation** for educational purposes. A production secure boot would need:

- Hardware root of trust (secure element, OTP fuses)
- Anti-rollback protection (version monotonic counter)
- Secure key storage (not compiled into bootloader)
- Fault injection countermeasures
- Side-channel resistant crypto
- Measured boot / attestation

## License

MIT

