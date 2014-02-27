#include <pru.h>
#include <pru_macros.hp>
#include "epwm.hp"
#include "timer.hp"
#include "adc.hp"
#include "mac.hp"


.origin 0
.entrypoint MAIN


MAIN:

 /* enable ocp wide accesses */

 LBCO r0, CONST_PRUCFG, 4, 4
 CLR r0, r0, 4
 SBCO r0, CONST_PRUCFG, 4, 4

 /* prepared pru to host shared memory */

 MOV r0, 0x000000120
 MOV r1, CTPPR_0
 ST32 r0, r1

 MOV r0, 0x00100000
 MOV r1, CTPPR_1
 ST32 r0, r1

 /* load shared values into r2,r3 ... r9 */

 LBCO r2, CONST_DDR, 0, 32

 /* setup epwm, adc and timer */

 EPWM_SETUP 1
 EPWM_SET_PERIOD 1, r2

 ADC_SETUP 0

 TIMER_SETUP 200000000

 /* main loop */

 WHILE_TRUE:
 EPWM_SET_DUTY 1, r3
 TIMER_WAIT
 ADC_READ 0
 JMP WHILE_TRUE

 /* store results from r2,r3 ... r9 into host memory */

 SBCO r2, CONST_PRUSHAREDRAM, 0, 32

 /* signal cpu we are done (never reached) */

 MOV r31.b0, PRU0_ARM_INTERRUPT + 16
 HALT
