
#ifndef _YALIN_H_
#define _YALIN_H_

#include "socket.h"
#include "types.h"
#include "yalin.h"
#include "rtattr.h"
#include "enum.h"

inline static std::string
strfmt(const char* fmt, ...)
{
  char str[1000];
  va_list args;
  va_start(args, fmt);
  vsprintf(str, fmt, args);
  va_end(args);
  return str;
}

inline static std::string
inetpton(const void* ptr, int afi)
{
  char buf[256];
  inet_ntop(afi, ptr, buf, sizeof(buf));
  return buf;
}


#include "addr.h"
#include "link.h"
#include "neigh.h"
#include "route.h"
#include "netconf.h"

#endif /* _YALIN_H_ */

