#!/bin/sh
set -ue

# LINK & ADDR
echo -------
sudo ip link add dum0 type dummy
sudo ip link set dum0 up
sudo ip addr add 10.0.0.1/24 dev dum0
sudo ip addr add 2001:1::1/64 dev dum0
ip addr show dev dum0
echo -------

# ROUTE
sudo ip route add 20.0.0.0/24 via 10.0.0.10
sudo ip route del 20.0.0.0/24
sudo ip route add 30.0.0.0/24 dev dum0
sudo ip route del 30.0.0.0/24
sudo ip route add 2001:2::/64 via 2001:1::10
sudo ip route del 2001:2::/64
sudo ip route add 2001:3::/64 dev dum0
sudo ip route del 2001:3::/64
ip route show
ip -6 route show
echo -------

# LINK & ADDR
sudo ip addr del 10.0.0.1/24 dev dum0
sudo ip addr del 2001:1::1/64 dev dum0
sudo ip link del dum0
