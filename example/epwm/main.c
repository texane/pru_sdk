/* signals */
/* ehrpwm0a, spi1_sclk, p9.31 */
/* ehrpwm0b, spi1_d0, p9.29 */
/* ehrpwm1a, p9.14 */
/* ehrpwm1b, p9.16 */
/* ehrpwm2a, p8.19 */
/* ehrpwm2b, p8.13 */


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../common/mio.h"


#define PERROR() printf("[!] %s:%u\n", __FUNCTION__, __LINE__)


/* cm */

/* doc: spruh73c, table 2.2 */
#define CM_PER_MIO_ADDR 0x44e00000
#define CM_PER_MIO_SIZE (128 * 1024)

/* doc: spruh73c, table 8.28 */
#define CM_PER_EPWMSS0_CLKCTRL 0xd4
#define CM_PER_EPWMSS1_CLKCTRL 0xcc
#define CM_PER_EPWMSS2_CLKCTRL 0xd8

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


/* pwmss */

/* doc: spruh73c, table 2.3 */
#define PWMSS0_MIO_ADDR 0x48300000
#define PWMSS1_MIO_ADDR 0x48302000
#define PWMSS2_MIO_ADDR 0x48304000
#define PWMSS_MIO_SIZE (4 * 1024)

/* doc: spruh73c, section 15 */
#define PWMSS_REG_IDVER 0x00
#define PWMSS_REG_SYSCONFIG 0x04
#define PWMSS_REG_CLKCONFIG 0x08
#define PWMSS_REG_CLKSTATUS 0x0c

#define PWMSS_EPWM_REG_BASE 0x200

/* doc: spruh73c, table 15.59 */
/* warning: 16 bit registers */
#define EPWM_REG_TBCTL (PWMSS_EPWM_REG_BASE + 0x00)
#define EPWM_REG_TBSTS (PWMSS_EPWM_REG_BASE + 0x02)
#define EPWM_REG_TBHSHR (PWMSS_EPWM_REG_BASE + 0x04)
#define EPWM_REG_TBPHS (PWMSS_EPWM_REG_BASE + 0x06)
#define EPWM_REG_TBCNT (PWMSS_EPWM_REG_BASE + 0x08)
#define EPWM_REG_TBPRD (PWMSS_EPWM_REG_BASE + 0x0a)

/* doc: spruh73c, table 15.65 */
/* warning: 16 bit registers */
#define EPWM_REG_CMPCTL (PWMSS_EPWM_REG_BASE + 0x0e)
#define EPWM_REG_CMPAHR (PWMSS_EPWM_REG_BASE + 0x10)
#define EPWM_REG_CMPA (PWMSS_EPWM_REG_BASE + 0x12)
#define EPWM_REG_CMPB (PWMSS_EPWM_REG_BASE + 0x14)

/* doc: spruh73c, table 15.69 */
/* warning: 16 bit registers */
#define EPWM_REG_AQCTLA (PWMSS_EPWM_REG_BASE + 0x16)
#define EPWM_REG_AQCTLB (PWMSS_EPWM_REG_BASE + 0x18)
#define EPWM_REG_AQSFRC (PWMSS_EPWM_REG_BASE + 0x1a)
#define EPWM_REG_AQCSFRC (PWMSS_EPWM_REG_BASE + 0x1c)

/* doc: spruh73c, table 15.74 */
/* warning: 16 bit registers */
#define EPWM_REG_DBCTL (PWMSS_EPWM_REG_BASE + 0x1e)
#define EPWM_REG_DBRED (PWMSS_EPWM_REG_BASE + 0x20)
#define EPWM_REG_DBFED (PWMSS_EPWM_REG_BASE + 0x22)

/* doc: spruh73c, table 15.78 */
/* warning: 16 bit registers */
#define EPWM_REG_TZSEL (PWMSS_EPWM_REG_BASE + 0x24)
#define EPWM_REG_TZCTL (PWMSS_EPWM_REG_BASE + 0x28)
#define EPWM_REG_TZEINT (PWMSS_EPWM_REG_BASE + 0x2a)
#define EPWM_REG_TZFLG (PWMSS_EPWM_REG_BASE + 0x2c)
#define EPWM_REG_TZCLR (PWMSS_EPWM_REG_BASE + 0x2e)
#define EPWM_REG_TZFRC (PWMSS_EPWM_REG_BASE + 0x30)

