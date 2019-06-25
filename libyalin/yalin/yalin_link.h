#ifndef _LINK_H_
#define _LINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include <string>
#include <algorithm>
#include <arpa/inet.h>
#include "netlink_helper.h"
#include "netlink_cache.h"

// extern netlink_cache_t* nlc;

namespace routerd {

struct link {
 public:
  const struct ifinfomsg* ifi;
  size_t len;
  rta_array* rtas;

 private:
  std::string to_iproute2_cli_adddel_link(uint16_t nlmsg_type) const
  {
    std::string ifname;
    if (rtas->get(IFLA_IFNAME)) {
      const struct rtattr* rta = rtas->get(IFLA_IFNAME);
      ifname = rta_readstr(rta);
    }

    assert(~ifi->ifi_change == 0);
    std::string str;
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
    return str;
  }
  std::string to_iproute2_cli_set_link(uint16_t nlmsg_type,
      netlink_cache_t *nlc) const
  {
    assert(nlmsg_type == RTM_NEWLINK);
    std::string ifname;
    if (rtas->get(IFLA_IFNAME)) {
      const struct rtattr* rta = rtas->get(IFLA_IFNAME);
      ifname = rta_readstr(rta);
    }

    std::string str;
    const struct ifinfomsg* _cache = netlink_cache_get_link(nlc, ifi->ifi_index);
    const size_t _cache_len = netlink_cachelen_get_link(nlc, ifi->ifi_index);
    if (_cache) {
      rta_array cache(IFLA_RTA(_cache), _cache_len);
      std::string lname = ifindex2str(ifi->ifi_index);

      auto old_mtu = rta_read32(cache.get(IFLA_MTU));
      auto new_mtu = rta_read32(rtas->get(IFLA_MTU));
      if (old_mtu != new_mtu) {
        str += strfmt("ip link set %s mtu %u", lname.c_str(), new_mtu);
      }
      uint8_t* old_addr = (uint8_t*)rta_readptr(cache.get(IFLA_ADDRESS));
      uint8_t* new_addr = (uint8_t*)rta_readptr(rtas->get(IFLA_ADDRESS));
      if (memcmp(old_addr, new_addr, 6) != 0) {
        str += strfmt("ip link set %s address "
                "%02x:%02x:%02x:%02x:%02x:%02x", lname.c_str(),
                new_addr[0], new_addr[1], new_addr[2],
                new_addr[3], new_addr[4], new_addr[5]);
      }
      netlink_cache_update_link(nlc, ifi, len);
    }
    return str;
  }
  std::string to_iproute2_cli_set_link_flag(uint16_t nlmsg_type) const
  {
    assert(nlmsg_type == RTM_NEWLINK);
    std::string ifname;
    if (rtas->get(IFLA_IFNAME)) {
      const struct rtattr* rta = rtas->get(IFLA_IFNAME);
      ifname = rta_readstr(rta);
    }

    uint32_t flags = ifi->ifi_flags;
    uint32_t change = ifi->ifi_change;
    std::string str = strfmt("ip link set dev %s ", ifname.c_str());
    if (change) {
      uint32_t affected_flag = ~uint32_t(0) & change;
      uint32_t destination_bit = flags & change;
      if (affected_flag & IFF_UP) str += destination_bit&affected_flag?"up":"down";
      if (affected_flag & IFF_PROMISC)
        str += destination_bit&affected_flag?"promisc on":"promisc off";
    } else {
      str += "UNSUPPORTED...?";
    }
    return str;
  }

 public:

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
  std::string to_iproute2_cli(uint16_t nlmsg_type, netlink_cache_t *nlc) const
  {
    if (~ifi->ifi_change == 0) return to_iproute2_cli_adddel_link(nlmsg_type);
    else if (ifi->ifi_change == 0) return to_iproute2_cli_set_link(nlmsg_type, nlc);
    else return to_iproute2_cli_set_link_flag(nlmsg_type);
  }
}; /* struct link */

} /* namespace routerd */

#endif /* _LINK_H_ */
