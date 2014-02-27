// TODO: program pin mx register
// TODO: enable ECAP clock (ECAP_CLK_EN)


#include <pru.h>
#include <pru_macros.hp>

// ECAP register offsets, when in APWM mode

#define APWM_REG_TSCTR 0x00
#define APWM_REG_CTRPHS 0x04
#define APWM_REG_APRD 0x08
#define APWM_REG_ACMP 0x0c
#define APWM_REG_APRD_SHADOW 0x10
#define APWM_REG_ACMP_SHADOW 0x14
#define APWM_REG_ECCTL1 0x28
#define APWM_REG_ECCTL2 0x2a
#define APWM_REG_REVID 0x5c


.origin 0
.entrypoint MAIN

MAIN:

LBCO r0, CONST_PRUCFG, 4, 4
CLR r0, r0, 4
SBCO r0, CONST_PRUCFG, 4, 4

// setup ecap in apwm mode

and r0, r0, 0
SBCO r0, CONST_ECAP, APWM_REG_TSCTR, 4

and r0, r0, 0
SBCO r0, CONST_ECAP, APWM_REG_CTRPHS, 4

MOV32 r0, 0x100000
SBCO r0, CONST_ECAP, APWM_REG_APRD, 4

MOV32 r0, 0x80000
SBCO r0, CONST_ECAP, APWM_REG_ACMP, 4

MOV32 r0, (((1 << 9) | (3 << 6) | (1 << 5) | (1 << 4)) << 16)
SBCO r0, CONST_ECAP, APWM_REG_ECCTL1, 4

// tell cpu we are done 
MOV r31.b0, PRU0_ARM_INTERRUPT + 16
HALT
