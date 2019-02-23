
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

netlink_cache_t* nlc;

namespace routerd {

struct ifaddr {
  const struct ifaddrmsg* ifa;
  size_t len;
  rta_array* rtas;

  ifaddr(const struct ifaddrmsg* ifa_, size_t rta_len)
  {
    this->ifa = ifa_;
    this->len = rta_len;
    rtas = new rta_array(IFA_RTA(ifa), rta_len);
  }
  std::string summary() const
  {
    return strfmt("if=%zd afi=%d prefixlen=%d",
        ifa->ifa_index, ifa->ifa_family, ifa->ifa_prefixlen);
  }
  std::string to_iproute2_cli(uint16_t nlmsg_type) const
  {
    const uint8_t* addr_ptr = (const uint8_t*)rta_readptr(rtas->get(IFLA_ADDRESS));
    std::string addr = inetpton(addr_ptr, ifa->ifa_family);
    std::string ifname = ifindex2str(ifa->ifa_index);
    return strfmt("ip -%d addr %s %s/%d dev %s",
        ifa->ifa_family==AF_INET?4:6,
        nlmsg_type==RTM_NEWADDR?"add":"del",
        addr.c_str(), ifa->ifa_prefixlen, ifname.c_str());
  }
}; /* struct ifaddr */

struct route {
  const struct rtmsg* rtm;
  size_t len;
  rta_array* rtas;

  route(const struct rtmsg* rtm_, size_t rta_len)
  {
    this->rtm = rtm_;
    this->len = rta_len;
    this->rtas = new rta_array(RTM_RTA(rtm), len);
  }
  std::string summary() const
  {
    return strfmt("afi=%u slen=%u dlen=%u tab=%u proto=%u",
              rtm->rtm_family, rtm->rtm_src_len, rtm->rtm_dst_len,
              rtm->rtm_table, rtm->rtm_protocol);
  }
  std::string to_iproute2_cli(uint16_t nlmsg_type) const
  {
    std::string src;
    if (rtas->get(RTA_SRC)) {
      auto* src_rta = rtas->get(RTA_SRC);
      auto* ptr = (const void*)rta_readptr(src_rta);
      if (ptr) src = inetpton(ptr, rtm->rtm_family);
    }

    std::string iif;
    if (rtas->get(RTA_IIF)) {
      auto* iif_rta = rtas->get(RTA_IIF);
      uint32_t index = rta_read32(iif_rta);
      iif = ifindex2str(index);
    }

    std::string dst;
    if (rtas->get(RTA_DST)) {
      auto* dst_rta = rtas->get(RTA_DST);
      auto* ptr = (const void*)rta_readptr(dst_rta);
      if (ptr) dst = inetpton(ptr, rtm->rtm_family);
    }

    std::string oif;
    if (rtas->get(RTA_OIF)) {
      auto* oif_rta = rtas->get(RTA_OIF);
      uint32_t index = rta_read32(oif_rta);
      oif = ifindex2str(index);
    }

    std::string gw;
    if (rtas->get(RTA_GATEWAY)) {
      auto* gw_rta = rtas->get(RTA_GATEWAY);
      auto* ptr = (const void*)rta_readptr(gw_rta);
      if (ptr) gw = inetpton(ptr, rtm->rtm_family);
      return strfmt("ip -%u route %s %s/%d via %s dev %s",
          rtm->rtm_family==AF_INET?4:6,
          nlmsg_type==RTM_NEWROUTE?"add":"del",
          dst.c_str(), rtm->rtm_dst_len, gw.c_str(), oif.c_str());
    }

    std::string encap;
    if (rtas->get(RTA_ENCAP_TYPE)) {
      auto* encap_rta = rtas->get(RTA_ENCAP_TYPE);
      uint16_t encap_type = rta_read16(encap_rta);
      if (encap_type == LWTUNNEL_ENCAP_SEG6) {

        std::string mode;
        std::string segs;
        uint8_t* d = (uint8_t*)rta_readptr(rtas->get(RTA_ENCAP));
        size_t l = rta_payload(rtas->get(RTA_ENCAP));
        struct rtattr* sub[50000];
        parse_rtattr(d, l, sub, sizeof(sub)/sizeof(sub[0]));
        if (sub[SEG6_IPTUNNEL_SRH]) {
          const struct seg6_iptunnel_encap* sie;
          sie = (const struct seg6_iptunnel_encap*)(sub[SEG6_IPTUNNEL_SRH]+1);
          switch (sie->mode) {
            case SEG6_IPTUN_MODE_INLINE: mode = "inline"; break;
            case SEG6_IPTUN_MODE_ENCAP: mode = "encap"; break;
            case SEG6_IPTUN_MODE_L2ENCAP: mode = "l2encap"; break;
          }

          const struct ipv6_sr_hdr* srh = sie->srh;
          const size_t n = srh->hdrlen/2;
          for (size_t i=0; i<n; i++) {
            const struct in6_addr* addr = &srh->segments[i];
            const std::string str = inetpton(addr, AF_INET6);
            segs += strfmt("%s%s", str.c_str(), i+1<n?",":"");
          }
        }

        return strfmt("ip -%u route %s %s/%d encap seg6 mode %s segs %s dev %s",
            rtm->rtm_family==AF_INET?4:6, nlmsg_type==RTM_NEWROUTE?"add":"del",
            dst.c_str(), rtm->rtm_dst_len, mode.c_str(), segs.c_str(),
            oif.c_str());

      } else if (encap_type == LWTUNNEL_ENCAP_SEG6_LOCAL) {

        uint8_t* d = (uint8_t*)rta_readptr(rtas->get(RTA_ENCAP));
        size_t l = rta_payload(rtas->get(RTA_ENCAP));
        struct rtattr* sub[50000];
        parse_rtattr(d, l, sub, sizeof(sub)/sizeof(sub[0]));

        std::string action;
        if (sub[SEG6_LOCAL_ACTION]) {
          const struct rtattr* rta = sub[SEG6_LOCAL_ACTION];
          uint32_t num = rta_read32(rta);
          action = strfmt("%s", SEG6_LOCAL_ACTION_to_str2(num));

          if (num == SEG6_LOCAL_ACTION_END_X && sub[SEG6_LOCAL_NH6]) {
            const struct rtattr* rta = sub[SEG6_LOCAL_NH6];
            const void* addr_ptr = rta_readptr(rta);
            action += strfmt(" nh6 %s", inetpton(addr_ptr, AF_INET6).c_str());
          } else if (num == SEG6_LOCAL_ACTION_END_DX2 && sub[SEG6_LOCAL_OIF]) {
            const struct rtattr* rta = sub[SEG6_LOCAL_OIF];
            uint32_t index = rta_read32(rta);
            action += strfmt(" oif %s", ifindex2str(index).c_str());
          } else if (num == SEG6_LOCAL_ACTION_END_T && sub[SEG6_LOCAL_TABLE]) {
            const struct rtattr* rta = sub[SEG6_LOCAL_OIF];
            uint32_t table_id = rta_read32(rta);
            action += strfmt(" table %d", table_id);
          }
        }

        return strfmt("ip -%u route %s %s/%d encap seg6local action %s dev %s",
            rtm->rtm_family==AF_INET?4:6, nlmsg_type==RTM_NEWROUTE?"add":"del",
            dst.c_str(), rtm->rtm_dst_len, action.c_str(),
            oif.c_str());

      }
    }

    return strfmt("ip -%u route %s %s/%d unknown-type",
        rtm->rtm_family==AF_INET?4:6,
        nlmsg_type==RTM_NEWROUTE?"add":"del",
        dst.c_str(), rtm->rtm_dst_len);
  }
}; /* struct ifaddr */

struct neigh {
  const struct ndmsg* ndm;
  size_t len;
  rta_array* rtas;

