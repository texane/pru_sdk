#include <stdint.h>
#include "pru_hal.h"

int main(void)
{
  volatile float x = 3.1415926;

  x *= 10.0;

  ocp_init();
  shm_init();

// volatile int i = 0; 
//  while (1) 
  {
    shm_write_uint32(0, 0xdeadbeef);
    shm_write_uint32(4, 0x12345678);
    shm_write_float(8, x);
 //   shm_write_float(12, i++);
  }

  /* for (i = 0; i != 8; ++i) */
  /* { */
  //   shm_write(i * 4, i); 
  /* } */

  __halt();

  return 0;
}
