/*
 * Copyright (c) 2019 Hiroki Shirokura
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vnet/vnet.h>
#include <vnet/plugin/plugin.h>
#include <vnet/osi/osi.h>
#include <vnet/fib/fib_types.h>
#include <vnet/ethernet/arp_packet.h>

#include <cplane_netdev/cplane_netdev.h>
#include <vlibapi/api.h>
#include <vlibmemory/api.h>
#include <vpp/app/version.h>
#include <vppinfra/vec.h>
#include <vnet/unix/tuntap.h>
#include <vlib/unix/unix.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_tun.h>
#include <netinet/in.h>

#include <cplane_netdev/cplane_netdev_msg_enum.h>
#define vl_typedefs
#include <cplane_netdev/cplane_netdev_all_api_h.h>
#undef vl_typedefs
#define vl_endianfun
#include <cplane_netdev/cplane_netdev_all_api_h.h>
#undef vl_endianfun
#define vl_print(handle, ...) vlib_cli_output (handle, __VA_ARGS__)
#define vl_printfun
#include <cplane_netdev/cplane_netdev_all_api_h.h>
#undef vl_printfun
#define vl_api_version(n,v) static uint32_t api_version=(v);
#include <cplane_netdev/cplane_netdev_all_api_h.h>
#undef vl_api_version
#define REPLY_MSG_ID_BASE mmp->msg_id_base
#include <vlibapi/api_helper_macros.h>
#define vl_msg_name_crc_list
#include <cplane_netdev/cplane_netdev_all_api_h.h>
#undef vl_msg_name_crc_list

#define foreach_cplane_netdev_plugin_api_msg \
_(TAP_INJECT_ENABLE_DISABLE, tap_inject_enable_disable) \
_(TAP_INJECT_DUMP, tap_inject_dump) \
_(TAP_INJECT_DETAILS, tap_inject_details) \

#define MTU 1500
#define MTU_BUFFERS ((MTU + VLIB_BUFFER_DATA_SIZE - 1) / VLIB_BUFFER_DATA_SIZE)
#define NUM_BUFFERS_TO_ALLOC 32
#define VLIB_BUFFER_DATA_SIZE (2048)

enum {
  NEXT_NEIGHBOR_ARP,
  NEXT_NEIGHBOR_ICMP6,
};

vlib_node_registration_t tap_inject_tx_node;
vlib_node_registration_t tap_inject_rx_node;
vlib_node_registration_t tap_inject_neighbor_node;

static cplane_netdev_main_t *
cplane_netdev_get_main(void)
{
  static cplane_netdev_main_t cplane_netdev_main;
  return &cplane_netdev_main;
}

static int
tap_inject_is_enabled (void)
{
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  return !!(im->flags & TAP_INJECT_F_ENABLED);
}

static int
tap_inject_is_config_disabled (void)
{
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  return !!(im->flags & TAP_INJECT_F_CONFIG_DISABLE);
}

static void
tap_inject_disable (void)
{
  vlib_main_t * vm = vlib_get_main();
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  if (! tap_inject_is_enabled ())
    return;

  printf("%s: start\n", __func__);
  ethernet_register_input_type (vm, ETHERNET_TYPE_ARP, vlib_get_node_by_name(vm, "arp-input")->index);
  ip4_register_protocol (IP_PROTOCOL_ICMP, vlib_get_node_by_name(vm, "ip4-icmp-input")->index);
  ip4_unregister_protocol (IP_PROTOCOL_OSPF);
  ip4_unregister_protocol (IP_PROTOCOL_TCP);
  ip4_unregister_protocol (IP_PROTOCOL_UDP);

  printf("%s: done\n", __func__);
  im->flags &= ~TAP_INJECT_F_ENABLED;
}

static clib_error_t *
tap_inject_enable (void)
{
  vlib_main_t * vm = vlib_get_main ();
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  if (tap_inject_is_enabled ())
    return 0;

  printf("%s: start\n", __func__);
  ethernet_register_input_type (vm, ETHERNET_TYPE_ARP, im->neighbor_node_index);
  ip4_register_protocol (IP_PROTOCOL_ICMP, im->tx_node_index);
  ip4_register_protocol (IP_PROTOCOL_OSPF, im->tx_node_index);
  ip4_register_protocol (IP_PROTOCOL_TCP , im->tx_node_index);
  ip4_register_protocol (IP_PROTOCOL_UDP , im->tx_node_index);

  /* ip6_register_protocol (IP_PROTOCOL_ICMP6, im->neighbor_node_index); */
  /* ip6_register_protocol (IP_PROTOCOL_OSPF, im->tx_node_index); */
  /* ip6_register_protocol (IP_PROTOCOL_TCP, im->tx_node_index); */
  /* ip6_register_protocol (IP_PROTOCOL_UDP, im->tx_node_index); */
  /* osi_register_input_protocol (OSI_PROTOCOL_isis, im->tx_node_index); */

