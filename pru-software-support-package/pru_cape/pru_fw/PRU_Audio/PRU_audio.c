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
#include <pru_iep.h>
#include <pru_intc.h>
#include "PRU_audio.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

void SH0_Load(uint16_t data);
void SH1_Load(uint16_t data);
uint16_t DAC_Cmd(uint16_t DAC_Address, uint16_t cmd, uint8_t data);
uint16_t Endian_Swap(uint16_t data);
void iep_timer_config(void);
void intc_config(void);

/* Mapping Constant table register to variable */
volatile far pruIntc CT_INTC __attribute__((cregister("PRU_INTC", far), peripheral));
volatile pruCfg CT_CFG __attribute__((cregister("PRU_CFG", near), peripheral));
volatile far pruIep CT_IEP __attribute__((cregister("PRU_IEP", near), peripheral));

#define PRU_IEP_EVT   7
#define CMP0_VAL        0xFFF


volatile uint8_t temp = 0;
volatile uint32_t temp3 = 0;
volatile uint32_t temp4 = 0;

void main()
{
	int i = 0;
	volatile uint16_t dataA = 0;
	volatile uint16_t dataB = 0;

	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* Select OCP_CLK as IEP clock source */
	CT_CFG.IEPCLK_bit.OCP_EN = 0x1;

	iep_timer_config();

	intc_config();
	
	CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x1;

	/* Setting PRU1 GPO Clk Dividers to 12.5 and 2 to get 8 MHz CLK */
	CT_CFG.GPCFG1_bit.PRU1_GPO_DIV0 = 0x17;
	CT_CFG.GPCFG1_bit.PRU1_GPO_DIV1 = 0x02;

	/* Init the configure registers and put them in shift out mode */
	CT_CFG.GPCFG1_bit.PRU1_GPO_MODE = PRU1_GPO_MODE_SERIAL;

	/* Init PRU1 GPO to 0 */
	__R30 = 0x0;
	SH0_Load(0);
	SH1_Load(0);

	/* Enable PRU1 GPO shift out */
	__R30 |= SHIFT_ENABLE_FLAG << SHIFT_ENABLE_SHIFT;

	while(1){

		for(i = 0; i < 0xFF; i++)
		{

			/* Poll until R31.30 is set */
			do{
				 while((__R31 & 0x40000000) == 0){
				 }
			/* Verify that the IEP is the source of the interrupt */
			} while (CT_INTC.HIPIR0 != PRU_IEP_EVT);

			/* Clear Compare0 status */
			CT_IEP.TMR_CMP_STS = 0x1;

			__delay_cycles(2);

        	/* Clear the status of the interrupt */
        	CT_INTC.SECR0 = (1 << PRU_IEP_EVT);

        	/* Format sine wave data for DAC */
			dataA = DAC_Cmd(DAC_A_ADDRESS, WRITE_NO_UPDATE, SineRaw[i]);

			/* Swap data endian from LSB first to MSB first per DAC format*/
			dataA = Endian_Swap(dataA);

			/* Load data into Shadow Register 0 */
			SH0_Load(dataA);

			while(!(CT_CFG.GPCFG1 & PRU1_GPO_SH_SEL_MASK));

			/* Wait for SH Flag to indicate SH0 being output */
			while((CT_CFG.GPCFG1 & PRU1_GPO_SH_SEL_MASK));

			/* Generate sync signal */
			__R30 |= 0x0004;
			__delay_cycles(2);
			__R30 &= ~0x4;

        	/* Format sine wave data for DAC */
			dataB = DAC_Cmd(DAC_B_ADDRESS, WRITE_UPDATE, SineRaw[i]);

			/* Swap data endian from LSB first to MSB first per DAC format */
			dataB = Endian_Swap(dataB);

			/* Load data into Shadow Register 1 */
			SH1_Load(dataB);

			/* Wait for SH Flag to indicate SH1 being output */
			while(!(CT_CFG.GPCFG1 & PRU1_GPO_SH_SEL_MASK));

			/* Generate sync signal */
			__R30 |= 0x0004;
			__delay_cycles(2);
			__R30 &= ~0x4;

		}
	}

	/* Clear Shadow Register 0 */
	SH0_Load(0);

	/* Clear Shadow Register 1 */
	SH1_Load(0);

	/* Stop shifting data */
	__R30 &= ~(SHIFT_ENABLE_FLAG << SHIFT_ENABLE_SHIFT);

	while(1);

}


