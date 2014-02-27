#include <pru.h>
#include <pru_macros.hp>


#define CM_PER_MIO_ADDR 0x44e00000

#define CM_PER_EPWMSS0_CLKCTRL 0xd4
#define CM_PER_EPWMSS1_CLKCTRL 0xcc
#define CM_PER_EPWMSS2_CLKCTRL 0xd8

#define PWMSS0_MIO_ADDR 0x48300000
#define PWMSS1_MIO_ADDR 0x48302000
#define PWMSS2_MIO_ADDR 0x48304000

/* doc: spruh73c, section 15 */
#define PWMSS_REG_IDVER 0x00
#define PWMSS_REG_SYSCONFIG 0x04
#define PWMSS_REG_CLKCONFIG 0x08
#define PWMSS_REG_CLKSTATUS 0x0c

#define PWMSS_EPWM_REG_BASE 0x200

/* doc: spruh73c, table 15.59 */
/* warning: 16 bit registers */
#define EPWM_REG_TBCTL (PWMSS_EPWM_REG_BASE + 0x00)
#define EPWM_REG_TBSTS (PWMSS_EPWM_REG_BASE + 0x02)
#define EPWM_REG_TBHSHR (PWMSS_EPWM_REG_BASE + 0x04)
#define EPWM_REG_TBPHS (PWMSS_EPWM_REG_BASE + 0x06)
#define EPWM_REG_TBCNT (PWMSS_EPWM_REG_BASE + 0x08)
#define EPWM_REG_TBPRD (PWMSS_EPWM_REG_BASE + 0x0a)

/* doc: spruh73c, table 15.65 */
/* warning: 16 bit registers */
#define EPWM_REG_CMPCTL (PWMSS_EPWM_REG_BASE + 0x0e)
#define EPWM_REG_CMPAHR (PWMSS_EPWM_REG_BASE + 0x10)
#define EPWM_REG_CMPA (PWMSS_EPWM_REG_BASE + 0x12)
#define EPWM_REG_CMPB (PWMSS_EPWM_REG_BASE + 0x14)

/* doc: spruh73c, table 15.69 */
/* warning: 16 bit registers */
#define EPWM_REG_AQCTLA (PWMSS_EPWM_REG_BASE + 0x16)
#define EPWM_REG_AQCTLB (PWMSS_EPWM_REG_BASE + 0x18)
#define EPWM_REG_AQSFRC (PWMSS_EPWM_REG_BASE + 0x1a)
#define EPWM_REG_AQCSFRC (PWMSS_EPWM_REG_BASE + 0x1c)

/* doc: spruh73c, table 15.74 */
/* warning: 16 bit registers */
#define EPWM_REG_DBCTL (PWMSS_EPWM_REG_BASE + 0x1e)
#define EPWM_REG_DBRED (PWMSS_EPWM_REG_BASE + 0x20)
#define EPWM_REG_DBFED (PWMSS_EPWM_REG_BASE + 0x22)

/* doc: spruh73c, table 15.78 */
/* warning: 16 bit registers */
#define EPWM_REG_TZSEL (PWMSS_EPWM_REG_BASE + 0x24)
#define EPWM_REG_TZCTL (PWMSS_EPWM_REG_BASE + 0x28)
#define EPWM_REG_TZEINT (PWMSS_EPWM_REG_BASE + 0x2a)
#define EPWM_REG_TZFLG (PWMSS_EPWM_REG_BASE + 0x2c)
#define EPWM_REG_TZCLR (PWMSS_EPWM_REG_BASE + 0x2e)
#define EPWM_REG_TZFRC (PWMSS_EPWM_REG_BASE + 0x30)

/* doc: spruh73c, table 15.85 */
/* warning: 16 bit registers */
#define EPWM_REG_ETSEL (PWMSS_EPWM_REG_BASE + 0x32)
#define EPWM_REG_ETPS (PWMSS_EPWM_REG_BASE + 0x34)
#define EPWM_REG_ETFLG (PWMSS_EPWM_REG_BASE + 0x36)
#define EPWM_REG_ETCLR (PWMSS_EPWM_REG_BASE + 0x38)
#define EPWM_REG_ETFRC (PWMSS_EPWM_REG_BASE + 0x3a)

