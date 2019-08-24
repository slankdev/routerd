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
#define vl_api_version(n,v) static uint32_t api_version=(v);
#include <tap_mirror/tap_mirror_all_api_h.h>
#undef vl_api_version
#define REPLY_MSG_ID_BASE mmp->msg_id_base
#include <vlibapi/api_helper_macros.h>
#define vl_msg_name_crc_list
#include <tap_mirror/tap_mirror_all_api_h.h>
#undef vl_msg_name_crc_list
#include <tap_mirror/tap_mirror_cli.h>

tap_mirror_main_t *
tap_mirror_get_main(void)
{
  static tap_mirror_main_t tap_mirror_main;
  return &tap_mirror_main;
}

int
tap_mirror_is_enabled (void)
{
  tap_mirror_main_t *xm = tap_mirror_get_main();
  return !!(xm->flags & TAP_MIRROR_F_ENABLED);
}

static int
get_tapfd(const char *name)
{
  tap_mirror_main_t *xm = tap_mirror_get_main();
  for (int i=0; i<vec_len(xm->contexts); i++) {
    tap_mirror_context_t *ctx = vec_elt(xm->contexts, i);
    if (!ctx)
      continue;
    assert(vec_len(ctx->tap_names) == vec_len(ctx->tap_fds));
    for (int i=0; i<vec_len(ctx->tap_names); i++) {
      if (vec_elt(ctx->tap_fds, i) <= 0)
        continue;
      uint8_t *name_ = vec_elt(ctx->tap_names, i);
      uint8_t *name__ = format(0, "%s", name);
      if (memcmp(name_, name__, strlen(name)) == 0) {
        return vec_elt(ctx->tap_fds, i);
      }
    }
  }
  return -1;
}

static int
get_or_open_tapfd(const char *name)
{
  tap_mirror_main_t *xm = tap_mirror_get_main();
  for (int i=0; i<vec_len(xm->contexts); i++) {
    tap_mirror_context_t *ctx = vec_elt(xm->contexts, i);
    if (!ctx)
      continue;
    assert(vec_len(ctx->tap_names) == vec_len(ctx->tap_fds));
    for (int i=0; i<vec_len(ctx->tap_names); i++) {
      if (vec_elt(ctx->tap_fds, i) <= 0)
        continue;
      uint8_t *name_ = vec_elt(ctx->tap_names, i);
      uint8_t *name__ = format(0, "%s", name);
      if (memcmp(name_, name__, strlen(name)) == 0) {
        return vec_elt(ctx->tap_fds, i);
      }
    }
  }

  int fd = open("/dev/net/tun", O_RDWR|O_NONBLOCK);
  if (fd < 0) {
    //"%s: failed. open tap-fd\n"
    return -1;
  }

  struct ifreq ifr;
  memset (&ifr, 0, sizeof (ifr));
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
  snprintf (ifr.ifr_name, IFNAMSIZ, "%s", name);
  int ret = ioctl (fd, TUNSETIFF, (void *) &ifr);
  if (ret < 0) {
    //"%s: ioctl(TUNSETIFF) failed.\n"
    close (fd);
    return -2;
  }

  return fd;
}

static int
set_link_up_down(const char *name, bool is_up)
{
  int fd = socket (AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    //"%s: socket failed.\n"
    return -1;
  }

  struct ifreq ifr;
  memset (&ifr, 0, sizeof (ifr));
  strncpy (ifr.ifr_name, name, IFNAMSIZ - 1);
  int ret = ioctl (fd, SIOCGIFFLAGS, &ifr);
  if (ret < 0) {
    //"%s: ioctl(SIOCGIFFLAGS) failed.\n"
    close (fd);
    return -2;
  }

  if (is_up) ifr.ifr_flags |= IFF_UP;
  else ifr.ifr_flags &= ~IFF_UP;
  ret = ioctl (fd, SIOCSIFFLAGS, &ifr);
  if (ret < 0) {
    //"%s: ioctl(SIOCSIFFLAGS) failed.\n"
    close (fd);
    return -3;
  }

  close (fd);
  return 0;
}

static vlib_node_runtime_t*
vlib_get_node_runtime_by_name(vlib_main_t *vm, const char *node_name)
{
  uint8_t *str_ptr = format(0, "%s", node_name);
  vlib_node_t *node = vlib_get_node_by_name(vlib_get_main(), str_ptr);
  vlib_node_runtime_t *runtime = node ?
       vlib_node_get_runtime(vm, node->index) : NULL;
  if (!runtime) {
    vlib_cli_output (vm,
      "%s: failed. no such node or runtime (%s)\n",
      __func__, node_name);
    return NULL;
  }
  return runtime;
}