#if 0
  {
    dpo_id_t dpo = DPO_INVALID;
    const mfib_prefix_t pfx_224_0_0_0 = {
        .fp_len = 24,
        .fp_proto = FIB_PROTOCOL_IP4,
        .fp_grp_addr = { .ip4.as_uint32_t = clib_host_to_net_uint32_t(0xe0000000), },
        .fp_src_addr = { .ip4.as_uint32_t = 0, },
    };

    dpo_set(&dpo, tap_inject_dpo_type, DPO_PROTO_IP4, ~0);
    index_t repi = replicate_create(1, DPO_PROTO_IP4);
    replicate_set_bucket(repi, 0, &dpo);

    mfib_table_entry_special_add(0,
         &pfx_224_0_0_0, MFIB_SOURCE_API,
         MFIB_ENTRY_FLAG_ACCEPT_ALL_ITF, repi);
    dpo_reset(&dpo);
  }
#endif

  printf("%s: done\n", __func__);
  im->flags |= TAP_INJECT_F_ENABLED;
  return 0;
}

static void
tap_inject_insert_tap (uint32_t sw_if_index, uint32_t tap_fd, uint32_t tap_if_index)
{
  /* printf("SLANKDEV: %s\n", __func__); */
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  vec_validate_init_empty (im->sw_if_index_to_tap_fd, sw_if_index, ~0);
  vec_validate_init_empty (im->sw_if_index_to_tap_if_index, sw_if_index, ~0);
  vec_validate_init_empty (im->tap_fd_to_sw_if_index, tap_fd, ~0);
  im->sw_if_index_to_tap_fd[sw_if_index] = tap_fd;
  im->sw_if_index_to_tap_if_index[sw_if_index] = tap_if_index;
  im->tap_fd_to_sw_if_index[tap_fd] = sw_if_index;
  hash_set (im->tap_if_index_to_sw_if_index, tap_if_index, sw_if_index);
}

static clib_error_t *
tap_inject_tap_read (clib_file_t * f)
{
  /* printf("SLANKDEV: %s 1\n", __func__); */
  vlib_main_t * vm = vlib_get_main ();
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  vec_add1 (im->rx_file_descriptors, f->file_descriptor);
  vlib_node_set_interrupt_pending (vm, im->rx_node_index);
  return 0;
}

