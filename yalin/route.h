#ifndef _YALIN_ROUTE_H_
#define _YALIN_ROUTE_H_

#include <assert.h>

#ifndef RTM_RTA
#error RTM_RTA isnt defined
#endif

inline static const char*
rta_type_ROUTE_to_str(uint16_t type)
{
  /* defined at /usr/include/linux/rtnetlink.h */
  switch (type) {
    case RTA_UNSPEC       : return "RTA_UNSPEC";
    case RTA_DST          : return "RTA_DST";
    case RTA_SRC          : return "RTA_SRC";
    case RTA_IIF          : return "RTA_IIF";
    case RTA_OIF          : return "RTA_OIF";
    case RTA_GATEWAY      : return "RTA_GATEWAY";
    case RTA_PRIORITY     : return "RTA_PRIORITY";
    case RTA_PREFSRC      : return "RTA_PREFSRC";
    case RTA_METRICS      : return "RTA_METRICS";
    case RTA_MULTIPATH    : return "RTA_MULTIPATH";
    case RTA_PROTOINFO    : return "RTA_PROTOINFO";
    case RTA_FLOW         : return "RTA_FLOW";
    case RTA_CACHEINFO    : return "RTA_CACHEINFO";
    case RTA_SESSION      : return "RTA_SESSION";
    case RTA_MP_ALGO      : return "RTA_MP_ALGO";
    case RTA_TABLE        : return "RTA_TABLE";
    case RTA_MARK         : return "RTA_MARK";
    case RTA_MFC_STATS    : return "RTA_MFC_STATS";
    case RTA_VIA          : return "RTA_VIA";
    case RTA_NEWDST       : return "RTA_NEWDST";
    case RTA_PREF         : return "RTA_PREF";
    case RTA_ENCAP_TYPE   : return "RTA_ENCAP_TYPE";
    case RTA_ENCAP        : return "RTA_ENCAP";
    case RTA_EXPIRES      : return "RTA_EXPIRES";
    case RTA_PAD          : return "RTA_PAD";
    case RTA_UID          : return "RTA_UID";
    case RTA_TTL_PROPAGATE: return "RTA_TTL_PROPAGATE";
    default: return "RTA_XXXUNKNOWNXXX";
  }
}

inline static std::string
rtmsg_rtattr_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-16s :: ",
      rta->rta_type, rta_type_ROUTE_to_str(rta->rta_type));
  switch (rta->rta_type) {

    case RTA_PRIORITY:
    case RTA_PREFSRC:
    case RTA_TABLE:
    case RTA_IIF:
    case RTA_OIF:
    {
      assert(rta->rta_len == 8);
      uint32_t val = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", val);
    }
    case RTA_PREF:
    {
      assert(rta->rta_len == 5);
      uint32_t val = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", val);
    }

    case RTA_DST:
    case RTA_SRC:
    case RTA_GATEWAY:
    {
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      if (addr_len == 4) return hdr + inetpton(addr_ptr, AF_INET);
      if (addr_len == 16) return hdr + inetpton(addr_ptr, AF_INET6);
      else return hdr + "unknown-addr-fmt";
    }

    // Others
    case RTA_METRICS:
    case RTA_MULTIPATH:
    case RTA_PROTOINFO:
    case RTA_FLOW:
    case RTA_SESSION:
    case RTA_MP_ALGO:
    case RTA_MARK:
    case RTA_MFC_STATS:
    case RTA_VIA:
    case RTA_NEWDST:
    case RTA_ENCAP_TYPE:
    case RTA_ENCAP:
    case RTA_EXPIRES:
    case RTA_PAD:
    case RTA_UID:
    case RTA_TTL_PROPAGATE:
    case RTA_CACHEINFO:
    default:
    {
      std::string val;
      val = strfmt("unknown-fmt(rta_len=%u,data=", rta->rta_len);
      const uint8_t* data = (const uint8_t*)(rta+1);
      size_t payload_len = size_t(rta->rta_len-sizeof(*rta));
      size_t n = std::min(size_t(4), payload_len);
      for (size_t i=0; i<n; i++)
        val += strfmt("%02x", data[i]);
      if (4 < payload_len) val += "...";
      val += ")";
      return hdr + val;
    }
  }
}

inline static std::string
rtmsg_summary(const struct rtmsg* rtm)
{
  return strfmt("fmly=%u dl=%u sl=%u tos=%u tab=%u"
      " pro=%u scope=%u type=%u f=0x%x",
      rtm->rtm_family, rtm->rtm_dst_len, rtm->rtm_src_len,
      rtm->rtm_tos, rtm->rtm_table, rtm->rtm_protocol,
      rtm->rtm_scope, rtm->rtm_type, rtm->rtm_flags);
}

inline static std::string
rtnl_route_summary(const struct nlmsghdr* hdr)
{
  struct rtmsg* rtm = (struct rtmsg*)(hdr + 1);
  std::string str = rtmsg_summary(rtm) + "\n";
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = RTM_RTA(rtm);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    std::string attr_str = rtmsg_rtattr_summary(rta);
    if (attr_str != "")
      str += "  " + attr_str + "\n";
  }
  return str;
}

#endif /* _YALIN_ROUTE_H_ */
