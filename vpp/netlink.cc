
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
#include "netlink.h"
#include "vpp.h"
#include "routerd.h"
#include "debug.h"

netlink_cache_t* nlc;
netlink_counter counter;
routerd_context rd_ctx;

static void set_link(uint32_t vpp_ifindex, bool is_up)
{
  if (connect_to_vpp("routerd", true) < 0) {
    printf("%s: Couldn't connect to vpe, exiting...\r\n", __func__);
    return;
  }
  uint16_t msg_id = random();
  set_interface_flag(find_msg_id(SET_IFC_FLAGS), msg_id, vpp_ifindex, is_up);
  int ret = vpp_waitmsg_retval();
  if (ret < 0) {
    printf("%s: failed\r\n", __func__);
  }
  disconnect_from_vpp ();
}

static void set_addr(uint32_t vpp_ifindex, uint32_t addr, uint8_t addr_len, bool is_add)
{
  if (connect_to_vpp("routerd", true) < 0) {
    printf("%s: Couldn't connect to vpe, exiting...\r\n", __func__);
    return;
  }
  uint16_t msg_id = random();
  set_interface_addr(find_msg_id(SET_IFC_ADDR), msg_id, vpp_ifindex, is_add,
      false/*is_ipv6*/, &addr, addr_len);
  int ret = vpp_waitmsg_retval();
  if (ret < 0) {
    printf("%s: failed\r\n", __func__);
  }
  disconnect_from_vpp ();
}

static uint32_t
ifindex_kernel2vpp(uint32_t k_idx)
{
  for (size_t i=0; i<rd_ctx.interfaces.size(); i++) {
    if (k_idx == rd_ctx.interfaces[i].kern_ifindex)
      return rd_ctx.interfaces[i].vpp_ifindex;
  }
  return 0;
}

static void
link_analyze_and_hook(const routerd::link &link,
    const netlink_cache_t *nlc)
{
  uint32_t affected_flag = ~uint32_t(0) & link.ifi->ifi_change;
  uint32_t target_bit = link.ifi->ifi_flags & link.ifi->ifi_change;
  if (affected_flag & IFF_UP) {
    bool next_is_up = target_bit & affected_flag;
    uint32_t vpp_ifindex = ifindex_kernel2vpp(link.ifi->ifi_index);
    if (vpp_ifindex == 0) {
      //printf("ignore kern%u\r\n", link.ifi->ifi_index);
      return;
    }
    set_link(vpp_ifindex, next_is_up);
  }
}

static void
addr_analyze_and_hook(const routerd::ifaddr &addr, bool is_new)
{
  if (addr.ifa->ifa_family != AF_INET) {
    printf("is not AF_INET ignore\n");
    return;
  }

  const void *addr_ptr = (const void*)rta_readptr(addr.rtas->get(IFLA_ADDRESS));
  if (!addr_ptr) {
    printf("addr not found\n\r");
    return;
  }

  uint32_t vpp_ifindex = ifindex_kernel2vpp(addr.ifa->ifa_index);
  uint32_t addr_ = *((uint32_t*)addr_ptr);
  uint8_t addr_len = addr.ifa->ifa_prefixlen;
  set_addr(vpp_ifindex, addr_, addr_len, is_new);
}

