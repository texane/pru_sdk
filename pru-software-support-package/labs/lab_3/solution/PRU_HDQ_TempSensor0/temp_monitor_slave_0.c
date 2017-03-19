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
#include <pru_iep.h>
#include "PRU_1wire.h"
#include <string.h>

volatile register uint32_t __R30;
volatile register uint32_t __R31;
	
/* Mapping Constant table register to variable */
volatile far pruIntc CT_INTC __attribute__((cregister("PRU_INTC", far), peripheral));
volatile pruCfg CT_CFG __attribute__((cregister("PRU_CFG", near), peripheral));
volatile far pruIep CT_IEP __attribute__((cregister("PRU_IEP", far), peripheral));
volatile far char CT_DMEM[64] __attribute__((cregister("PRU0_1", far), peripheral));

/******************************************************************************
**                           INTERNAL FUNCTION DEFINITIONS
******************************************************************************/
void write_cmd(unsigned int cmd);
void write_0(void);
void write_1(void);
unsigned int read();
unsigned int init();
void reset(void);
void timer(unsigned int cmp);

void iep_timer_config(void);
void iep_digio_config(void);

/* PRU peripheral system event */
#define PRU_IEP_EVT     7
/* PRU-to-PRU system event */
#define PRU_SLAVE_MASTER_EVT    20
#define PRU_MASTER_SLAVE_EVT    21
#define PRU_SLAVE_MASTER_EVT_TRIGGER  (__R31 = (PRU_SLAVE_MASTER_EVT - 16) | (1 << 5))
#define PRU_MASTER_SLAVE_EVT_TRIGGER  (__R31 = (PRU_MASTER_SLAVE_EVT - 16) | (1 << 5))

/* 1-Wire input (GPI) */
#define IN_1WIRE		(1 << 14)

/* 1-Wire output (IEP DigIO) */
#define OUT_1WIRE	(1 << 5)

#define SKIP_ROM	0xCC
#define CONVERT_T 	0x44
#define READ_SCRATCHPAD	0xBE

#define CMP0_VAL        0x17700   /* 480us @ 200MHz */
#define CMP1_VAL        0x2EE0    /* 60us  @ 200MHz */
#define CMP2_VAL        0xBB8     /* 15us  @ 200MHz */
#define CMP3_VAL	0xC8 	  /* 1us   @ 200MHz */
#define CMP4_VAL	0xAF0	  /* 14us  @ 200MHz */
	
#define wait480us	0x0	    /* CMP0 */
#define wait60us	0x1     /* CMP1 */
#define wait15us	0x2     /* CMP2 */
#define wait1us		0x3	    /* CMP3 */
#define wait14us	0x4     /* CMP4 */

 // CRC Byte Array
 unsigned char CRC_BYTE_ARRAY [256] =
 {
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
 };

#pragma DATA_SECTION(TEMP_SENSOR_BUF, ".TEMP_SENSOR_BUF")
far char TEMP_SENSOR_BUF[9];

void main(){
	unsigned int status, crc_calc, i, j = 0;
	char bit_val, byte_val = 0;

	/* Initialize TEMP_SENSOR_BUF to 0 */
	memset(TEMP_SENSOR_BUF, 0, 9);

	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* Configure IEP timer */
	iep_timer_config();	

	/* Configure IEP digio */
	iep_digio_config();
	CT_IEP.DIGIO_DATA_OUT_EN = OUT_1WIRE;		
	
	/* Interrupt Master PRU */
	PRU_SLAVE_MASTER_EVT_TRIGGER;

	while(1){

		/* Detect Master PRU interrupt: Read Temp */
		while((__R31 & 0x40000000) == 0){
		}

		/* Clear system event */
		CT_INTC.SECR0 = (1 << PRU_MASTER_SLAVE_EVT);

		/* Issue Convert T command to sensor */
		init();
		write_cmd(SKIP_ROM);	
		write_cmd(CONVERT_T);
		while(status == 0){
			status = read();
		}
		status = 0;

		/* Issue Read Scratchpad command to sensor */
		init();
		write_cmd(SKIP_ROM);
		write_cmd(READ_SCRATCHPAD);
	
 		crc_calc = 0;

		/* Read sensor scratchpad's 9 bytes of data */
		for(i=0; i < 9; i++){
			byte_val = 0;

			/* Read 8 bits per byte */
			for(j=0; j < 8; j++){
				bit_val = read();
				byte_val = byte_val | (bit_val << j);
			}

			/* Store each byte of data in memory (PRU Shared RAM) */
			TEMP_SENSOR_BUF[i] = byte_val;  

			/* Calculate CRC after each byte of data read */
			crc_calc = CRC_BYTE_ARRAY[crc_calc ^ byte_val];
		}
		
		/* Compare CRC values */
		/* If CRC value matches, interrupt Master that temp value is ready */
		if(crc_calc == 0x0){
			 PRU_SLAVE_MASTER_EVT_TRIGGER;
		}
		/* If CRC value does not match, re-sample temp */
		if(crc_calc != 0x0){
			PRU_MASTER_SLAVE_EVT_TRIGGER;
		}

	}	
}

/******************************************************************************
**                              WRITE_CMD FUNCTION
******************************************************************************/

void write_cmd(unsigned int cmd){
	unsigned int i;
	unsigned int len = 8;
	unsigned int bit = 0;

	for(i = 0; i < len; i++){
		bit = cmd & 0x1;
		if(bit == 0){
			write_0();
		}
		if(bit == 1){
			write_1();
		}
		cmd = cmd >> 1;
	}
}


