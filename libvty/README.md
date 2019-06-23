
# VUI: VTY UI Librariy

This is under the development. FRR's awesome VTY's wrapper for our toy systems.
This module helps you when developing CLI. very easy and good functionality.

```
sudo apt update && sudo apt insetall -y \
  libunwind-dev cmake build-essential
git clone https://github.com/CESNET/libyang.git /tmp/libyang && cd $_
mkdir build; cd build
cmake -DENABLE_LYD_PRIV=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr \
      -D CMAKE_BUILD_TYPE:String="Release" ..
make && sudo make install
```

```
cd libvty/src
make
./a.out
```

### define and install new command

```
```

### Create new node

```
DEFUN (netlink,
       netlink_cmd,
       "netlink",
       "Netlink setting\n")
{
  vty->node = find_netlink_node_id("netlink");
  vty->config = true;
  vty->xpath_index = 0;
  return CMD_SUCCESS;
}

DEFUN (filter,
       filter_cmd,
       "filter ifinfo-msg flag",
       "Setting rtnl's filter\n"
       "Setting rtnl-link's filter\n"
       "Setting rtnl-link flag's filter\n")
{
  vty_out(vty, "%s\n", __func__);
  return CMD_SUCCESS;
}

int main()
{
  vui_node_t *netlink_node = vui_node_new();
  netlink_node->name = strdup("netlink");
  netlink_node->prompt = strdup("%s(config-netlink)# ");
  netlink_node->parent = CONFIG_NODE;
  vui_node_install(vui, netlink_node);

  vui_install_element(vui, CONFIG_NODE, &netlink_cmd);
  vui_install_element(vui, netlink_node->node, &filter_cmd);
}
```

