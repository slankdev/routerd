#ifndef _YALIN_LINK_H_
#define _YALIN_LINK_H_

#include <assert.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/neighbour.h>
#include <linux/if_link.h>
#include <linux/if.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <string>
#include <vector>

#ifndef IFLA_RTA
#error IFLA_RTA isnt defined
#endif

inline static const char*
rta_type_IFLA_INFO_to_str(uint16_t type)
{
  switch (type) {
    case IFLA_INFO_UNSPEC    : return "IFLA_INFO_UNSPEC";
    case IFLA_INFO_KIND      : return "IFLA_INFO_KIND";
    case IFLA_INFO_DATA      : return "IFLA_INFO_DATA";
    case IFLA_INFO_XSTATS    : return "IFLA_INFO_XSTATS";
    case IFLA_INFO_SLAVE_KIND: return "IFLA_INFO_SLAVE_KIND";
    case IFLA_INFO_SLAVE_DATA: return "IFLA_INFO_SLAVE_DATA";
    default: return "IFLA_INFO_XXXXXXXXUNKNOWN";
  }
}

inline static const char*
rta_type_VLAN_to_str(uint16_t type)
{
  switch (type) {
    case IFLA_VLAN_UNSPEC     : return "IFLA_VLAN_UNSPEC";
    case IFLA_VLAN_ID         : return "IFLA_VLAN_ID";
    case IFLA_VLAN_FLAGS      : return "IFLA_VLAN_FLAGS";
    case IFLA_VLAN_EGRESS_QOS : return "IFLA_VLAN_EGRESS_QOS";
    case IFLA_VLAN_INGRESS_QOS: return "IFLA_VLAN_INGRESS_QOS";
    case IFLA_VLAN_PROTOCOL   : return "IFLA_VLAN_PROTOCOL";
    default: return "IFLA_VLAN_XXXXXXXXUNKNOWN";
  }
}

inline static const char*
rta_type_VRF_to_str(uint16_t type)
{
  switch (type) {
    case IFLA_VRF_UNSPEC: return "IFLA_VRF_UNSPEC";
    case IFLA_VRF_TABLE : return "IFLA_VRF_TABLE";
    default: return "IFLA_VRF_XXXXXXXXUNKNOWN";
  }
}

