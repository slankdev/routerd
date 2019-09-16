
#include "vpp.h"
#include "vpp_cli.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <vnet/session/application_interface.h>
#include <vlibmemory/api.h>
#include <vpp/api/vpe_msg_enum.h>
#include <vnet/api_errno.h>
#include <svm/fifo_segment.h>
#define vl_typedefs
#include <vpp/api/vpe_all_api_h.h>
#undef vl_typedefs
#define vl_endianfun
#include <vpp/api/vpe_all_api_h.h>
#undef vl_endianfun
#define vl_print(handle, ...)
#define vl_printfun
#include <vpp/api/vpe_all_api_h.h>
#undef vl_printfun
#define vl_typedefs
#include <cplane_netdev/cplane_netdev_all_api_h.h>
#undef vl_typedefs

#include "vpp.h"
#define VPP_STR "Show VPP information\n"

#ifdef hash_create
#undef hash_create
#endif
#ifdef hash_get
#undef hash_get
#endif
#ifdef hash_free
#undef hash_free
#endif

#include <vui/vui.h>

#ifdef NO_VPP
#warning NO_VPP defined
#endif

#include "debug.h"

extern void routerd_context_add_interface(
    uint32_t kernl_index, const char *kern_name,
    uint32_t vpp_index, const char *vpp_name);

static int
snprintf_ether_addr(char *str, size_t size, const void *buffer)
{
  const uint8_t *addr = (const uint8_t*)buffer;
  int ret = snprintf(str, size, "%02x:%02x:%02x:%02x:%02x:%02x",
      addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  return ret;
}
inline static const char*
vpp_vnet_strerror(int vnet_api_errno)
{
  switch (vnet_api_errno) {
#define _(a,b,c) case (b): return c;
    foreach_vnet_api_error
#undef _
    default: return "unknown";
  }
}

static void
strfmt_append(char *str, size_t size, char *fmt, ...)
{
  char buf0[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf0, sizeof(buf0), fmt, args);
  va_end(args);

  char *pos = &str[strlen(str)];
  size_t pos_size = size - strlen(str);
  snprintf(pos, pos_size, "%s", buf0);
}

static int
snprintf_prefix(char *str, size_t size,
    const struct prefix *prefix)
{
  char buf[128];
  inet_ntop(prefix->afi, prefix->u.raw, buf, sizeof(buf));
  return snprintf(str, size, "%s", buf);
}

static const char*
link_speed_to_str(uint64_t speed)
{
#define _10M  (10000000UL)
#define _100M (100000000UL)
#define _1G   (1000000000UL)
#define _10G  (10000000000UL)
#define _40G  (40000000000UL)
#define _100G (100000000000UL)
  if (speed == _10M ) return "10M";
  if (speed == _100M) return "100M";
  if (speed == _1G  ) return "1G";
  if (speed == _10G ) return "10G";
  if (speed == _40G ) return "40G";
  if (speed == _100G) return "100G";
  return "UNKNOWN";
}

DEFUN (vpp,
       vpp_cmd,
       "vpp",
       "VPP setting\n")
{
  vty->node = find_node_id_by_name("vpp");
  vty->config = true;
  vty->xpath_index = 0;
  return CMD_SUCCESS;
}

DEFUN (show_vpp_ip_fib,
       show_vpp_ip_fib_cmd,
       "show vpp ip fib",
       SHOW_STR
       VPP_STR
       "Show ip information\n"
       "Show ip-fib information\n")
{
  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  const uint32_t table_id = 0;
  ip_route_dump(random(), table_id);
  send_ping(random());
  printf("\r\n");

  void *msg = NULL;
  api_main_t *am = &api_main;
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 3)) {
    uint16_t msg_id = ntohs(*((uint16_t*)msg));
    uint16_t pong_id = find_msg_id(CONTROL_PING_REPLY);
    if (msg_id == pong_id) {
      printf("end entry\n\r");
      break;
    }
    vl_api_ip_route_details_t *mp = msg;

    uint32_t table_id = mp->route.table_id;
    uint32_t stats_index = mp->route.stats_index;
    uint32_t prefix_len = mp->route.prefix.len;
    uint32_t afi = mp->route.prefix.address.af;
    uint8_t n_paths = mp->route.n_paths;
    uint8_t addr[4] = {0,0,0,0};
    memcpy(addr, mp->route.prefix.address.un.ip4, prefix_len/8);

    printf(" %u.%u.%u.%u/%u path=[",
        addr[0], addr[1], addr[2], addr[3],
        prefix_len);

    for (size_t i=0; i<n_paths; i++) {
      vl_api_fib_path_t *fib_path = &mp->route.paths[i];
      uint32_t sw_if_index = ntohl(fib_path->sw_if_index);
      uint32_t table_id    = ntohl(fib_path->table_id);
      uint32_t rpf_id      = ntohl(fib_path->rpf_id);
      uint8_t  weight = fib_path->weight;
      uint8_t  preference = fib_path->preference;
      uint32_t type  = ntohl(fib_path->type);
      uint32_t flags = ntohl(fib_path->flags);
      uint32_t proto = ntohl(fib_path->proto);
      printf("{int[%u] table[%u] type=%u proto=%u nh=",
        sw_if_index, table_id, type, proto);

      vl_api_fib_path_nh_t *nh = &fib_path->nh;
      vl_api_address_union_t *address = &nh->address;
      uint8_t *ip4_address = address->ip4;
      printf("%u.%u.%u.%u}",
          ip4_address[0], ip4_address[1],
          ip4_address[2], ip4_address[3]);
    }
    printf("]\r\n");
  }

  vl_client_disconnect_from_vlib();
  return CMD_SUCCESS;
}

