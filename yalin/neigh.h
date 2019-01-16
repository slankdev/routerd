#ifndef _YALIN_NEIGH_H_
#define _YALIN_NEIGH_H_

inline static std::string
ndmsg_rtattr_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-16s :: ",
      rta->rta_type, rta_type_NEIGH_to_str(rta->rta_type));
  switch (rta->rta_type) {

    case NDA_LLADDR:
    {
      assert(rta->rta_len == 10);
      uint8_t* lladdr = (uint8_t*)(rta+1);
      std::string val;
      val = strfmt("%02x:%02x:%02x:%02x:%02x:%02x",
          lladdr[0], lladdr[1], lladdr[2],
          lladdr[3], lladdr[4], lladdr[5]);
      return hdr + val;
    }
    case NDA_DST:
    {
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      if (addr_len == 4) return hdr + inetpton(addr_ptr, AF_INET);
      if (addr_len == 16) return hdr + inetpton(addr_ptr, AF_INET6);
      else return hdr + "unknown-addr-fmt";
    }
    case NDA_PROBES:
    {
      assert(rta->rta_len == 8);
      uint32_t val = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", val);
    }

    case NDA_CACHEINFO:
    case NDA_VLAN:
    case NDA_PORT:
    case NDA_VNI:
    case NDA_IFINDEX:
    case NDA_MASTER:
    case NDA_LINK_NETNSID:
    case NDA_SRC_VNI:
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
ndmsg_summary(const struct ndmsg* ndm)
{
  return strfmt("fmly=%u index=%u state=%u flags=0x%x type=%u",
      ndm->ndm_family, ndm->ndm_ifindex, ndm->ndm_state,
      ndm->ndm_flags, ndm->ndm_type);
}

inline static std::string
rtnl_neigh_summary(const struct nlmsghdr* hdr)
{
  struct ndmsg* ndm = (struct ndmsg*)(hdr + 1);
  std::string str = ndmsg_summary(ndm) + "\n";
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = NDM_RTA(ndm);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    std::string attr_str = ndmsg_rtattr_summary(rta);
    if (attr_str != "")
      str += "  " + attr_str + "\n";
  }
  return str;
}

#endif /* _YALIN_NEIGH_H_ */
