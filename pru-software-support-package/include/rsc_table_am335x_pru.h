/*
 * Copyright (c) 2012-2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== rsc_table_am335x_pru.h ========
 *
 *  Define the resource table entries for all PRU cores. This will be
 *  incorporated into corresponding base images, and used by the remoteproc
 *  on the host-side to allocated/reserve resources.
 *
 */

#ifndef _RSC_TABLE_AM335X_PRU_H_
#define _RSC_TABLE_AM335X_PRU_H_

#include <stddef.h>
#include <rsc_types.h>
#include "pru_virtio_ids.h"

/* PRU Memory Map */
#define PRU_GLOBAL_BASE		0x4A300000

/* PRU Global Offsets */
#define PRU0_DRAM		(PRU_GLOBAL_BASE)
#define PRU1_DRAM		(PRU_GLOBAL_BASE + 0x00002000)

#define PRU_SHARED_DRAM		(PRU_GLOBAL_BASE + 0x00010000)

#define PRU_INTC		(PRU_GLOBAL_BASE + 0x00020000)

#define PRU0_CTRL		(PRU_GLOBAL_BASE + 0x00022000)
#define PRU1_CTRL		(PRU_GLOBAL_BASE + 0x00024000)

#define PRU0_DEBUG		(PRU_GLOBAL_BASE + 0x00022400)
#define PRU1_DEBUG		(PRU_GLOBAL_BASE + 0x00024400)

#define PRU_CFG			(PRU_GLOBAL_BASE + 0x00026000)

#define PRU_UART0		(PRU_GLOBAL_BASE + 0x00028000)

#define PRU_IEP			(PRU_GLOBAL_BASE + 0x0002E000)

#define PRU_ECAP0		(PRU_GLOBAL_BASE + 0x00030000)

#define PRU0_IRAM		(PRU_GLOBAL_BASE + 0x00034000)
#define PRU1_IRAM		(PRU_GLOBAL_BASE + 0x00038000)

/* PRU Local Offsets */
#define PRU_IRAM		(0x00000000)
#define PRU_DRAM		(0x00000000)
#define PRU_L_SHARED_DRAM	(0x00002000)

/* Sizes */
#define PRU0_IRAM_SIZE		(SZ_8K)
#define PRU1_IRAM_SIZE		(SZ_8K)

#define PRU0_DRAM_SIZE		(SZ_8K)
#define PRU1_DRAM_SIZE		(SZ_8K)

#define PRU_SHARED_DRAM_SIZE	(SZ_8K + SZ_4K)

/*
 * Sizes of the virtqueues (expressed in number of buffers supported,
 * and must be power of 2)
 */
#define PRU_RPMSG_VQ0_SIZE	2
#define PRU_RPMSG_VQ1_SIZE	2

/* flip up bits whose indices represent features we support */
#define RPMSG_PRU_C0_FEATURES	1

/* Definition for unused interrupts */
#define HOST_UNUSED		255

/* Mapping sysevts to a channel. Each pair contains a sysevt, channel */
struct ch_map pru_intc_map[] = { {17, 1}, {18, 0}, {19, 2}, {20, 3}, {21, 0},
				 {22, 1}, {24, 4}, {25, 5}, {26, 6}, {27, 7},
			       };

struct my_resource_table {
	struct resource_table base;

	uint32_t offset[2]; /* Should match 'num' in actual definition */

	/* rpmsg vdev entry */
	struct fw_rsc_vdev rpmsg_vdev;
	struct fw_rsc_vdev_vring rpmsg_vring0;
	struct fw_rsc_vdev_vring rpmsg_vring1;

	/* intc definition */
	struct fw_rsc_custom pru_ints;
};

#pragma DATA_SECTION(am335x_pru_remoteproc_ResourceTable, ".resource_table")
#pragma RETAIN(am335x_pru_remoteproc_ResourceTable)
struct my_resource_table am335x_pru_remoteproc_ResourceTable = {
	1,	/* we're the first version that implements this */
	2,	/* number of entries in the table */
	0, 0,	/* reserved, must be zero */
	/* offsets to entries */
	{
		offsetof(struct my_resource_table, rpmsg_vdev),
		offsetof(struct my_resource_table, pru_ints),
	},

	/* rpmsg vdev entry */
	{
		TYPE_VDEV, VIRTIO_ID_RPMSG, 0,
		RPMSG_PRU_C0_FEATURES, 0, 0, 0, 2, { 0, 0 },
		/* no config data */
	},
	/* the two vrings */
	/* TODO: What to do with vring da? */
	{ 0, 16, PRU_RPMSG_VQ0_SIZE, 1, 0 },
	{ 0, 16, PRU_RPMSG_VQ1_SIZE, 2, 0 },

	{
		TYPE_CUSTOM, TYPE_PRU_INTS,
		sizeof(struct fw_rsc_custom_ints),
		{ /* PRU_INTS version */
                  0x0000,
                  /* Channel-to-host mapping, 255 for unused */
                  0, 1, 2, 3, 0, 6, 1, 7, HOST_UNUSED, HOST_UNUSED,
                  /* Number of evts being mapped to channels */
                  (sizeof(pru_intc_map) / sizeof(struct ch_map)),
                  /* Pointer to the structure containing mapped events */
                  pru_intc_map,
                },
	},
};

#endif /* _RSC_TABLE_AM335X_PRU_H_ */
