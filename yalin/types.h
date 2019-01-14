#ifndef _NETLINK_TYPES_H_
#define _NETLINK_TYPES_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>
#include "flags.h"
#include "hexdump.h"


inline static const char*
ifi_type_to_str(uint16_t type)
{
  switch (type) {
    case ARPHRD_NETROM: return "NETROM";
    case ARPHRD_ETHER: return "ETHER";
    case ARPHRD_EETHER: return "EETHER";
    case ARPHRD_AX25: return "AX25";
    case ARPHRD_PRONET: return "PRONET";
    case ARPHRD_CHAOS: return "CHAOS";
    case ARPHRD_IEEE802: return "IEEE802";
    case ARPHRD_ARCNET: return "ARCNET";
    case ARPHRD_APPLETLK: return "APPLETLK";
    case ARPHRD_DLCI: return "DLCI";
    case ARPHRD_ATM: return "ATM";
    case ARPHRD_METRICOM: return "METRICOM";
    case ARPHRD_IEEE1394: return "IEEE1394";
    case ARPHRD_EUI64: return "EUI64";
    case ARPHRD_INFINIBAND: return "INFINIBAND";
    case ARPHRD_SLIP: return "SLIP";
    case ARPHRD_CSLIP: return "CSLIP";
    case ARPHRD_SLIP6: return "SLIP6";
    case ARPHRD_CSLIP6: return "CSLIP6";
    case ARPHRD_RSRVD: return "RSRVD";
    case ARPHRD_ADAPT: return "ADAPT";
    case ARPHRD_ROSE: return "ROSE";
    case ARPHRD_X25: return "X25";
    case ARPHRD_HWX25: return "HWX25";
    case ARPHRD_PPP: return "PPP";
    case ARPHRD_CISCO: return "CISCO";
    case ARPHRD_LAPB: return "LAPB";
    case ARPHRD_DDCMP: return "DDCMP";
    case ARPHRD_RAWHDLC: return "RAWHDLC";
    case ARPHRD_TUNNEL: return "TUNNEL";
    case ARPHRD_TUNNEL6: return "TUNNEL6";
    case ARPHRD_FRAD: return "FRAD";
    case ARPHRD_SKIP: return "SKIP";
    case ARPHRD_LOOPBACK: return "LOOPBACK";
    case ARPHRD_LOCALTLK: return "LOCALTLK";
    case ARPHRD_FDDI: return "FDDI";
    case ARPHRD_BIF: return "BIF";
    case ARPHRD_SIT: return "SIT";
    case ARPHRD_IPDDP: return "IPDDP";
    case ARPHRD_IPGRE: return "IPGRE";
    case ARPHRD_PIMREG: return "PIMREG";
    case ARPHRD_HIPPI: return "HIPPI";
    case ARPHRD_ASH: return "ASH";
    case ARPHRD_ECONET: return "ECONET";
    case ARPHRD_IRDA: return "IRDA";
    case ARPHRD_FCPP: return "FCPP";
    case ARPHRD_FCAL: return "FCAL";
    case ARPHRD_FCPL: return "FCPL";
    case ARPHRD_FCFABRIC: return "FCFABRIC";
    case ARPHRD_IEEE802_TR: return "IEEE802_TR";
    case ARPHRD_IEEE80211: return "IEEE80211";
    case ARPHRD_IEEE80211_PRISM: return "IEEE80211_PRISM";
    case ARPHRD_IEEE80211_RADIOTAP: return "IEEE80211_RADIOTAP";
    case ARPHRD_IEEE802154: return "IEEE802154";
    case ARPHRD_VOID: return "VOID";
    case ARPHRD_NONE: return "NONE";
    default: return "unknown";
  }
}

inline static const char* nlmsg_type_to_str(uint16_t type)
{
  switch (type) {
    case RTM_NEWLINK: return "RTM_NEWLINK";
    case RTM_DELLINK: return "RTM_DELLINK";
    case RTM_GETLINK: return "RTM_GETLINK";
    case RTM_NEWADDR: return "RTM_NEWADDR";
    case RTM_DELADDR: return "RTM_DELADDR";
    case RTM_GETADDR: return "RTM_GETADDR";
    case RTM_NEWROUTE: return "RTM_NEWROUTE";
    case RTM_DELROUTE: return "RTM_DELROUTE";
    case RTM_GETROUTE: return "RTM_GETROUTE";
    case RTM_NEWNEIGH: return "RTM_NEWNEIGH";
    case RTM_DELNEIGH: return "RTM_DELNEIGH";
    case RTM_GETNEIGH: return "RTM_GETNEIGH";
    case RTM_NEWRULE: return "RTM_NEWRULE";
    case RTM_DELRULE: return "RTM_DELRULE";
    case RTM_GETRULE: return "RTM_GETRULE";
    case RTM_NEWQDISC: return "RTM_NEWQDISC";
    case RTM_DELQDISC: return "RTM_DELQDISC";
    case RTM_GETQDISC: return "RTM_GETQDISC";
    case RTM_NEWTCLASS: return "RTM_NEWTCLASS";
    case RTM_DELTCLASS: return "RTM_DELTCLASS";
    case RTM_GETTCLASS: return "RTM_GETTCLASS";
    case RTM_NEWTFILTER: return "RTM_NEWTFILTER";
    case RTM_DELTFILTER: return "RTM_DELTFILTER";
    case RTM_GETTFILTER: return "RTM_GETTFILTER";
    case RTM_NEWACTION     : return "RTM_NEWACTION";
    case RTM_DELACTION     : return "RTM_DELACTION";
    case RTM_GETACTION     : return "RTM_GETACTION";
    case RTM_NEWPREFIX     : return "RTM_NEWPREFIX";
    case RTM_GETMULTICAST  : return "RTM_GETMULTICAST";
    case RTM_GETANYCAST    : return "RTM_GETANYCAST";
    case RTM_NEWNEIGHTBL   : return "RTM_NEWNEIGHTBL";
    case RTM_GETNEIGHTBL   : return "RTM_GETNEIGHTBL";
    case RTM_SETNEIGHTBL   : return "RTM_SETNEIGHTBL";
    case RTM_NEWNDUSEROPT  : return "RTM_NEWNDUSEROPT";
    case RTM_NEWADDRLABEL  : return "RTM_NEWADDRLABEL";
    case RTM_DELADDRLABEL  : return "RTM_DELADDRLABEL";
    case RTM_GETADDRLABEL  : return "RTM_GETADDRLABEL";
    case RTM_GETDCB        : return "RTM_GETDCB";
    case RTM_SETDCB        : return "RTM_SETDCB";
    case RTM_NEWNETCONF    : return "RTM_NEWNETCONF";
    case RTM_DELNETCONF    : return "RTM_DELNETCONF";
    case RTM_GETNETCONF    : return "RTM_GETNETCONF";
    case RTM_NEWMDB        : return "RTM_NEWMDB";
    case RTM_DELMDB        : return "RTM_DELMDB";
    case RTM_GETMDB        : return "RTM_GETMDB";
    case RTM_NEWNSID       : return "RTM_NEWNSID";
    case RTM_DELNSID       : return "RTM_DELNSID";
    case RTM_GETNSID       : return "RTM_GETNSID";
    case RTM_NEWSTATS      : return "RTM_NEWSTATS";
    case RTM_GETSTATS      : return "RTM_GETSTATS";
    case RTM_NEWCACHEREPORT: return "RTM_NEWCACHEREPORT";

    default:
      fprintf(stderr, "%s: unknown(%u)\n", __func__, type);
      return "UNKOWN";
  }
}

