/* STM32F103C8T6 Bootloader Linker Script
 * Bootloader occupies first 16KB of flash
 */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 16K
    SRAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 20K
}

/* Application region starts here */
_app_header_addr = 0x08004000;
_app_start_addr  = 0x08004100;

/* Stack at top of SRAM */
_stack_top = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS
{
    /* Vector table must be at start of flash */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* Code */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* Initialized data - loaded from flash, copied to SRAM */
    _sidata = LOADADDR(.data);
    
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT> FLASH

    /* Uninitialized data */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    /* Remove unwanted sections */
    /DISCARD/ :
    {
        *(.ARM.exidx*)
        *(.ARM.attributes*)
    }
}

