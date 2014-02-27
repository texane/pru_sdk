#include <pru.h>
#include <pru_macros.hp>

#define ADC_MIO_ADDR 0x44e0d000
#define ADC_REG_FIFO0COUNT 0xe4
#define ADC_REG_FIFO0DATA 0x100

.origin 0
.entrypoint MAIN

MAIN:

LBCO r0, CONST_PRUCFG, 4, 4
CLR r0, r0, 4
SBCO r0, CONST_PRUCFG, 4, 4

// to access prusharedram
MOV r0, 0x000000120
MOV r1, CTPPR_0
ST32 r0, r1
MOV r0, 0x00100000
MOV r1, CTPPR_1
ST32 r0, r1

// wait until data available in fifo
WAIT_DATA:
MOV r1, ADC_MIO_ADDR + ADC_REG_FIFO0COUNT
LBBO r0, r1, 0, 4
AND r0, r0, (1 << 7) - 1
QBEQ WAIT_DATA, r0, 0

// read fifo
MOV r1, ADC_MIO_ADDR + ADC_REG_FIFO0DATA
LBBO r0, r1, 0, 4
MOV r1, (1 << 12) - 1
AND r0, r0, r1

// store uint32_t into shared ram (4 bytes)
SBCO r0, CONST_PRUSHAREDRAM, 0, 4

MOV r31.b0, PRU0_ARM_INTERRUPT + 16
HALT
