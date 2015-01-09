#include <stdint.h>

void
ocp_init (void)
{
  /* enable ocp wide access */

  __asm__ __volatile__
          (
           " LBCO &r0, C4, 4, 4 \n"
           " CLR r0, r0, 4 \n"
           " SBCO &r0, C4, 4, 4 \n"
           );
}

void
shm_pru1_init (void)
{
  /* configure the programmable pointer register for */
  /* PRU1 by setting c28_pointer[15:0] field to 0x0120 */
  /* this will make C28 point to 0x00012000 (PRU shared RAM). */

  //for the PRU0,the CTPPR_0 address is 0x22000 + 0x28,
  //for the PRU1, the CTPPR_0 address is 0x24000 + 0x28

  /* save r4, r5 */
  __asm__ __volatile__
          (
           " SUB r2, r2, 8 \n"
           " SBBO &r4, r2, 0, 8 \n"
           );

  __asm__ __volatile__
          (
           " LDI32 r4, 0x0120 \n"
           " LDI32 r5, 0x24028 \n"
           " SBBO &r4, r5, 0x00, 4 \n"
           );

  /* restore r4, r5 */
  __asm__ __volatile__
          (
           " LBBO &r4, r2, 0, 8 \n"
           " ADD r2, r2, 8 \n"
           );
}

void
shm_write_uint32 (register uint32_t i, register uint32_t x)
{
  /* i is the absolute offset relative from shared memory start */
  /* write x at shm + i */

  __asm__ __volatile__
          (
           " SBCO &r15, C28, r14.w0, 4 \n"
           );
}

void
shm_write_float (register uint32_t i, register float x)
{
  __asm__ __volatile__
          (
           " SBCO &r15, C28, r14.w0, 4 \n"
           );
}

uint32_t
shm_pru1_read (register uint32_t i)
{
  /* i is the absolute offset relative from shared memory start */
  /* read x at shm + i */

  __asm__ __volatile__
          (
           " LDI32 r0, 0x000000120 \n"
           " LDI32 r1, 0x24028 \n"
           " SBBO &r0, r1, 0, 4 \n"

           " LDI32 r0, 0x00100000 \n"
           " LDI32 r1, 0x2402c \n"
           " SBBO &r0, r1, 0, 4 \n"

           " LBCO &r14, C31, 0, 4 \n"
           " JMP R3.w2 \n"
           );

  /* unreached */
  return 0;
}

void
adc_init (void) { }

uint16_t
adc_read (register uint32_t x, register uint32_t y, register uint32_t z)
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
