/* generate a software pwm of equal duty and frequency */
/* using both the CPU and the PRU. */
/* CPU uses xxx */
/* PRU uses pr1_pru0_pru_r30_15 (ie. pin_8_11) */


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"
#include "../common/mio.h"


#define PERROR() printf("[!] %s:%u\n", __FUNCTION__, __LINE__)


/* host pru shared memory */

static int write_frequency(uint32_t x)
{
  mio_handle_t mio;

  if (mio_open(&mio, 0x80001000, 0x1000)) return -1;

  mio_write_uint32(&mio, 0, x);
  /* mio_write_uint32(&mio, 4, y); */

  mio_close(&mio);

  return 0;
}


/* sigint handler */

static volatile unsigned int is_sigint = 0;

static void on_sigint(int x)
{
  is_sigint = 1;
}


/* main */

int main(int ac, char** av)
{
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  uint32_t freq;

  prussdrv_init();

  if (prussdrv_open(PRU_EVTOUT_0))
  {
    printf("prussdrv_open open failed\n");
    return -1;
  }

  prussdrv_pruintc_init(&pruss_intc_initdata);

  /* wave frequency */
  /* 1 second = 200000000 */
  /* default to 1 millisecond */
  freq = 200000;
  if (ac > 1) freq = (uint32_t)strtoul(av[1], NULL, 10);
  write_frequency(freq);

  /* execute code on pru0 */
#define PRU_NUM 0
  prussdrv_exec_program(PRU_NUM, "./spwm.bin");

  signal(SIGINT, on_sigint);
  while (is_sigint == 0) usleep(1000000);

  /* disable pru and close memory mapping */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();

  return 0;
}
