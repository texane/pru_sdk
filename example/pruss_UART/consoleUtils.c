/**
 * \file   consoleUtils.c
 *
 * \brief  This file contains generic wrapper functions for console utilities
 *         which allow user to configure the console type and redirect all the
 *         Console Input/Output to the console type selected.
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

#include <stdio.h>
#include "../../include/include/uartStdio.h"
#include "../../include/include/consoleUtils.h"

/****************************************************************************
**                    INTERNAL VARIABLE DEFINITION
****************************************************************************/
static enum consoleUtilsType ConsoleType;

/*****************************************************************************
**                    FUNCTION DEFINITIONS
*****************************************************************************/

/**
 * \brief   This function initializes the UART for use as the Console
 *          for all Input/Output operations.
 *
 * \return  None.
 *
 */
void ConsoleUtilsInit()
{
    /*
    ** Initializes the UART with appropriate Baud rate and Line
    ** characteristics for use as console.
    */
    UARTStdioInit();
}

/**
 * \brief   This function sets up the console for use in all IO operations
 *          based on the option provided by the user.
 *
 * \param   consoleFlag    console selection flag to choose between the
 *                         different console Type, takes differnt values
 *                         from the enum type
 *                         \enum consoleUtils.h consoleUtilsConsoleType
 *
 * \return  None.
 *
 */
void ConsoleUtilsSetType(enum consoleUtilsType consoleFlag)
{
    if(consoleFlag < CONSOLE_MAXTYPE)
    {
        ConsoleType = consoleFlag;
    }
    else
    {
        UARTPuts("Error! Invalid Console selection\r\n", -1);
    }
}

/**
 * \brief   This function is a wrapper for printf utility which prints
 *          the input string on the configured console type.
 *
 * \param   string is the format string.
 * \param   ... are the optional arguments, which depend on the contents of the
 *          format string.
 *          The type of the arguments after string must match the requirements
 *          of the format string. For example, if an integer was passed where
 *          a string was expected, an error of some kind will most likely occur
 *
 * \return  None.
 *
 */
void ConsoleUtilsPrintf(const char *string, ...)
{
    va_list arg;

    /* Start the variable arguments processing. */
    va_start (arg, string);

    if(CONSOLE_DEBUGGER == ConsoleType)
    {
        /*
        **  Adding a compile time check to reduce the binary size
        **  if semihosting is not needed.
        */
        #if defined(SEMIHOSTING)
        vprintf(string, arg);
        /*
        ** The input stream stdout is flushed to avoid reading unwanted
        ** characters from the CCS console
        */
        fflush(stdout);
        #else
        UARTPuts("Error! SemiHosting Support is not enabled\r\n", -1);
        #endif
    }
    else
    {
        UARTPrintf(string, arg);
    }
    /* End the variable arguments processing. */
    va_end(arg);
}

/**
 * \brief   This function is a wrapper for scanf utility which reads
 *          the input according to the format specifier from the
 *          configured console type.
 *
 * \param   format is the format specifiers.
 * \param   ... are the pointer arguments, which point to the locations
 *          where the input values are to be stored.
 *
 * \return  None.
 *
 */
int ConsoleUtilsScanf(const char *format, ...)
{
    va_list arg;
    int inputStatus = -1;

    /* Start the variable argument processing. */
    va_start(arg, format);

    if(CONSOLE_DEBUGGER == ConsoleType)
    {
        /*
        **  Adding a compile time check to reduce the binary size
        **  if semihosting is not needed.
        */
        #if defined(SEMIHOSTING)
        inputStatus = vscanf(format, arg);
        /*
        ** The input stream stdout is flushed to avoid reading unwanted
        ** characters from the CCS console
        */
        fflush(stdin);
        #else
        UARTPuts("Error! SemiHosting Support is not enabled\r\n", -1);
        #endif
    }
    else
    {
        inputStatus = UARTScanf(format, arg);
    }
    /* End the variable argument processing. */
    va_end(arg);
    return (inputStatus);
}

