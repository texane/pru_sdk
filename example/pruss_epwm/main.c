#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"
#include "../common/mio.h"


#define PERROR() printf("[!] %s:%u\n", __FUNCTION__, __LINE__)


/* cm */

/* doc: spruh73c, table 2.2 */
#define CM_PER_MIO_ADDR 0x44e00000
#define CM_PER_MIO_SIZE (128 * 1024)

/* doc: spruh73c, table 8.28 */
#define CM_PER_EPWMSS0_CLKCTRL 0xd4
#define CM_PER_EPWMSS1_CLKCTRL 0xcc
#define CM_PER_EPWMSS2_CLKCTRL 0xd8

static int cm_per_enable_pwmss(size_t i)
{
  /* enable clocking of the pwmss[i] */

  static const size_t off[] =
  {
    CM_PER_EPWMSS0_CLKCTRL,
    CM_PER_EPWMSS1_CLKCTRL,
    CM_PER_EPWMSS2_CLKCTRL
  };

  mio_handle_t mio;

  if (mio_open(&mio, CM_PER_MIO_ADDR, CM_PER_MIO_SIZE)) return -1;
  mio_write_uint32(&mio, off[i], 2);
  mio_close(&mio);

  return 0;
}


/* main */

int main(int ac, char** av)
{
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  unsigned int err;

  cm_per_enable_pwmss(0);
  cm_per_enable_pwmss(1);
  cm_per_enable_pwmss(2);

  prussdrv_init();

  err = prussdrv_open(PRU_EVTOUT_0);
  if (err)
  {
    printf("prussdrv_open open failed\n");
    return err;
  }

  prussdrv_pruintc_init(&pruss_intc_initdata);

  /* execute code on pru0 */
#define PRU_NUM 0
  prussdrv_exec_program(PRU_NUM, "./pruss_epwm.bin");

  /* wait until pru0 finishes */
  prussdrv_pru_wait_event(PRU_EVTOUT_0);
  prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);

  /* disable pru and close memory mapping */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();

  return 0;
}
