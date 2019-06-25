#ifndef _YALIN_NETCONF_H_
#define _YALIN_NETCONF_H_

#include <string>
#include <linux/netconf.h>
// #define NC_RTA(nc) (struct rtattr*)(nc+1)
#define NC_RTA(r) ((struct rtattr*)(((char*)(r))+NLMSG_ALIGN(sizeof(struct netconfmsg))))
#define NC_PAYLOAD(n) NLMSG_PAYLOAD(n,sizeof(struct netconfmsg))
// #define NDM_RTA(r) ((struct rtattr*)(((char*)(r))+NLMSG_ALIGN(sizeof(struct ndmsg))))

// inline static const char*
// nc_family_to_str(uint16_t family)
// {
// }

inline static std::string
netconfmsg_summary(const struct netconfmsg* nc)
{ return strfmt("family=%u", nc->ncm_family); }

inline static const char*
rta_type_NETCONF_to_str(uint16_t type)
{
  switch (type) {
    /* /usr/include/netconf.h */
    case NETCONFA_UNSPEC: return "NETCONFA_UNSPEC";
    case NETCONFA_IFINDEX: return "NETCONFA_IFINDEX";
    case NETCONFA_FORWARDING: return "NETCONFA_FORWARDING";
    case NETCONFA_RP_FILTER: return "NETCONFA_RP_FILTER";
    case NETCONFA_MC_FORWARDING: return "NETCONFA_MC_FORWARDING";
    case NETCONFA_PROXY_NEIGH: return "NETCONFA_PROXY_NEIGH";
    case NETCONFA_IGNORE_ROUTES_WITH_LINKDOWN: return "NETCONFA_IGNORE_ROUTES_WITH_LINKDOWN";
    case NETCONFA_INPUT: return "NETCONFA_INPUT";
    default: return "UNKNOWN";
  }
}

inline static std::string
netconfmsg_rtattr_summary(const struct rtattr* rta)
{
  char _hdr[256];
  snprintf(_hdr, sizeof(_hdr), "0x%04x %-24s :: ",
      rta->rta_type, rta_type_NETCONF_to_str(rta->rta_type));
  std::string hdr = _hdr;

  switch (rta->rta_type) {

    case NETCONFA_IFINDEX:
    case NETCONFA_FORWARDING:
    case NETCONFA_RP_FILTER:
    case NETCONFA_MC_FORWARDING:
    case NETCONFA_PROXY_NEIGH:
    case NETCONFA_IGNORE_ROUTES_WITH_LINKDOWN:
    case NETCONFA_INPUT:
    {
      assert(rta->rta_len == 8);
      char str[256];
      uint32_t num = *(uint32_t*)(rta+1);
      snprintf(str, sizeof(str), "%u", num);
      return hdr + str;
    }
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
rtnl_netconf_summary(const struct nlmsghdr* hdr)
{
  struct netconfmsg* nc = (struct netconfmsg*)(hdr + 1);
  std::string str = netconfmsg_summary(nc) + "\n";
  size_t rta_len = NC_PAYLOAD(hdr);
  for (struct rtattr* rta = NC_RTA(nc);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    std::string attr_str = netconfmsg_rtattr_summary(rta);
    if (attr_str != "")
      str += "  " + attr_str + "\n";
  }
  return str;
}

#endif /* _YALIN_NETCONF_H_ */
