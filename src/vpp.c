
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
#define vl_typedefs
#include <cplane_netdev/cplane_netdev_all_api_h.h>
#undef vl_typedefs
#include "hexdump.h"
#include "vpp.h"

#ifdef NO_VPP
#warning NO_VPP defined
#endif

routerd_main_t routerd_main;

static void*
msg_alloc_zero(size_t size)
{
  void* ptr = vl_msg_api_alloc(size);
  memset(ptr, 0, size);
  return ptr;
}

const char*
vpp_node_type_str(uint8_t num)
{
  static char *table[] = {
    [0] = "internale",
    [1] = "input",
    [2] = "pre_input",
    [3] = "process",
    [4] = NULL,
  };
  assert(table[num]);
  return table[num];
}

const char*
vpp_proc_flags_to_state(uint16_t flags)
{
#ifndef VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_CLOCK
#define VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_CLOCK (1 << 0)
#endif
#ifdef VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_EVENT
#define VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_EVENT (1 << 1)
#endif
  switch (flags &
      (VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_CLOCK |
       VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_EVENT)) {
  default:
    if (!(flags & VLIB_PROCESS_IS_RUNNING))
      return "done";
    break;

  case VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_CLOCK:
    return "time wait";

  case VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_EVENT:
    return "event wait";

  case (VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_EVENT |
        VLIB_PROCESS_IS_SUSPENDED_WAITING_FOR_CLOCK):
    return "any wait";
  }
  return "active";
}

int
get_node_info(uint16_t msg_id, const char *node_name)
{
  uint16_t vl_msg_id = find_msg_id(GET_NODE_INFO);
  routerd_main_t *rm = &routerd_main;
  vl_api_get_node_info_t *mp =
    vl_msg_api_alloc(sizeof(*mp));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(vl_msg_id);
  mp->client_index = rm->my_client_index;
  mp->context = htonl(msg_id);

  memcpy(mp->node_name, node_name, strlen(node_name));
  vl_msg_api_send_shmem(rm->vl_input_queue, (u8 *) &mp);
  return 0;
}

int
get_proc_info(uint16_t msg_id, const char *node_name)
{
  uint16_t vl_msg_id = find_msg_id(GET_PROC_INFO);
  routerd_main_t *rm = &routerd_main;
  vl_api_get_proc_info_t *mp =
    vl_msg_api_alloc(sizeof(*mp));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(vl_msg_id);
  mp->client_index = rm->my_client_index;
  mp->context = htonl(msg_id);

  memcpy(mp->node_name, node_name, strlen(node_name));
  vl_msg_api_send_shmem(rm->vl_input_queue, (u8 *) &mp);
  return 0;
}

uint32_t
find_msg_id(const char* msg)
{
  api_main_t * am = &api_main;
  hash_pair_t *hp;
#define _HASH_FUNC_IMPL_INSIDE_IMPL_ { \
  char *key = (char *)hp->key; \
  int msg_name_len = strlen(key) - 9; \
  if (strlen(msg) == msg_name_len && \
    strncmp(msg, (char *)hp->key, msg_name_len) == 0) \
    return (u32)hp->value[0]; }
  hash_foreach_pair (hp,
      am->msg_index_by_name_and_crc,
      (_HASH_FUNC_IMPL_INSIDE_IMPL_));
}

int
ip_route_add_del(uint16_t msg_id, bool is_add,
    const struct prefix *route, const struct prefix *nexthop,
    uint32_t nh_ifindex)
{
#ifndef NO_VPP
  routerd_main_t *rm = &routerd_main;
  vl_api_ip_route_add_del_t *mp =
    vl_msg_api_alloc(sizeof(*mp));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(find_msg_id(IP_ROUTE_ADD_DEL));
  mp->client_index = rm->my_client_index;
  mp->context = htonl(msg_id);
  assert(route->afi == nexthop->afi);
  mp->is_add = is_add ? 1 : 0;
  mp->is_multipath = 0;

  vl_api_ip_route_t *route_ = &mp->route;
  route_->table_id = 0;
  route_->stats_index = 0;
  route_->prefix.address.af = (route->afi == AF_INET6) ? 1 : 0;
  memcpy(&route_->prefix.address.un, route->u.raw, route->afi == AF_INET6 ? 16 : 4);
  route_->prefix.len = route->plen;
  route_->n_paths = 1;
  for (size_t i=0; i<route_->n_paths; i++) {
    vl_api_fib_path_t *fib_path = &route_->paths[i];
    fib_path->sw_if_index = htonl(nh_ifindex);
    fib_path->table_id = 0;
    fib_path->rpf_id = 0;
    fib_path->weight = 0;
    fib_path->preference = 0;
    fib_path->type = 0;
    fib_path->flags = 0;
    fib_path->proto = 0;
    memcpy(&fib_path->nh.address,
     nexthop->u.raw, route->afi==AF_INET6 ? 16 : 4);
  }

  vl_msg_api_send_shmem(rm->vl_input_queue, (u8 *) &mp);
#endif
}

int
create_loopback(uint32_t msg_id)
{
  routerd_main_t *rm = &routerd_main;
  vl_api_create_loopback_t *mp = msg_alloc_zero(sizeof(*mp));

  mp->_vl_msg_id = ntohs(find_msg_id(CREATE_LOOPBACK));
  mp->context = htonl(msg_id);
  mp->client_index = rm->my_client_index;

  vl_msg_api_send_shmem(rm->vl_input_queue, (u8 *) &mp);
}

