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
#ifndef __included_tap_mirror_h__
#define __included_tap_mirror_h__

#include <vnet/vnet.h>
#include <vnet/ip/ip.h>
#include <vnet/ethernet/ethernet.h>
#include <vppinfra/hash.h>
#include <vppinfra/error.h>

typedef struct {
  uint8_t *target_node_name;
  int *tap_fds;
  uint8_t **tap_names;
  vlib_node_function_t *target_fn;
  vlib_node_runtime_t *target_rt;
  uint32_t redirector_node_index;
  bool request_to_free;
} tap_mirror_context_t;

typedef struct {
  uint16_t msg_id_base;
  vlib_main_t *vlib_main;
  vnet_main_t *vnet_main;

#define TAP_MIRROR_F_ENABLED        (1U << 0)
  uint32_t flags;

  tap_mirror_context_t **contexts;
} tap_mirror_main_t;

tap_mirror_main_t *tap_mirror_get_main(void);
int tap_mirror_is_enabled (void);
int enable_tap_mirror(vlib_main_t *vm,
  const char *node_name, const char *tap_name);
void disable_tap_mirror(vlib_main_t *vm,
  const char *node_name, const char *tap_name);

#endif /* __included_tap_mirror_h__ */
