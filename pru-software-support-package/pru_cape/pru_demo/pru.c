/*
 * pru.c
 *
 * This file contains the device abstrction layer APIs for
 * Programmabled Real-time Unit (PRU).
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include "soc_AM335x.h"
#include "pru.h"
#include "hw_pruss.h"
#include "hw_pru_ctrl.h"
#include "hw_cm_per.h"
#include "hw_types.h"
#include <string.h>

//******************************************************************************
//    PRU ICSS Init
//      This function resets the PRU, enables the PRU clocks, and inits the
//      PRU memory to 0s.
//******************************************************************************
void PRUICSSInit(void)
{

	HWREG(SOC_PRM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) |= 0x2;  // *((unsigned int*) 0x44E00C00 ) |= 0x2; // reset PRU
	HWREG(SOC_PRM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &= 0xFFFFFFFD;   // *((unsigned int*) 0x44E00C00 ) &= 0xFFFFFFFD;

	HWREG(SOC_CM_PER_REGS + CM_PER_ICSS_CLKCTRL) |= 0x02;  //*((unsigned int*) 0x44E000E8 ) |= 0x02; // enabling the PRU-ICSS module taking it out of idle

	HWREG(SOC_PRM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) |= 0x2;  // *((unsigned int*) 0x44E00C00 ) |= 0x2; // reset PRU
	HWREG(SOC_PRM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &= 0xFFFFFFFD;   // *((unsigned int*) 0x44E00C00 ) &= 0xFFFFFFFD;

	/* Clear out the date memory of both PRU cores */
	PRUMemFill((SOC_PRUICSS1_REGS + SOC_PRUICSS_PRU0_DRAM_OFFSET),8*1024,0);    //Data 8KB RAM0
	PRUMemFill((SOC_PRUICSS1_REGS + SOC_PRUICSS_PRU1_DRAM_OFFSET),8*1024,0 );	 //Data 8KB RAM1
	PRUMemFill((SOC_PRUICSS1_REGS + SOC_PRUICSS_PRU0_IRAM_OFFSET),8*1024,0 );
	PRUMemFill((SOC_PRUICSS1_REGS + SOC_PRUICSS_PRU1_IRAM_OFFSET),8*1024,0 );

}

//******************************************************************************
//    PRU Memory Fill
//      This function takes and a pointer length and value to be placed and
//      iterates through the memory placing the pattern.
//******************************************************************************
void PRUMemFill(unsigned int StartAddress, unsigned int Length , unsigned int  Pattern)
{

	memset((unsigned char*)StartAddress, Pattern, (Length/4));

}

//******************************************************************************
//    PRU Memory Load
//      This function loads the info at the pointer into the starting address.
//******************************************************************************
void PRUMemLoad(unsigned int PRUICSSInstance, unsigned int MemoryType, unsigned int offset, unsigned int Length ,const unsigned int *Pointer)
{
 	unsigned int BaseAddress, StartAddress;

 	if(PRUICSSInstance == 1)
 		BaseAddress = SOC_PRUICSS1_REGS;

 	if(MemoryType == PRU0_DRAM)
 		StartAddress = BaseAddress + SOC_PRUICSS_PRU0_DRAM_OFFSET;
 	if(MemoryType == PRU1_DRAM)
 		StartAddress = BaseAddress + SOC_PRUICSS_PRU1_DRAM_OFFSET;
 	if(MemoryType == PRU0_IRAM)
 		StartAddress = BaseAddress + SOC_PRUICSS_PRU0_IRAM_OFFSET;
 	if(MemoryType == PRU1_IRAM)
 		StartAddress = BaseAddress + SOC_PRUICSS_PRU1_IRAM_OFFSET;
 	if(MemoryType == PRU_SHARED_RAM)
 		StartAddress = BaseAddress + SOC_PRUICSS_SHARED_RAM_OFFSET;

 	unsigned char *srcaddr;
 	unsigned char *destaddr;

 	srcaddr = (unsigned char*)Pointer;
 	destaddr =(unsigned char*)StartAddress;

 	memcpy(destaddr, srcaddr, Length);

}

//******************************************************************************
//    PRU ICSS Reset
//      This function resets the PRU cores.
//******************************************************************************
void PRUICSSReset(void){

	HWREG(SOC_PRM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) |= 0x2;    /* Reset PRU */
	HWREG(SOC_PRM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &= 0xFFFFFFFD;

}

//******************************************************************************
//    PRU Enable
//      This function enables the given PRU core to start fetching and
//      executing instructions from its respective IRAM.
//******************************************************************************
void PRUEnable(unsigned int PRUICSSInstance, unsigned int PRUCore){

	unsigned int BaseAddress;

 	if(PRUICSSInstance == 1)
 		BaseAddress = SOC_PRUICSS1_REGS;

	if(PRUCore == 0)
		HWREG(BaseAddress + SOC_PRUICSS_PRU0_CTRL_OFFSET + PRU_CTRL_CONTROL) = 0xB;
	if(PRUCore == 1)
		HWREG(BaseAddress + SOC_PRUICSS_PRU1_CTRL_OFFSET + PRU_CTRL_CONTROL) = 0xB;

}

//******************************************************************************
//    PRU Halt
//      This function halts the given PRU core.
//******************************************************************************
void PRUHalt(unsigned int PRUICSSInstance, unsigned int PRUCore){

	unsigned int BaseAddress;

 	if(PRUICSSInstance == 1)
 		BaseAddress = SOC_PRUICSS1_REGS;

	if(PRUCore == 0)
		HWREG(BaseAddress + SOC_PRUICSS_PRU0_CTRL_OFFSET + PRU_CTRL_CONTROL) = 0x0;
	if(PRUCore == 1)
		HWREG(BaseAddress + SOC_PRUICSS_PRU1_CTRL_OFFSET + PRU_CTRL_CONTROL) = 0x0;
}