static void
monitor_NEWLINK(const struct nlmsghdr* hdr)
{
  counter.link.new_cnt++;
  const struct ifinfomsg* ifi = (struct ifinfomsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::link link(ifi, ifa_payload_len);

  if (debug_enabled(NETLINK)) {
    std::string cli = link.to_iproute2_cli(RTM_NEWLINK, nlc).c_str();
    printf(" --> %s\r\n", cli.c_str());
  }

  link_analyze_and_hook(link, nlc);
}

static void
monitor_DELLINK(const struct nlmsghdr* hdr)
{
  counter.link.del_cnt++;
  const struct ifinfomsg* ifi = (struct ifinfomsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::link link(ifi, ifa_payload_len);

  if (debug_enabled(NETLINK)) {
    std::string cli = link.to_iproute2_cli(RTM_DELLINK, nlc).c_str();
    printf(" --> %s\r\n", cli.c_str());
  }
}

static void
monitor_NEWADDR(const struct nlmsghdr* hdr)
{
  counter.addr.new_cnt++;
  const struct ifaddrmsg* ifa = (struct ifaddrmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::ifaddr addr(ifa, ifa_payload_len);

  if (debug_enabled(NETLINK)) {
    std::string cli = addr.to_iproute2_cli(RTM_NEWADDR).c_str();
    printf(" --> %s\r\n", cli.c_str());
  }

  addr_analyze_and_hook(addr, true);
}

static void
monitor_DELADDR(const struct nlmsghdr* hdr)
{
  counter.addr.del_cnt++;
  const struct ifaddrmsg* ifa = (struct ifaddrmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::ifaddr addr(ifa, ifa_payload_len);

  if (debug_enabled(NETLINK)) {
    std::string cli = addr.to_iproute2_cli(RTM_DELADDR).c_str();
    printf(" --> %s\r\n", cli.c_str());
  }

  addr_analyze_and_hook(addr, false);
}

static void
monitor_NEWROUTE(const struct nlmsghdr* hdr)
{
  counter.route.new_cnt++;
  const struct rtmsg* rtm = (struct rtmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::route route(rtm, ifa_payload_len);

  if (debug_enabled(NETLINK)) {
    std::string cli = route.to_iproute2_cli(RTM_NEWROUTE).c_str();
    printf(" --> %s\r\n", cli.c_str());
  }
}

static void
monitor_DELROUTE(const struct nlmsghdr* hdr)
{
  counter.route.del_cnt++;
  const struct rtmsg* rtm = (struct rtmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::route route(rtm, ifa_payload_len);

  if (debug_enabled(NETLINK)) {
    std::string cli = route.to_iproute2_cli(RTM_DELROUTE).c_str();
    printf(" --> %s\r\n", cli.c_str());
  }
}

static void
monitor_NEWNEIGH(const struct nlmsghdr* hdr)
{
  counter.neigh.new_cnt++;
  struct ndmsg* ndm = (struct ndmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::neigh nei(ndm, ifa_payload_len);

  if (debug_enabled(NETLINK)) {
    std::string cli = nei.to_iproute2_cli(RTM_NEWNEIGH).c_str();
    printf(" --> %s\r\n", cli.c_str());
  }
}

static void
monitor_DELNEIGH(const struct nlmsghdr* hdr)
{
  counter.neigh.del_cnt++;
  struct ndmsg* ndm = (struct ndmsg*)(hdr + 1);
  const size_t ifa_payload_len = IFA_PAYLOAD(hdr);
  routerd::neigh nei(ndm, ifa_payload_len);

  if (debug_enabled(NETLINK)) {
    std::string cli = nei.to_iproute2_cli(RTM_DELNEIGH).c_str();
    printf(" --> %s\r\n", cli.c_str());
  }
}

static int
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

static void
vpp_sync_with_nlc(const netlink_cache_t *nlc)
{
  size_t n_link = nlc->links.size();
  for (size_t i=0; i<n_link; i++) {
    const struct ifinfomsg *ifi =
      (const struct ifinfomsg*)nlc->links[i].data();
    uint32_t vpp_index = ifindex_kernel2vpp(ifi->ifi_index);
    if (vpp_index == 0)
      continue;

    printf("link[%zd]: vpp%u kern%u\n", i, vpp_index, ifi->ifi_index);
    bool is_up = ifi->ifi_flags & IFF_UP;
    set_link(vpp_index, is_up);
  }
}

void
netlink_manager()
{
  uint32_t groups = ~0U;
  netlink_t *nl = netlink_open(groups, NETLINK_ROUTE);
  nlc = netlink_cache_alloc(nl);
  vpp_sync_with_nlc(nlc);
  netlink_dump_addr(nl);
  netlink_listen(nl, monitor, nullptr);
  netlink_close(nl);
}

