#ifndef _VPP_H_
#define _VPP_H_

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define CONTROL_PING "control_ping"
#define CONTROL_PING_REPLY "control_ping_reply"
#define SW_INTERFACE_DUMP "sw_interface_dump"
#define SW_INTERFACE_DETAILS "sw_interface_details"
#define SW_INTERFACE_SET_FLAGS "sw_interface_set_flags"
#define SW_INTERFACE_SET_FLAGS_REPLY "sw_interface_set_flags_reply"
#define SW_INTERFACE_ADD_DEL_ADDRESS "sw_interface_add_del_address"
#define SW_INTERFACE_ADD_DEL_ADDRESS_REPLY "sw_interface_add_del_address_reply"
#define IP_ADDRESS_DUMP "ip_address_dump"
#define IP_ADDRESS_DETAILS "ip_address_details"
#define CREATE_LOOPBACK "create_loopback"
#define CREATE_LOOPBACK_REPLY "create_loopback_reply"
#define IP_ROUTE_ADD_DEL "ip_route_add_del"
#define IP_ROUTE_ADD_DEL_REPLY "ip_route_add_del_reply"
#define TAP_INJECT_ENABLE_DISABLE "tap_inject_enable_disable"
#define TAP_INJECT_ENABLE_DISABLE_REPLAY "tap_inject_enable_disable_replay"
#define TAP_INJECT_DUMP "tap_inject_dump"
#define TAP_INJECT_DETAIL "tap_inject_detail"
#define GET_NODE_INFO "get_node_info"
#define GET_NODE_INFO_REPLY "get_node_info_reply"
#define GET_PROC_INFO "get_proc_info"
#define GET_PROC_INFO_REPLY "get_proc_info_reply"
#define IP_ROUTE_DUMP "ip_route_dump"
#define IP_ROUTE_DETAILS "ip_route_details"
#define SR_LOCALSID_ADD_DEL "sr_localsid_add_del"
#define SR_LOCALSID_ADD_DEL_REPLY "sr_localsid_add_del_REPLY"

#ifdef __cplusplus
extern "C" {
#endif

struct prefix {
  int afi;
  size_t plen;
  union {
    uint8_t raw[16];
    struct in6_addr in6;
    struct in_addr in4;
  } u;
};

typedef struct {
  void *vl_input_queue; /* unix_shared_memory_queue_t* */
  uint32_t my_client_index;
} routerd_main_t;

extern routerd_main_t routerd_main;

/*
 * Binary API wrapper
 */
int create_loopback(uint32_t msg_id);
int send_ping(uint32_t msg_id);
int dump_ifcs(uint32_t msg_id);
int ip_route_dump(uint32_t msg_id, uint32_t table_id);
int dump_ipaddrs(uint32_t msg_id, uint32_t ifindex, bool is_ipv6);
int set_interface_flag(uint32_t msg_id, uint32_t ifindex, bool is_up);
int set_interface_addr(uint32_t msg_id, uint32_t ifindex, bool is_add, bool is_ipv6, const void *addr_buffer, size_t addr_len);
int enable_disable_tap_inject(uint16_t msg_id, bool is_enable);
int32_t enable_disable_tap_inject_retval(void);
int tap_inject_dump(uint16_t msg_id);
int ip_route_add_del(uint16_t msg_id, bool is_add, const struct prefix *route, const struct prefix *nexthop, uint32_t nh_ifindex);
int ip6_route_srv6_end_dx4_add(uint16_t msg_id);
int get_node_info(uint16_t msg_id, const char *node_name);
int get_proc_info(uint16_t msg_id, const char *node_name);

/*
 * Helper
 */
const char* vpp_node_type_str(uint8_t num);
const char* vpp_proc_flags_to_state(uint16_t flags);
int connect_to_vpp (const char *name, bool no_rx_pthread);
void disconnect_from_vpp(void);
uint32_t find_msg_id(const char* msg);
void parse_prefix_str(const char *str, int afi, struct prefix *pref);

int32_t vpp_waitmsg_retval(void);

#ifdef __cplusplus
}
#endif
#endif /* _VPP_H_ */
