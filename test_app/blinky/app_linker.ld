/* Test Application Linker Script
 * Application starts at 0x08004100 (after bootloader + header)
 */

MEMORY
{
    /* Application region - after bootloader (16KB) and header (256B) */
    FLASH (rx)  : ORIGIN = 0x08004100, LENGTH = 48K - 256
    SRAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 20K
}

/* Stack at top of SRAM */
_stack_top = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS
{
    /* Vector table at start of app region */
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

    /* Initialized data */
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

    /DISCARD/ :
    {
        *(.ARM.exidx*)
        *(.ARM.attributes*)
    }
}

