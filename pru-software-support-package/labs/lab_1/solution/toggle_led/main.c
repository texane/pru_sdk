#include <stdint.h>
#include <pru_cfg.h>

volatile register uint32_t __R30;

/* Mapping Constant table register to variable */
volatile pruCfg CT_CFG __attribute__((cregister("C4_CFG", near), peripheral));

/*
 * main.c
 */
int main(void) {
	volatile uint32_t gpo;

	/* GPI Mode 0, GPO Mode 0 */
	CT_CFG.GPCFG0 = 0;

	/* Clear GPO pins */
	gpo = 0x0000;
	
	while(1){
		gpo = __R30;
		gpo ^= 0xF;
		__R30 = gpo;
		__delay_cycles(100000000); // half-second delay
	}
}