/* doc: spruh73c, table 15.91 */
/* warning: 16 bit registers */
#define EPWM_REG_PCCTL (PWMSS_EPWM_REG_BASE + 0x3c)


// procedure
//
// enable epwmss1 clocking
// mio_write_uint32(&mio, CM_PER_EPWMSS1_CLKCTRL, 2);
//
// enable epwm clock
// mio_or_uint32(mio, PWMSS_REG_CLKCONFIG, 1 << 8);
//
// mio_write_uint16(mio, EPWM_REG_TBCTL, (2 << 14) | (3 << 4));
// mio_write_uint16(mio, EPWM_REG_TBPHS, 0);
// mio_write_uint16(mio, EPWM_REG_TBPRD, 0xfa0);
// mio_write_uint16(mio, EPWM_REG_TBCNT, 0);
//
// mio_write_uint16(mio, EPWM_REG_CMPCTL, 0);
// mio_write_uint16(mio, EPWM_REG_CMPA, 0x7d0);
//
// mio_write_uint16(mio, EPWM_REG_AQCTLA, (3 << 4) | (2 << 0));
// mio_write_uint16(mio, EPWM_REG_AQCTLB, 0);
//
// mio_write_uint16(mio, EPWM_REG_TZSEL, 0);
// mio_write_uint16(mio, EPWM_REG_TZEINT, 0);
//
// mio_write_uint16(mio, EPWM_REG_ETSEL, 0);
// mio_write_uint16(mio, EPWM_REG_PCCTL, 0);


.origin 0
.entrypoint MAIN

MAIN:

LBCO r0, CONST_PRUCFG, 4, 4
CLR r0, r0, 4
SBCO r0, CONST_PRUCFG, 4, 4


// --

MOV32 r0, ((2 << 14) | (3 << 4))
MOV32 r1, EPWM_REG_TBCTL
SBCO r0, CONST_EHRPWM1, r1, 2

MOV32 r0, 0
MOV32 r1, EPWM_REG_TBPHS
SBCO r0, CONST_EHRPWM1, r1, 2

MOV32 r0, 0xfa0
MOV32 r1, EPWM_REG_TBPRD
SBCO r0, CONST_EHRPWM1, r1, 2

MOV32 r0, 0
MOV32 r1, EPWM_REG_TBCNT
SBCO r0, CONST_EHRPWM1, r1, 2


// --

MOV32 r0, 0
MOV32 r1, EPWM_REG_CMPCTL
SBCO r0, CONST_EHRPWM1, r1, 2

MOV32 r0, 0x7d0
MOV32 r1, EPWM_REG_CMPA
SBCO r0, CONST_EHRPWM1, r1, 2


// --

MOV32 r0, ((3 << 4) | (2 << 0))
MOV32 r1, EPWM_REG_AQCTLA
SBCO r0, CONST_EHRPWM1, r1, 2

MOV32 r0, 0
MOV32 r1, EPWM_REG_AQCTLB
SBCO r0, CONST_EHRPWM1, r1, 2


// --

MOV32 r0, 0
MOV32 r1, EPWM_REG_TZSEL
SBCO r0, CONST_EHRPWM1, r1, 2

MOV32 r0, 0
MOV32 r1, EPWM_REG_TZEINT
SBCO r0, CONST_EHRPWM1, r1, 2


// --

MOV32 r0, 0
MOV32 r1, EPWM_REG_ETSEL
SBCO r0, CONST_EHRPWM1, r1, 2

MOV32 r0, 0
MOV32 r1, EPWM_REG_PCCTL
SBCO r0, CONST_EHRPWM1, r1, 2


// tell cpu we are done 

MOV r31.b0, PRU0_ARM_INTERRUPT + 16
HALT
