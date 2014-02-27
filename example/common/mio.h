#ifndef MIO_H_INCLUDED
#define MIO_H_INCLUDED


#include <stdint.h>
#include <sys/types.h>


typedef struct mio_handle
{
  uintptr_t base;
  size_t off;
  size_t size;
} mio_handle_t;

int mio_open(mio_handle_t*, size_t, size_t);
int mio_close(mio_handle_t*);
uint32_t mio_read_uint32(mio_handle_t*, size_t);
void mio_write_uint32(mio_handle_t*, size_t, uint32_t);
void mio_and_uint32(mio_handle_t*, size_t, uint32_t);
void mio_or_uint32(mio_handle_t*, size_t, uint32_t);
uint16_t mio_read_uint16(mio_handle_t*, size_t);
void mio_write_uint16(mio_handle_t*, size_t, uint16_t);
void mio_and_uint16(mio_handle_t*, size_t, uint16_t);
void mio_or_uint16(mio_handle_t*, size_t, uint16_t);


#endif /* ! MIO_H_INCLUDED */
