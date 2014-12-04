#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


void ocp_init(void)
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

  /* save r4, r5 */
  __asm__ __volatile__
  (
   " SUB r2, r2, 8 \n"
   " SBBO &r4, r2, 0, 8 \n"
  );

  __asm__ __volatile__
  (
   " LDI32 r4, 0x0120 \n"
   " LDI32 r5, 0x22028 \n"
   " SBBO &r4, r5, 0x00, 4 \n"
  );

  /* restore r4, r5 */
  __asm__ __volatile__
  (
   " LBBO &r4, r2, 0, 8 \n"
   " ADD r2, r2, 8 \n"
  );
}

void shm_write_uint32(register uint32_t i, register uint32_t x)
{
  /* i is the absolute offset relative from shared memory start */
  /* write x at shm + i */

  __asm__ __volatile__
  (
   " SBCO &r15, C28, r14.w0, 4 \n"
  );
}

void shm_write_float(register uint32_t i, register float x)
{
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


uint16_t adc_read(register uint32_t x, register uint32_t y, register uint32_t z)
{
  /* must be compiled without optimizations */

  /* r2 used for the stack pointer, do not modify */
  /* r3 used for return address, do not modify */
  /* x=r14, y=r15, z=r16 registers used */
  /* r14 used for return values */

  __asm__ __volatile__
  (
   /* wait until data available in fifo */
   //"  ADC_WAIT_DATA:    MOV r1, 0x44e0d0e4 \n" //ADC_MIO_ADDR + ADC_REG_FIFO0COUNT \n"
   //"  LBBO r14, r1, 0, 4 \n"
   // "  AND r14, r14, (1 << 7) - 1 \n"
   // "  QBEQ ADC_WAIT_DATA, r14, 0 \n"

   /* read fifo */
 "  LDI32 r1, 0x44e0d100 \n" //ADC_MIO_ADDR + ADC_REG_FIFO0DATA \n"
 "  LBBO &r14, r1, 0, 4 \n"
 "  LDI32 r1, 0x00000fff \n"
 "  AND r14, r14, r1 \n"
 "  JMP R3.w2 \n"
  );

  return 0;
}

uint16_t pid_ctrl(register uint32_t A0, register uint32_t A1, register uint32_t Set)
{
  /* must be compiled without optimizations */

  /* r2 used for the stack pointer, do not modify */
  /* r3 used for return address, do not modify */
  /* x=r14, y=r15, z=r16 registers used */
  /* r14 used for return values */

  __asm__ __volatile__
    (                        
    /* read fifo */
 "  LDI32 r1, 0x44e0d100 \n" //ADC_MIO_ADDR + ADC_REG_FIFO0DATA \n"                                       
 "  LBBO &r14, r1, 0, 4 \n"
 "  LDI32 r1, 0x00000fff \n"
 "  AND r14, r14, r1 \n"


 "  JMP R3.w2 \n"
     );

  return 0;
}