DEFUN (show_vpp_interface,
       show_vpp_interface_cmd,
       "show vpp interface",
       SHOW_STR
       VPP_STR
       "Show VPP interface information\n")
{
#ifndef NO_VPP
  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  struct interface_info {
    bool enable;
    vl_api_sw_interface_details_t link;
    vl_api_ip_address_details_t addrs[256];
    size_t n_addrs;
  };

  struct interface_info interfaces[256];
  memset(interfaces, 0, sizeof(interfaces));
  void *msg = NULL;
  api_main_t *am = &api_main;

  /*
   * Get interfaces basic info
   */
  dump_ifcs(random());
  send_ping(random());
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 3)) {
    uint16_t msg_id = ntohs(*((uint16_t*)msg));
    uint16_t pong_id = find_msg_id(CONTROL_PING_REPLY);
    if (msg_id == pong_id) {
      break;
    }
    vl_api_sw_interface_details_t *mp = (vl_api_sw_interface_details_t*)msg;
    memcpy(&interfaces[ntohl(mp->sw_if_index)].link, mp, sizeof(*mp));
    interfaces[ntohl(mp->sw_if_index)].enable = true;
    dump_ipaddrs(random(), ntohl(mp->sw_if_index), false);
    dump_ipaddrs(random(), ntohl(mp->sw_if_index), true);
  }

  /*
   * Get interface ip-addrs info on each interface
   */
  send_ping(random());
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 1)) {
    uint16_t msg_id = ntohs(*((uint16_t*)msg));
    uint16_t pong_id = find_msg_id(CONTROL_PING_REPLY);
    if (msg_id == pong_id) {
      break;
    }
    vl_api_ip_address_details_t *mp = msg;
    const uint32_t ifindex = ntohl(mp->sw_if_index);
    const size_t n_addrs = interfaces[ifindex].n_addrs++;
    memcpy(&interfaces[ifindex].addrs[n_addrs], mp, sizeof(*mp));

  }

  /*
   * Disconnect from vlib
   */
  vl_client_disconnect_from_vlib();

  /*
   * Vty output
   */
  vty_out(vty, "\n");
  for (size_t i=0; i<256; i++) {
    if (!interfaces[i].enable)
      continue;

    char ipaddrs_str_buf[512];
    memset(ipaddrs_str_buf, 0, sizeof(ipaddrs_str_buf));
    for (size_t j=0; j<interfaces[i].n_addrs; j++) {
      vl_api_ip_address_details_t *mp = &interfaces[i].addrs[j];
      char str[128];
      /* inet_ntop(mp->is_ipv6 ? AF_INET6 : AF_INET, */
      /*     mp->ip, str, sizeof(str)); */
      /* strfmt_append(ipaddrs_str_buf,
       * sizeof(ipaddrs_str_buf), "%s/%u ", str, mp->prefix_length); */
    }
    if (strlen(ipaddrs_str_buf) == 0)
      snprintf(ipaddrs_str_buf, sizeof(ipaddrs_str_buf), "none");

    vl_api_sw_interface_details_t *mp = &interfaces[i].link;
    char buf[128];
    snprintf_ether_addr(buf, sizeof(buf), mp->l2_address);
    vty_out(vty,
        " %s: idx[%u], admin %s, link %s\n"
        "  Hardwre address: %s\n"
        "  Internet address: %s\n"
        "  MTU-LINK/L3/IP/IP6/MPLS: %u/%u/%u/%u/%u, Link-Speed/Duplex: %s/%s\n\n",
        mp->interface_name, clib_net_to_host_u32(mp->sw_if_index),
        mp->admin_up_down > 0 ? "up" : "down", mp->link_up_down > 0 ? "up" : "down", buf,
        ipaddrs_str_buf,
        ntohs(mp->link_mtu),
        ntohl(mp->mtu[0]), ntohl(mp->mtu[1]), ntohl(mp->mtu[2]), ntohl(mp->mtu[3]),
        link_speed_to_str(ntohl(mp->link_speed)),
        mp->link_duplex == 1 ? "Full" : "Half");

  }

