/**
 * \file   dcanLpBk.c
 *
 * \brief  The application will loopback the CAN frames received by
 *         it back to the sender.
 *
 *         Application Configuration:
 *
 *             Modules Used:
 *                 DCAN0
 *                 UART0
 *
 *             Configurable Parameters:
 *                 1) CAN frame data: 1 - 8 bytes
 *
 *             Hard-code configuartion of other parameters:
 *                 1) CAN communication using two nodes
 *                 2) CAN frame support - Data frames
 *                 3) Configured to Receive/Transmit CAN frames
 *                 4) Operating Speed: 1Mbps
 *
 *         Application Use Case:
 *             The application performs reception/transmission of CAN frames
 *             between two CAN nodes. This example configures the CAN controller
 *             to receive CAN frames.The received CAN frames are transmitted
 *             back on the CAN bus. The receive message objects are configured
 *             to receive CAN frames with any ID.
 *
 *         Running the example:
 *             Before execution of the example please connect the EVMs back to
 *             back using straight cables for DB9 connectors and on execution
 *             the application is configured to recive CAN frames. Upon
 *             reception the received CAN frame is transmitted back on the
 *             CAN bus.
 *
 */


/* Include the necessary header files */
//#include "consoleUtils.h"
#include "../../include/include/hw/soc_AM335x.h"
#include "../../include/include/armv7a/am335x/evmAM335x.h"
#include "../../include/include/pruss.h"
#include "../../include/include/hw/hw_types.h"
#include "../../include/include/hw/hw_control_AM335x.h"
#include "dcan.h"
#include "pru_hal.h"
#include "dcan_frame.h"

/******************************************************************************
 **                       INTERNAL MACRO DEFINITIONS
 ******************************************************************************/
#define DCAN_NO_INT_PENDING               (0x00000000u)
#define DCAN_IN_CLK                       (24000000u)
#define DCAN_ERROR_OCCURED                (0x8000u)
#define DCAN_BIT_RATE                     (1000000u)
#define CAN_TX_MSG_EXTD_ID                (0x1000u)
#define CAN_TX_MSG_STD_ID                 (0x02u)

#define REG_Len                 (4)
#define MEM_SIZE                (16)

/******************************************************************************
 **                       INTERNAL FUNCTION PROTOTYPES                     
 ******************************************************************************/
static void DCANAintcConfigure (void);
static void ConfigureDCAN (void);
static void DCANIsr0 (void);

/******************************************************************************
 **                       GLOBAL VARIABLE DEFINITIONS                   
 ******************************************************************************/
/* CAN frame details */
static unsigned int rxflag = (CAN_DATA_FRAME | CAN_MSG_DIR_RX);
static volatile unsigned int isrRxFlag = 1;
static volatile unsigned int isrFlag = 1;
static unsigned int canData[2];
static unsigned int canId = 2;
static unsigned int canId_Tx = 2;
static unsigned int bytes = 0;
static unsigned int rx_count = 0;
static unsigned int tx_count = 0;
unsigned int tx_flag = 0;
can_frame entryRx;
can_frame entryTx;

//0x87654321 0x90
//the data to be sent need to be two 32bits variables
unsigned int data_Tx[] = {(0x00000012 | 0x00003400 | 0x00560000 | 0x78000000), 0x90};
//unsigned int data_Tx[] = {0x12, 0x34, 0x56, 0x78, 0x90};

/******************************************************************************
 **                      INTERNAL FUNCTION DEFINITIONS
 ******************************************************************************/
