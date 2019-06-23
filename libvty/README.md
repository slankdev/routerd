
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
int slank_node_id = -1;
int main()
{
  slank_node_id = vui_alloc_new_node_id(vui, "netlink", CONFIG_NODE);
  slank_node.node = netlink_node_id;
  vui_install_node(vui, &netlink_node);
}
```