#endif
  return CMD_SUCCESS;
}

DEFUN(set_interface_address,
       set_interface_address_cmd,
       "set interface < ipv4 address [del] idx NAME A.B.C.D/M | ipv6 address [del] idx NAME X:X::X:X/M >",
       "Setting\n"
       "Interface setting\n"
       "Interface ipv4 address setting\n"
       "Interface ipv4 address setting\n"
       "Delete interface ipv4 address\n"
       "Specify interface index\n"
       "Specify interface index\n"
       "Specify interface address\n"
       "Interface ipv6 address setting\n"
       "Interface ipv6 address setting\n"
       "Delete interface ipv6 address\n"
       "Specify interface index\n"
       "Specify interface index\n"
       "Specify interface address\n")
{
  bool negate = strcmp(argv[4]->arg, "del") == 0;
  const size_t is_ipv6_pos = 2;
  const size_t ifindex_pos = negate ? 6 : 5;
  const size_t prefix_pos = negate ? 7 : 6;
  bool is_ipv6 = strcmp(argv[is_ipv6_pos]->arg, "ipv6") == 0;
  uint32_t ifindex = strtol(argv[ifindex_pos]->arg, NULL, 0);
  const char *pref_str = argv[prefix_pos]->arg;

  struct prefix prefix;
  parse_prefix_str(pref_str,
      is_ipv6 ? AF_INET6 : AF_INET, &prefix);

  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  set_interface_addr(3,
      ifindex, !negate, is_ipv6, prefix.u.raw, prefix.plen);

  void *msg = NULL;
  api_main_t *am = &api_main;
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 1)) {
    vl_api_sw_interface_add_del_address_reply_t * mp =
      (vl_api_sw_interface_add_del_address_reply_t*)msg;
    int32_t retval = ntohl(mp->retval);
    if (retval < 0) {
      vty_out(vty, "setting was failed: %s (%d)\n",
          vpp_vnet_strerror(retval), retval);
      vl_client_disconnect_from_vlib ();
      return CMD_WARNING_CONFIG_FAILED;
    }
    vl_client_disconnect_from_vlib();
    return CMD_SUCCESS;
  }

  vty_out(vty, "timeout\n");
  vl_client_disconnect_from_vlib ();
  return CMD_WARNING_CONFIG_FAILED;
}

DEFUN(set_interface_state,
       set_interface_state_cmd,
       "set interface state idx NAME <up|down>",
       "Setting\n"
       "Interface setting\n"
       "Interface state setting\n"
       "Specify interface index\n"
       "Specify interface index\n"
       "Set interface state up\n"
       "Set interface state down\n")
{
  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  uint32_t ifindex = strtol(argv[4]->arg, NULL, 0);
  bool is_up = strcmp(argv[5]->arg, "up") == 0 ? true : false;
  set_interface_flag(2, ifindex, is_up);

  void *msg2;
  api_main_t *am = &api_main;

  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg2, SVM_Q_TIMEDWAIT, 3)) {
    vl_api_sw_interface_set_flags_reply_t * mp =
      (vl_api_sw_interface_set_flags_reply_t*)msg2;
    int32_t retval = ntohl(mp->retval);
    if (retval < 0) {
      vty_out(vty, "setting was failed: %s (%d)\n",
          vpp_vnet_strerror(retval), retval);
      vl_client_disconnect_from_vlib ();
      return CMD_WARNING_CONFIG_FAILED;
    }
    break;
  }

  vl_client_disconnect_from_vlib ();
  return CMD_SUCCESS;
}

