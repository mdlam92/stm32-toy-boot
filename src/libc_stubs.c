/* Minimal libc stubs for freestanding environment
 * These are needed by micro-ecc and sha256
 */

#include "types.h"

void *memset(void *s, int c, size_t n)
{
    u8 *p = (u8 *)s;
    while (n--) {
        *p++ = (u8)c;
    }
    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    u8 *d = (u8 *)dest;
    const u8 *s = (const u8 *)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const u8 *p1 = (const u8 *)s1;
    const u8 *p2 = (const u8 *)s2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

