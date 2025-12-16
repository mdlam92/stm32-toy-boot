#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;

typedef volatile uint32_t reg32_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

#define BIT(n)          (1UL << (n))
#define ARRAY_SIZE(x)   (sizeof(x) / sizeof((x)[0]))

/* Memory barriers */
#define __DSB()  __asm__ volatile ("dsb" ::: "memory")
#define __ISB()  __asm__ volatile ("isb" ::: "memory")
#define __DMB()  __asm__ volatile ("dmb" ::: "memory")

/* Compiler hints */
#define UNUSED(x)       ((void)(x))
#define PACKED          __attribute__((packed))
#define ALIGNED(n)      __attribute__((aligned(n)))
#define NORETURN        __attribute__((noreturn))
#define WEAK            __attribute__((weak))
#define SECTION(s)      __attribute__((section(s)))

#endif /* TYPES_H */