DEFUN (create_loopback_interface,
       create_loopback_interface_cmd,
       "create loopback interface",
       "Create setting\n"
       "Create loopback interface\n"
       "Create loopback interface\n")
{
  if (connect_to_vpp("routerd", false) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  void *msg;
  api_main_t *am = &api_main;
  create_loopback(random());
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 3)) {
    vl_api_create_loopback_reply_t * mp = msg;
    int32_t retval = ntohl(mp->retval);
    if (retval < 0) {
      vty_out(vty, "setting was failed: %s (%d)\n",
          vpp_vnet_strerror(retval), retval);
      vl_client_disconnect_from_vlib ();
      return CMD_WARNING_CONFIG_FAILED;
    }
    vl_client_disconnect_from_vlib ();
    return CMD_SUCCESS;
  }

  vty_out(vty, "timeout\n");
  vl_client_disconnect_from_vlib ();
  return CMD_WARNING_CONFIG_FAILED;
}

DEFUN (vpe_connect,
       vpe_connect_cmd,
       "vpe connect",
       "VPP-VPE Setting\n"
       "Connect to VPP-VPE\n")
{
  if (connect_to_vpp("routerd", false) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }
  return CMD_SUCCESS;
}

DEFUN (vpe_disconnect,
       vpe_disconnect_cmd,
       "vpe disconnect",
       "VPP-VPE Setting\n"
       "Disconnect to VPP-VPE\n")
{
  vl_client_disconnect_from_vlib ();
  return CMD_SUCCESS;
}

DEFUN (ip_adddel_route,
       ip_adddel_route_cmd,
       "<add|del> < ipv4 route A.B.C.D/M via A.B.C.D nh-ifindex NAME | "
                   "ipv6 route X:X::X:X/M via X:X::X:X nh-ifindex NAME >",
       "Add setting\n"
       "Del setting\n"
       "Add ipv4 settting\n"
       "Add ipv4 route settting\n"
       "Specify ipv4 prefix\n"
       "Specify ipv4 nexthop\n"
       "Specify ipv4 nexthop\n"
       "Specify nexthop-ifindex\n"
       "Specify nexthop-ifindex\n"
       "Add ipv6 settting\n"
       "Add ipv6 route settting\n"
       "Specify ipv6 prefix\n"
       "Specify ipv6 nexthop\n"
       "Specify ipv6 nexthop\n"
       "Specify nexthop-ifindex\n"
       "Specify nexthop-ifindex\n")
{
  const size_t negate = strcmp(argv[0]->arg, "del") == 0;
  const bool is_ipv6 = strcmp(argv[1]->arg, "ipv6") == 0;
  const char *prefix_str = argv[3]->arg;
  const char *nexthop_str = argv[5]->arg;
  const char *nh_ifindex_str = argv[7]->arg;

  struct prefix route_pref;
  struct prefix nexthop_pref;
  int afi = is_ipv6 ? AF_INET6 : AF_INET;
  parse_prefix_str(prefix_str, afi, &route_pref);
  parse_prefix_str(nexthop_str, afi, &nexthop_pref);
  uint32_t nh_ifindex = strtol(nh_ifindex_str, NULL, 0);

  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  ip_route_add_del(10, !negate, &route_pref, &nexthop_pref, nh_ifindex);

  void *msg = NULL;
  api_main_t *am = &api_main;
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 1)) {
    vl_api_sw_interface_add_del_address_reply_t * mp =
      (vl_api_sw_interface_add_del_address_reply_t*)msg;
    int32_t retval = ntohl(mp->retval);
    if (retval < 0) {
      vty_out(vty, "setting was failed: %s (%d)\n",
          vpp_vnet_strerror(retval), retval);
      vl_client_disconnect_from_vlib ();
      return CMD_WARNING_CONFIG_FAILED;
    }
    vl_client_disconnect_from_vlib();
    return CMD_SUCCESS;
  }

  vty_out(vty, "timeout\n");
  vl_client_disconnect_from_vlib ();
  return CMD_WARNING_CONFIG_FAILED;
}