static clib_error_t *
tap_inject_tap_connect (vnet_hw_interface_t * hw)
{
  /* printf("SLANKDEV: %s\n", __func__); */
  vnet_main_t * vnet_main = vnet_get_main ();
  vnet_sw_interface_t * sw = vnet_get_sw_interface (vnet_main, hw->hw_if_index);
  static const int one = 1;
  int fd;
  struct ifreq ifr;
  clib_file_t template;
  uint8_t * name;

  memset (&ifr, 0, sizeof (ifr));
  memset (&template, 0, sizeof (template));
  ASSERT (hw->hw_if_index == sw->sw_if_index);

  /* Create the tap. */
  uint32_t tap_fd = open ("/dev/net/tun", O_RDWR);
  if ((int)tap_fd < 0)
    return clib_error_return (0, "failed to open tun device");

#define TAP_INJECT_TAP_BASE_NAME "neo"
  name = format (0, TAP_INJECT_TAP_BASE_NAME "%u%c", hw->hw_instance, 0);
  strncpy (ifr.ifr_name, (char *) name, sizeof (ifr.ifr_name) - 1);
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

  if (ioctl (tap_fd, TUNSETIFF, (void *)&ifr) < 0) {
    close (tap_fd);
    return 0;
    /* return clib_error_return (0, "failed to create tap"); */
  }

  if (ioctl (tap_fd, FIONBIO, &one) < 0) {
    close (tap_fd);
    return clib_error_return (0, "failed to set tap to non-blocking io");
  }

  /* Open a socket to configure the device. */
  fd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
  if (fd < 0) {
    close (tap_fd);
    return clib_error_return (0, "failed to configure tap");
  }

  if (hw->hw_address)
    clib_memcpy (ifr.ifr_hwaddr.sa_data, hw->hw_address, 6);

  ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

  /* Set the hardware address. */
  if (ioctl (fd, SIOCSIFHWADDR, &ifr) < 0) {
    close (tap_fd);
    close (fd);
    return clib_error_return (0, "failed to set tap hardware address");
  }

  /* Get the tap if index. */
  if (ioctl (fd, SIOCGIFINDEX, &ifr) < 0) {
    close (tap_fd);
    close (fd);
    return clib_error_return (0, "failed to procure tap if index");
  }

  close (fd);
  template.read_function = tap_inject_tap_read;
  template.file_descriptor = tap_fd;
  clib_file_add (&file_main, &template);
  tap_inject_insert_tap (sw->sw_if_index, tap_fd, ifr.ifr_ifindex);
  return 0;
}

static void
tap_inject_delete_tap (uint32_t sw_if_index)
{
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  uint32_t tap_fd = im->sw_if_index_to_tap_fd[sw_if_index];
  uint32_t tap_if_index = im->sw_if_index_to_tap_if_index[sw_if_index];
  im->sw_if_index_to_tap_if_index[sw_if_index] = ~0;
  im->sw_if_index_to_tap_fd[sw_if_index] = ~0;
  im->tap_fd_to_sw_if_index[tap_fd] = ~0;
  hash_unset (im->tap_if_index_to_sw_if_index, tap_if_index);
}

static uint32_t
tap_inject_lookup_tap_fd (uint32_t sw_if_index)
{
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  vec_validate_init_empty (im->sw_if_index_to_tap_fd, sw_if_index, ~0);
  return im->sw_if_index_to_tap_fd[sw_if_index];
}

static clib_error_t *
tap_inject_tap_disconnect (uint32_t sw_if_index)
{
  uint32_t tap_fd = tap_inject_lookup_tap_fd (sw_if_index);
  if (tap_fd == ~0)
    return clib_error_return (0, "failed to disconnect tap");

  tap_inject_delete_tap (sw_if_index);
  close (tap_fd);
  return 0;
}

static uint64_t
tap_inject_iface_isr (vlib_main_t * vm, vlib_node_runtime_t * node,
                      vlib_frame_t * f)
{
  cplane_netdev_main_t * im = cplane_netdev_get_main ();

  clib_error_t *err;
  uint32_t *hw_if_index;
  vec_foreach (hw_if_index, im->interfaces_to_enable) {
    printf("%s: hw_if_index: %u\n", __func__, *hw_if_index);
    vnet_hw_interface_t *hw = vnet_get_hw_interface (vnet_get_main (), *hw_if_index);
    if (hw->hw_class_index == ethernet_hw_interface_class.index) {
      err = tap_inject_tap_connect (hw);
      if (err) {
        printf("%s: err: %s(%ld)\n", __func__, err->what, err->code);
        break;
      }
    }
  }

  vec_foreach (hw_if_index, im->interfaces_to_disable)
    tap_inject_tap_disconnect (*hw_if_index);

  vec_free (im->interfaces_to_enable);
  vec_free (im->interfaces_to_disable);
  return err ? -1 : 0;
}

