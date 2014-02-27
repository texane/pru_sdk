#include <pru.h>
#include <pru_macros.hp>

.origin 0
.entrypoint MAIN

MAIN:

LBCO r0, CONST_PRUCFG, 4, 4
CLR r0, r0, 4
SBCO r0, CONST_PRUCFG, 4, 4

MOV r0, 0x000000120
MOV r1, CTPPR_0
ST32 r0, r1

MOV r0, 0x00100000
MOV r1, CTPPR_1
ST32 r0, r1

// load external ddr into r0,r1
LBCO r0, CONST_DDR, 0, 8

// mac, multiply only mode
// r27:r26 = r28 * r29
// am335xPruReferenceGuide, figure 11
// r25: mac mode, status
// r26: lower product
// r27: upper product
// r28: operand
// r29: operand
MOV r28, r0
MOV r29, r1
MOV r25, 0
XOUT 0, r25, 1

// load result in r26:r27 (8 bytes)
XIN 0, r26, 8

// store uint64_t into shared ram (8 bytes)
SBCO r26, CONST_PRUSHAREDRAM, 0, 8

MOV r31.b0, PRU0_ARM_INTERRUPT + 16
HALT
