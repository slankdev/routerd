
#ifndef _NETLINK_RTATTR_H_
#define _NETLINK_RTATTR_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>

inline static size_t
rtattr_payload_len(const struct rtattr* rta)
{ return rta->rta_len-sizeof(struct rtattr); }

inline static uint8_t*
rtattr_payload_ptr(const struct rtattr* rta)
{ return (uint8_t*)(rta + 1); }

inline static uint8_t
rtattr_read_8bit (const struct rtattr* attr)
{
  if (rtattr_payload_len(attr) > sizeof(uint8_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rtattr_payload_len(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rtattr_payload_len(attr));
    exit(1);
  }
  uint8_t val = *(uint8_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static uint16_t
rtattr_read_16bit(const struct rtattr* attr)
{
  if (rtattr_payload_len(attr) > sizeof(uint16_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rtattr_payload_len(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rtattr_payload_len(attr));
    exit(1);
  }
  uint16_t val = *(uint16_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static uint32_t
rtattr_read_32bit(const struct rtattr* attr)
{
  if (rtattr_payload_len(attr) > sizeof(uint32_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rtattr_payload_len(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rtattr_payload_len(attr));
    exit(1);
  }
  uint32_t val = *(uint32_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static uint64_t
rtattr_read_64bit(const struct rtattr* attr)
{
  if (rtattr_payload_len(attr) > sizeof(uint64_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rtattr_payload_len(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rtattr_payload_len(attr));
    exit(1);
  }
  uint64_t val = *(uint64_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static const char*
rtattr_read_str(const struct rtattr* attr)
{
  const char* strptr = (const char*)rtattr_payload_ptr(attr);
  const size_t strbuflen = strlen(strptr);
  const size_t payloadlen = rtattr_payload_len(attr);
  if (payloadlen > strbuflen) {
    fprintf(stderr, "%s: read miss (type=%u)\n", __func__, attr->rta_type);
    printf("payloadlen: %zd\n", payloadlen);
    printf("strbuflen: %zd\n", strbuflen);
    exit(1);
  }
  return strptr;
}

#endif /* _NETLINK_RTATTR_H_ */
