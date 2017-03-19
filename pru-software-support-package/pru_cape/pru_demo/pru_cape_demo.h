/*
 * pru_cape_demo.h
 *
 * This file contains the definitions for the PRU Cape demo code.
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

#ifndef PRU_CAPE_DEMO_H_
#define PRU_CAPE_DEMO_H_

#include "hw_types.h"
#include "soc_AM335x.h"
#include "hw_control_AM335x.h"
#include "hw_cm_per.h"
#include "hw_prm_per.h"
#include "consoleUtils.h"
#include "consoleUtils.h"
#include "uartStdio.h"

extern const unsigned char LED0_INST[0x1000];
extern const unsigned char LED0_DATA[0x1000];
extern const unsigned char LED1_INST[0x1000];
extern const unsigned char LED1_DATA[0x1000];
extern const unsigned char SW_INST[0x1000];
extern const unsigned char SW_DATA[0x1000];
extern const unsigned char AUDIO_INST[0x1000];
extern const unsigned char AUDIO_DATA[0x1000];
extern const unsigned char UART_INST[0x1000];
extern const unsigned char UART_DATA[0x1000];
extern const unsigned char SLAVE_INST[0x1000];
extern const unsigned char SLAVE_DATA[0x1000];
extern const unsigned char MASTER_INST[0x1000];
extern const unsigned char MASTER_DATA[0x1000];

/*****************************************************************************
**                    Macro definitions
*****************************************************************************/

#define PRU_ICSS1 	1
#define PRU0		0
#define PRU1		1

#define AM335X_SLEWCTRL_FAST            (0 << 6)
#define AM335X_SLEWCTRL_SLOW            (1 << 6)
#define AM335X_INPUT_EN                 (1 << 5)
#define AM335X_PULL_UP                  (1 << 4)
/* bit 3: 0 - enable, 1 - disable for pull enable */
#define AM335X_PULL_DISA                (1 << 3)

#define AM335X_PIN_OUTPUT               (0)
#define AM335X_PIN_OUTPUT_PULLUP        (AM335X_PULL_UP)
#define AM335X_PIN_INPUT                (AM335X_INPUT_EN | AM335X_PULL_DISA)
#define AM335X_PIN_INPUT_PULLUP         (AM335X_INPUT_EN | AM335X_PULL_UP)
#define AM335X_PIN_INPUT_PULLDOWN       (AM335X_INPUT_EN)


/*****************************************************************************
**             Function definitions & Global variables
*****************************************************************************/

void MainMenu(void);
int LEDTest(void);
int SwitchTest(void);
int AudioTest(void);
int HDQTest(void);
int UARTTest(void);
void UARTInit(void);
void PRUCapePinmux(void);

#endif