  neigh(const struct ndmsg* ndm_, size_t rta_len)
  {
    this->ndm = ndm_;
    this->len = rta_len;
    this->rtas = new rta_array(NDM_RTA(ndm), len);
  }
  std::string summary() const
  {
    return strfmt("afi=%u index=%u state=%u flag=0x%01x type=%u",
        ndm->ndm_family, ndm->ndm_ifindex, ndm->ndm_state,
        ndm->ndm_flags, ndm->ndm_type);
  }
  std::string to_iproute2_cli(uint16_t nlmsg_type) const
  {
    std::string lladdr;
    if (rtas->get(NDA_LLADDR)) {
      const struct rtattr* rta = rtas->get(NDA_LLADDR);
      assert(rta->rta_len == 10);
      uint8_t* ptr = (uint8_t*)(rta+1);
      lladdr = strfmt("%02x:%02x:%02x:%02x:%02x:%02x",
          ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
    }

    std::string dst;
    if (rtas->get(NDA_DST)) {
      const struct rtattr* rta = rtas->get(NDA_DST);
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      int afi = addr_len==4?AF_INET:AF_INET6;
      dst = inetpton(addr_ptr, afi);
    }

    return strfmt("ip nei %s %s lladdr %s dev %s",
        nlmsg_type==RTM_NEWNEIGH?"add":"del",
        dst.c_str(), lladdr.c_str(),
        ifindex2str(ndm->ndm_ifindex).c_str());
  }
}; /* struct neigh */

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

