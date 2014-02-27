#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"
#include "../common/mio.h"


/* host pru shared memory */

static int read_words(uint32_t x[2])
{
  static const size_t sharedram_offset = 2048;
  volatile uint32_t* p;

  prussdrv_map_prumem(4, (void**)&p);

  x[0] = p[sharedram_offset + 0];
  x[1] = p[sharedram_offset + 1];

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
  uint32_t x[2];

  prussdrv_init();

  if (prussdrv_open(PRU_EVTOUT_0))
  {
    printf("prussdrv_open open failed\n");
    return -1;
  }

  prussdrv_pruintc_init(&pruss_intc_initdata);

  /* execute code on pru0 */
#define PRU_NUM 0
  prussdrv_exec_program(PRU_NUM, "./iep.bin");

  signal(SIGINT, on_sigint);
  while (is_sigint == 0)
  {
    usleep(500000);
    read_words(x);
    printf("0x%08x, 0x%08x\n", x[0], x[1]);
  }

  /* disable pru and close memory mapping */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();

  return 0;
}
