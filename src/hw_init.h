#ifndef HW_INIT_H
#define HW_INIT_H

#include "types.h"

/* Initialize system clocks (HSI @ 8MHz for simplicity) */
void hw_clock_init(void);

/* Initialize debug LED on PC13 (active low on Blue Pill) */
void hw_led_init(void);

/* LED control */
void hw_led_on(void);
void hw_led_off(void);
void hw_led_toggle(void);

/* Simple blocking delay (not accurate, just for debug blinking) */
void hw_delay_ms(u32 ms);

/* Full hardware init */
void hw_init(void);

#endif /* HW_INIT_H */

