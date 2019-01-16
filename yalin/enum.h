
#ifndef __YALIN_ENUM_H_
#define __YALIN_ENUM_H_

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/neighbour.h>
#include <linux/if_link.h>

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

inline static const char*
rta_type_NEIGH_to_str(uint16_t type)
{
  switch (type) {
    /**/
    case NDA_UNSPEC       : return "NDA_UNSPEC";
    case NDA_DST          : return "NDA_DST";
    case NDA_LLADDR       : return "NDA_LLADDR";
    case NDA_CACHEINFO    : return "NDA_CACHEINFO";
    case NDA_PROBES       : return "NDA_PROBES";
    case NDA_VLAN         : return "NDA_VLAN";
    case NDA_PORT         : return "NDA_PORT";
    case NDA_VNI          : return "NDA_VNI";
    case NDA_IFINDEX      : return "NDA_IFINDEX";
    case NDA_MASTER       : return "NDA_MASTER";
    case NDA_LINK_NETNSID : return "NDA_LINK_NETNSID";
    case NDA_SRC_VNI      : return "NDA_SRC_VNI";
    default: return "NDA_UNKNOWN";
  }
}

#endif /* __YALIN_ENUM_H_ */

