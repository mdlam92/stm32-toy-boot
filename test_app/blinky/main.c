/* Test Application - Blinky
 * 
 * Simple LED blinker to verify bootloader works correctly.
 * Blinks the onboard LED (PC13) at a different rate than bootloader
 * so you can tell when the app has taken over.
 */

#include <stdint.h>

/* Register definitions (minimal, just what we need) */
#define RCC_BASE        0x40021000
#define GPIOC_BASE      0x40011000

#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define GPIOC_CRH       (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR       (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))

#define RCC_APB2ENR_IOPCEN  (1 << 4)
#define LED_PIN             13

/* Simple delay */
static void delay(volatile uint32_t count)
{
    while (count--);
}

/* Initialize LED GPIO */
static void led_init(void)
{
    /* Enable GPIOC clock */
    RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;
    
    /* Configure PC13 as output push-pull, 2MHz */
    uint32_t shift = (LED_PIN - 8) * 4;
    GPIOC_CRH = (GPIOC_CRH & ~(0xF << shift)) | (0x2 << shift);
}

/* Toggle LED */
static void led_toggle(void)
{
    GPIOC_ODR ^= (1 << LED_PIN);
}

int main(void)
{
    led_init();
    
    /* Blink pattern: 3 quick blinks then pause
     * This is visually distinct from bootloader's pattern
     */
    while (1) {
        /* Three quick blinks */
        for (int i = 0; i < 3; i++) {
            led_toggle();
            delay(100000);
            led_toggle();
            delay(100000);
        }
        
        /* Longer pause */
        delay(500000);
    }
    
    return 0;
}