/******************************************************************************
**                            1-WIRE PROTOCOL FUNCTIONS
******************************************************************************/

void write_0(){

	/* Drive DIG Output low */
	CT_IEP.DIGIO_DATA_OUT_EN = 0x0;
	
	timer(wait60us);

	/* Put DIG Output in tri-state (1-wire I/O pulled high) */
	CT_IEP.DIGIO_DATA_OUT_EN = OUT_1WIRE;
			
	timer(wait1us);

}

void write_1(){

	/* Drive DIG Output low */
	CT_IEP.DIGIO_DATA_OUT_EN = 0x0;
	
	timer(wait15us);

	/* Put DIG Output in tri-state (1-wire I/O pulled high) */
	CT_IEP.DIGIO_DATA_OUT_EN = OUT_1WIRE;
	
	timer(wait60us);
}

unsigned int read(){
	unsigned int bit_val;	

        /* Drive DIG Output low */
	    CT_IEP.DIGIO_DATA_OUT_EN = 0x0;
	
        timer(wait1us);

        /* Put DIG Output in tri-state (1-wire I/O pulled high) */
        CT_IEP.DIGIO_DATA_OUT_EN = OUT_1WIRE;

        timer(wait14us);

	bit_val = __R31 & IN_1WIRE;

	timer(wait60us);
	
	if(bit_val == IN_1WIRE){
		bit_val = 1;
	}
	else{
		bit_val = 0;
	}
	
	/* TEMP */
	timer(wait1us);

	return bit_val;
}

unsigned int init(){
        unsigned int present_pulse = 0;

        /* Drive OUT_1WIRE low */
        CT_IEP.DIGIO_DATA_OUT_EN = 0x0;

        timer(wait480us);

        /* Set OUT_1WIRE in tri-state mode */
        CT_IEP.DIGIO_DATA_OUT_EN = OUT_1WIRE;

        /* Enable Timer for Master Rx */
        CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x1;       /* Enable counter on event */

        /* Wait for presence pulse or timeout */
        while(((__R31 & IN_1WIRE) == 1) || ((__R31 & 0x40000000) == 0)){
        }

        if ((__R31 & IN_1WIRE) == 0){ /* IN_1WIRE clearred */

                present_pulse = 1;

                /* Wait for timeout */
                while((__R31 & 0x40000000) == 0){
                }
        }
        else{
                present_pulse = 0;
        }

        /* Disable IEP Timer */
        CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0; /* Disable counter */
        CT_IEP.TMR_CNT = 0;                  /* Clear counter */

        /* Clear IEP Timer system event */
        CT_IEP.TMR_CMP_STS = (1 << wait480us);
        __delay_cycles(4);
        CT_INTC.SECR0 = (1 << PRU_IEP_EVT);

        return (present_pulse);

}

/******************************************************************************
**                               TIMER FUNCTION
******************************************************************************/

void timer(unsigned int cmp){

        /* Enable Compare Value */
        CT_IEP.TMR_CMP_CFG_bit.CMP_EN = (1 << cmp);

        /* Enable Timer for Master Tx Reset Pulse (480us) */
        CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x1;          /* Enable counter */

        /* Detect IEP Timer interrupt */
        do{
                while((__R31 & 0x40000000) == 0){
                }
        /* Verify that the IEP is the source of the interrupt */
        } while (CT_INTC.HIPIR0 != PRU_IEP_EVT);

        /* Disable IEP Timer */
        CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0;  /* Disable counter */
        CT_IEP.TMR_CNT = 0x0;                 /* Clear counter */

        /* Clear IEP Timer system event */
        CT_IEP.TMR_CMP_STS = (1 << cmp);
        __delay_cycles(4);
        CT_INTC.SECR0 = (1 << PRU_IEP_EVT);
}


/******************************************************************************
**                          INTERNAL CONFIG FUNCTIONS
******************************************************************************/

void iep_timer_config(void){
 
        CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;          		/* Disable counter */
        CT_IEP.TMR_CNT = 0x0;                           	/* Reset Count register */
        CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;       	 	/* Clear overflow status register */
        CT_IEP.TMR_CMP0 = CMP0_VAL;                         /* Set compare0 value */
        CT_IEP.TMR_CMP1 = CMP1_VAL;                         /* Set compare1 value */
        CT_IEP.TMR_CMP2 = CMP2_VAL;                         /* Set compare2 value */
        CT_IEP.TMR_CMP3 = CMP3_VAL;                         /* Set compare3 value */
        CT_IEP.TMR_CMP4 = CMP4_VAL;			        		/* Set compare4 value */
        CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;           	/* Clear compare status */
        CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;             /* Disable compensation */
        CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 0x0;       /* Disable CMP0 and reset on event */
        CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 0x0;

        CT_INTC.SECR0 = 0xFFFFFFFF;                 /* Clear the status of all interrupts */
        CT_INTC.SECR1 = 0xFFFFFFFF;

        CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 0x1;	/* Configure incr value */

}

void iep_digio_config(){

		/* Enable software to control value of digio outputs */
		CT_IEP.DIGIO_EXP_bit.OUTVALID_OVR_EN = 0x1;
		CT_IEP.DIGIO_EXP_bit.SW_DATA_OUT_UPDATE = 0x1;

}
