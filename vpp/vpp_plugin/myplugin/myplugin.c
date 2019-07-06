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

#include <myplugin/myplugin.h>
#include <vlibapi/api.h>
#include <vlibmemory/api.h>
#include <vpp/app/version.h>
#include <vppinfra/vec.h>
#include <vnet/unix/tuntap.h>
#include <vlib/unix/unix.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

/* define message IDs */
#include <myplugin/myplugin_msg_enum.h>

/* define message structures */
#define vl_typedefs
#include <myplugin/myplugin_all_api_h.h>
#undef vl_typedefs

/* define generated endian-swappers */
#define vl_endianfun
#include <myplugin/myplugin_all_api_h.h>
#undef vl_endianfun

/* instantiate all the print functions we know about */
#define vl_print(handle, ...) vlib_cli_output (handle, __VA_ARGS__)
#define vl_printfun
#include <myplugin/myplugin_all_api_h.h>
#undef vl_printfun

/* Get the API version number */
#define vl_api_version(n,v) static u32 api_version=(v);
#include <myplugin/myplugin_all_api_h.h>
#undef vl_api_version

#define REPLY_MSG_ID_BASE mmp->msg_id_base
#include <vlibapi/api_helper_macros.h>

#define vl_msg_name_crc_list
#include <myplugin/myplugin_all_api_h.h>
#undef vl_msg_name_crc_list

#define tap_inject_main_t myplugin_main_t
#define tap_inject_get_main myplugin_get_main

vlib_node_registration_t tap_inject_tx_node;
vlib_node_registration_t tap_inject_rx_node;
myplugin_main_t myplugin_main;
myplugin_main_t *myplugin_get_main(void) { return &myplugin_main; }

/* List of message types that this plugin understands */

#define foreach_myplugin_plugin_api_msg                           \
_(MYPLUGIN_ENABLE_DISABLE, myplugin_enable_disable)

#define MTU 1500
#define MTU_BUFFERS ((MTU + VLIB_BUFFER_DATA_SIZE - 1) / VLIB_BUFFER_DATA_SIZE)
#define NUM_BUFFERS_TO_ALLOC 32
#define VLIB_BUFFER_DATA_SIZE (2048)

/* Action function shared between message handler and debug CLI */
static void
tap_inject_disable (void)
{
  myplugin_main_t * im = myplugin_get_main ();
  im->flags &= ~TAP_INJECT_F_ENABLED;
  clib_warning ("tap-inject is not actually disabled.");
}

static clib_error_t *
tap_inject_enable (void)
{
  vlib_main_t * vm = vlib_get_main ();
  myplugin_main_t * im = myplugin_get_main ();

  if (tap_inject_is_enabled ())
    return 0;

  /* ethernet_register_input_type (vm, ETHERNET_TYPE_ARP, im->neighbor_node_index); */
  ethernet_register_input_type (vm, ETHERNET_TYPE_ARP, im->tx_node_index);
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

    /* const mfib_prefix_t pfx_224_0_0_0 = { */
    /*     .fp_len = 24, */
    /*     .fp_proto = FIB_PROTOCOL_IP4, */
    /*     .fp_grp_addr = { */
    /*         .ip4.as_u32 = clib_host_to_net_u32(0xe0000000), */
    /*     }, */
    /*     .fp_src_addr = { */
    /*         .ip4.as_u32 = 0, */
    /*     }, */
    /* }; */

    dpo_set(&dpo, tap_inject_dpo_type, DPO_PROTO_IP4, ~0);
    index_t repi = replicate_create(1, DPO_PROTO_IP4);
    replicate_set_bucket(repi, 0, &dpo);

    /* mfib_table_entry_special_add(0, */
    /*                              &pfx_224_0_0_0, */
    /*                              MFIB_SOURCE_API, */
    /*                              MFIB_ENTRY_FLAG_ACCEPT_ALL_ITF, */
    /*                              repi); */

    dpo_reset(&dpo);
  }
