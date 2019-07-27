
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <algorithm>
#include <arpa/inet.h>
#include <yalin/yalin.h>

netlink_cache_t* nlc;

inline static void
monitor_NEWLINK(const struct nlmsghdr* hdr)
{
  const struct ifinfomsg* ifi = (struct ifinfomsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::link link(ifi, ifa_payload_len);

  std::string cli = link.to_iproute2_cli(RTM_NEWLINK, nlc).c_str();
  printf(" --> %s\n", cli.c_str());
}

inline static void
monitor_DELLINK(const struct nlmsghdr* hdr)
{
  const struct ifinfomsg* ifi = (struct ifinfomsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::link link(ifi, ifa_payload_len);

  std::string cli = link.to_iproute2_cli(RTM_DELLINK, nlc).c_str();
  printf(" --> %s\n", cli.c_str());
}

inline static void
monitor_NEWADDR(const struct nlmsghdr* hdr)
{
  const struct ifaddrmsg* ifa = (struct ifaddrmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::ifaddr addr(ifa, ifa_payload_len);

  std::string cli = addr.to_iproute2_cli(RTM_NEWADDR).c_str();
  printf(" --> %s\n", cli.c_str());
}

inline static void
monitor_DELADDR(const struct nlmsghdr* hdr)
{
  const struct ifaddrmsg* ifa = (struct ifaddrmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::ifaddr addr(ifa, ifa_payload_len);

  std::string cli = addr.to_iproute2_cli(RTM_DELADDR).c_str();
  printf(" --> %s\n", cli.c_str());
}

inline static void
monitor_NEWROUTE(const struct nlmsghdr* hdr)
{
  const struct rtmsg* rtm = (struct rtmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::route route(rtm, ifa_payload_len);

  std::string cli = route.to_iproute2_cli(RTM_NEWROUTE).c_str();
  printf(" --> %s\n", cli.c_str());
}

inline static void
monitor_DELROUTE(const struct nlmsghdr* hdr)
{
  const struct rtmsg* rtm = (struct rtmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::route route(rtm, ifa_payload_len);

  std::string cli = route.to_iproute2_cli(RTM_DELROUTE).c_str();
  printf(" --> %s\n", cli.c_str());
}

inline static void
monitor_NEWNEIGH(const struct nlmsghdr* hdr)
{
  struct ndmsg* ndm = (struct ndmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::neigh nei(ndm, ifa_payload_len);

  std::string cli = nei.to_iproute2_cli(RTM_NEWNEIGH).c_str();
  printf(" --> %s\n", cli.c_str());
}

inline static void
monitor_DELNEIGH(const struct nlmsghdr* hdr)
{
  struct ndmsg* ndm = (struct ndmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::neigh nei(ndm, ifa_payload_len);

  std::string cli = nei.to_iproute2_cli(RTM_DELNEIGH).c_str();
  printf(" --> %s\n", cli.c_str());

}

inline static int
monitor(const struct sockaddr_nl *who [[gnu::unused]],
         struct rtnl_ctrl_data* _dum_ [[gnu::unused]],
         struct nlmsghdr *n, void *arg [[gnu::unused]])
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

int
main()
{
  uint32_t groups = ~0U;
  printf("routerd-start\n");
  printf("subscribe groups: 0x%08x<%s>\n",
      groups, RTNLGRP_flags_to_str(groups).c_str());
  netlink_t* nl = netlink_open(groups, NETLINK_ROUTE);
  if (nl == NULL)
    return 1;

  nlc = netlink_cache_alloc(nl);
  if (nl == NULL)
    return 1;

  int ret = netlink_listen(nl, monitor, nullptr);
  if (ret < 0)
    return 1;

  netlink_close(nl);
}

