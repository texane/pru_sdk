#include <pru.h>
#include <pru_macros.hp>


// am335xPruReferenceGuide, table 25
#define PRUSS_PRU_CTRL_REG_WAKEUP_EN 0x08

// am335xPruReferenceGuide, table 190
// offsets from CONST_IEP
#define IEP_REG_GLOBAL_CFG 0x00
#define IEP_REG_GLOBAL_STATUS 0x04
#define IEP_REG_COMPEN 0x08
#define IEP_REG_COUNT 0x0c
#define IEP_REG_CMP_CFG 0x40
#define IEP_REG_CMP_STATUS 0x44
#define IEP_REG_CMP0 0x48


.origin 0
.entrypoint MAIN

MAIN:

LBCO r0, CONST_PRUCFG, 4, 4
CLR r0, r0, 4
SBCO r0, CONST_PRUCFG, 4, 4


//
// prepare PRU shared memory access

MOV r0, 0x000000120
MOV r1, CTPPR_0
ST32 r0, r1

MOV r0, 0x00100000
MOV r1, CTPPR_1
ST32 r0, r1


//
// setup IEP module

// enable timer clock PRU_ICSS_CFG.CGR.IEP_CLK_EN=1
LBCO r0, CONST_PRUCFG, 0x10, 4                    
SET r0, 17
SBCO r0, CONST_PRUCFG, 0x10, 4   

MOV r0, (1 << 8) | (1 << 4)
SBCO r0, CONST_IEP, IEP_REG_GLOBAL_CFG, 4

MOV r0, 0
SBCO r0, CONST_IEP, IEP_REG_COMPEN, 4

MOV r0, 0
SBCO r0, CONST_IEP, IEP_REG_COUNT, 4

MOV r0, (1 << 1) | (1 << 0)
SBCO r0, CONST_IEP, IEP_REG_CMP_CFG, 4

// timer comparison value. 200000000 is 1 second.
MOV r0, 200000000
SBCO r0, CONST_IEP, IEP_REG_CMP0, 4


//
// enable IEP interrupt source
// TODO: route interrupt controller channel 7
// TODO: and use sleep instruction instead of polling
// TODO: am335xPruReferenceGuide, section 6.3
// TODO: http://elinux.org/PRUSSv2_Interrupt_Controller
//
// MOV r0, 1 << 7
// MOV r1, CTRL_WAKEUP_EN
// ST32 r0, r1


//
// start IEP free running counter

LBCO r0, CONST_IEP, IEP_REG_GLOBAL_CFG, 4
SET r0, 0
SBCO r0, CONST_IEP, IEP_REG_GLOBAL_CFG, 4


//
// r4 starts with 0, and incremented by 1 every IEP event

MOV r4, 0
MOV r5, 0
JMP DO_STORE


//
// wait for IEP events

IEP_WAIT_EVENT:
// TODO: cf enable IEP interrupt source TODO, then use SLP 1
LBCO r2, CONST_IEP, IEP_REG_CMP_STATUS, 4
QBBC IEP_WAIT_EVENT, r2, 0
SBCO 0, CONST_IEP, IEP_REG_CMP_STATUS, 4

// increment and store uint32_t into shared ram (4 bytes)
ADD r4, r4, 1
ADD r5, r5, 2
DO_STORE:
SBCO r4, CONST_PRUSHAREDRAM, 0, 8

JMP IEP_WAIT_EVENT


//
// never reached

MOV r31.b0, PRU0_ARM_INTERRUPT + 16
HALT