int
send_ping(uint32_t msg_id)
{
  routerd_main_t *rm = &routerd_main;
  vl_api_control_ping_t *mp = msg_alloc_zero(sizeof(vl_api_control_ping_t));

  mp->_vl_msg_id = ntohs(find_msg_id(CONTROL_PING));
  mp->client_index = rm->my_client_index;
  mp->context = htonl(msg_id);

  vl_msg_api_send_shmem(rm->vl_input_queue, (u8 *) &mp);
}

int
ip_route_dump(uint32_t msg_id, uint32_t table_id)
{
  routerd_main_t *xm = &routerd_main;
  vl_api_ip_route_dump_t *mp = msg_alloc_zero(sizeof(*mp));

  mp->_vl_msg_id   = clib_host_to_net_u16(find_msg_id(IP_ROUTE_DUMP));
  mp->client_index = xm->my_client_index;
  mp->context      = clib_host_to_net_u32(msg_id);
  mp->table.table_id = clib_host_to_net_u32(table_id);
  mp->table.is_ip6 = 0;

  vl_msg_api_send_shmem(xm->vl_input_queue, (u8 *) &mp);
}

int
dump_ifcs(uint32_t msg_id)
{
  routerd_main_t *jm = &routerd_main;
  vl_api_sw_interface_dump_t *mp = msg_alloc_zero(sizeof(*mp));

  mp->_vl_msg_id = ntohs(find_msg_id(SW_INTERFACE_DUMP));
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(msg_id);

  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
}

int
dump_ipaddrs(uint32_t msg_id, uint32_t ifindex, bool is_ipv6)
{
  routerd_main_t *jm = &routerd_main;
  vl_api_ip_address_dump_t *mp = msg_alloc_zero(sizeof(*mp));

  mp->_vl_msg_id = ntohs(find_msg_id(IP_ADDRESS_DUMP));
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(msg_id);
  mp->sw_if_index = htonl(ifindex);
  mp->is_ipv6 = is_ipv6 ? 1 : 0;

  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
}

int
tap_inject_dump(uint16_t msg_id)
{
#ifndef NO_VPP
  routerd_main_t *jm = &routerd_main;
  vl_api_tap_inject_dump_t *mp = msg_alloc_zero(sizeof(*mp));

  mp->_vl_msg_id = ntohs(find_msg_id(TAP_INJECT_DUMP));
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(msg_id);

  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
#endif
}

int
set_interface_flag(uint32_t msg_id, uint32_t ifindex, bool is_up)
{
  routerd_main_t *jm = &routerd_main;
  vl_api_sw_interface_set_flags_t *mp = msg_alloc_zero(sizeof(*mp));

  mp->_vl_msg_id = ntohs(find_msg_id(SW_INTERFACE_SET_FLAGS));
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(msg_id);
  mp->sw_if_index = htonl(ifindex);
  mp->admin_up_down = is_up ? 1 : 0;

  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
}

int
set_interface_addr(uint32_t msg_id,
    uint32_t ifindex, bool is_add, bool is_ipv6,
    const void *addr_buffer, size_t addr_len)
{
  routerd_main_t *jm = &routerd_main;
  vl_api_sw_interface_add_del_address_t *mp = msg_alloc_zero(sizeof(*mp));

  mp->_vl_msg_id = ntohs(find_msg_id(SW_INTERFACE_ADD_DEL_ADDRESS));
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(msg_id);
  mp->sw_if_index = htonl(ifindex);
  mp->is_add = is_add;
  mp->is_ipv6 = is_ipv6 ? 1 : 0;
  mp->address_length = addr_len;
  memcpy(mp->address, addr_buffer, mp->address_length);

  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
}

int
enable_disable_tap_inject(uint16_t msg_id, bool is_enable)
{
#ifndef NO_VPP
  routerd_main_t *jm = &routerd_main;
  vl_api_tap_inject_enable_disable_t *mp = msg_alloc_zero(sizeof(*mp));

  mp->_vl_msg_id = ntohs(find_msg_id(TAP_INJECT_ENABLE_DISABLE));
  mp->client_index = jm->my_client_index;
  mp->context = clib_host_to_net_u32(msg_id);
  mp->is_enable = is_enable;

  vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
#endif
}

int32_t
enable_disable_tap_inject_retval(void)
{
#ifndef NO_VPP
  void *msg = NULL;
  api_main_t *am = &api_main;
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 1)) {
    vl_api_tap_inject_enable_disable_reply_t *mp = msg;
    int32_t retval = ntohl(mp->retval);
    return retval;
  }
  return (int32_t)-1;
#endif
}

void
disconnect_from_vpp(void)
{
  vl_client_disconnect_from_vlib();
}

int
connect_to_vpp (const char *name, bool no_rx_pthread)
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

int32_t vpp_waitmsg_retval(void)
{
  void *msg = NULL;
  api_main_t *am = &api_main;
  while (!svm_queue_sub (am->vl_input_queue, (u8 *) & msg, SVM_Q_TIMEDWAIT, 1)) {
    vl_api_sw_interface_add_del_address_reply_t * mp =
      (vl_api_sw_interface_add_del_address_reply_t*)msg;
    int32_t retval = ntohl(mp->retval);
    return retval;
  }
  return -1;
}

void
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

