
#ifndef _YALIN_FLAGS_H_
#define _YALIN_FLAGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX
#define MAX(a,b) a>b?a:b
#endif

#ifndef MIN
#define MIN(a,b) a<b?a:b
#endif

struct flags_str {
  char name_long[100];
  char name_short[100];
};

inline static const char*
snprintflags_long(char* str, size_t len, uint32_t flags,
    struct flags_str fstr[], size_t max_flag_bit)
{
  memset(str, 0x0, len);
  bool first = true;
  size_t n_loop = MIN(max_flag_bit, sizeof(flags)*8);
  for (size_t i=0; i<n_loop; i++) {
    if (flags & 0x1<<i) {
      if (first) first = false;
      else strncat(str, "|", len);
      strncat(str, fstr[i].name_long, len);
    }
  }
  return str;
}

inline static const char*
snprintflags_short(char* str, size_t len, uint32_t flags,
    struct flags_str fstr[], size_t max_flag_bit)
{
  memset(str, 0x0, len);
  bool first = true;
  size_t n_loop = MIN(max_flag_bit, sizeof(flags)*8);
  for (size_t i=0; i<n_loop; i++) {
    if (flags & 0x1<<i) {
      if (first) first = false;
      else strncat(str, "|", len);
      strncat(str, fstr[i].name_short, len);
    }
  }
  return str;
}

/**************************************************
 * Bellow is user defined snprintflags functions. *
 **************************************************/

inline static const char*
ifi_flags_to_str(uint32_t flags, char* str, size_t len)
{
  struct flags_str f[] = {
    {"IFA_F_TEMPORARY","non"},
    {"IFA_F_NODAD","non"},
    {"IFA_F_OPTIMISTIC","non"},
    {"IFA_F_DADFAILED","non"},
    {"IFA_F_HOMEADDRESS","non"},
    {"IFA_F_DEPRECATED","non"},
    {"IFA_F_TENTATIVE","non"},
    {"IFA_F_PERMANENT","non"},
    {"IFA_F_MANAGETEMPADDR","non"},
    {"IFA_F_NOPREFIXROUTE","non"},
    {"IFA_F_MCAUTOJOIN","non"},
    {"IFA_F_STABLE_PRIVACY","non"},
  };
  snprintflags_long(str, len, flags, f, sizeof(f)/sizeof(f[0]));
  return str;
}

inline static const char*
nlmsg_flags_to_str_new(uint32_t flags, char* str, size_t len)
{
  struct flags_str f_new[] = {
    {"NLM_F_REQUEST","non"},
    {"NLM_F_MULTI","non"},
    {"NLM_F_ACK","non"},
    {"NLM_F_ECHO","non"},
    {"NLM_F_DUMP_INTR","non"},
    {"NLM_F_DUMP_FILTERED","non"},
    {"NLM_F_REPLACE","non"},
    {"NLM_F_EXCL","non"},
    {"NLM_F_CREATE","non"},
    {"NLM_F_APPEND","non"},
  };
  char newstr[100];
  snprintflags_long(newstr, sizeof(newstr), flags, f_new, sizeof(f_new)/sizeof(f_new[0]));

  snprintf(str, len, "%s", newstr);
  return str;
}

inline static const char*
nlmsg_flags_to_str_get(uint32_t flags, char* str, size_t len)
{
  struct flags_str f_get[] = {
    {"NLM_F_REQUEST","non"},
    {"NLM_F_MULTI","non"},
    {"NLM_F_ACK","non"},
    {"NLM_F_ECHO","non"},
    {"NLM_F_DUMP_INTR","non"},
    {"NLM_F_DUMP_FILTERED","non"},
    {"NLM_F_ROOT","non"},
    {"NLM_F_MATCH","non"},
    {"NLM_F_ATOMIC","non"},
  };
  char getstr[100];
  snprintflags_long(getstr, sizeof(getstr), flags, f_get, sizeof(f_get)/sizeof(f_get[0]));

  snprintf(str, len, "%s", getstr);
  return str;
}

#endif /* _YALIN_FLAGS_H_ */

