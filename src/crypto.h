#ifndef CRYPTO_H
#define CRYPTO_H

#include "types.h"

/* We use secp256r1 (P-256) ECDSA signatures
 * Signature is 64 bytes (r || s, each 32 bytes)
 * Public key is 64 bytes (x || y, each 32 bytes, uncompressed without 0x04 prefix)
 */

#define ECDSA_SIG_SIZE      64
#define ECDSA_PUBKEY_SIZE   64
#define SHA256_DIGEST_LEN   32

/* Verify ECDSA signature over SHA256 hash of data
 * Returns 1 if signature is valid, 0 otherwise
 */
int crypto_verify_signature(const u8 *signature, u32 sig_len,
                            const u8 *data, u32 data_len);

/* Compute SHA256 hash
 * Output buffer must be 32 bytes
 */
void crypto_sha256(const u8 *data, u32 len, u8 *hash_out);

/* Get pointer to embedded public key */
const u8 *crypto_get_pubkey(void);

#endif /* CRYPTO_H */