inline static const char*
rta_type_BR_to_str(uint16_t type)
{
  switch (type) {
    case IFLA_BR_UNSPEC                    : return "IFLA_BR_UNSPEC";
    case IFLA_BR_FORWARD_DELAY             : return "IFLA_BR_FORWARD_DELAY";
    case IFLA_BR_HELLO_TIME                : return "IFLA_BR_HELLO_TIME";
    case IFLA_BR_MAX_AGE                   : return "IFLA_BR_MAX_AGE";
    case IFLA_BR_AGEING_TIME               : return "IFLA_BR_AGEING_TIME";
    case IFLA_BR_STP_STATE                 : return "IFLA_BR_STP_STATE";
    case IFLA_BR_PRIORITY                  : return "IFLA_BR_PRIORITY";
    case IFLA_BR_VLAN_FILTERING            : return "IFLA_BR_VLAN_FILTERING";
    case IFLA_BR_VLAN_PROTOCOL             : return "IFLA_BR_VLAN_PROTOCOL";
    case IFLA_BR_GROUP_FWD_MASK            : return "IFLA_BR_GROUP_FWD_MASK";
    case IFLA_BR_ROOT_ID                   : return "IFLA_BR_ROOT_ID";
    case IFLA_BR_BRIDGE_ID                 : return "IFLA_BR_BRIDGE_ID";
    case IFLA_BR_ROOT_PORT                 : return "IFLA_BR_ROOT_PORT";
    case IFLA_BR_ROOT_PATH_COST            : return "IFLA_BR_ROOT_PATH_COST";
    case IFLA_BR_TOPOLOGY_CHANGE           : return "IFLA_BR_TOPOLOGY_CHANGE";
    case IFLA_BR_TOPOLOGY_CHANGE_DETECTED  : return "IFLA_BR_TOPOLOGY_CHANGE_DETECTED";
    case IFLA_BR_HELLO_TIMER               : return "IFLA_BR_HELLO_TIMER";
    case IFLA_BR_TCN_TIMER                 : return "IFLA_BR_TCN_TIMER";
    case IFLA_BR_TOPOLOGY_CHANGE_TIMER     : return "IFLA_BR_TOPOLOGY_CHANGE_TIMER";
    case IFLA_BR_GC_TIMER                  : return "IFLA_BR_GC_TIMER";
    case IFLA_BR_GROUP_ADDR                : return "IFLA_BR_GROUP_ADDR";
    case IFLA_BR_FDB_FLUSH                 : return "IFLA_BR_FDB_FLUSH";
    case IFLA_BR_MCAST_ROUTER              : return "IFLA_BR_MCAST_ROUTER";
    case IFLA_BR_MCAST_SNOOPING            : return "IFLA_BR_MCAST_SNOOPING";
    case IFLA_BR_MCAST_QUERY_USE_IFADDR    : return "IFLA_BR_MCAST_QUERY_USE_IFADDR";
    case IFLA_BR_MCAST_QUERIER             : return "IFLA_BR_MCAST_QUERIER";
    case IFLA_BR_MCAST_HASH_ELASTICITY     : return "IFLA_BR_MCAST_HASH_ELASTICITY";
    case IFLA_BR_MCAST_HASH_MAX            : return "IFLA_BR_MCAST_HASH_MAX";
    case IFLA_BR_MCAST_LAST_MEMBER_CNT     : return "IFLA_BR_MCAST_LAST_MEMBER_CNT";
    case IFLA_BR_MCAST_STARTUP_QUERY_CNT   : return "IFLA_BR_MCAST_STARTUP_QUERY_CNT";
    case IFLA_BR_MCAST_LAST_MEMBER_INTVL   : return "IFLA_BR_MCAST_LAST_MEMBER_INTVL";
    case IFLA_BR_MCAST_MEMBERSHIP_INTVL    : return "IFLA_BR_MCAST_MEMBERSHIP_INTVL";
    case IFLA_BR_MCAST_QUERIER_INTVL       : return "IFLA_BR_MCAST_QUERIER_INTVL";
    case IFLA_BR_MCAST_QUERY_INTVL         : return "IFLA_BR_MCAST_QUERY_INTVL";
    case IFLA_BR_MCAST_QUERY_RESPONSE_INTVL: return "IFLA_BR_MCAST_QUERY_RESPONSE_INTVL";
    case IFLA_BR_MCAST_STARTUP_QUERY_INTVL : return "IFLA_BR_MCAST_STARTUP_QUERY_INTVL";
    case IFLA_BR_NF_CALL_IPTABLES          : return "IFLA_BR_NF_CALL_IPTABLES";
    case IFLA_BR_NF_CALL_IP6TABLES         : return "IFLA_BR_NF_CALL_IP6TABLES";
    case IFLA_BR_NF_CALL_ARPTABLES         : return "IFLA_BR_NF_CALL_ARPTABLES";
    case IFLA_BR_VLAN_DEFAULT_PVID         : return "IFLA_BR_VLAN_DEFAULT_PVID";
    case IFLA_BR_PAD                       : return "IFLA_BR_PAD";
    case IFLA_BR_VLAN_STATS_ENABLED        : return "IFLA_BR_VLAN_STATS_ENABLED";
    case IFLA_BR_MCAST_STATS_ENABLED       : return "IFLA_BR_MCAST_STATS_ENABLED";
    case IFLA_BR_MCAST_IGMP_VERSION        : return "IFLA_BR_MCAST_IGMP_VERSION";
    case IFLA_BR_MCAST_MLD_VERSION         : return "IFLA_BR_MCAST_MLD_VERSION";
    default: return "IFLA_BR_XXXXXXXXUNKNOWN";
  }
}

