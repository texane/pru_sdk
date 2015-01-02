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
#include <pru_uart.h>

/* Mapping Constant table register to variable */
volatile far pruUart CT_UART __attribute__((cregister("PRU_UART0", near), peripheral));

/* Commented out to prevent internal Loopback */
//#define HWLOOPBACK

/* If SWLOOPBACK is defined, this flag tells the main loop to copy rxBuf to txBuf */
#ifdef SWLOOPBACK
uint8_t lpbkFlag;
#endif

/* The FIFO size on the PRU UART is 16 bytes; however, we are (arbitrarily)
 * only going to send 8 at a time */
#define FIFO_SIZE	16
#define MAX_CHARS	8

/* Buffer type */
typedef struct {
	uint8_t data[FIFO_SIZE];
} uartBuffer;

/* This hostBuffer structure allows the PRU to know the host is filling
 * the data memory starting at address 0 */
#pragma LOCATION(hostBuffer, 0)
struct {
	uint8_t msg;
	uartBuffer data[FIFO_SIZE];
} hostBuffer;

/* This rxBuf structure is used by the receive data */
#pragma LOCATION(rxBuf, 0x100)
uartBuffer rxBuf;

/* This txBuf structure is defined to store send data */
#pragma LOCATION(txBuf, 0x200)
uartBuffer txBuf;



//******************************************************************************
//    Print Message Out
//      This function take in a string literal of any size and then fill the
//      TX FIFO when it's empty and waits until there is info in the RX FIFO
//      before returning.
//******************************************************************************
void PrintMessageOut(volatile char* Message , uint8_t MsgSize)
{
	uint8_t cnt,index = 0;

	while(1)
	{
		cnt = 0;

		/* Wait until the TX FIFO and the TX SR are completely empty */
		while(!CT_UART.LSR_bit.TEMT);

		while(index < MsgSize && cnt < MAX_CHARS)
		{
			CT_UART.THR= Message[index];
			index++;
			cnt++;
		}
		if(index == MsgSize)
			break;

	}

	/* Wait until the TX FIFO and the TX SR are completely empty */
	while(!CT_UART.LSR_bit.TEMT);

	/* Clear the TX FIFO */
	CT_UART.FCR_bit.TXCLR = 0x1;
	CT_UART.FCR_bit.TXCLR = 0x0;
	
}


//******************************************************************************
//    IEP Timer Config
//      This function waits until there is info in the RX FIFO and then returns
//      the first character entered.
//******************************************************************************
char ReadMessageIn()
{
	while(!CT_UART.LSR_bit.DR);

	return CT_UART.RBR_bit.DATA;
}

void main(){
	uint32_t i;
	volatile uint32_t not_done = 1;

	char rxBuffer[5];

	char GreetingMessage[] = "Hello you are in the PRU UART demo test please enter 5 characters  ";
	
	char ReplyMessage[] = "you typed: ";

	char NewLine[] = {0x0A,0x0D,0x00};

	/*** INITIALIZATION ***/

	/* Set up UART to function at 115200 baud - DLL divisor is 104 at 16x oversample
	 * 192MHz / 104 / 16 = ~115200 */
	CT_UART.DLL = 104;
	CT_UART.DLH = 0;
	CT_UART.MDR_bit.OSM_SEL = 0x0;

	/* Enable Interrupts in UART module. This allows the main thread to poll for
	 * Receive Data Available and Transmit Holding Register Empty */
	CT_UART.IER = 0x7;

	/* If FIFOs are to be used, select desired trigger level and enable
	 * FIFOs by writing to FCR. FIFOEN bit in FCR must be set first before
	 * other bits are configured */
	/* Enable FIFOs for now at 1-byte, and flush them */
	CT_UART.FCR = (0x80) | (0x4) | (0x2) | (0x01); // 8-byte RX FIFO trigger

	/* Choose desired protocol settings by writing to LCR */
	/* 8-bit word, 1 stop bit, no parity, no break control and no divisor latch */
	CT_UART.LCR = 3;

	/* If flow control is desired write appropriate values to MCR. */
	/* No flow control for now, but enable loopback for test */
#ifdef HWLOOPBACK
	CT_UART.MCR = 0x10;
#elif SWLOOPBACK
	CT_UART.MCR = 0x00;
#endif

	/* Choose desired response to emulation suspend events by configuring
	 * FREE bit and enable UART by setting UTRST and URRST in PWREMU_MGMT */
	/* Allow UART to run free, enable UART TX/RX */
	CT_UART.PWREMU_MGMT_bit.FREE = 0x1;
	CT_UART.PWREMU_MGMT_bit.URRST = 0x1;
	CT_UART.PWREMU_MGMT_bit.UTRST =0x1;

	/* Turn off RTS and CTS functionality */
	CT_UART.MCR_bit.AFE = 0x0;
	CT_UART.MCR_bit.RTS = 0x0;

	/*** END INITIALIZATION ***/

	/* Print out greeting message */
	PrintMessageOut(GreetingMessage,sizeof(GreetingMessage));

	/* Print out new line */
	PrintMessageOut(NewLine,2);

	/* Read in 5 characters from user, then echo them back out */
	for(i = 0; i < 5 ; i++)
	{
		rxBuffer[i] = ReadMessageIn();
	}

	PrintMessageOut(ReplyMessage,sizeof(ReplyMessage));

	/* Print out new line */
	PrintMessageOut(NewLine,2);

	PrintMessageOut(rxBuffer,sizeof(rxBuffer));

	/* Print out new line */
	PrintMessageOut(NewLine,2);

	/*** DONE SENDING DATA ***/
	/* Disable UART before halting */
	CT_UART.PWREMU_MGMT = 0x0;

	/* Halt PRU core */
	__halt();
}
