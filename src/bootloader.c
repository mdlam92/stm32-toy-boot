/* Secure Boot Challenge - Core Bootloader Logic
 * 
 * This file implements the main boot flow:
 * 1. Validate application header
 * 2. Verify cryptographic signature
 * 3. Jump to application
 * 
 * INTERVIEW NOTE: This code contains intentional security flaws.
 * Can you find them?
 */

#include "bootloader.h"
#include "crypto.h"
#include "flash_protect.h"
#include "hw_init.h"
#include "stm32f1xx.h"

/* Application header location in flash */
#define APP_HEADER  ((const app_header_t *)APP_HEADER_ADDR)

/* Application image location */
#define APP_IMAGE   ((const u8 *)APP_IMAGE_ADDR)

/*
 * Validate application header fields
 */
boot_status_t validate_header(const app_header_t *hdr)
{
    /* Check magic number */
    if (hdr->magic != APP_MAGIC) {
        return BOOT_ERR_BAD_MAGIC;
    }
    
    /* Validate size - must be non-zero and fit in flash */
    if (hdr->app_size == 0 || hdr->app_size > APP_MAX_SIZE) {
        return BOOT_ERR_BAD_SIZE;
    }
    
    /* Validate load address */
    if (hdr->load_addr != APP_IMAGE_ADDR) {
        return BOOT_ERR_BAD_ADDR;
    }
    
    /* Validate entry point - must be within application region */
    if (hdr->entry_point < APP_IMAGE_ADDR) {
        return BOOT_ERR_BAD_ADDR;
    }
    
    return BOOT_OK;
}

/*
 * Verify application signature
 */
boot_status_t verify_signature(const app_header_t *hdr, const u8 *app_data)
{
    
    if (!crypto_verify_signature(hdr->signature, SIGNATURE_SIZE,
                                  app_data, hdr->app_size)) {
        return BOOT_ERR_BAD_SIG;
    }
    
    return BOOT_OK;
}

/*
 * Jump to application
 * 
 * This function sets up the environment and transfers control
 * to the application. It should never return.
 */
NORETURN void boot_application(const app_header_t *hdr)
{
    /* Application's vector table */
    const u32 *app_vectors = (const u32 *)hdr->load_addr;
    
    /* Get stack pointer and reset handler from app's vector table */
    u32 app_sp = app_vectors[0];
    u32 app_reset = app_vectors[1];
    
    /* Disable interrupts during transition */
    __asm__ volatile ("cpsid i");
    
    /* Set vector table offset to application's vectors */
    SCB->VTOR = hdr->load_addr;
    
    /* Memory barriers before jump */
    __DSB();
    __ISB();
    
    /* Set stack pointer and jump to application */
    __asm__ volatile (
        "msr msp, %0    \n"  /* Set main stack pointer */
        "bx %1          \n"  /* Branch to reset handler */
        :
        : "r" (app_sp), "r" (app_reset)
        : "memory"
    );
    
    /* Should never reach here */
    while (1);
}

/*
 * Main bootloader entry point
 */
boot_status_t bootloader_run(void)
{
    boot_status_t status;
    
    /* Blink LED to indicate bootloader is running */
    hw_led_on();
    hw_delay_ms(100);
    hw_led_off();
    
            
    /* Validate application header */
    status = validate_header(APP_HEADER);
    if (status != BOOT_OK) {
        return status;
    }
    
    /* Verify signature */
    status = verify_signature(APP_HEADER, APP_IMAGE);
    if (status != BOOT_OK) {
        return status;
    }
    
    /* All checks passed - boot the application */
    boot_application(APP_HEADER);
    
    /* Never reached */
    return BOOT_OK;
}

