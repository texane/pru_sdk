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

volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* 1D Transfer Parameters */
typedef struct{
	uint32_t src;
	uint32_t dst;
	uint32_t cnt;
} mem1DParams;

/* Mapping Constant table register to variable */
volatile pruCfg CT_CFG __attribute__((cregister("CFG", near), peripheral));

/* Ensure pruCtrl structure is "placed" over top of CTRL registers */
#pragma DATA_SECTION(PRU0_CTRL, ".PRU0_CTRL");
far pruCtrl PRU0_CTRL;

mem1DParams buffer;

#define COPY_LENGTH	32
#define SIZE		64

void main(){
	mem1DParams params;
	uint8_t *srcPtr;
	uint8_t *dstPtr;
	uint8_t data;
	uint8_t srcBuf[SIZE];
	uint8_t dstBuf[SIZE];

	buffer.cnt = SIZE;
	buffer.src = &srcBuf;
	buffer.dst = &dstBuf;

	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* Configure C24 (PRU0 DRAM) and C25 (PRU1 DRAM) offsets as 0x0 */
	PRU0_CTRL.CTBIR0 = 0;
	PRU0_CTRL.CTBIR1 = 0;

	/* Load the MEM1D parameters */
	params = buffer;

	/* Copy the source and destination addresses into pointers.
	 * This allows a copy of the entirety of the buffer */
	srcPtr = ((uint8_t*)(params.src));
	dstPtr = ((uint8_t*)(params.dst));

	/* Continue copying until the count is 0 */
	while (params.cnt != 0){
		/* Load data from source pointer in DRAM into data buffer */
		data = *srcPtr;

		/* Copy data from data buffer into destination pointer in L3 */
		*dstPtr = data;

		/* Decrease the count */
		params.cnt -= COPY_LENGTH;

		/* Update source and destination pointers */
		params.src += COPY_LENGTH;
		params.dst += COPY_LENGTH;

		/* Store current state back into DRAM */
		buffer = params;
	}

	/* Halt PRU core */
	__halt();
}

