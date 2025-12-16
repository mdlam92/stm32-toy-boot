/* STM32 Secure Boot Challenge - Entry Point
 * 
 * This is a minimal "toy" bootloader demonstrating secure boot concepts.
 * The code is intentionally flawed for interview discussion purposes.
 */

#include "bootloader.h"
#include "hw_init.h"
#include "stm32f1xx.h"

/* Error indication via LED blinks */
static void indicate_error(boot_status_t err)
{
    /* Blink pattern indicates error type */
    u32 blinks = (u32)err;
    if (blinks == 0) blinks = 1;
    
    while (1) {
        for (u32 i = 0; i < blinks; i++) {
            hw_led_on();
            hw_delay_ms(200);
            hw_led_off();
            hw_delay_ms(200);
        }
        hw_delay_ms(1000);  /* Pause between sequences */
    }
}

int main(void)
{
    boot_status_t status;
    
    /* Initialize hardware */
    hw_init();
    
    /* Quick LED flash to show we're alive */
    hw_led_on();
    hw_delay_ms(50);
    hw_led_off();
    hw_delay_ms(50);
    hw_led_on();
    hw_delay_ms(50);
    hw_led_off();
    
    /* Run bootloader */
    status = bootloader_run();
    
    /* If we get here, boot failed */
    indicate_error(status);
    
    /* Never reached */
    return 0;
}

