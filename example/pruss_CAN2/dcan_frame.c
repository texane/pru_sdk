/**
 * \file   dcan_frame.c
 *
 * \brief  This file consists of wrapper functions which internally call 
 *         DCAN APIs.
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

/* Include the necessary header files */
#include "dcan_frame.h"
#include "../../include/include/hw/hw_dcan.h"
#include "dcan.h"
#include "pru_main.h"

extern unsigned int isr_tx_flag;
extern unsigned int isr_rx_flag;

/**
 * \brief   This function will configure a message object in the DCAN message 
 *          RAM. This function will internally call 'CANTxObjectConfig' or 
 *          'CANRxObjectConfig' functions depending on the flags sent by user.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   canPtr        Pointer to can_frame structure.
 *
 **/
void
CANMsgObjectConfig (unsigned int baseAdd, can_frame* canPtr)
{
  if ((canPtr->flag & CAN_DATA_FRAME) && (canPtr->flag & CAN_MSG_DIR_TX))
    {
      /* Configure a transmit message object */
      CANTxObjectConfig (baseAdd, canPtr);
      isr_tx_flag = 1;
    }
  else if ((canPtr->flag & CAN_DATA_FRAME) && (canPtr->flag & CAN_MSG_DIR_RX))
    {
      /* Configure a receive message object */
      CANRxObjectConfig (baseAdd, canPtr);
      isr_rx_flag = 1;
    }
  else
    {

    }
}

/**
 * \brief   This function will configure a message object in DCAN message RAM as a 
 *          transmit message object.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   canPtr        Pointer to can_frame structure.
 *
 **/
void
CANTxObjectConfig (unsigned int baseAdd, can_frame* canPtr)
{
  unsigned int msgNum;
  unsigned int idLen;

  idLen = (canPtr->flag & CAN_EXT_FRAME) ? DCAN_29_BIT_ID : DCAN_11_BIT_ID;

  /* Set the message valid bit */
  DCANMsgObjValidate (baseAdd, DCAN_IF1_REG);

  /* Set the message id of the frame to be transmitted */
  DCANMsgIdSet (baseAdd, canPtr->id, idLen, DCAN_IF1_REG);

  /* Set the message object direction as transmit */
  DCANMsgDirectionSet (baseAdd, DCAN_TX_DIR, DCAN_IF1_REG);

  /* Set the data length code */
  DCANDataLengthCodeSet (baseAdd, canPtr->dlc, DCAN_IF1_REG);

  /* Write data to the DCAN data registers */
  DCANDataWrite (baseAdd, (canPtr->data), DCAN_IF1_REG);

  /* Enable the transmit interrupt for the message object */
  DCANMsgObjIntEnable (baseAdd, DCAN_TRANSMIT_INT, DCAN_IF1_REG);

  /* Enable the DCAN FIFO End of block */
  DCANFIFOEndOfBlockControl (baseAdd, DCAN_END_OF_BLOCK_ENABLE, DCAN_IF1_REG);

  /* Get the transmit request status */
  msgNum = DCANTxRqstStatGet (baseAdd);

  /* Configure the command register */
  DCANCommandRegSet (baseAdd, (DCAN_DAT_A_ACCESS | DCAN_MSG_WRITE | DCAN_TXRQST_ACCESS |
                               DCAN_DAT_B_ACCESS | DCAN_ACCESS_CTL_BITS |
                               DCAN_ACCESS_ARB_BITS), msgNum, DCAN_IF1_REG);
}

/**
 * \brief   This function will configure a message object in DCAN message RAM 
 *          as a receive message object.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   canPtr        Pointer to can_frame structure.
 *
 **/
