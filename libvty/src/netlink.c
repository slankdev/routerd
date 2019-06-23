
#include "vui.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

static int netlink_node_id = -1;
static struct cmd_node netlink_node = {
  .node =  -1,
  .prompt = "%s(config-netlink)# ",
  .vtysh = 1,
};

DEFUN (netlink,
       netlink_cmd,
       "netlink",
       "Netlink setting\n")
{
  vty->node = netlink_node_id;
  vty->config = true;
  vty->xpath_index = 0;
  return CMD_SUCCESS;
}

DEFUN (filter_ifinfo_msg_flag,
       filter_ifinfo_msg_flag_cmd,
       "filter ifinfo-msg flag",
       "Setting rtnl's filter\n"
       "Setting rtnl-link's filter\n"
       "Setting rtnl-link flag's filter\n")
{
  vty_out(vty, "%s\n", __func__);
  return CMD_SUCCESS;
}

DEFUN (show_netlink_filter,
       show_netlink_filter_cmd,
       "show netlink filter",
       SHOW_STR
       "Show netlink information\n"
       "Show netlink filter\n")
{
  vty_out(vty, "%s\n", __func__);
  return CMD_SUCCESS;
}

DEFUN (show_netlink_cache,
       show_netlink_cache_cmd,
       "show netlink cache <iflink|addr|route|neigh>",
       SHOW_STR
       "Show netlink information\n"
       "Show netlink cache information\n"
       "Show netlink iflink cache\n"
       "Show netlink addr cache\n"
       "Show netlink route cache\n"
       "Show netlink neigh cache\n")
{
  vty_out(vty, "%s\n", __func__);
  return CMD_SUCCESS;
}

DEFUN (show_netlink_counter,
       show_netlink_counter_cmd,
       "show netlink counter",
       SHOW_STR
       "Show netlink information\n"
       "Show netlink message counter\n")
{
  vty_out(vty, "%s\n", __func__);
  return CMD_SUCCESS;
}

void
setup_netlink_node(vui_t *vui)
{
  netlink_node_id = vui_alloc_new_node_id(vui, "netlink", CONFIG_NODE);
  netlink_node.node = netlink_node_id;
  vui_install_node(vui, &netlink_node);
  vui_install_element(vui, ENABLE_NODE, &show_netlink_filter_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_netlink_cache_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_netlink_counter_cmd);
  vui_install_element(vui, CONFIG_NODE, &netlink_cmd);
  vui_install_element(vui, netlink_node_id, &filter_ifinfo_msg_flag_cmd);
  vui_install_default_element(vui, netlink_node_id);
}

