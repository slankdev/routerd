
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

netlink_cache_t* nlc;

namespace routerd {

union addr_t {
  uint32_t in4;
  struct in6_addr in6;
  uint8_t raw[16];
};

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

struct link {
  const struct ifinfomsg* ifi;
  size_t len;
  rta_array* rtas;

  link(const struct ifinfomsg* ifm, size_t rta_len)
  {
    ifi = ifm;
    len = rta_len;
    rtas = new rta_array(IFLA_RTA(ifm), rta_len);
  }
  std::string summary() const
  {
    return strfmt("if=%u type=%u change=0x%x flags=0x%x",
              ifi->ifi_index, ifi->ifi_type,
              ifi->ifi_change, ifi->ifi_flags);
  }
  std::string to_iproute2_cli(uint16_t nlmsg_type) const
  {
    std::string str;
    std::string ifname = rta_readstr((rtas->get(IFLA_IFNAME)));
    uint32_t ifindex = ifi->ifi_index;
    uint32_t flags = ifi->ifi_flags;
    uint32_t change = ifi->ifi_change;
    if (~ifi->ifi_change == 0) { // change is 0xff..ff
      uint8_t* d = (uint8_t*)rta_readptr(rtas->get(IFLA_LINKINFO));
      size_t l = rta_payload(rtas->get(IFLA_LINKINFO));
      struct rtattr* sub[50000];
      parse_rtattr(d, l, sub, sizeof(sub)/sizeof(sub[0]));
      std::string tname = rta_readstr(sub[IFLA_INFO_KIND]);
      std::string ope = nlmsg_type==RTM_NEWLINK?"add":"del";
      str += strfmt("ip link %s %s type %s",
          ope.c_str(), ifname.c_str(), tname.c_str());
      if (tname == "vlan") {
        uint32_t ii = rta_read32(rtas->get(IFLA_LINK));
        uint8_t* sd = (uint8_t*)rta_readptr(sub[IFLA_INFO_DATA]);
        size_t sdl = rta_payload(sub[IFLA_INFO_DATA]);
        struct rtattr* subsub[50000];
        parse_rtattr(sd, sdl, subsub, sizeof(subsub)/sizeof(subsub[0]));
        std::string lname = ifindex2str(ii);
        uint16_t id = rta_read16(subsub[IFLA_VLAN_ID]);
        str += strfmt(" link %s id %u", lname.c_str(), id);
      } else if (tname == "dummy") {
      } else if (tname == "bridge") {
      } else if (tname == "vrf") {
        uint8_t* sd = (uint8_t*)rta_readptr(sub[IFLA_INFO_DATA]);
        size_t sdl = rta_payload(sub[IFLA_INFO_DATA]);
        struct rtattr* subsub[50000];
        parse_rtattr(sd, sdl, subsub, sizeof(subsub)/sizeof(subsub[0]));
        uint32_t tableid = rta_read32(subsub[IFLA_VRF_TABLE]);
        str += strfmt(" table %u", tableid);
      } else {
        str += " UNKNOWNOPT???";
      }
    } else if (change == 0) {

      const struct ifinfomsg* _cache = netlink_cache_get_link(nlc, ifindex);
      const size_t _cache_len = netlink_cachelen_get_link(nlc, ifindex);
      if (_cache) {
        rta_array cache(IFLA_RTA(_cache), _cache_len);
        std::string lname = ifindex2str(ifi->ifi_index);

        auto old_mtu = rta_read32(cache.get(IFLA_MTU));
        auto new_mtu = rta_read32(rtas->get(IFLA_MTU));
        if (old_mtu != new_mtu) {
          str += strfmt("ip link set %s mtu %u", lname.c_str(), new_mtu);
          // str += strfmt(" (old:%u)", old_mtu)
        }
        uint8_t* old_addr = (uint8_t*)rta_readptr(cache.get(IFLA_ADDRESS));
        uint8_t* new_addr = (uint8_t*)rta_readptr(rtas->get(IFLA_ADDRESS));
        if (memcmp(old_addr, new_addr, 6) != 0) {
          str += strfmt("ip link set %s address "
                  "%02x:%02x:%02x:%02x:%02x:%02x", lname.c_str(),
                  new_addr[0], new_addr[1], new_addr[2],
                  new_addr[3], new_addr[4], new_addr[5]);
          // str += strfmt(" (old:%02x:%02x:%02x:%02x:%02x:%02x)",
          //         old_addr[0], old_addr[1], old_addr[2],
          //         old_addr[3], old_addr[4], old_addr[5]);
        }
        netlink_cache_update_link(nlc, ifi, len);
      }

    } else {
      str += strfmt("ip link set dev %s ", ifname.c_str());
      if (change) {
        uint32_t affected_flag = ~uint32_t(0) & change;
        uint32_t destination_bit = flags & change;
        if (affected_flag & IFF_UP) str += destination_bit&affected_flag?"up":"down";
        if (affected_flag & IFF_PROMISC) str += destination_bit&affected_flag?"promisc on":"promisc off";
      } else {
        str += "UNSUPPORTED...?";
      }
    }
    return str;
  }
}; /* struct link */

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
    auto* src_rta = rtas->get(RTA_SRC);
    if (src_rta) {
      auto* ptr = (const void*)rta_readptr(src_rta);
      if (ptr) src = inetpton(ptr, rtm->rtm_family);
    }

