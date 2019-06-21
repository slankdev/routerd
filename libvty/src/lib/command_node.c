
#include "command_node.h"

extern vector cmdvec;

const char *node_names[] = {
  "auth",               // AUTH_NODE,
  "view",               // VIEW_NODE,
  "auth enable",        // AUTH_ENABLE_NODE,
  "enable",             // ENABLE_NODE,
  "config",             // CONFIG_NODE,
  "debug",              // DEBUG_NODE,
  "northbound debug",   // NORTHBOUND_DEBUG_NODE,
  "vnc debug",          // DEBUG_VNC_NODE,
  "route-map debug",    // RMAP_DEBUG_NODE
  "aaa",                // AAA_NODE,
  "keychain",           // KEYCHAIN_NODE,
  "keychain key",       // KEYCHAIN_KEY_NODE,
  "logical-router",     // LOGICALROUTER_NODE,
  "static ip",          // IP_NODE,
  "interface",          // INTERFACE_NODE,
  "nexthop-group",      // NH_GROUP_NODE,
  "zebra",              // ZEBRA_NODE,
  "table",              // TABLE_NODE,
  "rip",                // RIP_NODE,
  "ripng",              // RIPNG_NODE,
  "babel",              // BABEL_NODE,
  "eigrp",              // EIGRP_NODE,
  "rfp defaults",       // RFP_DEFAULTS_NODE,
  "bgp evpn",           // BGP_EVPN_NODE,
  "ospf",               // OSPF_NODE,
  "ospf6",              // OSPF6_NODE,
  "ldp",                // LDP_NODE,
  "ldp ipv4",           // LDP_IPV4_NODE,
  "ldp ipv6",           // LDP_IPV6_NODE,
  "ldp ipv4 interface", // LDP_IPV4_IFACE_NODE,
  "ldp ipv6 interface", // LDP_IPV6_IFACE_NODE,
  "ldp l2vpn",          // LDP_L2VPN_NODE,
  "ldp",                // LDP_PSEUDOWIRE_NODE,
  "isis",               // ISIS_NODE,
  "ipv4 access list",   // ACCESS_NODE,
  "ipv4 prefix list",   // PREFIX_NODE,
  "ipv6 access list",   // ACCESS_IPV6_NODE,
  "MAC access list",    // ACCESS_MAC_NODE,
  "ipv6 prefix list",   // PREFIX_IPV6_NODE,
  "as list",            // AS_LIST_NODE,
  "community list",     // COMMUNITY_LIST_NODE,
  "routemap",           // RMAP_NODE,
  "pbr-map",            // PBRMAP_NODE,
  "smux",               // SMUX_NODE,
  "dump",               // DUMP_NODE,
  "forwarding",         // FORWARDING_NODE,
  "protocol",           // PROTOCOL_NODE,
  "mpls",               // MPLS_NODE,
  "pw",                 // PW_NODE,
  "vty",                // VTY_NODE,
  "link-params",        // LINK_PARAMS_NODE,
  "bgp evpn vni",       // BGP_EVPN_VNI_NODE,
  "rpki",               // RPKI_NODE
  "bgp ipv4 flowspec",  /* BGP_FLOWSPECV4_NODE */
  "bgp ipv6 flowspec",  /* BGP_FLOWSPECV6_NODE */
  "bfd",                /* BFD_NODE */
  "bfd peer",           /* BFD_PEER_NODE */
  "openfabric",         // OPENFABRIC_NODE
  "vrrp",               /* VRRP_NODE */
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

