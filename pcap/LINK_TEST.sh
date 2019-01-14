#!/bin/sh
set -ue

sudo ip link add br0 type bridge
sudo ip link del br0
sudo ip link add dum0 type dummy
sudo ip link set dum0 up
sudo ip link set dum0 down
sudo ip link add link dum0 name dum0.10 type vlan id 10
sudo ip link del dum0.10
sudo ip link add vxlan0 type vxlan id 100 local 1.1.1.1 remote 2.2.2.2 dev dum0 dstport 4789
sudo ip link del vxlan0
sudo ip link del dum0