void
CANRxObjectConfig (unsigned int baseAdd, can_frame* canPtr)
{
  unsigned int idLen;
  unsigned int msgIndex;

  msgIndex = (CAN_NUM_OF_MSG_OBJS / 2);

  idLen = (canPtr->flag & CAN_EXT_FRAME) ? DCAN_29_BIT_ID : DCAN_11_BIT_ID;

  /* Use Acceptance mask. */
  DCANUseAcceptanceMaskControl (baseAdd, DCAN_MASK_USED, DCAN_IF2_REG);

  /* Configure the DCAN mask registers for acceptance filtering. */
  DCANMsgObjectMskConfig (baseAdd, DCAN_IDENTIFIER_MSK (DCAN_ID_MASK,
                                                        DCAN_ID_MSK_11_BIT), DCAN_MSK_MSGDIR_DISABLE,
                          DCAN_MSK_EXT_ID_ENABLE, DCAN_IF2_REG);

  /* Set the message valid bit */
  DCANMsgObjValidate (baseAdd, DCAN_IF2_REG);

  /* Set the message id of the frame to be received */
  DCANMsgIdSet (baseAdd, canPtr->id, idLen, DCAN_IF2_REG);

  /* Set the message object direction as receive */
  DCANMsgDirectionSet (baseAdd, DCAN_RX_DIR, DCAN_IF2_REG);

  /* Enable the receive interrupt for the message object */
  DCANMsgObjIntEnable (baseAdd, DCAN_RECEIVE_INT, DCAN_IF2_REG);

  /* Enable the FIFO end of block */
  DCANFIFOEndOfBlockControl (baseAdd, DCAN_END_OF_BLOCK_ENABLE, DCAN_IF2_REG);

  /* Check for the message valid status for receive objects */
  while ((DCANMsgValidStatusGet (baseAdd, msgIndex)) &&
         (msgIndex <= (CAN_NUM_OF_MSG_OBJS - 1)))
    {
      msgIndex++;
    }

  /* Configure the command register */
  DCANCommandRegSet (baseAdd, (DCAN_ACCESS_CTL_BITS | DCAN_MSG_WRITE |
                               DCAN_ACCESS_MSK_BITS | DCAN_ACCESS_ARB_BITS),
                     msgIndex, DCAN_IF2_REG);
}

/**
 * \brief   Read data from a message object.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   msgNum        Message object number.
 * \param   data          Pointer to an unsigned integer. Used to fetch data 
 *                        bytes from data registers.
 * \param   ifReg         Interface register set used.
 *
 */
void
CANReadMsgObjData (unsigned int baseAdd, unsigned int msgNum,
                   unsigned int* data, unsigned int ifReg)
{
  /* Read a message object from CAN message RAM to Interface register */
  DCANCommandRegSet (baseAdd, (DCAN_DAT_A_ACCESS | DCAN_DAT_B_ACCESS |
                               DCAN_TXRQST_ACCESS | DCAN_CLR_INTPND |
                               DCAN_ACCESS_CTL_BITS | DCAN_ACCESS_ARB_BITS |
                               DCAN_ACCESS_MSK_BITS | DCAN_MSG_READ),
                     msgNum, ifReg);

  /* Clear the NewData bit */
  DCANNewDataControl (baseAdd, DCAN_NEW_DAT_CLR, ifReg);

  /* Read data bytes from interface register */
  DCANDataRead (baseAdd, data, ifReg);
}

/**
 * \brief   This function should be used to clear the interrupt pending status 
 *          of receive objects after a new message is received. This will clear
 *          the IntPnd status of the message object represented by msgNum.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   msgNum        Message object number.
 * \param   ifReg         Interface register set used.
 *
 **/
void
CANClrIntPndStat (unsigned int baseAdd, unsigned int msgNum,
                  unsigned int ifReg)
{
  /* Clear the IntPnd bit of DCAN_IFMCTL register */
  DCANClrIntPnd (baseAdd, ifReg);

  /* Set the ClrIntPnd bit of DCAN_IFCMD register */
  DCANCommandRegSet (baseAdd, DCAN_CLR_INTPND, msgNum, ifReg);
}

/**
 * \brief   This function can be used by the user to validate a message object.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   msgNum        Message object number.
 * \param   ifReg         Interface register set used.
 *
 **/
