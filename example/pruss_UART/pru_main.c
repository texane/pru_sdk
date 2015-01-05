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
#include "../../include/include/uart_irda_cir.h"
#include "../../include/include/consoleUtils.h"
#include "pru_hal.h"


/******************************************************************************
 **                       INTERNAL MACRO DEFINITIONS
 ******************************************************************************/
#define BAUD_RATE_115200           (115200u)
#define UART_MODULE_INPUT_CLK      (48000000u)

#define START_ADDR_DDR             (0x80000000u)
#define START_ADDR_DEV             (0x44000000u)
#define START_ADDR_OCMC            (0x40300000u)
#define NUM_SECTIONS_DDR           (512u)
#define NUM_SECTIONS_DEV           (960u)
#define NUM_SECTIONS_OCMC          (1u)

#define UART_INSTANCE_BASE_ADD     (SOC_UART_0_REGS)
#define UART_INSTANCE_INT_NUM      (SYS_INT_UART0INT)

/* Baud Rates of UART for communication. */
#define BAUD_RATE_115200          (115200u)
#define BAUD_RATE_128000          (128000u)
#define BAUD_RATE_230400          (230400u)
#define BAUD_RATE_460800          (460800u)
#define BAUD_RATE_921600          (921600u)

#define APP_EXIT                  (FALSE)
#define APP_CONTINUE              (TRUE)
#define NUM_TX_STRINGS            (2)


/******************************************************************************
 **                       INTERNAL FUNCTION PROTOTYPES                     
 ******************************************************************************/
#define MEM_SIZE 20
#define REG_Len 4

static void UartFIFOConfigure (void);
static void UartLineCharacSet (unsigned int lineCharConfig);
static void UartBaudRateSet (unsigned int baudRate);
static void UartConfigure (unsigned int baudRate, unsigned int lineCharConfig);


/******************************************************************************
 **                       GLOBAL VARIABLE DEFINITIONS                   
 ******************************************************************************/

/******************************************************************************
 **                      INTERNAL FUNCTION DEFINITIONS
 ******************************************************************************/
int
main (void)
{
  unsigned int i = 0;
  ocp_init ();
  shm_init ();

  for (i = 0; i < MEM_SIZE; i++)
    {
      shm_write_uint32 (REG_Len*i, 0xdeadbeef); //initiate the shared memory
    }

  // HWREG(0x00012004) = 0;
  /* Configuring the system clocks for UART0 instance. */
  UART0ModuleClkConfig ();
  shm_write_uint32 (REG_Len * 0, 0);
  /* Performing a module reset. */
  UARTModuleReset (UART_INSTANCE_BASE_ADD);
  shm_write_uint32 (REG_Len * 0, 1);
  /* Performing FIFO configurations. */
  UartFIFOConfigure ();
  shm_write_uint32 (REG_Len * 0, 2);
  UartConfigure (BAUD_RATE_115200, (UART_FRAME_WORD_LENGTH_8 |
                                    UART_FRAME_NUM_STB_1 |
                                    UART_PARITY_NONE));
  shm_write_uint32 (REG_Len * 0, 3);
  /* Select the console type based on compile time check */
  ConsoleUtilsSetType (CONSOLE_UART);


  /* Core loop */
  i = 0;
  while (1)
    {
      shm_write_uint32 (REG_Len * 0, i++);
      shm_write_uint32 (REG_Len * 1, HWREG (SOC_DCAN_0_REGS));

    }
}

/*
 ** A wrapper function performing FIFO configurations.
 */

static void
UartFIFOConfigure (void)
{
  unsigned int fifoConfig = 0;

  /*
   ** - Transmit Trigger Level Granularity is 4
   ** - Receiver Trigger Level Granularity is 1
   ** - Transmit FIFO Space Setting is 56. Hence TX Trigger level
   **   is 8 (64 - 56). The TX FIFO size is 64 bytes.
   ** - The Receiver Trigger Level is 1.
   ** - Clear the Transmit FIFO.
   ** - Clear the Receiver FIFO.
   ** - DMA Mode enabling shall happen through SCR register.
   ** - DMA Mode 0 is enabled. DMA Mode 0 corresponds to No
   **   DMA Mode. Effectively DMA Mode is disabled.
   */
  fifoConfig = UART_FIFO_CONFIG (UART_TRIG_LVL_GRANULARITY_4,
                                 UART_TRIG_LVL_GRANULARITY_1,
                                 UART_FCR_TX_TRIG_LVL_56,
                                 1,
                                 1,
                                 1,
                                 UART_DMA_EN_PATH_SCR,
                                 UART_DMA_MODE_0_ENABLE);

  /* Configuring the FIFO settings. */
  UARTFIFOConfig (UART_INSTANCE_BASE_ADD, fifoConfig);
}

/*
 ** A wrapper function performing UART Line Characteristics Configuration.
 */

static void
UartLineCharacSet (unsigned int lineCharConfig)
{
  unsigned int wLenStbFlag = 0;
  unsigned int parityFlag = 0;

  wLenStbFlag = (lineCharConfig & (UART_LCR_NB_STOP | UART_LCR_CHAR_LENGTH));
  parityFlag = (lineCharConfig & (UART_LCR_PARITY_TYPE2 |
                                  UART_LCR_PARITY_TYPE1 |
                                  UART_LCR_PARITY_EN));

  UARTLineCharacConfig (UART_INSTANCE_BASE_ADD, wLenStbFlag, parityFlag);
}

/*
 ** A wrapper function performing UART Baud Rate settings.
 */

static void
UartBaudRateSet (unsigned int baudRate)
{
  unsigned int divisorValue = 0;

  /* Computing the Divisor Value. */
  divisorValue = UARTDivisorValCompute (UART_MODULE_INPUT_CLK,
                                        baudRate,
                                        UART16x_OPER_MODE,
                                        UART_MIR_OVERSAMPLING_RATE_42);

  /* Programming the Divisor Latches. */
  UARTDivisorLatchWrite (UART_INSTANCE_BASE_ADD, divisorValue);
}

/*
 ** A wrapper function performing UART Configurations.
 */

static void
UartConfigure (unsigned int baudRate, unsigned int lineCharConfig)
{
  UartBaudRateSet (baudRate);

  /* Switching to Configuration Mode B. */
  UARTRegConfigModeEnable (UART_INSTANCE_BASE_ADD, UART_REG_CONFIG_MODE_B);

  UartLineCharacSet (lineCharConfig);

  /* Disabling write access to Divisor Latches. */
  UARTDivisorLatchDisable (UART_INSTANCE_BASE_ADD);

  /* Disabling Break Control. */
  UARTBreakCtl (UART_INSTANCE_BASE_ADD, UART_BREAK_COND_DISABLE);

  /* Switching to UART16x operating mode. */
  UARTOperatingModeSelect (UART_INSTANCE_BASE_ADD, UART16x_OPER_MODE);
}
