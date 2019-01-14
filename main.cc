
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

inline static std::string
strfmt(const char* fmt, ...)
{
  char str[1000];
  va_list args;
  va_start(args, fmt);
  vsprintf(str, fmt, args);
  va_end(args);
  return str;
}

inline static std::string
inetpton(const void* ptr, int afi)
{
  char buf[256];
  inet_ntop(afi, ptr, buf, sizeof(buf));
  return buf;
}

inline static std::string
nlmsghdr_summary(const struct nlmsghdr* hdr)
{
  char buf[256];
  const char* type = nlmsg_type_to_str(hdr->nlmsg_type);
  snprintf(buf, sizeof(buf), "%-12s f=0x%04x s=%010u p=%010u",
      type?type:"unknown", hdr->nlmsg_flags, hdr->nlmsg_seq, hdr->nlmsg_pid);
  return buf;
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
ifaddrmsg_summary(const struct ifaddrmsg* ifa)
{
  return strfmt("family=%u pref=%u ifindex=%u flags=0x%x"
      " scope=%u", ifa->ifa_family, ifa->ifa_prefixlen,
      ifa->ifa_index, ifa->ifa_flags, ifa->ifa_scope);
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

inline static const char*
rta_type_LINKINFO_to_str(uint16_t type)
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
      rta->rta_type, rta_type_LINKINFO_to_str(rta->rta_type));

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
    case IFLA_AF_SPEC:
    {
      char str[256];
      rta_to_str_IFLA_AF_SPEC(rta, str, sizeof(str));
      return hdr + str;
    }
    // default: return hdr + "unknown-rta";
    default: return "";
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
my_rtnl_link_summary(const struct nlmsghdr* hdr)
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

inline static std::string
my_rtnl_addr_summary(const struct nlmsghdr* hdr)
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

inline static std::string
my_rtnl_route_summary(const struct nlmsghdr* hdr)
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

inline static std::string
my_rtnl_neigh_summary(const struct nlmsghdr* hdr)
{ return __func__; }

static int
dump_msg(const struct sockaddr_nl *who,
         struct rtnl_ctrl_data* _dum_,
         struct nlmsghdr *n, void *arg)
{
  std::string str = nlmsghdr_summary(n) + " :: ";
  switch (n->nlmsg_type) {

    // case RTM_NEWLINK:
    // case RTM_DELLINK:
    // case RTM_GETLINK:
    //   str += my_rtnl_link_summary(n);
    //   break;
    //
    // case RTM_NEWADDR:
    // case RTM_DELADDR:
    // case RTM_GETADDR:
    //   str += my_rtnl_addr_summary(n);
    //   break;

    case RTM_NEWROUTE:
    case RTM_DELROUTE:
    case RTM_GETROUTE:
      str += my_rtnl_route_summary(n);
      break;

    // case RTM_NEWNEIGH:
    // case RTM_DELNEIGH:
    // case RTM_GETNEIGH:
    //   str += my_rtnl_neigh_summary(n);
    //   break;

    /* Invalid Case */
    default:
      return 0;
      // fprintf(stderr, "%s: unknown type(%u)\n", __func__, n->nlmsg_type);
      // fprintf(stderr, "please check with "
      //     "\'grep %u /usr/include/linux/rtnetlink.h\'\n", n->nlmsg_type);
      // hexdump(stderr, n, n->nlmsg_len);
      // exit(1);
      break;
  }
  printf("%s\n", str.c_str());
  return 0;
}

int
main(int argc, char **argv)
{
  uint32_t groups = ~0U;
  netlink_t* nl = netlink_open(groups, NETLINK_ROUTE);
  if (nl == NULL)
    return 1;

  int ret = netlink_listen(nl, dump_msg, NULL);
  if (ret < 0)
    return 1;

  netlink_close(nl);
}