/**
 * \brief  This function is a wrapper for Gets utility which reads
 *         a string from the configured console Type.
 *
 * \param  rxBuffer         Pointer to a buffer in the receiver.
 * \param  size             The number of bytes the receiver buffer can hold.
 *
 * \return pBuf             pointer to the buffer containing received data
 *
 * \return  None.
 */
char* ConsoleUtilsGets(char *rxBuffer, int size)
{
    char *pBuf = (void *)0;

    if(CONSOLE_DEBUGGER == ConsoleType)
    {
        /*
        **  Adding a compile time check to reduce the binary size
        **  if semihosting is not needed.
        */
        #if defined(SEMIHOSTING)
        pBuf = fgets(rxBuffer, size, stdin);
        /* The input stream stdin is flushed to avoid reading unwanted
        ** characters from the CCS console
        */
        fflush(stdin);
        #else
        UARTPuts("Error! SemiHosting Support is not enabled\r\n", -1);
        #endif
    }
    else
    {
        pBuf = UARTGets(rxBuffer, size);
    }

    return pBuf;
}

/**
 * \brief  This function is a wrapper funcion to print a string to
 *         the console type configured.
 *
 * \param  string       Pointer to a string to be printed on the console.
 * \param  size         The number of bytes the string buffer can hold.
 *
 * \return  None.
 */
void ConsoleUtilsPuts(char *string, int size)
{
    if(CONSOLE_DEBUGGER == ConsoleType)
    {
        /*
        **  Adding a compile time check to reduce the binary size
        **  if semihosting is not needed.
        */
        #if defined(SEMIHOSTING)
        fputs(string, stdout);
        /* the output stream of the CCS console is line buffered
        ** so prints on the console do not appear until a newline is entered
        ** to avoid this we flush the buffer explicitly
        */
        fflush(stdout);
        #else
        UARTPuts("Error! SemiHosting Support is not enabled\r\n", -1);
        #endif
    }
    else
    {
        UARTPuts(string, size);
    }
}

/**
 * \brief   This function is a wrapper funcion to read a character from
 *          console type configured.
 *
 * \return  Returns the entered byte read from the selected console.
 */
unsigned char ConsoleUtilsGetChar(void)
{
    unsigned char byte = 0u;
    unsigned char rxByte = 0u;

    if(CONSOLE_DEBUGGER == ConsoleType)
    {
        /*
        **  Adding a compile time check to reduce the binary size
        **  if semihosting is not needed.
        */
        #if defined(SEMIHOSTING)
        byte = getchar();
        /* The input stream stdin is flushed to avoid reading unwanted
        ** characters from the CCS console
        */
        fflush(stdin);
        #else
        UARTPuts("Error! SemiHosting Support is not enabled\r\n", -1);
        #endif
    }
    else
    {
        byte = UARTGetc();
        rxByte = byte;
        
        /* Wait until the byte entered is new line or carriage return */
        while((rxByte != '\n') && (rxByte != '\r'))
        {
           rxByte = UARTGetc();
        }
    }

    return(byte);
}

/**
 * \brief   This function is a wrapper funcion to print a character to
 *          the console type configured.
 *
 * \param   byte   The byte to be printed.
 *
 * \return  None.
 */
void ConsoleUtilsPutChar(unsigned char byte)
{
    if(CONSOLE_DEBUGGER == ConsoleType)
    {
        /*
        **  Adding a compile time check to reduce the binary size
        **  if semihosting is not needed.
        */
        #if defined(SEMIHOSTING)
        putchar(byte);
        /* the output stream of the CCS console is line buffered
        ** so prints on the console do not appear until a newline is entered
        ** to avoid this we flush the buffer explicitly
        */
        fflush(stdout);
        #else
        UARTPuts("Error! SemiHosting Support is not enabled\r\n", -1);
        #endif
    }
    else
    {
        UARTPutc(byte);
    }
}
