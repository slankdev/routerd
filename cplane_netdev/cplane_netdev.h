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
#ifndef __included_cplane_netdev_h__
#define __included_cplane_netdev_h__

#include <vnet/vnet.h>
#include <vnet/ip/ip.h>
#include <vnet/ethernet/ethernet.h>
#include <vppinfra/hash.h>
#include <vppinfra/error.h>

typedef struct {
  uint16_t msg_id_base;
  vlib_main_t *vlib_main;
  vnet_main_t *vnet_main;
  ethernet_main_t *ethernet_main;

#define TAP_INJECT_F_CONFIG_ENABLE  (1U << 0)
#define TAP_INJECT_F_CONFIG_DISABLE (1U << 1)
#define TAP_INJECT_F_ENABLED        (1U << 3)
  uint32_t flags;

  uint32_t *sw_if_index_to_tap_fd;
  uint32_t *sw_if_index_to_tap_if_index;
  uint32_t *tap_fd_to_sw_if_index;
  uint32_t *tap_if_index_to_sw_if_index;
  uint32_t *interfaces_to_enable;
  uint32_t *interfaces_to_disable;
  uint32_t *rx_file_descriptors;
  uint32_t rx_node_index;
  uint32_t tx_node_index;
  uint32_t neighbor_node_index;
  uint32_t *rx_buffers;
} cplane_netdev_main_t;

#endif /* __included_cplane_netdev_h__ */
