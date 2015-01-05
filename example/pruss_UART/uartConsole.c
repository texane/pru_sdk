/**
 * \file   uartConsole.c
 *
 * \brief  This file contains functions which interface interactively
 *         with the user through the serial console to perform some
 *         utility operations.
 */

/*
* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
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


#include "../../include/include/uart_irda_cir.h"
#include "../../include/include/hw/soc_AM335x.h"
#include "../../include/include/armv7a/am335x/beaglebone.h"
#include "../../include/include/hw/hw_types.h"

/******************************************************************************
**              INTERNAL MACRO DEFINITIONS
******************************************************************************/
#define UART_CONSOLE_BASE                    (SOC_UART_0_REGS)
#define BAUD_RATE_115200                     (115200)
#define UART_MODULE_INPUT_CLK                (48000000)

/******************************************************************************
**              INTERNAL FUNCTION DECLARATIONS
******************************************************************************/
static void UARTStdioInitExpClk(unsigned int baudRate,
                                unsigned int rxTrigLevel,
                                unsigned int txTrigLevel);
static void UartFIFOConfigure(unsigned int txTrigLevel,
                              unsigned int rxTrigLevel);
static void UartBaudRateSet(unsigned int baudRate);
void UARTConsolePutc(unsigned char data);
unsigned char UARTConsoleGetc(void);
void UARTConsoleInit(void);

/******************************************************************************
**              FUNCTION DEFINITIONS
******************************************************************************/

/**
 * \brief   This function initializes the specified UART instance for use.
 *          This does the following:        
 *          - Enables the FIFO and performs the FIFO settings\n
 *          - Performs the Baud Rate settings\n 
 *          - Configures the Line Characteristics to 8-N-1 format\n
 *
 * \param   baudRate     The baud rate to be used for communication
 * \param   rxTrigLevel  The receiver FIFO trigger level
 * \param   txTrigLevel  The transmitter FIFO trigger level
 *
 * \return  None
 *
 * \note    By default, a granularity of 1 will be selected for transmitter
 *          and receiver FIFO trigger levels.
 */

static void UARTStdioInitExpClk(unsigned int baudRate,
                                unsigned int rxTrigLevel,
                                unsigned int txTrigLevel)
{
    /* Performing a module reset. */
    UARTModuleReset(UART_CONSOLE_BASE);

    /* Performing FIFO configurations. */
    UartFIFOConfigure(txTrigLevel, rxTrigLevel);

    /* Performing Baud Rate settings. */
    UartBaudRateSet(baudRate);

    /* Switching to Configuration Mode B. */
    UARTRegConfigModeEnable(UART_CONSOLE_BASE, UART_REG_CONFIG_MODE_B);

    /* Programming the Line Characteristics. */
    UARTLineCharacConfig(UART_CONSOLE_BASE,
                         (UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1),
                         UART_PARITY_NONE);

    /* Disabling write access to Divisor Latches. */
    UARTDivisorLatchDisable(UART_CONSOLE_BASE);

    /* Disabling Break Control. */
    UARTBreakCtl(UART_CONSOLE_BASE, UART_BREAK_COND_DISABLE);

    /* Switching to UART16x operating mode. */
    UARTOperatingModeSelect(UART_CONSOLE_BASE, UART16x_OPER_MODE);
}

/*
** A wrapper function performing FIFO configurations.
*/

static void UartFIFOConfigure(unsigned int txTrigLevel,
                              unsigned int rxTrigLevel)
{
    unsigned int fifoConfig = 0;

    /* Setting the TX and RX FIFO Trigger levels as 1. No DMA enabled. */
    fifoConfig = UART_FIFO_CONFIG(UART_TRIG_LVL_GRANULARITY_1,
                                  UART_TRIG_LVL_GRANULARITY_1,
                                  txTrigLevel,
                                  rxTrigLevel,
                                  1,
                                  1,
                                  UART_DMA_EN_PATH_SCR,
                                  UART_DMA_MODE_0_ENABLE);

    /* Configuring the FIFO settings. */
    UARTFIFOConfig(UART_CONSOLE_BASE, fifoConfig);
}

/*
** A wrapper function performing Baud Rate settings.
*/

static void UartBaudRateSet(unsigned int baudRate)
{
    unsigned int divisorValue = 0;

    /* Computing the Divisor Value. */
    divisorValue = UARTDivisorValCompute(UART_MODULE_INPUT_CLK,
                                         baudRate,
                                         UART16x_OPER_MODE,
                                         UART_MIR_OVERSAMPLING_RATE_42);

    /* Programming the Divisor Latches. */
    UARTDivisorLatchWrite(UART_CONSOLE_BASE, divisorValue);
}

/**
 * \brief   This function initializes the specified UART instance for use.
 *          This does the following:
 *             - Enables the FIFO and performs the FIFO settings\n
 *          - Transmitter and Recevier Threshold Levels are 1 byte each\n
 *          - Baud Rate of 115200 bits per second(bps)\n
 *          - Configures the Line Characteristics to 8-N-1 format\n
 *
 * \param   None
 *
 * \return  None
 */

void UARTConsoleInit(void)
{
    /* Configuring the system clocks for UART0 instance. */
    UART0ModuleClkConfig();

    /* Performing the Pin Multiplexing for UART0 instance. */
    UARTPinMuxSetup(0);

    UARTStdioInitExpClk(BAUD_RATE_115200, 1, 1);
}

/**
 * \brief   This function puts a character on the serial console.
 *
 * \param   data    The character to be put on the serial console
 *
 * \return  None.
 */
void UARTConsolePutc(unsigned char data)
{
     UARTCharPut(UART_CONSOLE_BASE, data);
}

/**
 * \brief   This function puts a character on the serial console.
 *
 * \param   none
 *
 * \return  Character as input from the console
 */
unsigned char UARTConsoleGetc(void)
{
     return ((unsigned char)UARTCharGet(UART_CONSOLE_BASE));
}

/****************************** End Of File *********************************/
