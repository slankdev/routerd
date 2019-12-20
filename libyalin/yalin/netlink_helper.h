#ifndef _NETLINK_HELPER_H_
#define _NETLINK_HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <vector>
#include <string>

#include "netlink_helper.h"
// #include "core/addr.h"
// #include "core/link.h"
// #include "core/neigh.h"
// #include "core/route.h"
// #include "core/netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

inline static void
parse_rtattr(const void* buf, size_t buflen,
    struct rtattr* attrs[], size_t max_attrs);

struct rta_array {
 private:
  struct rtattr* attrs[50000];
 public:
  rta_array(struct rtattr* rta_head, size_t rta_len)
  {
    memset(attrs, 0, sizeof(attrs));
    parse_rtattr(rta_head, rta_len, attrs,
        sizeof(attrs)/sizeof(attrs[0]));
  }
  const struct rtattr* get(uint16_t type) const
  { return attrs[type]; }
};

inline static void
parse_rtattr(const void* buf, size_t buflen,
    struct rtattr* attrs[], size_t max_attrs)
{
  size_t rta_len = buflen;
  for (struct rtattr* rta = (struct rtattr*)buf;
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    if (rta->rta_type >= max_attrs)
      printf("rta->rta_type:%u, max_attr:%zd\n",
          rta->rta_type, max_attrs);
    assert(rta->rta_type < max_attrs);
    attrs[rta->rta_type] = rta;
  }
}

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
  memset(buf, 0, sizeof(buf));
  inet_ntop(afi, ptr, buf, sizeof(buf));
  return buf;
}

inline static std::string
ifindex2str(uint32_t ifindex)
{
  char str[IF_NAMESIZE];
  char* ret = if_indextoname(ifindex, str);
  return ret?ret:"none";
}

inline static size_t
rta_payload(const struct rtattr* rta)
{ return rta->rta_len-sizeof(struct rtattr); }

inline static uint8_t*
rtattr_payload_ptr(const struct rtattr* rta)
{ return (uint8_t*)(rta + 1); }