static clib_error_t *
tap_inject_enable_disable_all_interfaces (int enable)
{
  vnet_main_t * vnet_main = vnet_get_main ();
  cplane_netdev_main_t * im = cplane_netdev_get_main ();

  /* configure ip,ethernet nodes table */
  if (enable)
    tap_inject_enable ();
  else
    tap_inject_disable ();

  /* Collect all the interface indices. */
  vnet_hw_interface_t * interfaces = vnet_main->interface_main.hw_interfaces;
  uint32_t ** indices = enable ? &im->interfaces_to_enable : &im->interfaces_to_disable;
  vnet_hw_interface_t * hw;
  pool_foreach (hw, interfaces, vec_add1 (*indices, hw - interfaces));

  if (tap_inject_iface_isr (vlib_get_main (), 0, 0))
    return clib_error_return (0, "tap-inject interface add del isr failed");
  return 0;
}

static void
  vl_api_tap_inject_enable_disable_t_handler
  (vl_api_tap_inject_enable_disable_t * mp)
{
  printf("[tap-inject API] %s\n", __func__);
  vl_api_tap_inject_enable_disable_reply_t * rmp;
  cplane_netdev_main_t * mmp = cplane_netdev_get_main();

  int rv = 0;
  clib_error_t * err = tap_inject_enable_disable_all_interfaces (mp->is_enable ? 1 : 0);
  if (err) {
    tap_inject_enable_disable_all_interfaces (0);
    rv = -1;
  }

  REPLY_MACRO (VL_API_TAP_INJECT_ENABLE_DISABLE_REPLY);
}

static void
send_tap_inject_details(cplane_netdev_main_t *am,
    vl_api_registration_t * rp, uint32_t context,
    uint32_t vpp_index, uint32_t kern_index)
{
  vl_api_tap_inject_details_t *mp =
    vl_msg_api_alloc (sizeof (*mp));
  clib_memset (mp, 0, sizeof (*mp));
  mp->_vl_msg_id = ntohs(VL_API_TAP_INJECT_DETAILS);
  mp->sw_if_index = htonl(vpp_index);
  mp->kernel_if_index = htonl(kern_index);
  mp->context = context;

  vl_api_send_msg(rp, (uint8_t*)mp);
}

static void
  vl_api_tap_inject_dump_t_handler
  (vl_api_tap_inject_dump_t * mp)
{
  cplane_netdev_main_t *mm = cplane_netdev_get_main();
  vl_api_registration_t *rp =
    vl_api_client_index_to_registration (mp->client_index);
  printf("[tap-inject API] %s\n", __func__);

  uint32_t if_index = 3;
  uint32_t kern_index, vpp_index;
  vnet_main_t *vnet_main = vnet_get_main ();
  cplane_netdev_main_t *tm = cplane_netdev_get_main ();
  hash_foreach (kern_index, vpp_index, tm->tap_if_index_to_sw_if_index, {
      send_tap_inject_details(mm, rp, mp->context, vpp_index, kern_index);
      printf("%s: v/k: %u/%u\n", __func__, vpp_index, kern_index);
    }
  );
}

static void
  vl_api_tap_inject_details_t_handler
  (vl_api_tap_inject_details_t * mp)
{
  printf("[tap-inject API] %s\n", __func__);
}

