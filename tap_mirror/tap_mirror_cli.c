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
#include <vnet/mfib/mfib_types.h>
#include <vnet/mfib/mfib_table.h>
#include <vnet/ip/ip6_packet.h>
#include <vnet/dpo/dpo.h>
#include <vnet/dpo/replicate_dpo.h>

#include <tap_mirror/tap_mirror.h>
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

#include <tap_mirror/tap_mirror.h>
#include <tap_mirror/tap_mirror_msg_enum.h>
#define vl_typedefs
#include <tap_mirror/tap_mirror_all_api_h.h>
#undef vl_typedefs
#define vl_endianfun
#include <tap_mirror/tap_mirror_all_api_h.h>
#undef vl_endianfun
#define vl_print(handle, ...) vlib_cli_output (handle, __VA_ARGS__)
#define vl_printfun
#include <tap_mirror/tap_mirror_all_api_h.h>
#undef vl_printfun
#define REPLY_MSG_ID_BASE mmp->msg_id_base
#include <vlibapi/api_helper_macros.h>
#define vl_msg_name_crc_list
#include <tap_mirror/tap_mirror_all_api_h.h>
#undef vl_msg_name_crc_list

clib_error_t *
show_tap_mirror_fn (vlib_main_t *vm, unformat_input_t *input,
                 vlib_cli_command_t *cmd)
{
  if (!tap_mirror_is_enabled())
    return 0;

  tap_mirror_main_t *xm = tap_mirror_get_main();
  vlib_cli_output(vm, "\n");
  vlib_cli_output(vm, "  idx     tgt-node             dest-taps\n");
  vlib_cli_output(vm, " ------  -------------------  ---------------------");
  int n = vec_len(xm->contexts);
  for (int i=0; i<n; i++) {
    tap_mirror_context_t *ctx = vec_elt(xm->contexts, i);
    if (!ctx) {
      /* vlib_cli_output(vm, "  %3d%5s%-20s %-10s %s\n", i, "", "n/a", "n/a", "n/a"); */
      continue;
    }
    uint8_t *tap_fds_str = NULL;
    for (size_t i=0; i<vec_len(ctx->tap_fds); i++)
      if (ctx->tap_fds[i] > 0)
        tap_fds_str = format(tap_fds_str, "%s%s",
            vec_elt(ctx->tap_names, i),
            (i+1)<vec_len(ctx->tap_fds)?",":"");

    vlib_cli_output(vm, "  %3d%5s%-20s %-10s\n",
        i, "", ctx->target_node_name, tap_fds_str);
  }
  vlib_cli_output(vm, "\n");
  return 0;
}

clib_error_t *
set_tap_mirror_fn (vlib_main_t *vm,
    unformat_input_t *input, vlib_cli_command_t *cmd)
{
  unformat_input_t _line_input, *line_input = &_line_input;

  char *node_name = NULL;
  char *tap_name = NULL;
  bool is_reset = false;
  if (unformat_user (input, unformat_line_input, line_input)) {
    while (unformat_check_input (line_input) != UNFORMAT_END_OF_INPUT) {
      if (false) ;
      else if (unformat (line_input, "node %s", &node_name)) ;
      else if (unformat (line_input, "tap %s", &tap_name)) ;
      else if (unformat (line_input, "reset")) is_reset = true;

      else {
        unformat_free (line_input);
        return clib_error_return (0, "unknown input `%U'",
          format_unformat_error, input);
      }
    }
    unformat_free (line_input);
  }

  if (!node_name || !tap_name) {
    if (node_name) vec_free(node_name);
    if (tap_name) vec_free(tap_name);
    return clib_error_return (0, "failed: specify both node-name and tap-name");
  }

  if (0 /* debug code */) {
    vlib_cli_output (vm, "[%s]\n", __func__);
    vlib_cli_output (vm, " node: %s\n", node_name);
    vlib_cli_output (vm, " tap: %s\n", tap_name);
    vlib_cli_output (vm, " reset: %s\n", is_reset ? "true" : "false");
  }

  if (is_reset) {
    disable_tap_mirror(vm, node_name, tap_name);
  } else {
    int ret = enable_tap_mirror(vm, node_name, tap_name);
    if (ret < 0)
      return clib_error_return (0, "set_tap_mirro failed");
  }

  vec_free(node_name);
  vec_free(tap_name);
  return NULL;
}

