#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"
#include "../common/mio.h"


/* host pru shared memory */

static void zero_words(size_t n)
{
  mio_handle_t mio;
  size_t i;

  if (mio_open(&mio, 0x80001000, 0x1000))
  {
    printf("unable to zero_words\n");
    return ;
  }

  for (i = 0; i != n; ++i)
    mio_write_uint32(&mio, i * sizeof(uint32_t), 0);

  mio_close(&mio);
}

static int read_words(uint32_t* x, size_t n)
{
  static const size_t sharedram_offset = 2048;
  volatile uint32_t* p;
  size_t i;

  prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, (void**)&p);

  for (i = 0; i != 8; ++i) x[i] = p[sharedram_offset + i];

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
  uint32_t x[8];
  const size_t n = sizeof(x) / sizeof(x[0]);
  size_t i;

  prussdrv_init();

  if (prussdrv_open(PRU_EVTOUT_0))
  {
    printf("prussdrv_open open failed\n");
    return -1;
  }

  prussdrv_pruintc_init(&pruss_intc_initdata);

  /* zero_words(n); */

#define PRU_NUM 0

  /* write data from data.bin */
  prussdrv_load_datafile(PRU_NUM, "./data.bin");

  /* execute code on pru0 */
  prussdrv_exec_program_at(PRU_NUM, "./text.bin", START_ADDR);

  signal(SIGINT, on_sigint);
  while (is_sigint == 0)
  {
    usleep(1000000);
    read_words(x, n);
    for (i = 0; i != n; ++i)
    {
      printf("0x%08x (%f)\n", x[i], *((float*)(x + i)));
    }
    printf("\n");
  }

  /* disable pru and close memory mapping */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();

  return 0;
}
