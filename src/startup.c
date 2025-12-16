/* STM32F103 Startup Code
 * Vector table and Reset_Handler
 */

#include "stm32f1xx.h"

/* Linker symbols */
extern u32 _stack_top;
extern u32 _sidata;
extern u32 _sdata;
extern u32 _edata;
extern u32 _sbss;
extern u32 _ebss;

/* Forward declarations */
void Reset_Handler(void);
void Default_Handler(void);
int main(void);

/* Weak aliases for exception handlers */
void NMI_Handler(void)          WEAK;
void HardFault_Handler(void)    WEAK;
void MemManage_Handler(void)    WEAK;
void BusFault_Handler(void)     WEAK;
void UsageFault_Handler(void)   WEAK;
void SVC_Handler(void)          WEAK;
void DebugMon_Handler(void)     WEAK;
void PendSV_Handler(void)       WEAK;
void SysTick_Handler(void)      WEAK;

/* Vector table - placed at start of flash */
SECTION(".isr_vector")
const void *vector_table[] = {
    &_stack_top,            /* Initial stack pointer */
    Reset_Handler,          /* Reset */
    NMI_Handler,            /* NMI */
    HardFault_Handler,      /* Hard Fault */
    MemManage_Handler,      /* MPU Fault */
    BusFault_Handler,       /* Bus Fault */
    UsageFault_Handler,     /* Usage Fault */
    0, 0, 0, 0,             /* Reserved */
    SVC_Handler,            /* SVCall */
    DebugMon_Handler,       /* Debug Monitor */
    0,                      /* Reserved */
    PendSV_Handler,         /* PendSV */
    SysTick_Handler,        /* SysTick */
    /* IRQs would follow here... not needed for bootloader */
};

/* Reset handler - entry point after reset */
void Reset_Handler(void)
{
    u32 *src, *dst;

    /* Copy .data section from flash to SRAM */
    src = &_sidata;
    dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    /* Zero .bss section */
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    /* Call main */
    main();

    /* Should never return, but if it does... */
    while (1) {
        __asm__ volatile ("wfi");
    }
}

/* Default handlers - infinite loop */
void Default_Handler(void)
{
    while (1);
}

void NMI_Handler(void)
{
    while (1);
}

void HardFault_Handler(void)
{
    while (1);
}

void MemManage_Handler(void)
{
    while (1);
}

void BusFault_Handler(void)
{
    while (1);
}

void UsageFault_Handler(void)
{
    while (1);
}

void SVC_Handler(void)
{
    /* Not used */
}

void DebugMon_Handler(void)
{
    /* Not used */
}

void PendSV_Handler(void)
{
    /* Not used */
}

void SysTick_Handler(void)
{
    /* Not used */
}

