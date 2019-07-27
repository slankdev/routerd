# Copyright (c) 2019 Hiroki Shirokura
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

add_vpp_plugin(cplane_netdev
  SOURCES
  cplane_netdev.c
  cplane_netdev.h

  API_FILES
  cplane_netdev.api

  INSTALL_HEADERS
  cplane_netdev_all_api_h.h
  cplane_netdev_msg_enum.h
)
