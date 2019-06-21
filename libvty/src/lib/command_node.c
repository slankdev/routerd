
#include "command_node.h"

extern vector cmdvec;

const char *node_names[] = {
  [AUTH_NODE            ] = "auth"              , // AUTH_NODE
  [VIEW_NODE            ] = "view"              , // VIEW_NODE
  [AUTH_ENABLE_NODE     ] = "auth enable"       , // AUTH_ENABLE_NODE
  [ENABLE_NODE          ] = "enable"            , // ENABLE_NODE
  [CONFIG_NODE          ] = "config+"            , // CONFIG_NODE
  [DEBUG_NODE           ] = "debug"             , // DEBUG_NODE
  [NORTHBOUND_DEBUG_NODE] = "northbound debug"  , // NORTHBOUND_DEBUG_NODE
  [DEBUG_VNC_NODE       ] = "vnc debug"         , // DEBUG_VNC_NODE
  [RMAP_DEBUG_NODE      ] = "route-map debug"   , // RMAP_DEBUG_NODE
  [AAA_NODE             ] = "aaa"               , // AAA_NODE
  [KEYCHAIN_NODE        ] = "keychain"          , // KEYCHAIN_NODE
  [KEYCHAIN_KEY_NODE    ] = "keychain key"      , // KEYCHAIN_KEY_NODE
  [LOGICALROUTER_NODE   ] = "logical-router"    , // LOGICALROUTER_NODE
  [IP_NODE              ] = "static ip"         , // IP_NODE
  [INTERFACE_NODE       ] = "interface"         , // INTERFACE_NODE
  [NH_GROUP_NODE        ] = "nexthop-group"     , // NH_GROUP_NODE
  [ZEBRA_NODE           ] = "zebra"             , // ZEBRA_NODE
  [TABLE_NODE           ] = "table"             , // TABLE_NODE
  [RIP_NODE             ] = "rip"               , // RIP_NODE
  [RIPNG_NODE           ] = "ripng"             , // RIPNG_NODE
  [BABEL_NODE           ] = "babel"             , // BABEL_NODE
  [EIGRP_NODE           ] = "eigrp"             , // EIGRP_NODE
  [RFP_DEFAULTS_NODE    ] = "rfp defaults"      , // RFP_DEFAULTS_NODE
  [BGP_EVPN_NODE        ] = "bgp evpn"          , // BGP_EVPN_NODE
  [OSPF_NODE            ] = "ospf"              , // OSPF_NODE
  [OSPF6_NODE           ] = "ospf6"             , // OSPF6_NODE
  [LDP_NODE             ] = "ldp"               , // LDP_NODE
  [LDP_IPV4_NODE        ] = "ldp ipv4"          , // LDP_IPV4_NODE
  [LDP_IPV6_NODE        ] = "ldp ipv6"          , // LDP_IPV6_NODE
  [LDP_IPV4_IFACE_NODE  ] = "ldp ipv4 interface", // LDP_IPV4_IFACE_NODE
  [LDP_IPV6_IFACE_NODE  ] = "ldp ipv6 interface", // LDP_IPV6_IFACE_NODE
  [LDP_L2VPN_NODE       ] = "ldp l2vpn"         , // LDP_L2VPN_NODE
  [LDP_PSEUDOWIRE_NODE  ] = "ldp"               , // LDP_PSEUDOWIRE_NODE
  [ISIS_NODE            ] = "isis"              , // ISIS_NODE
  [ACCESS_NODE          ] = "ipv4 access list"  , // ACCESS_NODE
  [PREFIX_NODE          ] = "ipv4 prefix list"  , // PREFIX_NODE
  [ACCESS_IPV6_NODE     ] = "ipv6 access list"  , // ACCESS_IPV6_NODE
  [ACCESS_MAC_NODE      ] = "MAC access list"   , // ACCESS_MAC_NODE
  [PREFIX_IPV6_NODE     ] = "ipv6 prefix list"  , // PREFIX_IPV6_NODE
  [AS_LIST_NODE         ] = "as list"           , // AS_LIST_NODE
  [COMMUNITY_LIST_NODE  ] = "community list"    , // COMMUNITY_LIST_NODE
  [RMAP_NODE            ] = "routemap"          , // RMAP_NODE
  [PBRMAP_NODE          ] = "pbr-map"           , // PBRMAP_NODE
  [SMUX_NODE            ] = "smux"              , // SMUX_NODE
  [DUMP_NODE            ] = "dump"              , // DUMP_NODE
  [FORWARDING_NODE      ] = "forwarding"        , // FORWARDING_NODE
  [PROTOCOL_NODE        ] = "protocol"          , // PROTOCOL_NODE
  [MPLS_NODE            ] = "mpls"              , // MPLS_NODE
  [PW_NODE              ] = "pw"                , // PW_NODE
  [VTY_NODE             ] = "vty"               , // VTY_NODE
  [LINK_PARAMS_NODE     ] = "link-params"       , // LINK_PARAMS_NODE
  [BGP_EVPN_VNI_NODE    ] = "bgp evpn vni"      , // BGP_EVPN_VNI_NODE
  [RPKI_NODE            ] = "rpki"              , // RPKI_NODE
  [BGP_FLOWSPECV4_NODE  ] = "bgp ipv4 flowspec" , // BGP_FLOWSPECV4_NODE
  [BGP_FLOWSPECV6_NODE  ] = "bgp ipv6 flowspec" , // BGP_FLOWSPECV6_NODE
  [BFD_NODE             ] = "bfd"               , // BFD_NODE
  [BFD_PEER_NODE        ] = "bfd peer"          , // BFD_PEER_NODE
  [OPENFABRIC_NODE      ] = "openfabric"        , // OPENFABRIC_NODE
  [VRRP_NODE            ] = "vrrp"              , // VRRP_NODE
};
/* clang-format on */

static unsigned int
cmd_hash_key(const void *p)
{
  int size = sizeof(p);
  return jhash(p, size, 0);
}

static bool
cmd_hash_cmp(const void *a, const void *b)
{
  return a == b;
}

/*
 * Install top node of command vector.
 */
void install_node(struct cmd_node *node, int (*func)(struct vty *))
{
  vector_set_index(cmdvec, node->node, node);
  node->func = func;
  node->cmdgraph = graph_new();
  node->cmd_vector = vector_init(VECTOR_MIN_SIZE);
  // add start node
  struct cmd_token *token =
    cmd_token_new(START_TKN, CMD_ATTR_NORMAL, NULL, NULL);
  graph_new_node(node->cmdgraph, token,
           (void (*)(void *)) & cmd_token_del);
  node->cmd_hash = hash_create_size(16, cmd_hash_key, cmd_hash_cmp,
            "Command Hash");
}

