#ifndef _COMMAND_NODE_H
#define _COMMAND_NODE_H

#include "vector.h"
#include "vty.h"
#include "graph.h"
#include "memory.h"
#include "hash.h"
#include "command_graph.h"
#include "command_node.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * List of CLI nodes.
 * Please remember to update the
 * name array in command.c.
 */
enum node_type {
  AUTH_NODE,              /* Authentication mode of vty interface. */
  VIEW_NODE,              /* View node. Default mode of vty interface. */
  AUTH_ENABLE_NODE,       /* Authentication mode for change enable. */
  ENABLE_NODE,            /* Enable node. */
  CONFIG_NODE,            /* Config node. Default mode of config file. */
  DEBUG_NODE,             /* Debug node. */
  VRF_DEBUG_NODE,         /* Vrf Debug node. */
  NORTHBOUND_DEBUG_NODE,  /* Northbound Debug node. */
  DEBUG_VNC_NODE,         /* Debug VNC node. */
  RMAP_DEBUG_NODE,        /* Route-map debug node */
  AAA_NODE,               /* AAA node. */
  KEYCHAIN_NODE,          /* Key-chain node. */
  KEYCHAIN_KEY_NODE,      /* Key-chain key node. */
  LOGICALROUTER_NODE,     /* Logical-Router node. */
  IP_NODE,                /* Static ip route node. */
  VRF_NODE,               /* VRF mode node. */
  INTERFACE_NODE,         /* Interface mode node. */
  NH_GROUP_NODE,          /* Nexthop-Group mode node. */
  ZEBRA_NODE,             /* zebra connection node. */
  TABLE_NODE,             /* rtm_table selection node. */
  RIP_NODE,               /* RIP protocol mode node. */
  RIPNG_NODE,             /* RIPng protocol mode node. */
  BABEL_NODE,             /* BABEL protocol mode node. */
  EIGRP_NODE,             /* EIGRP protocol mode node. */
  BGP_NODE,               /* BGP protocol mode which includes BGP4+ */
  BGP_VPNV4_NODE,         /* BGP MPLS-VPN PE exchange. */
  BGP_VPNV6_NODE,         /* BGP MPLS-VPN PE exchange. */
  BGP_IPV4_NODE,          /* BGP IPv4 unicast address family.  */
  BGP_IPV4M_NODE,         /* BGP IPv4 multicast address family.  */
  BGP_IPV4L_NODE,         /* BGP IPv4 labeled unicast address family.  */
  BGP_IPV6_NODE,          /* BGP IPv6 address family */
  BGP_IPV6M_NODE,         /* BGP IPv6 multicast address family. */
  BGP_IPV6L_NODE,         /* BGP IPv6 labeled unicast address family. */
  BGP_VRF_POLICY_NODE,    /* BGP VRF policy */
  BGP_VNC_DEFAULTS_NODE,  /* BGP VNC nve defaults */
  BGP_VNC_NVE_GROUP_NODE, /* BGP VNC nve group */
  BGP_VNC_L2_GROUP_NODE,  /* BGP VNC L2 group */
  RFP_DEFAULTS_NODE,      /* RFP defaults node */
  BGP_EVPN_NODE,          /* BGP EVPN node. */
  OSPF_NODE,              /* OSPF protocol mode */
  OSPF6_NODE,             /* OSPF protocol for IPv6 mode */
  LDP_NODE,               /* LDP protocol mode */
  LDP_IPV4_NODE,          /* LDP IPv4 address family */
  LDP_IPV6_NODE,          /* LDP IPv6 address family */
  LDP_IPV4_IFACE_NODE,    /* LDP IPv4 Interface */
  LDP_IPV6_IFACE_NODE,    /* LDP IPv6 Interface */
  LDP_L2VPN_NODE,         /* LDP L2VPN node */
  LDP_PSEUDOWIRE_NODE,    /* LDP Pseudowire node */
  ISIS_NODE,              /* ISIS protocol mode */
  ACCESS_NODE,            /* Access list node. */
  PREFIX_NODE,            /* Prefix list node. */
  ACCESS_IPV6_NODE,       /* Access list node. */
  ACCESS_MAC_NODE,        /* MAC access list node*/
  PREFIX_IPV6_NODE,       /* Prefix list node. */
  AS_LIST_NODE,           /* AS list node. */
  COMMUNITY_LIST_NODE,    /* Community list node. */
  RMAP_NODE,              /* Route map node. */
  PBRMAP_NODE,            /* PBR map node. */
  SMUX_NODE,              /* SNMP configuration node. */
  DUMP_NODE,              /* Packet dump node. */
  FORWARDING_NODE,        /* IP forwarding node. */
  PROTOCOL_NODE,          /* protocol filtering node */
  MPLS_NODE,              /* MPLS config node */
  PW_NODE,                /* Pseudowire config node */
  VTY_NODE,               /* Vty node. */
  LINK_PARAMS_NODE,       /* Link-parameters node */
  BGP_EVPN_VNI_NODE,      /* BGP EVPN VNI */
  RPKI_NODE,              /* RPKI node for configuration of RPKI cache server connections.*/
  BGP_FLOWSPECV4_NODE,    /* BGP IPv4 FLOWSPEC Address-Family */
  BGP_FLOWSPECV6_NODE,    /* BGP IPv6 FLOWSPEC Address-Family */
  BFD_NODE,               /* BFD protocol mode. */
  BFD_PEER_NODE,          /* BFD peer configuration mode. */
  OPENFABRIC_NODE,        /* OpenFabric router configuration node */
  VRRP_NODE,              /* VRRP node */
  NODE_TYPE_MAX,          /* maximum */
};

extern const char *node_names[];

/*
 * Node which has some commands and
 * prompt string and configuration
 * function pointer .
 */
struct cmd_node {
  enum node_type node;       /* Node index. */
  const char *prompt;        /* Prompt character at vty interface. */
  int vtysh;                 /* Is this node's configuration goes to vtysh ? */
  int (*func)(struct vty *); /* Node's configuration write function */
  struct graph *cmdgraph;    /* Node's command graph */
  vector cmd_vector;         /* Vector of this node's command list. */
  struct hash *cmd_hash;     /* Hashed index of command node list, for de-dupping primarily */
};

#ifdef __cplusplus
}
#endif
#endif /* _COMMAND_NODE_H */
