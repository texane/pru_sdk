/* memory mapped ios */


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "mio.h"


int mio_open(mio_handle_t* mio, size_t off, size_t size)
{
  static const size_t page_size = 0x1000;
  static const size_t page_mask = page_size - 1;
  size_t x;
  int fd;

  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd == -1) return -1;

  /* align on page size */

  x = off & page_mask;
  if (x)
  {
    mio->off = x;
    off -= x;
    size += x;
  }
  else
  {
    mio->off = 0;
  }

  x = size & page_mask;
  if (x) size += page_size - x;

  mio->size = size;

  mio->base = (uintptr_t)
    mmap(NULL, mio->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, off);

  close(fd);

  if (mio->base == (uintptr_t)MAP_FAILED) return -1;

  return 0;
}

int mio_close(mio_handle_t* mio)
{
  munmap((void*)mio->base, mio->size);
  return 0;
}

uint32_t mio_read_uint32(mio_handle_t* mio, size_t off)
{
  /* off the offset in bytes */
  const size_t mio_off = mio->off + off;
  return ((volatile uint32_t*)mio->base)[mio_off / sizeof(uint32_t)];
}

void mio_write_uint32(mio_handle_t* mio, size_t off, uint32_t x)
{
  /* off the offset in bytes */
  const size_t mio_off = mio->off + off;
  ((volatile uint32_t*)mio->base)[mio_off / sizeof(uint32_t)] = x;
}

void mio_and_uint32(mio_handle_t* mio, size_t off, uint32_t x)
{
  const uint32_t xx = mio_read_uint32(mio, off);
  mio_write_uint32(mio, off, xx & x);
}

void mio_or_uint32(mio_handle_t* mio, size_t off, uint32_t x)
{
  const uint32_t xx = mio_read_uint32(mio, off);
  mio_write_uint32(mio, off, xx | x);
}

uint16_t mio_read_uint16(mio_handle_t* mio, size_t off)
{
  /* off the offset in bytes */
  const size_t mio_off = mio->off + off;
  return ((volatile uint16_t*)mio->base)[mio_off / sizeof(uint16_t)];
}

void mio_write_uint16(mio_handle_t* mio, size_t off, uint16_t x)
{
  /* off the offset in bytes */
  const size_t mio_off = mio->off + off;
  ((volatile uint16_t*)mio->base)[mio_off / sizeof(uint16_t)] = x;
}

void mio_and_uint16(mio_handle_t* mio, size_t off, uint16_t x)
{
  const uint16_t xx = mio_read_uint16(mio, off);
  mio_write_uint16(mio, off, xx & x);
}

void mio_or_uint16(mio_handle_t* mio, size_t off, uint16_t x)
{
  const uint16_t xx = mio_read_uint16(mio, off);
  mio_write_uint16(mio, off, xx | x);
}
