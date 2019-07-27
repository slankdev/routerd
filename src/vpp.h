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

#define CONTROL_PING_MESSAGE "control_ping"
#define CONTROL_PING_REPLY_MESSAGE "control_ping_reply"
#define DUMP_IFC_MESSAGE "sw_interface_dump"
#define IFC_DETAIL_MESSAGE "sw_interface_details"
#define SET_IFC_FLAGS "sw_interface_set_flags"
#define SET_IFC_FLAGS_REPLY "sw_interface_set_flags_reply"
#define SET_IFC_ADDR "sw_interface_add_del_address"
#define SET_IFC_ADDR_REPLY "sw_interface_add_del_address_reply"
#define DUMP_IP_ADDR_MESSAGE "ip_address_dump"
#define IP_ADDR_DETAILS_DETAIL_MESSAGE "ip_address_details"
#define CRT_LOOPBACK "create_loopback"
#define CRT_LOOPBACK_REPLY "create_loopback_reply"
#define IP_ADDDEL_ROUTE "ip_add_del_route"
#define IP_ADDDEL_ROUTE_REPLY "ip_add_del_route_reply"
#define ENABLE_DISABLE_TAPINJECT "tap_inject_enable_disable"
#define ENABLE_DISABLE_TAPINJECT_REPLAY "tap_inject_enable_disable"
#define TAPINJECT_DUMP_MESSAGE "tap_inject_dump"
#define TAPINJECT_DETAIL_MESSAGE "tap_inject_detail"
#define GET_NODE_INFO "get_node_info"
#define GET_NODE_INFO_REPLY "get_node_info_reply"
#define GET_PROC_INFO "get_proc_info"
#define GET_PROC_INFO_REPLY "get_proc_info_reply"

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
int create_loopback(uint16_t vl_msg_id, uint16_t msg_id);
int send_ping(uint16_t vl_msg_id, uint16_t msg_id);
int dump_ifcs(uint32_t vl_msg_id, uint32_t message_id);
int dump_ipaddrs(uint32_t vl_msg_id, uint32_t message_id, uint32_t ifindex, bool is_ipv6);
int set_interface_flag(uint32_t vl_msg_id, uint32_t message_id, uint32_t ifindex, bool is_up);
int set_interface_addr(uint32_t vl_msg_id, uint32_t msg_id,
    uint32_t ifindex, bool is_add, bool is_ipv6,
    const void *addr_buffer, size_t addr_len);
int enable_disable_tap_inject(uint16_t vl_msg_id, uint16_t msg_id, bool is_enable);
int32_t enable_disable_tap_inject_retval(void);
int tap_inject_dump(uint16_t vl_msg_id, uint16_t msg_id);
int ip_add_del_route(uint16_t vl_msg_id, uint16_t msg_id,
    bool is_add, const struct prefix *route,
    const struct prefix *nexthop, uint32_t nh_ifindex);
int get_node_info(uint16_t vl_msg_id, uint16_t msg_id, const char *node_name);
int get_proc_info(uint16_t vl_msg_id, uint16_t msg_id, const char *node_name);

/*
 * Helper
 */
const char* vpp_node_type_str(uint8_t num);
const char* vpp_proc_flags_to_state(uint16_t flags);
int connect_to_vpp (const char *name, bool no_rx_pthread);
void disconnect_from_vpp(void);
uint32_t find_msg_id(const char* msg);

int32_t vpp_waitmsg_retval(void);

#ifdef __cplusplus
}
#endif
#endif /* _VPP_H_ */
