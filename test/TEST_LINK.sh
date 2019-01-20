#!/bin/sh
set -ue

# LINK & ADDR
echo -------
IFNAME=br0
sudo ip link add $IFNAME type bridge
sudo ip addr show dev $IFNAME
sudo ip link del $IFNAME
echo -------
IFNAME=dum0
sudo ip link add $IFNAME type dummy
sudo ip addr show dev $IFNAME
sudo ip link del $IFNAME
echo -------
sudo ip link add dum1 type dummy
IFNAME=vxlan0
sudo ip link add $IFNAME type vxlan id 100 \
	local 1.1.1.1 remote 2.2.2.2 dev dum1 dstport 4789
sudo ip addr show dev $IFNAME
sudo ip link del $IFNAME
sudo ip link del dum1
echo -------
