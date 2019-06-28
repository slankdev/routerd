
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <vui/vui.h>
#include "netlink_cli.h"
#include "netlink.h"
#include "routerd.h"

DEFUN (netlink,
       netlink_cmd,
       "netlink",
       "Netlink setting\n")
{
  vty->node = find_node_id_by_name("netlink");
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

static std::string
mac_addr_str(const void *ptr_)
{
  const uint8_t *ptr = (const uint8_t*)ptr_;
  for (size_t i=0; i<6; i++)
    assert(ptr + i);
  char str[128];
  snprintf(str, sizeof(str), "%02x:%02x:%02x:%02x:%02x:%02x",
      ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
  return str;
}

DEFUN (show_netlink_cache,
       show_netlink_cache_cmd,
       "show netlink cache <link|addr|route|neigh>",
       SHOW_STR
       "Show netlink information\n"
       "Show netlink cache information\n"
       "Show netlink link cache\n"
       "Show netlink addr cache\n"
       "Show netlink route cache\n"
       "Show netlink neigh cache\n")
{
  vty_out(vty, "cache:%p\n", nlc);
  std::string type = argv[3]->arg;

  if (type == "link") {
    vty_out(vty, "link-cache@%p size=%zd\n", &nlc->links, nlc->links.size());
    for (size_t i=0; i<nlc->links.size(); i++) {
      buffer &buf = nlc->links[i];
      routerd::link link((const struct ifinfomsg*)buf.data(), buf.size());
      vty_out(vty, "  link[%zd]@%p: %s\n", i, &buf, link.summary().c_str());
      vty_out(vty, "      name: %s, flag: <%s>, mtu: %u\n"
                   "      address: %s\n",
          rta_readstr(link.rtas->get(IFLA_IFNAME)),
          ifm_flags2str(link.ifi->ifi_flags).c_str(),
          rta_read32(link.rtas->get(IFLA_MTU)),
          mac_addr_str(rta_readptr(link.rtas->get(IFLA_ADDRESS))).c_str());
    }
  }

  return CMD_SUCCESS;
}

DEFUN (show_netlink_counter,
       show_netlink_counter_cmd,
       "show netlink counter",
       SHOW_STR
       "Show netlink information\n"
       "Show netlink message counter\n")
{
  const netlink_counter &c = counter;
  vty_out(vty, "\n");
  vty_out(vty, " type   %10s %10s %10s\n", "new", "del", "tot");
  vty_out(vty, " ------ %10s %10s %10s\n", "----------", "----------", "----------");
  vty_out(vty, " link:  %10zd %10zd %10zd\n", c.link.new_cnt, c.link.del_cnt, c.link_all());
  vty_out(vty, " addr:  %10zd %10zd %10zd\n", c.addr.new_cnt, c.addr.del_cnt, c.addr_all());
  vty_out(vty, " route: %10zd %10zd %10zd\n", c.route.new_cnt, c.route.del_cnt, c.route_all());
  vty_out(vty, " neigh: %10zd %10zd %10zd\n", c.neigh.new_cnt, c.neigh.del_cnt, c.neigh_all());
  vty_out(vty, "\n");
  return CMD_SUCCESS;
}

DEFUN (set_interface_pair,
       set_interface_pair_cmd,
       "set interface pair kernel-ifindex <(0-4294967295)> vpp-ifindex <(0-4294967295)>",
       "Setting\n"
       "Interface setting\n"
       "Interface pair (kern/vpp) setting\n"
       "Specify kernel-ifindex\n"
       "Specify kernel-ifindex\n"
       "Specify vpp-ifindex\n"
       "Specify vpp-ifindex\n")
{
  uint32_t k_index = strtol(argv[4]->arg, NULL, 0);
  uint32_t v_index = strtol(argv[6]->arg, NULL, 0);
  vty_out(vty, "%s kern%u, vpp%u\n", __func__, k_index, v_index);
  rd_ctx.add_interface(k_index, v_index);
  return CMD_SUCCESS;
}

DEFUN (show_netlink_interface_pair,
       show_netlink_interface_pair_cmd,
       "show netlink interface pair",
       SHOW_STR
       "Show netlink information\n"
       "Show netlink-interface information\n"
       "Show netlink-interface-pair (kern/vpp) information\n")
{
  for (size_t i=0; i<rd_ctx.interfaces.size(); i++) {
    auto &iface = rd_ctx.interfaces[i];
    vty_out(vty, " interfaces[%zd]: <kern%u,vpp%u>\n",
        i, iface.kern_ifindex, iface.vpp_ifindex);
  }
  return CMD_SUCCESS;
}

void
setup_netlink_node(vui_t *vui)
{
  vui_node_t *netlink_node = vui_node_new();
  netlink_node->name = strdup("netlink");
  netlink_node->prompt = strdup("%s(config-netlink)# ");
  netlink_node->parent = CONFIG_NODE;
  vui_node_install(vui, netlink_node);

  vui_install_default_element(vui, netlink_node->node);
  vui_install_element(vui, CONFIG_NODE, &netlink_cmd);

  vui_install_element(vui, ENABLE_NODE, &show_netlink_filter_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_netlink_cache_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_netlink_counter_cmd);
  vui_install_element(vui, ENABLE_NODE, &show_netlink_interface_pair_cmd);
  vui_install_element(vui, netlink_node->node, &filter_ifinfo_msg_flag_cmd);
  vui_install_element(vui, netlink_node->node, &set_interface_pair_cmd);
}

