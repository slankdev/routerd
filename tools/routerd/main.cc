
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
#include "link.h"
#include "addr.h"
#include "route.h"
#include "neigh.h"

netlink_cache_t* nlc;

namespace routerd {

static int ip_link_add(const link* link)
{
  std::string cli = link->to_iproute2_cli(RTM_NEWLINK).c_str();
  printf(" --> %s\n", cli.c_str());
  return -1;
}
static int ip_link_del(const link* link)
{
  std::string cli = link->to_iproute2_cli(RTM_DELLINK).c_str();
  if (cli.size() > 0) printf(" --> %s\n", cli.c_str());
  return -1;
}
static int ip_addr_add(const ifaddr* addr)
{
  printf(" --> %s\n", addr->to_iproute2_cli(RTM_NEWADDR).c_str());
  return -1;
}
static int ip_addr_del(const ifaddr* addr)
{
  printf(" --> %s\n", addr->to_iproute2_cli(RTM_DELADDR).c_str());
  return -1;
}
static int ip_route_add(const route* route)
{
  std::string cli = route->to_iproute2_cli(RTM_NEWROUTE).c_str();
  if (cli.size() > 0) printf(" --> %s\n", cli.c_str());
  return -1;
}
static int ip_route_del(const route* route)
{
  std::string cli = route->to_iproute2_cli(RTM_DELROUTE).c_str();
  if (cli.size() > 0) printf(" --> %s\n", cli.c_str());
  return -1;
}
static int ip_neigh_add(const neigh* nei)
{
  std::string cli = nei->to_iproute2_cli(RTM_NEWNEIGH).c_str();
  if (cli.size() > 0) printf(" --> %s\n", cli.c_str());
  return -1;
}
static int ip_neigh_del(const neigh* nei)
{
  std::string cli = nei->to_iproute2_cli(RTM_DELNEIGH).c_str();
  if (cli.size() > 0) printf(" --> %s\n", cli.c_str());
  return -1;
}

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

  nlc = netlink_cache_alloc(nl);
  if (nl == NULL)
    return 1;

  int ret = netlink_listen(nl, routerd::monitor, nullptr);
  if (ret < 0)
    return 1;

  netlink_close(nl);
}

