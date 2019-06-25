#ifndef _ROUTE_H_
#define _ROUTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <algorithm>
#include <arpa/inet.h>
#include "netlink_helper.h"
#include "core/route.h"
#include "core/link.h"

namespace routerd {

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

} /* namespace routerd */

#endif /* _ROUTE_H_ */
