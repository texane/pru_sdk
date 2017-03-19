/*
 * hw_pruss.h
 *
 * This file contains the PRU Subsystem register definitions.
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

#ifndef _HW_PRUSS_H_
#define _HW_PRUSS_H_

#ifdef __cplusplus
extern "C" {
#endif

// AM335x Base Address for PRU-ICSS
#define SOC_PRUICSS1_REGS		0x4A300000

#define SOC_PRUICSS_PRU0_DRAM_OFFSET	(0x00000000)
#define SOC_PRUICSS_PRU1_DRAM_OFFSET	(0x00002000)
#define SOC_PRUICSS_SHARED_RAM_OFFSET	(0x00010000)
#define SOC_PRUICSS_INTC_OFFSET			(0x00020000)
#define SOC_PRUICSS_PRU0_CTRL_OFFSET	(0x00022000)
#define SOC_PRUICSS_PRU0_DBG_OFFSET		(0x00022400)
#define SOC_PRUICSS_PRU1_CTRL_OFFSET	(0x00024000)
#define SOC_PRUICSS_PRU1_DBG_OFFSET		(0x00024400)
#define SOC_PRUICSS_CFG_OFFSET			(0x00026000)
#define SOC_PRUICSS_UART_OFFSET			(0x00028000)
#define SOC_PRUICSS_IEP_OFFSET			(0x0002E000)
#define SOC_PRUICSS_ECAP_OFFSET			(0x00030000)
#define SOC_PRUICSS_PRU0_IRAM_OFFSET	(0x00034000)
#define SOC_PRUICSS_PRU1_IRAM_OFFSET	(0x00038000)

#define PRU0			0
#define PRU1			1

#define PRU0_DRAM		0
#define PRU1_DRAM		1
#define PRU0_IRAM		2
#define PRU1_IRAM		3
#define PRU_SHARED_RAM	4
#ifdef __cplusplus
}
#endif

#endif
