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
#include <pru_intc.h>
#include <pru_ctrl.h>
#include <pru_ecap.h>
#include "PRU_1wire.h"
#include <string.h>

volatile register uint32_t __R30;
volatile register uint32_t __R31;
	
/* Mapping Constant table register to variable */
volatile far pruIntc CT_INTC __attribute__((cregister("PRU_INTC", far), peripheral));
volatile pruCfg CT_CFG __attribute__((cregister("PRU_CFG", near), peripheral));
volatile pruEcap CT_ECAP __attribute__((cregister("PRU_ECAP", near), peripheral));

/******************************************************************************
**                           INTERNAL FUNCTION DEFINITIONS
******************************************************************************/
void intc_config(void);
void pwm_config(unsigned int time_interval);

	
/* PRU peripheral system event */
#define PRU_IEP_EVT      7
#define PRU_ECAP_EVT    15
/* PRU-to-PRU system event */
#define PRU_SLAVE_MASTER_EVT   20
#define PRU_MASTER_SLAVE_EVT   21
#define PRU_SLAVE_MASTER_EVT_TRIGGER  (__R31 = (PRU_SLAVE_MASTER_EVT - 16) | (1 << 5))
#define PRU_MASTER_SLAVE_EVT_TRIGGER  (__R31 = (PRU_MASTER_SLAVE_EVT - 16) | (1 << 5))


#define GPIO_BLUE	(1 << 3)
#define GPIO_RED	(1 << 5)

#define time 3

#pragma DATA_SECTION(TEMP_SENSOR_BUF, ".TEMP_SENSOR_BUF")
volatile far ds18b20_regs TEMP_SENSOR_BUF;

void main(){
	unsigned int int_val;
	unsigned int prev_temp, curr_temp = 0;

	/* Configure INTC */
	intc_config();
	
	/* Configure timer or PWM */
	pwm_config(time);

	/* Clear all GPO values */
	__R30 = 0x00000000;

	/* Detect interrupt from Slave PRU that configuration complete */
	while((__R31 & 0x80000000) == 0){
	}

	/* Clear system event */
	CT_INTC.SECR0 = (1 << PRU_SLAVE_MASTER_EVT);
	
	/* Kick off PWM timer */	
	CT_ECAP.ECCTL2 |= 0x10; // Run counter (TSCTRSTOP = 1)

	while(1){

		/* Detect interrupt */
		while((__R31 & 0x80000000) == 0){
	        }

		/* Identify highest priority event */
		int_val = CT_INTC.HIPIR1;

		/* PRU eCAP event */
		if(int_val == PRU_ECAP_EVT){

			/* Interrupt PRU slave to issue read temp */
			PRU_MASTER_SLAVE_EVT_TRIGGER;

			/* This line was added to enable setting a breakpoint in the the PRU slave event service routine */
			CT_ECAP.ECCTL2 &= 0xFFEF; // Stop counter (TSCTRSTOP = 0)

			/* Clear system event */
			CT_ECAP.ECCLR |= 0x40;	// (PRDEQ = 1)
			CT_ECAP.ECCLR |= 0x1;	// (INT = 1)
			CT_INTC.SECR0 = (1 << PRU_ECAP_EVT);

		}

		/* PRU slave event */
		if(int_val == PRU_SLAVE_MASTER_EVT){

			/* Clear system event */
			CT_INTC.SECR0 = (1 << PRU_SLAVE_MASTER_EVT);

			/* Read temperature value in Shared RAM */
			curr_temp = (((unsigned int)TEMP_SENSOR_BUF.MSB << 8) | (unsigned int)TEMP_SENSOR_BUF.LSB); 

			/* Compare current temp with previous temp */
			/* Temperature rising */
			if(curr_temp > prev_temp){
				/* Set red LED */
				__R30 &= ~(GPIO_BLUE);
				__R30 ^= GPIO_RED;
			}
			/* Temperature falling */
			if(curr_temp < prev_temp){
				/* Set blue LED */
				__R30 &= ~(GPIO_RED);
				__R30 ^= GPIO_BLUE;
			}
			/* Temperature constant */
			if(curr_temp == prev_temp){
				/* Turn off all LEDs */
				__R30 &= ~(GPIO_RED);
				__R30 &= ~(GPIO_BLUE);
			}

			/* Store current temp as previous temp */
			prev_temp = curr_temp;

			/* This line was added to enable setting a breakpoint in the the PRU slave event service routine */
			CT_ECAP.CAP1_bit.CAP1 = time * 200000000;
			CT_ECAP.ECCTL2 |= 0x10; // Run counter (TSCTRSTOP = 1)
	
		}
	}
}

void intc_config(void){

	/* Clear Channel & Host Map Registers */
	CT_INTC.CMR1 = 0x00000000;
	CT_INTC.CMR3 = 0x00000000;
	CT_INTC.CMR5 = 0x00000000;
	CT_INTC.HMR0 = 0x00000000;

	/* Map system event to channel */
	CT_INTC.CMR1_bit.CH_MAP_7  = 0x0;	// PRU_IEP_EVT   --> Channel 0
	CT_INTC.CMR3_bit.CH_MAP_15 = 0x1;	// PRU_ECAP_EVT  --> Channel 1
	CT_INTC.CMR5_bit.CH_MAP_20 = 0x1;	// PRU_SLAVE_MASTER_EVT --> Channel 1
	CT_INTC.CMR5_bit.CH_MAP_21 = 0x0;	// PRU_MASTER_SLAVE_EVT --> Channel 0

	/* Map channel to host interrupt */
	CT_INTC.HMR0_bit.HINT_MAP_0 = 0x0;	// Channel 0 --> Host 0
	CT_INTC.HMR0_bit.HINT_MAP_1 = 0x1;	// Channel 1 --> Host 1

	/* Ensure system events are cleared */
	CT_INTC.SECR0 = 0x00308080;

	/* Enable system events */
	CT_INTC.ESR0 = 0x00308080;
	
	/* Enable host interrupts */
	CT_INTC.HIER = 0x3;

	/* Globally enable host interrupts */
	CT_INTC.GER = 0x1;

}

void pwm_config(unsigned int time_interval){

	/* Initialize PRU ECAP for APWM mode */
	CT_ECAP.CAP1_bit.CAP1 = time_interval * 200000000;    // APRD active register
	CT_ECAP.ECCTL2 |= 0x200; 	// APWM mode
	CT_ECAP.ECEINT |= 0x40;     // Enable PRDEQ interrupt source (PRDEQ = 1)

}