static void
tap_mirror_context_add_new_tap(tap_mirror_context_t *ctx, const char *tap_name)
{
  int tap_fd = get_or_open_tapfd(tap_name);
  if (tap_fd < 0) {
    printf("OKASHII\n");
  }
  /* printf("new tap-fd:%d\n", tap_fd); */
  set_link_up_down(tap_name, true);

  vec_add1(ctx->tap_fds, tap_fd);
  vec_add1(ctx->tap_names, format(0, "%s", tap_name));
}

static clib_error_t *
tap_mirror_init (vlib_main_t *vm)
{
  tap_mirror_main_t * mmp = tap_mirror_get_main();
  mmp->vlib_main = vm;
  mmp->vnet_main = vnet_get_main();

  uint8_t * name = format (0, "tap_mirror_%08x%c", api_version, 0);
  mmp->msg_id_base = vl_msg_api_get_msg_ids
      ((char *) name, VL_MSG_FIRST_AVAILABLE);
  vec_free(name);

  vec_validate(mmp->contexts, 8);
  return NULL;
}

static int
get_event_type(const char *name)
{
  int idx = -1;
  tap_mirror_main_t *xm = tap_mirror_get_main();
  for (int i=0; i<vec_len(xm->contexts); i++) {
    tap_mirror_context_t *ctx = vec_elt(xm->contexts, i);
    if (!ctx) continue;
    if (strncmp((char*)ctx->target_node_name, name, strlen(name)) == 0) {
      idx = i;
      break;
    }
  }
  return idx;
}

static uint64_t
tap_mirror_input_fn (vlib_main_t * vm,
    vlib_node_runtime_t *rt, vlib_frame_t * f)
{
  vlib_node_t *node = vlib_get_node(vm, rt->node_index);
  const char* name = (const char*)node->name;
  int event_type = get_event_type(name);
  /* vlib_cli_output(vm, "mirror from %s type=%d\n", name, event_type); */
  tap_mirror_main_t *xm = tap_mirror_get_main();
  tap_mirror_context_t *ctx = vec_elt(xm->contexts, event_type);

  //check request_to_free and free
  if (ctx->request_to_free) {
    vlib_node_function_t *fn;
    fn = ctx->target_fn;
    ctx->target_rt->function = ctx->target_fn;
    vec_elt(xm->contexts, event_type) = NULL;
    clib_mem_free(ctx);
    return fn(vm, rt, f);
  }

  uint32_t *pkts = vlib_frame_vector_args (f);
  for (uint32_t i = 0; i < f->n_vectors; ++i) {
    uint32_t clones[2];
    uint32_t n_cloned = vlib_buffer_clone (vm,
        pkts[i], clones, 2, VLIB_BUFFER_CLONE_HEAD_SIZE);
    assert(n_cloned == 2);
    vlib_process_signal_event_mt(vm,
        ctx->redirector_node_index,
        event_type, clones[1]);
  }
  return ctx->target_fn(vm, rt, f);
}

static uint64_t
tap_mirror_redirector_fn (vlib_main_t *vm, vlib_node_runtime_t *rt, vlib_frame_t *f)
{
  uint64_t *event_data = 0;
  tap_mirror_main_t *xm = tap_mirror_get_main();
  while (true) {

    /* printf("%s:%d\n", __func__, __LINE__); */
    vlib_process_wait_for_event_or_clock(vm, 1.0);
    uint64_t event_type = vlib_process_get_events (vm, &event_data);
    switch (event_type) {
      case -1UL: break;
      default:
      {
        /* printf("%s: recv unknown event %lu\n", __func__, event_type); */
        uint64_t buffer_index = *event_data;
        vlib_buffer_t *b = vlib_get_buffer (vm, buffer_index);
        vlib_buffer_advance (b, -b->current_data);

        tap_mirror_context_t *ctx = vec_elt(xm->contexts, event_type);
        for (int i=0; i<vec_len(ctx->tap_fds); i++) {
          int tap_fd = vec_elt(ctx->tap_fds, i);
          if (tap_fd <= 0)
            continue;
          write(tap_fd,
              vlib_buffer_get_current(b),
              vlib_buffer_length_in_chain(vm, b));
        }
        vlib_buffer_free_one (vm, buffer_index);

        break;
      }
    }

    //printf("%s:%d\n", __func__, __LINE__);
    vec_reset_length(event_data);
    vlib_process_suspend (vm, 0 /* secs */ );
  }
  return 0;
}