int
main (void)
{
  unsigned int index = 0, i = 0;
  ocp_init ();
  shm_init ();

  for (i = 0; i < MEM_SIZE; i++)
    {
      shm_write_uint32 (REG_Len*i, 0xdeadbeef); //initiate the shared memory
    }

  // HWREG(0x00012004) = 0;

  /* Enable the DCAN0 module clock */
  DCANModuleClkConfig ();

  /* Perform the pinmux for DCAN0 */
  DCANPinMuxSetUp ();

  /* Initialize the DCAN message RAM */
  DCANMsgRAMInit (1);

  /* Initialize the UART console */
  // ConsoleUtilsInit();

  /* Select the console type based on compile time check */
  //  ConsoleUtilsSetType(CONSOLE_UART);

  /* Enable the processor IRQ */
  //   IntMasterIRQEnable();

  /* Register the DCAN interrupts */
  DCANAintcConfigure ();

  //  ConsoleUtilsPrintf("*** Waiting for data ***\n\n");

  /* Perform the DCAN configuration */
  ConfigureDCAN ();

  index = CAN_NUM_OF_MSG_OBJS;

  while (index--)
    {
      /* Invalidate all message objects in the message RAM */
      CANInValidateMsgObject (SOC_DCAN_0_REGS, index, DCAN_IF1_REG);
      CANInValidateMsgObject (SOC_DCAN_0_REGS, index, DCAN_IF2_REG);
    }
  entryRx.flag = rxflag;
  entryRx.id = canId;

  /* 
   ** Configure a receive message object to accept CAN 
   ** frames with standard ID.
   */
  CANMsgObjectConfig (SOC_DCAN_0_REGS, &entryRx);

  entryTx.flag = (CAN_MSG_DIR_TX | CAN_DATA_FRAME);
  entryTx.id = canId_Tx;
  // entryTx.dlc = sizeof (data_Tx) / sizeof (data_Tx[0]);
  entryTx.dlc = 5; // this is the number count by char(1 Byte)
  entryTx.data = data_Tx;

  /*
   ** Configure a transmit message object to transmit CAN
   ** frames with standard ID.
   */

  CANMsgObjectConfig (SOC_DCAN_0_REGS, &entryTx);

  /* Start the CAN transfer */
  DCANNormalModeSet (SOC_DCAN_0_REGS);

  /* Enable the error interrupts */
  DCANIntEnable (SOC_DCAN_0_REGS, DCAN_ERROR_INT);

  /* Enable the interrupt line 0 of DCAN module */
  DCANIntLineEnable (SOC_DCAN_0_REGS, DCAN_INT_LINE0);

  /* Core loop */
  i = 0;
  while (1)
    {
      shm_write_uint32 (REG_Len * 0, i++);
      shm_write_uint32 (REG_Len * 1, HWREG (SOC_DCAN_0_REGS + DCAN_ES));
      shm_write_uint32 (REG_Len * 2, HWREG (SOC_DCAN_0_REGS + DCAN_BTR));
      shm_write_uint32 (REG_Len * 3, DCANIntRegStatusGet (SOC_DCAN_0_REGS, DCAN_INT_LINE0_STAT));
      shm_write_uint32 (REG_Len * 4, DCANIFMsgCtlStatusGet (SOC_DCAN_0_REGS, DCAN_IF1_REG));
      shm_write_uint32 (REG_Len * 5, DCANIFMsgCtlStatusGet (SOC_DCAN_0_REGS, DCAN_IF2_REG));
      shm_write_uint32 (REG_Len * 6, rx_count);
      shm_write_uint32 (REG_Len * 7, tx_count);
      shm_write_uint32 (REG_Len * 8, HWREG (SOC_CONTROL_REGS + CONTROL_CONF_UART_CTSN (1)));
      shm_write_uint32 (REG_Len * 9, HWREG (SOC_CONTROL_REGS + CONTROL_CONF_UART_RTSN (1)));

      DCANIsr0 ();

      entryTx.data[0] = i;
      if (tx_flag == 0)
        {
          CANMsgObjectConfig (SOC_DCAN_0_REGS, &entryTx);
        }

      __delay_cycles (100000000);
    }
}

/*
 ** This function will configure DCAN with the required parameters.
 */
static void
ConfigureDCAN (void)
{
  /* Reset the DCAN module */
  DCANReset (SOC_DCAN_0_REGS);

  /* Enter the Initialization mode of CAN controller */
  DCANInitModeSet (SOC_DCAN_0_REGS);

  /* Enable the write access to the DCAN configuration registers */
  DCANConfigRegWriteAccessControl (SOC_DCAN_0_REGS, DCAN_CONF_REG_WR_ACCESS_ENABLE);

  /* Configure the bit timing values for CAN communication */
  CANSetBitTiming (SOC_DCAN_0_REGS, DCAN_IN_CLK, DCAN_BIT_RATE);

  // set it to test mode:
  DCANTestModeControl (SOC_DCAN_0_REGS, DCAN_TEST_MODE_ENABLE);
  // internal loop-back
  // DCANTestModesEnable (SOC_DCAN_0_REGS, DCAN_TST_LPBCK_MD);
  // external loop-back
  DCANTestModesEnable (SOC_DCAN_0_REGS, DCAN_TST_EXTLPBCK_MD);

  /* Disable the write access to the DCAN configuration registers */
  DCANConfigRegWriteAccessControl (SOC_DCAN_0_REGS, DCAN_CONF_REG_WR_ACCESS_DISABLE);
}

/*
 ** DCAN Isr for Interrupt line 0.
 */