    std::string iif;
    auto* iif_rta = rtas->get(RTA_IIF);
    if (iif_rta) {
      uint32_t index = rta_read32(iif_rta);
      iif = ifindex2str(index);
    }

    std::string dst;
    auto* dst_rta = rtas->get(RTA_DST);
    if (dst_rta) {
      auto* ptr = (const void*)rta_readptr(dst_rta);
      if (ptr) dst = inetpton(ptr, rtm->rtm_family);
    }

    std::string oif;
    auto* oif_rta = rtas->get(RTA_OIF);
    if (oif_rta) {
      uint32_t index = rta_read32(oif_rta);
      oif = ifindex2str(index);
    }

    std::string gw;
    auto* gw_rta = rtas->get(RTA_GATEWAY);
    if (gw_rta) {
      auto* ptr = (const void*)rta_readptr(gw_rta);
      if (ptr) gw = inetpton(ptr, rtm->rtm_family);

      return strfmt("ip -%u route %s %s/%d via %s dev %s",
          rtm->rtm_family==AF_INET?4:6,
          nlmsg_type==RTM_NEWROUTE?"add":"del",
          dst.c_str(), rtm->rtm_dst_len, gw.c_str(), oif.c_str());
    }

    std::string encap;
    auto* encap_rta = rtas->get(RTA_ENCAP_TYPE);
    if (encap_rta) {
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
        return strfmt("seg6local");
      }
    }

    /* assert */
    return strfmt("ip -%u route %s %s/%d unknown-type",
        rtm->rtm_family==AF_INET?4:6,
        nlmsg_type==RTM_NEWROUTE?"add":"del",
        dst.c_str(), rtm->rtm_dst_len);
  }
}; /* struct ifaddr */

struct neigh {
  uint32_t ifindex;
  uint32_t afi;
  addr_t addr;
  uint8_t lladdr[6];
  uint32_t state;
  struct rtattr* attrs[1000];

  neigh(const struct ndmsg* ndm, size_t rta_len)
  {
    memset(this, 0, sizeof(*this));
    ifindex = ndm->ndm_ifindex;
    afi = ndm->ndm_family;
    state = ndm->ndm_state;

    memset(attrs, 0x0, sizeof(attrs));
    parse_rtattr(NDM_RTA(ndm), rta_len, attrs,
        sizeof(attrs)/sizeof(attrs[0]));

    if (attrs[NDA_LLADDR]) {
      struct rtattr* rta = attrs[NDA_LLADDR];
      assert(rta->rta_len == 10);
      uint8_t* ptr = (uint8_t*)(rta+1);
      memcpy(lladdr, ptr, sizeof(lladdr));
    }
    if (attrs[NDA_DST]) {
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
{
  printf("NEWLINK  [%s]", link->summary().c_str());
  printf(" --> %s\n", link->to_iproute2_cli(RTM_NEWLINK).c_str());
  return -1;
}
static int ip_link_del(const link* link)
{
  printf("DELLINK  [%s]", link->summary().c_str());
  printf(" --> %s\n", link->to_iproute2_cli(RTM_DELLINK).c_str());
  return -1;
}
static int ip_addr_add(const ifaddr* addr)
{
  printf("NEWADDR  [%s]", addr->summary().c_str());
  printf(" --> %s\n", addr->to_iproute2_cli(RTM_NEWADDR).c_str());
  return -1;
}
static int ip_addr_del(const ifaddr* addr)
{
  printf("DELADDR  [%s]", addr->summary().c_str());
  printf(" --> %s\n", addr->to_iproute2_cli(RTM_DELADDR).c_str());
  return -1;
}
static int ip_route_add(const route* route)
{
  printf("NEWROUTE [%s]", route->summary().c_str());
  printf(" --> %s\n", route->to_iproute2_cli(RTM_NEWROUTE).c_str());
  return -1;
}
static int ip_route_del(const route* route)
{
  printf("DELROUTE [%s]", route->summary().c_str());
  printf(" --> %s\n", route->to_iproute2_cli(RTM_DELROUTE).c_str());
  return -1;
}
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
#if 0
    case RTM_NEWNEIGH: monitor_NEWNEIGH(n); break;
    case RTM_DELNEIGH: monitor_DELNEIGH(n); break;
#endif
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

