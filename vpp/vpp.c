
#include <vui/vui.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <vnet/session/application_interface.h>
#include <vlibmemory/api.h>
#include <vpp/api/vpe_msg_enum.h>
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

typedef struct
{
  unix_shared_memory_queue_t * vl_input_queue;
  u32 my_client_index;
} routerd_main_t;
routerd_main_t routerd_main;

pid_t gettid(void) { return syscall(SYS_gettid); }

inline static uint32_t
find_msg_id(char* msg) {
  api_main_t * am = &api_main;
  hash_pair_t *hp;
  hash_foreach_pair (hp, am->msg_index_by_name_and_crc, ({
       char *key = (char *)hp->key; // key format: name_crc
       int msg_name_len = strlen(key) - 9; // ignore crc
       if (strlen(msg) == msg_name_len &&
           strncmp(msg, (char *)hp->key, msg_name_len) == 0) {
         return (u32)hp->value[0];
       }
   }));
}

static void
stop_signal (int signum)
{
  routerd_main_t *rm = &routerd_main;
}

static void
stats_signal (int signum)
{
  routerd_main_t *rm = &routerd_main;
}

static clib_error_t *
setup_signal_handlers (void)
{
  signal (SIGINT, stats_signal);
  signal (SIGQUIT, stop_signal);
  signal (SIGTERM, stop_signal);
  return 0;
}

static int
send_ping(u16 ping_id, u16 msg_id) {
  routerd_main_t *rm = &routerd_main;

  vl_api_control_ping_t * mp =
    vl_msg_api_alloc(sizeof(vl_api_control_ping_t));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(ping_id);
  mp->client_index = rm->my_client_index;
  mp->context = htonl(msg_id);
  vl_msg_api_send_shmem(rm->vl_input_queue, (u8 *) &mp);
  printf("Sending ping. Ping msg id: %u, message id: %u\n", ping_id, msg_id);
}

static int
dump_ifcs(u32 dump_id, u32 message_id) {
  routerd_main_t *jm = &routerd_main;

  vl_api_sw_interface_dump_t * mp;
  mp = vl_msg_api_alloc(sizeof(*mp));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(dump_id);
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(message_id);

  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
  printf("\r\nSending dump interfaces. Msg id: %d, message id: %d\r\n", dump_id, message_id);
}

static void
vl_api_control_ping_reply_t_handler(vl_api_control_ping_reply_t * mp) {
    printf("\tPing reply received, message id: %d, with retval: %d\n",
        htonl(mp->context), mp->retval);
}

static void vl_api_sw_interface_details_t_handler(vl_api_sw_interface_details_t * mp) {
    /* printf("command handled on tid=%d\r\n", gettid()); */
    printf("\tInterface, message id: %d, interface index: %d\r\n",
        clib_net_to_host_u32(mp->context), clib_net_to_host_u32(mp->sw_if_index));
}

int
connect_to_vpp (char *name, bool no_rx_pthread)
{
  routerd_main_t *rm = &routerd_main;
  api_main_t *am = &api_main;

  int ret = -1;
  if (no_rx_pthread)
    ret = vl_client_connect_to_vlib_no_rx_pthread("/vpe-api", name, 32);
  else
    ret = vl_client_connect_to_vlib("/vpe-api", name, 32);
  if (ret < 0) {
    clib_warning ("shmem connect failed");
    return -1;
  }

  rm->my_client_index = am->my_client_index;
  rm->vl_input_queue = am->shmem_hdr->vl_input_queue;

  vl_msg_api_set_handlers(
			find_msg_id(CONTROL_PING_REPLY_MESSAGE),
			CONTROL_PING_REPLY_MESSAGE,
      vl_api_control_ping_reply_t_handler, vl_noop_handler,
      vl_api_control_ping_reply_t_endian,
      vl_api_control_ping_reply_t_print,
      sizeof(vl_api_control_ping_reply_t), 1);

  vl_msg_api_set_handlers(
      find_msg_id(IFC_DETAIL_MESSAGE),
      IFC_DETAIL_MESSAGE,
      vl_api_sw_interface_details_t_handler, vl_noop_handler,
      vl_api_sw_interface_details_t_endian,
      vl_api_sw_interface_details_t_print,
      sizeof(vl_api_sw_interface_details_t), 1);

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
  vty_out(vty, "CONTROL_PING_MESSAGE      : %04x\n", find_msg_id(CONTROL_PING_MESSAGE      ));
  vty_out(vty, "CONTROL_PING_REPLY_MESSAGE: %04x\n", find_msg_id(CONTROL_PING_REPLY_MESSAGE));
  vty_out(vty, "DUMP_IFC_MESSAGE          : %04x\n", find_msg_id(DUMP_IFC_MESSAGE          ));
  vty_out(vty, "IFC_DETAIL_MESSAGE        : %04x\n", find_msg_id(IFC_DETAIL_MESSAGE        ));
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
  routerd_main_t *rm = &routerd_main;
  unformat_input_t _argv, *a = &_argv;
  clib_mem_init_thread_safe (0, 256 << 20);

  if (connect_to_vpp("routerd", true) < 0) {
    svm_region_exit ();
    vty_out(vty, "Couldn't connect to vpe, exiting...\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  dump_ifcs(find_msg_id(DUMP_IFC_MESSAGE), 2);
  send_ping(find_msg_id(CONTROL_PING_MESSAGE), 1);

  uword msg;
  api_main_t *am = &api_main;
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 1)) {
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

  vl_client_disconnect_from_vlib ();
  return CMD_SUCCESS;
}

DEFUN (vpe_connect,
       vpe_connect_cmd,
       "vpe connect",
       "VPP-VPE Setting\n"
       "Connect to VPP-VPE\n")
{
  routerd_main_t *rm = &routerd_main;
  unformat_input_t _argv, *a = &_argv;
  clib_mem_init_thread_safe (0, 256 << 20);

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
}

