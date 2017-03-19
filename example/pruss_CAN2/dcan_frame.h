/**
 * \file   dcan_frame.h
 *
 * \brief  This file consists of macro definitions and function prototypes
 *         used in dcan_frame.c file.
 *
 */

/*
 * Copyright (C) 2005 Marc Kleine-Budde, Pengutronix
 * Copyright (C) 2006 Andrey Volkov, Varma Electronics
 * Copyright (C) 2008-2009 Wolfgang Grandegger <wg@grandegger.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the version 2 of the GNU General Public License
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _DCAN_FRAME_H_
#define _DCAN_FRAME_H_

#ifdef __cplusplus
extern "C" {
#endif

    /******************************************************************************
     **                          MACRO DEFINITIONS
     ******************************************************************************/
    /*
     ** These macros can be used as 'flag' present in the 'can_frame' structure.
     */
#define CAN_DATA_FRAME                  (0x00000001u)
#define CAN_EXT_FRAME                   (0x00000004u)
#define CAN_MSG_DIR_TX                  (0x00000010u)
#define CAN_MSG_DIR_RX                  (0x00000020u)

    /* Total No. of message objects available in the DCAN message RAM */
#define CAN_NUM_OF_MSG_OBJS             (64u)

    /* Macros used by CANBitTimeCalculator */
#define CAN_CALC_MAX_ERROR              (50u)
#define BIT_RATE_ERR_WARN               (2u)
#define BIT_RATE_ERR_MAX                (1u)
#define EXTRACT_BRPE_VAL                (0x3C0u)
#define NO_BIT_RATE_ERR                 (3u)
#define BRPE_SHIFT                      (10u)

    /******************************************************************************/
    /*                        STRUCTURE DECLARATIONS                              */
    /******************************************************************************/

    /*
     ** Structure which will hold information related to a CANframe which will be 
     ** stored in a message object.
     */
    typedef struct _can_frame {
        unsigned int flag;
        unsigned int id;
        unsigned int dlc;
        unsigned int *data;
    } can_frame;

    /*
     ** Structure used by the CAN Bit time calculator. Will hold values related to 
     ** bit-time configuration.
     */
    struct _dcan_bittiming {
        unsigned int bitRate; /* Bit-rate in bits/second */
        unsigned int samplePnt; /* Sample point in one-tenth of a percent */
        unsigned long tq; /* Time quanta (tq) in nanoseconds */
        unsigned int propSeg; /* Propagation segment in tq */
        unsigned int phaseSeg1; /* Phase buffer segment 1 in tq */
        unsigned int phaseSeg2; /* Phase buffer segment 2 in tq */
        unsigned int sjw; /* Synchronisation jump width in tq */
        unsigned int brp; /* Bit-rate prescaler */
    };

    /*
     ** Structure used by the CAN Bit time calculator. Will hold values related to 
     ** bit-time configuration.
     */
    struct _dcan_hw_params {
        unsigned int tseg1Min; /* Time segement 1 = prop_seg + phase_seg1 */
        unsigned int tseg1Max;
        unsigned int tseg2Min; /* Time segement 2 = phase_seg2 */
        unsigned int tseg2Max;
        unsigned int sjwMax; /* Synchronisation jump width */
        unsigned int brpMin; /* Bit-rate prescaler */
        unsigned int brpMax;
        unsigned int brpInc;
    };

    /******************************************************************************
     **                         FUNCTION PROTOTYPES
     ******************************************************************************/
    extern void CANMsgObjectConfig(unsigned int baseAdd, can_frame* canPtr);
    extern void CANTxObjectConfig(unsigned int baseAdd, can_frame* canPtr);
    extern void CANRxObjectConfig(unsigned int baseAdd, can_frame* canPtr);
    extern void CANClrIntPndStat(unsigned int baseAdd, unsigned int msgNum,
            unsigned int ifReg);
    extern void CANValidateMsgObject(unsigned int baseAdd, unsigned int msgNum,
            unsigned int ifReg);
    extern void CANInValidateMsgObject(unsigned int baseAdd, unsigned int msgNum,
            unsigned int ifReg);
    extern void CANReadMsgObjData(unsigned int baseAdd, unsigned int msgNum,
            unsigned int* data, unsigned int ifReg);
    extern void CANTxIntDisable(unsigned int baseAdd, unsigned int msgNum,
            unsigned int ifReg);
    extern void CANRxIntDisable(unsigned int baseAdd, unsigned int msgNum,
            unsigned int ifReg);
    extern void CANUpdateDataBytes(unsigned int baseAdd, unsigned int* dataPtr,
            unsigned int msgNum, unsigned int ifReg);
    /* Bit timing calculation functions */
    extern int canUpdatSamPt(const struct _dcan_hw_params *ptr,
            int sampl_pt, int tseg, int *tseg1, int *tseg2);

    extern unsigned int CANSetBitTiming(unsigned int baseAdd, unsigned int clkFreq,
            unsigned int bitRate);

#ifdef __cplusplus
}
#endif

#endif

/*************************** END OF FILE **************************************/
