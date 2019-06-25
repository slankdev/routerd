#ifndef _HEXDUMP_H_
#define _HEXDUMP_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

inline static void
hexdump(FILE* fp, const void *buffer, size_t bufferlen)
{
  const uint8_t *data = (const uint8_t*)(buffer);
  size_t row = 0;
  while (bufferlen > 0) {
    fprintf(fp, "\r%04zx:   ", row);

    size_t n;
    if (bufferlen < 16) n = bufferlen;
    else                n = 16;

    for (size_t i = 0; i < n; i++) {
      if (i == 8) fprintf(fp, " ");
      fprintf(fp, " %02x", data[i]);
    }
    for (size_t i = n; i < 16; i++) {
      fprintf(fp, "   ");
    }
    fprintf(fp, "   ");
    for (size_t i = 0; i < n; i++) {
      if (i == 8) fprintf(fp, "  ");
      uint8_t c = data[i];
      if (!(0x20 <= c && c <= 0x7e)) c = '.';
      fprintf(fp, "%c", c);
    }
    fprintf(fp, "\r\n");
    bufferlen -= n;
    data += n;
    row  += n;
  }
}

#ifdef __cplusplus
}
#endif
#endif /* _HEXDUMP_H_ */
