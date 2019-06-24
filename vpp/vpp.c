
#include <vui/vui.h>
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
#include "hexdump.h"

#define VPP_STR "Show VPP information\n"

#define CONTROL_PING_MESSAGE "control_ping"
#define CONTROL_PING_REPLY_MESSAGE "control_ping_reply"
#define DUMP_IFC_MESSAGE "sw_interface_dump"
#define IFC_DETAIL_MESSAGE "sw_interface_details"
#define SET_IFC_FLAGS "sw_interface_set_flags"
#define SET_IFC_FLAGS_REPLY "sw_interface_set_flags_reply"
#define SET_IFC_ADDR "sw_interface_add_del_address"
#define SET_IFC_ADDR_REPLY "sw_interface_add_del_address_reply"

typedef struct
{
  unix_shared_memory_queue_t * vl_input_queue;
  u32 my_client_index;
} routerd_main_t;
routerd_main_t routerd_main;

pid_t gettid(void) { return syscall(SYS_gettid); }

inline static uint32_t
find_msg_id(char* msg)
{
  api_main_t * am = &api_main;
  hash_pair_t *hp;
#define _HASH_FUNC_IMPL_INSIDE_IMPL_ { \
  char *key = (char *)hp->key; \
  int msg_name_len = strlen(key) - 9; \
  if (strlen(msg) == msg_name_len && \
    strncmp(msg, (char *)hp->key, msg_name_len) == 0) \
    return (u32)hp->value[0]; \
}
  hash_foreach_pair (hp,
      am->msg_index_by_name_and_crc,
      (_HASH_FUNC_IMPL_INSIDE_IMPL_));
}

static int
send_ping(u16 ping_id, u16 msg_id)
{
  routerd_main_t *rm = &routerd_main;
  vl_api_control_ping_t *mp =
    vl_msg_api_alloc(sizeof(vl_api_control_ping_t));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(ping_id);
  mp->client_index = rm->my_client_index;
  mp->context = htonl(msg_id);
  vl_msg_api_send_shmem(rm->vl_input_queue, (u8 *) &mp);
}

static int
dump_ifcs(u32 dump_id, u32 message_id)
{
  routerd_main_t *jm = &routerd_main;
  vl_api_sw_interface_dump_t *mp;
  mp = vl_msg_api_alloc(sizeof(*mp));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(dump_id);
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(message_id);
  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
}

static int
set_interface_flag(u32 set_id, u32 message_id, uint32_t ifindex, bool is_up)
{
  routerd_main_t *jm = &routerd_main;
  vl_api_sw_interface_set_flags_t *mp =
    vl_msg_api_alloc(sizeof(*mp));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(set_id);
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(message_id);
  mp->sw_if_index = htonl(ifindex);
  mp->admin_up_down = is_up ? 1 : 0;
  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
}

static int
set_interface_addr(uint32_t set_id, uint32_t msg_id,
    uint32_t ifindex, bool is_add, bool is_ipv6,
    const void *addr_buffer, size_t addr_len)
{
  routerd_main_t *jm = &routerd_main;
  vl_api_sw_interface_add_del_address_t *mp =
    vl_msg_api_alloc(sizeof(*mp));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(set_id);
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(msg_id);
  mp->sw_if_index = htonl(ifindex);
  mp->is_add = is_add;
  mp->is_ipv6 = is_ipv6 ? 1 : 0;
  mp->address_length = addr_len;
  memcpy(mp->address, addr_buffer, mp->address_length);
  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
}

static int
connect_to_vpp (char *name, bool no_rx_pthread)
{
  routerd_main_t *rm = &routerd_main;
  api_main_t *am = &api_main;
  int ret = -1;
  if (no_rx_pthread) ret = vl_client_connect_to_vlib_no_rx_pthread("/vpe-api", name, 32);
  else ret = vl_client_connect_to_vlib("/vpe-api", name, 32);
  if (ret < 0) {
    clib_warning ("shmem connect failed");
    return -1;
  }
  rm->my_client_index = am->my_client_index;
  rm->vl_input_queue = am->shmem_hdr->vl_input_queue;
  return 0;
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

DEFUN (show_vpp_vpe_message_table,
       show_vpp_vpe_message_table_cmd,
       "show vpp vpe message-table",
       SHOW_STR
       VPP_STR
       "Show VPP VPE information\n"
       "Show VPP VPE message-table\n")
{
  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }
  vty_out(vty, "CONTROL_PING_MESSAGE      : %04x\n", find_msg_id(CONTROL_PING_MESSAGE      ));
  vty_out(vty, "CONTROL_PING_REPLY_MESSAGE: %04x\n", find_msg_id(CONTROL_PING_REPLY_MESSAGE));
  vty_out(vty, "DUMP_IFC_MESSAGE          : %04x\n", find_msg_id(DUMP_IFC_MESSAGE          ));
  vty_out(vty, "IFC_DETAIL_MESSAGE        : %04x\n", find_msg_id(IFC_DETAIL_MESSAGE        ));
  vl_client_disconnect_from_vlib ();
  return CMD_SUCCESS;
}