void
CANValidateMsgObject (unsigned int baseAdd, unsigned int msgNum,
                      unsigned int ifReg)
{
  /* Set the MsgVal bit of DCAN_IFARB register */
  DCANMsgObjValidate (baseAdd, ifReg);

  /* Set the Arb bit of DCAN_IFCMD register */
  DCANCommandRegSet (baseAdd, (DCAN_ACCESS_ARB_BITS | DCAN_MSG_WRITE),
                     msgNum, ifReg);
}

/**
 * \brief   This function can be used by the user to invalidate a message 
 *          object.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   msgNum        Message object number.
 * \param   ifReg         Interface register set used.
 *
 **/
void
CANInValidateMsgObject (unsigned int baseAdd, unsigned int msgNum,
                        unsigned int ifReg)
{
  /* Clear the MsgVal bit of DCAN_IFARB register */
  DCANMsgObjInvalidate (baseAdd, ifReg);

  /* Set the Arb bit of DCAN_IFCMD register */
  DCANCommandRegSet (baseAdd, (DCAN_ACCESS_ARB_BITS | DCAN_MSG_WRITE),
                     msgNum, ifReg);
}

/**
 * \brief   This function can be used to disable the transmit interrupt of a 
 *          message object.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   msgNum        Message object number.
 * \param   ifReg         Interface register set used.
 *
 **/
void
CANTxIntDisable (unsigned int baseAdd, unsigned int msgNum,
                 unsigned int ifReg)
{
  /* Disable the message object transmit interrupt */
  DCANMsgObjIntDisable (baseAdd, DCAN_TRANSMIT_INT, ifReg);

  /* Set the CTL bit of the command register */
  DCANCommandRegSet (baseAdd, (DCAN_ACCESS_CTL_BITS | DCAN_MSG_WRITE),
                     msgNum, ifReg);
}

/**
 * \brief   This function can be used to disable the receive interrupt of a 
 *          message object.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   msgNum        Message object number.
 * \param   ifReg         Interface register set used.
 *
 **/
void
CANRxIntDisable (unsigned int baseAdd, unsigned int msgNum,
                 unsigned int ifReg)
{
  /* Disable the message object receive interrupt */
  DCANMsgObjIntDisable (baseAdd, DCAN_RECEIVE_INT, ifReg);

  /* Set the CTL bit of the command register */
  DCANCommandRegSet (baseAdd, (DCAN_ACCESS_CTL_BITS | DCAN_MSG_WRITE),
                     msgNum, ifReg);
}

/**
 * \brief   This function can be used to update the data bytes of a transmit 
 *          object and set TxRqst for this message object.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   dataPtr       Pointer to unsigned integer. Used to write data 
 *                        bytes to data registers.
 * \param   msgNum        Message object number.
 * \param   ifReg         Interface register set used.
 *
 **/
void
CANUpdateDataBytes (unsigned int baseAdd, unsigned int* dataPtr,
                    unsigned int msgNum, unsigned int ifReg)
{
  /* Populate the data bytes in the data registers */
  DCANDataWrite (baseAdd, dataPtr, ifReg);

  /* Set the DataA, DataB, TxRqst and WR of the IF_CMD register */
  DCANCommandRegSet (baseAdd, (DCAN_DAT_A_ACCESS | DCAN_DAT_B_ACCESS |
                               DCAN_TXRQST_ACCESS | DCAN_MSG_WRITE),
                     msgNum, ifReg);
}

/**
 * \brief   This function takes I/P Clk frequency, required bit-rate on the 
 *          CAN bus and calculates the value to be programmed to BTR register
 *          and sends the BTR value to 'DCANBitTimingConfig' API.
 *
 * \param   baseAdd       Base Address of the DCAN Module Registers.
 * \param   clkFreq       I/P clock frequency to DCAN controller.
 * \param   bitRate       Required bit-rate on the CAN bus.
 *
 * \return  Returns the error value if error is present.
 *
 **/

unsigned int
CANSetBitTiming (unsigned int baseAdd, unsigned int clkFreq,
                 unsigned int bitRate)
{
  unsigned int errVal = 0;


  /* Set the BTR value to the DCAN bittiming register */
  DCANBitTimingConfig (baseAdd, 9985u);

  return errVal;
}
