#ifndef _YALIN_CORE_NEXTHOP_H_
#define _YALIN_CORE_NEXTHOP_H_

#include <assert.h>
#include <linux/nexthop.h>
#include <linux/lwtunnel.h>
#include <linux/seg6.h>
#include <linux/seg6_local.h>
#include <linux/seg6_iptunnel.h>
#include "../netlink_helper.h"
#include "route.h"

#define NHM_RTA(r) ((struct rtattr*)((char*)(r) + sizeof(struct nhmsg)))

inline static std::string
nhmsg_summary(const struct nhmsg *nhm)
{
  return strfmt("fmly=%u scope=%u pro=%u flags=0x%x",
      nhm->nh_family, nhm->nh_scope,
      nhm->nh_protocol, nhm->nh_flags);
}

inline static const char*
nexthop_grp_type_to_str(uint16_t type)
{
  /* defined at /usr/include/linux/nexthop.h */
  switch (type) {
    case NEXTHOP_GRP_TYPE_MPATH: return "NEXTHOP_GRP_TYPE_MPATH";
    default: return "NHA_GRP_TYPE_XXXUNKNOWNXXX";
  }
}

inline static const char*
rta_type_NEXTHOP_to_str(uint16_t type)
{
  /* defined at /usr/include/linux/nexthop.h */
  switch (type) {
    case NHA_UNSPEC       : return "NHA_UNSPEC";
    case NHA_ID           : return "NHA_ID";
    case NHA_GROUP        : return "NHA_GROUP";
    case NHA_GROUP_TYPE   : return "NHA_GROUP_TYPE";
    case NHA_BLACKHOLE    : return "NHA_BLACKHOLE";
    case NHA_OIF          : return "NHA_OIF";
    case NHA_GATEWAY      : return "NHA_GATEWAY";
    case NHA_ENCAP_TYPE   : return "NHA_ENCAP_TYPE";
    case NHA_ENCAP        : return "NHA_ENCAP";
    case NHA_GROUPS       : return "NHA_GROUPS";
    case NHA_MASTER       : return "NHA_MASTER";
    default: return "NHA_XXXUNKNOWNXXX";
  }
}

inline static std::string
nhmsg_rtattr_summary(const struct rtattr *rta)
{
  std::string hdr = strfmt("0x%04x %-16s :: ",
      rta->rta_type, rta_type_NEXTHOP_to_str(rta->rta_type));
  switch (rta->rta_type) {
    case NHA_ID:
    case NHA_OIF:
    {
      assert(rta->rta_len == 8);
      uint32_t num = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", num);
    }

    case NHA_ENCAP_TYPE:
    {
      assert(rta->rta_len == 6);
      uint16_t val = *(uint16_t*)(rta+1);
      return hdr + strfmt("%u (%s)", val,
          lwtunnel_encap_types_to_str(val));
    }

    case NHA_GROUP:
    {
      std::string str = "array-data\n";
      assert(((rta->rta_len - 4) % sizeof(struct nexthop_grp)) == 0);
      size_t num_grp = (rta->rta_len-4) / sizeof(struct nexthop_grp);
      struct nexthop_grp *array = (struct nexthop_grp*)(rta + 1);
      for (size_t i=0; i<num_grp; i++) {
        struct nexthop_grp *grp = &array[i];
        std::string attr_str = strfmt("id=%u weight=%u", grp->id, grp->weight);
        if (attr_str != "")
          str += "    " + attr_str + "\n";
      }
      str.pop_back();
      return hdr + str;
    }

    case NHA_GROUP_TYPE:
    {
      assert(rta->rta_len == 6);
      uint16_t val = *(uint16_t*)(rta+1);
      return hdr + strfmt("%u (%s)", val,
          nexthop_grp_type_to_str(val));
    }

    case NHA_ENCAP:
    {
      uint16_t encap_kind;
      {
        const uint8_t* ptr = (const uint8_t*)rta;
        ptr += rta->rta_len;
        const struct rtattr* rtan = (const struct rtattr*)ptr;
        assert(rtan->rta_type == NHA_ENCAP_TYPE);
        assert(rtan->rta_len == 6);
        uint16_t val = *(uint16_t*)(rtan+1);
        encap_kind = val;
      }

      std::string str = "nested-data\n";
      size_t rta_len = rta->rta_len - sizeof(*rta);
      const uint8_t* data = (const uint8_t*)(rta + 1);
      for (const struct rtattr* rta = (const struct rtattr*)data;
           RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
        std::string attr_str;
        if (encap_kind==LWTUNNEL_ENCAP_SEG6_LOCAL)
          attr_str = rtmsg_rtattr_SEG6_LOCAL_summary(rta);
        else if (encap_kind==LWTUNNEL_ENCAP_SEG6)
          attr_str = rtmsg_rtattr_SEG6_summary(rta);

        if (attr_str != "")
          str += "    " + attr_str + "\n";
      }
      str.pop_back();
      return hdr + str;
    }

    case NHA_GATEWAY:
    {
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      if (addr_len == 4) return hdr + inetpton(addr_ptr, AF_INET);
      if (addr_len == 16) return hdr + inetpton(addr_ptr, AF_INET6);
      else return hdr + "unknown-addr-fmt";
    }

    case NHA_BLACKHOLE:
    case NHA_GROUPS:
    case NHA_MASTER:
    case NHA_UNSPEC:
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
  return hdr + strfmt("Unknown%p", rta);
}

inline static std::string
rtnl_nexthop_summary(const struct nlmsghdr* hdr)
{
  struct nhmsg *nhm = (struct nhmsg*)(hdr + 1);
  std::string str = nhmsg_summary(nhm) + "\n";
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = NHM_RTA(nhm);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {

    std::string attr_str = nhmsg_rtattr_summary(rta);
    if (attr_str != "")
      str += "  " + attr_str + "\n";
  }
  return str;
}

#endif /* _YALIN_CORE_NEXTHOP_H_ */