DEFUN (show_vpp_tap_inject,
       show_vpp_tap_inject_cmd,
       "show vpp tap-inject",
       SHOW_STR
       VPP_STR
       "Show VPP tap-inject information\n")
{
#ifndef NO_VPP
  vty_out(vty, "%s\n", __func__);
  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "%s: Couldn't connect to vpe, exiting...\r\n", __func__);
    return CMD_WARNING_CONFIG_FAILED;
  }

  api_main_t *am = &api_main;
  void *msg = NULL;

  tap_inject_dump(2);
  send_ping(random());
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 3)) {
    uint16_t msg_id = ntohs(*((uint16_t*)msg));
    uint16_t pong_id = find_msg_id(CONTROL_PING_REPLY);
    if (msg_id == pong_id) {
      vty_out(vty, "pong recv\n");
      break;
    }

    vl_api_tap_inject_details_t *mp = msg;
    vty_out(vty, "recv interface info v/k=%u/%u\n",
        ntohl(mp->sw_if_index), ntohl(mp->kernel_if_index));
    /* memcpy(&interfaces[ntohl(mp->sw_if_index)].link, mp, sizeof(*mp)); */
    /* interfaces[ntohl(mp->sw_if_index)].enable = true; */
  }

  disconnect_from_vpp ();
  return CMD_SUCCESS;
#endif
}

DEFUN (enable_tap_inject,
       enable_tap_inject_cmd,
       "<enable|disable> tap-inject",
       "Enable parameter\n"
       "Disable parameter\n"
       "vpp\'s dataplane tap-inject\n")
{
  const size_t is_enable = strcmp(argv[0]->arg, "enable") == 0;
  if (debug_enabled(CLI))
    printf("%s\r\n", __func__);

  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "%s: Couldn't connect to vpe, exiting...\r\n", __func__);
    return CMD_WARNING_CONFIG_FAILED;
  }

  enable_disable_tap_inject(random(), is_enable);
  int32_t retval = enable_disable_tap_inject_retval();
  disconnect_from_vpp ();
  return CMD_SUCCESS; // XXX
  return retval != -1 ? CMD_WARNING_CONFIG_FAILED : CMD_SUCCESS;
}

DEFUN (enable_cplane_netdev_sync,
       enable_cplane_netdev_sync_cmd,
       "<enable|disable> cplane-netdev sync",
       "Enable parameter\n"
       "Disable parameter\n"
       "vpp\'s cplane-netdev using tap-inject\n"
       "Sync info\n")
{
#ifndef NO_VPP
  if (debug_enabled(CLI))
    printf("%s\r\n", __func__);

  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "%s: Couldn't connect to vpe, exiting...\r\n", __func__);
    return CMD_WARNING_CONFIG_FAILED;
  }

  api_main_t *am = &api_main;
  void *msg = NULL;
  tap_inject_dump(2);
  send_ping(random());
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 3)) {
    uint16_t msg_id = ntohs(*((uint16_t*)msg));
    uint16_t pong_id = find_msg_id(CONTROL_PING_REPLY);
    if (msg_id == pong_id) {
      break;
    }

    vl_api_tap_inject_details_t *mp = msg;
    uint32_t vpp_index = ntohl(mp->sw_if_index);
    uint32_t kern_index = ntohl(mp->kernel_if_index);

    char kern_name[256];
    char *ret = if_indextoname(kern_index, kern_name);
    assert(ret);

    routerd_context_add_interface(
        kern_index, kern_name,
        vpp_index, "vpp");
  }

  disconnect_from_vpp ();
#endif
  return CMD_SUCCESS;
}

DEFUN (show_vpp_proc_info,
       show_vpp_proc_info_cmd,
       "show vpp proc-info NAME",
       SHOW_STR
       VPP_STR
       "Show VPP process information\n"
       "Specify VPP process name (as-node-name)\n")
{
  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  api_main_t *am = &api_main;
  void *msg = NULL;
  const char *name = argv[3]->arg;
  get_proc_info(random(), name);
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 1)) {
    vl_api_get_proc_info_reply_t *mp = msg;
    if (mp->retval < 0) {
      vty_out(vty, "not such proc or node-type isn't process\n");
      disconnect_from_vpp();
      return CMD_SUCCESS;
    }

    vty_out(vty, "flags: %u (%s)\n", mp->proc_flags,
        vpp_proc_flags_to_state(mp->proc_flags));
    disconnect_from_vpp();
    return CMD_SUCCESS;
  }

  // timeout
  vty_out(vty, "timeout\n");
  disconnect_from_vpp();
  return CMD_WARNING_CONFIG_FAILED;
}