inline static const char*
rta_type_VXLAN_to_str(uint16_t type)
{
  switch (type) {
    case IFLA_VXLAN_UNSPEC: return "IFLA_VXLAN_UNSPEC";
    case IFLA_VXLAN_ID: return "IFLA_VXLAN_ID";
    case IFLA_VXLAN_GROUP: return "IFLA_VXLAN_GROUP";
    case IFLA_VXLAN_LINK: return "IFLA_VXLAN_LINK";
    case IFLA_VXLAN_LOCAL: return "IFLA_VXLAN_LOCAL";
    case IFLA_VXLAN_TTL: return "IFLA_VXLAN_TTL";
    case IFLA_VXLAN_TOS: return "IFLA_VXLAN_TOS";
    case IFLA_VXLAN_LEARNING: return "IFLA_VXLAN_LEARNING";
    case IFLA_VXLAN_AGEING: return "IFLA_VXLAN_AGEING";
    case IFLA_VXLAN_LIMIT: return "IFLA_VXLAN_LIMIT";
    case IFLA_VXLAN_PORT_RANGE: return "IFLA_VXLAN_PORT_RANGE";
    case IFLA_VXLAN_PROXY: return "IFLA_VXLAN_PROXY";
    case IFLA_VXLAN_RSC: return "IFLA_VXLAN_RSC";
    case IFLA_VXLAN_L2MISS: return "IFLA_VXLAN_L2MISS";
    case IFLA_VXLAN_L3MISS: return "IFLA_VXLAN_L3MISS";
    case IFLA_VXLAN_PORT: return "IFLA_VXLAN_PORT";
    case IFLA_VXLAN_GROUP6: return "IFLA_VXLAN_GROUP6";
    case IFLA_VXLAN_LOCAL6: return "IFLA_VXLAN_LOCAL6";
    case IFLA_VXLAN_UDP_CSUM: return "IFLA_VXLAN_UDP_CSUM";
    case IFLA_VXLAN_UDP_ZERO_CSUM6_TX: return "IFLA_VXLAN_UDP_ZERO_CSUM6_TX";
    case IFLA_VXLAN_UDP_ZERO_CSUM6_RX: return "IFLA_VXLAN_UDP_ZERO_CSUM6_RX";
    case IFLA_VXLAN_REMCSUM_TX: return "IFLA_VXLAN_REMCSUM_TX";
    case IFLA_VXLAN_REMCSUM_RX: return "IFLA_VXLAN_REMCSUM_RX";
    case IFLA_VXLAN_GBP: return "IFLA_VXLAN_GBP";
    case IFLA_VXLAN_REMCSUM_NOPARTIAL: return "IFLA_VXLAN_REMCSUM_NOPARTIAL";
    case IFLA_VXLAN_COLLECT_METADATA: return "IFLA_VXLAN_COLLECT_METADATA";
    case IFLA_VXLAN_LABEL: return "IFLA_VXLAN_LABEL";
    case IFLA_VXLAN_GPE: return "IFLA_VXLAN_GPE";
    case IFLA_VXLAN_FAN_MAP: return "IFLA_VXLAN_FAN_MAP";
    default: return "IFLA_BR_XXXXXXXXUNKNOWN";
  }
}

