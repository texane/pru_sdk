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
#include <pru_ctrl.h>
#include <pru_intc.h>

volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Mapping Constant table register to variable */
volatile far pruIntc CT_INTC __attribute__((cregister("INTC", far), peripheral));
volatile pruCfg CT_CFG __attribute__((cregister("CFG", near), peripheral));
volatile far uint32_t CT_L3 __attribute__((cregister("L3OCMC", near), peripheral));
volatile far uint32_t CT_DDR __attribute__((cregister("DDR", near), peripheral));

/* PRU-to-ARM interrupt */
#define PRU0_ARM_INTERRUPT (19+16)

/* PRU0 Control Register structure */
#pragma DATA_SECTION(PRU0_CTRL, ".PRU0_CTRL");
volatile far pruCtrl PRU0_CTRL;

#define HOST_NUM	2
#define CHAN_NUM	2

void main(){
	uint32_t *pDdr = (uint32_t *) &CT_DDR;
	uint32_t score;

	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* Wait until receipt of interrupt on host 0 */
	while((__R31 & 0x40000000) == 0){
	}

	/* Clear system event in SECR1 */
	CT_INTC.SECR1 = 0x1;

	/* Clear system event enable in ECR1 */
	CT_INTC.ECR1 = 0x1;

	/* Point C30 (L3) to 0x3000 offset and C31 (DDR) to 0x0 offset */
	PRU0_CTRL.CTPPR1 = 0x00003000;

	/* Load value from DDR, decrement, and store it in L3 */
	score = pDdr[0];
	score--;
	CT_L3 = score;

	/* Halt PRU core */
	__halt();
}