inline static const char*
rtn_type_to_str(uint8_t type)
{
  switch (type) {
    case RTN_UNSPEC: return "RTN_UNSPEC";
    case RTN_UNICAST: return "RTN_UNICAST";
    case RTN_LOCAL: return "RTN_LOCAL";
    case RTN_BROADCAST: return "RTN_BROADCAST";
    case RTN_ANYCAST: return "RTN_ANYCAST";
    case RTN_MULTICAST: return "RTN_MULTICAST";
    case RTN_BLACKHOLE: return "RTN_BLACKHOLE";
    case RTN_UNREACHABLE: return "RTN_UNREACHABLE";
    case RTN_PROHIBIT: return "RTN_PROHIBIT";
    case RTN_THROW: return "RTN_THROW";
    case RTN_NAT: return "RTN_NAT";
    case RTN_XRESOLVE: return "RTN_XRESOLVE";
    default:
      fprintf(stderr, "%s: unknown(%u)\n", __func__, type);
      exit(1);
  }
}

inline static const char*
ifa_family_to_str(uint8_t family)
{
  switch (family) {
    case AF_UNSPEC    : return "AF_UNSPEC";
    case AF_UNIX      : return "AF_UNIX";
    case AF_INET      : return "AF_INET";
    case AF_AX25      : return "AF_AX25";
    case AF_IPX       : return ";AF_IPX";
    case AF_APPLETALK : return "AF_APPLETALK";
    case AF_NETROM    : return "AF_NETROM";
    case AF_BRIDGE    : return "AF_BRIDGE";
    case AF_ATMPVC    : return "AF_ATMPVC";
    case AF_X25       : return "AF_X25";
    case AF_INET6     : return "AF_INET6";
    case AF_ROSE      : return "AF_ROSE";
    case AF_DECnet    : return "AF_DECnet";
    case AF_NETBEUI   : return "AF_NETBEUI";
    case AF_SECURITY  : return "AF_SECURITY";
    case AF_KEY       : return "AF_KEY";
    case AF_NETLINK   : return "AF_NETLINK";
    case AF_PACKET    : return "AF_PACKET";
    case AF_ASH       : return "AF_ASH";
    case AF_ECONET    : return "AF_ECONET";
    case AF_ATMSVC    : return "AF_ATMSVC";
    case AF_RDS       : return "AF_RDS";
    case AF_SNA       : return "AF_SNA";
    case AF_IRDA      : return "AF_IRDA";
    case AF_PPPOX     : return "AF_PPPOX";
    case AF_WANPIPE   : return "AF_WANPIPE";
    case AF_LLC       : return "AF_LLC";
    case AF_IB        : return "AF_IB";
    case AF_MPLS      : return "AF_MPLS";
    case AF_CAN       : return "AF_CAN";
    case AF_TIPC      : return "AF_TIPC";
    case AF_BLUETOOTH : return "AF_BLUETOOTH";
    case AF_IUCV      : return "AF_IUCV";
    case AF_RXRPC     : return "AF_RXRPC";
    case AF_ISDN      : return "AF_ISDN";
    case AF_PHONET    : return "AF_PHONET";
    case AF_IEEE802154: return "AF_IEEE802154";
    case AF_CAIF      : return "AF_CAIF";
    case AF_ALG       : return "AF_ALG";
    case AF_NFC       : return "AF_NFC";
    case AF_VSOCK     : return "AF_VSOCK";
    case AF_MAX       : return "AF_MAX";
    default:
      fprintf(stderr, "%s: unknown\n", __func__);
      exit(1);
  }
}

inline static const char*
rta_type_to_str(uint16_t type)
{
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
    default:
      fprintf(stderr, "%s: unknown(%u)\n", __func__, type);
      return "unknown";
      break;
  }
}


#endif /* _NETLINK_TYPES_H_ */
