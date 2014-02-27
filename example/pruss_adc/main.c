/* signals */
/* ain0, p9.39 */


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"
#include "../common/mio.h"


#define PERROR() printf("[!] %s:%u\n", __FUNCTION__, __LINE__)


/* cm */

/* doc: spruh73c, table 2.2 */
#define CM_WKUP_MIO_ADDR 0x44e00400
#define CM_WKUP_MIO_SIZE 0x1000

/* doc: spruh73c, table 8.28 */
#define CM_WKUP_ADC_TSC_CLKCTRL 0xbc

static int cm_wkup_enable_adc_tsc(void)
{
  mio_handle_t mio;

  if (mio_open(&mio, CM_WKUP_MIO_ADDR, CM_WKUP_MIO_SIZE)) return -1;
  mio_write_uint32(&mio, CM_WKUP_ADC_TSC_CLKCTRL, 2);
  mio_close(&mio);

  return 0;
}


/* tsc adc */

/* doc: spruh73c, table 2.2 */
#define ADC_MIO_ADDR 0x44e0d000
#define ADC_MIO_SIZE (8 * 1024)

/* doc: spruh73c, table 12.4 */
#define ADC_REG_SYSCONFIG 0x10
#define ADC_REG_IRQENABLE_SET 0x2c
#define ADC_REG_IRQWAKEUP 0x34
#define ADC_REG_DMAENABLE_SET 0x38
#define ADC_REG_CTRL 0x40
#define ADC_REG_ADC_CLKDIV 0x4c
#define ADC_REG_STEPENABLE 0x54
#define ADC_REG_TS_CHARGE_DELAY 0x60
#define ADC_REG_STEP1CONFIG 0x64
#define ADC_REG_FIFO0COUNT 0xe4
#define ADC_REG_FIFO0DATA 0x100

static int adc_open(mio_handle_t* mio)
{
  /* enable adc_tsc clocking */
  if (cm_wkup_enable_adc_tsc()) return -1;

  if (mio_open(mio, ADC_MIO_ADDR, ADC_MIO_SIZE)) return -1;

  /* disable the module, enable step registers writing */
  mio_write_uint32(mio, ADC_REG_CTRL, 1 << 2);

  /* setup adc */
  mio_write_uint32(mio, ADC_REG_SYSCONFIG, 1 << 2);
  mio_write_uint32(mio, ADC_REG_IRQENABLE_SET, 0);
  mio_write_uint32(mio, ADC_REG_IRQWAKEUP, 0);
  mio_write_uint32(mio, ADC_REG_DMAENABLE_SET, 0);
  mio_write_uint32(mio, ADC_REG_ADC_CLKDIV, 0);
  mio_write_uint32(mio, ADC_REG_TS_CHARGE_DELAY, 1);

  /* enable channel 1 */
  mio_write_uint32(mio, ADC_REG_STEPENABLE, (1 << 1) | (1 << 0));

  /* channel 1, continuous mode */
  mio_write_uint32(mio, ADC_REG_STEP1CONFIG, 1);

  /* enable the module */
  mio_write_uint32(mio, ADC_REG_CTRL, 1);

  return 0;
}

static int adc_close(mio_handle_t* mio)
{
  mio_close(mio);
  return 0;
}

static uint32_t adc_read(mio_handle_t* mio)
{
  static const size_t sharedram_offset = 2048;
  volatile uint32_t* p;
  prussdrv_map_prumem(4, (void**)&p);
  return p[sharedram_offset + 0];
}


/* main */

int main(int ac, char** av)
{
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  mio_handle_t mio;

  if (adc_open(&mio))
  {
    PERROR();
    return -1;
  }

  prussdrv_init();

  if (prussdrv_open(PRU_EVTOUT_0))
  {
    printf("prussdrv_open open failed\n");
    adc_close(&mio);
    return -1;
  }

  prussdrv_pruintc_init(&pruss_intc_initdata);

  /* execute code on pru0 */
#define PRU_NUM 0
  prussdrv_exec_program(PRU_NUM, "./adc.bin");

  /* wait until pru0 finishes */
  prussdrv_pru_wait_event(PRU_EVTOUT_0);
  prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);

  printf("0x%08x\n", adc_read(&mio));

  adc_close(&mio);

  /* disable pru and close memory mapping */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();

  return 0;
}