inline static uint8_t
rta_read8(const struct rtattr* attr)
{
  if (rta_payload(attr) > sizeof(uint8_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rta_payload(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rta_payload(attr));
    exit(1);
  }
  uint8_t val = *(uint8_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static void*
rta_readptr(const struct rtattr* rta)
{ return (void*)(rta + 1); }

inline static uint16_t
rta_read16(const struct rtattr* attr)
{
  if (rta_payload(attr) > sizeof(uint16_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rta_payload(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rta_payload(attr));
    exit(1);
  }
  uint16_t val = *(uint16_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static uint32_t
rta_read32(const struct rtattr* attr)
{
  if (rta_payload(attr) > sizeof(uint32_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rta_payload(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rta_payload(attr));
    exit(1);
  }
  uint32_t val = *(uint32_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static uint64_t
rta_read64(const struct rtattr* attr)
{
  if (rta_payload(attr) > sizeof(uint64_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rta_payload(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rta_payload(attr));
    exit(1);
  }
  uint64_t val = *(uint64_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static const char*
rta_readstr(const struct rtattr* attr)
{
  const char* strptr = (const char*)rtattr_payload_ptr(attr);
  const size_t strbuflen = strlen(strptr);
  const size_t payloadlen = rta_payload(attr);
  if (payloadlen < strbuflen) {
    fprintf(stderr, "%s: read miss (type=%u)\n", __func__, attr->rta_type);
    printf("payloadlen: %zd\n", payloadlen);
    printf("strbuflen: %zd\n", strbuflen);
    exit(1);
  }
  return strptr;
}

inline static const char*
nlmsg_type_to_str(uint16_t type)
{
  switch (type) {
    case RTM_NEWLINK: return "RTM_NEWLINK";
    case RTM_DELLINK: return "RTM_DELLINK";
    case RTM_GETLINK: return "RTM_GETLINK";
    case RTM_NEWADDR: return "RTM_NEWADDR";
    case RTM_DELADDR: return "RTM_DELADDR";
    case RTM_GETADDR: return "RTM_GETADDR";
    case RTM_NEWROUTE: return "RTM_NEWROUTE";
    case RTM_DELROUTE: return "RTM_DELROUTE";
    case RTM_GETROUTE: return "RTM_GETROUTE";
    case RTM_NEWNEIGH: return "RTM_NEWNEIGH";
    case RTM_DELNEIGH: return "RTM_DELNEIGH";
    case RTM_GETNEIGH: return "RTM_GETNEIGH";
    case RTM_NEWRULE: return "RTM_NEWRULE";
    case RTM_DELRULE: return "RTM_DELRULE";
    case RTM_GETRULE: return "RTM_GETRULE";
    case RTM_NEWQDISC: return "RTM_NEWQDISC";
    case RTM_DELQDISC: return "RTM_DELQDISC";
    case RTM_GETQDISC: return "RTM_GETQDISC";
    case RTM_NEWTCLASS: return "RTM_NEWTCLASS";
    case RTM_DELTCLASS: return "RTM_DELTCLASS";
    case RTM_GETTCLASS: return "RTM_GETTCLASS";
    case RTM_NEWTFILTER: return "RTM_NEWTFILTER";
    case RTM_DELTFILTER: return "RTM_DELTFILTER";
    case RTM_GETTFILTER: return "RTM_GETTFILTER";
    case RTM_NEWACTION: return "RTM_NEWACTION";
    case RTM_DELACTION: return "RTM_DELACTION";
    case RTM_GETACTION: return "RTM_GETACTION";
    case RTM_NEWPREFIX: return "RTM_NEWPREFIX";
    case RTM_GETMULTICAST: return "RTM_GETMULTICAST";
    case RTM_GETANYCAST: return "RTM_GETANYCAST";
    case RTM_NEWNEIGHTBL: return "RTM_NEWNEIGHTBL";
    case RTM_GETNEIGHTBL: return "RTM_GETNEIGHTBL";
    case RTM_SETNEIGHTBL: return "RTM_SETNEIGHTBL";
    case RTM_NEWNDUSEROPT: return "RTM_NEWNDUSEROPT";
    case RTM_NEWADDRLABEL: return "RTM_NEWADDRLABEL";
    case RTM_DELADDRLABEL: return "RTM_DELADDRLABEL";
    case RTM_GETADDRLABEL: return "RTM_GETADDRLABEL";
    case RTM_GETDCB: return "RTM_GETDCB";
    case RTM_SETDCB: return "RTM_SETDCB";
    case RTM_NEWNETCONF: return "RTM_NEWNETCONF";
    case RTM_DELNETCONF: return "RTM_DELNETCONF";
    case RTM_GETNETCONF: return "RTM_GETNETCONF";
    case RTM_NEWMDB: return "RTM_NEWMDB";
    case RTM_DELMDB: return "RTM_DELMDB";
    case RTM_GETMDB: return "RTM_GETMDB";
    case RTM_NEWNSID: return "RTM_NEWNSID";
    case RTM_DELNSID: return "RTM_DELNSID";
    case RTM_GETNSID: return "RTM_GETNSID";
    case RTM_NEWSTATS: return "RTM_NEWSTATS";
    case RTM_GETSTATS: return "RTM_GETSTATS";
    case RTM_NEWNEXTHOP: return "RTM_NEWNEXTHOP";
    case RTM_DELNEXTHOP: return "RTM_DELNEXTHOP";
    case RTM_GETNEXTHOP: return "RTM_GETNEXTHOP";
    case RTM_NEWCACHEREPORT: return "RTM_NEWCACHEREPORT";
    default: return "RTM_XXXUNKNOWNXXX";
  }
}

inline static std::string
RTNLGRP_flags_to_str(uint32_t f)
{
  std::string str;
	if (f & RTNLGRP_NONE         ) str += "|NONE";
	if (f & RTNLGRP_LINK         ) str += "|LINK";
	if (f & RTNLGRP_NOTIFY       ) str += "|NOTIFY";
	if (f & RTNLGRP_NEIGH        ) str += "|NEIGH";
	if (f & RTNLGRP_TC           ) str += "|TC";
	if (f & RTNLGRP_IPV4_IFADDR  ) str += "|IPV4_IFADDR";
	if (f & RTNLGRP_IPV4_MROUTE  ) str += "|IPV4_MROUTE";
	if (f & RTNLGRP_IPV4_ROUTE   ) str += "|IPV4_ROUTE";
	if (f & RTNLGRP_IPV4_RULE    ) str += "|IPV4_RULE";
	if (f & RTNLGRP_IPV6_IFADDR  ) str += "|IPV6_IFADDR";
	if (f & RTNLGRP_IPV6_MROUTE  ) str += "|IPV6_MROUTE";
	if (f & RTNLGRP_IPV6_ROUTE   ) str += "|IPV6_ROUTE";
	if (f & RTNLGRP_IPV6_IFINFO  ) str += "|IPV6_IFINFO";
	if (f & RTNLGRP_DECnet_IFADDR) str += "|DECnet_IFADDR";
	if (f & RTNLGRP_NOP2         ) str += "|NOP2";
	if (f & RTNLGRP_DECnet_ROUTE ) str += "|DECnet_ROUTE";
	if (f & RTNLGRP_DECnet_RULE  ) str += "|DECnet_RULE";
	if (f & RTNLGRP_NOP4         ) str += "|NOP4";
	if (f & RTNLGRP_IPV6_PREFIX  ) str += "|IPV6_PREFIX";
	if (f & RTNLGRP_IPV6_RULE    ) str += "|IPV6_RULE";
	if (f & RTNLGRP_ND_USEROPT   ) str += "|ND_USEROPT";
	if (f & RTNLGRP_PHONET_IFADDR) str += "|PHONET_IFADDR";
	if (f & RTNLGRP_PHONET_ROUTE ) str += "|PHONET_ROUTE";
	if (f & RTNLGRP_DCB          ) str += "|DCB";
	if (f & RTNLGRP_IPV4_NETCONF ) str += "|IPV4_NETCONF";
	if (f & RTNLGRP_IPV6_NETCONF ) str += "|IPV6_NETCONF";
	if (f & RTNLGRP_MDB          ) str += "|MDB";
	if (f & RTNLGRP_MPLS_ROUTE   ) str += "|MPLS_ROUTE";
	if (f & RTNLGRP_NSID         ) str += "|NSID";
	if (f & RTNLGRP_MPLS_NETCONF ) str += "|MPLS_NETCONF";
	if (f & RTNLGRP_IPV4_MROUTE_R) str += "|IPV4_MROUTE_R";
	if (f & RTNLGRP_IPV6_MROUTE_R) str += "|IPV6_MROUTE_R";
  return str;
}

#ifdef __cplusplus
}
#endif
#endif /* _NETLINK_HELPER_H_ */
