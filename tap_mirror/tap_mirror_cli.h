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
#ifndef __included_tap_mirror_cli_h__
#define __included_tap_mirror_cli_h__

#include <vnet/vnet.h>
#include <vnet/ip/ip.h>
#include <vnet/ethernet/ethernet.h>
#include <vppinfra/hash.h>
#include <vppinfra/error.h>

clib_error_t *show_tap_mirror_fn (vlib_main_t * vm,
  unformat_input_t * input, vlib_cli_command_t * cmd);
clib_error_t *set_tap_mirror_fn (vlib_main_t * vm,
  unformat_input_t * input, vlib_cli_command_t * cmd);

#endif /* __included_tap_mirror_cli_h__ */
