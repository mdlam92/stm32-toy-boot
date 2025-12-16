/* micro-ecc configuration for STM32F103 bootloader
 * Include this BEFORE uECC.h to configure the library
 */

#ifndef UECC_CONFIG_H
#define UECC_CONFIG_H

/* We're on ARM Cortex-M3 with Thumb-2 */
#define uECC_PLATFORM uECC_arm_thumb2

/* 32-bit words */
#define uECC_WORD_SIZE 4

/* Only enable secp256r1 (P-256) - saves code space */
#define uECC_SUPPORTS_secp160r1 0
#define uECC_SUPPORTS_secp192r1 0
#define uECC_SUPPORTS_secp224r1 0
#define uECC_SUPPORTS_secp256r1 1
#define uECC_SUPPORTS_secp256k1 0

/* Optimization level - 2 is a good balance */
#define uECC_OPTIMIZATION_LEVEL 2

/* We don't need point compression */
#define uECC_SUPPORT_COMPRESSED_POINT 0

/* We don't need key generation in the bootloader (verify only) */
/* But micro-ecc doesn't have a separate config for this */

#endif /* UECC_CONFIG_H */