/* doc: spruh73c, table 15.85 */
/* warning: 16 bit registers */
#define EPWM_REG_ETSEL (PWMSS_EPWM_REG_BASE + 0x32)
#define EPWM_REG_ETPS (PWMSS_EPWM_REG_BASE + 0x34)
#define EPWM_REG_ETFLG (PWMSS_EPWM_REG_BASE + 0x36)
#define EPWM_REG_ETCLR (PWMSS_EPWM_REG_BASE + 0x38)
#define EPWM_REG_ETFRC (PWMSS_EPWM_REG_BASE + 0x3a)

/* doc: spruh73c, table 15.91 */
/* warning: 16 bit registers */
#define EPWM_REG_PCCTL (PWMSS_EPWM_REG_BASE + 0x3c)

/* doc: spruh73c, table 15.99 */
/* note: doc < rev. j is wrong, register offset is 0xc0. see this post: */
/* http://e2e.ti.com/support/embedded/starterware/f/790/p/301065/1139333.aspx */
/* warning: 16 bit registers */
#define EPWM_REG_HRCTL (PWMSS_EPWM_REG_BASE + 0xc0)

static int pwmss_open(mio_handle_t* mio, unsigned int i)
{
  /* i the pwmss id, in [0:2] */

  const uintptr_t addr[] =
  {
    PWMSS0_MIO_ADDR,
    PWMSS1_MIO_ADDR,
    PWMSS2_MIO_ADDR
  };

  /* enable pwmss clocking */
  if (cm_per_enable_pwmss(i)) return -1;

  if (mio_open(mio, addr[i], PWMSS_MIO_SIZE)) return -1;

  return 0;
}

static int pwmss_close(mio_handle_t* mio)
{
  mio_close(mio);
  return 0;
}

static int pwmss_set_epwm(mio_handle_t* mio, uint16_t p, uint16_t d, uint16_t hrd)
{
  /* mio the pwmss mio region */
  /* p the period */
  /* d the duty */
  /* hrd the high resolution duty */

  /* enable epwm clock */
  mio_or_uint32(mio, PWMSS_REG_CLKCONFIG, 1 << 8);

  /* doc: spruh73c, table 15.60 */
  mio_write_uint16(mio, EPWM_REG_TBCTL, (2 << 14) | (3 << 4));
  mio_write_uint16(mio, EPWM_REG_TBPHS, 0);
  mio_write_uint16(mio, EPWM_REG_TBPRD, p);
  mio_write_uint16(mio, EPWM_REG_TBCNT, 0);

  mio_write_uint16(mio, EPWM_REG_CMPAHR, hrd << 8);
  mio_write_uint16(mio, EPWM_REG_HRCTL, 2 << 0);

  /* doc: spruh73c, table 15.66 */
  mio_write_uint16(mio, EPWM_REG_CMPCTL, 0);
  mio_write_uint16(mio, EPWM_REG_CMPA, d);

  /* doc: spruh73c, table 15.70 */
  mio_write_uint16(mio, EPWM_REG_AQCTLA, (3 << 4) | (2 << 0));
  /* doc: spruh73c, table 15.71 */
  mio_write_uint16(mio, EPWM_REG_AQCTLB, 0);

  /* doc: spruh73c, table 15.79 */
  mio_write_uint16(mio, EPWM_REG_TZSEL, 0);
  /* doc: spruh73c, table 15.81 */
  mio_write_uint16(mio, EPWM_REG_TZEINT, 0);

  /* doc: spruh73c, table 15.86 */
  mio_write_uint16(mio, EPWM_REG_ETSEL, 0);

  /* doc: spruh73c, table 15.91 */
  mio_write_uint16(mio, EPWM_REG_PCCTL, 0);

  return 0;
}


/* main */

int main(int ac, char** av)
{
  mio_handle_t mio;

  if (pwmss_open(&mio, 1))
  {
    PERROR();
    return -1;
  }

  pwmss_set_epwm(&mio, 0x10, 0x5, 0x60);
  pwmss_close(&mio);

  return 0;
}