#endif

  im->flags |= TAP_INJECT_F_ENABLED;
  return 0;
}

void
tap_inject_insert_tap (u32 sw_if_index, u32 tap_fd, u32 tap_if_index)
{
  printf("SLANKDEV: %s\n", __func__);
  tap_inject_main_t * im = tap_inject_get_main ();
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
  tap_inject_main_t * im = tap_inject_get_main ();
  vec_add1 (im->rx_file_descriptors, f->file_descriptor);
  vlib_node_set_interrupt_pending (vm, im->rx_node_index);
  return 0;
}

static clib_error_t *
tap_inject_tap_connect (vnet_hw_interface_t * hw)
{
  printf("SLANKDEV: %s\n", __func__);
  vnet_main_t * vnet_main = vnet_get_main ();
  vnet_sw_interface_t * sw = vnet_get_sw_interface (vnet_main, hw->hw_if_index);
  static const int one = 1;
  int fd;
  struct ifreq ifr;
  clib_file_t template;
  u8 * name;

  memset (&ifr, 0, sizeof (ifr));
  memset (&template, 0, sizeof (template));
  ASSERT (hw->hw_if_index == sw->sw_if_index);

  /* Create the tap. */
  u32 tap_fd = open ("/dev/net/tun", O_RDWR);
  if ((int)tap_fd < 0)
    return clib_error_return (0, "failed to open tun device");

#define TAP_INJECT_TAP_BASE_NAME "neo"
  name = format (0, TAP_INJECT_TAP_BASE_NAME "%u%c", hw->hw_instance, 0);
  strncpy (ifr.ifr_name, (char *) name, sizeof (ifr.ifr_name) - 1);
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

  if (ioctl (tap_fd, TUNSETIFF, (void *)&ifr) < 0) {
    close (tap_fd);
    return clib_error_return (0, "failed to create tap");
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
tap_inject_delete_tap (u32 sw_if_index)
{
  tap_inject_main_t * im = tap_inject_get_main ();
  u32 tap_fd = im->sw_if_index_to_tap_fd[sw_if_index];
  u32 tap_if_index = im->sw_if_index_to_tap_if_index[sw_if_index];
  im->sw_if_index_to_tap_if_index[sw_if_index] = ~0;
  im->sw_if_index_to_tap_fd[sw_if_index] = ~0;
  im->tap_fd_to_sw_if_index[tap_fd] = ~0;
  hash_unset (im->tap_if_index_to_sw_if_index, tap_if_index);
}

static u32
tap_inject_lookup_tap_fd (u32 sw_if_index)
{
  tap_inject_main_t * im = tap_inject_get_main ();
  vec_validate_init_empty (im->sw_if_index_to_tap_fd, sw_if_index, ~0);
  return im->sw_if_index_to_tap_fd[sw_if_index];
}

static clib_error_t *
tap_inject_tap_disconnect (u32 sw_if_index)
{
  u32 tap_fd = tap_inject_lookup_tap_fd (sw_if_index);
  if (tap_fd == ~0)
    return clib_error_return (0, "failed to disconnect tap");

  tap_inject_delete_tap (sw_if_index);
  close (tap_fd);
  return 0;
}

static uword
tap_inject_iface_isr (vlib_main_t * vm, vlib_node_runtime_t * node,
                      vlib_frame_t * f)
{
  clib_error_t * err;
  u32 * hw_if_index;
  myplugin_main_t * im = myplugin_get_main ();
  vec_foreach (hw_if_index, im->interfaces_to_enable) {
    vnet_hw_interface_t *hw = vnet_get_hw_interface (vnet_get_main (), *hw_if_index);
    if (hw->hw_class_index == ethernet_hw_interface_class.index) {
      err = tap_inject_tap_connect (hw);
      if (err)
        break;
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
  myplugin_main_t * im = myplugin_get_main ();

  if (enable)
    tap_inject_enable ();
  else
    tap_inject_disable ();

  /* Collect all the interface indices. */
  vnet_hw_interface_t * interfaces = vnet_main->interface_main.hw_interfaces;
  u32 ** indices = enable ? &im->interfaces_to_enable : &im->interfaces_to_disable;

  vnet_hw_interface_t * hw;
  pool_foreach (hw, interfaces, vec_add1 (*indices, hw - interfaces));

  if (tap_inject_iface_isr (vlib_get_main (), 0, 0))
    return clib_error_return (0, "tap-inject interface add del isr failed");

  return 0;
}

int
myplugin_enable_disable(myplugin_main_t * mmp, u32 sw_if_index,
                                   int enable_disable)
{
  printf("SLANKDEV: %s\n", __func__);
  return 0;
}

/* API message handler */
static void
vl_api_myplugin_enable_disable_t_handler(vl_api_myplugin_enable_disable_t * mp)
{
  vl_api_myplugin_enable_disable_reply_t * rmp;
  myplugin_main_t * mmp = &myplugin_main;
  int rv = myplugin_enable_disable (mmp,
      ntohl(mp->sw_if_index), (int) (mp->enable_disable));
  REPLY_MACRO(VL_API_MYPLUGIN_ENABLE_DISABLE_REPLY);
}

/* Set up the API message handling tables */
static clib_error_t *
myplugin_plugin_api_hookup(vlib_main_t *vm)
{
  myplugin_main_t * mmp = &myplugin_main;
#define _(N,n)                                                  \
    vl_msg_api_set_handlers((VL_API_##N + mmp->msg_id_base),     \
                           #n,					\
                           vl_api_##n##_t_handler,              \
                           vl_noop_handler,                     \
                           vl_api_##n##_t_endian,               \
                           vl_api_##n##_t_print,                \
                           sizeof(vl_api_##n##_t), 1);
    foreach_myplugin_plugin_api_msg;
#undef _
    return 0;
}

static void
setup_message_id_table(myplugin_main_t *mmp, api_main_t *am)
{
#define _(id,n,crc)   vl_msg_api_add_msg_name_crc (am, #n "_" #crc, id + mmp->msg_id_base);
  foreach_vl_msg_name_crc_myplugin ;
#undef _
}

static clib_error_t *
myplugin_init (vlib_main_t *vm)
{
  myplugin_main_t * mmp = &myplugin_main;
  mmp->vlib_main = vm;
  mmp->vnet_main = vnet_get_main();
  mmp->tx_node_index = tap_inject_tx_node.index;
  mmp->rx_node_index = tap_inject_rx_node.index;

  u8 * name = format (0, "myplugin_%08x%c", api_version, 0);
  mmp->msg_id_base = vl_msg_api_get_msg_ids
      ((char *) name, VL_MSG_FIRST_AVAILABLE);

  clib_error_t *error = myplugin_plugin_api_hookup (vm);
  setup_message_id_table (mmp, &api_main);
  vec_free(name);
  vec_alloc (mmp->rx_buffers, NUM_BUFFERS_TO_ALLOC);
  vec_reset_length (mmp->rx_buffers);
  return error;
}

static clib_error_t *
tap_inject_cli (vlib_main_t * vm, unformat_input_t * input,
                 vlib_cli_command_t * cmd)
{
  myplugin_main_t * im = myplugin_get_main ();
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

static u8 *
format_tap_inject_tap_name (u8 * s, va_list * args)
{
  int fd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
  if (fd < 0)
    return 0;

  struct ifreq ifr;
  memset (&ifr, 0, sizeof (ifr));
  ifr.ifr_ifindex = va_arg (*args, u32);
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

  u32 k, v;
  vnet_main_t * vnet_main = vnet_get_main ();
  tap_inject_main_t * im = tap_inject_get_main ();
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

static uword
tap_inject_tx (vlib_main_t * vm, vlib_node_runtime_t * node, vlib_frame_t * f)
{
  /* printf("SLANKDEV %s\n", __func__); */
  u32* pkts = vlib_frame_vector_args (f);

  for (uint32_t i = 0; i < f->n_vectors; ++i) {
    vlib_buffer_t *b = vlib_get_buffer (vm, pkts[i]);
    uint32_t sw_ifindex = vnet_buffer (b)->sw_if_index[VLIB_RX];
    uint32_t fd = tap_inject_lookup_tap_fd (sw_ifindex);
    if (fd == ~0) {
      printf("SLANKDEV %s: sw-idx=%u, not found\n", __func__, sw_ifindex);
      continue;
    }

    /* Re-wind the buffer to the start of the Ethernet header. */
    vlib_buffer_advance (b, -b->current_data);
    tap_inject_tap_send_buffer (fd, b);
  }

  vlib_buffer_free (vm, pkts, f->n_vectors);
  return f->n_vectors;
}

static u32
tap_inject_lookup_sw_if_index_from_tap_fd (u32 tap_fd)
{
  tap_inject_main_t * im = tap_inject_get_main ();
  vec_validate_init_empty (im->tap_fd_to_sw_if_index, tap_fd, ~0);
  return im->tap_fd_to_sw_if_index[tap_fd];
}

static inline uword
tap_rx (vlib_main_t * vm, vlib_node_runtime_t * node, vlib_frame_t * f, int fd)
{
  tap_inject_main_t * im = tap_inject_get_main ();

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
    u32 * to_next;

    hw = vnet_get_hw_interface (vnet_get_main (), sw_if_index);
    new_frame = vlib_get_frame_to_node (vm, hw->output_node_index);
    to_next = vlib_frame_vector_args (new_frame);
    to_next[0] = bi[0];
    new_frame->n_vectors = 1;
    vlib_put_frame_to_node (vm, hw->output_node_index, new_frame);
  }

  return 1;
}

static uword
tap_inject_rx (vlib_main_t * vm, vlib_node_runtime_t * node, vlib_frame_t * f)
{
  tap_inject_main_t * im = tap_inject_get_main ();
  uword count = 0;
  u32 * fd;
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

int
tap_inject_is_enabled (void)
{
  myplugin_main_t * im = myplugin_get_main ();
  return !!(im->flags & TAP_INJECT_F_ENABLED);
}

int
tap_inject_is_config_disabled (void)
{
  myplugin_main_t * im = myplugin_get_main ();
  return !!(im->flags & TAP_INJECT_F_CONFIG_DISABLE);
}

VLIB_INIT_FUNCTION (myplugin_init);

VNET_FEATURE_INIT (myplugin, static) =
{
  .arc_name = "device-input",
  .node_name = "myplugin",
  .runs_before = VNET_FEATURES ("ethernet-input"),
};

VLIB_PLUGIN_REGISTER () =
{
  .version = VPP_BUILD_VER,
  .description = "myplugin plugin description goes here",
};

VLIB_CLI_COMMAND (enable_tap_cmd, static) = {
  .path = "enable tap-inject",
  .short_help ="enable tap-inject",
  .function = tap_inject_cli,
  .function_arg = 1,
};

VLIB_CLI_COMMAND (disable_tap_cmd, static) = {
  .path = "disable tap-inject",
  .short_help ="disable tap-inject",
  .function = tap_inject_cli,
  .function_arg = 0,
};

VLIB_CLI_COMMAND (show_tap_inject_cmd, static) = {
  .path = "show tap-inject",
  .short_help = "show tap-inject",
  .function = show_tap_inject,
};

VLIB_REGISTER_NODE (tap_inject_tx_node) = {
  .function = tap_inject_tx,
  .name = "myplugin-tap-inject-tx",
  .vector_size = sizeof (u32),
  .type = VLIB_NODE_TYPE_INTERNAL,
};

VLIB_REGISTER_NODE (tap_inject_rx_node) = {
  .function = tap_inject_rx,
  .name = "myplugin-tap-inject-rx",
  .vector_size = sizeof (u32),
  .type = VLIB_NODE_TYPE_INPUT,
  .state = VLIB_NODE_STATE_INTERRUPT,
};

