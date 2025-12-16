#ifndef FLASH_PROTECT_H
#define FLASH_PROTECT_H

#include "types.h"

/* Check if bootloader flash region is write-protected */
int is_bootloader_locked(void);

/* Enable write protection on bootloader region
 * This modifies option bytes and triggers a reset
 * Returns 0 on success, -1 on failure
 */
int lock_bootloader_region(void);

/* Check if read protection (RDP) is enabled */
int is_rdp_enabled(void);

/* Enable read protection level 1
 * WARNING: This prevents debugger access!
 * Returns 0 on success, -1 on failure
 */
int enable_rdp(void);

#endif /* FLASH_PROTECT_H */

