#ifndef _YALIN_ADDR_H_
#define _YALIN_ADDR_H_

#include <assert.h>

#ifndef IFA_RTA
#error IFA_RTA isnt defined
#endif

inline static const char*
rta_type_ADDR_to_str(uint16_t type)
{
  /* defined at /usr/include/linux/if_addr.h */
  switch (type) {
    case IFA_UNSPEC   : return "IFA_UNSPEC";
    case IFA_ADDRESS  : return "IFA_ADDRESS";
    case IFA_LOCAL    : return "IFA_LOCAL";
    case IFA_LABEL    : return "IFA_LABEL";
    case IFA_BROADCAST: return "IFA_BROADCAST";
    case IFA_ANYCAST  : return "IFA_ANYCAST";
    case IFA_CACHEINFO: return "IFA_CACHEINFO";
    case IFA_MULTICAST: return "IFA_MULTICAST";
    case IFA_FLAGS    : return "IFA_FLAGS";
    default: return "IFA_UNKWNOWN";
  }
}

inline static std::string
ifaddrmsg_rtattr_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-16s :: ",
      rta->rta_type, rta_type_ADDR_to_str(rta->rta_type));
  switch (rta->rta_type) {

    case IFA_ADDRESS:
    case IFA_LOCAL:
    {
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      if (addr_len == 4) return hdr + inetpton(addr_ptr, AF_INET);
      if (addr_len == 16) return hdr + inetpton(addr_ptr, AF_INET6);
      else return hdr + "unknown-addr-fmt";
    }
    case IFA_LABEL:
    {
      const char* val = (const char*)(rta+1);
      return hdr + val;
    }
    case IFA_FLAGS:
    {
      uint32_t val = *(uint32_t*)(rta+1);
      return hdr + strfmt("0x%x", val);
    }
    case IFA_BROADCAST:
    case IFA_ANYCAST:
    case IFA_CACHEINFO:
    case IFA_MULTICAST:
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
ifaddrmsg_summary(const struct ifaddrmsg* ifa)
{
  return strfmt("family=%u pref=%u ifindex=%u flags=0x%x"
      " scope=%u", ifa->ifa_family, ifa->ifa_prefixlen,
      ifa->ifa_index, ifa->ifa_flags, ifa->ifa_scope);
}

inline static std::string
rtnl_addr_summary(const struct nlmsghdr* hdr)
{
  struct ifaddrmsg* ifa = (struct ifaddrmsg*)(hdr + 1);
  std::string str = ifaddrmsg_summary(ifa) + "\n";
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = IFA_RTA(ifa);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    std::string attr_str = ifaddrmsg_rtattr_summary(rta);
    if (attr_str != "")
      str += "  " + attr_str + "\n";
  }
  return str;
}

#endif /* _YALIN_ADDR_H_ */
