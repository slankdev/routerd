
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <vui/vui.h>

DEFUN (tap,
       tap_cmd,
       "tap",
       "TUN/TAP setting\n")
{
  vty->node = find_node_id_by_name("tap");
  vty->config = true;
  vty->xpath_index = 0;
  return CMD_SUCCESS;
}

/* DEFUN (filter_ifinfo_msg_flag, */
/*        filter_ifinfo_msg_flag_cmd, */
/*        "filter ifinfo-msg flag", */
/*        "Setting rtnl's filter\n" */
/*        "Setting rtnl-link's filter\n" */
/*        "Setting rtnl-link flag's filter\n") */
/* { */
/*   vty_out(vty, "%s\n", __func__); */
/*   return CMD_SUCCESS; */
/* } */
/*  */
/* DEFUN (show_netlink_filter, */
/*        show_netlink_filter_cmd, */
/*        "show netlink filter", */
/*        SHOW_STR */
/*        "Show netlink information\n" */
/*        "Show netlink filter\n") */
/* { */
/*   vty_out(vty, "%s\n", __func__); */
/*   return CMD_SUCCESS; */
/* } */
/*  */
/* DEFUN (show_netlink_cache, */
/*        show_netlink_cache_cmd, */
/*        "show netlink cache <iflink|addr|route|neigh>", */
/*        SHOW_STR */
/*        "Show netlink information\n" */
/*        "Show netlink cache information\n" */
/*        "Show netlink iflink cache\n" */
/*        "Show netlink addr cache\n" */
/*        "Show netlink route cache\n" */
/*        "Show netlink neigh cache\n") */
/* { */
/*   vty_out(vty, "%s\n", __func__); */
/*   return CMD_SUCCESS; */
/* } */
/*  */
/* DEFUN (show_netlink_counter, */
/*        show_netlink_counter_cmd, */
/*        "show netlink counter", */
/*        SHOW_STR */
/*        "Show netlink information\n" */
/*        "Show netlink message counter\n") */
/* { */
/*   vty_out(vty, "%s\n", __func__); */
/*   return CMD_SUCCESS; */
/* } */

void
setup_tap_node(vui_t *vui)
{
  vui_node_t *tap_node = vui_node_new();
  tap_node->name = strdup("tap");
  tap_node->prompt = strdup("%s(config-tap)# ");
  tap_node->parent = CONFIG_NODE;
  vui_node_install(vui, tap_node);

  vui_install_default_element(vui, tap_node->node);
  vui_install_element(vui, CONFIG_NODE, &tap_cmd);

  /* vui_install_element(vui, ENABLE_NODE, &show_netlink_filter_cmd); */
  /* vui_install_element(vui, ENABLE_NODE, &show_netlink_cache_cmd); */
  /* vui_install_element(vui, ENABLE_NODE, &show_netlink_counter_cmd); */
  /* vui_install_element(vui, netlink_node->node, &filter_ifinfo_msg_flag_cmd); */
}

