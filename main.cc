
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <algorithm>
#include <arpa/inet.h>
#include "yalin/yalin.h"
#include "stub.h"

namespace routerd {

struct ifaddr {
  uint16_t ifindex;
  uint16_t afi;
  uint16_t prefix;
  uint32_t flags;
  union {
    uint8_t raw[16];
    uint32_t in4;
    struct in6_addr in6;
  } addr;
  ifaddr(const struct ifaddrmsg* ifa, size_t rta_len)
  {
    ifindex = ifa->ifa_index;
    afi     = ifa->ifa_family;
    prefix  = ifa->ifa_prefixlen;

    for (struct rtattr* rta = IFA_RTA(ifa);
         RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
      switch (rta->rta_type) {
        case IFA_ADDRESS:
        {
          uint8_t* addr_ptr = (uint8_t*)(rta+1);
          size_t addr_len = rta->rta_len - sizeof(*rta);
          if (addr_len == 4) memcpy(addr.raw, addr_ptr, addr_len);
          if (addr_len == 16) memcpy(addr.raw, addr_ptr, addr_len);
          break;
        }
        case IFA_FLAGS:
        {
          uint32_t val = *(uint32_t*)(rta+1);
          flags = val;
          break;
        }
        default: break;
      }
    }
  }
  std::string summary() const
  {
    std::string addrstr = inetpton(addr.raw, afi);
    return strfmt("%u: %s/%u 0x%x",
        ifindex, addrstr.c_str(), prefix, flags);
  }
};

static int ip_addr_add(const ifaddr* addr)
{ printf("ADD [%s]\n", addr->summary().c_str()); return -1; }
static int ip_addr_del(const ifaddr* addr)
{ printf("DEL [%s]\n", addr->summary().c_str()); return -1; }

inline static void
monitor_NEWADDR(const struct nlmsghdr* hdr)
{
  const struct ifaddrmsg* ifa = (struct ifaddrmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::ifaddr addr(ifa, ifa_payload_len);
  ip_addr_add(&addr);
}

inline static void
monitor_DELADDR(const struct nlmsghdr* hdr)
{
  const struct ifaddrmsg* ifa = (struct ifaddrmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::ifaddr addr(ifa, ifa_payload_len);
  ip_addr_del(&addr);
}

inline static int
monitor(const struct sockaddr_nl *who,
         struct rtnl_ctrl_data* _dum_,
         struct nlmsghdr *n, void *arg)
{
  switch (n->nlmsg_type) {
    case RTM_NEWADDR: monitor_NEWADDR(n); break;
    case RTM_DELADDR: monitor_DELADDR(n); break;
    default: break;
  }
  return 0;
}

} /* namespace routerd */

int
main(int argc, char **argv)
{
  uint32_t groups = ~0U;
  netlink_t* nl = netlink_open(groups, NETLINK_ROUTE);
  if (nl == NULL)
    return 1;

  int ret = netlink_listen(nl, routerd::monitor, NULL);
  if (ret < 0)
    return 1;

  netlink_close(nl);
}