inline static const char*
rta_type_LINK_to_str(uint16_t type)
{
  /* defined at /usr/include/linux/if_link.h */
  switch (type) {
    case IFLA_UNSPEC         : return "IFLA_UNSPEC";
    case IFLA_ADDRESS        : return "IFLA_ADDRESS";
    case IFLA_BROADCAST      : return "IFLA_BROADCAST";
    case IFLA_IFNAME         : return "IFLA_IFNAME";
    case IFLA_MTU            : return "IFLA_MTU";
    case IFLA_LINK           : return "IFLA_LINK ";
    case IFLA_QDISC          : return "IFLA_QDISC";
    case IFLA_STATS          : return "IFLA_STATS";
    case IFLA_COST           : return "IFLA_COST";
    case IFLA_PRIORITY       : return "IFLA_PRIORITY";
    case IFLA_MASTER         : return "IFLA_MASTER";
    case IFLA_WIRELESS       : return "IFLA_WIRELESS";
    case IFLA_PROTINFO       : return "IFLA_PROTINFO";
    case IFLA_TXQLEN         : return "IFLA_TXQLEN";
    case IFLA_MAP            : return "IFLA_MAP";
    case IFLA_WEIGHT         : return "IFLA_WEIGHT";
    case IFLA_OPERSTATE      : return "IFLA_OPERSTATE";
    case IFLA_LINKMODE       : return "IFLA_LINKMODE";
    case IFLA_LINKINFO       : return "IFLA_LINKINFO";
    case IFLA_NET_NS_PID     : return "IFLA_NET_NS_PID";
    case IFLA_IFALIAS        : return "IFLA_IFALIAS";
    case IFLA_NUM_VF         : return "IFLA_NUM_VF";
    case IFLA_VFINFO_LIST    : return "IFLA_VFINFO_LIST";
    case IFLA_STATS64        : return "IFLA_STATS64";
    case IFLA_VF_PORTS       : return "IFLA_VF_PORTS";
    case IFLA_PORT_SELF      : return "IFLA_PORT_SELF";
    case IFLA_AF_SPEC        : return "IFLA_AF_SPEC";
    case IFLA_GROUP          : return "IFLA_GROUP";
    case IFLA_NET_NS_FD      : return "IFLA_NET_NS_FD";
    case IFLA_EXT_MASK       : return "IFLA_EXT_MASK";
    case IFLA_PROMISCUITY    : return "IFLA_PROMISCUITY";
    case IFLA_NUM_TX_QUEUES  : return "IFLA_NUM_TX_QUEUES";
    case IFLA_NUM_RX_QUEUES  : return "IFLA_NUM_RX_QUEUES";
    case IFLA_CARRIER        : return "IFLA_CARRIER";
    case IFLA_PHYS_PORT_ID   : return "IFLA_PHYS_PORT_ID";
    case IFLA_CARRIER_CHANGES: return "IFLA_CARRIER_CHANGES";
    case IFLA_PHYS_SWITCH_ID : return "IFLA_PHYS_SWITCH_ID";
    case IFLA_LINK_NETNSID   : return "IFLA_LINK_NETNSID";
    case IFLA_PHYS_PORT_NAME : return "IFLA_PHYS_PORT_NAME";
    case IFLA_PROTO_DOWN     : return "IFLA_PROTO_DOWN";
    case IFLA_GSO_MAX_SEGS      : return "IFLA_GSO_MAX_SEGS";
    case IFLA_GSO_MAX_SIZE      : return "IFLA_GSO_MAX_SIZE";
    case IFLA_PAD               : return "IFLA_PAD";
    case IFLA_XDP               : return "IFLA_XDP";
    case IFLA_EVENT             : return "IFLA_EVENT";
    case IFLA_NEW_NETNSID       : return "IFLA_NEW_NETNSID";
    case IFLA_IF_NETNSID        : return "IFLA_IF_NETNSID";
    case IFLA_CARRIER_UP_COUNT  : return "IFLA_CARRIER_UP_COUNT";
    case IFLA_CARRIER_DOWN_COUNT: return "IFLA_CARRIER_DOWN_COUNT";
    case IFLA_NEW_IFINDEX       : return "IFLA_NEW_IFINDEX";
    default: return "IFLA_XXXXUNKNOWNXXX";
  }
}
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
  std::string tmp_kind = "";

  switch (rta->rta_type) {
    case IFLA_INFO_KIND:
    case IFLA_INFO_SLAVE_KIND:
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
    case IFLA_MASTER:
    case IFLA_MTU:
    case IFLA_LINK:
    {
      assert(rta->rta_len == 8);
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
    case IFLA_OPERSTATE:
    case IFLA_LINKMODE:
    case IFLA_CARRIER:
    case IFLA_PROTO_DOWN:
    {
      assert(rta->rta_len == 5);
      uint8_t num = *(uint8_t*)(rta+1);
      return hdr + strfmt("%u", num);
    }
    case IFLA_CARRIER_CHANGES:
    case IFLA_TXQLEN:
    case IFLA_GROUP:
    case IFLA_NUM_TX_QUEUES:
    case IFLA_GSO_MAX_SEGS:
    case IFLA_GSO_MAX_SIZE:
    case IFLA_CARRIER_UP_COUNT:
    case IFLA_CARRIER_DOWN_COUNT:
    case IFLA_NUM_RX_QUEUES:
    {
      if (rta->rta_len != 8)
        printf("type=%u rta_len=%u\n",
            rta->rta_type, rta->rta_len);
      assert(rta->rta_len == 8);
      uint32_t num = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", num);
    }

    //  0x0006 IFLA_QDISC               :: unknown-fmt(rta_len=9,data=6e6f6f70...)
    //  0x000e IFLA_MAP                 :: unknown-fmt(rta_len=36,data=00000000...)
    //  0x0017 IFLA_STATS64             :: unknown-fmt(rta_len=196,data=00000000...)
    //  0x0007 IFLA_STATS               :: unknown-fmt(rta_len=100,data=00000000...)
    case IFLA_QDISC:
    case IFLA_MAP:
    case IFLA_STATS64:
    case IFLA_STATS:
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
ifm_flags2str(uint32_t flag)
{
  std::vector<std::string> tmp;
  if (flag & IFF_UP         ) tmp.push_back("UP");
  if (flag & IFF_BROADCAST  ) tmp.push_back("BROADCAST");
  if (flag & IFF_DEBUG      ) tmp.push_back("DEBUG");
  if (flag & IFF_LOOPBACK   ) tmp.push_back("LOOPBACK");
  if (flag & IFF_POINTOPOINT) tmp.push_back("POINTOPOINT");
  if (flag & IFF_RUNNING    ) tmp.push_back("RUNNING");
  if (flag & IFF_NOARP      ) tmp.push_back("NOARP");
  if (flag & IFF_PROMISC    ) tmp.push_back("PROMISC");
  if (flag & IFF_NOTRAILERS ) tmp.push_back("NOTRAILERS");
  if (flag & IFF_ALLMULTI   ) tmp.push_back("ALLMULTI");
  if (flag & IFF_MASTER     ) tmp.push_back("MASTER");
  if (flag & IFF_SLAVE      ) tmp.push_back("SLAVE");
  if (flag & IFF_MULTICAST  ) tmp.push_back("MULTICAST");
  if (flag & IFF_PORTSEL    ) tmp.push_back("PORTSEL");
  if (flag & IFF_AUTOMEDIA  ) tmp.push_back("AUTOMEDIA");
  if (flag & IFF_DYNAMIC    ) tmp.push_back("DYNAMIC");
  if (flag & IFF_LOWER_UP   ) tmp.push_back("LOWER_UP");
  if (flag & IFF_DORMANT    ) tmp.push_back("DORMANT");
  if (flag & IFF_ECHO       ) tmp.push_back("ECHO");
  std::string ret;
  for (size_t i=0; i<tmp.size(); i++)
    ret += tmp[i] + ((i+1)<tmp.size()?"|":"");
  return ret;
}

inline static std::string
ifinfomsg_summary(const struct ifinfomsg* ifm)
{
  uint16_t affected_flag = ~uint16_t(0) & ifm->ifi_change;
  return strfmt("family=%u type=%u ifindex=%u flags=0x%08x<%s> change=0x%08x<%s>",
      ifm->ifi_family, ifm->ifi_type, ifm->ifi_index, ifm->ifi_flags,
      ifm_flags2str(ifm->ifi_flags).c_str(), ifm->ifi_change,
      ifm_flags2str(affected_flag).c_str());
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
