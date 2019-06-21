
#include "command.h"
#include "command_node.h"

int vty_netlink_enter(struct vty *vty)
{
  vty->node = NETLINK_NODE;
  vty->config = true;
  vty->xpath_index = 0;
  return CMD_SUCCESS;
}

DEFUN (netlink,
       netlink_cmd,
       "netlink",
       "Netlink setting\n")
{
  return vty_netlink_enter(vty);
}

DEFUN (filter_ifinfo_msg_flag,
       filter_ifinfo_msg_flag_cmd,
       "filter ifinfo-msg flag",
       "Setting rtnl's filter\n"
       "Setting rtnl-link's filter\n"
       "Setting rtnl-link flag's filter\n")
{
  vty_out(vty, "%s node=%d\n", __func__, vty->node);
  return CMD_SUCCESS;
}

DEFUN (show_netlink_mask,
       show_netlink_mask_cmd,
       "show netlink mask",
       SHOW_STR
       "Show netlink information\n"
       "Show netlink mask\n")
{
  vty_out(vty, "%s\n", __func__);
  return CMD_SUCCESS;
}

void
setup_netlink_node()
{
  static struct cmd_node netlink_node = { NETLINK_NODE, "%s(config-netlink)# ", 1 };
  install_node(&netlink_node, NULL);
  install_element(ENABLE_NODE, &show_netlink_mask_cmd);
  install_element(CONFIG_NODE, &netlink_cmd);
  install_element(NETLINK_NODE, &filter_ifinfo_msg_flag_cmd);
  install_default(NETLINK_NODE);
}
