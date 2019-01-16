#ifndef _YALIN_LINK_H_
#define _YALIN_LINK_H_

inline static std::string
ifinfomsg_rtattr_VXLAN_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-30s :: ",
      rta->rta_type, rta_type_VXLAN_to_str(rta->rta_type));
  switch (rta->rta_type) {
    case IFLA_VXLAN_GROUP:
    case IFLA_VXLAN_LOCAL:
    {
      uint8_t* val = (uint8_t*)(rta+1);
      return hdr + strfmt("%u.%u.%u.%u",
          val[0], val[1], val[2], val[3]);
    }
    case IFLA_VXLAN_ID:
    case IFLA_VXLAN_LINK:
    case IFLA_VXLAN_LABEL:
    case IFLA_VXLAN_AGEING:
    case IFLA_VXLAN_LIMIT:
    case IFLA_VXLAN_PORT_RANGE:
    {
      uint32_t val = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", val);
    }
    case IFLA_VXLAN_TTL:
    case IFLA_VXLAN_TOS:
    case IFLA_VXLAN_LEARNING:
    case IFLA_VXLAN_PROXY:
    case IFLA_VXLAN_RSC:
    case IFLA_VXLAN_L2MISS:
    case IFLA_VXLAN_L3MISS:
    case IFLA_VXLAN_COLLECT_METADATA:
    case IFLA_VXLAN_PORT:
    case IFLA_VXLAN_UDP_CSUM:
    case IFLA_VXLAN_UDP_ZERO_CSUM6_TX:
    case IFLA_VXLAN_UDP_ZERO_CSUM6_RX:
    case IFLA_VXLAN_REMCSUM_TX:
    case IFLA_VXLAN_REMCSUM_RX:
    {
      uint8_t val = *(uint8_t*)(rta+1);
      return hdr + strfmt("%lu", val);
    }

    case IFLA_VXLAN_GROUP6:
    case IFLA_VXLAN_LOCAL6:
    case IFLA_VXLAN_GBP:
    case IFLA_VXLAN_REMCSUM_NOPARTIAL:
    case IFLA_VXLAN_GPE:
    case IFLA_VXLAN_FAN_MAP:
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
ifinfomsg_rtattr_BR_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-34s :: ",
      rta->rta_type, rta_type_BR_to_str(rta->rta_type));
  switch (rta->rta_type) {
    case IFLA_BR_FORWARD_DELAY:
    case IFLA_BR_HELLO_TIME:
    case IFLA_BR_MAX_AGE:
    case IFLA_BR_AGEING_TIME:
    case IFLA_BR_STP_STATE:
    case IFLA_BR_ROOT_PATH_COST:
    case IFLA_BR_MCAST_HASH_ELASTICITY:
    case IFLA_BR_MCAST_HASH_MAX:
    case IFLA_BR_MCAST_LAST_MEMBER_CNT:
    case IFLA_BR_MCAST_STARTUP_QUERY_CNT:
    {
      uint32_t val = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", val);
    }
    case IFLA_BR_HELLO_TIMER:
    case IFLA_BR_TCN_TIMER:
    case IFLA_BR_TOPOLOGY_CHANGE_TIMER:
    case IFLA_BR_GC_TIMER:
    case IFLA_BR_ROOT_ID:
    case IFLA_BR_BRIDGE_ID:
    case IFLA_BR_MCAST_LAST_MEMBER_INTVL:
    case IFLA_BR_MCAST_MEMBERSHIP_INTVL:
    case IFLA_BR_MCAST_QUERIER_INTVL:
    case IFLA_BR_MCAST_QUERY_INTVL:
    case IFLA_BR_MCAST_QUERY_RESPONSE_INTVL:
    case IFLA_BR_MCAST_STARTUP_QUERY_INTVL:
    {
      uint64_t val = *(uint64_t*)(rta+1);
      return hdr + strfmt("%lu", val);
    }
    case IFLA_BR_PRIORITY:
    case IFLA_BR_GROUP_FWD_MASK:
    case IFLA_BR_ROOT_PORT:
    case IFLA_BR_VLAN_PROTOCOL:
    case IFLA_BR_VLAN_DEFAULT_PVID:
    {
      uint16_t val = *(uint16_t*)(rta+1);
      return hdr + strfmt("%lu", val);
    }
    case IFLA_BR_VLAN_FILTERING:
    case IFLA_BR_TOPOLOGY_CHANGE:
    case IFLA_BR_TOPOLOGY_CHANGE_DETECTED:
    case IFLA_BR_VLAN_STATS_ENABLED:
    case IFLA_BR_MCAST_ROUTER:
    case IFLA_BR_MCAST_SNOOPING:
    case IFLA_BR_MCAST_QUERY_USE_IFADDR:
    case IFLA_BR_MCAST_QUERIER:
    case IFLA_BR_MCAST_STATS_ENABLED:
    case IFLA_BR_MCAST_IGMP_VERSION:
    case IFLA_BR_MCAST_MLD_VERSION:
    case IFLA_BR_NF_CALL_IPTABLES:
    case IFLA_BR_NF_CALL_IP6TABLES:
    case IFLA_BR_NF_CALL_ARPTABLES:
    {
      uint8_t val = *(uint8_t*)(rta+1);
      return hdr + strfmt("%lu", val);
    }

    /*
     * 0x14 IFLA_BR_GROUP_ADDR               :: rta_len=10
     */
    case IFLA_BR_FDB_FLUSH:
    case IFLA_BR_PAD:
    case IFLA_BR_GROUP_ADDR:
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
ifinfomsg_rtattr_VRF_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-24s :: ",
      rta->rta_type, rta_type_VRF_to_str(rta->rta_type));
  switch (rta->rta_type) {
    case IFLA_VRF_TABLE:
    {
      uint32_t val = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", val);
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

std::string tmp_kind;
inline static std::string
ifinfomsg_rtattr_VLAN_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-24s :: ",
      rta->rta_type, rta_type_VLAN_to_str(rta->rta_type));
  switch (rta->rta_type) {
    case IFLA_VLAN_ID:
    case IFLA_VLAN_PROTOCOL:
    {
      uint16_t val = *(uint16_t*)(rta+1);
      return hdr + strfmt("%u", val);
    }
    case IFLA_VLAN_FLAGS:
    {
      uint64_t val = *(uint64_t*)(rta+1);
      return hdr + strfmt("0x%lx", val);
    }
    case IFLA_VLAN_EGRESS_QOS:
    case IFLA_VLAN_INGRESS_QOS:
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
ifinfomsg_rtattr_LINKINFO_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-24s :: ",
      rta->rta_type, rta_type_IFLA_INFO_to_str(rta->rta_type));

  switch (rta->rta_type) {
    case IFLA_INFO_KIND:
    {
      const char* val = (const char*)(rta+1);
      tmp_kind = val;
      return hdr + val;
    }
    case IFLA_INFO_DATA:
    {
      std::string str = "nested-data\n";
      size_t rta_len = rta->rta_len - sizeof(*rta);
      const uint8_t* data = (const uint8_t*)(rta + 1);
      for (const struct rtattr* rta = (const struct rtattr*)data;
           RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
        std::string astr;
        if (tmp_kind == "vlan") astr = ifinfomsg_rtattr_VLAN_summary(rta);
        else if (tmp_kind == "vrf") astr = ifinfomsg_rtattr_VRF_summary(rta);
        else if (tmp_kind == "bridge") astr = ifinfomsg_rtattr_BR_summary(rta);
        else if (tmp_kind == "vxlan") astr = ifinfomsg_rtattr_VXLAN_summary(rta);

        if (astr != "")
          str += "      " + astr + "\n";
      }
      str.pop_back();
      return hdr + str;
    }
    case IFLA_INFO_XSTATS    :
    case IFLA_INFO_SLAVE_KIND:
    case IFLA_INFO_SLAVE_DATA:
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
ifinfomsg_rtattr_summary(const struct rtattr* rta)
{
  char _hdr[256];
  snprintf(_hdr, sizeof(_hdr), "0x%04x %-24s :: ",
      rta->rta_type, rta_type_LINK_to_str(rta->rta_type));
  std::string hdr = _hdr;

  switch (rta->rta_type) {
    case IFLA_IFNAME:
    {
      const char* val = (const char*)(rta+1);
      return hdr + val;
    }
    case IFLA_ADDRESS:
    case IFLA_BROADCAST:
    {
      char substr[1000] = {0};
      size_t data_len = rta->rta_len-sizeof(struct rtattr);
      uint8_t* data_ptr = (uint8_t*)(rta + 1);
      for (size_t i=0; i<data_len; i++) {
        char subsubstr[100];
        snprintf(subsubstr, sizeof(subsubstr), "%02x%s",
            data_ptr[i], i+1<data_len?":":"");
        strncat(substr, subsubstr, strlen(subsubstr));
      }
      return hdr + substr;
    }
    case IFLA_PROMISCUITY:
    {
      uint32_t promisc = *(uint32_t*)(rta+1);
      return hdr + (promisc==1?"on":"off");
    }
    case IFLA_MTU:
    case IFLA_LINK:
    {
      char str[256];
      uint32_t num = *(uint32_t*)(rta+1);
      snprintf(str, sizeof(str), "%u", num);
      return hdr + str;
    }
    case IFLA_LINKINFO:
    {
      std::string str = "nested-data\n";
      size_t rta_len = rta->rta_len - sizeof(*rta);
      const uint8_t* data = (const uint8_t*)(rta + 1);
      for (const struct rtattr* rta = (const struct rtattr*)data;
           RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
        std::string attr_str = ifinfomsg_rtattr_LINKINFO_summary(rta);
        if (attr_str != "")
          str += "    " + attr_str + "\n";
      }
      str.pop_back();
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
ifinfomsg_summary(const struct ifinfomsg* ifm)
{
  char buf[256];
  snprintf(buf, sizeof(buf), "family=%u type=%u ifindex=%u "
      "flags=0x%x change=0x%x", ifm->ifi_family, ifm->ifi_type,
      ifm->ifi_index, ifm->ifi_flags, ifm->ifi_change);
  return buf;
}

inline static std::string
rtnl_link_summary(const struct nlmsghdr* hdr)
{
  struct ifinfomsg* ifm = (struct ifinfomsg*)(hdr + 1);
  std::string str = ifinfomsg_summary(ifm) + "\n";
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = IFLA_RTA(ifm);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    std::string attr_str = ifinfomsg_rtattr_summary(rta);
    if (attr_str != "")
      str += "  " + attr_str + "\n";
  }
  return str;
}

#endif /* _YALIN_LINK_H_ */
