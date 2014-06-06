#ifndef PRU_SDK_H_INCLUDED
#define PRU_SDK_H_INCLUDED


#include <stdint.h>


void pru_init(void);

void shm_init(void);
void shm_write(register uint32_t, register uint32_t);
uint32_t shm_read(register uint32_t);

void adc_init(void);
uint16_t adc_read(register uint32_t, register uint32_t, register uint32_t);


#endif /* PRU_SDK_H_INCLUDED */
