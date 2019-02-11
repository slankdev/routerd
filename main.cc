
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

namespace routerd {

union addr_t {
  uint32_t in4;
  struct in6_addr in6;
  uint8_t raw[16];
};

struct link {
  uint32_t ifindex;
  uint32_t type;
  uint32_t change;
  uint32_t flags;
  link(const struct ifinfomsg* ifm, size_t rta_len)
  {
    ifindex = ifm->ifi_index;
    type = ifm->ifi_type;
    change = ifm->ifi_change;
    flags = ifm->ifi_flags;

    struct rtattr* attrs[1000];
    memset(attrs, 0x0, sizeof(attrs));
    parse_rtattr(IFLA_RTA(ifm), rta_len, attrs,
        sizeof(attrs)/sizeof(attrs[0]));
  }
  std::string summary() const
  {
    return strfmt("if=%u type=%u change=0x%x flags=0x%x",
        ifindex, type, change, flags);
  }
}; /* struct link */

struct ifaddr {
  uint16_t ifindex;
  uint16_t afi;
  uint16_t prefix;
  uint32_t flags;
  addr_t addr;
  ifaddr(const struct ifaddrmsg* ifa, size_t rta_len)
  {
    ifindex = ifa->ifa_index;
    afi     = ifa->ifa_family;
    prefix  = ifa->ifa_prefixlen;

    struct rtattr* attrs[1000];
    memset(attrs, 0x0, sizeof(attrs));
    parse_rtattr(IFA_RTA(ifa), rta_len, attrs,
        sizeof(attrs)/sizeof(attrs[0]));

    if (attrs[IFA_ADDRESS]) {
      struct rtattr* rta = attrs[IFA_ADDRESS];
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      if (addr_len == 4) memcpy(addr.raw, addr_ptr, addr_len);
      if (addr_len == 16) memcpy(addr.raw, addr_ptr, addr_len);
    }
    if (attrs[IFA_FLAGS]) {
      struct rtattr* rta = attrs[IFA_ADDRESS];
      uint32_t val = *(uint32_t*)(rta+1);
      flags = val;
    }
  }
  std::string summary() const
  {
    std::string addrstr = inetpton(addr.raw, afi);
    return strfmt("%s/%u if=%u 0x%x",
        addrstr.c_str(), prefix, ifindex, flags);
  }
}; /* struct ifaddr */

struct route {
  uint16_t table;
  uint16_t oif_index;
  uint16_t iif_index;
  uint32_t priority;
  uint16_t afi;
  uint32_t dst_pref;
  uint32_t src_pref;
  uint32_t proto;
  addr_t dst;
  addr_t src;
  addr_t gw;

