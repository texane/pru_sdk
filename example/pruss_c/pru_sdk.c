#include <stdint.h>


void pru_init(void)
{
  /* enable ocp wide access */

  __asm__ __volatile__
  (
   " LBCO &r0, C4, 4, 4 \n"
   " CLR r0, r0, 4 \n"
   " SBCO &r0, C4, 4, 4 \n"
  );
}

void shm_init(void)
{
  /* configure the programmable pointer register for */
  /* PRU0 by setting c28_pointer[15:0] field to 0x0120 */
  /* this will make C28 point to 0x00012000 (PRU shared RAM). */

  __asm__ __volatile__
  (
   " LDI32 r0, 0x0120 \n"
   " LDI32 r1, 0x22028 \n"
   " SBBO &r0, r1, 0x00, 4 \n"
  );
}

void shm_write(register uint32_t i, register uint32_t x)
{
  /* i is the absolute offset relative from shared memory start */
  /* write x at shm + i */

  __asm__ __volatile__
  (
   " SBCO &r15, C28, r14.w0, 4 \n"
  );
}

uint32_t shm_read(register uint32_t i)
{
  /* i is the absolute offset relative from shared memory start */
  /* read x at shm + i */

  __asm__ __volatile__
  (
   " LDI32 r0, 0x000000120 \n"
   " LDI32 r1, 0x22028 \n"
   " SBBO &r0, r1, 0, 4 \n"

   " LDI32 r0, 0x00100000 \n"
   " LDI32 r1, 0x2202c \n"
   " SBBO &r0, r1, 0, 4 \n"

   " LBCO &r14, C31, 0, 4 \n"
   " JMP R3.w2 \n"
  );

  /* unreached */
  return 0;
}

void adc_init(void)
{
}

uint16_t adc_read(register uint32_t x, register uint32_t y, register uint32_t z)
{
  /* must be compiled without optimizations */

  /* r2 used for the stack pointer, do not modify */
  /* r3 used for return address, do not modify */
  /* x=r14, y=r15, z=r16 registers used */
  /* r14 used for return values */

  __asm__ __volatile__
  (
   " JMP R3.w2 \n"
  );

  return 0;
}
