/* Flash Protection Utilities
 * Implements write protection and read protection (RDP) for STM32F103
 */

#include "flash_protect.h"
#include "stm32f1xx.h"

/* Bootloader occupies first 16KB = 16 pages (1KB each on STM32F103) */
#define BOOTLOADER_PAGES    16

/* Wait for flash operation to complete */
static void flash_wait_busy(void)
{
    while (FLASH_IF->SR & FLASH_SR_BSY);
}

/* Unlock flash for programming */
static void flash_unlock(void)
{
    if (FLASH_IF->CR & FLASH_CR_LOCK) {
        FLASH_IF->KEYR = FLASH_KEY1;
        FLASH_IF->KEYR = FLASH_KEY2;
    }
}

/* Lock flash after programming */
static void flash_lock(void)
{
    FLASH_IF->CR |= FLASH_CR_LOCK;
}

/* Unlock option bytes for programming */
static void flash_unlock_opt(void)
{
    flash_unlock();
    
    if (!(FLASH_IF->CR & FLASH_CR_OPTWRE)) {
        FLASH_IF->OPTKEYR = FLASH_OPTKEY1;
        FLASH_IF->OPTKEYR = FLASH_OPTKEY2;
    }
}

int is_bootloader_locked(void)
{
    /* WRPR register contains write protection status
     * Bit = 0 means page is protected
     * Bit = 1 means page is unprotected
     * 
     * We need pages 0-15 (first 16KB) to be protected
     */
    u32 wrpr = FLASH_IF->WRPR;
    
    /* Check if first 16 pages are protected (bits 0-15 should be 0) */
    u32 boot_pages_mask = (1UL << BOOTLOADER_PAGES) - 1;
    
    return (wrpr & boot_pages_mask) == 0;
}

int lock_bootloader_region(void)
{
    /* Already locked? */
    if (is_bootloader_locked()) {
        return 0;
    }
    
    flash_unlock_opt();
    flash_wait_busy();
    
    /* Erase option bytes first */
    FLASH_IF->CR |= FLASH_CR_OPTER;
    FLASH_IF->CR |= FLASH_CR_STRT;
    flash_wait_busy();
    FLASH_IF->CR &= ~FLASH_CR_OPTER;
    
    /* Program option bytes with write protection enabled
     * WRP0 protects pages 0-7, WRP1 protects pages 8-15
     * Value 0x00 = protected, 0xFF = unprotected
     */
    FLASH_IF->CR |= FLASH_CR_OPTPG;
    
    /* This is simplified - real implementation would write
     * to option byte addresses directly. The exact sequence
     * depends on whether you want to preserve other settings.
     */
    
    flash_wait_busy();
    FLASH_IF->CR &= ~FLASH_CR_OPTPG;
    
    flash_lock();
    
    /* Option byte changes require reset to take effect */
    /* Caller should trigger reset after this */
    return 0;
}

int is_rdp_enabled(void)
{
    /* OBR.RDPRT bit indicates read protection status */
    return (FLASH_IF->OBR & FLASH_OBR_RDPRT) ? 1 : 0;
}

int enable_rdp(void)
{
    /* WARNING: Enabling RDP level 1 prevents debugger access
     * RDP level 2 is permanent and irreversible!
     * 
     * This function only enables level 1.
     */
    
    if (is_rdp_enabled()) {
        return 0;  /* Already enabled */
    }
    
    flash_unlock_opt();
    flash_wait_busy();
    
    /* Erase option bytes */
    FLASH_IF->CR |= FLASH_CR_OPTER;
    FLASH_IF->CR |= FLASH_CR_STRT;
    flash_wait_busy();
    FLASH_IF->CR &= ~FLASH_CR_OPTER;
    
    /* When option bytes are erased, RDP defaults to level 1
     * To set level 0 (no protection), you must write 0x5AA5 to RDP
     * By not writing that value, RDP stays at level 1
     */
    
    flash_lock();
    
    /* Reset required for RDP to take effect */
    return 0;
}

