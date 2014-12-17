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
#include "pru_hal.h"


/******************************************************************************
 **                       INTERNAL MACRO DEFINITIONS
 ******************************************************************************/


/******************************************************************************
 **                       INTERNAL FUNCTION PROTOTYPES                     
 ******************************************************************************/
#define MEM_SIZE 20
#define REG_Len 4

/******************************************************************************
 **                       GLOBAL VARIABLE DEFINITIONS                   
 ******************************************************************************/



/******************************************************************************
 **                      INTERNAL FUNCTION DEFINITIONS
 ******************************************************************************/
int
main (void)
{
  unsigned int  i = 0;
  ocp_init ();
  shm_init ();

  for (i = 0; i < MEM_SIZE; i++)
    {
      shm_write_uint32 (REG_Len*i, 0xdeadbeef); //initiate the shared memory
    }

  // HWREG(0x00012004) = 0;


  /* Core loop */
  i = 0;
  while (1)
    {
      shm_write_uint32 (REG_Len * 0, i++);
      shm_write_uint32 (REG_Len * 1, HWREG (SOC_DCAN_0_REGS ));
 
    }
}