static int
snprintf_ether_addr(char *str, size_t size, const void *buffer)
{
  const uint8_t *addr = (const void*)buffer;
  int ret = snprintf(str, size, "%02x:%02x:%02x:%02x:%02x:%02x",
      addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  return ret;
}

DEFUN (show_vpp_interface,
       show_vpp_interface_cmd,
       "show vpp interface",
       SHOW_STR
       VPP_STR
       "Show VPP interface information\n")
{
  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  dump_ifcs(find_msg_id(DUMP_IFC_MESSAGE), 2);
  send_ping(find_msg_id(CONTROL_PING_MESSAGE), 1);

  void *msg = NULL;
  api_main_t *am = &api_main;
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 3)) {
    uint16_t msg_id = ntohs(*((uint16_t*)msg));
    uint16_t pong_id = find_msg_id(CONTROL_PING_REPLY_MESSAGE);
    if (msg_id == pong_id)
      break;

    vl_api_sw_interface_details_t * mp = (vl_api_sw_interface_details_t*)msg;
    char buf[128];
    snprintf_ether_addr(buf, sizeof(buf), mp->l2_address);
    vty_out(vty, " %s: idx=%u admin=%s link=%s mac=%s\n",
        mp->interface_name,
        clib_net_to_host_u32(mp->sw_if_index),
        mp->admin_up_down > 0 ? "up" : "down",
        mp->link_up_down > 0 ? "up" : "down", buf);
  }

  vl_client_disconnect_from_vlib();
  return CMD_SUCCESS;
}

struct prefix {
  int afi;
  size_t plen;
  union {
    uint8_t raw[16];
    struct in6_addr in6;
    struct in_addr in4;
  } u;
};

static void
parse_prefix_str(const char *str, int afi, struct prefix *pref)
{
  assert(afi == AF_INET || afi == AF_INET6);
  pref->afi = afi;

  if (afi == AF_INET) {
    char *pnt = strchr(str, '/');
    if (pnt == NULL) {
      inet_pton(AF_INET, str, &pref->u.in4);
      pref->plen = 32;
    } else {
      char buf[128];
      strncpy(buf, str, sizeof(buf));
      buf[pnt - str] = '\0';
      const char *plen_str = &buf[pnt - str + 1];
      inet_pton(AF_INET, buf, &pref->u.in4);
      pref->plen = strtol(plen_str, NULL, 0);
    }
  } else {
    char *pnt = strchr(str, '/');
    if (pnt == NULL) {
      inet_pton(AF_INET6, str, &pref->u.in6);
      pref->plen = 128;
    } else {
      char buf[128];
      strncpy(buf, str, sizeof(buf));
      buf[pnt - str] = '\0';
      const char *plen_str = &buf[pnt - str + 1];
      inet_pton(AF_INET6, buf, &pref->u.in6);
      pref->plen = strtol(plen_str, NULL, 0);
    }
  }
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

  set_interface_addr(find_msg_id(SET_IFC_ADDR), 3,
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
  set_interface_flag(find_msg_id(SET_IFC_FLAGS), 2, ifindex, is_up);

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
  vui_install_element(vui, ENABLE_NODE, &show_vpp_vpe_message_table_cmd);
  vui_install_element(vui, vpp_node->node, &vpe_connect_cmd);
  vui_install_element(vui, vpp_node->node, &vpe_disconnect_cmd);

  // TODO: change node ENABLE_NODE -> vpp_node->node
  vui_install_element(vui, ENABLE_NODE, &set_interface_address_cmd);
  vui_install_element(vui, ENABLE_NODE, &set_interface_state_cmd);
  /* vui_install_element(vui, ENABLE_NODE, &create_loopback_interface); */
  /* vui_install_element(vui, ENABLE_NODE, &add_route); */
}

