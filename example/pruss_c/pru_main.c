#include <stdint.h>
#include "pru_sdk.h"

int main(void)
{
  volatile float x = 3.1415;

  x *= 10.0;

  pru_init();
  shm_init();

  /* i = 0; */
  /* while (1) */
  {
    shm_write_uint32(0, 0xdeadbeef);
    shm_write_uint32(4, 0x2b2b2c2d);
    shm_write_float(8, x);
  }

  /* for (i = 0; i != 8; ++i) */
  /* { */
  /*   shm_write(i * 4, i); */
  /* } */

  __halt();

  return 0;
}
