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

// ***************************************
// *    Global Structure Definitions     *
// ***************************************

#include <stdint.h>
#include <rsc_table_am335x_pru.h>

/* R31 is used to generate the "I'm done" back to the ARM */
volatile register uint8_t __R31;

/* Accessing the operands structure within the multiplyParams structure
 * forces the compiler to pair the registers together */
typedef struct {
	uint32_t op1;
	uint32_t op2;
} operands;

#define NUMMACS 256

/* Need to create a while loop inside main to wait for interrupt from host.
 * The interrupt will signify that a buffer of data has been passed and is
 * ready for MAC processing. This will be passed by rpmsg driver.
 */
void main(){
	uint32_t i;
	uint16_t numMacs = NUMMACS; // Arbitrary number
	uint64_t result = 0;
	volatile uint64_t storeValue = 0;
	operands buf[NUMMACS];

	for (i = 0; i < 256; i++){
		buf[i].op1 = i;
		buf[i].op2 = i+1;
	}

	/* Perform numMacs MAC operations */
	for (i = 0; i < numMacs; i++){
		result += (uint64_t)buf[i].op1 * (uint64_t)buf[i].op2;
	}

	storeValue = result;

	/* Nothing to do so halt */
	__halt();
}
