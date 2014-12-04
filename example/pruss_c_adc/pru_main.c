#include <stdint.h>
#include "pru_hal.h"

int main(void)
{
   volatile float x = 3.1415;
   volatile float y;
   volatile float z;
   int i, j;
   x *= 12.345;
   y = x * 1.2345;

  ocp_init();
  shm_init();
 
  /* i = 0; */
  while (1) 
  {
    for(i=0;i<1000000;i++)
    { 
       z= z + (float)adc_read(0, 0, 0);  
       for(j=0;j<10;j++);
    }
    z= z * 0.000000000439453125; // Result in Volt
    shm_write_uint32(0, 0xdeadbeef);
    shm_write_uint32(4, 0x2b2b2c2d);
    shm_write_float(8, x);
    shm_write_float(12, y);
    shm_write_float(16, z);
    shm_write_uint32(20, 0x12345678);
    shm_write_uint32(24, 0x23456789);
    shm_write_uint32(28, 0x34567890);
    z= 0.0;
   }

  /* for (i = 0; i != 8; ++i) */
  /* { */
  /*   shm_write(i * 4, i); */
  /* } */

  __halt();

  return 0;
}
