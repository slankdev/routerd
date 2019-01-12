
#ifndef _NETLINK_HEXDUMP_H_
#define _NETLINK_HEXDUMP_H_

#include <stdio.h>
#include <stdint.h>

inline static void
hexdump(FILE* fp, const void* buffer, size_t bufferlen)
{
  const uint8_t *data = (const uint8_t*)(buffer);
  size_t row = 0;
  while (bufferlen > 0) {
    fprintf(fp, "%04zx:   ", row);

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
    fprintf(fp, "\n");
    bufferlen -= n;
    data += n;
    row  += n;
  }
}

inline static void
carrydump(FILE* fp, const char* comment,
    const void* buffer, size_t bufferlen)
{
  if (comment) fprintf(fp, "// %s \n", comment);
  fprintf(fp, "static uint8_t arry[] = {\n");

  const uint8_t *data = (const uint8_t*)(buffer);
  size_t row = 0;
  while (bufferlen > 0) {
    fprintf(fp, "  ");

    size_t n;
    if (bufferlen < 8) n = bufferlen;
    else               n = 8;

    for (size_t i = 0; i < n; i++) {
      fprintf(fp, "0x%02x, ", data[i]);
    }
    for (size_t i = n; i < 8; i++) {
      fprintf(fp, "      ");
    }
    fprintf(fp, "   /*");
    for (size_t i = 0; i < n; i++) {
      if (i == 8) fprintf(fp, "  ");
      uint8_t c = data[i];
      if (!(0x20 <= c && c <= 0x7e)) c = '.';
      fprintf(fp, "%c", c);
    }
    fprintf(fp, "*/");
    fprintf(fp, "\n");
    bufferlen -= n;
    data += n;
    row  += n;
  }
  fprintf(fp, "};\n");
}

#endif /* _NETLINK_HEXDUMP_H_ */
