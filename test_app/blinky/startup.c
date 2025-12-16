/* Test Application Startup Code */

#include <stdint.h>

/* Linker symbols */
extern uint32_t _stack_top;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

/* Forward declarations */
void Reset_Handler(void);
void Default_Handler(void);
int main(void);

/* Weak exception handlers */
__attribute__((weak)) void NMI_Handler(void)        { while(1); }
__attribute__((weak)) void HardFault_Handler(void)  { while(1); }
__attribute__((weak)) void MemManage_Handler(void)  { while(1); }
__attribute__((weak)) void BusFault_Handler(void)   { while(1); }
__attribute__((weak)) void UsageFault_Handler(void) { while(1); }
__attribute__((weak)) void SVC_Handler(void)        { }
__attribute__((weak)) void DebugMon_Handler(void)   { }
__attribute__((weak)) void PendSV_Handler(void)     { }
__attribute__((weak)) void SysTick_Handler(void)    { }

/* Vector table */
__attribute__((section(".isr_vector")))
const void *vector_table[] = {
    &_stack_top,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0, 0, 0, 0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
};

void Reset_Handler(void)
{
    uint32_t *src, *dst;

    /* Copy .data */
    src = &_sidata;
    dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    /* Zero .bss */
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    main();

    while (1) {
        __asm__ volatile ("wfi");
    }
}

