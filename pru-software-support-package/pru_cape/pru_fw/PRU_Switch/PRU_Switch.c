/*
 * Copyright (c) 2014, Texas Instruments, Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. */


#include <stdint.h>
#include <pru_cfg.h>

volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Mapping Constant table register to variable */
volatile pruCfg	CT_CFG __attribute__((cregister("PRU_CFG",near) , peripheral));

#define SW1		(1 << 5)		/* SW1 offset */
#define SW2		(1 << 7)		/* SW2 offset */
#define SW1_2	(SW1 | SW2)	/* SW1 & SW2 offset */

#define LED_BLUE	(1 << 0)
#define LED_GREEN	(1 << 1)
#define LED_ORANGE	(1 << 2)
#define LED_RED 	(1 << 3)
#define LED_Group1	(LED_BLUE | LED_GREEN)
#define LED_Group2	(LED_ORANGE | LED_RED)

void main()
{
	/* Configure GPI and GPO as Mode 0 (Direct Connect) */
    CT_CFG.GPCFG0 = 0x0000;
	
    /* Turn on all LEDs */
	__R30 = (LED_Group1 | LED_Group2);

	/* TODO: Create stop condition, else it will toggle indefinitely */
	while(1)
	{

		/* Both Switch 1 & 2 are pressed */
		if( (__R31 & SW1_2) == 0)
			__R30 = (LED_Group1 | LED_Group2);

		/* Switch 1 is pressed */
		else if( (__R31 & SW1) != SW1)
			__R30 = LED_Group1;// & ~LED_Group2;

		/* Switch 2 is pressed */
		else if( (__R31 & SW2) != SW2)
			__R30 = LED_Group2;// & ~(LED_Group2);

		/* Neither Switch 1/2 are pressed */
		else
			__R30 = ~(LED_Group1 | LED_Group2);

	}
	
	/* Halt the PRU core - shouldn't get here */
	__halt();
}
