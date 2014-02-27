#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"
#include "../common/mio.h"


#define PERROR() printf("[!] %s:%u\n", __FUNCTION__, __LINE__)


/* cm */

/* doc: spruh73c, table 2.2 */
#define CM_PER_MIO_ADDR 0x44e00000
#define CM_PER_MIO_SIZE (128 * 1024)

/* doc: spruh73c, table 2.2 */
#define CM_WKUP_MIO_ADDR 0x44e00400
#define CM_WKUP_MIO_SIZE 0x1000

/* doc: spruh73c, table 8.28 */
#define CM_PER_EPWMSS0_CLKCTRL 0xd4
#define CM_PER_EPWMSS1_CLKCTRL 0xcc
#define CM_PER_EPWMSS2_CLKCTRL 0xd8

/* doc: spruh73c, table 8.28 */
#define CM_WKUP_ADC_TSC_CLKCTRL 0xbc

static int cm_per_enable_pwmss(size_t i)
{
  /* enable clocking of the pwmss[i] */

  static const size_t off[] =
  {
    CM_PER_EPWMSS0_CLKCTRL,
    CM_PER_EPWMSS1_CLKCTRL,
    CM_PER_EPWMSS2_CLKCTRL
  };

  mio_handle_t mio;

  if (mio_open(&mio, CM_PER_MIO_ADDR, CM_PER_MIO_SIZE)) return -1;
  mio_write_uint32(&mio, off[i], 2);
  mio_close(&mio);

  return 0;
}

static int cm_wkup_enable_adc_tsc(void)
{
  mio_handle_t mio;

  if (mio_open(&mio, CM_WKUP_MIO_ADDR, CM_WKUP_MIO_SIZE)) return -1;
  mio_write_uint32(&mio, CM_WKUP_ADC_TSC_CLKCTRL, 2);
  mio_close(&mio);

  return 0;
}


/* epwm */

static int epwm_setup(void)
{
  cm_per_enable_pwmss(0);
  cm_per_enable_pwmss(1);
  cm_per_enable_pwmss(2);

  return 0;
}


/* tsc adc */

/* doc: spruh73c, table 2.2 */
#define ADC_MIO_ADDR 0x44e0d000
#define ADC_MIO_SIZE (8 * 1024)

/* doc: spruh73c, table 12.4 */
#define ADC_REG_SYSCONFIG 0x10
#define ADC_REG_IRQENABLE_SET 0x2c
#define ADC_REG_IRQWAKEUP 0x34
#define ADC_REG_DMAENABLE_SET 0x38
#define ADC_REG_CTRL 0x40
#define ADC_REG_ADC_CLKDIV 0x4c
#define ADC_REG_STEPENABLE 0x54
#define ADC_REG_TS_CHARGE_DELAY 0x60
#define ADC_REG_STEP1CONFIG 0x64
#define ADC_REG_FIFO0COUNT 0xe4
#define ADC_REG_FIFO0DATA 0x100

static int adc_setup(void)
{
  mio_handle_t mio;

  /* enable adc_tsc clocking */
  if (cm_wkup_enable_adc_tsc())
  {
    PERROR();
    return -1;
  }

  if (mio_open(&mio, ADC_MIO_ADDR, ADC_MIO_SIZE))
  {
    PERROR();
    return -1;
  }

  /* disable the module, enable step registers writing */
  mio_write_uint32(&mio, ADC_REG_CTRL, 1 << 2);

  /* setup adc */
  mio_write_uint32(&mio, ADC_REG_SYSCONFIG, 1 << 2);
  mio_write_uint32(&mio, ADC_REG_IRQENABLE_SET, 0);
  mio_write_uint32(&mio, ADC_REG_IRQWAKEUP, 0);
  mio_write_uint32(&mio, ADC_REG_DMAENABLE_SET, 0);
  mio_write_uint32(&mio, ADC_REG_ADC_CLKDIV, 0);
  mio_write_uint32(&mio, ADC_REG_TS_CHARGE_DELAY, 1);

  /* enable channel 1 */
  mio_write_uint32(&mio, ADC_REG_STEPENABLE, (1 << 1) | (1 << 0));

  /* channel 1, continuous mode */
  mio_write_uint32(&mio, ADC_REG_STEP1CONFIG, 1);

  /* enable the module */
  mio_write_uint32(&mio, ADC_REG_CTRL, 1);

  mio_close(&mio);

  return 0;
}


/* write arguments from command line to shared memory */

static uint32_t get_uint32(const char* s)
{
  int base = 10;
  if ((strlen(s) > 2) && (s[0] == '0') && (s[1] == 'x')) base = 16;
  return (uint32_t)strtoul(s, NULL, base);
}

static int write_args(int ac, const char** av)
{
  mio_handle_t mio;
  size_t i;

  if (mio_open(&mio, 0x80001000, 0x1000)) return -1;

  for (i = 0; i != (size_t)ac; ++i)
    mio_write_uint32(&mio, i * sizeof(uint32_t), get_uint32(av[i]));

  mio_close(&mio);

  return 0;
}

static int read_res(uint32_t res[8])
{
  static const size_t sharedram_offset = 2048;
  volatile uint32_t* p;
  size_t i;

  prussdrv_map_prumem(4, (void**)&p);

  for (i = 0; i != 8; ++i) res[i] = p[sharedram_offset + i];

  return 0;
}


/* main */

int main(int ac, const char** av)
{
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  uint32_t res[8];
  size_t i;

  adc_setup();
  epwm_setup();

  prussdrv_init();

  if (prussdrv_open(PRU_EVTOUT_0))
  {
    printf("prussdrv_open open failed\n");
    return -1;
  }

  prussdrv_pruintc_init(&pruss_intc_initdata);

  write_args(ac - 1, av + 1);

  /* execute code on pru0 */
#define PRU_NUM 0
  prussdrv_exec_program(PRU_NUM, "./main.bin");

  /* wait until pru0 finishes */
  prussdrv_pru_wait_event(PRU_EVTOUT_0);
  prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);

  read_res(res);
  for (i = 0; i != 8; ++i) printf("0x%08x\n", res[i]);

  /* disable pru and close memory mapping */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();

  return 0;
}
