#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"
#include "../common/mio.h"

#define PERROR() printf("[!] %s:%u\n", __FUNCTION__, __LINE__)

/* doc: spruh73c, table 2.2 */
#define CM_WKUP_MIO_ADDR 0x44e00400
#define CM_WKUP_MIO_SIZE 0x1000

/* doc: spruh73c, table 8.28 */
#define CM_WKUP_ADC_TSC_CLKCTRL 0xbc

/* host pru shared memory */

static void zero_words(size_t n)
{
  mio_handle_t mio;
  size_t i;

  if (mio_open(&mio, 0x80001000, 0x1000))
  {
    printf("unable to zero_words\n");
    return ;
  }

  for (i = 0; i != n; ++i)
    mio_write_uint32(&mio, i * sizeof(uint32_t), 0);

  mio_close(&mio);
}

static int cm_wkup_enable_adc_tsc(void)
{
  mio_handle_t mio;

  if (mio_open(&mio, CM_WKUP_MIO_ADDR, CM_WKUP_MIO_SIZE)) return -1;
  mio_write_uint32(&mio, CM_WKUP_ADC_TSC_CLKCTRL, 2);
  mio_close(&mio);

  return 0;
}


static int read_words(uint32_t* x, size_t n)
{
  static const size_t sharedram_offset = 2048;
  volatile uint32_t* p;
  size_t i;

  prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, (void**)&p);

  for (i = 0; i != 8; ++i) x[i] = p[sharedram_offset + i];

  return 0;
}


/* sigint handler */

static volatile unsigned int is_sigint = 0;

static void on_sigint(int x)
{
  is_sigint = 1;
}


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

/* doc: spruh73c, table 2.2 */
#define CM_WKUP_MIO_ADDR 0x44e00400
#define CM_WKUP_MIO_SIZE 0x1000

/* doc: spruh73c, table 8-88 */
#define CM_CLKSEL_DPLL_CORE 0x68

#define CM_CLKDCOLDO_DPLL_PER 0x7c
#define CM_DIV_M4_DPLL_CORE 0x80
#define CM_CLKMODE_DPLL_PER 0x8c

#define CM_CLKSEL_DPLL_PERIPH 0x9c
#define CM_DIV_M2_DPLL_MPU 0xa8


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
#define ADC_REG_TS_CHARGE_STEPCONFIG 0x5c
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
  mio_write_uint32(&mio, ADC_REG_TS_CHARGE_STEPCONFIG, 1 << 19);

  /* enable channel 1 */
  mio_write_uint32(&mio, ADC_REG_STEPENABLE, (1 << 1) | (1 << 0));

  /* channel 1, continuous mode, averaging = 2 */
  mio_write_uint32(&mio, ADC_REG_STEP1CONFIG, 5 );                                             

  /* channel 2, continuous mode, averaging = 2 */

  //  mio_write_uint32(&mio, ADC_REG_STEP1CONFIG, 5 | (1 << 19));

  /* enable the module */
  mio_write_uint32(&mio, ADC_REG_CTRL, 1);
  mio_close(&mio);

  return 0;
}

 static void setup_per_dpll(mio_handle_t* mio, uint32_t m, uint32_t n)
 {
   /* epwmss is clocked by L4_PER_CLK (table 8-21) */

   /* figure 8-9, table 8-20 */
   /* clock path: */
   /* xtal master clock, clk_m_osc (25MHz) */
   /* adpll, clkdcoldo */
   /* hsdivider.m4, core_clkoutm4 (200MHz) */
   /* prcm, /2 */
   /* l4_per */

   /* 8.1.1.6.6.1 */

   uint32_t x;

   x = mio_read_uint32(mio, CM_CLKSEL_DPLL_CORE);
   x &= ~((1 << 20) - 1);
   x |= (m << 8) | n;
   mio_write_uint32(mio, CM_CLKSEL_DPLL_CORE, x);
 }

 static void print_clksel_dpll(mio_handle_t* mio)
 {
   /* dpll_mult is M */
   /* dpll_div  is N */

   uint32_t x;

   x = mio_read_uint32(mio, CM_CLKSEL_DPLL_CORE);
   printf("dpll_mult      : 0x%08x\n", (x >> 8) & ((1 << 11) - 1));
   printf("dpll_div       : 0x%08x\n", x & ((1 << 7) - 1));

   printf("--\n");
 }




/* main */

int main(int ac, char** av)
{
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  uint32_t x[8];
  const size_t n = sizeof(x) / sizeof(x[0]);
  size_t i;
  //  mio_handle_t mio;

  //  print_clksel_dpll(&mio);
  /* was: 960 24 */
  //  setup_per_dpll(&mio, 40, 0);
  //  print_clksel_dpll(&mio);

  adc_setup();
  epwm_setup();

  prussdrv_init();

  if (prussdrv_open(PRU_EVTOUT_0))
  {
    printf("prussdrv_open open failed\n");
    return -1;
  }

  prussdrv_pruintc_init(&pruss_intc_initdata);

  /* zero_words(n); */

#define PRU_NUM 0

  /* write data from data.bin */
  prussdrv_load_datafile(PRU_NUM, "./data.bin");

  /* execute code on pru0 */
  prussdrv_exec_program_at(PRU_NUM, "./text.bin", START_ADDR);

  signal(SIGINT, on_sigint);
  while (is_sigint == 0)
  {
    usleep(1000000);
    read_words(x, n);
    for (i = 0; i != n; ++i)
    {
      printf("0x%08x (%f)\n", x[i], *((float*)(x + i)));
    }
    printf("\n");
  }

  /* disable pru and close memory mapping */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();

  return 0;
}