static void
DCANIsr0 (void)
{
  unsigned char *dataPtr;
  unsigned int index = 0;
  unsigned int errVal;
  unsigned int msgNum;

  while (DCANIntRegStatusGet (SOC_DCAN_0_REGS, DCAN_INT_LINE0_STAT))
    {
      if (DCANIntRegStatusGet (SOC_DCAN_0_REGS, DCAN_INT_LINE0_STAT) ==
          DCAN_ERROR_OCCURED)
        {
          /* Check the status of DCAN Status and error register */
          errVal = DCANErrAndStatusRegInfoGet (SOC_DCAN_0_REGS);

          if (errVal & DCAN_MOD_IN_BUS_OFF_STATE)
            {
              //     ConsoleUtilsPrintf("**DCAN is in Bus-off state**\n");

              /* 
               ** This feature will automatically get the CAN bus to bus-on 
               ** state once the error counters are below the error warning 
               ** limit. 
               */
              DCANAutoBusOnControl (SOC_DCAN_0_REGS, DCAN_AUTO_BUS_ON_ENABLE);
            }

          if (errVal & DCAN_ERR_WARN_STATE_RCHD)
            {
              //     ConsoleUtilsPrintf("At least one of the error counters have");
              //     ConsoleUtilsPrintf(" reached the error warning limit\n");
            }
        }

      if ((DCANIntRegStatusGet (SOC_DCAN_0_REGS, DCAN_INT_LINE0_STAT) !=
           DCAN_NO_INT_PENDING) &&
          ((DCANIntRegStatusGet (SOC_DCAN_0_REGS, DCAN_INT_LINE0_STAT) !=
            DCAN_ERROR_OCCURED)))
        {
          /* Get the number of the message object which caused the interrupt */
          msgNum = DCANIntRegStatusGet (SOC_DCAN_0_REGS, DCAN_INT_LINE0_STAT);
          shm_write_uint32 (REG_Len * 10, msgNum);

          /* Interrupt handling for transmit objects */
          if (msgNum < (CAN_NUM_OF_MSG_OBJS / 2))
            {
              /* Clear the Interrupt pending status */
              CANClrIntPndStat (SOC_DCAN_0_REGS, msgNum, DCAN_IF2_REG);
              tx_count++;
              tx_flag = 0;
            }

          if ((msgNum >= (CAN_NUM_OF_MSG_OBJS / 2)) && (msgNum < CAN_NUM_OF_MSG_OBJS))
            {
              /* Read a received message from message RAM to interface register */
              CANReadMsgObjData (SOC_DCAN_0_REGS, msgNum, (unsigned int*) canData,
                                 DCAN_IF2_REG);
              rx_count++;
              if ((DCANIFArbStatusGet (SOC_DCAN_0_REGS, DCAN_IF2_REG) &
                   DCAN_EXT_ID_READ) == DCAN_29_BIT_ID)
                {
                  entryRx.flag = (CAN_EXT_FRAME | CAN_DATA_FRAME | CAN_MSG_DIR_TX);
                  entryRx.id = CAN_TX_MSG_EXTD_ID;
                }
              else
                {
                  entryRx.flag = (CAN_DATA_FRAME | CAN_MSG_DIR_TX);
                  entryRx.id = CAN_TX_MSG_STD_ID;
                }

              /* Clear the Interrupt pending status */
              CANClrIntPndStat (SOC_DCAN_0_REGS, msgNum, DCAN_IF2_REG);

              dataPtr = (unsigned char*) canData;

              //     ConsoleUtilsPrintf("Data received = ");

              bytes = (DCANIFMsgCtlStatusGet (SOC_DCAN_0_REGS, DCAN_IF2_REG) &
                       DCAN_DAT_LEN_CODE_READ);
              shm_write_uint32 (REG_Len * (11 + index), bytes);

              //  shm_write_uint32 (REG_Len * (12), *canData);
              // shm_write_uint32 (REG_Len * (13), *(canData + 1));
              /* Print the received data bytes on the UART console */
              for (index = 0; index < bytes; index++)
                {
                  // ConsoleUtilsPrintf("%c", *dataPtr++);

                  // store char data in 32bits, so only low 1 Byte is used
                  shm_write_uint32 (REG_Len * (14 + index), *(dataPtr++));
                }

              //     ConsoleUtilsPrintf("\r\n");

              //   isrFlag = 0;

              /* Populate the can_frame structure with the CAN frame information */
              //    entryRx.dlc = bytes;
              //    entryRx.data = (unsigned int*) canData;

              /* Configure a transmit message object */
              //CANMsgObjectConfig(SOC_DCAN_0_REGS, &entry);
            }
        }
    }
}

/* Interrupt mapping to AINTC and registering CAN ISR */
static void
DCANAintcConfigure (void) {
  //TODO
  /* Set up the ARM interrupt controller */
  //  IntAINTCInit();

  /* Register the DCAN Interrupt handler for interrupt line 0*/
  //   IntRegister(SYS_INT_DCAN0_INT0, DCANIsr0);

  /* Assign priority to the interrupt */
  //   IntPrioritySet(SYS_INT_DCAN0_INT0, 0, AINTC_HOSTINT_ROUTE_IRQ);

  /* Enable the system interrupts in AINTC */
  //  IntSystemEnable(SYS_INT_DCAN0_INT0);

  /* Register the DCAN Interrupt handler for parity interrupt */
  //   IntRegister(SYS_INT_DCAN0_PARITY, DCANParityIsr);

  /* Assign priority to the interrupt */
  //   IntPrioritySet(SYS_INT_DCAN0_PARITY, 0, AINTC_HOSTINT_ROUTE_IRQ);

  /* Enable the system interrupts in AINTC */
  //   IntSystemEnable(SYS_INT_DCAN0_PARITY);


  // map host 0-7 to channel 0-7
  // HWREG(AM33XX_INTC_PHYS_BASE + PRU_INTC_HMR1_REG) = 0x03020100;
  //  HWREG(AM33XX_INTC_PHYS_BASE + PRU_INTC_HMR2_REG) = 0x07060504;

  //map system event 40,41,42,43 to channel 0 
  // HWREG(AM33XX_INTC_PHYS_BASE + PRU_INTC_CMR10_REG) = 0x00000000;
}
