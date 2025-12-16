#ifndef STM32F1XX_H
#define STM32F1XX_H

#include "types.h"

/* ============== Base Addresses ============== */

#define FLASH_BASE      0x08000000UL
#define SRAM_BASE       0x20000000UL
#define PERIPH_BASE     0x40000000UL

/* APB1 */
#define APB1_BASE       (PERIPH_BASE + 0x00000000UL)

/* APB2 */
#define APB2_BASE       (PERIPH_BASE + 0x00010000UL)
#define GPIOA_BASE      (APB2_BASE + 0x0800UL)
#define GPIOB_BASE      (APB2_BASE + 0x0C00UL)
#define GPIOC_BASE      (APB2_BASE + 0x1000UL)

/* AHB */
#define AHB_BASE        (PERIPH_BASE + 0x00020000UL)
#define RCC_BASE        (AHB_BASE + 0x1000UL)
#define FLASH_R_BASE    (AHB_BASE + 0x2000UL)

/* Cortex-M3 internal */
#define SCS_BASE        0xE000E000UL
#define SCB_BASE        (SCS_BASE + 0x0D00UL)
#define NVIC_BASE       (SCS_BASE + 0x0100UL)

/* Debug registers */
#define DBGMCU_BASE     0xE0042000UL

/* ============== RCC Registers ============== */

typedef struct {
    reg32_t CR;         /* Clock control */
    reg32_t CFGR;       /* Clock configuration */
    reg32_t CIR;        /* Clock interrupt */
    reg32_t APB2RSTR;   /* APB2 peripheral reset */
    reg32_t APB1RSTR;   /* APB1 peripheral reset */
    reg32_t AHBENR;     /* AHB peripheral enable */
    reg32_t APB2ENR;    /* APB2 peripheral enable */
    reg32_t APB1ENR;    /* APB1 peripheral enable */
    reg32_t BDCR;       /* Backup domain control */
    reg32_t CSR;        /* Control/status */
} RCC_TypeDef;

#define RCC     ((RCC_TypeDef *)RCC_BASE)

/* RCC_CR bits */
#define RCC_CR_HSION        BIT(0)
#define RCC_CR_HSIRDY       BIT(1)
#define RCC_CR_HSEON        BIT(16)
#define RCC_CR_HSERDY       BIT(17)
#define RCC_CR_PLLON        BIT(24)
#define RCC_CR_PLLRDY       BIT(25)

/* RCC_CFGR bits */
#define RCC_CFGR_SW_HSI     0x00
#define RCC_CFGR_SW_HSE     0x01
#define RCC_CFGR_SW_PLL     0x02
#define RCC_CFGR_SWS_HSI    0x00
#define RCC_CFGR_SWS_HSE    0x04
#define RCC_CFGR_SWS_PLL    0x08

/* RCC_APB2ENR bits */
#define RCC_APB2ENR_IOPAEN  BIT(2)
#define RCC_APB2ENR_IOPBEN  BIT(3)
#define RCC_APB2ENR_IOPCEN  BIT(4)
#define RCC_APB2ENR_AFIOEN  BIT(0)

/* ============== GPIO Registers ============== */

typedef struct {
    reg32_t CRL;    /* Port config low (pins 0-7) */
    reg32_t CRH;    /* Port config high (pins 8-15) */
    reg32_t IDR;    /* Input data */
    reg32_t ODR;    /* Output data */
    reg32_t BSRR;   /* Bit set/reset */
    reg32_t BRR;    /* Bit reset */
    reg32_t LCKR;   /* Lock */
} GPIO_TypeDef;

#define GPIOA   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *)GPIOC_BASE)

/* GPIO config values (4 bits per pin) */
#define GPIO_MODE_INPUT     0x0
#define GPIO_MODE_OUT_10MHZ 0x1
#define GPIO_MODE_OUT_2MHZ  0x2
#define GPIO_MODE_OUT_50MHZ 0x3

#define GPIO_CNF_OUT_PP     0x0  /* Push-pull */
#define GPIO_CNF_OUT_OD     0x1  /* Open-drain */
#define GPIO_CNF_AF_PP      0x2  /* Alternate function push-pull */
#define GPIO_CNF_AF_OD      0x3  /* Alternate function open-drain */

#define GPIO_CNF_IN_ANALOG  0x0
#define GPIO_CNF_IN_FLOAT   0x1
#define GPIO_CNF_IN_PUPD    0x2

/* ============== Flash Registers ============== */

