#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"

int main(int ac, char** av)
{
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  unsigned int err;

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
  prussdrv_exec_program(PRU_NUM, "./pruss_apwm.bin");

  /* wait until pru0 finishes */
  prussdrv_pru_wait_event(PRU_EVTOUT_0);
  prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);

  /* disable pru and close memory mapping */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();

  return 0;
}