//******************************************************************************
//    Shadow Register 0 Load
//      This function enables the connection between R30 and SH0 so that data
//      loaded into R30[0-15] will also be loaded into SH0.
//******************************************************************************
void SH0_Load(uint16_t data){

	/* Clear R30 data */
	__R30 &= ~0xFFFF;

	/* Enable the load control for shadow reg 0 */
	__R30 |= SH0_LOAD_CONTROL_ENABLE << SH0_LOAD_CONTROL_SHIFT;

	/* Load the data to the shadow reg (corresponds to the bottom 2 byte of R30) */
	__R30 =  __R30 | data ;

	/* Disable the load control for shadow reg 0 and restore R30 */
	__R30 &= ~(SH0_LOAD_CONTROL_ENABLE << SH0_LOAD_CONTROL_SHIFT);
}


//******************************************************************************
//    Shadow Register 1 Load
//      This function enables the connection between R30 and SH1 so that data
//      loaded into R30[0-15] will also be loaded into SH1.
//******************************************************************************
void SH1_Load(uint16_t data){

	/* Clear R30 data */
	__R30 &= ~0xFFFF;

	/* Enable the load control for shadow reg 1 */
	__R30 |= SH1_LOAD_CONTROL_ENABLE << SH1_LOAD_CONTROL_SHIFT;

	/* Load the data to the shadow reg (corresponds to the bottom 2 byte of R30) */
	__R30 =  __R30 | data ;

	/* Disable the load control for shadow reg 1 and restore R30 */
	__R30 &= ~(SH1_LOAD_CONTROL_ENABLE << SH1_LOAD_CONTROL_SHIFT);

}


//******************************************************************************
//    DAC Command
//      This function creates the DAC command for DAC082S085.  The expected
//      command format is as follows.
//
//			bits: 15 - 14 | DAC Register
//      		  13 - 12 | DAC Op Code
//			      11 -  4 | DAC value
//      		   3 -  0 | DONT CARE
//
//      Registers will be pushed out of DAC when SH1(holding DAC B cmd/value)
//      is written to the DAC
//******************************************************************************
uint16_t DAC_Cmd(uint16_t DAC_Address, uint16_t cmd, uint8_t data){

	uint16_t output = 0;

	/* Create DAC command */
	output = ( (DAC_Address << DAC_ADDRESS_SHIFT) | (cmd << DAC_CMD_SHIFT) | (data << 4));

	return output;
}


//******************************************************************************
//    Endian Swap
//      This function reverses the input data's endianness. The shadow registers
//      are read LSB first but the DAC requires MSB first.  Therefore, the DAC
//      command endianness must be reversed.
//******************************************************************************
uint16_t Endian_Swap(uint16_t data){

	data = (((data & 0xaaaa) >> 1) | ((data & 0x5555) << 1));
  	data = (((data & 0xcccc) >> 2) | ((data & 0x3333) << 2));
  	data = (((data & 0xf0f0) >> 4) | ((data & 0x0f0f) << 4));
    data = ((data >> 8) | (data << 8));

    return data;

}

//******************************************************************************
//    IEP Timer Config
//      This function configures the PRU IEP Timer.
//******************************************************************************
void iep_timer_config(void){
 
        CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;             /* Disable counter */
        CT_IEP.TMR_CNT = 0xFFFFFFFF;               	   /* Reset Count register */
        CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;          /* Clear overflow status register */
        CT_IEP.TMR_CMP0 = CMP0_VAL;                    /* Set compare0 value */
        CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;         /* Clear compare status */
        CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;        /* Disable compensation */
        CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 0x1;  /* Disable CMP0 and reset on event */
        CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 0x1;
        CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 0x1;		/* Configure incr value */
        CT_INTC.SECR0 = 0xFFFFFFFF;                     /* Clear the status of all interrupts */
        CT_INTC.SECR1 = 0xFFFFFFFF;

}

//******************************************************************************
//    PRU INTC Config
//      This function configures the PRU Interrupt Controller.
//******************************************************************************
void intc_config(void){

	/* Set interrupts 0 to 31 to active high */
	CT_INTC.SIPR0 = 0xFFFFFFFF;

	/* Clear mapping for sys evt 4 to 7 */
	CT_INTC.CMR1 = 0x00000000;

	/* Set event 7 to channel 0 */
	CT_INTC.CMR1_bit.CH_MAP_7 = 0x0;

	/* Map channel 0 to host 0 */
	CT_INTC.HMR0_bit.HINT_MAP_0 = 0x0;

	/* Enable system event (7th bit = sys interrupt 7) */
	CT_INTC.ESR0 = 0x80;

	/* Enable host interrupt 0 */
	CT_INTC.HIER = 0x1;

	/* Globally enable interrupts */
	CT_INTC.GER = 0x1;
}

