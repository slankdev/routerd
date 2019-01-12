
#include <yalin/yalin.h>

static uint8_t arry[] = {
  0xb8, 0x02, 0x1a, 0x00, 0x7c, 0x00, 0x02, 0x00,    /*....|...*/
  0x78, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,    /*x.......*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,    /*........*/
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,    /*........*/
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,    /*.....'..*/
  0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x38, 0x02, 0x0a, 0x00, 0x08, 0x00, 0x01, 0x00,    /*8.......*/
  0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x05, 0x00,    /*........*/
  0xff, 0xff, 0x00, 0x00, 0x26, 0x96, 0x91, 0x00,    /*....&...*/
  0xf8, 0x9e, 0x00, 0x00, 0xe8, 0x03, 0x00, 0x00,    /*........*/
  0xa4, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x40, 0x00, 0x00, 0x00, 0xdc, 0x05, 0x00, 0x00,    /*@.......*/
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,    /*........*/
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,    /*........*/
  0x03, 0x00, 0x00, 0x00, 0xa0, 0x0f, 0x00, 0x00,    /*........*/
  0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x80, 0x3a, 0x09, 0x00, 0x80, 0x51, 0x01, 0x00,    /*.:...Q..*/
  0x03, 0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0x00,    /*....X...*/
  0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,    /*........*/
  0x01, 0x00, 0x00, 0x00, 0x60, 0xea, 0x00, 0x00,    /*....`...*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,    /*.....'..*/
  0xe8, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x24, 0x01, 0x03, 0x00,    /*....$...*/
  0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*$.......*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x34, 0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x00,    /*4.......*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x07, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
  0x05, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,    /*........*/
};

int main()
{
  hexdump(stdout, arry, sizeof(arry));
  struct rtattr* attr = (struct rtattr*)arry;

  printf("--------------\n");

  char str[1000] = {0};
  rta_to_str_IFLA_AF_SPEC(attr, str, sizeof(str));
  printf("%s\n", str);
}
