#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"
#include "../common/mio.h"


/* host pru shared memory */

static int write_operands(uint32_t a, uint32_t b)
{
  mio_handle_t mio;

  if (mio_open(&mio, 0x80001000, 0x1000)) return -1;

  mio_write_uint32(&mio, 0, a);
  mio_write_uint32(&mio, 4, b);

  mio_close(&mio);

  return 0;
}

static int read_result(uint64_t* x)
{
  static const size_t sharedram_offset = 2048;
  volatile uint32_t* p;
  uint32_t lo;
  uint32_t hi;

  prussdrv_map_prumem(4, (void**)&p);

  lo = p[sharedram_offset + 0];
  hi = p[sharedram_offset + 1];

  *x = ((uint64_t)hi << 32) | (uint64_t)lo;

  return 0;
}


/* main */

int main(int ac, char** av)
{
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  const uint32_t a = strtoul(av[1], NULL, 10);
  const uint32_t b = strtoul(av[2], NULL, 10);
  unsigned int err;
  uint64_t res;

  if (write_operands(a, b))
  {
    printf("write_operands failed\n");
    return -1;
  }

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
  prussdrv_exec_program(PRU_NUM, "./mac.bin");

  /* wait until pru0 finishes */
  prussdrv_pru_wait_event(PRU_EVTOUT_0);
  prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);

  read_result(&res);
  printf("%u * %u == %llu (0x%016llx)\n", a, b, res, res);

  /* disable pru and close memory mapping */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();

  return 0;
}