typedef struct {
    reg32_t ACR;        /* Access control */
    reg32_t KEYR;       /* Key */
    reg32_t OPTKEYR;    /* Option key */
    reg32_t SR;         /* Status */
    reg32_t CR;         /* Control */
    reg32_t AR;         /* Address */
    reg32_t RESERVED;
    reg32_t OBR;        /* Option byte */
    reg32_t WRPR;       /* Write protection */
} FLASH_TypeDef;

#define FLASH_IF    ((FLASH_TypeDef *)FLASH_R_BASE)

/* Flash keys */
#define FLASH_KEY1      0x45670123UL
#define FLASH_KEY2      0xCDEF89ABUL
#define FLASH_OPTKEY1   0x45670123UL
#define FLASH_OPTKEY2   0xCDEF89ABUL

/* FLASH_ACR bits */
#define FLASH_ACR_LATENCY_0     0x0
#define FLASH_ACR_LATENCY_1     0x1
#define FLASH_ACR_LATENCY_2     0x2
#define FLASH_ACR_PRFTBE        BIT(4)

/* FLASH_SR bits */
#define FLASH_SR_BSY            BIT(0)
#define FLASH_SR_PGERR          BIT(2)
#define FLASH_SR_WRPRTERR       BIT(4)
#define FLASH_SR_EOP            BIT(5)

/* FLASH_CR bits */
#define FLASH_CR_PG             BIT(0)
#define FLASH_CR_PER            BIT(1)
#define FLASH_CR_MER            BIT(2)
#define FLASH_CR_OPTPG          BIT(4)
#define FLASH_CR_OPTER          BIT(5)
#define FLASH_CR_STRT           BIT(6)
#define FLASH_CR_LOCK           BIT(7)
#define FLASH_CR_OPTWRE         BIT(9)

/* FLASH_OBR bits */
#define FLASH_OBR_OPTERR        BIT(0)
#define FLASH_OBR_RDPRT         BIT(1)

/* ============== SCB Registers ============== */

typedef struct {
    reg32_t CPUID;      /* CPU ID */
    reg32_t ICSR;       /* Interrupt control state */
    reg32_t VTOR;       /* Vector table offset */
    reg32_t AIRCR;      /* Application interrupt/reset control */
    reg32_t SCR;        /* System control */
    reg32_t CCR;        /* Configuration control */
    reg32_t SHPR1;      /* System handler priority 1 */
    reg32_t SHPR2;      /* System handler priority 2 */
    reg32_t SHPR3;      /* System handler priority 3 */
    reg32_t SHCSR;      /* System handler control/state */
    reg32_t CFSR;       /* Configurable fault status */
    reg32_t HFSR;       /* Hard fault status */
    reg32_t DFSR;       /* Debug fault status */
    reg32_t MMFAR;      /* MemManage fault address */
    reg32_t BFAR;       /* Bus fault address */
    reg32_t AFSR;       /* Auxiliary fault status */
} SCB_TypeDef;

#define SCB     ((SCB_TypeDef *)SCB_BASE)

/* AIRCR key for write access */
#define SCB_AIRCR_VECTKEY       0x05FA0000UL
#define SCB_AIRCR_SYSRESETREQ   BIT(2)

/* ============== Debug MCU Registers ============== */

typedef struct {
    reg32_t IDCODE;     /* MCU device ID */
    reg32_t CR;         /* Debug MCU config */
} DBGMCU_TypeDef;

#define DBGMCU  ((DBGMCU_TypeDef *)DBGMCU_BASE)

/* DBGMCU_CR bits */
#define DBGMCU_CR_DBG_SLEEP     BIT(0)
#define DBGMCU_CR_DBG_STOP      BIT(1)
#define DBGMCU_CR_DBG_STANDBY   BIT(2)

/* ============== Option Bytes ============== */

#define OB_BASE         0x1FFFF800UL

typedef struct {
    reg32_t RDP;        /* Read protection + USER */
    reg32_t DATA;       /* Data0 + Data1 */
    reg32_t WRP01;      /* Write protection pages 0-15 */
    reg32_t WRP23;      /* Write protection pages 16-31 */
} OB_TypeDef;

#define OB      ((OB_TypeDef *)OB_BASE)

/* Read protection levels */
#define OB_RDP_LEVEL_0  0x5AA5  /* No protection */
#define OB_RDP_LEVEL_1  0x0000  /* Read protection active */

/* ============== Utility Macros ============== */

/* System reset */
static inline void NVIC_SystemReset(void)
{
    __DSB();
    SCB->AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
    __DSB();
    while(1);
}

#endif /* STM32F1XX_H */