  route(const struct rtmsg* rtm, size_t rta_len)
  {
    memset(this, 0, sizeof(*this));

    afi       = rtm->rtm_family;
    table     = rtm->rtm_table;
    dst_pref  = rtm->rtm_dst_len;
    src_pref  = rtm->rtm_src_len;
    proto     = rtm->rtm_protocol;

    struct rtattr* attrs[1000];
    memset(attrs, 0x0, sizeof(attrs));
    parse_rtattr(RTM_RTA(rtm), rta_len, attrs,
        sizeof(attrs)/sizeof(attrs[0]));

    if (attrs[RTA_PRIORITY]) {
      struct rtattr* rta = attrs[RTA_PRIORITY];
      assert(rta->rta_len == 8);
      uint32_t val = *(uint32_t*)(rta+1);
      priority = val;
    }
    if (attrs[RTA_IIF]) {
      struct rtattr* rta = attrs[RTA_IIF];
      assert(rta->rta_len == 8);
      uint32_t val = *(uint32_t*)(rta+1);
      iif_index = val;
    }
    if (attrs[RTA_OIF]) {
      struct rtattr* rta = attrs[RTA_OIF];
      assert(rta->rta_len == 8);
      uint32_t val = *(uint32_t*)(rta+1);
      oif_index = val;
    }
    if (attrs[RTA_DST]) {
      struct rtattr* rta = attrs[RTA_DST];
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      if (addr_len == 4) memcpy(dst.raw, addr_ptr, addr_len);
      if (addr_len == 16) memcpy(dst.raw, addr_ptr, addr_len);
    }
    if (attrs[RTA_SRC]) {
      struct rtattr* rta = attrs[RTA_SRC];
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      if (addr_len == 4) memcpy(src.raw, addr_ptr, addr_len);
      if (addr_len == 16) memcpy(src.raw, addr_ptr, addr_len);
    }
    if (attrs[RTA_GATEWAY]) {
      struct rtattr* rta = attrs[RTA_GATEWAY];
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      if (addr_len == 4) memcpy(gw.raw, addr_ptr, addr_len);
      if (addr_len == 16) memcpy(gw.raw, addr_ptr, addr_len);
    }
  }
  std::string summary() const
  {
    std::string src_str = inetpton(src.raw, afi);
    std::string dst_str = inetpton(dst.raw, afi);
    std::string gw_str = inetpton(gw.raw, afi);
    return strfmt("s=%s/%u d=%s/%u gw=%s iif=%u oif=%u "
      "prio=%u tab=%u proto=%u",
      src_str.c_str(), src_pref, dst_str.c_str(), dst_pref,
      gw_str.c_str(), iif_index, oif_index, priority, table, proto);
  }
}; /* struct ifaddr */

struct neigh {
  uint32_t ifindex;
  uint32_t afi;
  addr_t addr;
  uint8_t lladdr[6];
  uint32_t state;
  neigh(const struct ndmsg* ndm, size_t rta_len)
  {
    memset(this, 0, sizeof(*this));
    ifindex = ndm->ndm_ifindex;
    afi = ndm->ndm_family;
    state = ndm->ndm_state;

    struct rtattr* attrs[1000];
    memset(attrs, 0x0, sizeof(attrs));
    parse_rtattr(NDM_RTA(ndm), rta_len, attrs,
        sizeof(attrs)/sizeof(attrs[0]));

    if (attrs[ NDA_LLADDR]) {
      struct rtattr* rta = attrs[NDA_LLADDR];
      assert(rta->rta_len == 10);
      uint8_t* ptr = (uint8_t*)(rta+1);
      memcpy(lladdr, ptr, sizeof(lladdr));
    }
    if (attrs[ NDA_DST]) {
      struct rtattr* rta = attrs[NDA_DST];
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      if (addr_len == 4) memcpy(addr.raw, addr_ptr, addr_len);
      if (addr_len == 16) memcpy(addr.raw, addr_ptr, addr_len);
    }
  }
  std::string summary() const
  {
    std::string addr_str = inetpton(addr.raw, afi);
    return strfmt("%s lladdr="
        "%02x:%02x:%02x:%02x:%02x:%02x if=%u state=0x%x",
        addr_str.c_str(),
        lladdr[0], lladdr[1], lladdr[2],
        lladdr[3], lladdr[4], lladdr[5],
        ifindex, state);
  }
}; /* struct neigh */

static int ip_link_add(const link* link)
{ printf("NEWLINK  [%s]\n", link->summary().c_str()); return -1; }
static int ip_link_del(const link* link)
{ printf("DELLINK  [%s]\n", link->summary().c_str()); return -1; }
static int ip_addr_add(const ifaddr* addr)
{ printf("NEWADDR  [%s]\n", addr->summary().c_str()); return -1; }
static int ip_addr_del(const ifaddr* addr)
{ printf("DELADDR  [%s]\n", addr->summary().c_str()); return -1; }
static int ip_route_add(const route* route)
{ printf("NEWROUTE [%s]\n", route->summary().c_str()); return -1; }
static int ip_route_del(const route* route)
{ printf("DELROUTE [%s]\n", route->summary().c_str()); return -1; }
static int ip_neigh_add(const neigh* nei)
{ printf("NEWNEIGH [%s]\n", nei->summary().c_str()); return -1; }
static int ip_neigh_del(const neigh* nei)
{ printf("DELNEIGH [%s]\n", nei->summary().c_str()); return -1; }

inline static void
monitor_NEWLINK(const struct nlmsghdr* hdr)
{
  const struct ifinfomsg* ifi = (struct ifinfomsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::link link(ifi, ifa_payload_len);
  ip_link_add(&link);
}

inline static void
monitor_DELLINK(const struct nlmsghdr* hdr)
{
  const struct ifinfomsg* ifi = (struct ifinfomsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::link link(ifi, ifa_payload_len);
  ip_link_del(&link);
}

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

inline static void
monitor_NEWROUTE(const struct nlmsghdr* hdr)
{
  const struct rtmsg* rtm = (struct rtmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::route route(rtm, ifa_payload_len);
  ip_route_add(&route);
}

inline static void
monitor_DELROUTE(const struct nlmsghdr* hdr)
{
  const struct rtmsg* rtm = (struct rtmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::route route(rtm, ifa_payload_len);
  ip_route_del(&route);
}

inline static void
monitor_NEWNEIGH(const struct nlmsghdr* hdr)
{
  struct ndmsg* ndm = (struct ndmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::neigh nei(ndm, ifa_payload_len);
  ip_neigh_add(&nei);
}

inline static void
monitor_DELNEIGH(const struct nlmsghdr* hdr)
{
  struct ndmsg* ndm = (struct ndmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::neigh nei(ndm, ifa_payload_len);
  ip_neigh_del(&nei);
}

inline static int
monitor(const struct sockaddr_nl *who,
         struct rtnl_ctrl_data* _dum_,
         struct nlmsghdr *n, void *arg)
{
  switch (n->nlmsg_type) {
    case RTM_NEWLINK: monitor_NEWLINK(n); break;
    case RTM_DELLINK: monitor_DELLINK(n); break;
    case RTM_NEWADDR: monitor_NEWADDR(n); break;
    case RTM_DELADDR: monitor_DELADDR(n); break;
    case RTM_NEWROUTE: monitor_NEWROUTE(n); break;
    case RTM_DELROUTE: monitor_DELROUTE(n); break;
    case RTM_NEWNEIGH: monitor_NEWNEIGH(n); break;
    case RTM_DELNEIGH: monitor_DELNEIGH(n); break;
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

