#include <stdint.h>
#include "pru_sdk.h"

int main(void)
{
  /* uint32_t i; */

  pru_init();
  shm_init();

  /* i = 0; */
  /* while (1) */
  {
    shm_write(0, 0xbeefb00b);
    /* shm_write(4, 0x2b2b2c2d); */
    /* shm_write(8, 0x2c2b2c2d); */
  }

  while (1) ;

  /* for (i = 0; i != 8; ++i) */
  /* { */
  /*   shm_write(i * 4, i); */
  /* } */

  __asm__ __volatile__
  (
   " HALT \n"
  );

  volatile float x = 1.3;

  x *= 2.3;

  return 0;
}
