/* Cryptographic Operations
 * 
 * Uses micro-ecc for ECDSA (secp256r1) and Brad Conte's SHA256.
 * This is real cryptography, not a stub.
 */

#include "crypto.h"
#include "bootloader.h"
#include "uECC.h"
#include "sha256.h"

/* 
 * Embedded public key for signature verification (secp256r1 / P-256)
 * This is the raw 64-byte public key (x || y coordinates, no 0x04 prefix)
 * 
 * Generate a new keypair with:
 *   python3 tools/genkey.py
 * 
 * Then update this array with the output.
 */
static const u8 public_key[64] = {
    /* X coordinate (32 bytes) */
    0x0d, 0xbb, 0xcd, 0x7c, 0x03, 0x94, 0x5f, 0x6d,
    0x23, 0x38, 0x50, 0x06, 0x1c, 0x4e, 0x57, 0x01,
    0x5c, 0x26, 0x5d, 0xf8, 0x15, 0x63, 0x88, 0x0a,
    0x41, 0xf3, 0xea, 0xf1, 0x59, 0x02, 0x64, 0x83,
    /* Y coordinate (32 bytes) */
    0x10, 0xd2, 0x33, 0xbb, 0xfb, 0xf0, 0xdd, 0x92,
    0x61, 0x38, 0x85, 0x94, 0x7f, 0x71, 0x33, 0xef,
    0xaf, 0x9f, 0x06, 0x96, 0x28, 0x6f, 0xf9, 0x2f,
    0x08, 0xf8, 0xe8, 0x96, 0xcf, 0x68, 0x8e, 0xa1,
};

/* Verify ECDSA-P256 signature over SHA256 hash of data */
int crypto_verify_signature(const u8 *signature, u32 sig_len,
                            const u8 *data, u32 data_len)
{
    u8 hash[32];
    SHA256_CTX ctx;
    uECC_Curve curve = uECC_secp256r1();
    
    /* Validate signature length */
    if (sig_len != ECDSA_SIG_SIZE) {
        return 0;
    }
    
    if (data_len == 0) {
        return 0;
    }
    
    /* Compute SHA256 hash of the data */
    sha256_init(&ctx);
    sha256_update(&ctx, data, data_len);
    sha256_final(&ctx, hash);
    
    /* Verify ECDSA signature */
    return uECC_verify(public_key, hash, sizeof(hash), signature, curve);
}

/*
 * Compute SHA256 hash
 */
void crypto_sha256(const u8 *data, u32 len, u8 *hash_out)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, hash_out);
}

/*
 * Get pointer to embedded public key
 */
const u8 *crypto_get_pubkey(void)
{
    return public_key;
}
