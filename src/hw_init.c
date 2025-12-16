/* Hardware Initialization
 * Clock setup and debug LED
 */

#include "hw_init.h"
#include "stm32f1xx.h"

/* PC13 is the onboard LED on Blue Pill (active low) */
#define LED_PORT    GPIOC
#define LED_PIN     13

void hw_clock_init(void)
{
    /* Use HSI (8MHz internal RC) - keeps things simple
     * In production you'd configure HSE + PLL for 72MHz
     */
    
    /* Enable HSI (should already be on after reset) */
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));
    
    /* Select HSI as system clock */
    RCC->CFGR = (RCC->CFGR & ~0x3) | RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & 0x0C) != RCC_CFGR_SWS_HSI);
    
    /* Flash latency: 0 wait states for <= 24MHz */
    FLASH_IF->ACR = FLASH_ACR_LATENCY_0 | FLASH_ACR_PRFTBE;
}

void hw_led_init(void)
{
    /* Enable GPIOC clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    
    /* Configure PC13 as output push-pull, 2MHz */
    /* CRH handles pins 8-15, each pin uses 4 bits */
    /* PC13 is bits [23:20] in CRH */
    u32 config = (GPIO_CNF_OUT_PP << 2) | GPIO_MODE_OUT_2MHZ;
    u32 shift = (LED_PIN - 8) * 4;  /* Pin 13 -> shift 20 */
    
    LED_PORT->CRH = (LED_PORT->CRH & ~(0xF << shift)) | (config << shift);
    
    /* Start with LED off (pin high = LED off on Blue Pill) */
    hw_led_off();
}

void hw_led_on(void)
{
    /* Active low: clear pin to turn on */
    LED_PORT->BRR = BIT(LED_PIN);
}

void hw_led_off(void)
{
    /* Active low: set pin to turn off */
    LED_PORT->BSRR = BIT(LED_PIN);
}

void hw_led_toggle(void)
{
    LED_PORT->ODR ^= BIT(LED_PIN);
}

void hw_delay_ms(u32 ms)
{
    /* Very rough delay at 8MHz HSI
     * ~8000 cycles per ms, loop is ~4 cycles
     */
    volatile u32 count = ms * 2000;
    while (count--);
}

void hw_init(void)
{
    hw_clock_init();
    hw_led_init();
}