static tap_mirror_context_t*
get_or_new_tap_mirror_context(const char *node_name)
{

  tap_mirror_main_t *xm = tap_mirror_get_main();
  vlib_node_t *redirector_node =
       vlib_get_node_by_name(vlib_get_main(),
       (uint8_t*)"tap-mirror-redirector");

  //search exist
  for (int i=0; i<vec_len(xm->contexts); i++) {
    tap_mirror_context_t *ctx = vec_elt(xm->contexts, i);
    if (!ctx)
      continue;
    if (memcmp(ctx->target_node_name,
        node_name, strlen(node_name)) == 0)
      return ctx;
  }

  //create new
  for (int i=0; i<vec_len(xm->contexts); i++) {
    tap_mirror_context_t *ctx = vec_elt(xm->contexts, i);
    if (!ctx) {
      ctx = clib_mem_alloc(sizeof(tap_mirror_context_t));
      memset(ctx, 0, sizeof(tap_mirror_context_t));
      ctx->target_node_name = format(0, "%s", node_name);

      vlib_node_runtime_t *runtime =
        vlib_get_node_runtime_by_name(vlib_get_main(), node_name);
      if (!runtime) {
        clib_mem_free(ctx);
        return NULL;
      }

      ctx->request_to_free = false;
      ctx->redirector_node_index = redirector_node->index;
      ctx->target_rt = runtime;
      ctx->target_fn = runtime->function;
      runtime->function = tap_mirror_input_fn;

      vec_validate(ctx->tap_fds, 8);
      vec_validate(ctx->tap_names, 8);
      vec_elt(xm->contexts, i) = ctx;
      return ctx;
    }
  }

  return NULL;
}

static tap_mirror_context_t*
get_tap_mirror_context(tap_mirror_main_t *xm, const char *node_name)
{
  //search exist
  for (int i=0; i<vec_len(xm->contexts); i++) {
    tap_mirror_context_t *ctx = vec_elt(xm->contexts, i);
    if (!ctx)
      continue;
    if (memcmp(ctx->target_node_name,
        node_name, strlen(node_name)) == 0)
      return ctx;
  }
  return NULL;
}

int
enable_tap_mirror(vlib_main_t *vm,
  const char *node_name, const char *tap_name)
{
  tap_mirror_main_t *xm = tap_mirror_get_main();
  xm->flags |= TAP_MIRROR_F_ENABLED;

  tap_mirror_context_t *ctx = get_or_new_tap_mirror_context(node_name);
  if (!ctx) {
    vlib_cli_output(vm, "create miss\n");
    return -1;
  }
  tap_mirror_context_add_new_tap(ctx, tap_name);
  return 0;
}

void
disable_tap_mirror(vlib_main_t *vm,
  const char *node_name, const char *tap_name)
{
  // search ctx
  tap_mirror_main_t *xm = tap_mirror_get_main();
  tap_mirror_context_t *ctx = get_tap_mirror_context(xm, node_name);
  if (!ctx) {
    vlib_cli_output(vm, "tap-mirror-context not found\n");
    return;
  }

  // delete tap-fd
  int tap_fd = 0;
  for (int i=0; i<vec_len(ctx->tap_fds); i++) {
    if (vec_elt(ctx->tap_fds, i) <= 0)
      continue;
    if (memcmp(vec_elt(ctx->tap_names, i), tap_name, strlen(tap_name)) == 0) {
      tap_fd = vec_elt(ctx->tap_fds, i);
      vec_elt(ctx->tap_fds, i) = -1;
      vec_elt(ctx->tap_names, i) = NULL;
    }
  }

  // if tap referencing node is nothing, close(fd)
  int exist_fd = get_tapfd(tap_name);
  if (exist_fd < 0)
    close(tap_fd);

  // if no tap-fds delete ctx request;
  size_t cnt = 0;
  for (int i=0; i<vec_len(ctx->tap_fds); i++) {
    if (vec_elt(ctx->tap_fds, i) <= 0)
      continue;
    cnt ++;
  }
  if (cnt == 0)
    ctx->request_to_free = true;
}

VLIB_INIT_FUNCTION (tap_mirror_init);

VNET_FEATURE_INIT (tap_mirror, static) =
{
  .arc_name = "device-input",
  .node_name = "tap-mirror",
  .runs_after = VNET_FEATURES ("ethernet-input"),
};

VLIB_REGISTER_NODE (tap_mirror_redirector, static) = {
  .function = tap_mirror_redirector_fn,
  .name = "tap-mirror-redirector",
  .type = VLIB_NODE_TYPE_PROCESS,
};

VLIB_PLUGIN_REGISTER () =
{
  .version = VPP_BUILD_VER,
  .description = "tap_mirror plugin for operational debug",
};

VLIB_CLI_COMMAND (set_node_tap_mirror, static) = {
  .path = "set tap-mirror",
  .short_help = "set tap-mirror {node <node-name>} {tap <tap-name>} [reset]",
  .function = set_tap_mirror_fn,
};

VLIB_CLI_COMMAND (show_tap_inject_cmd, static) = {
  .path = "show tap-mirror",
  .short_help = "show tap-mirror",
  .function = show_tap_mirror_fn,
};

