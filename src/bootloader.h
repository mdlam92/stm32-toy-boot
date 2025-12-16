#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "types.h"

/* Application header magic number */
#define APP_MAGIC       0xDEADBEEFUL

/* Header size must be 256 bytes */
#define APP_HEADER_SIZE 256

/* Flash layout */
#define BOOTLOADER_START    0x08000000UL
#define BOOTLOADER_SIZE     0x4000UL        /* 16KB */
#define APP_HEADER_ADDR     0x08004000UL
#define APP_IMAGE_ADDR      0x08004100UL    /* Header + 256 bytes */
#define FLASH_END           0x08010000UL    /* 64KB total flash */

/* Maximum application size */
#define APP_MAX_SIZE        (FLASH_END - APP_IMAGE_ADDR)

/* Signature size (ECDSA P-256: r || s, 64 bytes) */
#define SIGNATURE_SIZE      64

/* Application header structure
 * This is placed at APP_HEADER_ADDR and describes the application image
 */
typedef struct PACKED {
    u32 magic;              /* Must be APP_MAGIC (0xDEADBEEF) */
    u32 version;            /* Application version (semantic, packed) */
    u32 app_size;           /* Size of application binary in bytes */
    u32 entry_point;        /* Entry point address (usually reset vector) */
    u32 load_addr;          /* Load address (should match APP_IMAGE_ADDR) */
    u8  signature[SIGNATURE_SIZE];  /* ECDSA P-256 signature over app binary */
    u8  reserved[172];      /* Pad to 256 bytes total */
} app_header_t;

_Static_assert(sizeof(app_header_t) == APP_HEADER_SIZE, 
               "app_header_t must be exactly 256 bytes");

/* Boot status codes */
typedef enum {
    BOOT_OK = 0,
    BOOT_ERR_NO_APP,        /* No valid application found */
    BOOT_ERR_BAD_MAGIC,     /* Invalid magic number */
    BOOT_ERR_BAD_SIZE,      /* Invalid size field */
    BOOT_ERR_BAD_ADDR,      /* Invalid load/entry address */
    BOOT_ERR_BAD_SIG,       /* Signature verification failed */
    BOOT_ERR_NOT_LOCKED,    /* Bootloader region not protected */
} boot_status_t;

/* Main bootloader entry point
 * Validates application and jumps to it if valid
 * Returns error code if boot fails (should not return on success)
 */
boot_status_t bootloader_run(void);

/* Validate application header
 * Checks magic, size bounds, and address validity
 */
boot_status_t validate_header(const app_header_t *hdr);

/* Verify application signature
 * Returns BOOT_OK if signature is valid
 */
boot_status_t verify_signature(const app_header_t *hdr, const u8 *app_data);

/* Jump to application
 * Sets up stack and jumps to entry point
 * Does not return
 */
NORETURN void boot_application(const app_header_t *hdr);

#endif /* BOOTLOADER_H */