DEFUN (show_vpp_node_info,
       show_vpp_node_info_cmd,
       "show vpp node-info NAME",
       SHOW_STR
       VPP_STR
       "Show VPP node information\n"
       "Specify VPP node name\n")
{
  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  api_main_t *am = &api_main;
  void *msg = NULL;
  const char *name = argv[3]->arg;
  get_node_info(random(), name);
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 1)) {
    vl_api_get_node_info_reply_t *mp = msg;
    if (mp->retval < 0) {
      vty_out(vty, "not such node\n");
      disconnect_from_vpp();
      return CMD_SUCCESS;
    }

    vty_out(vty, "index: %u\n", mp->node_index);
    vty_out(vty, "type : %u (%s)\n",
        mp->node_type,
        vpp_node_type_str(mp->node_type)
        );
    vty_out(vty, "state: %u\n", mp->node_state);
    vty_out(vty, "flags: %u\n", mp->node_flags);
    disconnect_from_vpp();
    return CMD_SUCCESS;
  }

  // timeout
  vty_out(vty, "timeout\n");
  disconnect_from_vpp();
  return CMD_WARNING_CONFIG_FAILED;
}

static bool
vpp_startup_config_process_is_done()
{
  if (connect_to_vpp("routerd-wait", true) < 0) {
    svm_region_exit ();
    return false;
  }

  api_main_t *am = &api_main;
  void *msg = NULL;
  get_proc_info(random(), "startup-config-process");
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 1)) {
    vl_api_get_proc_info_reply_t *mp = msg;
    if (mp->retval < 0) {
      disconnect_from_vpp();
      return false;
    }

    bool done = (mp->proc_flags == 0);
    disconnect_from_vpp();
    return done;
  }

  // timeout
  disconnect_from_vpp();
  return false;
}

DEFUN (wait_startup_config_process_done,
       wait_startup_config_process_done_cmd,
       "wait-startup-config-process-done try-count <(1-99)> interval <(0-10)>",
       "Wait VPP's startup-config-process was done\n"
       "Specify try count\n"
       "Specify try count\n"
       "Specify try interval (second) \n"
       "Specify try interval (second) \n")
{
  size_t try_cnt = strtol(argv[2]->arg, NULL, 0);
  size_t interval = strtol(argv[4]->arg, NULL, 0);
  if (debug_enabled(CLI))
    printf("waiting startup-config-process\r\n");

  for (size_t i=0; i<try_cnt; i++) {
    bool done = vpp_startup_config_process_is_done();
    if (done)
      break;
    sleep(interval);
  }

  if (debug_enabled(CLI))
    printf("waiting startup-config-process...done\r\n");
  return CMD_SUCCESS;
}

void
setup_vpp_node(vui_t *vui)
{
  routerd_main_t *rm = &routerd_main;
  unformat_input_t _argv, *a = &_argv;
  clib_mem_init_thread_safe (0, 256 << 20);

  vui_node_t *vpp_node = vui_node_new();
  vpp_node->name = strdup("vpp");
  vpp_node->prompt = strdup("%s(config-vpp)# ");
  vpp_node->parent = CONFIG_NODE;
  vui_node_install(vui, vpp_node);

  vui_install_default_element(vui, vpp_node->node);
  vui_install_element(vui, CONFIG_NODE, &vpp_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_vpp_interface_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_vpp_ip_fib_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_vpp_tap_inject_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_vpp_node_info_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_vpp_proc_info_cmd);
  vui_install_element(vui, vpp_node->node, &vpe_connect_cmd);
  vui_install_element(vui, vpp_node->node, &vpe_disconnect_cmd);
  vui_install_element(vui, vpp_node->node, &enable_tap_inject_cmd);
  vui_install_element(vui, vpp_node->node, &enable_cplane_netdev_sync_cmd);
  vui_install_element(vui, vpp_node->node, &set_interface_address_cmd);
  vui_install_element(vui, vpp_node->node, &set_interface_state_cmd);
  vui_install_element(vui, vpp_node->node, &create_loopback_interface_cmd);
  vui_install_element(vui, vpp_node->node, &ip_adddel_route_cmd);
  vui_install_element(vui, vpp_node->node, &wait_startup_config_process_done_cmd);
}
