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

/* Mapping Constant table registers to variables */
volatile far pruCfg CT_CFG __attribute__((cregister("CFG", near), peripheral));
volatile far uint32_t CT_DDR __attribute__((cregister("DDR", near), peripheral));

/* This is a char so that I can force access to R31.b0 for the host interrupt */
volatile register uint8_t __R31;

/* Definition of control register structures. */
#pragma DATA_SECTION(PRU0_CTRL, ".PRU0_CTRL");
volatile far pruCtrl PRU0_CTRL;

/* PRU-to-ARM interrupt */
#define PRU_ARM_INTERRUPT (19+16)

int main(){
	uint32_t *ddrPtr = (uint32_t *)&CT_DDR;
	uint32_t result;

	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* Ensure C31 is pointed to the start of DDR (0 offset) */
	PRU0_CTRL.CTPPR1_bit.C31_BLK_POINTER = 0;

	/* Read value from DDR, store in the PRU_CFG pin_mx register */
	result = *ddrPtr;
	CT_CFG.PIN_MX = result;

	/* Halt PRU core */
	__halt();
}