/* Set up the API message handling tables */
static clib_error_t *
cplane_netdev_plugin_api_hookup(vlib_main_t *vm)
{
  cplane_netdev_main_t * mmp = cplane_netdev_get_main();
#define _(N,n)                                                  \
    vl_msg_api_set_handlers((VL_API_##N + mmp->msg_id_base),     \
                           #n,					\
                           vl_api_##n##_t_handler,              \
                           vl_noop_handler,                     \
                           vl_api_##n##_t_endian,               \
                           vl_api_##n##_t_print,                \
                           sizeof(vl_api_##n##_t), 1);
    foreach_cplane_netdev_plugin_api_msg;
#undef _
    return 0;
}

static void
setup_message_id_table(cplane_netdev_main_t *mmp, api_main_t *am)
{
#define _(id,n,crc)   vl_msg_api_add_msg_name_crc (am, #n "_" #crc, id + mmp->msg_id_base);
  foreach_vl_msg_name_crc_cplane_netdev ;
#undef _
}

static clib_error_t *
cplane_netdev_init (vlib_main_t *vm)
{
  cplane_netdev_main_t * mmp = cplane_netdev_get_main();
  mmp->vlib_main = vm;
  mmp->vnet_main = vnet_get_main();
  mmp->tx_node_index = tap_inject_tx_node.index;
  mmp->rx_node_index = tap_inject_rx_node.index;
  mmp->neighbor_node_index = tap_inject_neighbor_node.index;

  uint8_t * name = format (0, "cplane_netdev_%08x%c", api_version, 0);
  mmp->msg_id_base = vl_msg_api_get_msg_ids
      ((char *) name, VL_MSG_FIRST_AVAILABLE);

  clib_error_t *error = cplane_netdev_plugin_api_hookup (vm);
  setup_message_id_table (mmp, &api_main);
  vec_free(name);
  vec_alloc (mmp->rx_buffers, NUM_BUFFERS_TO_ALLOC);
  vec_reset_length (mmp->rx_buffers);
  return error;
}

static clib_error_t*
create_cplane_netdev(vlib_main_t *vm, uint32_t ifindex, const char *name)
{
  vlib_cli_output (vm, "%s id %u name %s\n", __func__, ifindex, name);

  vnet_hw_interface_t *hw = vnet_get_hw_interface (vnet_get_main(), ifindex);
  if (! hw)
    return clib_error_return(0, "invalid ifindex (%u)", ifindex);

  if (hw->hw_class_index == ethernet_hw_interface_class.index) {
    clib_error_t *err = tap_inject_tap_connect (hw);
    if (err)
      return clib_error_return(0, "%s: err: %s(%d)\n", __func__, err->what, err->code);
  }

  return 0;
}

static clib_error_t *
create_cplane_netdev_command_fn (vlib_main_t * vm,
    unformat_input_t * input, vlib_cli_command_t * cmd)
{
  unformat_input_t _line_input, *line_input = &_line_input;
  vlib_cli_output (vm, "%s\n", __func__);
  uint32_t id = ~0;
  char *name;

  if (unformat_user (input, unformat_line_input, line_input)) {
    while (unformat_check_input (line_input) != UNFORMAT_END_OF_INPUT) {
      if (unformat (line_input, "id %u", &id)) ;
      else if (unformat (line_input, "name %s", &name)) ;

      else {
        unformat_free (line_input);
        return clib_error_return (0, "unknown input `%U'",
          format_unformat_error, input);
      }

    }
    unformat_free (line_input);
  }

  clib_error_t *err = create_cplane_netdev(vm, id, name);
  vec_free(name);
  return err;
}

static clib_error_t *
enable_disable_tap_inject_cmd_fn (vlib_main_t * vm, unformat_input_t * input,
                 vlib_cli_command_t * cmd)
{
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  if (cmd->function_arg) {
    if (tap_inject_is_config_disabled ())
      return clib_error_return (0, "tap-inject is disabled in config, thus cannot be enabled.");

    /* Enable */
    clib_error_t * err = tap_inject_enable_disable_all_interfaces (1);
    if (err) {
      tap_inject_enable_disable_all_interfaces (0);
      return err;
    }

    im->flags |= TAP_INJECT_F_CONFIG_ENABLE;
  } else {
    /* Disable */
    tap_inject_enable_disable_all_interfaces (0);
    im->flags &= ~TAP_INJECT_F_CONFIG_ENABLE;
  }

  return 0;
}

static uint8_t *
format_tap_inject_tap_name (uint8_t * s, va_list * args)
{
  int fd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
  if (fd < 0)
    return 0;

  struct ifreq ifr;
  memset (&ifr, 0, sizeof (ifr));
  ifr.ifr_ifindex = va_arg (*args, uint32_t);
  if (ioctl (fd, SIOCGIFNAME, &ifr) < 0) {
    close (fd);
    return 0;
  }

  close (fd);
  return format (s, "%s", ifr.ifr_name);
}

static clib_error_t *
show_tap_inject (vlib_main_t * vm, unformat_input_t * input,
                 vlib_cli_command_t * cmd)
{
  if (tap_inject_is_config_disabled ()) {
    vlib_cli_output (vm, "tap-inject is disabled in config.\n");
    return 0;
  }

  if (!tap_inject_is_enabled ()) {
    vlib_cli_output (vm, "tap-inject is not enabled.\n");
    return 0;
  }

  uint32_t k, v;
  vnet_main_t * vnet_main = vnet_get_main ();
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  hash_foreach (k, v, im->tap_if_index_to_sw_if_index,
    /* routine */ {
      vnet_sw_interface_t *iface = vnet_get_sw_interface(vnet_main, v);
      vlib_cli_output (vm, "%U -> %U",
              format_vnet_sw_interface_name, vnet_main, iface,
              format_tap_inject_tap_name, k);
      vlib_cli_output(vm, "  vpp%u %U \n", v, format_vnet_sw_interface_name, vnet_main, iface);
      vlib_cli_output(vm, "  kern%u %U \n", k, format_tap_inject_tap_name, k);
    }
  );
  return 0;
}

static inline void
tap_inject_tap_send_buffer (int fd, vlib_buffer_t * b)
{
  struct iovec iov;
  iov.iov_base = vlib_buffer_get_current (b);
  iov.iov_len = b->current_length;

  ssize_t n_bytes = writev (fd, &iov, 1);
  if (n_bytes < 0)
    clib_warning ("writev failed");
  else if (n_bytes < b->current_length || b->flags & VLIB_BUFFER_NEXT_PRESENT)
    clib_warning ("buffer truncated");
}

static uint64_t
tap_inject_tx (vlib_main_t * vm, vlib_node_runtime_t * node, vlib_frame_t * f)
{
  /* printf("SLANKDEV %s\n", __func__); */
  uint32_t* pkts = vlib_frame_vector_args (f);
  for (uint32_t i = 0; i < f->n_vectors; ++i) {
    vlib_buffer_t *b = vlib_get_buffer (vm, pkts[i]);
    uint32_t sw_ifindex = vnet_buffer (b)->sw_if_index[VLIB_RX];
    uint32_t fd = tap_inject_lookup_tap_fd (sw_ifindex);
    if (fd == ~0) {
      /* printf("SLANKDEV %s: sw-idx=%u, not found\n", __func__, sw_ifindex); */
      continue;
    }

    /* Re-wind the buffer to the start of the Ethernet header. */
    vlib_buffer_advance (b, -b->current_data);
    tap_inject_tap_send_buffer (fd, b);
  }

  vlib_buffer_free (vm, pkts, f->n_vectors);
  return f->n_vectors;
}

static uint64_t
tap_inject_neighbor (vlib_main_t * vm, vlib_node_runtime_t * node, vlib_frame_t * f)
{
  /* printf("SLANKDEV %s\n", __func__); */
  uint32_t* pkts = vlib_frame_vector_args (f);
  for (uint32_t i = 0; i < f->n_vectors; ++i) {

    uint32_t bi = pkts[i];
    vlib_buffer_t *b = vlib_get_buffer (vm, bi);
    uint32_t sw_ifindex = vnet_buffer (b)->sw_if_index[VLIB_RX];
    uint32_t fd = tap_inject_lookup_tap_fd (sw_ifindex);
    if (fd == ~0) {
      /* printf("SLANKDEV %s: sw-idx=%u, not found\n", __func__, sw_ifindex); */
      continue;
    }

    /* Re-wind the buffer to the start of the Ethernet header. */
    vlib_buffer_advance (b, -b->current_data);
    tap_inject_tap_send_buffer (fd, b);

    ethernet_header_t * eth = vlib_buffer_get_current (b);
    const uint16_t ether_type = htons(eth->type);
    assert (ether_type == ETHERNET_TYPE_ARP ||
            ether_type == ETHERNET_TYPE_IP6);

    uint32_t next = ~0;
    if (ether_type == ETHERNET_TYPE_ARP) {
        ethernet_arp_header_t * arp = (void *)(eth + 1);
        if (arp->opcode == ntohs (ETHERNET_ARP_OPCODE_reply))
          next = NEXT_NEIGHBOR_ARP;
    } else if (ether_type == ETHERNET_TYPE_IP6) {
      ip6_header_t * ip = (void *)(eth + 1);
      icmp46_header_t * icmp = (void *)(ip + 1);
      assert (ip->protocol == IP_PROTOCOL_ICMP6);
      if (icmp->type == ICMP6_neighbor_advertisement)
        next = NEXT_NEIGHBOR_ICMP6;
    }

    if (next == ~0) {
      vlib_buffer_free (vm, &bi, 1);
      continue;
    }

    /* ARP and ICMP6 expect to start processing after the Ethernet header. */
    uint32_t next_index = node->cached_next_index;
    uint32_t n_left, *to_next;
    vlib_buffer_advance (b, sizeof (ethernet_header_t));
    vlib_get_next_frame (vm, node, next_index, to_next, n_left);
    *(to_next++) = bi;
    --n_left;
    vlib_validate_buffer_enqueue_x1 (vm, node, next_index, to_next, n_left, bi, next);
    vlib_put_next_frame (vm, node, next_index, n_left);
  }
  return f->n_vectors;
}


static uint32_t
tap_inject_lookup_sw_if_index_from_tap_fd (uint32_t tap_fd)
{
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  vec_validate_init_empty (im->tap_fd_to_sw_if_index, tap_fd, ~0);
  return im->tap_fd_to_sw_if_index[tap_fd];
}

static inline uint64_t
tap_rx (vlib_main_t * vm, vlib_node_runtime_t * node, vlib_frame_t * f, int fd)
{
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  uint32_t sw_if_index = tap_inject_lookup_sw_if_index_from_tap_fd (fd);
  if (sw_if_index == ~0)
    return 0;

  /* Allocate buffers in bulk when there are less than enough to rx an MTU. */
  if (vec_len (im->rx_buffers) < MTU_BUFFERS) {
    uint32_t len = vec_len (im->rx_buffers);
    uint8_t buffer_pool_index = vlib_buffer_pool_get_default_for_numa(vm, 0);
    len = vlib_buffer_alloc_from_pool (vm,
                  &im->rx_buffers[len], NUM_BUFFERS_TO_ALLOC,
                  buffer_pool_index);

    _vec_len (im->rx_buffers) += len;
    if (vec_len (im->rx_buffers) < MTU_BUFFERS) {
      clib_warning ("failed to allocate buffers");
      return 0;
    }
  }

  /* Fill buffers from the end of the list to make it easier to resize. */
  struct iovec iov[MTU_BUFFERS];
  uint32_t bi[MTU_BUFFERS];
  for (uint32_t i = 0, j = vec_len (im->rx_buffers) - 1; i < MTU_BUFFERS; ++i, --j) {
    vlib_buffer_t * b;
    bi[i] = im->rx_buffers[j];
    b = vlib_get_buffer (vm, bi[i]);
    iov[i].iov_base = b->data;
    iov[i].iov_len = VLIB_BUFFER_DATA_SIZE;
  }

  ssize_t n_bytes = readv (fd, iov, MTU_BUFFERS);
  if (n_bytes < 0) {
    clib_warning ("readv failed");
    return 0;
  }

  vlib_buffer_t * b = vlib_get_buffer (vm, bi[0]);
  vnet_buffer (b)->sw_if_index[VLIB_RX] = sw_if_index;
  vnet_buffer (b)->sw_if_index[VLIB_TX] = sw_if_index;
  ssize_t n_bytes_left = n_bytes - VLIB_BUFFER_DATA_SIZE;
  if (n_bytes_left > 0) {
    b->total_length_not_including_first_buffer = n_bytes_left;
    b->flags |= VLIB_BUFFER_TOTAL_LENGTH_VALID;
  }

  uint32_t i;
  b->current_length = n_bytes;
  for (i = 1; n_bytes_left > 0; ++i, n_bytes_left -= VLIB_BUFFER_DATA_SIZE) {
    vlib_buffer_t * b = vlib_get_buffer (vm, bi[i - 1]);
    b->current_length = VLIB_BUFFER_DATA_SIZE;
    b->flags |= VLIB_BUFFER_NEXT_PRESENT;
    b->next_buffer = bi[i];
    b = vlib_get_buffer (vm, bi[i]);
    b->current_length = n_bytes_left;
  }

  _vec_len (im->rx_buffers) -= i;
  /* Get the packet to the output node. */
  {
    vnet_hw_interface_t * hw;
    vlib_frame_t * new_frame;
    uint32_t * to_next;

    hw = vnet_get_hw_interface (vnet_get_main (), sw_if_index);
    new_frame = vlib_get_frame_to_node (vm, hw->output_node_index);
    to_next = vlib_frame_vector_args (new_frame);
    to_next[0] = bi[0];
    new_frame->n_vectors = 1;
    vlib_put_frame_to_node (vm, hw->output_node_index, new_frame);
  }

  return 1;
}

static uint64_t
tap_inject_rx (vlib_main_t * vm, vlib_node_runtime_t * node, vlib_frame_t * f)
{
  cplane_netdev_main_t * im = cplane_netdev_get_main ();
  uint64_t count = 0;
  uint32_t * fd;
  vec_foreach (fd, im->rx_file_descriptors) {
    if (tap_rx (vm, node, f, *fd) != 1) {
      clib_warning ("rx failed");
      count = 0;
      break;
    }
    ++count;
  }
  vec_free (im->rx_file_descriptors);
  return count;
}

VLIB_INIT_FUNCTION (cplane_netdev_init);

VNET_FEATURE_INIT (cplane_netdev, static) =
{
  .arc_name = "device-input",
  .node_name = "cplane_netdev",
  .runs_before = VNET_FEATURES ("ethernet-input"),
};

VLIB_PLUGIN_REGISTER () =
{
  .version = VPP_BUILD_VER,
  .description = "cplane_netdev plugin description goes here",
};

VLIB_CLI_COMMAND (create_cplane_netdev_command, static) = {
  .path = "create cplane-netdev",
  .short_help ="create cplane-netdev {id <if-id>} [name <name>]",
  .function = create_cplane_netdev_command_fn,
};

VLIB_CLI_COMMAND (enable_tap_inject_cmd, static) = {
  .path = "enable tap-inject",
  .short_help ="enable tap-inject",
  .function = enable_disable_tap_inject_cmd_fn,
  .function_arg = 1,
};

VLIB_CLI_COMMAND (disable_tap_inject_cmd, static) = {
  .path = "disable tap-inject",
  .short_help ="disable tap-inject",
  .function = enable_disable_tap_inject_cmd_fn,
  .function_arg = 0,
};

VLIB_CLI_COMMAND (show_tap_inject_cmd, static) = {
  .path = "show tap-inject",
  .short_help = "show tap-inject",
  .function = show_tap_inject,
};

VLIB_REGISTER_NODE (tap_inject_tx_node) = {
  .function = tap_inject_tx,
  .name = "cplane_netdev-tap-inject-tx",
  .vector_size = sizeof (uint32_t),
  .type = VLIB_NODE_TYPE_INTERNAL,
};

VLIB_REGISTER_NODE (tap_inject_rx_node) = {
  .function = tap_inject_rx,
  .name = "cplane_netdev-tap-inject-rx",
  .vector_size = sizeof (uint32_t),
  .type = VLIB_NODE_TYPE_INPUT,
  .state = VLIB_NODE_STATE_INTERRUPT,
};

VLIB_REGISTER_NODE (tap_inject_neighbor_node) = {
  .function = tap_inject_neighbor,
  .name = "cplane_netdev-tap-inject-neigh",
  .vector_size = sizeof (uint32_t),
  .type = VLIB_NODE_TYPE_INTERNAL,
  .state = VLIB_NODE_STATE_INTERRUPT,
  .n_next_nodes = 2,
  .next_nodes = {
    [NEXT_NEIGHBOR_ARP] = "arp-input",
    [NEXT_NEIGHBOR_ICMP6] = "icmp6-neighbor-solicitation",
  },
};

